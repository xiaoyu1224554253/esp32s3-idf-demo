#include "bsp_board.h"
#include "bsp_lcd.h"
#include "bsp_touch.h"
#include "bsp_backlight.h"
#include "esp_log.h"

static const char *TAG = "bsp_board";

esp_err_t bsp_board_init(void)
{
    esp_err_t ret = ESP_OK;

    ESP_LOGI(TAG, "Initializing BSP");

    ret = bsp_backlight_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "backlight init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = bsp_lcd_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "lcd init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = bsp_touch_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "touch init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "BSP initialized");
    return ESP_OK;
}
