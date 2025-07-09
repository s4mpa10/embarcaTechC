#include "oled_display.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "inc/ssd1306.h" // A biblioteca base do display
#include <stdio.h>
#include <string.h>

// --- Constantes Privadas ---
#define OLED_I2C_PORT i2c1
#define OLED_SDA_PIN 14
#define OLED_SCL_PIN 15
#define OLED_I2C_BAUDRATE 400000

// --- Variáveis Privadas (encapsuladas neste módulo) ---
static struct render_area frame_area;
static uint8_t ssd_buffer[ssd1306_buffer_length];

// --- Implementação das Funções ---

void oled_init() {
    i2c_init(OLED_I2C_PORT, OLED_I2C_BAUDRATE);
    gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA_PIN);
    gpio_pull_up(OLED_SCL_PIN);

    ssd1306_init(); // Função da biblioteca base
    
    // Configura a área de renderização para a tela inteira
    frame_area.start_column = 0;
    frame_area.end_column = ssd1306_width - 1;
    frame_area.start_page = 0;
    frame_area.end_page = ssd1306_n_pages - 1;
    calculate_render_area_buffer_length(&frame_area);

    oled_clear();
    oled_render();

    sleep_ms(100);
    printf("Display OLED inicializado no I2C1.\n");
}

void oled_clear() {
    memset(ssd_buffer, 0, ssd1306_buffer_length);
}

void oled_draw_string(int x, int y, const char *message) {
    // ssd1306_draw_string é da biblioteca base
    ssd1306_draw_string(ssd_buffer, x, y, message);
}

void oled_render() {
    // render_on_display é da biblioteca base
    render_on_display(ssd_buffer, &frame_area);
}