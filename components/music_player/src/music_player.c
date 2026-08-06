#include "music_player.h"
#include "audio_engine.h"
#include "bsp_sdcard.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

static const char *TAG = "music_player";

static music_player_state_t s_state = {0};

#define PROGRESS_UPDATE_INTERVAL_MS 1000

static bool is_mp3(const char *filename)
{
    size_t len = strlen(filename);
    return len > 4 && strcasecmp(filename + len - 4, ".mp3") == 0;
}

static void extract_title(const char *filename, char *out_title, size_t out_len)
{
    strncpy(out_title, filename, out_len - 1);
    out_title[out_len - 1] = '\0';

    size_t len = strlen(out_title);
    if (len > 4 && strcasecmp(out_title + len - 4, ".mp3") == 0) {
        out_title[len - 4] = '\0';
    }
}

static void progress_update_task(void *arg)
{
    (void)arg;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(PROGRESS_UPDATE_INTERVAL_MS));
        if (s_state.is_playing && s_state.progress_ms < s_state.duration_ms) {
            s_state.progress_ms += PROGRESS_UPDATE_INTERVAL_MS;
        }
    }
}

void music_player_init(void)
{
    memset(&s_state, 0, sizeof(s_state));
    s_state.volume = 50;
    s_state.play_mode = PLAY_MODE_SEQUENTIAL;
    s_state.current_index = -1;

    audio_engine_init();
    audio_engine_set_volume(s_state.volume);

    xTaskCreate(progress_update_task, "progress_update", 2048, NULL, 2, NULL);

    ESP_LOGI(TAG, "music player initialized");
}

esp_err_t music_player_scan_sdcard(void)
{
    s_state.track_count = 0;

    if (!bsp_sdcard_is_mounted()) {
        ESP_LOGE(TAG, "sdcard not mounted");
        return ESP_ERR_INVALID_STATE;
    }

    DIR *dir = opendir(bsp_sdcard_get_mount_point());
    if (dir == NULL) {
        ESP_LOGE(TAG, "failed to open sdcard dir");
        return ESP_FAIL;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!is_mp3(entry->d_name)) continue;
        if (s_state.track_count >= MUSIC_PLAYER_MAX_TRACKS) break;

        music_player_track_t *track = &s_state.tracks[s_state.track_count];
        const char *mount = bsp_sdcard_get_mount_point();
        strncpy(track->path, mount, sizeof(track->path) - 1);
        track->path[sizeof(track->path) - 1] = '\0';
        size_t path_len = strlen(track->path);
        if (path_len < sizeof(track->path) - 1) {
            track->path[path_len++] = '/';
            track->path[path_len] = '\0';
        }
        if (path_len < sizeof(track->path) - 1) {
            strncat(track->path, entry->d_name, sizeof(track->path) - path_len - 1);
        }
        extract_title(entry->d_name, track->title, sizeof(track->title));
        strncpy(track->artist, "Unknown Artist", sizeof(track->artist) - 1);
        track->duration_ms = 0;

        s_state.track_count++;
    }
    closedir(dir);

    ESP_LOGI(TAG, "scanned %lu tracks", s_state.track_count);
    return ESP_OK;
}

uint32_t music_player_get_track_count(void)
{
    return s_state.track_count;
}

const music_player_track_t *music_player_get_track(uint32_t index)
{
    if (index >= s_state.track_count) return NULL;
    return &s_state.tracks[index];
}

const music_player_state_t *music_player_get_state(void)
{
    return &s_state;
}

void music_player_play(uint32_t index)
{
    if (index >= s_state.track_count) return;

    s_state.current_index = index;
    s_state.progress_ms = 0;
    s_state.duration_ms = s_state.tracks[index].duration_ms;

    audio_engine_play_file(s_state.tracks[index].path);
    s_state.is_playing = true;

    ESP_LOGI(TAG, "play track %lu: %s", index, s_state.tracks[index].title);
}

void music_player_pause(void)
{
    audio_engine_pause();
    s_state.is_playing = false;
    ESP_LOGI(TAG, "paused");
}

void music_player_resume(void)
{
    if (s_state.current_index < 0 && s_state.track_count > 0) {
        music_player_play(0);
        return;
    }
    audio_engine_resume();
    s_state.is_playing = true;
    ESP_LOGI(TAG, "resumed");
}

void music_player_toggle(void)
{
    if (s_state.is_playing) {
        music_player_pause();
    } else {
        music_player_resume();
    }
}

void music_player_next(void)
{
    if (s_state.track_count == 0) return;

    int next = 0;
    switch (s_state.play_mode) {
        case PLAY_MODE_RANDOM:
            next = rand() % s_state.track_count;
            break;
        case PLAY_MODE_REPEAT_ONE:
            next = s_state.current_index >= 0 ? s_state.current_index : 0;
            break;
        default:
            next = s_state.current_index + 1;
            if (next >= (int)s_state.track_count) {
                next = 0;
            }
            break;
    }
    music_player_play(next);
}

void music_player_prev(void)
{
    if (s_state.track_count == 0) return;

    int prev = s_state.current_index - 1;
    if (prev < 0) {
        prev = s_state.track_count - 1;
    }
    music_player_play(prev);
}

void music_player_stop(void)
{
    audio_engine_stop();
    s_state.is_playing = false;
    s_state.current_index = -1;
    s_state.progress_ms = 0;
}

void music_player_set_volume(uint8_t volume)
{
    if (volume > 100) volume = 100;
    s_state.volume = volume;
    audio_engine_set_volume(volume);
}

uint8_t music_player_get_volume(void)
{
    return s_state.volume;
}

void music_player_set_play_mode(music_player_play_mode_t mode)
{
    s_state.play_mode = mode;
}

music_player_play_mode_t music_player_get_play_mode(void)
{
    return s_state.play_mode;
}

void music_player_set_progress(uint32_t progress_ms)
{
    s_state.progress_ms = progress_ms;
}

uint32_t music_player_get_progress_ms(void)
{
    return s_state.progress_ms;
}

bool music_player_is_playing(void)
{
    return s_state.is_playing;
}
