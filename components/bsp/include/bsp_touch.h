#ifndef BSP_TOUCH_H
#define BSP_TOUCH_H

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_touch_init(void);
bool bsp_touch_read(uint16_t *x, uint16_t *y, bool *pressed);

// Coordinate mapping for landscape mode
void bsp_touch_map_to_landscape(uint16_t raw_x, uint16_t raw_y, uint16_t *lcd_x, uint16_t *lcd_y);

#ifdef __cplusplus
}
#endif

#endif // BSP_TOUCH_H
