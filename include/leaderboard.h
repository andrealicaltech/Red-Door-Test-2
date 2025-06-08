#ifndef __LEADERBOARD_H__
#define __LEADERBOARD_H__

#include "body.h"
#include "game_state.h"

void create_reg_text(TTF_Font *font, vector_t top_left, vector_t size,
                     char *text, double count);

/*
Makes a scoreboard showing the player's score as they progress through a level.
*/
void make_scoreboard(double width, double height, vector_t center,
                     color_t color);

void render_score_text(state_t *state);

void render_coin_text(state_t *state);

void update_score(state_t *state);
#endif