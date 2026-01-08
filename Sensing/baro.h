#pragma once
#include <Arduino.h>
#include "Utilities/i2c.h"

// LPS22HB I2C address
#define BARO_ADDR       0x5C
#define BARO_WHO_AM_I   0xB1
#define BARO_WHO_REG    0x0F

// Registers
#define PRESS_OUT_XL    0x28
#define TEMP_OUT_L      0x2B
#define CTRL_REG1       0x10

// Scale factors
#define PRESS_SCALE     4096.0f   // 1 LSB = 1/4096 hPa
#define TEMP_SCALE      100.0f    // 1 LSB = 1/100 °C
#define SEA_LEVEL_PRESSURE 1013.25f  // hPa

// Initialize barometer
bool baro_init();

// Read raw pressure (hPa) and temperature (°C)
bool baro_read(float &pressure_hpa, float &temperature_c);

// Read altitude above sea level (m)
bool baro_read_altitude(float &altitude_m);
