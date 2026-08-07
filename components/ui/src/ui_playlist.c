#include "ui_playlist.h"
#include "ui_manager.h"
#include "ui_player.h"
#include "player_engine.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "ui_playlist";

static ui_playlist_t s_playlist;

static const char *s_mode_labels[UI_PLAY_MODE_COUNT] = {"\xF0\x9F\x94\x84 顺序播放", "\xF0\x9F\x94\x80 随机播放", "\xF0\x9F\x94\x81 单曲循环"};

static const ui_song_info_t s_default_songs[] = {
    {"夜曲", "周杰伦", "十一月的萧邦", "03:46"},
    {"晴天", "周杰伦", "叶惠美", "04:29"},
    {"七里香", "周杰伦", "七里香", "04:59"},
    {"稻香", "周杰伦", "魔杰座", "03:43"},
    {"告白气球", "周杰伦", "周杰伦的床边故事", "03:35"},
    {"青花瓷", "周杰伦", "我很忙", "03:59"},
};

static void song_click_cb(lv_event_t *e)
{
    lv_obj_t *item = lv_event_get_target(e);
    int idx = (int)(intptr_t)lv_obj_get_user_data(item);
    if (idx >= 0 && idx < s_playlist.song_count) {
        player_engine_play_index(idx);
    }
}

static void mode_click_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    int mode = (int)(intptr_t)lv_obj_get_user_data(btn);
    ui_playlist_set_mode((ui_play_mode_t)mode);
    player_engine_set_mode((player_mode_t)mode);
}

static void update_active_visual(void)
{
    for (int i = 0; i < s_playlist.song_count; i++) {
        lv_obj_t *item = s_playlist.song_items[i];
        lv_obj_t *num = s_playlist.song_nums[i];
        if (i == s_playlist.active_index) {
            ui_set_style_text(lv_obj_get_child(item, 1), UI_COLOR_PRIMARY, UI_FONT_NORMAL);
            ui_set_style_text(num, UI_COLOR_PRIMARY, UI_FONT_SMALL);
            lv_label_set_text(num, "\xE2\x96\xB6");
        } else {
            ui_set_style_text(lv_obj_get_child(item, 1), UI_COLOR_TEXT, UI_FONT_NORMAL);
            ui_set_style_text(num, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);
            char buf[4];
            snprintf(buf, sizeof(buf), "%d", i + 1);
            lv_label_set_text(num, buf);
        }
    }
}

ui_playlist_t *ui_playlist_get(void)
{
    return &s_playlist;
}

