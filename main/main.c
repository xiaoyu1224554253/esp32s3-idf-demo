#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_backlight.h"
#include "bsp_lcd.h"
#include "lvgl_port.h"
#include "lvgl.h"

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

    ret = lvgl_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "LVGL port init failed");
        return;
    }

    lvgl_port_lock();

    // Simple test UI
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello ES3C28P!");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Touch me");
    lv_obj_center(btn_label);

    lvgl_port_unlock();

    ESP_LOGI(TAG, "UI created");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
