#ifndef SERVO_H
#define SERVO_H

#include "pico/stdlib.h"

// --- Definições para o Servo Motor SG90 ---
#define PINO_SERVO 8
#define PWM_FREQ   50
#define PWM_WRAP   19999
#define SERVO_PULSE_MIN_US 500
#define SERVO_PULSE_CENTER_US 1500
#define SERVO_PULSE_MAX_US 2500

// --- Protótipos de Funções ---
void init_servo_pwm();
void set_servo_angle(float angle_degrees);

#endif // SERVO_H