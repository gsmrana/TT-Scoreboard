/*
 * Handler_Display.cpp
 *
 * Created: 17/12/2019 11:15:19 PM
 *  Author: GSM Rana
 */ 

#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "DriverIO.h"
#include "DMD.h"
#include "Arial14.h"
#include "SystemFont5x7.h"
#include "Arial_black_16.h"
#include "Typedefs.h"
#include "Handler_Button.h"
#include "Handler_Display.h"

tt_game_t ttgame;
char tittle_text[] = "ReliSource TT Score Board, Powered by Firmware Team!";

static DMD dmd(1, 1);
static char digit_buffer[16];
static tt_team_t team_a;
static tt_team_t team_b;

static void handler_display_scan_isr();
static void handler_display_test_pixel_by_column();
static void handler_display_test_pixel_by_row();
static void handler_display_test_pattern();
static void handler_display_test_numbers();

void handler_display_init()
{
	ttgame.left_team = &team_a;
	ttgame.right_team = &team_b;
	dmd.clearScreen(false);
	DMD_SCAN_ISR__Init();	
	_delay_ms(1000);
	
	// enable this line to test the DMD pixels
	//handler_display_test_pixel_by_column();
	//handler_display_test_pixel_by_row();
	
	handler_display_test_pattern();
	handler_display_test_numbers();
	dmd.clearScreen(true);
}

static void handler_display_scan_isr()
{
	dmd.scanDisplayBySPI();
}

static void handler_display_test_pixel_by_column()
{
	dmd.clearScreen(true);
	for (uint8_t x = 0; x < DMD_PIXELS_ACROSS; x++)
	{
		for (uint8_t y = 0; y < DMD_PIXELS_DOWN; y++)
		{
			dmd.writePixel(x, y, GRAPHICS_NORMAL, true);
			_delay_ms(500);
			dmd.writePixel(x, y, GRAPHICS_NOR, true);
		}
	}
}

static void handler_display_test_pixel_by_row()
{
	dmd.clearScreen(true);
	for (uint8_t y = 0; y < DMD_PIXELS_DOWN; y++)
	{
		for (uint8_t x = 0; x < DMD_PIXELS_ACROSS; x++)
		{
			dmd.writePixel(x, y, GRAPHICS_NORMAL, true);
			_delay_ms(500);
			dmd.writePixel(x, y, GRAPHICS_NOR, true);
		}
	}
}

static void handler_display_test_pattern()
{
	dmd.drawTestPattern(PATTERN_STRIPE_0);
	_delay_ms(1000);
	dmd.drawTestPattern(PATTERN_STRIPE_1);
	_delay_ms(1000);
	dmd.drawTestPattern(PATTERN_ALT_0);
	_delay_ms(1000);
	dmd.drawTestPattern(PATTERN_ALT_1);
	_delay_ms(1000);
}

static void handler_display_test_numbers()
{
	dmd.clearScreen(true);
	dmd.selectFont(Arial_14);
	for (char i = '0'; i < '9'; i++)
	{
		dmd.drawChar(0, 0, i, GRAPHICS_NORMAL);
	}
}

void handler_display_match_winner()
{
	if (ttgame.left_team->match_score == MATCH_DUOS_MIN_SCORE &&
		ttgame.right_team->match_score == MATCH_DUOS_MIN_SCORE)
	{
		ttgame.match_state = MATCH_DEUCE;
	}

	if (ttgame.match_state == MATCH_DEUCE)
	{
		if (ttgame.left_team->match_score >= (ttgame.right_team->match_score + MATCH_WIN_DUOS_SCORE))
		{
			ttgame.left_team->series_score++;
			ttgame.match_winner_side = PLAYER_SIDE_LEFT;
			ttgame.match_state = MATCH_FINISHED;
		}
		if (ttgame.right_team->match_score >= (ttgame.left_team->match_score + MATCH_WIN_DUOS_SCORE))
		{
			ttgame.right_team->series_score++;
			ttgame.match_winner_side = PLAYER_SIDE_RIGHT;
			ttgame.match_state = MATCH_FINISHED;
		}
		
		// toggle service side
		if (ttgame.service_side == PLAYER_SIDE_LEFT)
			ttgame.service_side = PLAYER_SIDE_RIGHT;
		else
			ttgame.service_side = PLAYER_SIDE_LEFT;
	}
	else // before DEUCE
	{
		if (ttgame.left_team->match_score >= MATCH_WIN_MIN_SCORE)
		{
			ttgame.left_team->series_score++;
			ttgame.match_winner_side = PLAYER_SIDE_LEFT;
			ttgame.match_state = MATCH_FINISHED;
		}
		if (ttgame.right_team->match_score >= MATCH_WIN_MIN_SCORE)
		{
			ttgame.right_team->series_score++;
			ttgame.match_winner_side = PLAYER_SIDE_RIGHT;
			ttgame.match_state = MATCH_FINISHED;
		}
		
		// toggle service side
		if (((ttgame.left_team->match_score + ttgame.right_team->match_score) % 2) == 0)
		{
			if (ttgame.service_side == PLAYER_SIDE_LEFT)
				ttgame.service_side = PLAYER_SIDE_RIGHT;
			else
				ttgame.service_side = PLAYER_SIDE_LEFT;
		}
	}
}

