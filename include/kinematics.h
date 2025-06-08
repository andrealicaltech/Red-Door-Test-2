#ifndef __KINEMATICS_H__
#define __KINEMATICS_H__

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "state.h"
#include "vector.h"

void revert_duck(state_t *state);

vector_t get_curr_gravity(state_t *state);
vector_t get_curr_jump_vel(state_t *state);

void on_key(char key, key_event_type_t type, double held_time, state_t *state);

void manipulate_player(state_t *state, double dt);

/*
Calculation of the smallest distance before an obstacle
  at which the player can currently jump without colliding with the obstacle's
  vertical edge

`h_player`: distance between ground and player centroid
`h_obstacle`: distance between ground and top of obstacle
*/
double get_smallest_obst_clearing_dist(state_t *state, double h_player,
                                       double h_obstacle);

#endif