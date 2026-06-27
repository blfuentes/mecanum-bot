#include "control.h"

#include <driver/gpio.h>
#include <esp_adc/adc_cali.h>
#include <esp_adc/adc_cali_scheme.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_err.h>
#include <esp_log.h>

static const char* CONTROL_TAG = "CONTROL";

#define ADC_SATURATION_RAW 4090
#define CONTROL_SAMPLES    8

static adc_oneshot_unit_handle_t adc_handle;
static adc_channel_t left_control_x_channel  = ADC_CHANNEL_0;
static adc_channel_t left_control_y_channel  = ADC_CHANNEL_1;
static adc_channel_t right_control_x_channel = ADC_CHANNEL_2;
static adc_channel_t right_control_y_channel = ADC_CHANNEL_3;

static adc_cali_handle_t adc_cali_handle;
static bool adc_cali_enabled = false;

static gpio_num_t left_button_pin  = GPIO_NUM_NC;
static gpio_num_t right_button_pin = GPIO_NUM_NC;

static bool adc_calibration_init(void) {
    esp_err_t ret = ESP_FAIL;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id  = ADC_UNIT_1,
        .atten    = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &adc_cali_handle);
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id      = ADC_UNIT_1,
        .atten        = ADC_ATTEN_DB_12,
        .bitwidth     = ADC_BITWIDTH_12,
        .default_vref = 1100,
    };
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &adc_cali_handle);
#endif

    if (ret == ESP_OK) {
        ESP_LOGI(CONTROL_TAG, "ADC calibration enabled");
        return true;
    }

    ESP_LOGW(CONTROL_TAG, "ADC calibration not available: %s", esp_err_to_name(ret));
    return false;
}

static bool adc_channel_from_gpio(gpio_num_t gpio, adc_channel_t* channel) {
    switch (gpio) {
        case GPIO_NUM_0:
            *channel = ADC_CHANNEL_0;
            return true;
        case GPIO_NUM_1:
            *channel = ADC_CHANNEL_1;
            return true;
        case GPIO_NUM_2:
            *channel = ADC_CHANNEL_2;
            return true;
        case GPIO_NUM_3:
            *channel = ADC_CHANNEL_3;
            return true;
        case GPIO_NUM_4:
            *channel = ADC_CHANNEL_4;
            return true;
        default:
            return false;
    }
}

static void configure_joystick_button(gpio_num_t pin) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
}

void control_init(const ControlConfig* config) {
    // Left Joystick
    left_button_pin = config->left_config.button_control_pin;
    configure_joystick_button(left_button_pin);
    if ((config != NULL) &&
        !adc_channel_from_gpio(config->left_config.x_control_pin, &left_control_x_channel)) {
        ESP_LOGW(CONTROL_TAG, "Invalid LEFT CONTROL X GPIO for ADC: %d. Using ADC_CHANNEL_0.",
                 (int)config->left_config.x_control_pin);
        left_control_x_channel = ADC_CHANNEL_0;
    }
    if ((config != NULL) &&
        !adc_channel_from_gpio(config->left_config.y_control_pin, &left_control_y_channel)) {
        ESP_LOGW(CONTROL_TAG, "Invalid LEFT CONTROL Y GPIO for ADC: %d. Using ADC_CHANNEL_1.",
                 (int)config->left_config.y_control_pin);
        left_control_y_channel = ADC_CHANNEL_1;
    }
    // Right Joystick
    right_button_pin = config->right_config.button_control_pin;
    configure_joystick_button(right_button_pin);
    if ((config != NULL) &&
        !adc_channel_from_gpio(config->right_config.x_control_pin, &right_control_x_channel)) {
        ESP_LOGW(CONTROL_TAG, "Invalid RIGHT CONTROL X GPIO for ADC: %d. Using ADC_CHANNEL_2.",
                 (int)config->right_config.x_control_pin);
        right_control_x_channel = ADC_CHANNEL_2;
    }
    if ((config != NULL) &&
        !adc_channel_from_gpio(config->right_config.y_control_pin, &right_control_y_channel)) {
        ESP_LOGW(CONTROL_TAG, "Invalid RIGHT CONTROL Y GPIO for ADC: %d. Using ADC_CHANNEL_3.",
                 (int)config->right_config.y_control_pin);
        right_control_y_channel = ADC_CHANNEL_3;
    }

    // Initialize ADC for LEFT CONTROL X
    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&unit_cfg, &adc_handle);

    adc_oneshot_chan_cfg_t left_control_chan_cfg = {
        .atten    = ADC_ATTEN_DB_12,  // Full range ~0-3.3V
        .bitwidth = ADC_BITWIDTH_12,  // 0-4095
    };
    adc_oneshot_config_channel(adc_handle, left_control_x_channel, &left_control_chan_cfg);
    adc_oneshot_config_channel(adc_handle, left_control_y_channel, &left_control_chan_cfg);

    // Initialize ADC for hygrometer
    adc_oneshot_chan_cfg_t right_control_chan_cfg = {
        .atten    = ADC_ATTEN_DB_12,  // Full range ~0-3.3V
        .bitwidth = ADC_BITWIDTH_12,  // 0-4095
    };
    adc_oneshot_config_channel(adc_handle, right_control_x_channel, &right_control_chan_cfg);
    adc_oneshot_config_channel(adc_handle, right_control_y_channel, &right_control_chan_cfg);

    adc_cali_enabled = adc_calibration_init();

    ESP_LOGI(CONTROL_TAG, "ADC channels configured");
}

