#ifndef BSP_AUDIO_H
#define BSP_AUDIO_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_AUDIO_SAMPLE_RATE   44100
#define BSP_AUDIO_BITS          16
#define BSP_AUDIO_CHANNELS      2

esp_err_t bsp_audio_init(void);
esp_err_t bsp_audio_start(void);
esp_err_t bsp_audio_stop(void);
esp_err_t bsp_audio_set_volume(uint8_t volume);
int bsp_audio_write(const int16_t *data, int samples);

#ifdef __cplusplus
}
#endif

#endif /* BSP_AUDIO_H */
