#include "aht10.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <stdio.h>

// --- Constantes Privadas do Módulo ---
#define AHT10_ADDR 0x38
#define AHT10_CMD_INITIALIZE 0xE1
#define AHT10_CMD_MEASURE 0xAC
#define AHT10_CMD_SOFT_RESET 0xBA
#define AHT10_STATUS_BUSY_MASK 0x80
#define AHT10_STATUS_CAL_MASK 0x08

#define AHT10_I2C_PORT i2c0
#define AHT10_SDA_PIN 0
#define AHT10_SCL_PIN 1
#define AHT10_I2C_BAUDRATE 100000

// --- Implementação das Funções ---

void aht10_init() {
    i2c_init(AHT10_I2C_PORT, AHT10_I2C_BAUDRATE);
    gpio_set_function(AHT10_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(AHT10_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(AHT10_SDA_PIN);
    gpio_pull_up(AHT10_SCL_PIN);

    printf("I2C0 para AHT10 configurado. Tentando resetar AHT10...\n");
    aht10_reset();

    uint8_t init_cmd[3] = {AHT10_CMD_INITIALIZE, 0x08, 0x00};
    int ret = i2c_write_blocking(AHT10_I2C_PORT, AHT10_ADDR, init_cmd, 3, false);
    
    if (ret < 0) {
        printf("Erro ao escrever comando de inicializacao para AHT10.\n");
        return;
    }

    sleep_ms(300);

    uint8_t status;
    i2c_read_blocking(AHT10_I2C_PORT, AHT10_ADDR, &status, 1, false);
    if (!(status & AHT10_STATUS_CAL_MASK)) {
        printf("AHT10 NAO CALIBRADO! Tente reiniciar o sistema.\n");
    } else {
        printf("AHT10 inicializado e calibrado com sucesso.\n");
    }
}

void aht10_reset() {
    uint8_t reset_cmd = AHT10_CMD_SOFT_RESET;
    int ret = i2c_write_blocking(AHT10_I2C_PORT, AHT10_ADDR, &reset_cmd, 1, false);
    if (ret < 0) {
        printf("Erro ao enviar comando de reset para AHT10.\n");
    }
    sleep_ms(20);
}

bool aht10_read_data(float *humidity, float *temperature) {
    uint8_t measure_cmd[3] = {AHT10_CMD_MEASURE, 0x33, 0x00};
    if (i2c_write_blocking(AHT10_I2C_PORT, AHT10_ADDR, measure_cmd, 3, false) < 0) {
        printf("Erro ao enviar comando de medicao para AHT10.\n");
        return false;
    }

    sleep_ms(80);

    uint8_t status_byte;
    i2c_read_blocking(AHT10_I2C_PORT, AHT10_ADDR, &status_byte, 1, false);
    if (status_byte & AHT10_STATUS_BUSY_MASK) {
        printf("AHT10 Ocupado, nao foi possivel ler os dados.\n");
        return false;
    }

    uint8_t data[6];
    if (i2c_read_blocking(AHT10_I2C_PORT, AHT10_ADDR, data, 6, false) < 0) {
        printf("Erro ao ler dados do AHT10.\n");
        return false;
    }

    uint32_t raw_humidity = ((uint32_t)(data[1]) << 16) | ((uint32_t)(data[2]) << 8) | data[3];
    raw_humidity >>= 4;

    uint32_t raw_temperature = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];

    *humidity = (float)raw_humidity * 100.0f / 1048576.0f;
    *temperature = (float)raw_temperature * 200.0f / 1048576.0f - 50.0f;

    return true;
}