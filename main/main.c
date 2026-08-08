#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "lvgl_port.h"
#include "ui_manager.h"
#include "player_engine.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "ESP32-S3 Music Player starting...");

    esp_err_t ret = bsp_board_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP init failed");
        return;
    }

    ret = lvgl_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "LVGL init failed");
        return;
    }

    ui_manager_init();
    ESP_LOGI(TAG, "UI initialized");

    ret = player_engine_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "player engine init failed");
        return;
    }
    ESP_LOGI(TAG, "player engine initialized");

    ESP_LOGI(TAG, "system initialized");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
