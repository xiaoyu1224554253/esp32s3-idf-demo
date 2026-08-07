#ifndef BSP_LCD_H
#define BSP_LCD_H

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_lcd_init(void);
esp_lcd_panel_handle_t bsp_lcd_get_panel(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_LCD_H */