void control_update(ControlData* data) {
    int left_x_sum  = 0;
    int left_y_sum  = 0;
    int right_x_sum = 0;
    int right_y_sum = 0;

    esp_err_t ret;
    for (int i = 0; i < CONTROL_SAMPLES; ++i) {
        int l_x_sample = 0;
        int l_y_sample = 0;
        int r_x_sample = 0;
        int r_y_sample = 0;
        // adc_oneshot_read(adc_handle, left_control_x_channel, &l_x_sample);
        // adc_oneshot_read(adc_handle, left_control_y_channel, &l_y_sample);
        // adc_oneshot_read(adc_handle, right_control_x_channel, &r_x_sample);
        // adc_oneshot_read(adc_handle, right_control_y_channel, &r_y_sample);

        ret = adc_oneshot_read(adc_handle, left_control_x_channel, &l_x_sample);
        if (ret != ESP_OK) {
            ESP_LOGE(CONTROL_TAG, "ADC read failed: %s", esp_err_to_name(ret));
        }
        ret = adc_oneshot_read(adc_handle, left_control_y_channel, &l_y_sample);
        if (ret != ESP_OK) {
            ESP_LOGE(CONTROL_TAG, "ADC read failed: %s", esp_err_to_name(ret));
        }
        ret = adc_oneshot_read(adc_handle, right_control_x_channel, &r_x_sample);
        if (ret != ESP_OK) {
            ESP_LOGE(CONTROL_TAG, "ADC read failed: %s", esp_err_to_name(ret));
        }
        ret = adc_oneshot_read(adc_handle, right_control_y_channel, &r_y_sample);
        if (ret != ESP_OK) {
            ESP_LOGE(CONTROL_TAG, "ADC read failed: %s", esp_err_to_name(ret));
        }
        left_x_sum += l_x_sample;
        left_y_sum += l_y_sample;
        right_x_sum += r_x_sample;
        right_y_sum += r_y_sample;
    }
    int l_x_value = left_x_sum / CONTROL_SAMPLES;
    int l_y_value = left_y_sum / CONTROL_SAMPLES;
    int r_x_value = right_x_sum / CONTROL_SAMPLES;
    int r_y_value = right_y_sum / CONTROL_SAMPLES;
    int l_x_mV    = 0;
    int l_y_mV    = 0;
    int r_x_mV    = 0;
    int r_y_mV    = 0;
    if (adc_cali_enabled) {
        adc_cali_raw_to_voltage(adc_cali_handle, l_x_value, &l_x_mV);
        adc_cali_raw_to_voltage(adc_cali_handle, l_y_value, &l_y_mV);
        adc_cali_raw_to_voltage(adc_cali_handle, r_x_value, &r_x_mV);
        adc_cali_raw_to_voltage(adc_cali_handle, r_y_value, &r_y_mV);
    }

    data->left_control.x_level      = l_x_value;
    data->left_control.y_level      = l_y_value;
    data->right_control.x_level     = r_x_value;
    data->right_control.y_level     = r_y_value;
    data->left_control.press_level  = (gpio_get_level(left_button_pin) == 0);
    data->right_control.press_level = (gpio_get_level(right_button_pin) == 0);

    // ESP_LOGI(CONTROL_TAG, "Left X: %d Y: %d :: Right X: %d Y: %d");
}