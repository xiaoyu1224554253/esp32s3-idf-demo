#include "ui_search.h"
#include "ui_manager.h"
#include "player_engine.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "ui_search";

static ui_search_t s_search;

static const char *s_default_history[] = {"周杰伦", "林俊杰", "陈奕迅", "夜曲"};

static const ui_search_result_t s_default_results[] = {
    {"夜曲", "周杰伦", "网络", "03:46"},
    {"晴天", "周杰伦", "网络", "04:29"},
    {"七里香", "周杰伦", "SD卡", "04:59"},
    {"稻香", "周杰伦", "SD卡", "03:43"},
    {"告白气球", "周杰伦", "网络", "03:35"},
    {"青花瓷", "周杰伦", "SD卡", "03:59"},
    {"江南", "林俊杰", "网络", "04:10"},
    {"修炼爱情", "林俊杰", "网络", "04:45"},
    {"十年", "陈奕迅", "SD卡", "03:28"},
    {"富士山下", "陈奕迅", "网络", "04:20"},
};

static void search_internal(const char *keyword)
{
    ui_search_set_status("搜索中...", true);

    /* Simple filter on default library */
    ui_search_result_t filtered[UI_SEARCH_MAX_RESULTS];
    uint8_t count = 0;
    if (keyword == NULL || strlen(keyword) == 0) {
        memcpy(filtered, s_default_results, sizeof(s_default_results));
        count = sizeof(s_default_results) / sizeof(s_default_results[0]);
        if (count > UI_SEARCH_MAX_RESULTS) {
            count = UI_SEARCH_MAX_RESULTS;
        }
    } else {
        for (int i = 0; i < sizeof(s_default_results) / sizeof(s_default_results[0]) && count < UI_SEARCH_MAX_RESULTS; i++) {
            if (strstr(s_default_results[i].title, keyword) || strstr(s_default_results[i].artist, keyword)) {
                filtered[count++] = s_default_results[i];
            }
        }
    }

    /* Simulate async delay later via timer */
    ui_search_set_results(filtered, count);
    ui_search_set_status(NULL, false);
}

static void search_submit_cb(lv_event_t *e)
{
    const char *text = lv_textarea_get_text(s_search.input);
    search_internal(text);
}

static void voice_click_cb(lv_event_t *e)
{
    ui_search_set_listening(true);
    ui_search_set_status("正在聆听...", true);
    /* Simulate voice recognition after 1.5s */
    lv_textarea_set_text(s_search.input, "周杰伦");
    ui_search_set_listening(false);
    search_internal("周杰伦");
}

static void history_click_cb(lv_event_t *e)
{
    lv_obj_t *tag = lv_event_get_target(e);
    lv_obj_t *label = lv_obj_get_child(tag, 0);
    const char *text = lv_label_get_text(label);
    lv_textarea_set_text(s_search.input, text);
    search_internal(text);
}

static void result_click_cb(lv_event_t *e)
{
    lv_obj_t *item = lv_event_get_target(e);
    int idx = (int)(intptr_t)lv_obj_get_user_data(item);
    if (idx >= 0 && idx < s_search.result_count) {
        ui_set_style_bg(item, UI_COLOR_PRIMARY, LV_OPA_18);
    }
}

ui_search_t *ui_search_get(void)
{
    return &s_search;
}

