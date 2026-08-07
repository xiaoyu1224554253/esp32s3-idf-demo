#include "bsp_lcd.h"
#include "bsp_pins.h"
#include "esp_lcd_ili9341.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/spi_master.h"
#include "esp_log.h"

static const char *TAG = "bsp_lcd";

static esp_lcd_panel_handle_t s_panel = NULL;

esp_err_t bsp_lcd_init(void)
{
    esp_err_t ret = ESP_OK;

    spi_bus_config_t bus_cfg = {
        .sclk_io_num = BSP_LCD_SCK_GPIO,
        .mosi_io_num = BSP_LCD_MOSI_GPIO,
        .miso_io_num = BSP_LCD_MISO_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = BSP_LCD_HOR_RES * BSP_LCD_VER_RES * sizeof(uint16_t),
    };
    ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "spi bus init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    esp_lcd_panel_io_spi_config_t io_cfg = {
        .dc_gpio_num = BSP_LCD_DC_GPIO,
        .cs_gpio_num = BSP_LCD_CS_GPIO,
        .pclk_hz = 40 * 1000 * 1000,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    esp_lcd_panel_io_handle_t io_handle = NULL;
    ret = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_cfg, &io_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "panel io create failed: %s", esp_err_to_name(ret));
        return ret;
    }

    esp_lcd_panel_dev_config_t panel_cfg = {
        .reset_gpio_num = BSP_LCD_RST_GPIO,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
    };
    ret = esp_lcd_new_panel_ili9341(io_handle, &panel_cfg, &s_panel);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "panel create failed: %s", esp_err_to_name(ret));
        return ret;
    }

    esp_lcd_panel_reset(s_panel);
    esp_lcd_panel_init(s_panel);
    esp_lcd_panel_mirror(s_panel, false, false);
    esp_lcd_panel_swap_xy(s_panel, true);
    esp_lcd_panel_set_gap(s_panel, 0, 0);
    esp_lcd_panel_invert_color(s_panel, false);
    esp_lcd_panel_disp_on_off(s_panel, true);

    ESP_LOGI(TAG, "LCD initialized: %dx%d", BSP_LCD_HOR_RES, BSP_LCD_VER_RES);
    return ESP_OK;
}

esp_lcd_panel_handle_t bsp_lcd_get_panel(void)
{
    return s_panel;
}
