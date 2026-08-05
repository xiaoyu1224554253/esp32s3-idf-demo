#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_backlight.h"
#include "bsp_lcd.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "ESP32-S3 Music Player starting");

    esp_err_t ret = bsp_board_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BSP init failed");
        return;
    }

    bsp_backlight_set(100);

    // Test: fill screen with red, green, blue
    while (1) {
        ESP_LOGI(TAG, "Fill red");
        bsp_lcd_fill_screen(0xF800); // RGB565 red
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "Fill green");
        bsp_lcd_fill_screen(0x07E0); // RGB565 green
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "Fill blue");
        bsp_lcd_fill_screen(0x001F); // RGB565 blue
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
