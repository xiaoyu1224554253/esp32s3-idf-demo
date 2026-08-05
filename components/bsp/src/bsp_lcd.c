#include "bsp_lcd.h"
#include "bsp_pins.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"

static const char *TAG = "bsp_lcd";

static esp_lcd_panel_io_handle_t panel_io = NULL;
static esp_lcd_panel_handle_t panel = NULL;

esp_err_t bsp_lcd_init(void)
{
    esp_err_t ret = ESP_OK;

    // SPI bus configuration
    spi_bus_config_t buscfg = {
        .mosi_io_num     = BSP_LCD_MOSI_GPIO,
        .miso_io_num     = BSP_LCD_MISO_GPIO,
        .sclk_io_num     = BSP_LCD_SCK_GPIO,
        .quadwp_io_num   = GPIO_NUM_NC,
        .quadhd_io_num   = GPIO_NUM_NC,
        .max_transfer_sz = BSP_LCD_HOR_RES * BSP_LCD_VER_RES * sizeof(uint16_t),
    };
    ret = spi_bus_initialize(BSP_LCD_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "spi bus init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // Panel IO configuration
    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num       = BSP_LCD_CS_GPIO,
        .dc_gpio_num       = BSP_LCD_DC_GPIO,
        .spi_mode          = BSP_LCD_SPI_MODE,
        .pclk_hz           = BSP_LCD_SPI_CLOCK_HZ,
        .trans_queue_depth = 10,
        .lcd_cmd_bits      = 8,
        .lcd_param_bits    = 8,
    };
    ret = esp_lcd_new_panel_io_spi(BSP_LCD_SPI_HOST, &io_config, &panel_io);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "panel io init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // Panel driver configuration
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = BSP_LCD_RST_GPIO,
        .rgb_ele_order  = BSP_LCD_RGB_ORDER,
        .bits_per_pixel = 16,
    };
    ret = esp_lcd_new_panel_ili9341(panel_io, &panel_config, &panel);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ili9341 panel init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    esp_lcd_panel_reset(panel);
    esp_lcd_panel_init(panel);
    esp_lcd_panel_invert_color(panel, BSP_LCD_INVERT_COLOR);
    esp_lcd_panel_swap_xy(panel, BSP_LCD_SWAP_XY);
    esp_lcd_panel_mirror(panel, BSP_LCD_MIRROR_X, BSP_LCD_MIRROR_Y);

    // Turn on display
    esp_lcd_panel_disp_on_off(panel, true);

    ESP_LOGI(TAG, "LCD initialized: %dx%d, swap_xy=%d", BSP_LCD_HOR_RES, BSP_LCD_VER_RES, BSP_LCD_SWAP_XY);
    return ESP_OK;
}

esp_err_t bsp_lcd_fill_screen(uint16_t color)
{
    if (panel == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    uint16_t *buffer = heap_caps_malloc(BSP_LCD_HOR_RES * sizeof(uint16_t), MALLOC_CAP_DMA);
    if (buffer == NULL) {
        return ESP_ERR_NO_MEM;
    }

    for (int i = 0; i < BSP_LCD_HOR_RES; i++) {
        buffer[i] = color;
    }

    for (int y = 0; y < BSP_LCD_VER_RES; y++) {
        esp_lcd_panel_draw_bitmap(panel, 0, y, BSP_LCD_HOR_RES, y + 1, buffer);
    }

    free(buffer);
    return ESP_OK;
}

esp_lcd_panel_handle_t bsp_lcd_get_panel(void)
{
    return panel;
}

esp_lcd_panel_io_handle_t bsp_lcd_get_panel_io(void)
{
    return panel_io;
}
