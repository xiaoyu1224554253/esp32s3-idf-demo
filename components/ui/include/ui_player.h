#ifndef UI_PLAYER_H
#define UI_PLAYER_H

#include "lvgl.h"
#include "ui_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_obj_t *page;
    lv_obj_t *cover;
    lv_obj_t *song_name;
    lv_obj_t *artist_name;
    lv_obj_t *album_name;
    lv_obj_t *source_badge;
    lv_obj_t *lyric_line;
    lv_obj_t *progress_bar;
    lv_obj_t *progress_fill;
    lv_obj_t *progress_dot;
    lv_obj_t *time_start;
    lv_obj_t *time_end;
    lv_obj_t *btn_prev;
    lv_obj_t *btn_play;
    lv_obj_t *btn_next;
} ui_player_t;

ui_player_t *ui_player_get(void);
void ui_player_create(lv_obj_t *parent);
void ui_player_set_song(const char *name, const char *artist, const char *album, const char *source);
void ui_player_set_lyric(const char *lyric);
void ui_player_set_progress(uint32_t current_ms, uint32_t total_ms);
void ui_player_set_playing(bool playing);

#ifdef __cplusplus
}
#endif

#endif /* UI_PLAYER_H */
