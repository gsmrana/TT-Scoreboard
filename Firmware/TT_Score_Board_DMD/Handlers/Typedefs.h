/*
 * Typedefs.h
 *
 * Created: 17/12/2019 11:27:33 PM
 *  Author: Rana
 */ 


#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include "DriverIO.h"

#define FIRMWARE_VERSION			100

#define MATCH_WIN_MIN_SCORE			11
#define MATCH_DEUCE_MIN_SCORE		10
#define MATCH_WIN_DEUCE_SCORE		2


enum _match_state
{
	MATCH_RUNNING,
	MATCH_DEUCE,
	MATCH_FINISHED
};

enum _player_side
{
	PLAYER_SIDE_LEFT,
	PLAYER_SIDE_RIGHT,
	PLAYER_SIDE_NONE
};

typedef struct _tt_team
{
	uint8_t match_score;
	uint8_t series_score;
} tt_team_t;

typedef struct _tt_game
{
	uint8_t app_mode;
	uint8_t display_state;
	uint8_t service_side;
	uint8_t match_state;
	tt_team_t *left_team;
	tt_team_t *right_team;
	uint8_t match_winner_side;
	uint8_t flash_winner_state;
	Timer_t digit_update_timer;
} tt_game_t;


#endif /* TYPEDEFS_H_ */