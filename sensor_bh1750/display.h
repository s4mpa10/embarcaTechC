#ifndef DISPLAY_H
#define DISPLAY_H

#include "pico/stdlib.h"

// Funções públicas
void display_init();
void exibir_mensagem(char *mensagem);
void atualizar_display_por_lux(float lux);

#endif // DISPLAY_H