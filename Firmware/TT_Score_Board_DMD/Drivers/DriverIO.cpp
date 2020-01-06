/*
 * DriverIO.cpp
 *
 * Created: 17/12/2019 11:12:32 PM
 *  Author: GSM Rana
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "DriverIO.h"

#define clockCyclesPerMicrosecond()	 (F_CPU / 1000000L)
#define clockCyclesToMicroseconds(a) ((a) / clockCyclesPerMicrosecond())
#define microsecondsToClockCycles(a) ((a) * clockCyclesPerMicrosecond())

// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

static uint8_t timer0_fract = 0;
static volatile uint32_t timer0_millis = 0;
static volatile uint32_t timer0_overflow_count = 0;

//************************************************************************
void DriverIO_Init()
{
	// using timer0 overflow ISR with prescaler 64
	TCCR0A = _BV(WGM01)|_BV(WGM00);
	TIMSK0 = _BV(TOIE0);
	TCCR0B = _BV(CS01)|_BV(CS00);
	
#ifdef DEBUG_LOG_ENABLE
	DriverUart_Init(DEBUG_LOG_BAUDRATE);
#endif
		
	// enable global IRQ
	sei();
}

//************************************************************************
Timer_t millis()
{
	uint32_t m;
	uint8_t oldSREG = SREG;
	cli();
	m = timer0_millis;
	SREG = oldSREG;
	return m;
}

//************************************************************************
ISR(TIMER0_OVF_vect)
{
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	uint32_t m = timer0_millis;
	uint8_t f = timer0_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX)
	{
		f -= FRACT_MAX;
		m += 1;
	}

	timer0_fract = f;
	timer0_millis = m;
	timer0_overflow_count++;
}