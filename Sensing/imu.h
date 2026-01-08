#pragma once
#include <Arduino.h>

bool imu_init();
bool imu_read_accel(float &ax, float &ay, float &az); // returns m/s^2, Z is gravity-compensated
bool imu_read_gyro(float &gx_dps, float &gy_dps, float &gz_dps);
