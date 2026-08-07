#include "player_engine.h"
#include "bsp_audio.h"
#include "bsp_sdcard.h"
#include "bsp_board.h"
#include "ui_player.h"
#include "ui_playlist.h"
#include "lvgl_port.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_random.h"
#include "dirent.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "minimp3_ex.h"

static const char *TAG = "player_engine";

#define PLAYER_MAX_SONGS        32
#define PLAYER_CMD_QUEUE_LEN    8
#define PLAYER_TASK_STACK_SIZE  8192
#define PLAYER_TASK_PRIORITY    4
#define PLAYER_PCM_BUF_SAMPLES  1152

typedef enum {
    PLAYER_CMD_NONE = 0,
    PLAYER_CMD_PLAY_INDEX,
    PLAYER_CMD_TOGGLE,
    PLAYER_CMD_NEXT,
    PLAYER_CMD_PREV,
    PLAYER_CMD_SEEK,
    PLAYER_CMD_STOP,
} player_cmd_t;

typedef struct {
    player_cmd_t cmd;
    uint32_t arg;
} player_cmd_msg_t;

static player_song_t s_songs[PLAYER_MAX_SONGS];
static uint8_t s_song_count = 0;
static uint8_t s_current_index = 0;
static player_mode_t s_mode = PLAYER_MODE_SEQUENCE;

static mp3dec_ex_t s_mp3d;
static bool s_mp3_open = false;
static bool s_playing = false;
static uint32_t s_current_ms = 0;
static uint32_t s_duration_ms = 0;

static QueueHandle_t s_cmd_queue = NULL;
static SemaphoreHandle_t s_state_mux = NULL;

static int16_t s_pcm_buf[PLAYER_PCM_BUF_SAMPLES * 2];

static void send_cmd(player_cmd_t cmd, uint32_t arg)
{
    if (s_cmd_queue == NULL) {
        return;
    }
    player_cmd_msg_t msg = {.cmd = cmd, .arg = arg};
    xQueueSend(s_cmd_queue, &msg, 0);
}

