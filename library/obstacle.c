#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "asset.h"
#include "asset_cache.h"
#include "background.h"
#include "collision.h"
#include "constants.h"
#include "forces.h"
#include "game_state.h"
#include "kinematics.h"
#include "obstacle.h"

const double EDGE_TOLERANCE = 3.0;
const double FACTOR = 0.5;

// Need this to estimate tolerance for landing on top of an obstacle
// - tolerance for when the function was last called
const double AVG_DT_S = 0.05;

vector_t get_obstacle_dims(body_t *obstacle) {
  assert(strcmp(body_get_info(obstacle), OBSTACLE_INFO) == 0);
  vector_t *top_left = list_get(body_get_shape(obstacle), 0);
  vector_t *bottom_right = list_get(body_get_shape(obstacle), 2);
  double width = bottom_right->x - top_left->x;
  double height = bottom_right->y - top_left->y;
  assert(width > 0 && height > 0);
  free(top_left);
  free(bottom_right);
  return (vector_t){.x = width, .y = height};
}

void obstacle_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                                void *aux, double force_const) {
  bool b1_is_player = strcmp(body_get_info(body1), PLAYER_INFO) == 0;
  body_t *player = b1_is_player ? body1 : body2;
  body_t *obstacle = b1_is_player ? body2 : body1;

  state_t *state = (state_t *)aux;
  vector_t player_centroid = body_get_centroid(player);
  vector_t obstacle_centroid = body_get_centroid(obstacle);
  vector_t obstacle_dims = get_obstacle_dims(obstacle);

  vector_t dim = vec_multiply(FACTOR, PLAYER_DIMS);
  vector_t ob_dim = vec_multiply(FACTOR, obstacle_dims);
  double player_right_edge = player_centroid.x + dim.x;
  double obstacle_left_edge = obstacle_centroid.x - ob_dim.x;

  double y_gap = (player_centroid.y - dim.y) - (obstacle_centroid.y + ob_dim.y);
  if (player_right_edge >= obstacle_left_edge &&
      fabs(y_gap) <= (get_curr_gravity(state).y * AVG_DT_S)) {
    body_set_velocity(player, VEC_ZERO);
    state->player_motion = REGULAR;
    state->curr_player_obstacle = obstacle;
    state->jump_start_y = PLAYER_CENTER_POS.y;
  } else if (player_right_edge > obstacle_left_edge ||
             obstacle_left_edge - player_right_edge < EDGE_TOLERANCE) {
    state->is_game_over = true;
    body_remove(obstacle);
    state->n_queued_obstacles -= 1;
  }
}

void check_player_falling_off_edge(state_t *state) {
  body_t *obstacle = state->curr_player_obstacle;
  if (!obstacle) {
    return;
  }

  // Fall off when the majority of the player's body is falling off
  vector_t player_centroid = body_get_centroid(state->player);
  vector_t obstacle_centroid = body_get_centroid(obstacle);

  double obstacle_right_edge =
      (obstacle_centroid.x + (FACTOR * get_obstacle_dims(obstacle).x));
  double edge_dist = obstacle_right_edge - player_centroid.x;

  if (edge_dist < EDGE_TOLERANCE) {
    state->curr_player_obstacle = NULL;
    state->player_motion = FALLING;
    state->jump_start_y = PLAYER_CENTER_POS.y;
  }
}

