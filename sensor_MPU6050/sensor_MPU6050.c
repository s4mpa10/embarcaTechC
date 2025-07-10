#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "mpu6050.h"
#include "servo.h"
#include "display.h"

// --- Limite de Inclinação para Alerta ---
#define ANGULO_ALERTA_GRAUS 30.0f

int main() {
    stdio_init_all();
    printf("Iniciando sistema de monitoramento de inclinacao e controle de servo...\\n");

    // --- Inicialização de Periféricos ---
    init_oled();
    init_servo_pwm();
    mpu6050_init();

    // --- Mensagem de Início no OLED ---
    clear_oled_display();
    display_message_oled("MPU6050 + Servo", 0);
    display_message_oled("Pronto!", 2);
    sleep_ms(2000);
    clear_oled_display();

    float pitch = 0.0f;
    float roll = 0.0f;
    int16_t accel_data[3];
    int16_t gyro_data[3];
    char alerta_str[32];

    // --- Posição Inicial do Servo Motor ---
    set_servo_angle(90.0f);
    sleep_ms(500);

    // --- Loop Principal do Programa ---
    while (true) {
        // 1. Leitura e Cálculo dos Ângulos
        mpu6050_read_raw_data(accel_data, gyro_data);
        calculate_angles(accel_data, &pitch, &roll);
        printf("Pitch: %.2f deg, Roll: %.2f deg\n", pitch, roll);

        // 2. Controle Dinâmico do Servo Motor
        float servo_angle = pitch + 90.0f;
        
        if (servo_angle < 0.0f) {
            servo_angle = 0.0f;
        }
        if (servo_angle > 180.0f) {
            servo_angle = 180.0f;
        }
        set_servo_angle(servo_angle);

        // 3. Lógica de Alerta Visual para o OLED
        if (fabs(pitch) > ANGULO_ALERTA_GRAUS || fabs(roll) > ANGULO_ALERTA_GRAUS) {
            snprintf(alerta_str, sizeof(alerta_str), "ALERTA: Inclinado!");
        } else {
            alerta_str[0] = '\0';
        }

        // 4. Atualiza o Display OLED
        display_angles_oled(pitch, roll, alerta_str);

        sleep_ms(100);
    }

    return 0;
}