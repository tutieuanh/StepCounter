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
	
	I2C1->CR1 |= 0x8000; 		// Dat lai I2C
	I2C1->CR1 &= ~0x8000;		// Che do hoat dong binh thuong
	I2C1->CR2 = 0x18;				// Dat tan so cua xung clock la 24MHz
	I2C1->CCR = 0x5A;				// Cau hinh thanh ghi dieu khien clock 
	I2C1->TRISE = 0x19;			// Cau hinh thanh ghi thoi gian  
	I2C1->CR1 |= 1;					// Kich hoat I2C
}

void i2cStart()
{
	I2C1->CR1 |= 0x400;			// 	Bat che do gui/nhan ACK
	I2C1->CR1 |= 0x100;			// 	Khoi tao START
	while (!(I2C1->SR1 & 1)){};
}

void i2cStop()
{
	volatile uint16_t temp;
	
	temp = I2C1->SR1;
	temp = I2C1->SR2;				// 	Doc SR1 va SR2 de xoa bit ADDR	
	I2C1->CR1 |= 0x200; 		// 	Dung I2C
}

void i2cAddress(uint8_t address)
{
	volatile uint16_t temp;
	
	I2C1->DR = address;  						//  Gui dia chi
	while((I2C1->SR1 & 0x2)==0){}; 	// 	Cho bit ADDR duoc dat len 1
	while((I2C1->SR1 & 0x2))
	{
		temp = I2C1->SR1; 
		temp = I2C1->SR2;  						// 	Doc SR1 va SR2 de xoa bit ADDR
		if((I2C1->SR1 & 0x2)==0)
		{
			break;
		}
	}
}

void i2cWrite(uint8_t data)
{
	while((I2C1->SR1 & 0x80) == 0){}  // 	Cho bit TXE duoc dat thanh 1
	I2C1->DR = data;  								// 	Gui du lieu
	while((I2C1->SR1 & 0x80) == 0){} 	//	Cho bit TXE duoc dat thanh 1
}

void i2cRead(uint8_t address, uint8_t *buffer, uint8_t size)
{
	uint16_t temp;
	int sizeRemaining = size;
	
	if (size == 1)
	{	
		I2C1->DR = address;  								//  Gui dia chi
		while (!(I2C1->SR1 & 0x2));  				// 	Cho bit ADDR duoc dat thanh 1
		
		I2C1->CR1 &= ~(0x400);  						// 	Xoa bit ACK 
		temp = I2C1->SR1 | I2C1->SR2;  			//	Doc SR1 va SR2 de xoa bit ADDR
		I2C1->CR1 |= 0x200;  								// 	Dung I2C

		while (!(I2C1->SR1 & 0x40));  			// Cho bit RxNE duoc dat thanh 1
		buffer[size-sizeRemaining] = I2C1->DR;  // Doc du lieu tu thanh ghi DR
	}	
	else 
	{
		I2C1->DR = address;  								//  Gui dia chi
		while (!(I2C1->SR1 & 0x2));  				// 	Cho bit ADDR duoc dat thanh 1
		
		temp = I2C1->SR1 | I2C1->SR2;  			//	Doc SR1 va SR2 de xoa bit ADDR
		
		while (sizeRemaining>2)
		{
			while (!(I2C1->SR1 & 0x40));  			// Cho bit RxNE duoc dat thanh 1
			
			buffer[size-sizeRemaining] = I2C1->DR;  // Gan du lieu tu DR vao buffer			
			
			I2C1->CR1 |= 0x400;  								// Dat bit ACK thanh 1 de tiep tuc nhan du lieu
			
			sizeRemaining--;
		}
		
		while (!(I2C1->SR1 & 0x40));  				// Cho bit RxNE duoc dat thanh 1
		buffer[size-sizeRemaining] = I2C1->DR;
		
		I2C1->CR1 &= ~(0x400);  							// Xoa bit ACK 
		
		I2C1->CR1 |= 0x200;  									// Dung I2C
		
		sizeRemaining--;
		
		while (!(I2C1->SR1 & 0x40));  						// Cho bit RxNE duoc dat thanh 1
		buffer[size-sizeRemaining] = I2C1->DR;  	// Gan du lieu tu DR vao buffer
	}	
}