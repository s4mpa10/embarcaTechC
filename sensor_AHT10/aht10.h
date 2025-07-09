#ifndef AHT10_H
#define AHT10_H

#include "pico/stdlib.h"
#include <stdbool.h>

// --- Funções Públicas ---
// Inicializa a comunicação I2C e o sensor AHT10.
void aht10_init();

// Tenta ler os dados de umidade e temperatura do sensor.
// Retorna 'true' em caso de sucesso e 'false' em caso de falha.
bool aht10_read_data(float *humidity, float *temperature);

// Envia um comando de soft reset para o sensor.
void aht10_reset();

#endif // AHT10_H