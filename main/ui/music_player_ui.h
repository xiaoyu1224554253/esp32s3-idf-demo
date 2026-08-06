#ifndef MUSIC_PLAYER_UI_H
#define MUSIC_PLAYER_UI_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void ui_init(void);
void ui_show_now_playing(void);
void ui_show_playlist(void);
void ui_show_radio(void);
void ui_update_now_playing(void);
void ui_periodic_update(void);

#ifdef __cplusplus
}
#endif

#endif // MUSIC_PLAYER_UI_H
