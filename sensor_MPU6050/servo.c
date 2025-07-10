#include "servo.h"
#include <stdio.h>
#include "hardware/pwm.h"
#include "hardware/gpio.h"

static uint32_t angle_to_duty_cycles(float angle_degrees) {
    uint32_t duty_us = (uint32_t)(SERVO_PULSE_MIN_US + (angle_degrees / 180.0f) * (SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US));
    
    if (duty_us < SERVO_PULSE_MIN_US) duty_us = SERVO_PULSE_MIN_US;
    if (duty_us > SERVO_PULSE_MAX_US) duty_us = SERVO_PULSE_MAX_US;
    
    return duty_us;
}

void init_servo_pwm() {
    gpio_set_function(PINO_SERVO, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PINO_SERVO);
    uint chan_num = pwm_gpio_to_channel(PINO_SERVO);

    pwm_set_wrap(slice_num, PWM_WRAP);
    pwm_set_clkdiv(slice_num, 125.0f);
    pwm_set_chan_level(slice_num, chan_num, 0);
    pwm_set_enabled(slice_num, true);
    printf("Servo PWM configurado no pino GP%d.\\n", PINO_SERVO);
}

void set_servo_angle(float angle_degrees) {
    uint slice_num = pwm_gpio_to_slice_num(PINO_SERVO);
    uint chan_num = pwm_gpio_to_channel(PINO_SERVO);
    uint32_t duty_us = angle_to_duty_cycles(angle_degrees);
    pwm_set_chan_level(slice_num, chan_num, duty_us);
}