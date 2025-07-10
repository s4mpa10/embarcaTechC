#ifndef MPU6050_H
#define MPU6050_H

#include "pico/stdlib.h"

// --- Definições para o Sensor MPU6050 ---
#define MPU6050_ADDR         0x68 // Endereço I2C do MPU6050 (AD0 para GND)
#define MPU6050_PWR_MGMT_1   0x6B // Registrador Power Management 1
#define MPU6050_ACCEL_XOUT_H 0x3B // Registrador MSB do X do Acelerômetro
#define ACCEL_SCALE_FACTOR   16384.0f

// --- Definições de Pinos e I2C para o MPU6050 ---
#define MPU6050_I2C_PORT    i2c0
#define MPU6050_SDA_PIN     0
#define MPU6050_SCL_PIN     1
#define MPU6050_I2C_BAUDRATE 100000

// --- Protótipos de Funções ---
void mpu6050_init();
void mpu6050_read_raw_data(int16_t accel[3], int16_t gyro[3]);
void calculate_angles(int16_t accel[3], float *pitch, float *roll);

#endif // MPU6050_H