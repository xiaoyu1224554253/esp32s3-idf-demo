#include "bsp_backlight.h"
#include "bsp_pins.h"
#include "driver/ledc.h"
#include "esp_log.h"

static const char *TAG = "bsp_backlight";

#define BACKLIGHT_LEDC_TIMER      LEDC_TIMER_0
#define BACKLIGHT_LEDC_MODE       LEDC_LOW_SPEED_MODE
#define BACKLIGHT_LEDC_CHANNEL    LEDC_CHANNEL_0
#define BACKLIGHT_LEDC_DUTY_RES   LEDC_TIMER_8_BIT
#define BACKLIGHT_LEDC_FREQUENCY  5000

esp_err_t bsp_backlight_init(void)
{
    ledc_timer_config_t timer_cfg = {
        .speed_mode       = BACKLIGHT_LEDC_MODE,
        .duty_resolution  = BACKLIGHT_LEDC_DUTY_RES,
        .timer_num        = BACKLIGHT_LEDC_TIMER,
        .freq_hz          = BACKLIGHT_LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_cfg));

    ledc_channel_config_t channel_cfg = {
        .speed_mode     = BACKLIGHT_LEDC_MODE,
        .channel        = BACKLIGHT_LEDC_CHANNEL,
        .timer_sel      = BACKLIGHT_LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = BSP_LCD_BL_GPIO,
        .duty           = 0,
        .hpoint         = 0,
    };
    return ledc_channel_config(&channel_cfg);
}

esp_err_t bsp_backlight_set(uint8_t brightness)
{
    if (brightness > 100) {
        brightness = 100;
    }
    uint32_t duty = (brightness * 255) / 100;
    esp_err_t ret = ledc_set_duty(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL, duty);
    if (ret != ESP_OK) {
        return ret;
    }
    return ledc_update_duty(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL);
}
