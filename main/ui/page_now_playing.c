#include "music_player_ui.h"
#include "music_player.h"

static lv_obj_t *title_label = NULL;
static lv_obj_t *artist_label = NULL;
static lv_obj_t *play_btn = NULL;
static lv_obj_t *play_btn_label = NULL;
static lv_obj_t *progress_bar = NULL;
static lv_obj_t *time_label = NULL;

static void format_time(uint32_t ms, char *buf, size_t len)
{
    uint32_t total_seconds = ms / 1000;
    uint32_t minutes = total_seconds / 60;
    uint32_t seconds = total_seconds % 60;
    snprintf(buf, len, "%02lu:%02lu", minutes, seconds);
}

static void play_btn_event_cb(lv_event_t *e)
{
    (void)e;
    music_player_toggle();
    ui_update_now_playing();
}

static void next_btn_event_cb(lv_event_t *e)
{
    (void)e;
    music_player_next();
    ui_update_now_playing();
}

static void prev_btn_event_cb(lv_event_t *e)
{
    (void)e;
    music_player_prev();
    ui_update_now_playing();
}

static void playlist_btn_event_cb(lv_event_t *e)
{
    (void)e;
    ui_show_playlist();
}

static void radio_btn_event_cb(lv_event_t *e)
{
    (void)e;
    ui_show_radio();
}

void ui_show_now_playing(void)
{
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x121212), 0);

    // Title
    title_label = lv_label_create(scr);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 30);
    lv_label_set_text(title_label, "No track");

    // Artist
    artist_label = lv_label_create(scr);
    lv_obj_set_style_text_color(artist_label, lv_color_hex(0xaaaaaa), 0);
    lv_obj_align(artist_label, LV_ALIGN_TOP_MID, 0, 60);
    lv_label_set_text(artist_label, "Unknown Artist");

    // Progress bar
    progress_bar = lv_slider_create(scr);
    lv_obj_set_size(progress_bar, 260, 10);
    lv_obj_align(progress_bar, LV_ALIGN_CENTER, 0, 20);
    lv_slider_set_range(progress_bar, 0, 100);

    // Time label
    time_label = lv_label_create(scr);
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xaaaaaa), 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 45);
    lv_label_set_text(time_label, "00:00 / 00:00");

    // Control buttons
    lv_obj_t *prev_btn = lv_btn_create(scr);
    lv_obj_set_size(prev_btn, 60, 40);
    lv_obj_align(prev_btn, LV_ALIGN_BOTTOM_MID, -80, -30);
    lv_obj_add_event_cb(prev_btn, prev_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *prev_label = lv_label_create(prev_btn);
    lv_label_set_text(prev_label, "<<");
    lv_obj_center(prev_label);

    play_btn = lv_btn_create(scr);
    lv_obj_set_size(play_btn, 80, 50);
    lv_obj_align(play_btn, LV_ALIGN_BOTTOM_MID, 0, -30);
    lv_obj_add_event_cb(play_btn, play_btn_event_cb, LV_EVENT_CLICKED, NULL);
    play_btn_label = lv_label_create(play_btn);
    lv_label_set_text(play_btn_label, "Play");
    lv_obj_center(play_btn_label);

    lv_obj_t *next_btn = lv_btn_create(scr);
    lv_obj_set_size(next_btn, 60, 40);
    lv_obj_align(next_btn, LV_ALIGN_BOTTOM_MID, 80, -30);
    lv_obj_add_event_cb(next_btn, next_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *next_label = lv_label_create(next_btn);
    lv_label_set_text(next_label, ">>");
    lv_obj_center(next_label);

    // Playlist button
    lv_obj_t *pl_btn = lv_btn_create(scr);
    lv_obj_set_size(pl_btn, 100, 36);
    lv_obj_align(pl_btn, LV_ALIGN_BOTTOM_RIGHT, -10, -80);
    lv_obj_add_event_cb(pl_btn, playlist_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *pl_label = lv_label_create(pl_btn);
    lv_label_set_text(pl_label, "List");
    lv_obj_center(pl_label);

    // Radio button
    lv_obj_t *radio_btn = lv_btn_create(scr);
    lv_obj_set_size(radio_btn, 80, 36);
    lv_obj_align(radio_btn, LV_ALIGN_BOTTOM_LEFT, 10, -80);
    lv_obj_add_event_cb(radio_btn, radio_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *radio_label = lv_label_create(radio_btn);
    lv_label_set_text(radio_label, "Radio");
    lv_obj_center(radio_label);

    ui_update_now_playing();
}

void ui_update_now_playing(void)
{
    const music_player_state_t *state = music_player_get_state();
    const music_player_track_t *track = NULL;

    if (state->current_index >= 0 && (uint32_t)state->current_index < state->track_count) {
        track = &state->tracks[state->current_index];
    }

    if (title_label) {
        lv_label_set_text(title_label, track ? track->title : "No track");
    }
    if (artist_label) {
        lv_label_set_text(artist_label, track ? track->artist : "Unknown Artist");
    }
    if (play_btn_label) {
        lv_label_set_text(play_btn_label, state->is_playing ? "Pause" : "Play");
    }

    if (progress_bar) {
        int value = 0;
        if (state->duration_ms > 0) {
            value = (int)((state->progress_ms * 100) / state->duration_ms);
        }
        lv_slider_set_value(progress_bar, value, LV_ANIM_OFF);
    }

    if (time_label) {
        char buf[32];
        char total[16];
        format_time(state->progress_ms, buf, sizeof(buf));
        format_time(state->duration_ms, total, sizeof(total));
        strncat(buf, " / ", sizeof(buf) - strlen(buf) - 1);
        strncat(buf, total, sizeof(buf) - strlen(buf) - 1);
        lv_label_set_text(time_label, buf);
    }
}

void ui_periodic_update(void)
{
    ui_update_now_playing();
}

void ui_init(void)
{
    // Theme setup can be added here
}
