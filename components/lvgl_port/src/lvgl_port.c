#include "lvgl_port.h"
#include "bsp_lcd.h"
#include "bsp_touch.h"
#include "bsp_pins.h"
#include "lvgl.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

static const char *TAG = "lvgl_port";

#define LVGL_DRAW_BUFFER_SIZE   (BSP_LCD_HOR_RES * BSP_LCD_VER_RES / 10)
#define LVGL_TASK_STACK_SIZE    8192
#define LVGL_TASK_PRIORITY      5

static SemaphoreHandle_t s_lvgl_mux = NULL;
static lv_disp_drv_t s_disp_drv;
static lv_indev_drv_t s_indev_drv;
static lv_color_t *s_draw_buf1 = NULL;
static lv_color_t *s_draw_buf2 = NULL;

static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel = (esp_lcd_panel_handle_t)drv->user_data;
    int offset_x1 = area->x1;
    int offset_y1 = area->y1;
    int offset_x2 = area->x2;
    int offset_y2 = area->y2;

    esp_lcd_panel_draw_bitmap(panel, offset_x1, offset_y1, offset_x2 + 1, offset_y2 + 1, color_map);
    lv_disp_flush_ready(drv);
}

static void lvgl_touch_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    uint16_t x = 0, y = 0;
    bool pressed = false;

    if (bsp_touch_read(&x, &y, &pressed) && pressed) {
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static void lvgl_tick_task(void *arg)
{
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5));
        lv_tick_inc(5);
    }
}

static void lvgl_task(void *arg)
{
    while (1) {
        lvgl_port_lock();
        uint32_t delay_ms = lv_timer_handler();
        lvgl_port_unlock();
        if (delay_ms > 500) {
            delay_ms = 500;
        }
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

esp_err_t lvgl_port_init(void)
{
    ESP_LOGI(TAG, "initialize LVGL");

    lv_init();

    s_draw_buf1 = heap_caps_malloc(LVGL_DRAW_BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    if (s_draw_buf1 == NULL) {
        ESP_LOGE(TAG, "draw buffer 1 allocation failed");
        return ESP_ERR_NO_MEM;
    }
    s_draw_buf2 = heap_caps_malloc(LVGL_DRAW_BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    if (s_draw_buf2 == NULL) {
        ESP_LOGE(TAG, "draw buffer 2 allocation failed");
        return ESP_ERR_NO_MEM;
    }

    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, s_draw_buf1, s_draw_buf2, LVGL_DRAW_BUFFER_SIZE);

    lv_disp_drv_init(&s_disp_drv);
    s_disp_drv.hor_res = BSP_LCD_HOR_RES;
    s_disp_drv.ver_res = BSP_LCD_VER_RES;
    s_disp_drv.flush_cb = lvgl_flush_cb;
    s_disp_drv.draw_buf = &draw_buf;
    s_disp_drv.user_data = bsp_lcd_get_panel();
    lv_disp_drv_register(&s_disp_drv);

    lv_indev_drv_init(&s_indev_drv);
    s_indev_drv.type = LV_INDEV_TYPE_POINTER;
    s_indev_drv.read_cb = lvgl_touch_cb;
    lv_indev_drv_register(&s_indev_drv);

    s_lvgl_mux = xSemaphoreCreateRecursiveMutex();
    if (s_lvgl_mux == NULL) {
        ESP_LOGE(TAG, "lvgl mutex creation failed");
        return ESP_ERR_NO_MEM;
    }

    xTaskCreate(lvgl_tick_task, "lvgl_tick", 2048, NULL, LVGL_TASK_PRIORITY, NULL);
    xTaskCreate(lvgl_task, "lvgl_task", LVGL_TASK_STACK_SIZE, NULL, LVGL_TASK_PRIORITY, NULL);

    return ESP_OK;
}

void lvgl_port_lock(void)
{
    if (s_lvgl_mux) {
        xSemaphoreTakeRecursive(s_lvgl_mux, portMAX_DELAY);
    }
}

void lvgl_port_unlock(void)
{
    if (s_lvgl_mux) {
        xSemaphoreGiveRecursive(s_lvgl_mux);
    }
}
