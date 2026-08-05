idf_component_register(
    SRCS "src/lvgl_port.c"
    INCLUDE_DIRS "include"
    REQUIRES bsp esp_lvgl_port lvgl
)
