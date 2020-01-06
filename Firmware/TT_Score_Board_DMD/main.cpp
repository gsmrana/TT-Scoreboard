/*
 * TT_Score_Board_DMD_Native.cpp
 *
  * Controller: ATmega328P
  *
  * low_fuses:		0xFF
  * high_fuses:		0xDF
  * extended_fuses:	0xFF
  *
  * MCU Clock: 16MHz crystal
  *
 * Created: 16/12/2019 8:00:15 PM
 * Author : GSM Rana
 */ 

#include <avr/io.h>
#include "DriverIO.h"
#include "Handler_Button.h"
#include "Handler_Display.h"

int main(void)
{
    DriverIO_Init();
    DebugLog(tittle_text);	
    handler_display_init();
    handler_button_init();
	
    while (1) 
    {
  		handler_button_mannager();
  		handler_display_manager();
    }
}
