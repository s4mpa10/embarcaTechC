#include "display.h"
#include "inc/ssd1306.h"
#include <string.h>
#include <stdio.h>

// Definições do OLED
const uint I2C_SDA_OLED = 14;  // GP14
const uint I2C_SCL_OLED = 15;  // GP15

// Variáveis globais para o OLED
struct render_area frame_area;
uint8_t ssd[ssd1306_buffer_length];

void display_init() {
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(I2C_SDA_OLED, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_OLED, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_OLED);
    gpio_pull_up(I2C_SCL_OLED);

    ssd1306_init();
    frame_area.start_column = 0;
    frame_area.end_column = ssd1306_width - 1;
    frame_area.start_page = 0;
    frame_area.end_page = ssd1306_n_pages - 1;
    calculate_render_area_buffer_length(&frame_area);
    memset(ssd, 0, ssd1306_buffer_length);
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