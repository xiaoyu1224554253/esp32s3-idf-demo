#include "ui_player.h"
#include "ui_manager.h"
#include "player_engine.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "ui_player";

static ui_player_t s_player;

static const char *s_lyrics[] = {
    "一群嗜血的蚂蚁 被腐肉所吸引",
    "我面无表情 看孤独的风景",
    "失去你 爱恨开始分明",
    "失去你 还有什么事好关心",
    "当鸽子不再象征和平",
    "我终于被提醒 广场上喂食的是秃鹰",
    "我用漂亮的押韵 形容被掠夺一空的爱情",
};
static uint8_t s_lyric_index = 2;

static void format_time(uint32_t ms, char *buf, size_t len)
{
    uint32_t total_sec = ms / 1000;
    uint32_t min = total_sec / 60;
    uint32_t sec = total_sec % 60;
    snprintf(buf, len, "%02lu:%02lu", min, sec);
}

static void update_progress_visual(uint32_t current_ms, uint32_t total_ms)
{
    int pct = 0;
    if (total_ms > 0) {
        pct = (int)((uint64_t)current_ms * 100 / total_ms);
        if (pct > 100) {
            pct = 100;
        }
    }
    lv_coord_t bar_w = lv_obj_get_width(s_player.progress_bar);
    lv_coord_t fill_w = (bar_w * pct) / 100;
    lv_obj_set_width(s_player.progress_fill, fill_w);
    lv_obj_align_to(s_player.progress_dot, s_player.progress_bar, LV_ALIGN_LEFT_MID, fill_w, 0);
}

static void progress_click_cb(lv_event_t *e)
{
    lv_indev_t *indev = lv_indev_get_act();
    lv_point_t vect;
    lv_indev_get_vect(indev, &vect);
    lv_area_t coords;
    lv_obj_get_coords(s_player.progress_bar, &coords);
    lv_point_t press;
    lv_indev_get_point(indev, &press);
    int x = press.x - coords.x1;
    lv_coord_t bar_w = lv_obj_get_width(s_player.progress_bar);
    if (x < 0) {
        x = 0;
    } else if (x > bar_w) {
        x = bar_w;
    }
    uint32_t total_ms = player_engine_get_duration_ms();
    uint32_t seek_ms = (uint64_t)x * total_ms / bar_w;
    player_engine_seek(seek_ms);
}

static void btn_play_cb(lv_event_t *e)
{
    player_engine_toggle_play();
}

static void btn_prev_cb(lv_event_t *e)
{
    player_engine_prev();
}

static void btn_next_cb(lv_event_t *e)
{
    player_engine_next();
}

ui_player_t *ui_player_get(void)
{
    return &s_player;
}

