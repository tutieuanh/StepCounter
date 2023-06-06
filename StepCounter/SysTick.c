#include "SysTick.h"

volatile uint32_t msTicks=0;
volatile uint32_t ledTicks=0;

void SysTickInit(void)
{
	SysTick->CTRL = 0;
	SysTick->LOAD = 72000-1;
	SysTick->VAL = 0;
	SysTick->CTRL = 7;
}

void SysTick_Handler(void)
{
	msTicks++;
	ledTicks++;
}

uint32_t getMs(void)
{
	return msTicks;
}

uint32_t getUs(void)
{
	return getMs()*1000 + (72000 - SysTick->VAL)/72;
}

uint32_t getLedTicks(void)
{
	return ledTicks;
}

void setLedTicks(uint32_t value)
{
	ledTicks = value;
}

void delayMs(uint32_t ms)
{
	msTicks=0;
	uint32_t end = getMs() + ms;
	while (getMs() < end);
}

void delayUs(uint32_t us)
{
	msTicks=0;
	uint32_t end = getUs() + us;
  while (getUs() < end);
}

