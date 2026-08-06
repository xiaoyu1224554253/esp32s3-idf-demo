#include "audio_engine.h"
#include "mp3_decoder.h"
#include "bsp_audio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "audio_engine";

#define AUDIO_ENGINE_TASK_STACK_SIZE    8192
#define AUDIO_ENGINE_TASK_PRIORITY      5
#define PCM_BUFFER_SAMPLES              (1152 * 2)

static TaskHandle_t s_task_handle = NULL;
static mp3_decoder_t *s_decoder = NULL;
static char s_current_path[256] = {0};
static volatile bool s_playing = false;
static volatile bool s_paused = false;
static uint8_t s_volume = 50;

static int16_t s_pcm_buffer[PCM_BUFFER_SAMPLES];

static void audio_engine_task(void *arg)
{
    while (1) {
        if (!s_playing) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        if (s_paused) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        mp3_info_t info;
        int samples = mp3_decoder_decode_frame(s_decoder, s_pcm_buffer,
                                                PCM_BUFFER_SAMPLES, &info);
        if (samples <= 0) {
            ESP_LOGI(TAG, "playback finished");
            s_playing = false;
            continue;
        }

        // Convert mono to stereo if needed
        if (info.channels == 1) {
            for (int i = samples - 1; i >= 0; i--) {
                s_pcm_buffer[i * 2 + 1] = s_pcm_buffer[i];
                s_pcm_buffer[i * 2] = s_pcm_buffer[i];
            }
            samples *= 2;
        }

        bsp_audio_write(s_pcm_buffer, samples);
    }
}

esp_err_t audio_engine_init(void)
{
    s_decoder = mp3_decoder_create();
    if (s_decoder == NULL) {
        return ESP_ERR_NO_MEM;
    }

    xTaskCreate(audio_engine_task, "audio_engine", AUDIO_ENGINE_TASK_STACK_SIZE,
                NULL, AUDIO_ENGINE_TASK_PRIORITY, &s_task_handle);

    ESP_LOGI(TAG, "audio engine initialized");
    return ESP_OK;
}

esp_err_t audio_engine_play_file(const char *path)
{
    if (path == NULL || s_decoder == NULL) return ESP_ERR_INVALID_STATE;

    s_playing = false;
    vTaskDelay(pdMS_TO_TICKS(50));

    if (!mp3_decoder_open_file(s_decoder, path)) {
        return ESP_FAIL;
    }

    strncpy(s_current_path, path, sizeof(s_current_path) - 1);
    s_current_path[sizeof(s_current_path) - 1] = '\0';

    s_paused = false;
    s_playing = true;

    ESP_LOGI(TAG, "playing %s", path);
    return ESP_OK;
}

esp_err_t audio_engine_stop(void)
{
    s_playing = false;
    return ESP_OK;
}

esp_err_t audio_engine_pause(void)
{
    if (s_playing) {
        s_paused = true;
    }
    return ESP_OK;
}

esp_err_t audio_engine_resume(void)
{
    if (s_playing) {
        s_paused = false;
    }
    return ESP_OK;
}

esp_err_t audio_engine_set_volume(uint8_t volume)
{
    s_volume = volume > 100 ? 100 : volume;
    return bsp_audio_set_volume(s_volume);
}

bool audio_engine_is_playing(void)
{
    return s_playing && !s_paused;
}
