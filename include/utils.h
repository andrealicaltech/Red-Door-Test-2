#ifndef __MATH_H__
#define __MATH_H__

#include "game_state.h"
/*
Equivalent to a % b for double types. Cast (a/b) to an int and multiply that by
b. Return difference from a
*/
double mod_d(double a, double b);

/*
Returns minimum value of two doubles and b
*/
double min_d(double a, double b);

/*
Returns maximum value of two doubles and b
*/
double max_d(double a, double b);

char *get_player_sprite_duck_path(state_t *state);

char *get_player_sprite_normal_path(state_t *state);

#endif