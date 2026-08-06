idf_component_register(
    SRCS "main.c"
         "ui/page_now_playing.c"
         "ui/page_playlist.c"
         "ui/page_radio.c"
    INCLUDE_DIRS "."
                 "ui"
    PRIV_REQUIRES bsp lvgl_port music_player
)
