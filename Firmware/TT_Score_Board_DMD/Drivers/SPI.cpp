#include "SPI.h"

SPIClass SPI;

void SPIClass::begin() 
{
  SPCR |= (1<<MSTR);
  SPCR |= (1<<SPE);
}

void SPIClass::end() 
{
  SPCR &= ~(1<<SPE);
}

void SPIClass::setBitOrder(uint8_t bitOrder)
{
  if(bitOrder == LSBFIRST)  SPCR |= (1<<DORD);
  else SPCR &= ~(1<<DORD);
}

void SPIClass::setDataMode(uint8_t mode)
{
  SPCR = (SPCR & ~SPI_MODE_MASK) | mode;
}

void SPIClass::setClockDivider(uint8_t rate)
{
  SPCR = (SPCR & ~SPI_CLOCK_MASK) | (rate & SPI_CLOCK_MASK);
  SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((rate >> 2) & SPI_2XCLOCK_MASK);
}

uint8_t SPIClass::transfer(uint8_t data)
{
	SPDR = data;					// send data
	while(!(SPSR & (1<<SPIF)));		// wait for transfer complete
	return(SPDR);					// receive data
}

void SPIClass::attachInterrupt() 
{
	SPCR |= (1<<SPIE);
}

void SPIClass::detachInterrupt() 
{
	SPCR &= ~(1<<SPIE);
}
