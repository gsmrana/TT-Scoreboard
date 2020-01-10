/*
 * Handler_Button.h
 *
 * Created: 17/12/2019 11:14:31 PM
 *  Author: Rana
 */ 


#ifndef HANDLER_BUTTON_H_
#define HANDLER_BUTTON_H_

#include <avr/io.h>
#include <avr/sfr_defs.h>

#define REMOTE_BUTTON_A					_BV(PIND2)
#define REMOTE_BUTTON_B					_BV(PIND6)
#define REMOTE_BUTTON_C					_BV(PIND7)
#define REMOTE_BUTTON_D					_BV(PIND5)
#define REMOTE_BUTTON_IO_MASK			(REMOTE_BUTTON_A|REMOTE_BUTTON_B|REMOTE_BUTTON_C|REMOTE_BUTTON_D)
#define REMOTE_BUTTON_IO_PINS			(PIND & REMOTE_BUTTON_IO_MASK)

#define REMOTE_VT_IO_MASK				_BV(PIND3)
#define REMOTE_VT_IO_PIN				(PIND & REMOTE_VT_IO_MASK)
#define REMOTE_VT_ISR_VECT				INT1_vect

#define RTC_ISR_UPDATE_TIME_MS			1000
#define BUTTON_SHORT_DEBOUNCE_TIME_MS	50
#define BUTTON_LONG_DEBOUNCE_COUNT		4
#define BUTTON_LONG_DEBOUNCE_TIME_MS	500

// set button, VT pin as input with  pull-up 
// enable VT pin INT1 IRQ
#define Button_GPIO_ISR__Init()		\
({									\
	DDRD &= ~REMOTE_BUTTON_IO_MASK; \
	DDRD &= ~REMOTE_VT_IO_MASK;		\
	PORTD |= REMOTE_BUTTON_IO_MASK;	\
	PORTD |= REMOTE_VT_IO_MASK;		\
	EICRA |= _BV(ISC10);			\
	EIMSK |= _BV(INT1);				\
})

enum _button_state
{
	BUTTON_CLEAR,
	BUTTON_PRESSED,
};

typedef struct _remote_input_t
{
	uint8_t button_press_backup;
	uint8_t button_short_press;
	uint8_t button_long_press;
	uint8_t debounce_long_count;
	Timer_t debounce_long_timer;	
} remote_input_t;

typedef struct _softrtc_t
{
	uint8_t second;
	uint8_t minute;
	uint8_t colon_on;
	Timer_t isr_timer;
} softrtc_t;

extern volatile softrtc_t softrtc;
extern volatile remote_input_t remote;

void handler_button_init();
void handler_button_check_on_startup();
void handler_button_mannager();

#endif /* HANDLER_BUTTON_H_ */