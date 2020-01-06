/*
 * DriverIO.h
 *
 * Created: 17/12/2019 11:12:12 PM
 *  Author: GSM Rana
 */ 


#ifndef DRIVERIO_H_
#define DRIVERIO_H_

#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

//#define DEBUG_LOG_ENABLE
#define DEBUG_LOG_BAUDRATE	57600

#define F(s)				PSTR(s)
#define SEC_TO_MS(s)		(s*1000UL)

#ifdef DEBUG_LOG_ENABLE
	#include "DriverUart.h"
	#define DebugLog		DriverUart_Print
#else
	#define DebugLog(...)	asm("nop")
#endif

typedef uint32_t Timer_t;

/************ Function Prototypes **********/
void DriverIO_Init();
Timer_t millis();
#define Millis	millis()


#endif /* DRIVERIO_H_ */