double next_obst_x(state_t *state, body_t *last_obstacle) {
  vector_t last_obstacle_dims = get_obstacle_dims(last_obstacle);
  vector_t last_obstacle_centroid = body_get_centroid(last_obstacle);

  vector_t curr_obst_speed = state->bg.bg_3_building_vel;

  double smallest_clearing_dist_before_obst = get_smallest_obst_clearing_dist(
      state, PLAYER_DIMS.y, last_obstacle_centroid.y);
  double expected_x_dist_with_jump =
      (2 * get_curr_jump_vel(state).y / get_curr_gravity(state).y) *
      curr_obst_speed.x;

  /*
  Suppose we jump at the minimum distance before the obstacle.
  2 possibilities depending on the width of the obstacle:
  1. We clear the obstacle completely: x_dist_with_jump >
  smallest_clearing_dist_before_obst + obst_width. Calculate corresponding
  landing point.
  2. Land on top of the obstacle. Give enough space to fall off the right edge
  obstacle (the player has no control during this). Now just need to give
  arbitrary max(reaction_space, running space + clearing_space) before the next
  obstacle
  */
  double final_x = 0.0;
  vector_t last_dim = vec_multiply(FACTOR, last_obstacle_dims);
  if (expected_x_dist_with_jump >
      smallest_clearing_dist_before_obst + last_obstacle_dims.x) {
    final_x = (last_obstacle_centroid.x - last_dim.x -
               smallest_clearing_dist_before_obst) +
              expected_x_dist_with_jump;

  } else {
    double fall_time =
        sqrt(2 * last_obstacle_dims.y / get_curr_gravity(state).y);
    final_x =
        last_obstacle_centroid.x + last_dim.x + (fall_time * curr_obst_speed.x);
  }

  // Additional random spacing between obstacles
  double running_space = rand() % ((int)(MAX.x));
  // if running space is low, need to guarantee that we give the player enough
  // distance to jump such that they clear the height of the obstacle
  double clearing_space =
      get_smallest_obst_clearing_dist(state, PLAYER_DIMS.y, OBSTACLE_HW);

  // It is possible that the spacing is small enough that it doesn't given
  // reasonable reaction time for a player
  return final_x + max_d(running_space + clearing_space,
                         MIN_REACTION_TIME_S * curr_obst_speed.x);
}

body_t *get_nth_obstacle(state_t *state, size_t n) {
  size_t count = -1;
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (strcmp(body_get_info(body), OBSTACLE_INFO) == 0) {
      count++;
    }
    if (count == n) {
      return body;
    }
  }

  return NULL;
}

void update_obstacles(state_t *state) {
  size_t n_stacked = (size_t)(1 + (rand() % (MAX_STACKED_OBSTACLES - 1)));
  size_t width = n_stacked * OBSTACLE_HW;
  size_t height = OBSTACLE_HW;

  // Default value is edge of the screen
  double x = MAX.x;
  double y =
      (PLAYER_CENTER_POS.y - PLAYER_DIMS.y / 2) + (OBSTACLE_HW / 2); // TODO

  if (state->n_queued_obstacles > 0) {
    // Use 1 + n_q - 1 for clarity: first body is player. Subtract 1 for
    // zero-indexing
    body_t *last_obstacle =
        get_nth_obstacle(state, state->n_queued_obstacles - 1);
    x = next_obst_x(state, last_obstacle);
  }

  vector_t new_centroid = (vector_t){.x = (FACTOR * width) + x, .y = y};
  body_t *new_obstacle =
      make_object(width, height, new_centroid, (void *)OBSTACLE_INFO);

  scene_add_body(state->scene, new_obstacle);
  char *path = malloc(sizeof(char) * strlen(OBSTACLE_GEN_PATH_TEMPLATE) +
                      (log10(n_stacked) + 1) + strlen(OBSTACLE_IMG_EXT) + 1);
  sprintf(path, "%s%zu%s", OBSTACLE_GEN_PATH_TEMPLATE, n_stacked,
          OBSTACLE_IMG_EXT);
  asset_make_image_with_body(path, new_obstacle);
  body_set_velocity(new_obstacle,
                    vec_multiply(-1, state->bg.bg_3_building_vel));

  body_t *player_body = get_player(state);
  create_collision(state->scene, player_body, new_obstacle,
                   obstacle_collision_handler, state, 0, NULL);

  state->n_queued_obstacles += 1;
}

void clean_obstacles(state_t *state) {
  if (scene_bodies(state->scene) == 1) {
    return;
  }
  for (size_t i = 1; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (strcmp(body_get_info(body), OBSTACLE_INFO) == 0) {
      vector_t obst_pos = body_get_centroid(body);
      vector_t obst_dims = get_obstacle_dims(body);
      if (obst_pos.x + (obst_dims.x / 2) < MIN.x) {
        state->n_queued_obstacles -= 1;
        body_remove(body);

        // This should never happen if obstacle generation and edge detection is
        // correct
        if (state->curr_player_obstacle == body) {
          state->curr_player_obstacle = NULL;
        }
      } else {
        body_set_velocity(body, vec_multiply(-1, state->bg.bg_3_building_vel));
      }
    }
  }
}