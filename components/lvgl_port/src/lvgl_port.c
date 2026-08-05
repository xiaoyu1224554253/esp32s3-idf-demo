#include "lvgl_port.h"
#include "bsp_lcd.h"
#include "bsp_touch.h"
#include "bsp_pins.h"
#include "lvgl.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

static const char *TAG = "lvgl_port";

static lv_disp_t *disp = NULL;
static SemaphoreHandle_t lvgl_mutex = NULL;

static void disp_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel = bsp_lcd_get_panel();
    if (panel == NULL) {
        lv_disp_flush_ready(drv);
        return;
    }

    int offset_x = 0;
    int offset_y = 0;
    esp_lcd_panel_draw_bitmap(panel, offset_x + area->x1, offset_y + area->y1,
                              offset_x + area->x2 + 1, offset_y + area->y2 + 1,
                              color_map);
    lv_disp_flush_ready(drv);
}

static void touch_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    uint16_t x, y;
    bool pressed = false;

    if (bsp_touch_read(&x, &y, &pressed)) {
        data->point.x = x;
        data->point.y = y;
        data->state = pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static void lvgl_tick_task(void *arg)
{
    (void)arg;
    while (1) {
        lv_tick_inc(5);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

static void lvgl_task(void *arg)
{
    (void)arg;
    while (1) {
        if (xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE) {
            lv_timer_handler();
            xSemaphoreGive(lvgl_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

esp_err_t lvgl_port_init(void)
{
    ESP_LOGI(TAG, "Initializing LVGL port");

    lvgl_mutex = xSemaphoreCreateMutex();
    if (lvgl_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create LVGL mutex");
        return ESP_FAIL;
    }

    lv_init();

    // LVGL tick task
    xTaskCreatePinnedToCore(lvgl_tick_task, "lvgl_tick", 2048, NULL, 5, NULL, 1);

    // LVGL display buffer
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf1[BSP_LCD_HOR_RES * 20];
    static lv_color_t buf2[BSP_LCD_HOR_RES * 20];
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, BSP_LCD_HOR_RES * 20);

    // Display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = BSP_LCD_HOR_RES;
    disp_drv.ver_res = BSP_LCD_VER_RES;
    disp_drv.flush_cb = disp_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = 0;
    disp_drv.direct_mode = 0;
    disp = lv_disp_drv_register(&disp_drv);
    if (disp == NULL) {
        ESP_LOGE(TAG, "Failed to register display driver");
        return ESP_FAIL;
    }

    // Input device driver
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read_cb;
    lv_indev_t *indev = lv_indev_drv_register(&indev_drv);
    if (indev == NULL) {
        ESP_LOGE(TAG, "Failed to register input device");
        return ESP_FAIL;
    }

    // LVGL main task
    xTaskCreatePinnedToCore(lvgl_task, "lvgl_task", 4096, NULL, 5, NULL, 1);

    ESP_LOGI(TAG, "LVGL port initialized");
    return ESP_OK;
}

void lvgl_port_lock(void)
{
    if (lvgl_mutex != NULL) {
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
    }
}

void lvgl_port_unlock(void)
{
    if (lvgl_mutex != NULL) {
        xSemaphoreGive(lvgl_mutex);
    }
}
