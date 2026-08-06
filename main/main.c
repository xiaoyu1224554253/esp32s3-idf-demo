#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_audio.h"

static const char *TAG = "MAIN";

#define SINE_SAMPLES 256
static int16_t sine_buffer[SINE_SAMPLES];

void app_main(void)
{
    ESP_LOGI(TAG, "Audio sine wave test");

    esp_err_t ret = bsp_board_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP init failed");
        return;
    }

    bsp_audio_set_volume(50);
    bsp_audio_start();

    for (int i = 0; i < SINE_SAMPLES; i++) {
        sine_buffer[i] = (int16_t)(30000 * sin(2 * M_PI * 1000 * i / BSP_AUDIO_SAMPLE_RATE));
    }

    while (1) {
        bsp_audio_write(sine_buffer, SINE_SAMPLES);
    }
}
