#include <stdio.h>
#include "pico/stdlib.h"

// Inclui as interfaces dos nossos módulos
#include "aht10.h"
#include "oled_display.h"
#include "buzzer.h"
#include "ledRGB.h"

int main() {
    stdio_init_all();
    sleep_ms(1000); // Pequena pausa para o terminal serial conectar

    printf("Iniciando sistema com AHT10 e OLED...\n");

    // Inicializa os módulos
    oled_init();
    aht10_init();
    buzzer_init(BUZZER_PIN);
    ledrgb_init(LED_RED_PIN,GPIO_OUT);
    ledrgb_set_color(LED_RED_PIN, 0);


    // Mensagem de boas-vindas no OLED
    oled_clear();
    oled_draw_string(5, 8, "Iniciando...");
    oled_draw_string(5, 24, "AHT10 & OLED");
    oled_render();
    sleep_ms(2000);

    float humidity, temperature;
    char temp_str[20];
    char hum_str[20];

    char aviso_temp_str[20];
    char aviso_hum_str[20];

    while (true) {
        if (aht10_read_data(&humidity, &temperature)) {
            printf("Umidade: %.2f %%RH, Temperatura: %.2f C\n", humidity, temperature);
            
            // Prepara as strings para exibição
            snprintf(temp_str, sizeof(temp_str), "Temp: %.1f C", temperature);
            snprintf(hum_str, sizeof(hum_str), "Umid: %.1f %%", humidity);
            
            // Exibe no OLED
            oled_clear();
            oled_draw_string(5, 8, temp_str);
            oled_draw_string(5, 24, hum_str);
            oled_render();


            if (humidity > 70){
                printf("SOS - Umidade: %.2f %%RH\n", humidity);

                snprintf(aviso_hum_str, sizeof(aviso_hum_str), "SOS-Umid:%.1f %%", humidity);

                buzzer_beep(BUZZER_PIN,DEFAULT_BEEP_DURATION_MS,50);

                oled_clear();
                oled_draw_string(5, 8, aviso_hum_str);
                oled_render();

            }

            if(temperature > 20){
                printf("SOS - Temperatura: %.2f C\n", temperature);

                snprintf(aviso_temp_str, sizeof(aviso_temp_str), "SOS-Temp:%.1f C", temperature);

                buzzer_beep(BUZZER_PIN,DEFAULT_BEEP_DURATION_MS,50);

                oled_clear();
                oled_draw_string(5, 8, aviso_temp_str);
                oled_render();

            }
            
        } else {
            printf("Falha na leitura do AHT10. Tentando resetar...\n");
            
            // Exibe mensagem de erro no OLED
            oled_clear();
            oled_draw_string(5, 16, "Erro no Sensor!");
            oled_render();
            
            aht10_reset(); // Tenta recuperar o sensor
            sleep_ms(500);
        }
        sleep_ms(3000); // 3 segundos entre uma leitura e a nova leitura
    }

    return 0;
}