/*
 * TT_Score_Board_DMD_Native.cpp
 *
  * Controller: ATmega328P
  *
  * low_fuses:		0xFF
  * high_fuses:		0xDF
  * extended_fuses:	0xFC
  *
  * MCU Clock: 16MHz crystal
  *
 * Created: 16/12/2019 8:00:15 PM
 * Author : GSM Rana
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "DriverIO.h"
#include "Handler_Button.h"
#include "Handler_Display.h"

int main(void)
{
    DriverIO_Init();
    DebugLog(tittle_text);
	handler_button_init();
    handler_display_init();
	
	// check for press and hold button down
	handler_button_check_on_startup();
	
	// startup display pixel test
	handler_display_test_pattern();
	handler_display_test_numbers();
	handler_display_clear();
	
    while (1) 
    {
  		handler_button_mannager();
  		handler_display_manager();
    }
}
