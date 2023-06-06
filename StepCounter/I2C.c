#include "I2C.h"

void i2cInit()
{
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	
	// Pin PB6 enable 
	GPIOB->CRL |= GPIO_CRL_MODE6;
	GPIOB->CRL |= GPIO_CRL_CNF6;
	
	// Pin PB7 enable
	GPIOB->CRL |= GPIO_CRL_MODE7;
	GPIOB->CRL |= GPIO_CRL_CNF7;
	
	I2C1->CR1 |= 0x8000;
	I2C1->CR1 &= ~0x8000;
	I2C1->CR2 = 0x18;
	I2C1->CCR = 0x5A;
	I2C1->TRISE = 0x19;
	I2C1->CR1 |= 1;
}

void i2cStart()
{
	I2C1->CR1 |= 0x400;			// Enable the ACK
	I2C1->CR1 |= 0x100;			// Generate START
	while (!(I2C1->SR1 & 1)){};
}

void i2cStop()
{
	volatile uint16_t tmp;
	
	tmp = I2C1->SR1;
	tmp = I2C1->SR2;
	I2C1->CR1 |= 0x200; // Stop I2C
}

void i2cAddress(uint8_t address)
{
	volatile uint16_t tmp;
	
	I2C1->DR = address;  //  send the address
	while((I2C1->SR1 & 2)==0){}; // wait for ADDR bit to set
	while((I2C1->SR1 & 2))
	{
		tmp = I2C1->SR1; 
		tmp = I2C1->SR2;  // read SR1 and SR2 to clear the ADDR bit
		if((I2C1->SR1 & 2)==0)
		{
			break;
		}
	}
}

void i2cWrite(uint8_t data)
{
	while((I2C1->SR1 & 0x80) == 0){}  // wait for TXE bit to set 
	I2C1->DR = data;  // send data
	while((I2C1->SR1 & 0x80) == 0){} // co the thay the dong duoi
	//while(!(I2C1->SR1 & (1<<2)));  // wait for BTF to set
}

void i2cRead(uint8_t address, uint8_t *buffer, uint8_t size)
{
	int remaining = size;
	
	if (size == 1)
	{	
		I2C1->DR = address;  //  send the address
		while (!(I2C1->SR1 & (1<<1)));  // wait for ADDR bit to set
		
		I2C1->CR1 &= ~(1<<10);  // clear the ACK bit 
		uint8_t temp = I2C1->SR1 | I2C1->SR2;  // read SR1 and SR2 to clear the ADDR bit.... EV6 condition
		I2C1->CR1 |= (1<<9);  // Stop I2C

		while (!(I2C1->SR1 & (1<<6)));  // wait for RxNE to set	
		buffer[size-remaining] = I2C1->DR;  // Read the data from the DATA REGISTER
	}	
	else 
	{
		I2C1->DR = address;  //  send the address
		while (!(I2C1->SR1 & (1<<1)));  // wait for ADDR bit to set
		
		uint8_t temp = I2C1->SR1 | I2C1->SR2;  // read SR1 and SR2 to clear the ADDR bit
		
		while (remaining>2)
		{
			while (!(I2C1->SR1 & (1<<6)));  // wait for RxNE to set
			
			buffer[size-remaining] = I2C1->DR;  // copy the data into the buffer			
			
			I2C1->CR1 |= 1<<10;  // Set the ACK bit to Acknowledge the data received
			
			remaining--;
		}
		
		while (!(I2C1->SR1 & (1<<6)));  // wait for RxNE to set
		buffer[size-remaining] = I2C1->DR;
		
		I2C1->CR1 &= ~(1<<10);  // clear the ACK bit 
		
		I2C1->CR1 |= (1<<9);  // Stop I2C
		
		remaining--;
		
		while (!(I2C1->SR1 & (1<<6)));  // wait for RxNE to set
		buffer[size-remaining] = I2C1->DR;  // copy the data into the buffer
	}	
}