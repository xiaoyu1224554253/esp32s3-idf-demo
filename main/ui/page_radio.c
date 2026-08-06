#include "music_player_ui.h"
#include "esp_log.h"

static const char *TAG = "ui_radio";

static void back_btn_event_cb(lv_event_t *e)
{
    (void)e;
    ui_show_now_playing();
}

void ui_show_radio(void)
{
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x121212), 0);

    lv_obj_t *title = lv_label_create(scr);
    lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_label_set_text(title, "Radio");

    lv_obj_t *back_btn = lv_btn_create(scr);
    lv_obj_set_size(back_btn, 60, 30);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_event_cb(back_btn, back_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);

    lv_obj_t *info = lv_label_create(scr);
    lv_obj_set_style_text_color(info, lv_color_hex(0xaaaaaa), 0);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(info, "Network radio not implemented");
}
