#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t audio_engine_init(void);
esp_err_t audio_engine_play_file(const char *path);
esp_err_t audio_engine_stop(void);
esp_err_t audio_engine_pause(void);
esp_err_t audio_engine_resume(void);
esp_err_t audio_engine_set_volume(uint8_t volume);
bool audio_engine_is_playing(void);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_ENGINE_H
