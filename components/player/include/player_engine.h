#ifndef PLAYER_ENGINE_H
#define PLAYER_ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PLAYER_MODE_SEQUENCE = 0,
    PLAYER_MODE_RANDOM,
    PLAYER_MODE_REPEAT,
} player_mode_t;

typedef struct {
    char title[64];
    char artist[64];
    char album[64];
    char path[128];
    uint32_t duration_ms;
} player_song_t;

esp_err_t player_engine_init(void);
void player_engine_play_index(uint8_t index);
void player_engine_play_pause(void);
void player_engine_toggle_play(void);
void player_engine_next(void);
void player_engine_prev(void);
void player_engine_seek(uint32_t ms);
void player_engine_set_mode(player_mode_t mode);
bool player_engine_is_playing(void);
uint32_t player_engine_get_current_ms(void);
uint32_t player_engine_get_duration_ms(void);
uint8_t player_engine_get_current_index(void);
uint8_t player_engine_get_song_count(void);
const player_song_t *player_engine_get_songs(void);

#ifdef __cplusplus
}
#endif

#endif /* PLAYER_ENGINE_H */
