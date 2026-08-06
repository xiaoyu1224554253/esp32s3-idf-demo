idf_component_register(
    SRCS "src/bsp_board.c"
         "src/bsp_lcd.c"
         "src/bsp_touch.c"
         "src/bsp_backlight.c"
         "src/bsp_audio.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_lcd esp_timer esp_driver_ledc esp_codec_dev
)
