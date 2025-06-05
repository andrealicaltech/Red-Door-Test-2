#ifndef __OBSTACLE_H__
#define __OBSTACLE_H__

#include "body.h"
#include "math_utils.h"
#include "state.h"
#include "vector.h"

/*
Makes a rectangular-shaped obstacle of the specified height and width
*/
body_t *make_obstacle(size_t w, size_t h, vector_t center);

/*
Returns the width and height of an obstacle.
Depends on make_obstacle adding the bottom right corner third.
*/
vector_t get_obstacle_dims(body_t *obstacle);

/*
Calculation of the smallest distance before an obstacle
  at which the player can currently jump without colliding with the obstacle's
  vertical edge

`h_player`: distance between ground and player centroid
`h_obstacle`: distance between ground and top of obstacle
*/
double get_smallest_obst_clearing_dist(state_t *state, double h_player,
                                       double h_obstacle);

/*
Guarrantee that if the player jumps from the latest possible point to clear
the last obstacle in the queue, there is sufficient space before the next
obstacle for them to jump at the earliest possible point and clear the
obstacle.

`last_obstacle`: pointer to the most recently added obstacle, which should
currently be the last scheduled obstacle to appear
*/

double next_obst_x(state_t *state, body_t *last_obstacle);

/*
Called in the main loop.
Check if the timer to spawn the next obstacle has elapsed.
If so, add an obstacle to the edge of the far left of the screen and reset a
random timer
*/
void update_obstacles(state_t *state);

/*
Called in the main loop. Loops through all obstacles and removes those which
have passed the right edge of the screen. Assumes that the first object in the
state is the player
*/
void clean_obstacles(state_t *state);

#endif
