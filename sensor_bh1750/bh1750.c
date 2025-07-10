#include "bh1750.h"
#include <stdio.h>

// Definições do sensor
#define I2C_BH1750_PORT i2c0
#define I2C_BH1750_SDA 0  // GP0
#define I2C_BH1750_SCL 1  // GP1
#define BH1750_ADDR 0x23
#define BH1750_CONTINUOUS_HIGH_RES_MODE 0x10

void bh1750_init() {
    i2c_init(I2C_BH1750_PORT, 100 * 1000);
    gpio_set_function(I2C_BH1750_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_BH1750_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_BH1750_SDA);
    gpio_pull_up(I2C_BH1750_SCL);

    uint8_t buf[1] = {BH1750_CONTINUOUS_HIGH_RES_MODE};
    i2c_write_blocking(I2C_BH1750_PORT, BH1750_ADDR, buf, 1, false);
}

float bh1750_read_lux() {
    uint8_t data[2];
    int result = i2c_read_blocking(I2C_BH1750_PORT, BH1750_ADDR, data, 2, false);
    if (result != 2) {
        printf("Erro ao ler o sensor BH1750\n");
        return -1;
    }
    uint16_t raw = (data[0] << 8) | data[1];
    return raw / 1.2;
}