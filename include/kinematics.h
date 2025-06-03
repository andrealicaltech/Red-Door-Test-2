#ifndef __KINEMATICS_H__
#define __KINEMATICS_H__

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "state.h"
#include "vector.h"


void revert_duck(state_t *state);

vector_t get_curr_jump_vel(state_t *state);

void on_key(char key, key_event_type_t type, double held_time, state_t *state);

void manipulate_player(state_t *state, double dt);

#endif