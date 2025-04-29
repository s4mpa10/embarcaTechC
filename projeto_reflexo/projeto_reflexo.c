#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define BUTTON_A 5    // GPIO conectado ao Botão A
#define BUTTON_B 6    // GPIO conectado ao Botão B
#define LED_PIN 13    // GPIO conectado ao LED na cor vermelha

// Função que aguarda de 1 a 7 segundos aleatórios antes de acender o LED
void espera_aleatoria() {
    int tempo_espera = (rand() % 7 + 1) * 1000;  // Gera número entre 1 e 7, em milissegundos
    sleep_ms(tempo_espera);
    gpio_put(LED_PIN, 1);  // Acende o LED
    sleep_ms(200);  // Mantém o LED aceso por 200ms
    gpio_put(LED_PIN, 0);
}

// Função que converte o tempo de milessegundos para segundos
int tempo_reacao_button(int tempo){
    float tempo_reacao;

    tempo_reacao = tempo / 1000.0; 

    return tempo_reacao;
}


int main() {
    stdio_init_all();
    srand(time_us_64());  // Inicializa a semente do gerador de números aleatórios

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);  // Garante que o LED comece desligado

    while (1) {
        // Se botão B for pressionado, inicia o processo
        if (gpio_get(BUTTON_B) == 0) {
            espera_aleatoria();

            // Captura o tempo em que o LED foi aceso
            absolute_time_t inicio_reacao = get_absolute_time();

            // Aguarda o botão A ser pressionado (tempo de reação)
            while (gpio_get(BUTTON_A) != 0) {
                tight_loop_contents();  // Aguarda ocupando pouco a CPU
            }

            // Captura o tempo de reação e calcula o intervalo
            absolute_time_t fim_reacao = get_absolute_time();
            int64_t tempo_reacao = absolute_time_diff_us(inicio_reacao, fim_reacao) / 1000;  // Em milissegundos

            gpio_put(LED_PIN, 0);  // Desliga o LED

            printf("Tempo de reação em milisegundos: %lld ms\n", tempo_reacao);
            printf("Tempo de reação em segundos: %.3f s\n", tempo_reacao_button(tempo_reacao));

            // Aguarda o botão A ser solto antes de permitir nova rodada
            while (gpio_get(BUTTON_A) == 0) {
                sleep_ms(10);
            }
        }
    }

    return 0;
}
