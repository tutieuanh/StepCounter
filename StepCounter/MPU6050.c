#include "MPU6050.h"

int16_t accelX = 0;
int16_t accelY = 0;
int16_t accelZ = 0;

void mpuWrite(uint8_t address, uint8_t reg, uint8_t data)
{
	i2cStart();
	i2cAddress(address);
	i2cWrite(reg);
	i2cWrite(data);
	i2cStop();
}

void mpuRead(uint8_t address, uint8_t reg, uint8_t *buffer, uint8_t size)
{
	i2cStart();
	i2cAddress(address);
	i2cWrite(reg);
	i2cStart();  // repeated start
	i2cRead(address+0x01, buffer, size);
	i2cStop();
}

void mpu6050Init(void)
{
	uint8_t check;
	uint8_t data;

	mpuRead(MPU6050_ADDR, WHO_AM_I_REG, &check, 1);

	if (check == 104)
	{
		data = 0;
		mpuWrite(MPU6050_ADDR, PWR_MGMT_1_REG, data);
		
		data = 0x07;
		mpuWrite(MPU6050_ADDR, SMPLRT_DIV_REG, data);

		data = 0x00;
		mpuWrite(MPU6050_ADDR, ACCEL_CONFIG_REG, data);
	}
}

void mpu6050ReadAccel(float *Ax, float *Ay, float *Az)
{
	uint8_t data[6];

	mpuRead(MPU6050_ADDR, ACCEL_XOUT_H_REG, data, 6);

	accelX = (int16_t)(data[0] << 8 | data[1]);
	accelY = (int16_t)(data[2] << 8 | data[3]);
	accelZ = (int16_t)(data[4] << 8 | data[5]);

	*Ax = accelX/16384.0;
	*Ay = accelY/16384.0;
	//*Az = accelZ/16384.0;
	*Az = accelZ/14418.0;
}
