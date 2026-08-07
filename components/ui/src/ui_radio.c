#include "ui_radio.h"
#include "ui_manager.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "ui_radio";

static ui_radio_t s_radio;

static const char *s_default_cats[] = {"全部", "流行", "古典", "爵士", "电子", "民谣", "新闻", "动漫"};

static const ui_station_info_t s_default_stations[] = {
    {"华语流行 FM", "热门金曲 24h 不停歇", "FM 88.7", "\xF0\x9F\x8E\xA4", 0xFF6B81},
    {"古典音乐厅", "交响乐与室内乐精选", "FM 92.1", "\xF0\x9F\x8E\xBB", 0x74B9FF},
    {"爵士咖啡馆", "慵懒午后 轻松爵士", "FM 95.3", "\xF0\x9F\x8E\xB7", 0xFDCB6E},
    {"电音浪潮", "EDM / House / Techno", "FM 97.6", "\xF0\x9F\x8E\xA7", 0xA29BFE},
    {"民谣时光", "城市与远方的故事", "FM 100.2", "\xF0\x9F\xAA\x95", 0x55EFC4},
    {"新闻资讯台", "整点新闻 + 天气播报", "FM 103.5", "\xF0\x9F\x93\xB0", 0xDFE6E9},
    {"ACG 动漫电台", "二次元 OST / OP / ED", "FM 106.9", "\xE2\x9C\xA8", 0xFD79A8},
    {"深夜故事会", "情感夜话 治愈陪伴", "FM 108.0", "\xF0\x9F\x8C\x99", 0xFFEAA7},
};

static void cat_click_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    int idx = (int)(intptr_t)lv_obj_get_user_data(btn);
    if (idx >= 0 && idx < s_radio.cat_count) {
        s_radio.active_cat = idx;
        for (int i = 0; i < s_radio.cat_count; i++) {
            if (i == idx) {
                ui_set_style_bg(s_radio.cat_chips[i], UI_COLOR_PRIMARY, LV_OPA_10);
                ui_set_style_border(s_radio.cat_chips[i], UI_COLOR_PRIMARY, 1, 12);
                lv_obj_set_style_border_opa(s_radio.cat_chips[i], LV_OPA_40, LV_PART_MAIN);
                ui_set_style_text(lv_obj_get_child(s_radio.cat_chips[i], 0), UI_COLOR_PRIMARY, UI_FONT_SMALL);
            } else {
                ui_set_style_bg(s_radio.cat_chips[i], UI_COLOR_SURFACE_LIGHT, LV_OPA_COVER);
                ui_set_style_border(s_radio.cat_chips[i], UI_COLOR_BORDER, 1, 12);
                ui_set_style_text(lv_obj_get_child(s_radio.cat_chips[i], 0), UI_COLOR_TEXT_SECONDARY, UI_FONT_SMALL);
            }
        }
    }
}

static void station_click_cb(lv_event_t *e)
{
    lv_obj_t *item = lv_event_get_target(e);
    int idx = (int)(intptr_t)lv_obj_get_user_data(item);
    if (idx >= 0 && idx < s_radio.station_count) {
        ui_radio_set_active_station(idx);
        ui_radio_set_now_playing(s_default_stations[idx].name, s_default_stations[idx].desc);
    }
}

static void np_btn_click_cb(lv_event_t *e)
{
    ui_radio_set_playing(!s_radio.playing);
}

ui_radio_t *ui_radio_get(void)
{
    return &s_radio;
}

