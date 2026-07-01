#include "servomotor.h"
#include <driver/ledc.h>
#include <esp_pm.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// SERVO
#define LEDC_TIMER    LEDC_TIMER_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT

// 50 Hz servo frame: 20 ms period
// Approximate pulse widths:
// 1.0 ms -> full reverse
// 1.5 ms -> neutral
// 2.0 ms -> full forward
#define LEDC_DUTY_MIN     (410)  // ~1.0 ms
#define LEDC_DUTY_NEUTRAL (614)  // ~1.5 ms
#define LEDC_DUTY_MAX     (819)  // ~2.0 ms
#define LEDC_DUTY         LEDC_DUTY_NEUTRAL

#if CONFIG_PM_ENABLE
#define LEDC_CLK_SRC   LEDC_USE_RC_FAST_CLK
#define LEDC_FREQUENCY (50)
#else
#define LEDC_CLK_SRC   LEDC_AUTO_CLK
#define LEDC_FREQUENCY (50)
#endif

void servomotor_init(ServoMotor* servomotor, gpio_num_t servoPin, ledc_mode_t speed_mode,
                     ledc_channel_t channel) {
    // Servo config
#if CONFIG_PM_ENABLE
    esp_pm_config_t pm_config = {.max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
                                 .min_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
                                 .light_sleep_enable = true
#endif
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
#endif
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode      = speed_mode,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num       = LEDC_TIMER,
        .freq_hz         = LEDC_FREQUENCY,
        .clk_cfg         = LEDC_CLK_SRC,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode = speed_mode,
        .channel    = channel,
        .timer_sel  = LEDC_TIMER,
        .gpio_num   = servoPin,
        .duty       = LEDC_DUTY,
        .hpoint     = 0,
#if CONFIG_PM_ENABLE
        .sleep_mode = LEDC_SLEEP_MODE_KEEP_ALIVE,
#endif
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    // Set duty to 50% and apply it
    ESP_ERROR_CHECK(ledc_set_duty(speed_mode, channel, LEDC_DUTY));
    ESP_ERROR_CHECK(ledc_update_duty(speed_mode, channel));

    servomotor->pin        = servoPin;
    servomotor->speed_mode = speed_mode;
    servomotor->channel    = channel;
}

void drive(ServoMotor* servomotor, bool forward) {
    // SG90 360° continuous rotation:
    //   duty < LEDC_DUTY_NEUTRAL  -> spins in reverse
    //   duty = LEDC_DUTY_NEUTRAL  -> STOP (dead band ~1.5 ms)
    //   duty > LEDC_DUTY_NEUTRAL  -> spins forward
    // Set a fixed value above neutral for continuous forward motion.
    // LEDC_DUTY_MAX (~2.0 ms) = full speed forward
    const int duty = forward ? LEDC_DUTY_MAX : LEDC_DUTY_MIN;

    ESP_ERROR_CHECK(ledc_set_duty(servomotor->speed_mode, servomotor->channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(servomotor->speed_mode, servomotor->channel));

    // // Nothing more to do — hold this duty forever.
    // while (1) {
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    // }
}

void stop(ServoMotor* servomotor) {
    // SG90 360° continuous rotation:
    //   duty < LEDC_DUTY_NEUTRAL  -> spins in reverse
    //   duty = LEDC_DUTY_NEUTRAL  -> STOP (dead band ~1.5 ms)
    //   duty > LEDC_DUTY_NEUTRAL  -> spins forward
    // Set a fixed value above neutral for continuous forward motion.
    // LEDC_DUTY_MAX (~2.0 ms) = full speed forward
    const int duty = LEDC_DUTY_NEUTRAL;

    ESP_ERROR_CHECK(ledc_set_duty(servomotor->speed_mode, servomotor->channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(servomotor->speed_mode, servomotor->channel));

    // // Nothing more to do — hold this duty forever.
    // while (1) {
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    // }
}