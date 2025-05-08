#include <stdio.h> // Biblioteca padrão de entrada e saída
#include "pico/stdlib.h"  // Biblioteca padrão do Raspberry Pi Pico
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "lwip/altcp_tls.h"
#include "example_http_client_util.h"
#include "hardware/pwm.h" // Biblioteca para controle de PWM no RP2040
#include "hardware/adc.h" // Biblioteca para manipulação do ADC no RP2040

// ======= CONFIGURAÇÕES ======= //
#define HOST "192.168.18.93"
#define PORT 5000
#define DEBOUNCE_MS 50       // Tempo de debounce em milissegundos
// Definição dos pinos usados para o joystick 
const int VRX = 26;          // Pino de leitura do eixo X do joystick (conectado ao ADC)
const int VRY = 27;          // Pino de leitura do eixo Y do joystick (conectado ao ADC)
const int ADC_CHANNEL_0 = 0; // Canal ADC para o eixo X do joystick
const int ADC_CHANNEL_1 = 1; // Canal ADC para o eixo Y do joystick
const int SW = 22;           // Pino de leitura do botão do joystick
const float DIVIDER_PWM = 16.0;          // Divisor fracional do clock para o PWM
const uint16_t PERIOD = 4096;            // Período do PWM (valor máximo do contador)
// ============================ //


// Função para configurar o joystick (pinos de leitura e ADC)
void setup_joystick()
{
  // Inicializa o ADC e os pinos de entrada analógica
  adc_init();         // Inicializa o módulo ADC
  adc_gpio_init(VRX); // Configura o pino VRX (eixo X) para entrada ADC
  adc_gpio_init(VRY); // Configura o pino VRY (eixo Y) para entrada ADC

  // Inicializa o pino do botão do joystick
  gpio_init(SW);             // Inicializa o pino do botão
  gpio_set_dir(SW, GPIO_IN); // Configura o pino do botão como entrada
  gpio_pull_up(SW);          // Ativa o pull-up no pino do botão para evitar flutuações
}

// Função de configuração geral
void setup()
{
  stdio_init_all();                                // Inicializa a porta serial para saída de dados
  setup_joystick();                                // Chama a função de configuração do joystick
}

// Função para ler os valores dos eixos do joystick (X e Y)
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value)
{
  // Leitura do valor do eixo X do joystick
  adc_select_input(ADC_CHANNEL_0); // Seleciona o canal ADC para o eixo X
  sleep_us(2);                     // Pequeno delay para estabilidade
  *vrx_value = adc_read();         // Lê o valor do eixo X (0-4095)

  // Leitura do valor do eixo Y do joystick
  adc_select_input(ADC_CHANNEL_1); // Seleciona o canal ADC para o eixo Y
  sleep_us(2);                     // Pequeno delay para estabilidade
  *vry_value = adc_read();         // Lê o valor do eixo Y (0-4095)
}


// Função principal
int main() {
    stdio_init_all();

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
    bool last_button_state = true;
    absolute_time_t last_debounce_time = get_absolute_time();

    uint16_t vrx_value, vry_value, sw_value; // Variáveis para armazenar os valores do joystick (eixos X e Y) e botão
    setup();                                 // Chama a função de configuração
    printf("Joystick-PWM\n");                // Exibe uma mensagem inicial via porta serial

    int duty = 0;
       
    // Loop principal
    while (1) {
        joystick_read_axis(&vrx_value, &vry_value); // Lê os valores dos eixos do joystick
        printf("X: %d Y: %d\n", vrx_value, vry_value); // Exibe os valores lidos via porta serial

        // Pequeno delay antes da próxima leitura
        sleep_ms(100); // Espera 100 ms antes de repetir o ciclo

        bool current_button_state = gpio_get(SW); // Lê o estado atual do botão do joystick (pressionado ou não)

        // Verifica se o estado do botão A mudou e se o tempo de debounce passou
        if (current_button_state != last_button_state) {
            last_debounce_time = get_absolute_time(); // Atualiza o tempo de debounce
        }

        // Verifica se o tempo de debounce passou
        if (absolute_time_diff_us(last_debounce_time, get_absolute_time()) > DEBOUNCE_MS * 1000) {
            // Apenas mude o estado se o botão estiver realmente pressionado
            if (current_button_state == 0) { // Botão pressionado A 
                if (0 == 0) {
                    char status[15] = {0};  // Zera o array
                    strcpy(status, "A_Pressionado");  // Copia a string para o array
                    printf("Aumentando nível: %d - (%d)\n", duty);
                    // sprintf(url, "/mensagem?msgA=Nivel_%d_%s", duty, status);
                    sprintf(url, "/mensagem?msgA=%d&msgB=%d", vrx_value, vry_value);
                }
            } else {
                // Botão solto, atualiza a URL para indicar que o botão foi solto
                char status[8] = {0};  // Zera o array
                strcpy(status, "A_Solto");  // Copia a string para o array
                sprintf(url, "/mensagem?msgA=%d&msgB=%d", vrx_value, vry_value);
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

        last_button_state  = current_button_state; // Atualiza o estado anterior do botão
    
    }

    return 0; // Nunca chegará aqui devido ao while(1)
}
