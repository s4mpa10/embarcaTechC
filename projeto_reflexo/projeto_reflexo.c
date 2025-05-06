#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include <string.h>
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812b.pio.h"


#define BUTTON_A 5    // GPIO conectado ao Botão A
#define BUTTON_B 6    // GPIO conectado ao Botão B
#define MATRIZ_LED 7  // GPIO conectado ao pino da matriz de LEDs
#define LED_COUNT 25  // DEFINIÇÃO DA QUANTIDADE DE LEDs NA MATRIZ PARA SER SELECIONADO

const uint I2C_SDA = 14; // GPIO conectado ao pino SDA do OLED
const uint I2C_SCL = 15; // GPIO conectado ao pino SCL do OLED

// Definição de pixel GRB
struct pixel_t {
    uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
  };
  typedef struct pixel_t pixel_t;
  typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.
  
  // Declaração do buffer de pixels que formam a matriz.
  npLED_t leds[LED_COUNT];
  
  // Variáveis para uso da máquina PIO.
  PIO np_pio;
  uint sm;
  

void npInit(uint pin) {
    // Cria programa PIO.
    uint offset = pio_add_program(pio0, &ws2812b_program);
    np_pio = pio0;
  
    // Toma posse de uma máquina PIO.
    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0) {
      np_pio = pio1;
      sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
    }
  
    // Inicia programa na máquina PIO obtida.
    ws2812b_program_init(np_pio, sm, offset, pin, 800000.f);
  
    // Limpa buffer de pixels.
    for (uint i = 0; i < LED_COUNT; ++i) {
      leds[i].R = 0;
      leds[i].G = 0;
      leds[i].B = 0;
    }
}


void npWrite() {
    // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
    for (uint i = 0; i < LED_COUNT; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

void npSetLED(int index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < 0 || index >= LED_COUNT) return;
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}


void npClear() {
    for (uint i = 0; i < LED_COUNT; ++i)
      npSetLED(i, 0, 0, 0);
}

void espera_aleatoria() {
    int tempo_espera = (rand() % 7 + 1) * 1000;  // Gera número entre 1 e 7 segundos
    sleep_ms(tempo_espera);

    int indice_led = rand() % LED_COUNT; // Seleciona um LED aleatório (0 a 24)

    // Acende o LED escolhido em vermelho
    npClear();
    npSetLED(indice_led, 255, 0, 0);   // Usa o LED aleatório
    npWrite();
    sleep_ms(100);                     // Mantém aceso por 200ms

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

void init_button(){
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
}

void init_i2c(){
    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();
}

int main() {
    stdio_init_all();
    srand(time_us_64());  // Inicializa a semente do gerador de números aleatórios

    init_button();

    npInit(MATRIZ_LED);
    npClear();
    npWrite();

    init_i2c();
    
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
            int64_t tempo_reacao_micro = absolute_time_diff_us(inicio_reacao, fim_reacao);  // Em microsegundos
            int64_t tempo_reacao = absolute_time_diff_us(inicio_reacao, fim_reacao) / 1000;  // Em milissegundos

            // gpio_put(LED_PIN, 0);  // Desliga o LED

            printf("Tempo de reação em microsegundos: %lld ms\n", tempo_reacao_micro);
            printf("Tempo de reação em milisegundos: %lld ms\n", tempo_reacao);
            printf("Tempo de reação em segundos: %d s\n", tempo_reacao_button(tempo_reacao));

            // Limpa o display OLED
            memset(ssd, 0, ssd1306_buffer_length);

            // Converte valores para string
            char buffer_us[24];
            char buffer_ms[24];
            char buffer_s[24];
            snprintf(buffer_us, sizeof(buffer_us), "Tempo: %lld us", tempo_reacao_micro);
            snprintf(buffer_ms, sizeof(buffer_ms), "Tempo: %lld ms", tempo_reacao);
            snprintf(buffer_s, sizeof(buffer_s), "Tempo: %d s", tempo_reacao_button(tempo_reacao));

            // Exibe as strings no display
            ssd1306_draw_string(ssd, 0, 0, "Reacao medida:");
            ssd1306_draw_string(ssd, 0, 10, buffer_us);
            ssd1306_draw_string(ssd, 0, 30, buffer_ms);
            ssd1306_draw_string(ssd, 0, 50, buffer_s);

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