void ui_player_create(lv_obj_t *parent)
{
    memset(&s_player, 0, sizeof(s_player));
    s_player.page = parent;
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_hor(parent, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_top(parent, 4, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(parent, 2, LV_PART_MAIN);

    /* Top row: cover + song info */
    lv_obj_t *top = lv_obj_create(parent);
    lv_obj_set_size(top, UI_SCREEN_WIDTH - 16, 76);
    lv_obj_set_flex_flow(top, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(top, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_gap(top, 8, LV_PART_MAIN);
    ui_set_style_bg(top, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(top, 0, LV_PART_MAIN);

    s_player.cover = lv_obj_create(top);
    lv_obj_set_size(s_player.cover, 70, 70);
    ui_set_style_bg(s_player.cover, UI_COLOR_PRIMARY, LV_OPA_COVER);
    lv_obj_set_style_radius(s_player.cover, 8, LV_PART_MAIN);
    lv_obj_set_style_border_width(s_player.cover, 0, LV_PART_MAIN);

    lv_obj_t *cover_icon = lv_label_create(s_player.cover);
    lv_label_set_text(cover_icon, "\xF0\x9F\x8E\xB5");
    ui_set_style_text(cover_icon, UI_COLOR_TEXT, UI_FONT_NORMAL);
    lv_obj_center(cover_icon);

    lv_obj_t *info = lv_obj_create(top);
    lv_obj_set_size(info, UI_SCREEN_WIDTH - 16 - 70 - 8, 70);
    lv_obj_set_flex_flow(info, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(info, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_gap(info, 1, LV_PART_MAIN);
    ui_set_style_bg(info, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(info, 0, LV_PART_MAIN);

    s_player.song_name = lv_label_create(info);
    lv_label_set_text(s_player.song_name, "夜曲");
    ui_set_style_text(s_player.song_name, UI_COLOR_TEXT, UI_FONT_NORMAL);
    lv_obj_set_style_text_font(s_player.song_name, UI_FONT_NORMAL, LV_PART_MAIN);
    lv_label_set_long_mode(s_player.song_name, LV_LABEL_LONG_DOT);
    lv_obj_set_width(s_player.song_name, UI_SCREEN_WIDTH - 16 - 70 - 8);

    s_player.artist_name = lv_label_create(info);
    lv_label_set_text(s_player.artist_name, "周杰伦");
    ui_set_style_text(s_player.artist_name, UI_COLOR_TEXT_SECONDARY, UI_FONT_SMALL);

    s_player.album_name = lv_label_create(info);
    lv_label_set_text(s_player.album_name, "十一月的萧邦");
    ui_set_style_text(s_player.album_name, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);

    s_player.source_badge = lv_obj_create(info);
    lv_obj_set_size(s_player.source_badge, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    ui_set_style_bg(s_player.source_badge, UI_COLOR_PRIMARY, LV_OPA_10);
    ui_set_style_border(s_player.source_badge, UI_COLOR_PRIMARY, 0, 10);
    lv_obj_set_style_pad_hor(s_player.source_badge, 6, LV_PART_MAIN);
    lv_obj_set_style_pad_ver(s_player.source_badge, 2, LV_PART_MAIN);
    lv_obj_t *badge_label = lv_label_create(s_player.source_badge);
    lv_label_set_text(badge_label, "\xF0\x9F\x8C\x90 网络搜索");
    ui_set_style_text(badge_label, UI_COLOR_PRIMARY, UI_FONT_SMALL);

    /* Lyrics box */
    lv_obj_t *lyrics_box = lv_obj_create(parent);
    lv_obj_set_size(lyrics_box, UI_SCREEN_WIDTH - 16, 40);
    ui_set_style_bg(lyrics_box, UI_COLOR_SURFACE_LIGHT, LV_OPA_COVER);
    ui_set_style_border(lyrics_box, UI_COLOR_BORDER, 1, 8);
    lv_obj_set_style_border_opa(lyrics_box, LV_OPA_10, LV_PART_MAIN);

    s_player.lyric_line = lv_label_create(lyrics_box);
    lv_label_set_text(s_player.lyric_line, "失去你 爱恨开始分明");
    ui_set_style_text(s_player.lyric_line, UI_COLOR_TEXT, UI_FONT_NORMAL);
    lv_obj_center(s_player.lyric_line);
    lv_label_set_long_mode(s_player.lyric_line, LV_LABEL_LONG_DOT);
    lv_obj_set_width(s_player.lyric_line, UI_SCREEN_WIDTH - 36);

    /* Progress area */
    lv_obj_t *progress_area = lv_obj_create(parent);
    lv_obj_set_size(progress_area, UI_SCREEN_WIDTH - 16, 30);
    ui_set_style_bg(progress_area, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(progress_area, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(progress_area, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(progress_area, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_gap(progress_area, 2, LV_PART_MAIN);

    lv_obj_t *time_row = lv_obj_create(progress_area);
    lv_obj_set_size(time_row, UI_SCREEN_WIDTH - 16, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(time_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(time_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    ui_set_style_bg(time_row, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(time_row, 0, LV_PART_MAIN);

    s_player.time_start = lv_label_create(time_row);
    lv_label_set_text(s_player.time_start, "01:24");
    ui_set_style_text(s_player.time_start, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);

    s_player.time_end = lv_label_create(time_row);
    lv_label_set_text(s_player.time_end, "03:46");
    ui_set_style_text(s_player.time_end, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);

    s_player.progress_bar = lv_obj_create(progress_area);
    lv_obj_set_size(s_player.progress_bar, UI_SCREEN_WIDTH - 16, 5);
    ui_set_style_bg(s_player.progress_bar, UI_COLOR_BORDER, LV_OPA_COVER);
    ui_set_style_border(s_player.progress_bar, UI_COLOR_BORDER, 0, 3);
    lv_obj_add_event_cb(s_player.progress_bar, progress_click_cb, LV_EVENT_PRESSED, NULL);

    s_player.progress_fill = lv_obj_create(s_player.progress_bar);
    lv_obj_set_size(s_player.progress_fill, (UI_SCREEN_WIDTH - 16) * 35 / 100, 5);
    ui_set_style_bg(s_player.progress_fill, UI_COLOR_PRIMARY, LV_OPA_COVER);
    ui_set_style_border(s_player.progress_fill, UI_COLOR_PRIMARY, 0, 3);
    lv_obj_align(s_player.progress_fill, LV_ALIGN_LEFT_MID, 0, 0);

    s_player.progress_dot = lv_obj_create(s_player.progress_bar);
    lv_obj_set_size(s_player.progress_dot, 11, 11);
    ui_set_style_bg(s_player.progress_dot, UI_COLOR_TEXT, LV_OPA_COVER);
    lv_obj_set_style_radius(s_player.progress_dot, 6, LV_PART_MAIN);
    lv_obj_set_style_border_width(s_player.progress_dot, 0, LV_PART_MAIN);
    lv_obj_align_to(s_player.progress_dot, s_player.progress_bar, LV_ALIGN_LEFT_MID, (UI_SCREEN_WIDTH - 16) * 35 / 100, 0);

    /* Controls */
    lv_obj_t *controls = lv_obj_create(parent);
    lv_obj_set_size(controls, UI_SCREEN_WIDTH - 16, 38);
    lv_obj_set_flex_flow(controls, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(controls, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(controls, 16, LV_PART_MAIN);
    ui_set_style_bg(controls, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(controls, 0, LV_PART_MAIN);

    s_player.btn_prev = ui_create_round_btn(controls, 34, UI_COLOR_SURFACE_LIGHT, "\xE2\x8F\xAE", UI_FONT_NORMAL);
    s_player.btn_play = ui_create_round_btn(controls, 38, UI_COLOR_PRIMARY, "\xE2\x96\xB6", UI_FONT_NORMAL);
    s_player.btn_next = ui_create_round_btn(controls, 34, UI_COLOR_SURFACE_LIGHT, "\xE2\x8F\xAD", UI_FONT_NORMAL);

    lv_obj_add_event_cb(s_player.btn_play, btn_play_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_player.btn_prev, btn_prev_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_player.btn_next, btn_next_cb, LV_EVENT_CLICKED, NULL);

    /* Make buttons easier to touch */
    lv_obj_set_style_pad_all(s_player.btn_prev, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(s_player.btn_play, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(s_player.btn_next, 0, LV_PART_MAIN);
}

void ui_player_set_song(const char *name, const char *artist, const char *album, const char *source)
{
    lv_label_set_text(s_player.song_name, name ? name : "");
    lv_label_set_text(s_player.artist_name, artist ? artist : "");
    lv_label_set_text(s_player.album_name, album ? album : "");
    if (s_player.source_badge) {
        lv_obj_t *label = lv_obj_get_child(s_player.source_badge, 0);
        if (label) {
            lv_label_set_text_fmt(label, "\xF0\x9F\x8C\x90 %s", source ? source : "网络搜索");
        }
    }
}

void ui_player_set_lyric(const char *lyric)
{
    lv_label_set_text(s_player.lyric_line, lyric ? lyric : "");
}

void ui_player_set_progress(uint32_t current_ms, uint32_t total_ms)
{
    char buf[16];
    format_time(current_ms, buf, sizeof(buf));
    lv_label_set_text(s_player.time_start, buf);
    format_time(total_ms, buf, sizeof(buf));
    lv_label_set_text(s_player.time_end, buf);
    update_progress_visual(current_ms, total_ms);
}

void ui_player_set_playing(bool playing)
{
    lv_obj_t *label = lv_obj_get_child(s_player.btn_play, 0);
    if (label) {
        lv_label_set_text(label, playing ? "\xE2\x8F\xB8" : "\xE2\x96\xB6");
    }
}

void ui_player_next_lyric(void)
{
    s_lyric_index = (s_lyric_index + 1) % (sizeof(s_lyrics) / sizeof(s_lyrics[0]));
    ui_player_set_lyric(s_lyrics[s_lyric_index]);
}
