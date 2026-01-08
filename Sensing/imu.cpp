#include "imu.h"
#include "Utilities/i2c.h"
#include <Wire.h>

#define IMU_ADDR 0x6A
#define WHO_AM_I_REG 0x0F
#define WHO_AM_I_VAL 0x6C
#define CTRL1_XL 0x10
#define CTRL2_G 0x11
#define OUTX_L_XL 0x28
#define OUTX_L_G  0x22

#define ACCEL_SCALE 0.122f
#define GYRO_SCALE  4.375f  // mdps/LSB

bool imu_init(){
	Wire.setSDA(PB9);
	Wire.setSCL(PB8);
	Wire.begin();
	if(i2c_read(IMU_ADDR,WHO_AM_I_REG)!=WHO_AM_I_VAL) return false;
	i2c_write(IMU_ADDR,CTRL1_XL,0x48);
	i2c_write(IMU_ADDR,CTRL2_G,0x4C);
	return true;
}

bool imu_read_accel(float &ax_g,float &ay_g,float &az_g){
	uint8_t buf[6];
	if(!i2c_read_multi(IMU_ADDR,OUTX_L_XL,buf,6)) return false;
	ax_g=(int16_t)((buf[1]<<8)|buf[0])*ACCEL_SCALE/1000.0f;
	ay_g=(int16_t)((buf[3]<<8)|buf[2])*ACCEL_SCALE/1000.0f;
	az_g=(int16_t)((buf[5]<<8)|buf[4])*ACCEL_SCALE/1000.0f;
	return true;
}

bool imu_read_gyro(float &gx_dps, float &gy_dps, float &gz_dps){
	uint8_t buf[6];
	if(!i2c_read_multi(IMU_ADDR, OUTX_L_G, buf, 6)) return false;
	gx_dps = (int16_t)((buf[1]<<8)|buf[0]) * GYRO_SCALE / 1000.0f;
	gy_dps = (int16_t)((buf[3]<<8)|buf[2]) * GYRO_SCALE / 1000.0f;
	gz_dps = (int16_t)((buf[5]<<8)|buf[4]) * GYRO_SCALE / 1000.0f;
	return true;
}
