#include "music_player_ui.h"
#include "music_player.h"

static void track_click_cb(lv_event_t *e)
{
    uint32_t index = (uint32_t)(uintptr_t)lv_event_get_user_data(e);
    music_player_play(index);
    ui_show_now_playing();
}

static void back_btn_event_cb(lv_event_t *e)
{
    (void)e;
    ui_show_now_playing();
}

void ui_show_playlist(void)
{
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x121212), 0);

    // Title
    lv_obj_t *title = lv_label_create(scr);
    lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_label_set_text(title, "Playlist");

    // Back button
    lv_obj_t *back_btn = lv_btn_create(scr);
    lv_obj_set_size(back_btn, 60, 30);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_event_cb(back_btn, back_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);

    // List
    lv_obj_t *list = lv_list_create(scr);
    lv_obj_set_size(list, 300, 180);
    lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, -10);

    uint32_t count = music_player_get_track_count();
    for (uint32_t i = 0; i < count; i++) {
        const music_player_track_t *track = music_player_get_track(i);
        if (track == NULL) continue;

        lv_obj_t *btn = lv_list_add_btn(list, NULL, track->title);
        lv_obj_add_event_cb(btn, track_click_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
    }
}
