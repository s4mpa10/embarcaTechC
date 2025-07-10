#include "pico/stdlib.h"
#include "bh1750.h"
#include "rgb_led.h"
#include "display.h"
#include <stdio.h>

// Definido em rgb_led.c, mas usado aqui para os níveis de brilho
extern const uint16_t periodo;

void atualizar_sistema(float lux) {
    char mensagem[50];
    
    if (lux < 100) {
        snprintf(mensagem, sizeof(mensagem), "%.0f lx", lux);
        printf("Nível 1 - Luminosidade extremamente baixa: %.1f lx\n", lux);
        acender_led(0, 0, 0);
    } 
    else if (lux >= 100 && lux < 250) {
        snprintf(mensagem, sizeof(mensagem), "%.0f lx", lux);
        printf("Nível 2 - Luminosidade baixa: %.1f lx\n", lux);
        acender_led(0, periodo * 0.2, 0);
    } 
    else if (lux >= 250 && lux < 400) {
        snprintf(mensagem, sizeof(mensagem), "%.0f lx", lux);
        printf("Nível 3 - Luminosidade Intermediário: %.1f lx\n", lux);
        acender_led(0, periodo * 0.5, periodo * 0.2);
    } 
    else if (lux >= 400 && lux < 600) {
        snprintf(mensagem, sizeof(mensagem), "%.0f lx", lux);
        printf("Nível 4 - Luminosidade boa: %.1f lx\n", lux);
        acender_led(periodo * 0.2, periodo, periodo * 0.5);
    } 
    else {
        snprintf(mensagem, sizeof(mensagem), "%.0f lx", lux);
        printf("Nível 5 - Luminosidade extrema: %.1f lx\n", lux);
        acender_led(periodo, periodo, periodo);
    }
    
    exibir_mensagem(mensagem);
}


int main() {
    stdio_init_all();
    
    bh1750_init();
    display_init();
    rgb_led_init();

    exibir_mensagem("Sistema Iniciado");
    sleep_ms(2000);
        
    while (true) {
        float lux = bh1750_read_lux();
        
        if (lux >= 0) {
            atualizar_sistema(lux);
        } else {
            exibir_mensagem("Erro no sensor");
        }
        
        sleep_ms(100);
    }
    
    return 0;
}