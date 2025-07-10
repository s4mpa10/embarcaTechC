#ifndef DISPLAY_H
#define DISPLAY_H

#include "pico/stdlib.h"

// --- Protótipos de Funções ---
void init_oled();
void clear_oled_display();
void display_message_oled(const char *message, int line);
void display_angles_oled(float pitch, float roll, const char *alerta_str);

#endif // DISPLAY_H