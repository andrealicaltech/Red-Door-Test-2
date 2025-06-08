#ifndef __MAGNET_H__
#define __MAGNET_H__

#include "forces.h"
#include "game_state.h"

body_t *make_magnet(vector_t center);

void apply_magnet(state_t *state, double dt);

void magnet_body_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                                   void *aux, double force_const);

#endif