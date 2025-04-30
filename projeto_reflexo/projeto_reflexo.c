#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include <ctype.h>
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include <string.h>
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2818b.pio.h"

#define BUTTON_A 5    // GPIO conectado ao Botão A
#define BUTTON_B 6    // GPIO conectado ao Botão B
#define LED_PIN 13    // GPIO conectado ao LED na cor vermelha
#define LED_COUNT 25

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

// Função que aguarda de 1 a 7 segundos aleatórios antes de acender o LED
// void espera_aleatoria() {
//     int tempo_espera = (rand() % 7 + 1) * 1000;  // Gera número entre 1 e 7, em milissegundos
//     sleep_ms(tempo_espera);
//     gpio_put(LED_PIN, 1);  // Acende o LED
//     sleep_ms(200);  // Mantém o LED aceso por 200ms
//     gpio_put(LED_PIN, 0);
// }

void espera_aleatoria() {
    int tempo_espera = (rand() % 7 + 1) * 1000;  // Gera número entre 1 e 7 segundos
    sleep_ms(tempo_espera);

    int indice_led = rand() % LED_COUNT; // Seleciona um LED aleatório (0 a 24)

    // Acende o LED escolhido em vermelho
    npClear();                         // Limpa todos os LEDs
    npSetLED(indice_led, 255, 0, 0);   // Vermelho
    npWrite();                         // Envia ao LED
    sleep_ms(200);                     // Mantém aceso por 200ms

    // Apaga o LED
    npClear();
    npWrite();
}


// Função que converte o tempo de milessegundos para segundos
int tempo_reacao_button(int tempo){
    int tempo_reacao;

    tempo_reacao = tempo / 1000; 

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
    // gpio_put(LED_PIN, 0);  // Garante que o LED comece desligado
    npInit(LED_PIN);
    npClear();
    npWrite();

    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();
    
    // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);
    
    // zera o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

restart:


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
            printf("Tempo de reação em segundos: %d s\n", tempo_reacao_button(tempo_reacao));

            // Limpa o display OLED
            memset(ssd, 0, ssd1306_buffer_length);

            // Converte valores para string
            char buffer_ms[24];
            char buffer_s[24];
            snprintf(buffer_ms, sizeof(buffer_ms), "Tempo: %lld ms", tempo_reacao);
            snprintf(buffer_s, sizeof(buffer_s), "Tempo: %d s", tempo_reacao_button(tempo_reacao));

            // Exibe as strings no display
            ssd1306_draw_string(ssd, 0, 0, "Reacao medida:");
            ssd1306_draw_string(ssd, 0, 20, buffer_ms);
            ssd1306_draw_string(ssd, 0, 30, buffer_s);

            // Renderiza no display
            render_on_display(ssd, &frame_area);

            // Aguarda o botão A ser solto antes de permitir nova rodada
            while (gpio_get(BUTTON_A) == 0) {
                sleep_ms(10);
            }
        }
    }

    return 0;
}










// #include "pico/stdlib.h"
// #include "stdio.h"
// #include "stdlib.h"
// #include "time.h"
// #include <ctype.h>
// #include "pico/binary_info.h"
// #include "inc/ssd1306.h"
// #include <string.h>
// #include "hardware/i2c.h"

// #define BUTTON_A 5    // GPIO conectado ao Botão A
// #define BUTTON_B 6    // GPIO conectado ao Botão B
// #define LED_PIN 13    // GPIO conectado ao LED na cor vermelha

// const uint I2C_SDA = 14;
// const uint I2C_SCL = 15;

// // Função que aguarda de 1 a 7 segundos aleatórios antes de acender o LED
// void espera_aleatoria() {
//     int tempo_espera = (rand() % 7 + 1) * 1000;  // Gera número entre 1 e 7, em milissegundos
//     sleep_ms(tempo_espera);
//     gpio_put(LED_PIN, 1);  // Acende o LED
//     sleep_ms(200);  // Mantém o LED aceso por 200ms
//     gpio_put(LED_PIN, 0);
// }

// // Função que converte o tempo de milessegundos para segundos
// int tempo_reacao_button(int tempo){
//     int tempo_reacao;

//     tempo_reacao = tempo / 1000; 

//     return tempo_reacao;
// }

// int main() {
//     stdio_init_all();
//     srand(time_us_64());  // Inicializa a semente do gerador de números aleatórios

//     gpio_init(BUTTON_A);
//     gpio_set_dir(BUTTON_A, GPIO_IN);
//     gpio_pull_up(BUTTON_A);

//     gpio_init(BUTTON_B);
//     gpio_set_dir(BUTTON_B, GPIO_IN);
//     gpio_pull_up(BUTTON_B);

//     gpio_init(LED_PIN);
//     gpio_set_dir(LED_PIN, GPIO_OUT);
//     gpio_put(LED_PIN, 0);  // Garante que o LED comece desligado

//     // Inicialização do i2c
//     i2c_init(i2c1, ssd1306_i2c_clock * 1000);
//     gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
//     gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
//     gpio_pull_up(I2C_SDA);
//     gpio_pull_up(I2C_SCL);

//     // Processo de inicialização completo do OLED SSD1306
//     ssd1306_init();
    
//     // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
//     struct render_area frame_area = {
//         start_column : 0,
//         end_column : ssd1306_width - 1,
//         start_page : 0,
//         end_page : ssd1306_n_pages - 1
//     };

//     calculate_render_area_buffer_length(&frame_area);
    
//     // zera o display inteiro
//     uint8_t ssd[ssd1306_buffer_length];
//     memset(ssd, 0, ssd1306_buffer_length);
//     render_on_display(ssd, &frame_area);

// restart:


//     while (1) {
//         // Se botão B for pressionado, inicia o processo
//         if (gpio_get(BUTTON_B) == 0) {
//             espera_aleatoria();

//             // Captura o tempo em que o LED foi aceso
//             absolute_time_t inicio_reacao = get_absolute_time();

//             // Aguarda o botão A ser pressionado (tempo de reação)
//             while (gpio_get(BUTTON_A) != 0) {
//                 tight_loop_contents();  // Aguarda ocupando pouco a CPU
//             }

//             // Captura o tempo de reação e calcula o intervalo
//             absolute_time_t fim_reacao = get_absolute_time();
//             int64_t tempo_reacao = absolute_time_diff_us(inicio_reacao, fim_reacao) / 1000;  // Em milissegundos

//             gpio_put(LED_PIN, 0);  // Desliga o LED

//             printf("Tempo de reação em milisegundos: %lld ms\n", tempo_reacao);
//             printf("Tempo de reação em segundos: %d s\n", tempo_reacao_button(tempo_reacao));

//             // Limpa o display OLED
//             memset(ssd, 0, ssd1306_buffer_length);

//             // Converte valores para string
//             char buffer_ms[24];
//             char buffer_s[24];
//             snprintf(buffer_ms, sizeof(buffer_ms), "Tempo: %lld ms", tempo_reacao);
//             snprintf(buffer_s, sizeof(buffer_s), "Tempo: %d s", tempo_reacao_button(tempo_reacao));

//             // Exibe as strings no display
//             ssd1306_draw_string(ssd, 0, 0, "Reacao medida:");
//             ssd1306_draw_string(ssd, 0, 20, buffer_ms);
//             ssd1306_draw_string(ssd, 0, 30, buffer_s);

//             // Renderiza no display
//             render_on_display(ssd, &frame_area);

//             // Aguarda o botão A ser solto antes de permitir nova rodada
//             while (gpio_get(BUTTON_A) == 0) {
//                 sleep_ms(10);
//             }
//         }
//     }

//     return 0;
// }