void ui_playlist_create(lv_obj_t *parent)
{
    memset(&s_playlist, 0, sizeof(s_playlist));
    s_playlist.page = parent;
    s_playlist.active_index = 0;
    s_playlist.play_mode = UI_PLAY_MODE_SEQUENCE;

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

    s_playlist.title_main = lv_label_create(header);
    lv_label_set_text(s_playlist.title_main, "我的歌单");
    ui_set_style_text(s_playlist.title_main, UI_COLOR_TEXT, UI_FONT_NORMAL);

    s_playlist.count_label = lv_label_create(header);
    lv_label_set_text(s_playlist.count_label, "SD 卡 · 0 首");
    ui_set_style_text(s_playlist.count_label, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);

    /* Mode buttons */
    lv_obj_t *actions = lv_obj_create(parent);
    lv_obj_set_size(actions, UI_SCREEN_WIDTH - 16, 24);
    lv_obj_set_flex_flow(actions, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(actions, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(actions, 6, LV_PART_MAIN);
    ui_set_style_bg(actions, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(actions, 0, LV_PART_MAIN);

    for (int i = 0; i < UI_PLAY_MODE_COUNT; i++) {
        s_playlist.mode_btns[i] = lv_btn_create(actions);
        lv_obj_set_size(s_playlist.mode_btns[i], (UI_SCREEN_WIDTH - 16 - 12) / 3, 24);
        lv_obj_set_style_radius(s_playlist.mode_btns[i], 12, LV_PART_MAIN);
        ui_set_style_bg(s_playlist.mode_btns[i], UI_COLOR_SURFACE_LIGHT, LV_OPA_COVER);
        ui_set_style_border(s_playlist.mode_btns[i], UI_COLOR_BORDER, 1, 12);
        lv_obj_set_user_data(s_playlist.mode_btns[i], (void *)(intptr_t)i);
        lv_obj_add_event_cb(s_playlist.mode_btns[i], mode_click_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_t *label = lv_label_create(s_playlist.mode_btns[i]);
        lv_label_set_text(label, s_mode_labels[i]);
        ui_set_style_text(label, UI_COLOR_TEXT_SECONDARY, UI_FONT_SMALL);
        lv_obj_center(label);
    }
    ui_playlist_set_mode(UI_PLAY_MODE_SEQUENCE);

    /* Song list */
    s_playlist.list = lv_obj_create(parent);
    lv_obj_set_size(s_playlist.list, UI_SCREEN_WIDTH - 16, UI_CONTENT_HEIGHT - 24 - 24 - 12);
    ui_set_style_bg(s_playlist.list, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(s_playlist.list, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(s_playlist.list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(s_playlist.list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_gap(s_playlist.list, 0, LV_PART_MAIN);

    ui_playlist_set_songs(s_default_songs, sizeof(s_default_songs) / sizeof(s_default_songs[0]));
    ui_playlist_set_active(0);
}

void ui_playlist_set_count(uint8_t count)
{
    s_playlist.song_count = count;
    lv_label_set_text_fmt(s_playlist.count_label, "SD 卡 · %d 首", count);
}

void ui_playlist_set_songs(const ui_song_info_t *songs, uint8_t count)
{
    if (count > UI_PLAYLIST_MAX_SONGS) {
        count = UI_PLAYLIST_MAX_SONGS;
    }

    /* Clear existing items */
    lv_obj_clean(s_playlist.list);
    memset(s_playlist.song_items, 0, sizeof(s_playlist.song_items));
    memset(s_playlist.song_nums, 0, sizeof(s_playlist.song_nums));
    s_playlist.song_count = 0;

    for (int i = 0; i < count; i++) {
        lv_obj_t *item = lv_btn_create(s_playlist.list);
        lv_obj_set_size(item, UI_SCREEN_WIDTH - 16, 30);
        lv_obj_set_style_radius(item, 0, LV_PART_MAIN);
        ui_set_style_bg(item, UI_COLOR_BG, LV_OPA_COVER);
        lv_obj_set_style_border_width(item, 0, LV_PART_MAIN);
        lv_obj_set_style_border_side(item, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN);
        lv_obj_set_style_border_color(item, UI_COLOR_TEXT, LV_PART_MAIN);
        lv_obj_set_style_border_opa(item, LV_OPA_5, LV_PART_MAIN);
        lv_obj_set_user_data(item, (void *)(intptr_t)i);
        lv_obj_add_event_cb(item, song_click_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_t *num = lv_label_create(item);
        lv_obj_set_size(num, 18, LV_SIZE_CONTENT);
        lv_obj_align(num, LV_ALIGN_LEFT_MID, 0, 0);
        lv_obj_set_style_text_align(num, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        ui_set_style_text(num, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);
        char buf[4];
        snprintf(buf, sizeof(buf), "%d", i + 1);
        lv_label_set_text(num, buf);
        s_playlist.song_nums[i] = num;

        lv_obj_t *info = lv_obj_create(item);
        lv_obj_set_size(info, UI_SCREEN_WIDTH - 16 - 18 - 20, 28);
        lv_obj_align(info, LV_ALIGN_LEFT_MID, 22, 0);
        lv_obj_set_flex_flow(info, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(info, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        ui_set_style_bg(info, UI_COLOR_BG, LV_OPA_COVER);
        lv_obj_set_style_border_width(info, 0, LV_PART_MAIN);

        lv_obj_t *title = lv_label_create(info);
        lv_label_set_text(title, songs[i].title);
        ui_set_style_text(title, UI_COLOR_TEXT, UI_FONT_NORMAL);
        lv_label_set_long_mode(title, LV_LABEL_LONG_DOT);
        lv_obj_set_width(title, UI_SCREEN_WIDTH - 60);

        lv_obj_t *sub = lv_label_create(info);
        lv_label_set_text_fmt(sub, "%s · %s", songs[i].artist, songs[i].album);
        ui_set_style_text(sub, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);
        lv_label_set_long_mode(sub, LV_LABEL_LONG_DOT);
        lv_obj_set_width(sub, UI_SCREEN_WIDTH - 60);

        s_playlist.song_items[i] = item;
        s_playlist.song_count++;
    }
    ui_playlist_set_count(s_playlist.song_count);
    ui_playlist_set_active(0);
}

void ui_playlist_set_active(uint8_t index)
{
    if (index >= s_playlist.song_count) {
        return;
    }
    s_playlist.active_index = index;
    update_active_visual();
}

void ui_playlist_set_mode(ui_play_mode_t mode)
{
    if (mode >= UI_PLAY_MODE_COUNT) {
        return;
    }
    s_playlist.play_mode = mode;
    for (int i = 0; i < UI_PLAY_MODE_COUNT; i++) {
        if (i == mode) {
            ui_set_style_bg(s_playlist.mode_btns[i], UI_COLOR_PRIMARY, LV_OPA_10);
            ui_set_style_border(s_playlist.mode_btns[i], UI_COLOR_PRIMARY, 1, 12);
            lv_obj_set_style_border_opa(s_playlist.mode_btns[i], LV_OPA_40, LV_PART_MAIN);
            ui_set_style_text(lv_obj_get_child(s_playlist.mode_btns[i], 0), UI_COLOR_PRIMARY, UI_FONT_SMALL);
        } else {
            ui_set_style_bg(s_playlist.mode_btns[i], UI_COLOR_SURFACE_LIGHT, LV_OPA_COVER);
            ui_set_style_border(s_playlist.mode_btns[i], UI_COLOR_BORDER, 1, 12);
            ui_set_style_text(lv_obj_get_child(s_playlist.mode_btns[i], 0), UI_COLOR_TEXT_SECONDARY, UI_FONT_SMALL);
        }
    }
}
