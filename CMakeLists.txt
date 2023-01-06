idf_component_register(
    SRCS "blemidi.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES "bt" "nvs_flash" "console"
)
