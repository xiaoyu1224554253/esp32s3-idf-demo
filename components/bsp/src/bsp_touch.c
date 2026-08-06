#include "bsp_touch.h"
#include "bsp_pins.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "bsp_touch";

static i2c_master_dev_handle_t touch_dev = NULL;

// Physical touch panel resolution (portrait)
#define TOUCH_PANEL_WIDTH   240
#define TOUCH_PANEL_HEIGHT  320

// LCD resolution after rotation (landscape)
#define LCD_WIDTH           BSP_LCD_HOR_RES
#define LCD_HEIGHT          BSP_LCD_VER_RES

extern i2c_master_bus_handle_t bsp_i2c_get_bus(void);

esp_err_t bsp_touch_init(void)
{
    esp_err_t ret = ESP_OK;

    i2c_master_bus_handle_t i2c_bus = bsp_i2c_get_bus();
    if (i2c_bus == NULL) {
        ESP_LOGE(TAG, "i2c bus not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    i2c_device_config_t dev_cfg = {
        .device_address = BSP_TOUCH_I2C_ADDR,
        .scl_speed_hz   = 400000,
        .scl_wait_us    = 0,
    };
    ret = i2c_master_bus_add_device(i2c_bus, &dev_cfg, &touch_dev);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "touch device add failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // Reset touch controller
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BSP_TOUCH_RST_GPIO),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(BSP_TOUCH_RST_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(BSP_TOUCH_RST_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG, "touch initialized");
    return ESP_OK;
}

void bsp_touch_map_to_landscape(uint16_t raw_x, uint16_t raw_y, uint16_t *lcd_x, uint16_t *lcd_y)
{
    // Clamp raw coordinates
    if (raw_x >= TOUCH_PANEL_WIDTH) raw_x = TOUCH_PANEL_WIDTH - 1;
    if (raw_y >= TOUCH_PANEL_HEIGHT) raw_y = TOUCH_PANEL_HEIGHT - 1;

    // Rotate 90 degrees clockwise: (x, y) -> (y, 239 - x)
    *lcd_x = (raw_y * LCD_WIDTH) / TOUCH_PANEL_HEIGHT;
    *lcd_y = LCD_HEIGHT - 1 - ((raw_x * LCD_HEIGHT) / TOUCH_PANEL_WIDTH);
}

bool bsp_touch_read(uint16_t *x, uint16_t *y, bool *pressed)
{
    if (touch_dev == NULL) {
        return false;
    }

    uint8_t reg = 0x02;
    uint8_t buf[5] = {0};

    esp_err_t ret = i2c_master_transmit_receive(touch_dev, &reg, 1, buf, 5, 50);
    if (ret != ESP_OK) {
        return false;
    }

    uint8_t points = buf[0] & 0x0F;
    if (points == 0) {
        *pressed = false;
        return true;
    }

    uint16_t raw_x = ((buf[1] & 0x0F) << 8) | buf[2];
    uint16_t raw_y = ((buf[3] & 0x0F) << 8) | buf[4];
    *pressed = true;

    bsp_touch_map_to_landscape(raw_x, raw_y, x, y);

    ESP_LOGD(TAG, "touch raw x=%u y=%u -> lcd x=%u y=%u", raw_x, raw_y, *x, *y);
    return true;
}
