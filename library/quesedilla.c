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
#include "list.h"
#include "magnet.h"
#include "obstacle.h"

const size_t COIN_NUM_POINTS = 20;
const double COIN_SPACING = 12.5;
const double PARABOLIC_COIN_SPACING = 50;
const size_t MIN_COINS_PER_PATH = 3;
const size_t V_LARG_NUM_COINS = 100;
const double Y_TOLERANCE = 10.0;
const double PARABOLIC_PATH_PCT = 50.0;
const double MAGNET_TRANSLATION = 15.0;

double coin_spacing(state_t *state) { return COIN_SPACING; }

list_t *flat_path(state_t *state, vector_t min_start_pos,
                  vector_t max_end_pos) {
  if (min_start_pos.y != max_end_pos.y) {
    return NULL;
  }
  double delta = (max_end_pos.x - min_start_pos.x);
  if (delta < 0) {
    return NULL;
  }
  size_t max_packed_coins = (size_t)((max_end_pos.x - min_start_pos.x) /
                                     (coin_spacing(state) + (COIN_RAD * 2)));

  if (max_packed_coins < MIN_COINS_PER_PATH) {
    // Bad UX to generate too few coins - return early
    return NULL;
  }

  // If the max packed coins is the preset minimum, just use that number.
  // Otherwise generate a number between present minimum and calculated maximum
  // Require the ternary operator to avoid a div-by-zero
  size_t packed_coins =
      max_packed_coins == MIN_COINS_PER_PATH
          ? max_packed_coins
          : (rand() % (max_packed_coins - MIN_COINS_PER_PATH)) +
                MIN_COINS_PER_PATH;
  double empty_space = (max_end_pos.x - min_start_pos.x) -
                       ((coin_spacing(state) + (COIN_RAD * 2)) * packed_coins);
  double x_offset = min_start_pos.x + (rand() % (int)empty_space);

  list_t *coin_positions = list_init(packed_coins, free);
  for (size_t i = 0; i < packed_coins; i++) {
    vector_t *pos = malloc(sizeof(vector_t));
    *pos = (vector_t){
        .x = (x_offset + (i * ((COIN_RAD * 2) + coin_spacing(state)))),
        .y = min_start_pos.y};
    list_add(coin_positions, pos);
  }

  return coin_positions;
}

list_t *parabolic_path(state_t *state, vector_t min_start_pos,
                       vector_t max_end_pos) {
  double expected_x_dist_with_jump =
      (2 * get_curr_jump_vel(state).y / get_curr_gravity(state).y) *
      state->bg.bg_3_building_vel.x;

  double delta = max_end_pos.x - min_start_pos.x;
  // Need tolerance between expected x_dist and delta
  if (delta < 0 || expected_x_dist_with_jump - delta < 1.0) {
    return NULL;
  }

  double x_offset =
      min_start_pos.x + (rand() % ((int)(delta - expected_x_dist_with_jump)));

  list_t *coin_positions = list_init(V_LARG_NUM_COINS, free);

  // Constants to use in calculation of points along arc
  double u_y = get_curr_jump_vel(state).y;
  double v_x = state->bg.bg_3_building_vel.x;
  double g = get_curr_gravity(state).y;

  double DS = PARABOLIC_COIN_SPACING;
  double x = 0;
  double y = 0;

  // These are values required for our approximation of dx which we reuse
  double t1 = pow(u_y / v_x, 2);
  double t2 = 2 * u_y * g / pow(v_x, 3);
  double t3 = pow(g / (v_x * v_x), 2);

  /*
  Local approximation of ds approxeq s, dx approxeq x to give constant arc
  length
  */
  do {
    vector_t *pos = malloc(sizeof(vector_t));
    // term should be always positive
    *pos = (vector_t){.x = x_offset + x, .y = min_start_pos.y + y};
    list_add(coin_positions, pos);
    double squared_deriv = t1 - (t2 * x) + (t3 * pow(x, 2));
    x += DS / sqrt(1 + squared_deriv);
    y = u_y * (x / v_x) - (0.5 * g) * pow((x / v_x), 2);
  } while (x_offset + x < max_end_pos.x && y >= 0);

  return coin_positions;
}

