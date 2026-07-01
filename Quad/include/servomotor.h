#ifndef SERVOMOTOR_H__
#define SERVOMOTOR_H__

#include <driver/gpio.h>
#include <driver/ledc.h>

typedef struct {
    gpio_num_t pin;
    ledc_mode_t speed_mode;
    ledc_channel_t channel;
} ServoMotor;

void servomotor_init(ServoMotor* servomotor, gpio_num_t servoPin, ledc_mode_t speed_mode,
                     ledc_channel_t channel);
void drive(ServoMotor* servomotor, bool forward);
void stop(ServoMotor* servomotor);

#endif  // SERVOMOTOR_H__