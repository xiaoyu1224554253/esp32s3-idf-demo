#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "lvgl_port.h"
#include "music_player.h"
#include "music_player_ui.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "Music Player starting");

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

    music_player_init();
    music_player_scan_sdcard();

    ui_init();
    ui_show_now_playing();

    while (1) {
        ui_update_now_playing();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
