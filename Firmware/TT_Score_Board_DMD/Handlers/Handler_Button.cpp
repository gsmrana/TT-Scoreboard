/*
 * Handler_Button.cpp
 *
 * Created: 17/12/2019 11:14:48 PM
 *  Author: Rana
 */ 

#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Typedefs.h"
#include "DriverIO.h"
#include "Handler_Button.h"
#include "Handler_Display.h"

volatile softrtc_t softrtc;
volatile remote_input_t remote;

static void handler_softrtc_update_isr();
static void handler_remote_button_press_isr();
static void handler_remote_long_press_debounce();
static void handler_button_short_press();
static void handler_button_long_press();

void handler_button_init()
{
	Button_GPIO_ISR__Init();	
}

void handler_button_check_on_startup()
{
	if(!REMOTE_VT_IO_PIN)
		return;
	
	// set the display brightness based on button press
	if(REMOTE_BUTTON_IO_PINS & REMOTE_BUTTON_C)
	{
		handler_display_set_brightness(30);		
	}
	else if(REMOTE_BUTTON_IO_PINS & REMOTE_BUTTON_D)
	{
		handler_display_set_brightness(50);
	}
	
	_delay_ms(1000);
	remote.button_press_backup = BUTTON_CLEAR;
	remote.button_short_press = BUTTON_CLEAR;
	remote.button_long_press = BUTTON_CLEAR;
}

static void handler_softrtc_update_isr()
{
	if(ttgame.match_state == MATCH_FINISHED)
	{
		softrtc.colon_on = true;
		return;
	}
	
	softrtc.second++;
	softrtc.colon_on = !softrtc.colon_on;	
	if (softrtc.second >= 60)
	{
		softrtc.minute++;
		softrtc.second = 0;
	}
	if (softrtc.minute >= 60)
	{
		softrtc.minute = 0;
	}
}

static void handler_remote_button_press_isr()
{
	uint8_t vtstate = REMOTE_VT_IO_PIN ? 1 : 0;	
	if(vtstate) //rising edge, button downed
	{
		remote.button_press_backup = REMOTE_BUTTON_IO_PINS;
		remote.debounce_long_timer = BUTTON_LONG_DEBOUNCE_TIME_MS + Millis;
	}
	else //falling edge, button released
	{
		if(remote.button_press_backup != BUTTON_CLEAR)
		{
			remote.button_short_press = remote.button_press_backup;
			remote.button_press_backup = BUTTON_CLEAR;
		}
	}
	
	DebugLog("\rremote_isr: %d, %.2X, ", vtstate, remote.button_short_press);
}

static void handler_remote_long_press_debounce()
{
	if (remote.button_press_backup != BUTTON_CLEAR)
	{
		if(remote.button_press_backup == REMOTE_BUTTON_IO_PINS && Millis > remote.debounce_long_timer)
		{
			if(++remote.debounce_long_count >= BUTTON_LONG_DEBOUNCE_COUNT)
			{
				remote.button_long_press = remote.button_press_backup;
				remote.button_press_backup = BUTTON_CLEAR;
			}
			remote.debounce_long_timer = BUTTON_LONG_DEBOUNCE_TIME_MS + Millis;
		}
	}
}