void ui_radio_create(lv_obj_t *parent)
{
    memset(&s_radio, 0, sizeof(s_radio));
    s_radio.page = parent;
    s_radio.active_cat = 0;
    s_radio.active_station = 0;
    s_radio.playing = true;

    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_hor(parent, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_top(parent, 4, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(parent, 2, LV_PART_MAIN);

    /* Header */
    lv_obj_t *header = lv_obj_create(parent);
    lv_obj_set_size(header, UI_SCREEN_WIDTH - 16, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    ui_set_style_bg(header, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(header, 0, LV_PART_MAIN);

    s_radio.title_main = lv_label_create(header);
    lv_label_set_text(s_radio.title_main, "电台");
    ui_set_style_text(s_radio.title_main, UI_COLOR_TEXT, UI_FONT_NORMAL);

    s_radio.count_label = lv_label_create(header);
    lv_label_set_text(s_radio.count_label, "8 个频道在线");
    ui_set_style_text(s_radio.count_label, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);

    /* Category row */
    s_radio.category_row = lv_obj_create(parent);
    lv_obj_set_size(s_radio.category_row, UI_SCREEN_WIDTH - 16, 24);
    lv_obj_set_flex_flow(s_radio.category_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(s_radio.category_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(s_radio.category_row, 6, LV_PART_MAIN);
    ui_set_style_bg(s_radio.category_row, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(s_radio.category_row, 0, LV_PART_MAIN);

    ui_radio_set_categories(s_default_cats, sizeof(s_default_cats) / sizeof(s_default_cats[0]));

    /* Now playing card */
    s_radio.now_playing = lv_obj_create(parent);
    lv_obj_set_size(s_radio.now_playing, UI_SCREEN_WIDTH - 16, 56);
    ui_set_style_bg(s_radio.now_playing, UI_COLOR_PRIMARY, LV_OPA_14);
    ui_set_style_border(s_radio.now_playing, UI_COLOR_PRIMARY, 1, 10);
    lv_obj_set_style_border_opa(s_radio.now_playing, LV_OPA_20, LV_PART_MAIN);

    lv_obj_t *np_cover = lv_obj_create(s_radio.now_playing);
    lv_obj_set_size(np_cover, 42, 42);
    lv_obj_align(np_cover, LV_ALIGN_LEFT_MID, 0, 0);
    ui_set_style_bg(np_cover, UI_COLOR_ACCENT_PURPLE, LV_OPA_COVER);
    lv_obj_set_style_radius(np_cover, 6, LV_PART_MAIN);
    lv_obj_set_style_border_width(np_cover, 0, LV_PART_MAIN);
    lv_obj_t *np_icon = lv_label_create(np_cover);
    lv_label_set_text(np_icon, "\xF0\x9F\x93\xBB");
    ui_set_style_text(np_icon, UI_COLOR_TEXT, UI_FONT_NORMAL);
    lv_obj_center(np_icon);

    lv_obj_t *np_info = lv_obj_create(s_radio.now_playing);
    lv_obj_set_size(np_info, UI_SCREEN_WIDTH - 16 - 42 - 42 - 16, 50);
    lv_obj_align(np_info, LV_ALIGN_LEFT_MID, 50, 0);
    lv_obj_set_flex_flow(np_info, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(np_info, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_gap(np_info, 1, LV_PART_MAIN);
    ui_set_style_bg(np_info, UI_COLOR_PRIMARY, LV_OPA_0);
    lv_obj_set_style_border_width(np_info, 0, LV_PART_MAIN);

    s_radio.np_station = lv_label_create(np_info);
    lv_label_set_text(s_radio.np_station, "华语流行 FM");
    ui_set_style_text(s_radio.np_station, UI_COLOR_TEXT, UI_FONT_NORMAL);
    lv_label_set_long_mode(s_radio.np_station, LV_LABEL_LONG_DOT);
    lv_obj_set_width(s_radio.np_station, UI_SCREEN_WIDTH - 120);

    s_radio.np_program = lv_label_create(np_info);
    lv_label_set_text(s_radio.np_program, "正在播放：晴天 - 周杰伦");
    ui_set_style_text(s_radio.np_program, UI_COLOR_TEXT_SECONDARY, UI_FONT_SMALL);
    lv_label_set_long_mode(s_radio.np_program, LV_LABEL_LONG_DOT);
    lv_obj_set_width(s_radio.np_program, UI_SCREEN_WIDTH - 120);

    lv_obj_t *np_meta = lv_obj_create(np_info);
    lv_obj_set_size(np_meta, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(np_meta, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(np_meta, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(np_meta, 4, LV_PART_MAIN);
    ui_set_style_bg(np_meta, UI_COLOR_PRIMARY, LV_OPA_0);
    lv_obj_set_style_border_width(np_meta, 0, LV_PART_MAIN);

    lv_obj_t *live_dot = lv_obj_create(np_meta);
    lv_obj_set_size(live_dot, 6, 6);
    ui_set_style_bg(live_dot, UI_COLOR_LIVE_RED, LV_OPA_COVER);
    lv_obj_set_style_radius(live_dot, 3, LV_PART_MAIN);
    lv_obj_set_style_border_width(live_dot, 0, LV_PART_MAIN);

    lv_obj_t *meta_label = lv_label_create(np_meta);
    lv_label_set_text(meta_label, "直播中 · 128kbps");
    ui_set_style_text(meta_label, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);

    s_radio.np_btn = ui_create_round_btn(s_radio.now_playing, 32, UI_COLOR_PRIMARY, "\xE2\x8F\xB8", UI_FONT_SMALL);
    lv_obj_align(s_radio.np_btn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_event_cb(s_radio.np_btn, np_btn_click_cb, LV_EVENT_CLICKED, NULL);

    /* Station list */
    s_radio.station_list = lv_obj_create(parent);
    lv_obj_set_size(s_radio.station_list, UI_SCREEN_WIDTH - 16, UI_CONTENT_HEIGHT - 24 - 24 - 56 - 12);
    ui_set_style_bg(s_radio.station_list, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(s_radio.station_list, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(s_radio.station_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(s_radio.station_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_gap(s_radio.station_list, 0, LV_PART_MAIN);

    ui_radio_set_stations(s_default_stations, sizeof(s_default_stations) / sizeof(s_default_stations[0]));
    ui_radio_set_active_station(0);
}

void ui_radio_set_categories(const char **cats, uint8_t count)
{
    if (count > 8) {
        count = 8;
    }
    lv_obj_clean(s_radio.category_row);
    s_radio.cat_count = 0;
    for (int i = 0; i < count; i++) {
        lv_obj_t *chip = lv_btn_create(s_radio.category_row);
        lv_obj_set_size(chip, LV_SIZE_CONTENT, 24);
        lv_obj_set_style_pad_hor(chip, 10, LV_PART_MAIN);
        lv_obj_set_style_radius(chip, 12, LV_PART_MAIN);
        ui_set_style_bg(chip, UI_COLOR_SURFACE_LIGHT, LV_OPA_COVER);
        ui_set_style_border(chip, UI_COLOR_BORDER, 1, 12);
        lv_obj_set_user_data(chip, (void *)(intptr_t)i);
        lv_obj_add_event_cb(chip, cat_click_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_t *label = lv_label_create(chip);
        lv_label_set_text(label, cats[i]);
        ui_set_style_text(label, UI_COLOR_TEXT_SECONDARY, UI_FONT_SMALL);
        lv_obj_center(label);

        s_radio.cat_chips[i] = chip;
        s_radio.cat_count++;
    }
    s_radio.active_cat = 0;
    if (s_radio.cat_count > 0) {
        ui_set_style_bg(s_radio.cat_chips[0], UI_COLOR_PRIMARY, LV_OPA_10);
        ui_set_style_border(s_radio.cat_chips[0], UI_COLOR_PRIMARY, 1, 12);
        lv_obj_set_style_border_opa(s_radio.cat_chips[0], LV_OPA_40, LV_PART_MAIN);
        ui_set_style_text(lv_obj_get_child(s_radio.cat_chips[0], 0), UI_COLOR_PRIMARY, UI_FONT_SMALL);
    }
}

void ui_radio_set_stations(const ui_station_info_t *stations, uint8_t count)
{
    if (count > UI_RADIO_MAX_STATIONS) {
        count = UI_RADIO_MAX_STATIONS;
    }
    lv_obj_clean(s_radio.station_list);
    memset(s_radio.station_items, 0, sizeof(s_radio.station_items));
    s_radio.station_count = 0;

    for (int i = 0; i < count; i++) {
        lv_obj_t *item = lv_btn_create(s_radio.station_list);
        lv_obj_set_size(item, UI_SCREEN_WIDTH - 16, 36);
        lv_obj_set_style_radius(item, 0, LV_PART_MAIN);
        ui_set_style_bg(item, UI_COLOR_BG, LV_OPA_COVER);
        lv_obj_set_style_border_width(item, 0, LV_PART_MAIN);
        lv_obj_set_style_border_side(item, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN);
        lv_obj_set_style_border_color(item, UI_COLOR_TEXT, LV_PART_MAIN);
        lv_obj_set_style_border_opa(item, LV_OPA_5, LV_PART_MAIN);
        lv_obj_set_user_data(item, (void *)(intptr_t)i);
        lv_obj_add_event_cb(item, station_click_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_t *icon = lv_obj_create(item);
        lv_obj_set_size(icon, 34, 34);
        lv_obj_align(icon, LV_ALIGN_LEFT_MID, 0, 0);
        ui_set_style_bg(icon, lv_color_hex(stations[i].icon_color), LV_OPA_COVER);
        lv_obj_set_style_radius(icon, 6, LV_PART_MAIN);
        lv_obj_set_style_border_width(icon, 0, LV_PART_MAIN);
        lv_obj_t *icon_label = lv_label_create(icon);
        lv_label_set_text(icon_label, stations[i].icon);
        ui_set_style_text(icon_label, UI_COLOR_TEXT, UI_FONT_NORMAL);
        lv_obj_center(icon_label);

        lv_obj_t *info = lv_obj_create(item);
        lv_obj_set_size(info, UI_SCREEN_WIDTH - 16 - 34 - 50, 34);
        lv_obj_align(info, LV_ALIGN_LEFT_MID, 40, 0);
        lv_obj_set_flex_flow(info, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(info, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        ui_set_style_bg(info, UI_COLOR_BG, LV_OPA_COVER);
        lv_obj_set_style_border_width(info, 0, LV_PART_MAIN);

        lv_obj_t *name = lv_label_create(info);
        lv_label_set_text(name, stations[i].name);
        ui_set_style_text(name, UI_COLOR_TEXT, UI_FONT_NORMAL);
        lv_label_set_long_mode(name, LV_LABEL_LONG_DOT);
        lv_obj_set_width(name, UI_SCREEN_WIDTH - 110);

        lv_obj_t *desc = lv_label_create(info);
        lv_label_set_text(desc, stations[i].desc);
        ui_set_style_text(desc, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);
        lv_label_set_long_mode(desc, LV_LABEL_LONG_DOT);
        lv_obj_set_width(desc, UI_SCREEN_WIDTH - 110);

        lv_obj_t *freq = lv_label_create(item);
        lv_obj_align(freq, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_label_set_text(freq, stations[i].freq);
        ui_set_style_text(freq, UI_COLOR_PRIMARY, UI_FONT_SMALL);
        lv_obj_set_style_pad_hor(freq, 5, LV_PART_MAIN);
        lv_obj_set_style_pad_ver(freq, 1, LV_PART_MAIN);
        ui_set_style_bg(freq, UI_COLOR_PRIMARY, LV_OPA_10);
        lv_obj_set_style_radius(freq, 6, LV_PART_MAIN);

        s_radio.station_items[i] = item;
        s_radio.station_count++;
    }
}

void ui_radio_set_active_station(uint8_t index)
{
    if (index >= s_radio.station_count) {
        return;
    }
    s_radio.active_station = index;
    for (int i = 0; i < s_radio.station_count; i++) {
        if (i == index) {
            ui_set_style_bg(s_radio.station_items[i], UI_COLOR_PRIMARY, LV_OPA_6);
        } else {
            ui_set_style_bg(s_radio.station_items[i], UI_COLOR_BG, LV_OPA_COVER);
        }
    }
}

void ui_radio_set_playing(bool playing)
{
    s_radio.playing = playing;
    lv_obj_t *label = lv_obj_get_child(s_radio.np_btn, 0);
    if (label) {
        lv_label_set_text(label, playing ? "\xE2\x8F\xB8" : "\xE2\x96\xB6");
    }
}

void ui_radio_set_now_playing(const char *station, const char *program)
{
    lv_label_set_text(s_radio.np_station, station ? station : "");
    lv_label_set_text_fmt(s_radio.np_program, "正在播放：%s", program ? program : "");
}
