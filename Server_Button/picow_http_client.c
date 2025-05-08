#include <stdio.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "lwip/altcp_tls.h"
#include "example_http_client_util.h"
#include "hardware/pwm.h"

// ======= CONFIGURAÇÕES ======= //
#define HOST "172.20.10.5"
#define PORT 5000
#define INTERVALO_MS 1000    // Intervalo entre mensagens (1 segundo)
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define LED_GREEN_PIN 11
#define DEBOUNCE_MS 50       // Tempo de debounce em milissegundos
uint16_t duty = 0;
// ============================= //

void setup_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);          // Configura o pino como PWM
    uint slice_num = pwm_gpio_to_slice_num(pin);    // Descobre o slice correspondente
    pwm_set_wrap(slice_num, 65535);                 // Define a resolução (0-65535)
    pwm_set_enabled(slice_num, true);               // Ativa o PWM nesse slice
}

void set_pwm_duty(uint pin, uint16_t duty) {
    pwm_set_gpio_level(pin, duty);                  // Define o duty cycle do pino (0-65535)
}

int atualizar_iluminacao(int nivel) {
    // Liga ou desliga o LED externo de acordo com o nível
    switch (nivel) {
    case 0:
        set_pwm_duty(LED_GREEN_PIN, 0);
        return 0;
        break;
    case 1:
        // 16383.75 por nível 1
        set_pwm_duty(LED_GREEN_PIN, 16384);
        return 16384;
        break;
    case 2:
        // 32767.5 nível 2
        set_pwm_duty(LED_GREEN_PIN, 32768);
        return 32768;
        break;
    case 3:
        // 49151.25 nível 3
        set_pwm_duty(LED_GREEN_PIN, 49152);
        return 49152;
        break;
    case 4:
        // 65535 nível 4
        set_pwm_duty(LED_GREEN_PIN, 65535);
        return 65535;
        break;
    }
    return 0;
}

void setup_init_button(){
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
}

void setup_init_led(){
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_put(LED_GREEN_PIN, false);
}

int main() {

    stdio_init_all();

    setup_init_button();
    setup_init_led();
    
    printf("\nIniciando cliente HTTP...\n");

    if (cyw43_arch_init()) {
        printf("Erro ao inicializar Wi-Fi\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    printf("Conectando a %s...\n", WIFI_SSID);
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Falha na conexão Wi-Fi\n");
        return 1;
    }
    printf("Conectado! IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));

    int counter = 0;
    char url[128];
    // bool last_button_state = true; // Estado anterior do botão
    // absolute_time_t last_debounce_time = get_absolute_time(); // Último tempo de debounce
    bool last_button_state_A = true;
    bool last_button_state_B = true;
    absolute_time_t last_debounce_time_A = get_absolute_time();
    absolute_time_t last_debounce_time_B = get_absolute_time();

    setup_pwm(LED_GREEN_PIN);

    while(1) {
        bool current_button_state_A = gpio_get(BUTTON_A_PIN);
        bool current_button_state_B = gpio_get(BUTTON_B_PIN);

        // Verifica se o estado do botão A mudou e se o tempo de debounce passou
        if (current_button_state_A != last_button_state_A) {
            last_debounce_time_A = get_absolute_time(); // Atualiza o tempo de debounce
        }

        // Verifica se o tempo de debounce passou
        if (absolute_time_diff_us(last_debounce_time_A, get_absolute_time()) > DEBOUNCE_MS * 1000) {
            // Apenas mude o estado se o botão estiver realmente pressionado
            if (current_button_state_A == 0) { // Botão pressionado A 
                if (duty < 4) {
                    duty++;
                    int valor = atualizar_iluminacao(duty);
                    char status[15] = {0};  // Zera o array
                    strcpy(status, "A_Pressionado");  // Copia a string para o array
                    printf("Aumentando nível: %d - (%d)\n", duty, valor);
                    sprintf(url, "/mensagem?msg=Nivel_%d_%s", duty, status);
                }
            } else {
                // Botão solto, atualiza a URL para indicar que o botão foi solto
                char status[8] = {0};  // Zera o array
                strcpy(status, "A_Solto");  // Copia a string para o array
                sprintf(url, "/mensagem?msg=Nivel_%d_%s", duty, status);
            }

            // Configura requisição
            EXAMPLE_HTTP_REQUEST_T req = {0};
            req.hostname = HOST;
            req.url = url;
            req.port = PORT;
            req.headers_fn = http_client_header_print_fn;
            req.recv_fn = http_client_receive_print_fn;

            // Envia requisição
            printf("[%d] Enviando: %s\n", counter, url);
            int result = http_client_request_sync(cyw43_arch_async_context(), &req);

            // Verifica resultado
            if (result == 0) {
                printf("Sucesso!\n");
            } else {
                printf("Erro %d - Verifique conexão\n", result);
                cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000);
            }
        }

        last_button_state_A  = current_button_state_A; // Atualiza o estado anterior do botão


        // Verifica se o estado do botão A mudou e se o tempo de debounce passou
        if (current_button_state_B != last_button_state_B) {
            last_debounce_time_B = get_absolute_time(); // Atualiza o tempo de debounce
        }

        // Verifica se o tempo de debounce passou
        if (absolute_time_diff_us(last_debounce_time_B, get_absolute_time()) > DEBOUNCE_MS * 1000) {
            // Apenas mude o estado se o botão estiver realmente pressionado
            if (current_button_state_B == 0) { // Botão pressionado A 
                if (duty > 0) {
                    duty--;
                    int valor = atualizar_iluminacao(duty);
                    char status[15] = {0};  // Zera o array
                    strcpy(status, "B_Pressionado");  // Copia a string para o array
                    printf("Diminuindo nível: %d - (%d)\n", duty, valor);
                    sprintf(url, "/mensagem?msg=Nivel_%d_%s", duty, status);
                }
            } else {
                // Botão solto, atualiza a URL para indicar que o botão foi solto
                char status[8] = {0};  // Zera o array
                strcpy(status, "B_Solto");  // Copia a string para o array
                sprintf(url, "/mensagem?msg=Nivel_%d_%s", duty, status);
            }

            // Configura requisição
            EXAMPLE_HTTP_REQUEST_T req = {0};
            req.hostname = HOST;
            req.url = url;
            req.port = PORT;
            req.headers_fn = http_client_header_print_fn;
            req.recv_fn = http_client_receive_print_fn;

            // Envia requisição
            printf("[%d] Enviando: %s\n", counter, url);
            int result = http_client_request_sync(cyw43_arch_async_context(), &req);

            // Verifica resultado
            if (result == 0) {
                printf("Sucesso!\n");
            } else {
                printf("Erro %d - Verifique conexão\n", result);
                cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000);
            }
        }

        last_button_state_B  = current_button_state_B; // Atualiza o estado anterior do botão

        sleep_ms(INTERVALO_MS);
    }

    return 0; // Nunca chegará aqui devido ao while(1)
}