#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "asset.h"
#include "asset_cache.h"
#include "collision.h"
#include "constants.h"
#include "forces.h"
#include "game_state.h"
#include "kinematics.h"
#include "obstacle.h"

// TODOs: remove
#include <emscripten.h>


const double EDGE_TOLERANCE = 3.0;
const size_t MAX_N_QUEUED_OBST = 15;

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
  body_t *obstacle =
      body_init_with_info(c, 1, OBS_COLOR, (void *)OBSTACLE_INFO, NULL);
  body_set_centroid(obstacle, center);
  printf("Made obstacle w=%f, h=%f, center.x=%f\n", w, h, center.x);
  return obstacle;
}

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

  double player_right_edge = player_centroid.x + 0.5 * PLAYER_DIMS.x;
  double obstacle_left_edge = obstacle_centroid.x - (0.5 * obstacle_dims.x);

  double delta = (player_centroid.y - 0.5 * PLAYER_DIMS.y) -
                 (obstacle_centroid.y + 0.5 * obstacle_dims.y);
  if (fabs(delta) <= EDGE_TOLERANCE) {
    body_set_velocity(player, VEC_ZERO);
    state->player_motion = REGULAR;
    state->curr_player_obstacle = obstacle;
    state->jump_start_y = PLAYER_CENTER_POS.y;
    printf("Stuck to the top of obstacle_centroid.x=%f, obstacle_dims.x=%f\n",
           obstacle_centroid.x, obstacle_dims.x);
  } else if (player_right_edge > obstacle_left_edge ||
             obstacle_left_edge - player_right_edge < EDGE_TOLERANCE) {
    printf("head-on collision-you lose!\n");
    emscripten_force_exit(2);
    state->is_game_over = true;
  } else {
    printf("Can't handle collision\n");
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
      (obstacle_centroid.x + (0.5 * get_obstacle_dims(obstacle).x));
  double edge_dist = obstacle_right_edge - player_centroid.x;

  // printf("obstacle_right_edge=%f, edge_dist=%f, EDGE_TOL=%f\n",
  //        obstacle_right_edge, edge_dist, EDGE_TOLERANCE);

  if (edge_dist < EDGE_TOLERANCE) {
    state->curr_player_obstacle = NULL;
    state->player_motion = FALLING;
    state->jump_start_y = PLAYER_CENTER_POS.y;
  }
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
  double time = (u + sqrt(u * u - 2 * get_curr_gravity(state).y * min_del_h)) /
                get_curr_gravity(state).y;
  double vx = state->bg.bg_3_building_vel.x;

  // Return the x-distance that will be covered in that time
  return vx * time;
}

double next_obst_x(state_t *state, body_t *last_obstacle) {
  vector_t last_obstacle_dims = get_obstacle_dims(last_obstacle);
  vector_t last_obstacle_centroid = body_get_centroid(last_obstacle);
  double last_obst_end =
      max_d(last_obstacle_centroid.x + last_obstacle_dims.x, MAX.x);
  vector_t curr_obst_speed = state->bg.bg_3_building_vel;

  double expected_x_dist_with_jump =
      (2 * get_curr_jump_vel(state).y / get_curr_gravity(state).y) *
      curr_obst_speed.x;
  double smallest_clearing_dist = get_smallest_obst_clearing_dist(
      state, PLAYER_DIMS.y, last_obstacle_centroid.y);
  double furthest_poss_x =
      last_obst_end - smallest_clearing_dist + expected_x_dist_with_jump;
  // printf("last_obst_end=%f, smallest_clearing_dist=%f, "
  //        "expected_x_dist_with_jump=%f, furthest_poss_x=%f\n",
  //        last_obst_end, smallest_clearing_dist, expected_x_dist_with_jump,
  //        furthest_poss_x);

  // Additional random spacing between obstacles
  double running_space = rand() % ((int)(MAX.x / 5.0));
  // if running space is low, need to guarantee that we give the player enough
  // distance to jump such that they clear the height of the obstacle
  double clearing_space =
      get_smallest_obst_clearing_dist(state, PLAYER_DIMS.y, OBSTACLE_HW);
  // printf("running_space=%f,clearing_space=%f\n", running_space,
  // clearing_space);

  // It is possible that the spacing is small enough that it doesn't given
  // reasonable reaction time for a player
  return furthest_poss_x + max_d(running_space + clearing_space,
                                 MIN_REACTION_TIME_S * curr_obst_speed.x);
}



body_t *get_nth_obstacle(state_t *state, size_t n){
  size_t count = -1;
  for (size_t i = 0; i < scene_bodies(state->scene); i++){
    body_t *body = scene_get_body(state->scene, i);
    if (strcmp(body_get_info(body), OBSTACLE_INFO) == 0){
      count++;
    }
    if (count == n){
      return body;
    }
  }
  printf("Could not find obstacle %zu\n", n);
  return NULL;
}

void update_obstacles(state_t *state) {

  // Force avoid a double overflow
  if (state->n_queued_obstacles == MAX_N_QUEUED_OBST) {
    // printf("Would return here\n");
    return;
  }

  size_t width =
      (size_t)((1 + (rand() % (MAX_STACKED_OBSTACLES - 1))) * OBSTACLE_HW);
  size_t height = OBSTACLE_HW;

  // Default value is edge of the screen
  double x = MAX.x + (OBSTACLE_HW * MAX_STACKED_OBSTACLES);
  double y = (PLAYER_CENTER_POS.y - PLAYER_DIMS.y / 2) + (OBSTACLE_HW/2); // TODO

  if (state->n_queued_obstacles > 0) {
    // Use 1 + n_q - 1 for clarity: first body is player. Subtract 1 for
    // zero-indexing
    body_t *last_obstacle = get_nth_obstacle(state, state->n_queued_obstacles - 1);
    x = next_obst_x(state, last_obstacle);
  }

  vector_t new_centroid = (vector_t){.x = x, .y = y};
  body_t *new_obstacle = make_obstacle(width, height, new_centroid);

  scene_add_body(state->scene, new_obstacle);
  asset_make_image_with_body(OBSTACLE_SPRITE_PATH, new_obstacle);
  body_set_velocity(new_obstacle,
                    vec_multiply(-1, state->bg.bg_3_building_vel));

  body_t *player = scene_get_body(state->scene, 0);
  create_collision(state->scene, player, new_obstacle,
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