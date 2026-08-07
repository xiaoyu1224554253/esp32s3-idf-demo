#include "bsp_board.h"
#include "bsp_pins.h"
#include "bsp_lcd.h"
#include "bsp_touch.h"
#include "bsp_backlight.h"
#include "bsp_audio.h"
#include "bsp_sdcard.h"
#include "esp_log.h"

static const char *TAG = "bsp_board";

static i2c_master_bus_handle_t s_i2c_bus = NULL;

esp_err_t bsp_board_init(void)
{
    esp_err_t ret = ESP_OK;

    /* I2C master bus for touch and ES8311 */
    i2c_master_bus_config_t i2c_bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .sda_io_num = BSP_TOUCH_SDA_GPIO,
        .scl_io_num = BSP_TOUCH_SCL_GPIO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ret = i2c_new_master_bus(&i2c_bus_cfg, &s_i2c_bus);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c bus create failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = bsp_backlight_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "backlight init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    bsp_backlight_set(100);

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

    ret = bsp_sdcard_init();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "sdcard init failed: %s, continuing without sdcard", esp_err_to_name(ret));
    }

    ESP_LOGI(TAG, "BSP initialized");
    return ESP_OK;
}

i2c_master_bus_handle_t bsp_i2c_get_bus(void)
{
    return s_i2c_bus;
}
