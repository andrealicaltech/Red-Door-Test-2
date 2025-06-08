#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <stdlib.h>

#include "color.h"
#include "vector.h"

/*
MARK: Constants
*/
extern const vector_t MIN;
extern const vector_t MAX;
extern const vector_t LEADERBOARD_SIZE;

extern const vector_t PLAYER_DIMS;
extern const vector_t PLAYER_CENTER_POS;
extern const char *PLAYER_INFO;

extern const double OUTER_RADIUS;
extern const double INNER_RADIUS;

extern const double MIN_REACTION_TIME_S;

// obstacle;
extern const size_t OBSTACLE_HW;
extern const size_t MAX_STACKED_OBSTACLES;
extern const size_t MAX_N_QUEUED_OBST;
extern const double FIRST_OBSTACLE_WAIT_TIME;
extern const size_t OBSTACLE_CAPACITY;
extern const double MAX_TIME_UPDATE;
extern const char *OBSTACLE_INFO;
extern const char *MAGNET_INFO;
extern const char *COIN_INFO;
extern const double MAGNET_FREQUENCY;
extern const double MAGNET_PERIOD_S;

// pts of player depending on action
extern const size_t PLAYER_STANDING_PTS;
extern const size_t PLAYER_RUNNING;
extern const size_t QUESADILLA_PTS;

// player movements
extern const vector_t DUCK_INITIAL_VELOCITY;
extern const vector_t JUMP_INITIAL_VELOCITY;
extern const double DUCK_ACCELERATION_CHANGE;

// Background and obstacle velocity. Background 3 is the top layer (i.e. the
// velocity of the game)
extern const vector_t INIT_BACKGROUND_1_SKY_VELOCITY;
extern const vector_t INIT_BACKGROUND_2_TREE_VELOCITY;
extern const vector_t INIT_BACKGROUND_3_BUILDINGS_VELOCITY;

extern const double JUMP_VEL_COMPONENT_RATIO;
extern const double GRAV_JUMP_VEL_RATIO;

extern const color_t SPRITE_COLOR;
extern const color_t OBS_COLOR;
extern const color_t COIN_COLOR;
extern const color_t LEADERBOARD_COLOR;
extern const color_t MAGNET_COLOR;
extern const color_t TEXT_COLOR;

extern const size_t BODY_ASSETS;
extern const char *START_SCREEN_PATH;
extern const char *GAME_OVER_PATH;

extern const char *PLAYER_SPRITE_AMUDHAN_PATH;
extern const char *PLAYER_SPRITE_ANDREA_PATH;
extern const char *PLAYER_SPRITE_ARJUN_PATH;
extern const char *BUILDING_PATH;
extern const char *SHOP_PATH;
extern const char *TREE_PATH;
extern const char *SKY_PATH;
extern const char *OBSTACLE_SPRITE_PATH;
extern const char *OBSTACLE_SPRITE_PATH;
extern const char *QUESADILLA_PATH;
extern const char *GAME_MUSIC_PATH;
extern const char *JUMP_MUSIC_PATH;
extern const char *MENU_MUSIC_PATH;
extern const double OBSTACLE_START_WAIT_TIME;
extern const double GAME_OVER_WAIT_TIME;

extern const size_t MAX_GAMES;

#endif