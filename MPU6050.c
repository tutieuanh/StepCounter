#include "MPU6050.h"

int16_t accelX = 0;
int16_t accelY = 0;
int16_t accelZ = 0;

void mpuWrite(uint8_t address, uint8_t reg, uint8_t data)
{
	i2cStart();							// 	Khoi tao I2C
	i2cAddress(address);		//	Gui dia chi cua cam bien
	i2cWrite(reg);					//	Gui dia chi cua thanh ghi can ghi du lieu
	i2cWrite(data);					//	Gui du lieu can ghi vao thanh ghi
	i2cStop();							//	Dung I2C
}

void mpuRead(uint8_t address, uint8_t reg, uint8_t *buffer, uint8_t size)
{
	i2cStart();								// 	Khoi tao I2C
	i2cAddress(address);			//	Gui dia chi cua cam bien
	i2cWrite(reg);						//	Gui dia chi cua thanh ghi can ghi du lieu
	i2cStart();								// 	Khoi dong lai I2C
	i2cRead(address+0x01, buffer, size);		//	Doc gia tri tu thanh ghi vao mang
	i2cStop();								//	Dung I2C
}

void mpu6050Init(void)
{
	uint8_t check;
	uint8_t data;

	// Kiem tra xem cam bien co phan hoi (san sang su dung) hay khong 
	mpuRead(MPU6050_ADDR, WHO_AM_I_REG, &check, 1);

	if (check == 104) // Cam bien tra ve 0x68 neu da san sang
	{
		// Khoi tao cam bien bang cách ghi vao thanh ghi 0x6B
		data = 0;
		mpuWrite(MPU6050_ADDR, PWR_MGMT_1_REG, data);
		
		// Dat toc do la 1KHz bang cach ghi vao thanh ghi 0x6B
		data = 0x07;
		mpuWrite(MPU6050_ADDR, SMPLRT_DIV_REG, data);

		// Dat cau hinh cho gia toc tai thanh ghi 0x1C
		data = 0x00;
		mpuWrite(MPU6050_ADDR, ACCEL_CONFIG_REG, data);
	}
}

void mpu6050ReadAccel(float *Ax, float *Ay, float *Az)
{
	uint8_t data[6];

	// Doc 6 byte du lieu bat dau tu thanh ghi 0x3B
	mpuRead(MPU6050_ADDR, ACCEL_XOUT_H_REG, data, 6);

	// Lay cac gia tri du lieu tu mang 
	accelX = (int16_t)(data[0] << 8 | data[1]);
	accelY = (int16_t)(data[2] << 8 | data[3]);
	accelZ = (int16_t)(data[4] << 8 | data[5]);

	// Chuyen doi cac gia tri thanh gia toc trong truong
	*Ax = accelX/16384.0;
	*Ay = accelY/16384.0;
	//*Az = accelZ/16384.0;
	*Az = accelZ/14418.0;
}
