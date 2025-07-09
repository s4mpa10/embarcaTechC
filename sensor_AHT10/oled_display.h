#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

// --- Funções Públicas ---
// Inicializa a comunicação I2C e o display OLED.
void oled_init();

// Limpa todo o conteúdo do display.
void oled_clear();

// Escreve uma mensagem em uma linha específica (0-7).
void oled_draw_string(int x, int y, const char *message);

// Envia o buffer de imagem para ser exibido no display.
void oled_render();

#endif // OLED_DISPLAY_H