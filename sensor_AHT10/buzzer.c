#include "buzzer.h"
#include "hardware/clocks.h"
#include "ledRGB.h"

// Definições de pinos e parâmetros
#define BUZZER_PIN 21
#define DEFAULT_BUZZER_FREQUENCY 1000  // Hz
#define DEFAULT_BEEP_DURATION_MS 500   // ms
#define DEFAULT_PAUSE_DURATION_MS 100  // ms


void buzzer_init(uint pin) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Calcular o divisor de clock para a frequência desejada
    // O counter do PWM roda a clock_sys / (divisor * 256)
    uint frequency = DEFAULT_BUZZER_FREQUENCY;
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

    //Ligando o LED
    ledrgb_on(LED_RED_PIN);

    // Desligar o buzzer
    buzzer_stop(pin);

    sleep_ms(DEFAULT_PAUSE_DURATION_MS); // Pausa de 100ms
}

void buzzer_stop(uint pin) {
    pwm_set_gpio_level(pin, 0);
}

