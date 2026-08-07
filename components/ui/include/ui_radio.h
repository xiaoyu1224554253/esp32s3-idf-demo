#ifndef UI_RADIO_H
#define UI_RADIO_H

#include "lvgl.h"
#include "ui_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UI_RADIO_MAX_STATIONS   16

typedef struct {
    const char *name;
    const char *desc;
    const char *freq;
    const char *icon;
    uint32_t icon_color;
} ui_station_info_t;

typedef struct {
    lv_obj_t *page;
    lv_obj_t *title_main;
    lv_obj_t *count_label;
    lv_obj_t *category_row;
    lv_obj_t *cat_chips[8];
    uint8_t cat_count;
    uint8_t active_cat;
    lv_obj_t *now_playing;
    lv_obj_t *np_station;
    lv_obj_t *np_program;
    lv_obj_t *np_btn;
    lv_obj_t *station_list;
    lv_obj_t *station_items[UI_RADIO_MAX_STATIONS];
    uint8_t station_count;
    uint8_t active_station;
    bool playing;
} ui_radio_t;

ui_radio_t *ui_radio_get(void);
void ui_radio_create(lv_obj_t *parent);
void ui_radio_set_categories(const char **cats, uint8_t count);
void ui_radio_set_stations(const ui_station_info_t *stations, uint8_t count);
void ui_radio_set_active_station(uint8_t index);
void ui_radio_set_playing(bool playing);
void ui_radio_set_now_playing(const char *station, const char *program);

#ifdef __cplusplus
}
#endif

#endif /* UI_RADIO_H */
