#ifndef BSP_SDCARD_H
#define BSP_SDCARD_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_sdcard_init(void);
const char *bsp_sdcard_mount_point(void);
bool bsp_sdcard_mounted(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_SDCARD_H */
