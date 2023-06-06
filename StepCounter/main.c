#include "stm32f10x.h"
#include <stdio.h>
#include <math.h>
#include "SysTick.h"
#include "LCD1602.h"
#include "MPU6050.h"

void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);

void init(void);
void displayLoading(void);
void blinkLedGreen(uint16_t ms);
void calibrate();
void initThreshold();
void counter();

uint8_t statusLedGreen = 0;
uint8_t statusLCD = 1;
uint8_t mode = 0;
uint8_t resetStep = 0;

uint32_t steps = 0;
char buffer[32];

float Ax = 0, Ay = 0, Az = 0;
float AxAvg = 0, AyAvg = 0, AzAvg = 0;
float AxValue[100] = {0};
float AyValue[100] = {0};
float AzValue[100] = {0};
float accTotal = 0;
float min = 0, max = 0;
float threshold = 0;
uint16_t sampling = 0;
uint8_t flag = 0;

float prevAcc = 0, accValue = 0, delta = 0;

void counterStep()
{
	mpu6050ReadAccel(&Ax, &Ay, &Az);

	accTotal = sqrt((Ax * Ax) + (Ay * Ay) + (Az * Az));
	
	accValue = (accTotal + prevAcc) / 2;	
	
	delta = accTotal - prevAcc;
	
	prevAcc = accTotal;
	
	if (delta > 0.15 && delta < 0.9)
	{
		steps++;
	}
	//else if((delta < 0.15 || delta > 0.9) && flag == 1)
	//{
		//flag = 0;
	//}
}

int main()
{
	init();
	
	//calibrate();
	
	displayLoading();
	
	while(1)
	{
		if (resetStep)
		{
			steps = 0;
			lcdClear();
			resetStep = 0;
		}
		
		if (mode)
		{
			blinkLedGreen(400);
			
			if (statusLCD)
			{
				lcdClear();
				statusLCD = 0;
			}
			
			//counter();
					
			counterStep();
			
			lcdMessage(0, 0, "Steps: ");
			sprintf (buffer, "%d", steps);
			lcdMessage(0, 7, buffer);
		
			lcdMessage(1, 0, "A");
			sprintf(buffer, "%.2f", accTotal);
			lcdMessage(1, 1, buffer);
		
			lcdMessage(1, 8, "T");
			sprintf(buffer, "%.2f", delta);
			lcdMessage(1, 9, buffer);
			
			delayMs(300);
		}
		else
		{
			if(!statusLCD)
			{
				lcdClear();
				statusLCD = 1;
			}
			
			//initThreshold();
			
			lcdMessage(0, 0, "Steps: ");
			sprintf (buffer, "%d", steps);
			lcdMessage(0, 7, buffer);
			lcdMessage(1, 0, "SW1: SC  SW2: RS");
						
			delayMs(200);
		}
	}
}

void EXTI0_IRQHandler(void)
{
	if(EXTI->PR & (1u<<0))
	{
		EXTI->PR |= (1u<<0);
		
		statusLedGreen = ~statusLedGreen;
		mode = ~mode;
		
		if (GPIOC->IDR & (1u<<14))
		{
			GPIOC->ODR &= ~GPIO_ODR_ODR14;
		}
		else
		{
			GPIOC->ODR |= GPIO_ODR_ODR14;
			GPIOC->ODR &= ~GPIO_ODR_ODR15;
		}
	}
}

void EXTI1_IRQHandler(void)
{
	if(EXTI->PR & (1u<<1))
	{
		EXTI->PR |= (1u<<1);
		
		resetStep = 1;
	}
}

void init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;
	
	GPIOA->CRL &= ~GPIO_CRL_CNF0;
	GPIOA->CRL |= GPIO_CRL_CNF0_1;
	
	GPIOA->CRL &= ~GPIO_CRL_CNF1;
	GPIOA->CRL |= GPIO_CRL_CNF1_1;
	
	GPIOC->CRH &= ~GPIO_CRH_MODE14;
	GPIOC->CRH &= ~GPIO_CRH_CNF14;
	GPIOC->CRH |= GPIO_CRH_MODE14_0;
	
	GPIOC->CRH &= ~GPIO_CRH_MODE15;
	GPIOC->CRH &= ~GPIO_CRH_CNF15;
	GPIOC->CRH |= GPIO_CRH_MODE15_0;
	
	GPIOC->ODR |= GPIO_ODR_ODR14;
	GPIOC->ODR &= ~GPIO_ODR_ODR15;
	
	AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0;
	EXTI->IMR |= EXTI_IMR_MR0;
	EXTI->RTSR |= EXTI_RTSR_TR0;
	EXTI->FTSR &= ~EXTI_FTSR_TR0;
	NVIC_SetPriority(EXTI0_IRQn, 0);
	NVIC_EnableIRQ(EXTI0_IRQn);
	
	AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI1;
	EXTI->IMR |= EXTI_IMR_MR1;
	EXTI->RTSR |= EXTI_RTSR_TR1;
	EXTI->FTSR &= ~EXTI_FTSR_TR1;
	NVIC_SetPriority(EXTI1_IRQn, 0);
	NVIC_EnableIRQ(EXTI1_IRQn);
	
	SystemInit();
	SysTickInit();
	
	lcdInit();
	
	mpu6050Init();
}

void displayLoading(void)
{
	lcdClear();
  lcdCmd(0x80|0x00);
	
  lcdSend("Loading");
	delayMs(500);
	
  lcdSend(".");
	delayMs (500);
	
  lcdSend(".");
	delayMs(500);
	
  lcdSend(".");
	delayMs(500);
	
	lcdClear();
}

void blinkLedGreen(uint16_t ms)
{
	if (statusLedGreen && getLedTicks() > ms)
	{
		GPIOC->ODR ^= GPIO_ODR_ODR15;
		setLedTicks(0);
	}
}

void calibrate()
{
	float sumX=0, sumY=0, sumZ=0;
	for(int i=0; i<100; i++)
	{
		mpu6050ReadAccel(&AxValue[i], &AyValue[i], &AzValue[i]);
		sumX = sumX + AxValue[i];
		sumY = sumY + AyValue[i];
		sumZ = sumZ + AzValue[i];
	}
	AxAvg = sumX / 100.0;
	AyAvg = sumY / 100.0;
	AzAvg = sumZ / 100.0;
}

/*float prevAcc = 0, accValue = 0;

void initThreshold()
{
	mpu6050ReadAccel(&Ax, &Ay, &Az);

	accTotal = sqrt(((Ax - AxAvg) * (Ax - AxAvg)) + ((Ay - AyAvg) * (Ay - AyAvg)) + ((Az - AzAvg) * (Az - AzAvg)));
	//accTotal = sqrt((Ax * Ax) + (Ay * Ay) + (Az * Az ));
	
	accValue = (accTotal + prevAcc) / 2;
	
	prevAcc = accTotal;	
	
	if(sampling == 0)
	{
		max = accValue;
		min = accValue;
	}
	else if(max < accValue)
	{
		max = accValue;
	}
	else if(min > accValue)
	{
		min = accValue;
	}
		
	sampling++;

	if(sampling >= 10)
	{
		threshold = (max + min) / 2 + 0.06;
		sampling = 0;
	}
}

void counter()
{
	initThreshold();
	
	if (accValue > threshold && flag == 0)
	{
		steps++;
		flag = 1;
	}
	else if (accValue > threshold && flag == 1)
	{
	}
	else if (accValue < threshold && flag == 1)
	{
		flag = 0;
	}
}*/