idf_component_register(
    SRCS "src/ui_common.c"
         "src/ui_manager.c"
         "src/ui_player.c"
         "src/ui_playlist.c"
         "src/ui_radio.c"
         "src/ui_search.c"
         "fonts/font_cn_12.c"
         "fonts/font_cn_14.c"
    INCLUDE_DIRS "include"
    REQUIRES lvgl
)
