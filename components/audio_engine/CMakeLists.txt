idf_component_register(
    SRCS "src/audio_engine.c"
         "src/mp3_decoder.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "lib"
    REQUIRES bsp freertos
)
