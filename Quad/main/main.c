/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <esp_log.h>
#include <esp_now.h>
#include <inttypes.h>
#include <libnow.h>
#include <sprites.h>
#include <ssd1306.h>
#include <stdio.h>
#include "display.h"
#include "driver/ledc.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

static const char* LIBNOW_TAG  = "LIBNOW";
static const char* MESSAGE_TAG = "MESSAGE";
static const char* SERVO_TAG   = "SERVO";

static const int CONTROL_THRESHOLD = 50;

// SERVO

Display display;
float angle = 100.0f;

message_control_status last_msg = {.left_control.x_value  = -1,
                                   .left_control.y_value  = -1,
                                   .left_control.pressed  = false,
                                   .right_control.x_value = -1,
                                   .right_control.y_value = -1,
                                   .right_control.pressed = false};

bool messageChanged(const message_control_status msg) {
    return (abs(last_msg.left_control.x_value - msg.left_control.x_value) > CONTROL_THRESHOLD ||
            abs(last_msg.left_control.y_value - msg.left_control.y_value) > CONTROL_THRESHOLD ||
            abs(last_msg.left_control.pressed - msg.left_control.pressed) > CONTROL_THRESHOLD ||
            abs(last_msg.right_control.x_value - msg.right_control.x_value) > CONTROL_THRESHOLD ||
            abs(last_msg.right_control.y_value - msg.right_control.y_value) > CONTROL_THRESHOLD ||
            abs(last_msg.right_control.pressed - msg.right_control.pressed) > CONTROL_THRESHOLD);
}

static void printFace(message_control_status data) {
    if (data.left_control.x_value > 2000 && data.left_control.x_value < 3000 &&
        data.left_control.y_value > 2000 && data.left_control.y_value < 3000 &&
        data.right_control.x_value > 2000 && data.right_control.x_value < 3000 &&
        data.right_control.y_value > 2000 && data.right_control.y_value < 3000) {
        display_matrix_content(&display, HappyFace);
    }
    // - Left + Left
    else if (data.left_control.x_value < 1000 && data.right_control.x_value < 1000) {
        display_matrix_content(&display, LookLeftLeft);
    }
    // - Left + Right
    else if (data.left_control.x_value < 1000 && data.right_control.x_value > 3000) {
        display_matrix_content(&display, LookLeftRight);
    }
    // - Left + Up
    else if (data.left_control.x_value < 1000 && data.right_control.y_value < 1000) {
        display_matrix_content(&display, LookLeftUp);
    }
    // - Left + Down
    else if (data.left_control.x_value < 1000 && data.right_control.y_value > 3000) {
        display_matrix_content(&display, LookLeftDown);
    }
    // - Right + Left
    else if (data.left_control.x_value > 3000 && data.right_control.x_value < 1000) {
        display_matrix_content(&display, LookRightLeft);
    }
    // - Right + Right
    else if (data.left_control.x_value > 3000 && data.right_control.x_value > 3000) {
        display_matrix_content(&display, LookRightRight);
    }
    // - Right + Up
    else if (data.left_control.x_value > 3000 && data.right_control.y_value < 1000) {
        display_matrix_content(&display, LookRightUp);
    }
    // - Right + Down
    else if (data.left_control.x_value > 3000 && data.right_control.y_value > 3000) {
        display_matrix_content(&display, LookRightDown);
    }
    // - Up + Left
    else if (data.left_control.y_value < 1000 && data.right_control.x_value < 1000) {
        display_matrix_content(&display, LookUpLeft);
    }
    // - Up + Right
    else if (data.left_control.y_value < 1000 && data.right_control.x_value > 3000) {
        display_matrix_content(&display, LookUpRight);
    }
    // - Up + Up
    else if (data.left_control.y_value < 1000 && data.right_control.y_value < 1000) {
        display_matrix_content(&display, LookUpUp);
    }
    // - Up + Down
    else if (data.left_control.y_value < 1000 && data.right_control.y_value > 3000) {
        display_matrix_content(&display, LookUpDown);
    }
    // - Down + Left
    else if (data.left_control.y_value > 3000 && data.right_control.x_value < 1000) {
        display_matrix_content(&display, LookDownLeft);
    }
    // - Down + Right
    else if (data.left_control.y_value > 3000 && data.right_control.x_value > 3000) {
        display_matrix_content(&display, LookDownRight);
    }
    // - Down + Up
    else if (data.left_control.y_value > 3000 && data.right_control.y_value < 1000) {
        display_matrix_content(&display, LookDownUp);
    }
    // - Down + Down
    else if (data.left_control.y_value > 3000 && data.right_control.y_value > 3000) {
        display_matrix_content(&display, LookDownDown);
    }
}

static void recvcb(const esp_now_recv_info_t* esp_now_info, const uint8_t* data, int data_len) {
    message_control_status msg = *(message_control_status*)&data[0];
    if (messageChanged(msg)) {
        ESP_LOGI(MESSAGE_TAG, "Message changed. LeftX: %d , LeftY: %d, RightX: %d, RightY:  %d",
                 msg.left_control.x_value, msg.left_control.y_value, msg.right_control.x_value,
                 msg.right_control.y_value);
        last_msg = msg;  // Update last message
        printFace(msg);
    }
}

void app_main(void) {

    // Initialize LibNow
    ESP_LOGI(LIBNOW_TAG, "Initializing LibNow...");
    libnow_init();
    libnow_addPeer(DST_MANDO);
    esp_now_register_recv_cb(recvcb);
    ESP_LOGI(LIBNOW_TAG, "LibNow initialized");

    display_init(&display);

    // Servo config

    // display_show_status(&display, "0123456789abcdef", "0123456789abcdef", "0123456789abcdef",
    //                     "0123456789abcdef", "0123456789abcdef", "0123456789abcdef",
    //                     "0123456789abcdef", "0123456789abcdef");

    // /* Print chip information */
    // esp_chip_info_t chip_info;
    // uint32_t flash_size;
    // esp_chip_info(&chip_info);
    // printf("This is %s chip with %d CPU core(s), %s%s%s%s, ", CONFIG_IDF_TARGET, chip_info.cores,
    //        (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
    //        (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
    //        (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
    //        (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    // unsigned major_rev = chip_info.revision / 100;
    // unsigned minor_rev = chip_info.revision % 100;
    // printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    // if (esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
    //     printf("Get flash size failed");
    //     return;
    // }

    // printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
    //        (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    // printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    // for (int i = 10; i >= 0; i--) {
    //     printf("Restarting in %d seconds...\n", i);
    //     if (i % 2 == 0) {
    //         display_matrix_content(&display, LookLeft);
    //     } else {
    //         display_matrix_content(&display, LookRight);
    //     }
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
    // // display_show_status(&display, HappyFace[0], HappyFace[1], HappyFace[2], HappyFace[3],
    // //                     HappyFace[4], HappyFace[5], HappyFace[6], HappyFace[7]);
    // display_matrix_content(&display, HappyFace);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // printf("Restarting now.\n");
    // fflush(stdout);
    // esp_restart();
}
