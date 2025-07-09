/**
 * @file buzzer.c
 * @brief Implementação do driver para controle de buzzer via PWM
 * @author Seu Nome
 * @date 06/12/2024
 */

#include "buzzer.h"
#include "hardware/clocks.h"

void buzzer_init(uint pin, uint frequency) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Calcular o divisor de clock para a frequência desejada
    // O counter do PWM roda a clock_sys / (divisor * 256)
    float divisor = (float)clock_get_hz(clk_sys) / (frequency * 4096);

    // Configurar o PWM
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, divisor);
    pwm_config_set_wrap(&config, 4095);  // 12 bits de resolução
    pwm_init(slice_num, &config, true);

    // Iniciar com o buzzer desligado
    buzzer_stop(pin);
}

void buzzer_beep(uint pin, uint duration_ms, uint8_t duty_cycle) {
    // Validar duty cycle
    if (duty_cycle > 100) duty_cycle = 100;

    // Calcular nível do PWM (0-4095)
    uint16_t level = (4095 * duty_cycle) / 100;
    pwm_set_gpio_level(pin, level);

    // Manter o beep pelo tempo especificado
    sleep_ms(duration_ms);

    // Desligar o buzzer
    buzzer_stop(pin);
}

void buzzer_stop(uint pin) {
    pwm_set_gpio_level(pin, 0);
}

void buzzer_play_sequence(uint pin, const uint beeps[], const uint pauses[], uint count, uint8_t duty_cycle) {
    for (uint i = 0; i < count; i++) {
        buzzer_beep(pin, beeps[i], duty_cycle);
        if (i < count - 1) {
            sleep_ms(pauses[i]);
        }
    }
}