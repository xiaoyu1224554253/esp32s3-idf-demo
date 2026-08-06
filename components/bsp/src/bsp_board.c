#include "bsp_board.h"
#include "bsp_pins.h"
#include "bsp_lcd.h"
#include "bsp_touch.h"
#include "bsp_backlight.h"
#include "bsp_audio.h"
#include "esp_log.h"

static const char *TAG = "bsp_board";

static i2c_master_bus_handle_t i2c_bus = NULL;

i2c_master_bus_handle_t bsp_i2c_get_bus(void)
{
    return i2c_bus;
}

esp_err_t bsp_board_init(void)
{
    esp_err_t ret = ESP_OK;

    ESP_LOGI(TAG, "Initializing BSP");

    i2c_master_bus_config_t i2c_bus_cfg = {
        .i2c_port = BSP_TOUCH_I2C_NUM,
        .sda_io_num = BSP_TOUCH_I2C_SDA_GPIO,
        .scl_io_num = BSP_TOUCH_I2C_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags = {
            .enable_internal_pullup = 1,
        },
    };
    ret = i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c bus init failed: %s", esp_err_to_name(ret));
        return ret;
    }

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

    ret = bsp_audio_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "audio init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "BSP initialized");
    return ESP_OK;
}
