#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define BUZZER_PIN 21
#define DEFAULT_BUZZER_FREQUENCY 1000  // Hz
#define DEFAULT_BEEP_DURATION_MS 500   // ms
#define DEFAULT_PAUSE_DURATION_MS 100  // ms

// --- Funções Públicas ---
// Função para inicializar o buzzer
void buzzer_init(uint pin);

// Responsavel pelo som emitido no beep e também sua largura de pulso
void buzzer_beep(uint pin, uint duration_ms, uint8_t duty_cycle);

// Função de pausar o beep
void buzzer_stop(uint pin);

#endif // BUZZER_H