#ifndef UI_SEARCH_H
#define UI_SEARCH_H

#include "lvgl.h"
#include "ui_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UI_SEARCH_MAX_RESULTS   16
#define UI_SEARCH_MAX_HISTORY   8

typedef struct {
    const char *title;
    const char *artist;
    const char *source;
    const char *time;
} ui_search_result_t;

typedef struct {
    lv_obj_t *page;
    lv_obj_t *input;
    lv_obj_t *voice_btn;
    lv_obj_t *status;
    lv_obj_t *status_text;
    lv_obj_t *history_section;
    lv_obj_t *history_tags[UI_SEARCH_MAX_HISTORY];
    uint8_t history_count;
    lv_obj_t *result_section;
    lv_obj_t *result_list;
    lv_obj_t *result_items[UI_SEARCH_MAX_RESULTS];
    lv_obj_t *empty;
    uint8_t result_count;
    bool listening;
} ui_search_t;

ui_search_t *ui_search_get(void);
void ui_search_create(lv_obj_t *parent);
void ui_search_set_history(const char **history, uint8_t count);
void ui_search_set_results(const ui_search_result_t *results, uint8_t count);
void ui_search_set_status(const char *text, bool visible);
void ui_search_set_listening(bool listening);

#ifdef __cplusplus
}
#endif

#endif /* UI_SEARCH_H */
