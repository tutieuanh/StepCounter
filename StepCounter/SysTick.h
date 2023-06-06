#include "stm32f10x.h"

void SysTickInit(void);

uint32_t getMs(void);
uint32_t getUs(void);

uint32_t getLedTicks(void);
void setLedTicks(uint32_t value);

void delayMs(uint32_t ms);
void delayUs(uint32_t us);