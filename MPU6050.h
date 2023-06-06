#include "stm32f10x.h"
#include "I2C.h"

#define MPU6050_ADDR 0xD0

#define SMPLRT_DIV_REG 0x19
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75

void mpuWrite(uint8_t address, uint8_t reg, uint8_t data);
void mpuRead(uint8_t address, uint8_t reg, uint8_t *buffer, uint8_t size);
void mpu6050Init(void);
void mpu6050ReadAccel(float *Ax, float *Ay, float *Az);