static void handler_button_short_press()
{
	if (remote.button_short_press & REMOTE_BUTTON_A)
	{
		remote.button_short_press &= ~REMOTE_BUTTON_A;
		DebugLog("Short Press: A");
		if (ttgame.app_mode == APP_MODE_MATCH && ttgame.match_state < MATCH_FINISHED)
		{
			// left team score increment
			ttgame.left_team->match_score++;			
			handler_display_update_match_state();
		}
	}
	else if (remote.button_short_press & REMOTE_BUTTON_B)
	{
		remote.button_short_press &= ~REMOTE_BUTTON_B;	
		DebugLog("Short Press: B");
		if (ttgame.app_mode == APP_MODE_MATCH && ttgame.match_state < MATCH_FINISHED)
		{
			// right team score increment
			ttgame.right_team->match_score++;
			handler_display_update_match_state();
		}
	}
	else if (remote.button_short_press & REMOTE_BUTTON_C)
	{
		remote.button_short_press &= ~REMOTE_BUTTON_C;	
		DebugLog("Short Press: C");
		
		// start new match
		ttgame.left_team->match_score = 0;
		ttgame.right_team->match_score = 0;
		ttgame.initial_service_side = !ttgame.initial_service_side;
		ttgame.current_service_side = ttgame.initial_service_side;
		ttgame.match_winner_side = PLAYER_SIDE_NONE;
		ttgame.match_state = MATCH_RUNNING;
		softrtc.minute = 0;
		softrtc.second = 0;
	}
	else if (remote.button_short_press & REMOTE_BUTTON_D)
	{
		remote.button_short_press &= ~REMOTE_BUTTON_D;	
		DebugLog("Short Press: D");
		
		// switch display app modes
		ttgame.app_mode++;
		if (ttgame.app_mode >= MAX_APP_MODE)
		{
			if(ttgame.left_team->match_score > 0 || ttgame.right_team->match_score > 0 ||
				ttgame.left_team->series_score > 0 || ttgame.right_team->series_score > 0)
				ttgame.app_mode = APP_MODE_MATCH;
			else ttgame.app_mode = APP_MODE_TITTLE;
		}
		ttgame.display_state = DISPLAY_INIT;
	}
}

static void handler_button_long_press()
{
	if (remote.button_long_press & REMOTE_BUTTON_A)
	{
		remote.button_long_press &= ~REMOTE_BUTTON_A;
		DebugLog("Long Press: A");
		if (ttgame.app_mode == APP_MODE_MATCH && ttgame.match_state < MATCH_FINISHED && 
			ttgame.left_team->match_score > 0)
		{
			// left team score decrement
			ttgame.left_team->match_score--;
			handler_display_update_match_state();
		}
	}
	else if (remote.button_long_press & REMOTE_BUTTON_B)
	{
		remote.button_long_press &= ~REMOTE_BUTTON_B;
		DebugLog("Long Press: B");
		if (ttgame.app_mode == APP_MODE_MATCH && ttgame.match_state < MATCH_FINISHED && 
			ttgame.right_team->match_score > 0)
		{
			// right team score decrement
			ttgame.right_team->match_score--;
			handler_display_update_match_state();
		}		
	}
	else if (remote.button_long_press & REMOTE_BUTTON_C)
	{
		remote.button_long_press &= ~REMOTE_BUTTON_C;	
		DebugLog("Long Press: C");
		
		// clear series score
		ttgame.left_team->match_score = 0;
		ttgame.right_team->match_score = 0;
		ttgame.left_team->series_score = 0;
		ttgame.right_team->series_score = 0;
		ttgame.initial_service_side = PLAYER_SIDE_LEFT;
		ttgame.current_service_side = ttgame.initial_service_side;
		ttgame.match_winner_side = PLAYER_SIDE_NONE;
		ttgame.match_state = MATCH_RUNNING;
		softrtc.minute = 0;
		softrtc.second = 0;
	}	
	else if (remote.button_long_press & REMOTE_BUTTON_D)
	{
		remote.button_long_press &= ~REMOTE_BUTTON_D;
		DebugLog("Long Press: D");		
		
		// toggle team side
		ttgame.initial_service_side = !ttgame.initial_service_side;				
		ttgame.current_service_side = !ttgame.current_service_side;			
		
		tt_team_t *tt_team_ptr = ttgame.left_team;
		ttgame.left_team = ttgame.right_team;
		ttgame.right_team = tt_team_ptr;
		
		if(ttgame.match_winner_side == PLAYER_SIDE_LEFT)
			ttgame.match_winner_side = PLAYER_SIDE_RIGHT;
		else if(ttgame.match_winner_side == PLAYER_SIDE_RIGHT)
			ttgame.match_winner_side = PLAYER_SIDE_LEFT;
	}
}

void handler_button_mannager()
{
	if (Millis > softrtc.isr_timer)
	{
		handler_softrtc_update_isr();
		softrtc.isr_timer = RTC_ISR_UPDATE_TIME_MS + Millis;
	}
	
	handler_button_short_press();
	handler_button_long_press();
	handler_remote_long_press_debounce();
}

ISR(REMOTE_VT_ISR_VECT)
{
	handler_remote_button_press_isr();
}

