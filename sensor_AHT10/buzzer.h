#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Definições de pinos e parâmetros
#define BUZZER_PIN 21
#define DEFAULT_BUZZER_FREQUENCY 1000  // Hz
#define DEFAULT_BEEP_DURATION_MS 200   // ms
#define DEFAULT_PAUSE_DURATION_MS 100  // ms

/**
 * @brief Inicializa o buzzer com configuração PWM
 * @param pin Pino GPIO conectado ao buzzer
 * @param frequency Frequência desejada em Hz
 */
void buzzer_init(uint pin, uint frequency);

/**
 * @brief Emite um beep com duração especificada
 * @param pin Pino GPIO do buzzer
 * @param duration_ms Duração do beep em milissegundos
 * @param duty_cycle Ciclo de trabalho (0-100%)
 */
void buzzer_beep(uint pin, uint duration_ms, uint8_t duty_cycle);

/**
 * @brief Para o buzzer imediatamente
 * @param pin Pino GPIO do buzzer
 */
void buzzer_stop(uint pin);

/**
 * @brief Toca uma sequência de beeps
 * @param pin Pino GPIO do buzzer
 * @param beeps Array com durações dos beeps (ms)
 * @param pauses Array com pausas entre beeps (ms)
 * @param count Número de elementos nos arrays
 * @param duty_cycle Ciclo de trabalho (0-100%)
 */
void buzzer_play_sequence(uint pin, const uint beeps[], const uint pauses[], uint count, uint8_t duty_cycle);

#endif // BUZZER_H