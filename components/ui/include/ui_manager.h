#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "lvgl.h"
#include "ui_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_obj_t *screen;
    lv_obj_t *status_bar;
    lv_obj_t *status_time;
    lv_obj_t *status_wifi;
    lv_obj_t *status_bat;

    lv_obj_t *content;
    lv_obj_t *pages[UI_PAGE_COUNT];
    ui_page_t current_page;

    lv_obj_t *nav_bar;
    lv_obj_t *nav_items[UI_PAGE_COUNT];
} ui_manager_t;

ui_manager_t *ui_manager_get(void);
lv_obj_t *ui_manager_get_page(ui_page_t page);
ui_page_t ui_manager_get_current_page(void);
void ui_manager_switch_page(ui_page_t page);
void ui_manager_init(void);
void ui_manager_set_status_time(const char *time_str);
void ui_manager_set_wifi_status(bool connected);
void ui_manager_set_battery(uint8_t percent);

#ifdef __cplusplus
}
#endif

#endif /* UI_MANAGER_H */
