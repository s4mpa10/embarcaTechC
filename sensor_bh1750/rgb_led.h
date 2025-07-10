#ifndef RGB_LED_H
#define RGB_LED_H

#include "pico/stdlib.h"

// Funções públicas
void rgb_led_init();
void acender_led(float red, float green, float blue);

#endif // RGB_LED_H