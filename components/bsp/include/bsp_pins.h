#ifndef BSP_PINS_H
#define BSP_PINS_H

#ifdef __cplusplus
extern "C" {
#endif

/* LCD: ILI9341V, 4-Line SPI */
#define BSP_LCD_CS_GPIO     10
#define BSP_LCD_DC_GPIO     46
#define BSP_LCD_SCK_GPIO    12
#define BSP_LCD_MOSI_GPIO   11
#define BSP_LCD_MISO_GPIO   13
#define BSP_LCD_RST_GPIO    (-1)  /* 与 ESP32-S3 EN 复位共用 */
#define BSP_LCD_BL_GPIO     45    /* 高电平点亮 */

/* Touch: FT6336G, I2C */
#define BSP_TOUCH_SDA_GPIO  16
#define BSP_TOUCH_SCL_GPIO  15
#define BSP_TOUCH_RST_GPIO  18
#define BSP_TOUCH_INT_GPIO  17
#define BSP_TOUCH_I2C_ADDR  0x38

/* RGB LED: WS2812B */
#define BSP_RGB_LED_GPIO    42

/* SD Card: SDIO */
#define BSP_SD_CLK_GPIO     38
#define BSP_SD_CMD_GPIO     40
#define BSP_SD_D0_GPIO      39
#define BSP_SD_D1_GPIO      41
#define BSP_SD_D2_GPIO      48
#define BSP_SD_D3_GPIO      47

/* Audio: ES8311 + FM8002E PA, I2S */
#define BSP_AUDIO_PA_GPIO   1     /* 低电平使能功放 */
#define BSP_AUDIO_MCLK_GPIO 4
#define BSP_AUDIO_BCLK_GPIO 5
#define BSP_AUDIO_DOUT_GPIO 6     /* ESP32 -> ES8311 */
#define BSP_AUDIO_LRCK_GPIO 7
#define BSP_AUDIO_DIN_GPIO  8     /* ES8311 -> ESP32 */

/* Button / UART / Battery */
#define BSP_BOOT_GPIO       0
#define BSP_UART_RX_GPIO    43
#define BSP_UART_TX_GPIO    44
#define BSP_BAT_ADC_GPIO    9

/* LCD resolution after rotation (landscape) */
#define BSP_LCD_HOR_RES     320
#define BSP_LCD_VER_RES     240

#ifdef __cplusplus
}
#endif

#endif /* BSP_PINS_H */
