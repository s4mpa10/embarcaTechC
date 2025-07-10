#include "ledRGB.h"
#include "pico/stdlib.h"

#define LED_RED_PIN 13

void ledrgb_init(uint red_pin, bool out){
    gpio_init(red_pin);
    gpio_set_dir(red_pin, out);
}

void ledrgb_set_color(uint red_pin, bool status){
    gpio_put(red_pin, status);
}

void ledrgb_on(uint red_pin){
    gpio_put(red_pin, 1);
    
    sleep_ms(200);
    
    gpio_put(red_pin, 0);
    
}




