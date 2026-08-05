#ifndef BSP_PINS_H
#define BSP_PINS_H

#include "driver/gpio.h"

// Audio I2S
#define BSP_AUDIO_I2S_MCLK_GPIO     GPIO_NUM_4
#define BSP_AUDIO_I2S_BCLK_GPIO     GPIO_NUM_5
#define BSP_AUDIO_I2S_WS_GPIO       GPIO_NUM_7
#define BSP_AUDIO_I2S_DOUT_GPIO     GPIO_NUM_8
#define BSP_AUDIO_I2S_DIN_GPIO      GPIO_NUM_6
#define BSP_AUDIO_PA_GPIO           GPIO_NUM_1

// Audio Codec I2C
#define BSP_AUDIO_I2C_NUM           I2C_NUM_0
#define BSP_AUDIO_I2C_SCL_GPIO      GPIO_NUM_15
#define BSP_AUDIO_I2C_SDA_GPIO      GPIO_NUM_16

// LCD SPI
#define BSP_LCD_SPI_HOST            SPI3_HOST
#define BSP_LCD_CS_GPIO             GPIO_NUM_10
#define BSP_LCD_SCK_GPIO            GPIO_NUM_12
#define BSP_LCD_MOSI_GPIO           GPIO_NUM_11
#define BSP_LCD_MISO_GPIO           GPIO_NUM_13
#define BSP_LCD_DC_GPIO             GPIO_NUM_46
#define BSP_LCD_RST_GPIO            GPIO_NUM_NC
#define BSP_LCD_BL_GPIO             GPIO_NUM_45

// Touch I2C
#define BSP_TOUCH_I2C_NUM           I2C_NUM_0
#define BSP_TOUCH_I2C_SCL_GPIO      GPIO_NUM_15
#define BSP_TOUCH_I2C_SDA_GPIO      GPIO_NUM_16
#define BSP_TOUCH_RST_GPIO          GPIO_NUM_18
#define BSP_TOUCH_INT_GPIO          GPIO_NUM_17
#define BSP_TOUCH_I2C_ADDR          0x38

// Display parameters
#define BSP_LCD_HOR_RES             320
#define BSP_LCD_VER_RES             240
#define BSP_LCD_SWAP_XY             true
#define BSP_LCD_MIRROR_X            false
#define BSP_LCD_MIRROR_Y            false
#define BSP_LCD_INVERT_COLOR        true
#define BSP_LCD_RGB_ORDER           LCD_RGB_ELEMENT_ORDER_BGR
#define BSP_LCD_SPI_CLOCK_HZ        (20 * 1000 * 1000)
#define BSP_LCD_SPI_MODE            0

// Boot button
#define BSP_BOOT_BUTTON_GPIO        GPIO_NUM_0

#endif // BSP_PINS_H
