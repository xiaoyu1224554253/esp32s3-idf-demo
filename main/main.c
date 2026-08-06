#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_sdcard.h"
#include "audio_engine.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "MP3 playback test");

    esp_err_t ret = bsp_board_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP init failed");
        return;
    }

    ret = audio_engine_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "audio engine init failed");
        return;
    }

    audio_engine_set_volume(50);
    audio_engine_play_file("/sdcard/test.mp3");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
