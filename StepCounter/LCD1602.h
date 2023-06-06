#include "stm32f10x.h"
#include "SysTick.h"
#include "I2C.h"

void lcdWrite(uint8_t data);

void lcdCmd(uint8_t data);
void lcdInit();
void lcdClear();
void lcdData(uint8_t data);
void lcdSend(char msg[]);
void lcdMessage(uint8_t row, uint8_t col, char msg[]);