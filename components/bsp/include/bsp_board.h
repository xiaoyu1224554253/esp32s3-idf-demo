#ifndef BSP_BOARD_H
#define BSP_BOARD_H

#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_board_init(void);
i2c_master_bus_handle_t bsp_i2c_get_bus(void);

#ifdef __cplusplus
}
#endif

#endif // BSP_BOARD_H
