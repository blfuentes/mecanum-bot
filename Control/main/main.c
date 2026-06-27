#include <esp_adc/adc_cali.h>
#include <esp_adc/adc_cali_scheme.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
// #include <memory>

#include <stdio.h>

#include "control.h"
#include "libnow.h"

// TASKS
#define SENSOR_TASK_PERIOD_MS  20
#define SENSOR_TASK_STACK_SIZE 3072
#define SENSOR_TASK_PRIORITY   5

static const char* MAINT_TAG  = "CONTROL";
static const char* LIBNOW_TAG = "LIBNOW";

typedef struct {
    ControlData data;
} ControlMessage;

// JOYSTICK
adc_channel_t JOYSTICK_LEFT_CHANNEL_X = ADC_CHANNEL_0;  // GPIO00
gpio_num_t LEFT_CONTROL_X             = GPIO_NUM_0;
adc_channel_t JOYSTICK_LEFT_CHANNEL_Y = ADC_CHANNEL_1;  // GPIO01
gpio_num_t LEFT_CONTROL_Y             = GPIO_NUM_1;
gpio_num_t JOYSTICK_LEFT_BUTTON_PIN   = GPIO_NUM_4;

adc_channel_t JOYSTICK_RIGHT_CHANNEL_X = ADC_CHANNEL_2;  // GPIO02
gpio_num_t RIGHT_CONTROL_X             = GPIO_NUM_2;
adc_channel_t JOYSTICK_RIGHT_CHANNEL_Y = ADC_CHANNEL_3;  // GPIO03
gpio_num_t RIGHT_CONTROL_Y             = GPIO_NUM_3;
gpio_num_t JOYSTICK_RIGHT_BUTTON_PIN   = GPIO_NUM_5;

static void control_task(void* arg) {
    (void)arg;
    ControlMessage msg;
    message_control_status lib_msg = {0};
    // ESP_LOGI(MAINT_TAG, "Control task started");

    for (;;) {
        control_update(&msg.data);

        // ESP_LOGI(MAINT_TAG, "Read control data");

        ESP_LOGI(MAINT_TAG, "Left X: %d Y: %d :: Right X: %d Y: %d", msg.data.left_control.x_level,
                 msg.data.left_control.y_level, msg.data.right_control.x_level,
                 msg.data.right_control.y_level);

        lib_msg.left_control.x_value  = msg.data.left_control.x_level;
        lib_msg.left_control.y_value  = msg.data.left_control.y_level;
        lib_msg.left_control.pressed  = msg.data.left_control.press_level;
        lib_msg.right_control.x_value = msg.data.right_control.x_level;
        lib_msg.right_control.y_value = msg.data.right_control.y_level;
        lib_msg.right_control.pressed = msg.data.right_control.press_level;
        libnow_sendMessage(DST_ROBOT, &lib_msg);

        // Keep only the latest sample so display is always up to date.
        // BaseType_t ret = xQueueOverwrite(g_sensor_queue, &msg);
        // if (ret != pdPASS) {
        //     ESP_LOGE("SENSOR_TASK", "Failed to write to queue");
        // }

        vTaskDelay(pdMS_TO_TICKS(SENSOR_TASK_PERIOD_MS));
    }
}

void app_main(void) {
    // Initialize LibNow
    ESP_LOGI(LIBNOW_TAG, "Initializing LibNow...");
    libnow_init();
    libnow_addPeer(DST_ROBOT);
    ESP_LOGI(LIBNOW_TAG, "LibNow initialized");

    ControlConfig control_config = {.left_config.x_control_pin       = LEFT_CONTROL_X,
                                    .left_config.y_control_pin       = LEFT_CONTROL_Y,
                                    .left_config.button_control_pin  = JOYSTICK_LEFT_BUTTON_PIN,
                                    .right_config.x_control_pin      = RIGHT_CONTROL_X,
                                    .right_config.y_control_pin      = RIGHT_CONTROL_Y,
                                    .right_config.button_control_pin = JOYSTICK_RIGHT_BUTTON_PIN};
    control_init(&control_config);
    xTaskCreate(control_task, "control_task", SENSOR_TASK_STACK_SIZE, NULL, SENSOR_TASK_PRIORITY,
                NULL);
}
