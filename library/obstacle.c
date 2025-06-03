#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "collision.h"
#include "constants.h"
#include "game_state.h"
#include "kinematics.h"
#include "obstacle.h"

body_t *make_obstacle(size_t w, size_t h, vector_t center) {
  list_t *c = list_init(4, free);
  vector_t *v1 = malloc(sizeof(vector_t));
  *v1 = (vector_t){0, 0};
  list_add(c, v1);

  vector_t *v2 = malloc(sizeof(vector_t));
  *v2 = (vector_t){w, 0};
  list_add(c, v2);

  vector_t *v3 = malloc(sizeof(vector_t));
  *v3 = (vector_t){w, h};
  list_add(c, v3);

  vector_t *v4 = malloc(sizeof(vector_t));
  *v4 = (vector_t){0, h};
  list_add(c, v4);
  body_t *obstacle = body_init(c, 1, OBS_COLOR);
  body_set_centroid(obstacle, center);
  return obstacle;
}

vector_t get_obstacle_dims(body_t *obstacle) {
  assert(strcmp(body_get_info(obstacle), OBSTACLE_INFO) == 0);
  return *((vector_t *)list_get(body_get_shape(obstacle), 3));
}

double get_smallest_obst_clearing_dist(state_t *state, double h_player,
                                       double h_obstacle) {
  double u = get_curr_jump_vel(state).y;
  double min_del_h = h_obstacle - h_player;
  /*

  Find the time which results in the bottom of the player exactly hitting the
  top edge of the obstacle Solve for t in the y-axis h_o - h_p = ut - 0.5gt^2
  which gives (u + sqrt(u^2 - 2g(h_o-h_p)))/g
  */
  double time = (u + sqrt(u * u - 2 * Y_GRAV_ACCELERATION_MAG_PER_S * min_del_h)) /
                Y_GRAV_ACCELERATION_MAG_PER_S;
  double vx = state->bg.bg_3_building_vel.x;

  // Return the x-distance that will be covered in that time
  return vx * time;
}

double next_obst_x(state_t *state, body_t *last_obstacle) {
  vector_t last_obstacle_dims = get_obstacle_dims(last_obstacle);
  vector_t last_obstacle_centroid = body_get_centroid(last_obstacle);
  vector_t curr_obst_speed = state->bg.bg_3_building_vel;

  double expected_x_dist_with_jump =
      (2 * get_curr_jump_vel(state).y / Y_GRAV_ACCELERATION_MAG_PER_S) *
      curr_obst_speed.x;
  double furthest_poss_x =
      (last_obstacle_centroid.x - last_obstacle_dims.x -
       get_smallest_obst_clearing_dist(state, PLAYER_DIMS.y,
                                       last_obstacle_centroid.y)) +
      expected_x_dist_with_jump;

  // Additional random spacing between obstacles
  double running_space = rand() % ((int)MAX.x);
  // if running space is low, need to guarantee that we give the player enough
  // distance to jump such that they clear the height of the obstacle
  double clearing_space =
      get_smallest_obst_clearing_dist(state, PLAYER_DIMS.y, OBSTACLE_HW);

  // It is possible that this sum is small enough that it doesn't given
  // reasonable reaction time for a player
  return max_d(furthest_poss_x + running_space + clearing_space,
               MIN_REACTION_TIME_S * curr_obst_speed.x);
}

void update_obstacles(state_t *state) {
  if (state->time_till_next_obstacle <= 0.0) {
    double width = (rand() % MAX_STACKED_OBSTACLES) * OBSTACLE_HW;
    double height = OBSTACLE_HW;

    if (scene_bodies(state->scene) == 1){
      return;
    }

    body_t *last_obstacle =
        scene_get_body(state->scene, 1 + state->n_queued_obstacles);
    vector_t new_centroid = (vector_t){
        .x = (body_get_centroid(last_obstacle)).x +
             next_obst_x(state, last_obstacle),
        .y = (PLAYER_CENTER_POS.y - PLAYER_DIMS.y / 2) + OBSTACLE_HW // TODO
    };
    body_t *new_obstacle = make_obstacle(width, height, new_centroid);
    body_set_velocity(new_obstacle, state->bg.bg_3_building_vel);

    state->n_queued_obstacles += 1;
    state->time_till_next_obstacle = mod_d((double)rand(), AVG_TIME_OBSTACLES);
  }
}

// Arjun
void clean_obstacles(state_t *state) {
  if (scene_bodies(state->scene) == 1){
    return;
  }
  // First obstacle is always the player
  for (size_t i = 1; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (strcmp(body_get_info(body), OBSTACLE_INFO) == 0) {
      vector_t obst_pos = body_get_centroid(body);
      if (obst_pos.x > MAX.x) {
        body_free(body);
        state->n_queued_obstacles -= 1;
      }
    }
  }
}