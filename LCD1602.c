#include "LCD1602.h"

void lcdWrite(uint8_t data)
{
	i2cStart();
	i2cAddress(0x4E);
	i2cWrite(data);
	i2cStop();
}

void lcdCmd(uint8_t data)
{
	lcdWrite(0x08);
	
	delayUs(10);
	lcdWrite(0x0C);
	delayUs(5);
	lcdWrite(((data & 0x00F0) | 0x0C));
	delayUs(10);
	lcdWrite(((data & 0x00F0) | 0x08));
	
	delayUs(20);
	lcdWrite(0x0C);
	delayUs(5);
	lcdWrite((((data << 4) & 0x00F0) | 0x0C));
	delayUs(10);
	lcdWrite((((data << 4) & 0x00F0) | 0x08));
}

void lcdInit()
{
	i2cInit();
	
	delayMs(20);
	lcdWrite(0x08);
	delayUs(10);
	lcdWrite(0x0C);
	delayUs(5);
	lcdWrite(0x3C);
	delayUs(10);
	lcdWrite(0x38);
	
	delayMs(10);
	lcdWrite(0x08);
	delayUs(10);
	lcdWrite(0x0C);
	delayUs(5);
	lcdWrite(0x3C);
	delayUs(10);
	lcdWrite(0x38);
	
	delayMs(1);
	lcdWrite(0x08);
	delayUs(10);
	lcdWrite(0x0C);
	delayUs(5); 
	lcdWrite(0x3C);
	delayUs(10);
	lcdWrite(0x38);
	
	delayMs(1);
	lcdWrite(0x08);
	delayUs(10);
	lcdWrite(0x0C);
	delayUs(5);
	lcdWrite(0x2C);
	delayUs(10);
	lcdWrite(0x28);
	
	lcdCmd(0x2C);
	delayMs(5);
	lcdCmd(0x0C);
	delayMs(5);
	lcdCmd(0x01);
	delayMs(5);
	lcdCmd(0x02);
	delayMs(5);
}

void lcdClear()
{
	lcdCmd(0x80|0x00);
	for (int i=0; i<16; i++)
	{
		lcdData(' ');
	}
	lcdCmd(0x80|0x40);
	for (int i=0; i<16; i++)
	{
		lcdData(' ');
	}
}

void lcdData(uint8_t data)
{
	lcdWrite(0x09);
	
	delayUs(10);
	lcdWrite(0x0D);
	delayUs(5);
	lcdWrite(((data & 0x00F0) | 0x0D));
	delayUs(10);
	lcdWrite(((data & 0x00F0) | 0x09));
	
	delayUs(20);
	lcdWrite(0x0D);
	delayUs(5);
	lcdWrite((((data << 4) & 0x00F0) | 0x0D));
	delayUs(10);
	lcdWrite((((data << 4) & 0x00F0) | 0x09));
}

void lcdSend(char msg[])
{
	int32_t i = 0;
	while(msg[i])
	{
		lcdData((uint8_t)msg[i]);
		i++;
		delayUs(100);
	}
}

void lcdMessage(uint8_t row, uint8_t col, char msg[])
{
	int16_t pos = 0;
	if(row==0)
	{
		pos = 0;
	}
	else if(row==1)
	{
		pos = 0x40;
	}
	lcdCmd((uint8_t)0x80 + pos + col);
	delayUs(100);
	lcdSend(msg);
}