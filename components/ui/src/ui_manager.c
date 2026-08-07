#include "ui_manager.h"
#include "ui_player.h"
#include "ui_playlist.h"
#include "ui_radio.h"
#include "ui_search.h"
#include "esp_log.h"

static const char *TAG = "ui_manager";

static ui_manager_t s_ui;

static const char *s_nav_icons[UI_PAGE_COUNT] = {"\xF0\x9F\x8E\xB5", "\xF0\x9F\x93\x91", "\xF0\x9F\x93\xBB", "\xF0\x9F\x94\x8D"};
static const char *s_nav_labels[UI_PAGE_COUNT] = {"播放", "歌单", "电台", "搜索"};

static void nav_click_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    ui_page_t page = (ui_page_t)(uintptr_t)lv_obj_get_user_data(btn);
    ui_manager_switch_page(page);
}

static void create_status_bar(void)
{
    s_ui.status_bar = lv_obj_create(s_ui.screen);
    lv_obj_set_size(s_ui.status_bar, UI_SCREEN_WIDTH, UI_STATUS_BAR_HEIGHT);
    lv_obj_set_pos(s_ui.status_bar, 0, 0);
    ui_set_style_bg(s_ui.status_bar, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(s_ui.status_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(s_ui.status_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(s_ui.status_bar, 0, LV_PART_MAIN);

    s_ui.status_time = lv_label_create(s_ui.status_bar);
    lv_label_set_text(s_ui.status_time, "12:08");
    ui_set_style_text(s_ui.status_time, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);
    lv_obj_align(s_ui.status_time, LV_ALIGN_LEFT_MID, 8, 0);

    lv_obj_t *right = lv_obj_create(s_ui.status_bar);
    lv_obj_set_size(right, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(right, LV_ALIGN_RIGHT_MID, -8, 0);
    lv_obj_set_flex_flow(right, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(right, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(right, 6, LV_PART_MAIN);
    ui_set_style_bg(right, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(right, 0, LV_PART_MAIN);

    s_ui.status_wifi = lv_label_create(right);
    lv_label_set_text(s_ui.status_wifi, "WiFi");
    ui_set_style_text(s_ui.status_wifi, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);

    s_ui.status_bat = lv_label_create(right);
    lv_label_set_text(s_ui.status_bat, "85%");
    ui_set_style_text(s_ui.status_bat, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);
}

static void create_nav_bar(void)
{
    s_ui.nav_bar = lv_obj_create(s_ui.screen);
    lv_obj_set_size(s_ui.nav_bar, UI_SCREEN_WIDTH, UI_NAV_BAR_HEIGHT);
    lv_obj_set_pos(s_ui.nav_bar, 0, UI_SCREEN_HEIGHT - UI_NAV_BAR_HEIGHT);
    ui_set_style_bg(s_ui.nav_bar, UI_COLOR_SURFACE, LV_OPA_COVER);
    ui_set_style_border(s_ui.nav_bar, UI_COLOR_BORDER, 1, 0);
    lv_obj_set_style_border_side(s_ui.nav_bar, LV_BORDER_SIDE_TOP, LV_PART_MAIN);
    lv_obj_set_style_pad_all(s_ui.nav_bar, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(s_ui.nav_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(s_ui.nav_bar, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    for (int i = 0; i < UI_PAGE_COUNT; i++) {
        lv_obj_t *item = lv_btn_create(s_ui.nav_bar);
        lv_obj_set_size(item, UI_SCREEN_WIDTH / UI_PAGE_COUNT, UI_NAV_BAR_HEIGHT);
        lv_obj_set_style_radius(item, 0, LV_PART_MAIN);
        lv_obj_set_style_border_width(item, 0, LV_PART_MAIN);
        ui_set_style_bg(item, UI_COLOR_SURFACE, LV_OPA_COVER);
        lv_obj_set_user_data(item, (void *)(uintptr_t)i);
        lv_obj_add_event_cb(item, nav_click_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_t *cont = lv_obj_create(item);
        lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_center(cont);
        lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_gap(cont, 3, LV_PART_MAIN);
        ui_set_style_bg(cont, UI_COLOR_SURFACE, LV_OPA_COVER);
        lv_obj_set_style_border_width(cont, 0, LV_PART_MAIN);

        lv_obj_t *icon = lv_label_create(cont);
        lv_label_set_text(icon, s_nav_icons[i]);
        ui_set_style_text(icon, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);

        lv_obj_t *label = lv_label_create(cont);
        lv_label_set_text(label, s_nav_labels[i]);
        ui_set_style_text(label, UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);

        s_ui.nav_items[i] = item;
    }
}

static void create_content_area(void)
{
    s_ui.content = lv_obj_create(s_ui.screen);
    lv_obj_set_size(s_ui.content, UI_SCREEN_WIDTH, UI_CONTENT_HEIGHT);
    lv_obj_set_pos(s_ui.content, 0, UI_CONTENT_Y);
    ui_set_style_bg(s_ui.content, UI_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_border_width(s_ui.content, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(s_ui.content, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(s_ui.content, 0, LV_PART_MAIN);

    for (int i = 0; i < UI_PAGE_COUNT; i++) {
        s_ui.pages[i] = lv_obj_create(s_ui.content);
        lv_obj_set_size(s_ui.pages[i], UI_SCREEN_WIDTH, UI_CONTENT_HEIGHT);
        lv_obj_set_pos(s_ui.pages[i], 0, 0);
        ui_set_style_bg(s_ui.pages[i], UI_COLOR_BG, LV_OPA_COVER);
        lv_obj_set_style_border_width(s_ui.pages[i], 0, LV_PART_MAIN);
        lv_obj_set_style_radius(s_ui.pages[i], 0, LV_PART_MAIN);
        lv_obj_set_style_pad_all(s_ui.pages[i], 0, LV_PART_MAIN);
        lv_obj_add_flag(s_ui.pages[i], LV_OBJ_FLAG_HIDDEN);
    }
}

ui_manager_t *ui_manager_get(void)
{
    return &s_ui;
}

lv_obj_t *ui_manager_get_page(ui_page_t page)
{
    if (page < 0 || page >= UI_PAGE_COUNT) {
        return NULL;
    }
    return s_ui.pages[page];
}

ui_page_t ui_manager_get_current_page(void)
{
    return s_ui.current_page;
}

void ui_manager_switch_page(ui_page_t page)
{
    if (page < 0 || page >= UI_PAGE_COUNT || page == s_ui.current_page) {
        return;
    }

    lv_obj_add_flag(s_ui.pages[s_ui.current_page], LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(s_ui.pages[page], LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *prev = s_ui.nav_items[s_ui.current_page];
    lv_obj_t *next = s_ui.nav_items[page];

    ui_set_style_bg(prev, UI_COLOR_SURFACE, LV_OPA_COVER);
    lv_obj_t *prev_cont = lv_obj_get_child(prev, 0);
    for (int i = 0; i < lv_obj_get_child_cnt(prev_cont); i++) {
        ui_set_style_text(lv_obj_get_child(prev_cont, i), UI_COLOR_TEXT_MUTED, UI_FONT_SMALL);
    }

    ui_set_style_bg(next, UI_COLOR_PRIMARY, LV_OPA_10);
    lv_obj_t *next_cont = lv_obj_get_child(next, 0);
    for (int i = 0; i < lv_obj_get_child_cnt(next_cont); i++) {
        ui_set_style_text(lv_obj_get_child(next_cont, i), UI_COLOR_PRIMARY, UI_FONT_SMALL);
    }

    s_ui.current_page = page;
}

void ui_manager_init(void)
{
    s_ui.screen = lv_scr_act();
    ui_set_style_bg(s_ui.screen, UI_COLOR_BG, LV_OPA_COVER);

    create_status_bar();
    create_content_area();
    create_nav_bar();

    ui_player_create(s_ui.pages[UI_PAGE_PLAYER]);
    ui_playlist_create(s_ui.pages[UI_PAGE_PLAYLIST]);
    ui_radio_create(s_ui.pages[UI_PAGE_RADIO]);
    ui_search_create(s_ui.pages[UI_PAGE_SEARCH]);

    s_ui.current_page = UI_PAGE_PLAYER;
    lv_obj_clear_flag(s_ui.pages[UI_PAGE_PLAYER], LV_OBJ_FLAG_HIDDEN);

    ui_set_style_bg(s_ui.nav_items[UI_PAGE_PLAYER], UI_COLOR_PRIMARY, LV_OPA_10);
    lv_obj_t *cont = lv_obj_get_child(s_ui.nav_items[UI_PAGE_PLAYER], 0);
    for (int i = 0; i < lv_obj_get_child_cnt(cont); i++) {
        ui_set_style_text(lv_obj_get_child(cont, i), UI_COLOR_PRIMARY, UI_FONT_SMALL);
    }
}

void ui_manager_set_status_time(const char *time_str)
{
    if (s_ui.status_time) {
        lv_label_set_text(s_ui.status_time, time_str);
    }
}

void ui_manager_set_wifi_status(bool connected)
{
    if (s_ui.status_wifi) {
        lv_label_set_text(s_ui.status_wifi, connected ? "WiFi" : "");
    }
}

void ui_manager_set_battery(uint8_t percent)
{
    if (s_ui.status_bat) {
        lv_label_set_text_fmt(s_ui.status_bat, "%d%%", percent > 100 ? 100 : percent);
    }
}
