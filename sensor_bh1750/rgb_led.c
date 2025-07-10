#include "rgb_led.h"
#include "hardware/pwm.h"

// Definições dos LEDs
#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13

// Configuração PWM
const uint16_t periodo = 200;
const float divisor_pwm = 16.0;

void config_pwm(int led) {
    uint slice;
    gpio_set_function(led, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(led);
    pwm_set_clkdiv(slice, divisor_pwm);
    pwm_set_wrap(slice, periodo);
    pwm_set_gpio_level(led, 0); // Inicia desligado
    pwm_set_enabled(slice, true);
}

void rgb_led_init() {
    config_pwm(LED_GREEN);
    config_pwm(LED_BLUE);
    config_pwm(LED_RED);
}

void acender_led(float red, float green, float blue) {
    pwm_set_gpio_level(LED_RED, red);
    pwm_set_gpio_level(LED_GREEN, green);
    pwm_set_gpio_level(LED_BLUE, blue);
}