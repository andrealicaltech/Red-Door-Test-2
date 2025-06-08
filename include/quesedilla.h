#ifndef __QUESEDILLA_H__
#define __QUESEDILLA_H__

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "collision.h"
#include "constants.h"
#include "forces.h"
#include "game_state.h"
#include "kinematics.h"
#include "list.h"

double coin_spacing(state_t *state);

/*

Returns owned list of coins with regular spacing and random x-offset in the
given interval Min length of list is 3
*/
void gen_coin_arc(state_t *state, bool should_require_powerup);

void clean_coins(state_t *state);
#endif