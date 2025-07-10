#ifndef BH1750_H
#define BH1750_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Funções públicas
void bh1750_init();
float bh1750_read_lux();

#endif // BH1750_H