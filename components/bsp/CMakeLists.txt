idf_component_register(
    SRCS "src/bsp_board.c"
         "src/bsp_lcd.c"
         "src/bsp_touch.c"
         "src/bsp_backlight.c"
         "src/bsp_audio.c"
         "src/bsp_sdcard.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_lcd esp_timer esp_driver_ledc esp_driver_i2s esp_codec_dev fatfs
)