void handler_display_manager()
{
	switch (ttgame.app_mode)
	{
		case APP_MODE_TITTLE:
			if (ttgame.display_state == DISPLAY_INIT)
			{
				dmd.clearScreen(true);
				dmd.selectFont(Arial_Black_16);
				dmd.drawMarquee(tittle_text, strlen(tittle_text), DMD_PIXELS_ACROSS - 1, 0);
				ttgame.digit_update_timer = TITTLE_SCROLL_TIME_MS + Millis;
				ttgame.display_state = DISPLAY_UPDATE;
			}
			else if (ttgame.display_state == DISPLAY_UPDATE)
			{
				if (Millis > ttgame.digit_update_timer)
				{
					if (dmd.stepMarquee(-1, 0))
					ttgame.display_state = DISPLAY_INIT;
					else
					ttgame.digit_update_timer = TITTLE_SCROLL_TIME_MS + Millis;
				}
			}
			break;

		case APP_MODE_MATCH:
			if (ttgame.display_state == DISPLAY_INIT)
			{
				dmd.clearScreen(true);
				dmd.selectFont(System5x7);
				dmd.drawLine((DMD_PIXELS_ACROSS / 2) - 1, 0, (DMD_PIXELS_ACROSS / 2) - 1, DMD_PIXELS_DOWN - 1, GRAPHICS_NORMAL);
				ttgame.digit_update_timer = 0;
				ttgame.display_state = DISPLAY_UPDATE;
			}
			if (ttgame.display_state == DISPLAY_UPDATE)
			{
				if (Millis > ttgame.digit_update_timer)
				{
					bool need_to_update_score = true;
					
					//flash the winner score
					if (ttgame.match_state == MATCH_FINISHED)
					{
						ttgame.flash_winner_state = !ttgame.flash_winner_state;
						if (ttgame.flash_winner_state && ttgame.match_winner_side == PLAYER_SIDE_LEFT)
						{
							dmd.drawChar(1, 9, ' ', GRAPHICS_NORMAL);
							dmd.drawChar(7, 9, ' ', GRAPHICS_NORMAL);
							need_to_update_score = false;
						}
						else if (ttgame.flash_winner_state && ttgame.match_winner_side == PLAYER_SIDE_RIGHT)
						{
							dmd.drawChar(19, 9, ' ', GRAPHICS_NORMAL);
							dmd.drawChar(25, 9, ' ', GRAPHICS_NORMAL);
							need_to_update_score = false;
						}
					}
					
					//update the match score
					if (need_to_update_score)
					{
						sprintf(digit_buffer, "%02u|%02u,%02u|%02u",
							ttgame.left_team->series_score,
							ttgame.right_team->series_score,
							ttgame.left_team->match_score,
							ttgame.right_team->match_score
						);
						dmd.drawChar(4, 0, digit_buffer[1], GRAPHICS_NORMAL);
						dmd.drawChar(22, 0, digit_buffer[4], GRAPHICS_NORMAL);
						dmd.drawChar(1, 9, digit_buffer[6], GRAPHICS_NORMAL);
						dmd.drawChar(7, 9, digit_buffer[7], GRAPHICS_NORMAL);
						dmd.drawChar(19, 9, digit_buffer[9], GRAPHICS_NORMAL);
						dmd.drawChar(25, 9, digit_buffer[10], GRAPHICS_NORMAL);
					}
					
					//service side icon display
					if (ttgame.service_side == PLAYER_SIDE_LEFT)
					{
						dmd.drawFilledBox(DMD_PIXELS_ACROSS - 2, 0, DMD_PIXELS_ACROSS - 1, 1, GRAPHICS_NOR);
						dmd.drawFilledBox(0, 0, 1, 1, GRAPHICS_NORMAL);
					}
					else
					{
						dmd.drawFilledBox(0, 0, 1, 1, GRAPHICS_NOR);
						dmd.drawFilledBox(DMD_PIXELS_ACROSS - 2, 0, DMD_PIXELS_ACROSS - 1, 1, GRAPHICS_NORMAL);						
					}
					ttgame.digit_update_timer = SCORE_UPDATE_TIME_MS + Millis;
				}
			}
			break;
		
		case APP_MODE_TIMER:
			if (ttgame.display_state == DISPLAY_INIT)
			{
				dmd.clearScreen(true);
				dmd.selectFont(Arial_14);
				ttgame.digit_update_timer = 0;
				ttgame.display_state = DISPLAY_UPDATE;
			}
			if (ttgame.display_state == DISPLAY_UPDATE)
			{
				if (Millis > ttgame.digit_update_timer)
				{
					sprintf(digit_buffer, "%02u:%02u", softrtc.minute, softrtc.second);
					dmd.drawChar(1, 2, digit_buffer[0], GRAPHICS_NORMAL);
					dmd.drawChar(8, 2, digit_buffer[1], GRAPHICS_NORMAL);
					dmd.drawChar(17, 2, digit_buffer[3], GRAPHICS_NORMAL);
					dmd.drawChar(24, 2, digit_buffer[4], GRAPHICS_NORMAL);
					if (softrtc.colon_on)
						dmd.drawChar(15, 1, ':', GRAPHICS_OR);
					else
						dmd.drawChar(15, 1, ':', GRAPHICS_NOR);
					ttgame.digit_update_timer = CLOCK_UPDATE_TIME_MS + Millis;
				}
			}
			break;
	}
}

ISR(DMD_SCAN_ISR_VECT)
{
	handler_display_scan_isr();
}