static void load_song_meta(player_song_t *song)
{
    const char *filename = strrchr(song->path, '/');
    filename = filename ? filename + 1 : song->path;

    /* Try parse "artist - title.mp3" */
    char buf[128];
    strncpy(buf, filename, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    char *dot = strrchr(buf, '.');
    if (dot) {
        *dot = '\0';
    }

    char *dash = strstr(buf, " - ");
    if (dash) {
        *dash = '\0';
        strncpy(song->artist, buf, sizeof(song->artist) - 1);
        strncpy(song->title, dash + 3, sizeof(song->title) - 1);
    } else {
        strncpy(song->title, buf, sizeof(song->title) - 1);
        strcpy(song->artist, "Unknown");
    }
    song->title[sizeof(song->title) - 1] = '\0';
    song->artist[sizeof(song->artist) - 1] = '\0';
    strcpy(song->album, "SD Card");
}

static void scan_sdcard(void)
{
    s_song_count = 0;
    if (!bsp_sdcard_mounted()) {
        ESP_LOGW(TAG, "sdcard not mounted, using demo songs");
        return;
    }

    const char *mount = bsp_sdcard_mount_point();
    DIR *dir = opendir(mount);
    if (!dir) {
        ESP_LOGE(TAG, "failed to open sdcard root");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && s_song_count < PLAYER_MAX_SONGS) {
        const char *name = entry->d_name;
        size_t len = strlen(name);
        if (len < 4 || strcasecmp(name + len - 4, ".mp3") != 0) {
            continue;
        }
        snprintf(s_songs[s_song_count].path, sizeof(s_songs[s_song_count].path), "%s/%s", mount, name);
        load_song_meta(&s_songs[s_song_count]);
        s_songs[s_song_count].duration_ms = 0;
        s_song_count++;
    }
    closedir(dir);

    ESP_LOGI(TAG, "scanned %d mp3 files", s_song_count);
}

static void open_current_file(void)
{
    if (s_mp3_open) {
        mp3dec_ex_close(&s_mp3d);
        s_mp3_open = false;
    }

    if (s_song_count == 0) {
        return;
    }

    int err = mp3dec_ex_open(&s_mp3d, s_songs[s_current_index].path, MP3D_SEEK_TO_SAMPLE);
    if (err) {
        ESP_LOGE(TAG, "failed to open %s: %d", s_songs[s_current_index].path, err);
        return;
    }

    s_mp3_open = true;
    s_current_ms = 0;
    s_duration_ms = (uint32_t)((s_mp3d.samples * 1000ULL) / s_mp3d.info.hz);
    if (s_duration_ms == 0) {
        s_duration_ms = 1;
    }

    ESP_LOGI(TAG, "opened %s, duration %lu ms, samples %llu, rate %d",
             s_songs[s_current_index].path, s_duration_ms, s_mp3d.samples, s_mp3d.info.hz);

    /* Update UI */
    lvgl_port_lock();
    ui_player_set_song(s_songs[s_current_index].title, s_songs[s_current_index].artist,
                       s_songs[s_current_index].album, "SD卡");
    ui_playlist_set_active(s_current_index);
    ui_player_set_progress(0, s_duration_ms);
    lvgl_port_unlock();
}

static void play_index_internal(uint8_t index)
{
    if (s_song_count == 0) {
        return;
    }
    if (index >= s_song_count) {
        index = 0;
    }
    s_current_index = index;
    open_current_file();
    s_playing = true;
    bsp_audio_start();
}

static void next_internal(void)
{
    if (s_song_count == 0) {
        return;
    }
    if (s_mode == PLAYER_MODE_REPEAT) {
        play_index_internal(s_current_index);
        return;
    }
    if (s_mode == PLAYER_MODE_RANDOM) {
        play_index_internal(esp_random() % s_song_count);
        return;
    }
    uint8_t next = s_current_index + 1;
    if (next >= s_song_count) {
        next = 0;
    }
    play_index_internal(next);
}

static void prev_internal(void)
{
    if (s_song_count == 0) {
        return;
    }
    if (s_mode == PLAYER_MODE_REPEAT) {
        play_index_internal(s_current_index);
        return;
    }
    if (s_mode == PLAYER_MODE_RANDOM) {
        play_index_internal(esp_random() % s_song_count);
        return;
    }
    uint8_t prev = s_current_index == 0 ? s_song_count - 1 : s_current_index - 1;
    play_index_internal(prev);
}

static void seek_internal(uint32_t ms)
{
    if (!s_mp3_open) {
        return;
    }
    if (ms > s_duration_ms) {
        ms = s_duration_ms;
    }
    uint64_t sample = (uint64_t)ms * s_mp3d.info.hz / 1000;
    int err = mp3dec_ex_seek(&s_mp3d, sample);
    if (err) {
        ESP_LOGE(TAG, "seek failed: %d", err);
        return;
    }
    s_current_ms = ms;
}

static void update_ui_progress(void)
{
    static uint32_t last_ms = 0;
    if (abs((int)s_current_ms - (int)last_ms) < 500) {
        return;
    }
    last_ms = s_current_ms;
    lvgl_port_lock();
    ui_player_set_progress(s_current_ms, s_duration_ms);
    lvgl_port_unlock();
}

static void player_task(void *arg)
{
    (void)arg;
    player_cmd_msg_t msg;

    while (1) {
        /* Process commands */
        if (xQueueReceive(s_cmd_queue, &msg, s_playing ? 0 : pdMS_TO_TICKS(50)) == pdTRUE) {
            switch (msg.cmd) {
            case PLAYER_CMD_PLAY_INDEX:
                play_index_internal((uint8_t)msg.arg);
                break;
            case PLAYER_CMD_TOGGLE:
                s_playing = !s_playing;
                if (s_playing) {
                    bsp_audio_start();
                } else {
                    bsp_audio_stop();
                }
                lvgl_port_lock();
                ui_player_set_playing(s_playing);
                lvgl_port_unlock();
                break;
            case PLAYER_CMD_NEXT:
                next_internal();
                break;
            case PLAYER_CMD_PREV:
                prev_internal();
                break;
            case PLAYER_CMD_SEEK:
                seek_internal(msg.arg);
                break;
            case PLAYER_CMD_STOP:
                s_playing = false;
                bsp_audio_stop();
                break;
            default:
                break;
            }
        }

        if (!s_playing || !s_mp3_open) {
            continue;
        }

        /* Decode and play one chunk */
        mp3d_sample_t *buf = NULL;
        size_t samples = mp3dec_ex_read(&s_mp3d, s_pcm_buf, PLAYER_PCM_BUF_SAMPLES * 2);
        if (samples == 0) {
            /* End of file */
            next_internal();
            continue;
        }

        if (s_mp3d.info.channels == 1) {
            /* Convert mono to stereo */
            for (int i = samples - 1; i >= 0; i--) {
                s_pcm_buf[i * 2 + 1] = s_pcm_buf[i];
                s_pcm_buf[i * 2] = s_pcm_buf[i];
            }
            samples *= 2;
        }

        int written = bsp_audio_write(s_pcm_buf, samples);
        if (written < 0) {
            ESP_LOGE(TAG, "audio write error");
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if (s_mp3d.info.hz > 0) {
            s_current_ms += (uint32_t)((uint64_t)written * 1000 / s_mp3d.info.hz / 2);
        }
        update_ui_progress();
    }
}

esp_err_t player_engine_init(void)
{
    s_state_mux = xSemaphoreCreateMutex();
    if (s_state_mux == NULL) {
        return ESP_ERR_NO_MEM;
    }

    s_cmd_queue = xQueueCreate(PLAYER_CMD_QUEUE_LEN, sizeof(player_cmd_msg_t));
    if (s_cmd_queue == NULL) {
        vSemaphoreDelete(s_state_mux);
        return ESP_ERR_NO_MEM;
    }

    scan_sdcard();

    if (s_song_count > 0) {
        open_current_file();
    }

    xTaskCreate(player_task, "player_task", PLAYER_TASK_STACK_SIZE, NULL, PLAYER_TASK_PRIORITY, NULL);
    return ESP_OK;
}

void player_engine_play_index(uint8_t index)
{
    send_cmd(PLAYER_CMD_PLAY_INDEX, index);
}

void player_engine_play_pause(void)
{
    send_cmd(PLAYER_CMD_TOGGLE, 0);
}

void player_engine_toggle_play(void)
{
    send_cmd(PLAYER_CMD_TOGGLE, 0);
}

void player_engine_next(void)
{
    send_cmd(PLAYER_CMD_NEXT, 0);
}

void player_engine_prev(void)
{
    send_cmd(PLAYER_CMD_PREV, 0);
}

void player_engine_seek(uint32_t ms)
{
    send_cmd(PLAYER_CMD_SEEK, ms);
}

void player_engine_set_mode(player_mode_t mode)
{
    s_mode = mode;
}

bool player_engine_is_playing(void)
{
    return s_playing;
}

uint32_t player_engine_get_current_ms(void)
{
    return s_current_ms;
}

uint32_t player_engine_get_duration_ms(void)
{
    return s_duration_ms;
}

uint8_t player_engine_get_current_index(void)
{
    return s_current_index;
}

uint8_t player_engine_get_song_count(void)
{
    return s_song_count;
}

const player_song_t *player_engine_get_songs(void)
{
    return s_songs;
}
