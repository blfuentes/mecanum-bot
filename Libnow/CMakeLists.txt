idf_component_register(
    SRCS "src/libnow.c"
    INCLUDE_DIRS "src"
    REQUIRES esp_wifi nvs_flash esp_event esp_netif freertos log
)