body_t *make_coin(double radius, vector_t center) {
  center.y += radius;
  list_t *circle = list_init(COIN_NUM_POINTS, free);
  for (size_t i = 0; i < COIN_NUM_POINTS; i++) {
    double angle = 2 * M_PI * i / COIN_NUM_POINTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + radius * cos(angle),
                    center.y + radius * sin(angle)};
    list_add(circle, v);
  }
  body_t *coin =
      body_init_with_info(circle, 1, COIN_COLOR, (void *)COIN_INFO, NULL);
  return coin;
}

/*
Calculate the x-gap to leave after an obstacle before generating coins

Assume the player falls off the edge of an obstacle. Find the distance they will
cover while falling Then leave enough time for reaction
*/
double min_gap_after_obstacle(state_t *state, double obstacle_height) {
  double time = sqrt(2 * obstacle_height / get_curr_gravity(state).y);
  return (time * state->bg.bg_3_building_vel.x) +
         (MIN_REACTION_TIME_S * state->bg.bg_3_building_vel.x);
}

void quesedilla_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                                  void *aux, double force_const) {
  body_t *coin = strcmp(body_get_info(body2), COIN_INFO) == 0 ? body2 : body1;
  state_t *state = (state_t *)aux;
  state->n_coins_collected += 1;
  body_remove(coin);
}

void gen_coin_arc(state_t *state, bool should_include_powerup) {

  // The first obstacle will never have coins on top of it
  if (state->n_queued_obstacles < 2) {
    return;
  }

  // TODO: CQ - Clean this mess up

  body_t *slast_obstacle =
      get_nth_obstacle(state, state->n_queued_obstacles - 2);
  vector_t slast_obst_dim = get_obstacle_dims(slast_obstacle);

  vector_t slast_obstacle_end =
      vec_add(body_get_centroid(slast_obstacle),
              (vector_t){.x = get_obstacle_dims(slast_obstacle).x * 0.5 +
                              min_gap_after_obstacle(state, slast_obst_dim.y),
                         .y = 0});

  body_t *last_obstacle =
      get_nth_obstacle(state, state->n_queued_obstacles - 1);
  vector_t last_obst_dim = get_obstacle_dims(last_obstacle);
  vector_t last_obstacle_begin =
      vec_subtract(body_get_centroid(last_obstacle),
                   (vector_t){.x = last_obst_dim.x * 0.5 +
                                   get_smallest_obst_clearing_dist(
                                       state, PLAYER_DIMS.y, last_obst_dim.y),
                              .y = 0});

  list_t *points = NULL;

  uint8_t roll = rand() % 100;
  if (roll < PARABOLIC_PATH_PCT) {
    points = parabolic_path(state, slast_obstacle_end, last_obstacle_begin);
  } else {
    points = flat_path(state, slast_obstacle_end, last_obstacle_begin);
  }
  if (points) {
    body_t *player = scene_get_body(state->scene, 0);
    size_t powerup_idx = -1;
    if (should_include_powerup) {
      powerup_idx = rand() % list_size(points);
    }
    for (size_t i = 0; i < list_size(points); i++) {
      vector_t *center = list_get(points, i);
      body_t *new_body = (i == powerup_idx) ? make_magnet(MAGNET_RAD, *center)
                                            : make_coin(COIN_RAD, *center);
      scene_add_body(state->scene, new_body);
      char *path = (i == powerup_idx) ? CARD_PATH : QUESADILLA_PATH;
       collision_handler_t handler = (i == powerup_idx)
                                        ? magnet_body_collision_handler
                                        : quesedilla_collision_handler;
      asset_make_image_with_body(path, new_body);
     
      create_collision(state->scene, player, new_body, handler, state, 0, NULL);
      body_set_velocity(new_body,
                        vec_multiply(-1, state->bg.bg_3_building_vel));
    }
    list_free(points);
  }
}

void clean_coins(state_t *state) {
  if (scene_bodies(state->scene) == 1) {
    return;
  }
  for (size_t i = 1; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (strcmp(body_get_info(body), COIN_INFO) == 0) {
      vector_t body_pos = body_get_centroid(body);
      if (body_pos.x + (COIN_RAD * 2) < MIN.x) {
        body_remove(body);
      } else {
        body_set_velocity(body, vec_multiply(-1, state->bg.bg_3_building_vel));
      }
    }
  }
}