#ifndef LEDRGB_H
#define LEDRGB_H

#include "pico/stdlib.h"

#define LED_RED_PIN 13

// --- Funções Públicas ---
// Função para configurar e inicializar o ledrgb que nesse caso só vou utilizar a cor vermelha para simbolizar o alerta 
// Tem tem uma função para determinar o led como saída de informação(gpio_set_dir)
void ledrgb_init(uint red_pin, bool out);

// Responsavel por ligar e desligar o led 
void ledrgb_set_color(uint red_pin, bool status);

// Pisca o led, utilizada para determinar que o led foi ligado, passou um tempo aceso e depois foi desligado 
void ledrgb_on(uint red_pin);

#endif // LED_RGB_H