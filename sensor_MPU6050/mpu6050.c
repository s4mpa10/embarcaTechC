#include "mpu6050.h"
#include <stdio.h>
#include <string.h>
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <math.h>

// Definição de M_PI caso não exista em math.h
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void mpu6050_init() {
    i2c_init(MPU6050_I2C_PORT, MPU6050_I2C_BAUDRATE);
    gpio_set_function(MPU6050_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(MPU6050_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(MPU6050_SDA_PIN);
    gpio_pull_up(MPU6050_SCL_PIN);

    printf("I2C0 para MPU6050 configurado.\\n");
    sleep_ms(100);

    uint8_t buf[2];
    buf[0] = MPU6050_PWR_MGMT_1;
    buf[1] = 0x00;
    
    int ret = i2c_write_blocking(MPU6050_I2C_PORT, MPU6050_ADDR, buf, 2, false);
    if (ret == PICO_ERROR_GENERIC) {
        printf("Erro ao acordar MPU6050! Verifique conexoes e endereco I2C.\\n");
    } else {
        printf("MPU6050 acordado e inicializado com sucesso.\\n");
    }
    sleep_ms(100);
}

void mpu6050_read_raw_data(int16_t accel[3], int16_t gyro[3]) {
    uint8_t buffer[14];
    uint8_t reg_addr = MPU6050_ACCEL_XOUT_H;
    int ret = i2c_write_blocking(MPU6050_I2C_PORT, MPU6050_ADDR, &reg_addr, 1, true);
    if (ret == PICO_ERROR_GENERIC) {
        printf("Erro ao solicitar leitura de dados do MPU6050.\\n");
        memset(accel, 0, sizeof(int16_t)*3);
        memset(gyro, 0, sizeof(int16_t)*3);
        return;
    }
    
    ret = i2c_read_blocking(MPU6050_I2C_PORT, MPU6050_ADDR, buffer, 14, false);
    if (ret == PICO_ERROR_GENERIC) {
        printf("Erro ao ler dados do MPU6050.\\n");
        memset(accel, 0, sizeof(int16_t)*3);
        memset(gyro, 0, sizeof(int16_t)*3);
        return;
    }

    accel[0] = (int16_t)((buffer[0] << 8) | buffer[1]);
    accel[1] = (int16_t)((buffer[2] << 8) | buffer[3]);
    accel[2] = (int16_t)((buffer[4] << 8) | buffer[5]);
    gyro[0] = (int16_t)((buffer[8] << 8) | buffer[9]);
    gyro[1] = (int16_t)((buffer[10] << 8) | buffer[11]);
    gyro[2] = (int16_t)((buffer[12] << 8) | buffer[13]);
}

void calculate_angles(int16_t accel[3], float *pitch, float *roll) {
    float accel_x = (float)accel[0] / ACCEL_SCALE_FACTOR;
    float accel_y = (float)accel[1] / ACCEL_SCALE_FACTOR;
    float accel_z = (float)accel[2] / ACCEL_SCALE_FACTOR;

    *pitch = atan2f(accel_x, sqrtf(accel_y * accel_y + accel_z * accel_z)) * 180.0f / M_PI;
    *roll = atan2f(accel_y, sqrtf(accel_x * accel_x + accel_z * accel_z)) * 180.0f / M_PI;
}