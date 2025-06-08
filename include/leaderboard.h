#ifndef __LEADERBOARD_H__
#define __LEADERBOARD_H__

#include "body.h"
#include "game_state.h"
/*
Makes a scoreboard showing the player's score as they progress through a level.
*/
void make_scoreboard(double width, double height, vector_t center,
                        color_t color);

void render_scoreboard(state_t *state);

void update_text(state_t *state) 

#endif