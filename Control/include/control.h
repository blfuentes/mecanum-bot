#ifndef CONTROL_H__
#define CONTROL_H__

#include <driver/i2c_master.h>
#include <stdint.h>

typedef struct {
    gpio_num_t x_control_pin;
    gpio_num_t y_control_pin;
    gpio_num_t button_control_pin;
} JoystickDef;

typedef struct {
    JoystickDef left_config;
    JoystickDef right_config;
} ControlConfig;

typedef struct {
    uint16_t x_level;
    uint16_t y_level;
    bool press_level;
} JoystickData;

typedef struct {
    JoystickData left_control;
    JoystickData right_control;
} ControlData;

void control_init(const ControlConfig* config);

void control_update(ControlData* data);

#endif  // CONTROL_H__