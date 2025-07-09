#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "inc/ssd1306.h"

// Definições dos pinos (conforme seu código original)
#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13

// BH1750 - usando i2c0
#define I2C_BH1750_PORT i2c0
#define I2C_BH1750_SDA 0  // GP0
#define I2C_BH1750_SCL 1  // GP1

// OLED - usando i2c1
const uint I2C_SDA = 14;  // GP14
const uint I2C_SCL = 15;  // GP15

// Configuração PWM
const uint16_t periodo = 200;
const float divisor_pwm = 16.0;
const uint16_t stemp_pwm = 40;
uint8_t nivelLuminosidade = 0;
uint8_t contadorA = 0;

// BH1750
#define BH1750_ADDR 0x23
#define BH1750_CONTINUOUS_HIGH_RES_MODE 0x10

// Variáveis globais para o OLED
struct render_area frame_area;
uint8_t ssd[ssd1306_buffer_length];

void config_pwm(int led) {
    uint slice;
    gpio_set_function(led, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(led);
    pwm_set_clkdiv(slice, divisor_pwm);
    pwm_set_wrap(slice, periodo);
    pwm_set_gpio_level(led, stemp_pwm);
    pwm_set_enabled(slice, true);
}

void bh1750_init() {
    uint8_t buf[1] = {BH1750_CONTINUOUS_HIGH_RES_MODE};
    i2c_write_blocking(I2C_BH1750_PORT, BH1750_ADDR, buf, 1, false);
}

float bh1750_read_lux() {
    uint8_t data[2];
    int result = i2c_read_blocking(I2C_BH1750_PORT, BH1750_ADDR, data, 2, false);
    if (result != 2) {
        printf("Erro ao ler o sensor BH1750\n");
        return -1;
    }
    uint16_t raw = (data[0] << 8) | data[1];
    return raw / 1.2;
}

void exibir_mensagem(char *mensagem) {
    memset(ssd, 0, ssd1306_buffer_length);

    int length = strlen(mensagem);

    int char_width = 6;

    int total_width = length * char_width;

    int start_x = (ssd1306_width - total_width) / 2;
    if (start_x < 0) start_x = 0;

    ssd1306_draw_string(ssd, start_x, 0, mensagem);
    
    render_on_display(ssd, &frame_area);

}

void acender_led(float red, float green, float blue) {
    pwm_set_gpio_level(LED_RED, red);
    pwm_set_gpio_level(LED_GREEN, green);
    pwm_set_gpio_level(LED_BLUE, blue);
}   

void atualizar_display_por_lux(float lux) {
    char mensagem[50];
    
    if (lux < 100) {
        snprintf(mensagem, sizeof(mensagem), "%.0f lx", lux);
        printf("Nível 1 - Luminosidade extremamente baixa: %.1f lx\n",lux);
        acender_led(0,0,0);
    } 
    else if (lux >= 100 && lux < 250) {
        snprintf(mensagem, sizeof(mensagem), "%.0f lx", lux);
        printf("Nível 2 - Luminosidade baixa: %.1f lx\n",lux);
        acender_led(0,periodo * 0.2,0);
    } 
    else if (lux >= 250 && lux < 400) {
        snprintf(mensagem, sizeof(mensagem), "%.0f lx", lux);
        printf("Niível 3 - Luminosidade Intermediário: %.1f lx\n",lux);
        acender_led(0,periodo * 0.5,periodo * 0.2);
    } 
    else if (lux >= 400 && lux < 600) {
        snprintf(mensagem, sizeof(mensagem), "%.0f lx", lux);
        printf("Nível 4 - Luminosidade boa: %.1f lx\n",lux);
        acender_led(periodo * 0.2, periodo, periodo * 0.5);
    } 
    else {
        snprintf(mensagem, sizeof(mensagem), "%.0f lx", lux);
        printf("Nível 5 - Luminosidade extrema: %.1f lx\n",lux);
        acender_led(periodo,periodo,periodo);
    }
    
    exibir_mensagem(mensagem);
}

int main() {
    stdio_init_all();
    
    // Inicializa I2C0 para BH1750 (GP0 e GP1)
    i2c_init(I2C_BH1750_PORT, 100 * 1000);
    gpio_set_function(I2C_BH1750_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_BH1750_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_BH1750_SDA);
    gpio_pull_up(I2C_BH1750_SCL);
    
    // Inicializa I2C1 para OLED (GP14 e GP15)
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    // Inicializa OLED
    ssd1306_init();
    frame_area.start_column = 0;
    frame_area.end_column = ssd1306_width - 1;
    frame_area.start_page = 0;
    frame_area.end_page = ssd1306_n_pages - 1;
    calculate_render_area_buffer_length(&frame_area);
    memset(ssd, 0, ssd1306_buffer_length);
    
    // Inicializa BH1750
    bh1750_init();
    
    // Configura LEDs PWM
    config_pwm(LED_GREEN);
    config_pwm(LED_BLUE);
    config_pwm(LED_RED);

    exibir_mensagem("Sistema Iniciado");
    sleep_ms(2000);
        
    while (true) {
        float lux = bh1750_read_lux();
        
        if (lux >= 0) {
            atualizar_display_por_lux(lux);
        } else {
            char msg[30];
            exibir_mensagem(msg);
        }
        
        sleep_ms(100);
    }
    
    return 0;
}