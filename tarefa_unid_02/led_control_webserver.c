#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "hardware/pwm.h"

#define WIFI_SSID "iPhone"
#define WIFI_PASSWORD "senha123"

#define LED_PIN CYW43_WL_GPIO_LED_PIN
#define LED_GREEN_PIN 11
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

uint16_t duty = 0;

void setup_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);          // Configura o pino como PWM
    uint slice_num = pwm_gpio_to_slice_num(pin);    // Descobre o slice correspondente
    pwm_set_wrap(slice_num, 65535);                 // Define a resolução (0-65535)
    pwm_set_enabled(slice_num, true);               // Ativa o PWM nesse slice
}

void set_pwm_duty(uint pin, uint16_t duty) {
    pwm_set_gpio_level(pin, duty);                  // Define o duty cycle do pino (0-65535)
}

// int definir_nivel_luz(int valor) {
//     if (valor == 0) return 0;
//     else if (valor == 1) return 16384; // Quando o duty é 16383.75, o valor é 16384
//     else if (valor == 2) return 32768;
//     else if (valor == 3) return 49152;
//     else if (valor == 4) return 65535;
//     return 0;
// }

int atualizar_iluminacao(int nivel)
{
    // Liga ou desliga o LED externo de acordo com o nível
    switch (nivel)
    {
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

// Função de callback da conexão TCP
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    char *request = (char *)malloc(p->len + 1);
    memcpy(request, p->payload, p->len);
    request[p->len] = '\0';

    printf("Request: %s\n", request);

    if (strncmp(request, "GET /status", 11) == 0) {
        char status_response[32];
        snprintf(status_response, sizeof(status_response),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/plain\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "%d", duty);

        tcp_write(tpcb, status_response, strlen(status_response), TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);

        free(request);
        pbuf_free(p);
        return ERR_OK;
    }

    // Página HTML principal
    char html[2048];
    snprintf(html, sizeof(html),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Connection: close\r\n"
             "\r\n"
             "<!DOCTYPE html>\n"
             "<html>\n"
             "<head>\n"
             "<title>Nivel de Iluminacao</title>\n"
             "<style>\n"
             "body { font-family: Arial; text-align: center; margin-top: 50px; }\n"
             "h1 { font-size: 48px; }\n"
             "p { font-size: 36px; }\n"
             "button { font-size: 24px; padding: 10px 20px; margin: 10px; }\n"
             "</style>\n"
             "<script>\n"
             "let intervalo;\n"
             "function atualizarNivel() {\n"
             "  fetch('/status')\n"
             "    .then(response => response.text())\n"
             "    .then(data => {\n"
             "      document.getElementById('nivel').textContent = data;\n"
             "    });\n"
             "}\n"
             "document.addEventListener('visibilitychange', function() {\n"
             "  if (document.visibilityState === 'visible') {\n"
             "    intervalo = setInterval(atualizarNivel, 300);\n"
             "  } else {\n"
             "    clearInterval(intervalo);\n"
             "  }\n"
             "});\n"
             "window.onload = () => { atualizarNivel(); intervalo = setInterval(atualizarNivel, 300); }\n"
             "</script>\n"
             "</head>\n"
             "<body>\n"
             "<h1>Nivel de Iluminacao</h1>\n"
             "<p>Nivel Atual: <span id='nivel'>%d</span></p>\n"
             "</body>\n"
             "</html>\n", duty);

    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);

    free(request);
    pbuf_free(p);
    return ERR_OK;
}


static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, tcp_server_recv);
    return ERR_OK;
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


int inicializa_wifi_e_servidor(){
    if (cyw43_arch_init())
    {
        printf("Falha ao inicializar Wi-Fi\n");
        return -1;
    }
    cyw43_arch_enable_sta_mode();

    printf("Conectando ao Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000))
    {
        printf("Falha ao conectar\n");
        return -1;
    }

    printf("Conectado! IP: %s\n", ipaddr_ntoa(&netif_default->ip_addr));

    struct tcp_pcb *server = tcp_new();
    if (!server)
    {
        printf("Erro ao criar servidor TCP\n");
        return -1;
    }

    if (tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK)
    {
        printf("Falha ao associar porta 80\n");
        return -1;
    }

    server = tcp_listen(server);
    tcp_accept(server, tcp_server_accept);
    printf("Servidor ouvindo na porta 80\n");

    return 0;
}


// Para utilizar é necessário definir a variavel duty: uint16_t duty = 0; (Que vai de 0 a 65535). 

// Depois de definir a variável duty, é necessário utilizar a função set_pwm_duty(RED_PIN(DEFINIR O PINO DO LED), duty(VALOR DE BRILHO)); 

// 16.383,75 nivel 1 
// 32767.5 nível 2
// 49151.25 nível 3
// 65535 nível 4

int main() {
    stdio_init_all();

    setup_init_button();

    setup_init_led();

    inicializa_wifi_e_servidor();

    printf("Iniciando o monitoramento dos botões...\n");
    printf("Pressione o botão A para aumentar o nível e o botão B para diminuir\n");

    setup_pwm(LED_GREEN_PIN);

    while (true)
    {
        cyw43_arch_poll();

        // Botão A aumenta
        if (gpio_get(BUTTON_A_PIN) == 0)
        {
            if (duty < 4) {
                duty++;
                int valor = atualizar_iluminacao(duty);
                printf("Aumentando nível: %d - (%d)\n", duty, valor);
            }
        }

        // Botão B diminui
        if (gpio_get(BUTTON_B_PIN) == 0)
        {
            if (duty > 0) {
                duty--;
                int valor = atualizar_iluminacao(duty);
                printf("Diminuindo nível: %d - (%d)\n", duty, valor);
            }
        }

        printf("Nível Atual: %d\n", duty);

        sleep_ms(1000); // Espera 1 segundo para ler novamente os botões para evitar múltiplas leituras rápidas
    }

    cyw43_arch_deinit();
    return 0;
}