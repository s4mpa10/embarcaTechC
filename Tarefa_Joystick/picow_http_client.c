#include <stdio.h> 
#include "pico/stdlib.h"  
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "lwip/altcp_tls.h"
#include "example_http_client_util.h"
#include "hardware/pwm.h" // Biblioteca para controle de PWM no RP2040
#include "hardware/adc.h" // Biblioteca para manipulação do ADC no RP2040

// ======= CONFIGURAÇÕES ======= //
#define HOST "XXX.XXX.XX.XX"     // IP do servidor para envio das requisições
#define PORT 5000                // Porta do servidor
#define DEBOUNCE_MS 50           // Tempo de debounce em milissegundos

// Definição dos pinos usados para o joystick 
const int VRX = 26;          // Pino do eixo X do joystick (ADC)
const int VRY = 27;          // Pino do eixo Y do joystick (ADC)
const int ADC_CHANNEL_0 = 0; // Canal ADC para o eixo X
const int ADC_CHANNEL_1 = 1; // Canal ADC para o eixo Y
const int SW = 22;           // Pino de leitura do botão do joystick
const float DIVIDER_PWM = 16.0;          // Divisor fracional do clock para o PWM
const uint16_t PERIOD = 4096;            // Período do PWM (valor máximo do contador)
// ============================ //


// Função para configurar o joystick (pinos de leitura e ADC)
void setup_joystick()
{
  // Inicializa o ADC e os pinos de entrada analógica
  adc_init();                       // Inicializa o módulo ADC
  adc_gpio_init(VRX);               // Configura o pino VRX (eixo X) para entrada ADC
  adc_gpio_init(VRY);               // Configura o pino VRY (eixo Y) para entrada ADC
  gpio_init(SW);                    // Inicializa o pino do botão
  gpio_set_dir(SW, GPIO_IN);        // Configura o pino do botão como entrada
  gpio_pull_up(SW);                 // Ativa o pull-up no pino do botão para evitar flutuações
}

// Função de configuração geral
void setup() {
  stdio_init_all();     // Inicializa a comunicação serial
  setup_joystick();     // Configura o joystick, chamanda pela função
}

void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value) {
    // Leitura do valor do eixo X do joystick
    adc_select_input(ADC_CHANNEL_0); // Seleciona o canal ADC para o eixo X
    sleep_us(2);                     // Pequeno delay para estabilidade
    *vry_value = 4095 - adc_read();  // Lê valor e inverte para eixo X
  
    // Leitura do valor do eixo Y do joystick
    adc_select_input(ADC_CHANNEL_1); // Seleciona o canal ADC para o eixo Y
    sleep_us(2);                     // Pequeno delay para estabilidade
    *vrx_value = 4095 - adc_read();  // Lê valor e inverte para eixo Y
  }


void enviar_requisicao_http(const char *url) {
    EXAMPLE_HTTP_REQUEST_T req = {0};
    req.hostname = HOST;
    req.url = url;
    req.port = PORT;
    req.headers_fn = http_client_header_print_fn;
    req.recv_fn = http_client_receive_print_fn;

    printf("[%d] Enviando: %s\n", url);
    int result = http_client_request_sync(cyw43_arch_async_context(), &req);

    if (result == 0) {
        printf("Sucesso!\n");
    } else {
        printf("Erro %d - Verifique conexão\n", result);
        cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000);
    }
}

// Função principal
int main() {
    stdio_init_all();       // Inicializa comunicação serial

    printf("\nIniciando cliente HTTP...\n");

    if (cyw43_arch_init()) { // Inicializa Wi-Fi
        printf("Erro ao inicializar Wi-Fi\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();  // Ativa modo estação (STA)

    printf("Conectando a %s...\n", WIFI_SSID);
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Falha na conexão Wi-Fi\n");
        return 1;
    }
    printf("Conectado! IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));

    int counter = 0;        // Contador de requisições
    char url[128];          // Buffer para armazenar a URL
    absolute_time_t last_debounce_time = get_absolute_time();       // Marca tempo para debounce

    uint16_t vrx_value, vry_value, sw_value; // Variáveis para armazenar os valores do joystick (eixos X e Y) e botão
    setup();                                 // Chama a função de configuração do joystick e serial
    
    printf("Joystick-PWM\n");                // Exibe uma mensagem inicial via porta serial
       
    // Loop principal infinito
    while (1) {
        joystick_read_axis(&vrx_value, &vry_value); // Lê os valores dos eixos do joystick
        printf("X: %d Y: %d\n", vrx_value, vry_value); // Exibe os valores lidos via porta serial

        sprintf(url, "/mensagem?msgA=%d&msgB=%d", vrx_value, vry_value);  // Monta URL da requisição para o servidor
        
        enviar_requisicao_http(url); // Envia a requisição HTTP com os valores do joystick

        sleep_ms(100); // Aguarda 100ms antes de enviar a próxima requisição
    }
      
    return 0; // Nunca chegará aqui devido ao while(1)
}

