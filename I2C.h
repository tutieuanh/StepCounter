#include "stm32f10x.h"

void i2cInit(void);
void i2cStart(void);
void i2cStop(void);
void i2cAddress(uint8_t address);
void i2cWrite(uint8_t data);
void i2cRead(uint8_t address, uint8_t *buffer, uint8_t size);