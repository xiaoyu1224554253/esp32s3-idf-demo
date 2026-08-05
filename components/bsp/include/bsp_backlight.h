#ifndef BSP_BACKLIGHT_H
#define BSP_BACKLIGHT_H

#include "esp_err.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_backlight_init(void);
esp_err_t bsp_backlight_set(uint8_t brightness_percent);

#ifdef __cplusplus
}
#endif

#endif // BSP_BACKLIGHT_H
