#include "display.h"
#include <stdio.h>
#include <string.h>
#include "inc/ssd1306.h"

// --- Definições de Pinos e I2C para o Display OLED ---
#define OLED_I2C_PORT        i2c1
#define OLED_SDA_PIN         14
#define OLED_SCL_PIN         15
#define OLED_I2C_BAUDRATE    400000

// --- Variáveis Globais para o OLED ---
static struct render_area frame_area;
static uint8_t ssd_buffer[ssd1306_buffer_length];

void init_oled() {
    i2c_init(OLED_I2C_PORT, OLED_I2C_BAUDRATE);
    gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA_PIN);
    gpio_pull_up(OLED_SCL_PIN);

    ssd1306_init();
    frame_area.start_column = 0;
    frame_area.end_column = ssd1306_width - 1;
    frame_area.start_page = 0;
    frame_area.end_page = ssd1306_n_pages - 1;
    calculate_render_area_buffer_length(&frame_area);
    memset(ssd_buffer, 0, ssd1306_buffer_length);

    sleep_ms(100);
    printf("Display OLED inicializado no I2C1.\\n");
}

void clear_oled_display() {
    memset(ssd_buffer, 0, ssd1306_buffer_length);
}

void display_message_oled(const char *message, int line) {
    ssd1306_draw_string(ssd_buffer, 5, line * 8, (char *)message);
}

void display_angles_oled(float pitch, float roll, const char *alerta_str) {
    char pitch_str[32];
    char roll_str[32];

    snprintf(pitch_str, sizeof(pitch_str), "Pitch: %.1f deg", pitch);
    snprintf(roll_str, sizeof(roll_str), "Roll:  %.1f deg", roll);

    clear_oled_display();

    display_message_oled(pitch_str, 0);
    display_message_oled(roll_str, 2);

    if (alerta_str[0] != '\\0') {
        display_message_oled(alerta_str, 4);
    }
    
    render_on_display(ssd_buffer, &frame_area);
}