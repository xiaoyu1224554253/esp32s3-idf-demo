#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MUSIC_PLAYER_MAX_TRACKS 256
#define MUSIC_PLAYER_MAX_PATH_LEN 256
#define MUSIC_PLAYER_MAX_TITLE_LEN 128
#define MUSIC_PLAYER_MAX_ARTIST_LEN 128

typedef enum {
    PLAY_MODE_SEQUENTIAL,
    PLAY_MODE_RANDOM,
    PLAY_MODE_REPEAT_ONE,
    PLAY_MODE_REPEAT_ALL,
} music_player_play_mode_t;

typedef struct {
    char path[MUSIC_PLAYER_MAX_PATH_LEN];
    char title[MUSIC_PLAYER_MAX_TITLE_LEN];
    char artist[MUSIC_PLAYER_MAX_ARTIST_LEN];
    uint32_t duration_ms;
} music_player_track_t;

typedef struct {
    music_player_track_t tracks[MUSIC_PLAYER_MAX_TRACKS];
    uint32_t track_count;
    int32_t current_index;
    bool is_playing;
    uint8_t volume;
    music_player_play_mode_t play_mode;
    uint32_t progress_ms;
    uint32_t duration_ms;
} music_player_state_t;

void music_player_init(void);

// Playlist management
esp_err_t music_player_scan_sdcard(void);
uint32_t music_player_get_track_count(void);
const music_player_track_t *music_player_get_track(uint32_t index);
const music_player_state_t *music_player_get_state(void);

// Playback control
void music_player_play(uint32_t index);
void music_player_pause(void);
void music_player_resume(void);
void music_player_toggle(void);
void music_player_next(void);
void music_player_prev(void);
void music_player_stop(void);

// Volume and mode
void music_player_set_volume(uint8_t volume);
uint8_t music_player_get_volume(void);
void music_player_set_play_mode(music_player_play_mode_t mode);
music_player_play_mode_t music_player_get_play_mode(void);

// Progress
void music_player_set_progress(uint32_t progress_ms);
uint32_t music_player_get_progress_ms(void);

// Status
bool music_player_is_playing(void);

#ifdef __cplusplus
}
#endif
