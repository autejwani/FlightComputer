#include "baro.h"
#include <Wire.h>
#include <math.h>

bool baro_init()
{
	Wire.setSDA(PB9);
	Wire.setSCL(PB8);
	Wire.begin();

	uint8_t whoami = i2c_read(BARO_ADDR, BARO_WHO_REG);
	if (whoami != BARO_WHO_AM_I)
	{
		Serial.print("Barometer WHO_AM_I mismatch: 0x");
		Serial.println(whoami, HEX);
		return false;
	}

	// CTRL_REG1: active, 1 Hz ODR
	i2c_write(BARO_ADDR, CTRL_REG1, 0x10);

	Serial.println("Barometer initialized successfully");
	return true;
}

bool baro_read(float &pressure_hpa, float &temperature_c)
{
	uint8_t buf[5];
	if (!i2c_read_multi(BARO_ADDR, PRESS_OUT_XL, buf, 5))
		return false;

	// Pressure: 24-bit
	int32_t press_raw = (int32_t)buf[2] << 16 | (int32_t)buf[1] << 8 | buf[0];
	pressure_hpa = press_raw / PRESS_SCALE;

	// Temperature: 16-bit
	int16_t temp_raw = (int16_t)(buf[4] << 8 | buf[3]);
	temperature_c = temp_raw / TEMP_SCALE;

	return true;
}

bool baro_read_altitude(float &altitude_m)
{
	float pressure, temperature;
	if (!baro_read(pressure, temperature))
		return false;

	// Barometric formula for altitude (m)
	altitude_m = 44330.0f * (1.0f - powf(pressure / SEA_LEVEL_PRESSURE, 0.1903f));
	return true;
}
