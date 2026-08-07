#ifndef UI_PLAYLIST_H
#define UI_PLAYLIST_H

#include "lvgl.h"
#include "ui_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UI_PLAYLIST_MAX_SONGS   32

typedef struct {
    const char *title;
    const char *artist;
    const char *album;
    const char *time;
} ui_song_info_t;

typedef struct {
    lv_obj_t *page;
    lv_obj_t *title_main;
    lv_obj_t *count_label;
    lv_obj_t *mode_btns[UI_PLAY_MODE_COUNT];
    lv_obj_t *list;
    lv_obj_t *song_items[UI_PLAYLIST_MAX_SONGS];
    lv_obj_t *song_nums[UI_PLAYLIST_MAX_SONGS];
    uint8_t song_count;
    uint8_t active_index;
    ui_play_mode_t play_mode;
} ui_playlist_t;

ui_playlist_t *ui_playlist_get(void);
void ui_playlist_create(lv_obj_t *parent);
void ui_playlist_set_count(uint8_t count);
void ui_playlist_set_songs(const ui_song_info_t *songs, uint8_t count);
void ui_playlist_set_active(uint8_t index);
void ui_playlist_set_mode(ui_play_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* UI_PLAYLIST_H */
