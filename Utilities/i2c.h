#ifndef I2C_H
#define I2C_H

#include <Arduino.h>

bool i2c_init(void);
uint8_t i2c_read(uint8_t addr, uint8_t reg);
bool i2c_write(uint8_t addr, uint8_t reg, uint8_t val);
bool i2c_read_multi(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);
bool i2c_test_mag(uint8_t addr);
void i2c_identify_devices(void);

#endif
