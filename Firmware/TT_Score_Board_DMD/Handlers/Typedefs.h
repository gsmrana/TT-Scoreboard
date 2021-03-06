/*
 * Typedefs.h
 *
 * Created: 17/12/2019 11:27:33 PM
 *  Author: Rana
 */ 


#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include "DriverIO.h"

#define FIRMWARE_VERSION			102

#define TT_SET_WIN_MIN_SCORE		11
#define TT_SET_DEUCE_MIN_SCORE		10
#define TT_SET_WIN_DEUCE_SCORE		2


enum _tt_set_state
{
	TT_SET_RUNNING,
	TT_SET_DEUCE,
	TT_SET_FINISHED
};

enum _player_side
{
	PLAYER_SIDE_LEFT = 0,
	PLAYER_SIDE_RIGHT = 1,
	PLAYER_SIDE_COUNT = 2,
	PLAYER_SIDE_NONE = 0xFF
};

typedef struct _tt_team
{
	uint8_t current_set_score;
	uint8_t series_score;
} tt_team_t;

typedef struct _tt_game
{
	uint8_t app_mode;
	uint8_t display_state;
	tt_team_t *left_team;
	tt_team_t *right_team;
	uint8_t tt_set_state;
	uint8_t auto_side_swap;
	uint8_t initial_service_side;
	uint8_t current_service_side;	
	uint8_t tt_set_winner_side;
	uint8_t flash_winner_state;
	Timer_t digit_update_timer;
} tt_game_t;


#endif /* TYPEDEFS_H_ */