idf_component_register(SRCS "main.c" "control.c"
                    INCLUDE_DIRS "." "../include"
                    REQUIRES Libnow esp_driver_gpio esp_driver_i2c esp_driver_ledc esp_adc freertos log
                    PRIV_REQUIRES esp_common
)
