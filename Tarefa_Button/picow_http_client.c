#include <stdio.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "lwip/altcp_tls.h"
#include "example_http_client_util.h"
#include "hardware/pwm.h"

// ======= CONFIGURAÇÕES ===================================================== //
#define HOST "XXX.XXX.XX.XX" // Endereço IP do servidor
#define PORT 5000           // Porta do servidor
#define INTERVALO_MS 1000   // Intervalo entre mensagens (1 segundo)
#define BUTTON_A_PIN 5      // Pino do botão A (aumentar)
#define BUTTON_B_PIN 6      // Pino do botão B (diminuir)
#define LED_GREEN_PIN 11    // Pino do LED PWM
#define DEBOUNCE_MS 50      // Tempo de debounce em milissegundos
uint16_t duty = 0;          // Duty cycle inicial (nível de brilho do LED)
// ============================================================================ //

// Configura o pino como saída PWM
void setup_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);          // Configura o pino como PWM
    uint slice_num = pwm_gpio_to_slice_num(pin);    // Descobre o slice correspondente
    pwm_set_wrap(slice_num, 65535);                 // Define a resolução (0-65535)
    pwm_set_enabled(slice_num, true);               // Ativa o PWM nesse slice
}

// Define o duty cycle do pino PWM (0-65535)
void set_pwm_duty(uint pin, uint16_t duty) {
    pwm_set_gpio_level(pin, duty);                  
}

// Atualiza o nível de iluminação do LED ligando e desligando também conforme o nível recebido (0 a 4)
int atualizar_iluminacao(int nivel) {
    switch (nivel) {
    case 0:
        // 0 por nível 0
        set_pwm_duty(LED_GREEN_PIN, 0);
        return 0;
    case 1:
        // 16383.75 por nível 1
        set_pwm_duty(LED_GREEN_PIN, 16384);
        return 16384;
    case 2:
        // 32767.5 nível 2
        set_pwm_duty(LED_GREEN_PIN, 32768);
        return 32768;
        break;
    case 3:
        // 49151.25 nível 3
        set_pwm_duty(LED_GREEN_PIN, 49152);
        return 49152;
    case 4:
        // 65535 nível 4
        set_pwm_duty(LED_GREEN_PIN, 65535);
        return 65535;
    }
    return 0;
}

// Inicializa os botões como entrada com pull-up (Button A e B)
void setup_init_button(){
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
}

// Inicializa o LED como saída e o desliga (LED_GREEN_PIN)
void setup_init_led(){
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_put(LED_GREEN_PIN, false);
}

void enviar_requisicao_http(const char *url, int counter) {
    EXAMPLE_HTTP_REQUEST_T req = {0};
    req.hostname = HOST;
    req.url = url;
    req.port = PORT;
    req.headers_fn = http_client_header_print_fn;
    req.recv_fn = http_client_receive_print_fn;

    printf("[%d] Enviando: %s\n", counter, url);
    int result = http_client_request_sync(cyw43_arch_async_context(), &req);

    if (result == 0) {
        printf("Sucesso!\n");
    } else {
        printf("Erro %d - Verifique conexão\n", result);
        cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000);
    }
}

int main() {

    stdio_init_all();           // Inicializa UART/USB para printf
    setup_init_button();        // Inicializa botões
    setup_init_led();           // Inicializa LED
    
    printf("\nIniciando cliente HTTP...\n");

    // Inicializa Wi-Fi
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

    int counter = 0;                                                // Contador de mensagens para gerenciar quantas mensagens foram enviadas
    char url[128];                                                  // Buffer para URL da requisição
    bool last_button_state_A = true;                                // Estado anterior do botão A
    bool last_button_state_B = true;                                // Estado anterior do botão B   
    absolute_time_t last_debounce_time_A = get_absolute_time();
    absolute_time_t last_debounce_time_B = get_absolute_time();

    setup_pwm(LED_GREEN_PIN);                                       // Configura PWM no LED

    while(1) {
        bool current_button_state_A = gpio_get(BUTTON_A_PIN);
        bool current_button_state_B = gpio_get(BUTTON_B_PIN);

        // Verifica se o estado do botão A mudou e se o tempo de debounce passou
        if (current_button_state_A != last_button_state_A) {
            last_debounce_time_A = get_absolute_time(); // Atualiza o tempo de debounce
        }

        // ========== BOTÃO A (Incrementar Nível) ==========
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
                    sprintf(url, "/mensagem?msg=Nivel_%d_%s", duty, status); // Envia a URL com o nível e status para o servidor
                }
            } else { // Botão solto, atualiza a URL para indicar que o botão foi solto
                char status[8] = {0};  // Zera o array
                strcpy(status, "A_Solto");  // Copia a string para o array
                sprintf(url, "/mensagem?msg=Nivel_%d_%s", duty, status);
            }

            // Configura o envio da requisição HTTP
            enviar_requisicao_http(url, counter);
            // Envia requisição
        }

        last_button_state_A  = current_button_state_A; // Atualiza o estado anterior do botão


        // ========== BOTÃO B (Diminuir Nível) ==========
        // Verifica se o estado do botão B mudou e se o tempo de debounce passou
        if (current_button_state_B != last_button_state_B) {
            last_debounce_time_B = get_absolute_time(); // Atualiza o tempo de debounce
        }

        // Verifica se o tempo de debounce passou
        if (absolute_time_diff_us(last_debounce_time_B, get_absolute_time()) > DEBOUNCE_MS * 1000) {
            // Apenas mude o estado se o botão estiver realmente pressionado
            if (current_button_state_B == 0) { // Botão pressionado B 
                if (duty > 0) {
                    duty--;
                    int valor = atualizar_iluminacao(duty);
                    char status[15] = {0};  // Zera o array
                    strcpy(status, "B_Pressionado");  // Copia a string para o array
                    printf("Diminuindo nível: %d - (%d)\n", duty, valor);
                    sprintf(url, "/mensagem?msg=Nivel_%d_%s", duty, status); // Envia a URL com o nível e status para o servidor
                }
            } else { // Botão solto, atualiza a URL para indicar que o botão foi solto
                char status[8] = {0};  // Zera o array
                strcpy(status, "B_Solto");  // Copia a string para o array
                sprintf(url, "/mensagem?msg=Nivel_%d_%s", duty, status);
            }

            // Configura o envio da requisição HTTP
            enviar_requisicao_http(url, counter);
            // Envia requisição
        }

        last_button_state_B  = current_button_state_B;  // Atualiza o estado anterior do botão

        sleep_ms(INTERVALO_MS);         // Aguarda próximo loop, esperando 1 segundo
        counter++;                      // Incrementa contador de mensagens 
        if (counter > 1000) {           // Reseta contador após 1000 mensagens
            printf("Contador resetado\n");
            counter = 0;
        }
    }

    return 0; // Nunca atinge este ponto
}