void ui_search_create(lv_obj_t *parent)
{
    memset(&s_search, 0, sizeof(s_search));
    s_search.page = parent;

    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_hor(parent, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_top(parent, 4, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(parent, 2, LV_PART_MAIN);

    /* Search box */
    lv_obj_t *box = lv_obj_create(parent);
    lv_obj_set_size(box, UI_SCREEN_WIDTH - 16, 30);
    lv_obj_set_flex_flow(box, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(box, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(box, 6, LV_PART_MAIN);
    ui_set_style_bg(box, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(box, 0, LV_PART_MAIN);

    s_search.input = lv_textarea_create(box);
    lv_obj_set_size(s_search.input, UI_SCREEN_WIDTH - 16 - 36, 30);
    lv_textarea_set_one_line(s_search.input, true);
    lv_textarea_set_placeholder_text(s_search.input, "搜索歌曲、歌手、专辑...");
    ui_set_style_bg(s_search.input, UI_COLOR_SURFACE_LIGHT, LV_OPA_COVER);
    ui_set_style_border(s_search.input, UI_COLOR_BORDER, 1, 15);
    ui_set_style_text(s_search.input, UI_COLOR_TEXT, UI_FONT_SMALL);
    lv_obj_set_style_pad_hor(s_search.input, 12, LV_PART_MAIN);
    lv_obj_set_style_pad_ver(s_search.input, 6, LV_PART_MAIN);
    lv_obj_add_event_cb(s_search.input, search_submit_cb, LV_EVENT_READY, NULL);

    s_search.voice_btn = ui_create_round_btn(box, 30, UI_COLOR_PRIMARY, "\xF0\x9F\x8E\xA4", UI_FONT_SMALL);
    lv_obj_set_style_bg_opa(s_search.voice_btn, LV_OPA_15, LV_PART_MAIN);
    ui_set_style_border(s_search.voice_btn, UI_COLOR_PRIMARY, 1, 15);
    lv_obj_set_style_border_opa(s_search.voice_btn, LV_OPA_25, LV_PART_MAIN);
    lv_obj_add_event_cb(s_search.voice_btn, voice_click_cb, LV_EVENT_CLICKED, NULL);

    /* Status */
    s_search.status = lv_obj_create(parent);
    lv_obj_set_size(s_search.status, UI_SCREEN_WIDTH - 16, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(s_search.status, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(s_search.status, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(s_search.status, 6, LV_PART_MAIN);
    ui_set_style_bg(s_search.status, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(s_search.status, 0, LV_PART_MAIN);
    lv_obj_add_flag(s_search.status, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *spinner = lv_spinner_create(s_search.status, 700, 60);
    lv_obj_set_size(spinner, 11, 11);
    ui_set_style_bg(spinner, UI_COLOR_PRIMARY, LV_OPA_30);
    lv_obj_set_style_arc_color(spinner, UI_COLOR_PRIMARY, LV_PART_INDICATOR);

    s_search.status_text = lv_label_create(s_search.status);
    lv_label_set_text(s_search.status_text, "搜索中...");
    ui_set_style_text(s_search.status_text, UI_COLOR_PRIMARY, UI_FONT_SMALL);

    /* History section */
    s_search.history_section = lv_obj_create(parent);
    lv_obj_set_size(s_search.history_section, UI_SCREEN_WIDTH - 16, LV_SIZE_CONTENT);
    ui_set_style_bg(s_search.history_section, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(s_search.history_section, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(s_search.history_section, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(s_search.history_section, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_gap(s_search.history_section, 4, LV_PART_MAIN);

    lv_obj_t *hist_title = lv_label_create(s_search.history_section);
    lv_label_set_text(hist_title, "搜索历史");
    ui_set_style_text(hist_title, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);

    lv_obj_t *tag_row = lv_obj_create(s_search.history_section);
    lv_obj_set_size(tag_row, UI_SCREEN_WIDTH - 16, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(tag_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(tag_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_flex_wrap(tag_row, true, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(tag_row, 5, LV_PART_MAIN);
    ui_set_style_bg(tag_row, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(tag_row, 0, LV_PART_MAIN);

    for (int i = 0; i < sizeof(s_default_history) / sizeof(s_default_history[0]); i++) {
        lv_obj_t *tag = lv_btn_create(tag_row);
        lv_obj_set_size(tag, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_hor(tag, 8, LV_PART_MAIN);
        lv_obj_set_style_pad_ver(tag, 3, LV_PART_MAIN);
        lv_obj_set_style_radius(tag, 10, LV_PART_MAIN);
        ui_set_style_bg(tag, UI_COLOR_SURFACE_LIGHT, LV_OPA_COVER);
        ui_set_style_border(tag, UI_COLOR_BORDER, 1, 10);
        lv_obj_add_event_cb(tag, history_click_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_t *label = lv_label_create(tag);
        lv_label_set_text(label, s_default_history[i]);
        ui_set_style_text(label, UI_COLOR_TEXT_SECONDARY, UI_FONT_SMALL);
        lv_obj_center(label);

        s_search.history_tags[i] = tag;
        s_search.history_count++;
    }

    /* Result section */
    s_search.result_section = lv_obj_create(parent);
    lv_obj_set_size(s_search.result_section, UI_SCREEN_WIDTH - 16, LV_SIZE_CONTENT);
    ui_set_style_bg(s_search.result_section, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(s_search.result_section, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(s_search.result_section, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(s_search.result_section, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_gap(s_search.result_section, 4, LV_PART_MAIN);

    lv_obj_t *res_title = lv_label_create(s_search.result_section);
    lv_label_set_text(res_title, "搜索结果");
    ui_set_style_text(res_title, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);

    s_search.result_list = lv_obj_create(s_search.result_section);
    lv_obj_set_size(s_search.result_list, UI_SCREEN_WIDTH - 16, UI_CONTENT_HEIGHT - 30 - 6 - 24 - 8);
    ui_set_style_bg(s_search.result_list, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(s_search.result_list, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(s_search.result_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(s_search.result_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_gap(s_search.result_list, 0, LV_PART_MAIN);

    s_search.empty = lv_label_create(s_search.result_list);
    lv_label_set_text(s_search.empty, "未找到相关结果");
    ui_set_style_text(s_search.empty, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);
    lv_obj_center(s_search.empty);
    lv_obj_add_flag(s_search.empty, LV_OBJ_FLAG_HIDDEN);

    ui_search_set_results(s_default_results, sizeof(s_default_results) / sizeof(s_default_results[0]));
}

void ui_search_set_history(const char **history, uint8_t count)
{
    /* Not dynamically updated in this version */
    (void)history;
    (void)count;
}

void ui_search_set_results(const ui_search_result_t *results, uint8_t count)
{
    if (count > UI_SEARCH_MAX_RESULTS) {
        count = UI_SEARCH_MAX_RESULTS;
    }

    /* Clear list except empty label */
    lv_obj_t *child;
    while ((child = lv_obj_get_child(s_search.result_list, 0)) != NULL && child != s_search.empty) {
        lv_obj_del(child);
    }
    memset(s_search.result_items, 0, sizeof(s_search.result_items));
    s_search.result_count = 0;

    if (count == 0) {
        lv_obj_clear_flag(s_search.empty, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    lv_obj_add_flag(s_search.empty, LV_OBJ_FLAG_HIDDEN);

    for (int i = 0; i < count; i++) {
        lv_obj_t *item = lv_btn_create(s_search.result_list);
        lv_obj_set_size(item, UI_SCREEN_WIDTH - 16, 36);
        lv_obj_set_style_radius(item, 0, LV_PART_MAIN);
        ui_set_style_bg(item, UI_COLOR_BG, LV_OPA_COVER);
        lv_obj_set_style_border_width(item, 0, LV_PART_MAIN);
        lv_obj_set_style_border_side(item, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN);
        lv_obj_set_style_border_color(item, UI_COLOR_TEXT, LV_PART_MAIN);
        lv_obj_set_style_border_opa(item, LV_OPA_5, LV_PART_MAIN);
        lv_obj_set_user_data(item, (void *)(intptr_t)i);
        lv_obj_add_event_cb(item, result_click_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_t *thumb = lv_obj_create(item);
        lv_obj_set_size(thumb, 28, 28);
        lv_obj_align(thumb, LV_ALIGN_LEFT_MID, 0, 0);
        ui_set_style_bg(thumb, UI_COLOR_PRIMARY, LV_OPA_COVER);
        lv_obj_set_style_radius(thumb, 4, LV_PART_MAIN);
        lv_obj_set_style_border_width(thumb, 0, LV_PART_MAIN);
        lv_obj_t *thumb_icon = lv_label_create(thumb);
        lv_label_set_text(thumb_icon, "\xF0\x9F\x8E\xB5");
        ui_set_style_text(thumb_icon, UI_COLOR_TEXT, UI_FONT_SMALL);
        lv_obj_center(thumb_icon);

        lv_obj_t *info = lv_obj_create(item);
        lv_obj_set_size(info, UI_SCREEN_WIDTH - 16 - 28 - 6, 28);
        lv_obj_align(info, LV_ALIGN_LEFT_MID, 34, 0);
        lv_obj_set_flex_flow(info, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(info, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        ui_set_style_bg(info, UI_COLOR_BG, LV_OPA_COVER);
        lv_obj_set_style_border_width(info, 0, LV_PART_MAIN);

        lv_obj_t *title = lv_label_create(info);
        lv_label_set_text_fmt(title, "%s - %s", results[i].title, results[i].artist);
        ui_set_style_text(title, UI_COLOR_TEXT, UI_FONT_NORMAL);
        lv_label_set_long_mode(title, LV_LABEL_LONG_DOT);
        lv_obj_set_width(title, UI_SCREEN_WIDTH - 60);

        lv_obj_t *source = lv_label_create(info);
        lv_label_set_text_fmt(source, "%s · %s", results[i].source, results[i].time);
        ui_set_style_text(source, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);

        s_search.result_items[i] = item;
        s_search.result_count++;
    }
}

void ui_search_set_status(const char *text, bool visible)
{
    if (text) {
        lv_label_set_text(s_search.status_text, text);
    }
    if (visible) {
        lv_obj_clear_flag(s_search.status, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(s_search.status, LV_OBJ_FLAG_HIDDEN);
    }
}

void ui_search_set_listening(bool listening)
{
    s_search.listening = listening;
    if (listening) {
        ui_set_style_bg(s_search.voice_btn, UI_COLOR_PRIMARY, LV_OPA_COVER);
        ui_set_style_text(lv_obj_get_child(s_search.voice_btn, 0), UI_COLOR_TEXT, UI_FONT_SMALL);
    } else {
        lv_obj_set_style_bg_opa(s_search.voice_btn, LV_OPA_15, LV_PART_MAIN);
        ui_set_style_text(lv_obj_get_child(s_search.voice_btn, 0), UI_COLOR_PRIMARY, UI_FONT_SMALL);
    }
}
