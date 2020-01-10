/*
 * Handler_Display.h
 *
 * Created: 17/12/2019 11:15:03 PM
 *  Author: GSM Rana
 */ 


#ifndef HANDLER_DISPLAY_H_
#define HANDLER_DISPLAY_H_

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include "Typedefs.h"

#define TITTLE_SCROLL_TIME_MS		30
#define SCORE_UPDATE_TIME_MS		200
#define CLOCK_UPDATE_TIME_MS		500

#define DMD_SCAN_ISR_PERIOD_US		2000 //range 64 ~ 16320 microsecond, should be less than 2500
#define DMD_SCAN_TIMER_PRESCALLER	1024
#define DMD_SCAN_TIMER_OCR_VALUE	(((F_CPU/1000000)*DMD_SCAN_ISR_PERIOD_US)/DMD_SCAN_TIMER_PRESCALLER)
#define DMD_SCAN_ISR_VECT			TIMER2_COMPA_vect
				
// set Timer2 COMPA IRQ with pre-scaler 1024 (max)
#define DMD_SCAN_ISR__Init()				\
({											\
	TCCR2A = _BV(WGM21);					\
	TIMSK2 = _BV(OCIE2A);					\
	OCR2A  = DMD_SCAN_TIMER_OCR_VALUE;		\
	TCCR2B = _BV(CS22)|_BV(CS21)|_BV(CS20);	\
})

// set Timer1 OC1A PWM with pre-scaler 64
#define DMD_BRIGHTNESS_PWM__Init()			\
({											\
	TCCR1A = _BV(COM1A1)|_BV(WGM10);		\
	OCR1A  = 0xFF;							\
	TCCR1B = _BV(CS11)|_BV(CS10);			\
})

// set Timer1 OC1A duty cycle
#define DMD_OE_PWM_SET_DUTYCYCLE(p)			\
({											\
	OCR1A = (p*255)/100;					\
})

enum _app_mode
{
	APP_MODE_TITTLE,
	APP_MODE_MATCH,
	APP_MODE_TIMER,
	MAX_APP_MODE
};

enum _display_state
{
	DISPLAY_INIT,
	DISPLAY_UPDATE,
};

extern char tittle_text[];
extern tt_game_t ttgame;

void handler_display_init();
void handler_display_clear();
void handler_display_set_brightness(uint8_t percent);
void handler_display_test_pixel_by_column();
void handler_display_test_pixel_by_row();
void handler_display_test_pattern();
void handler_display_test_numbers();
void handler_display_match_winner();
void handler_display_manager();

#endif /* HANDLER_DISPLAY_H_ */