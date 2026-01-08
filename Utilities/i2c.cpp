#include "Utilities/i2c.h"
#include <Wire.h>
#include <Arduino.h>

// Initialize I2C bus and Serial interface
bool i2c_init()
{
    Wire.setSDA(PB9);
    Wire.setSCL(PB8);
    Wire.begin();
    Wire.setClock(100000);

    Serial.begin(115200);
    while (!Serial) {}
    return true;
}

// Reads a single byte from a register of an I2C device
// Returns 0xFF if read fails
uint8_t i2c_read(uint8_t addr, uint8_t reg)
{
    Wire.beginTransmission(addr);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0)
    {
        return 0xFF;
    }
    Wire.requestFrom(addr, (uint8_t)1);
    if (Wire.available())
    {
        return Wire.read();
    }
    return 0xFF;
}

// Writes a single byte to a register of an I2C device
// Returns true if write was successful
bool i2c_write(uint8_t addr, uint8_t reg, uint8_t val)
{
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(val);
    return (Wire.endTransmission() == 0);
}

// Reads multiple consecutive registers from an I2C device
// Returns true if all bytes were successfully read
bool i2c_read_multi(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
    Wire.beginTransmission(addr);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0)
        return false;

    Wire.requestFrom(addr, len);
    if (Wire.available() < len)
        return false;

    for (uint8_t i = 0; i < len; i++)
        buf[i] = Wire.read();

    return true;
}

// Checks if an MLX90393 magnetometer is present at the given address
// Returns true if a valid status byte is read after sending the RESET command
bool i2c_test_mag(uint8_t addr)
{
    Wire.beginTransmission(addr);
    Wire.write(0xF0); // RESET command
    if (Wire.endTransmission() != 0)
        return false;

    delay(10);

    Wire.requestFrom(addr, (uint8_t)2);
    if (Wire.available() < 2)
    {
        return false;
    }
    uint8_t status0 = Wire.read();
    uint8_t status1 = Wire.read();
    // Valid magnetometer response: bit 7 should be 0
    return ((status0 & 0x80) == 0);
}

// Identifies known I2C devices on the bus and prints one-line FOUND/NOT FOUND
void i2c_identify_devices()
{
    bool mag_found  = false;
    bool baro_found = false;
    bool imu_found  = false;

    const uint8_t WHO_AM_I_REG = 0x0F;
    const uint8_t IMU_WHOAMI   = 0x6C; // LSM6DSO IMU
    const uint8_t BARO_WHOAMI  = 0xB1; // LPS22HB Barometer

    // Scan addresses from 0x01 to 0x7E
    for (uint8_t addr = 1; addr < 127; addr++)
    {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() != 0)
            continue;

        // Check for Magnetometer (MLX90393) addresses 0x0C–0x0F
        if (!mag_found && addr >= 0x0C && addr <= 0x0F)
        {
            if (i2c_test_mag(addr))
                mag_found = true;
        }

        // Check WHO_AM_I for STM devices
        uint8_t whoami = i2c_read(addr, WHO_AM_I_REG);

        if (!imu_found && whoami == IMU_WHOAMI)
            imu_found = true;

        if (!baro_found && whoami == BARO_WHOAMI)
            baro_found = true;
    }

    // Print results in one-line format
    Serial.print("Looking for Magnetometer ... ");
    Serial.println(mag_found ? "FOUND" : "NOT FOUND");

    Serial.print("Looking for Barometer ... ");
    Serial.println(baro_found ? "FOUND" : "NOT FOUND");

    Serial.print("Looking for IMU ... ");
    Serial.println(imu_found ? "FOUND" : "NOT FOUND");
}
