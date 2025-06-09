#include "assert.h"
#include "asset.h"
#include "asset_cache.h"
#include "collision.h"
#include "constants.h"
#include "forces.h"
#include "game_state.h"
#include "list.h"

const double MAGNET_ACCELERATION_S = 1250.0;
const size_t MAGNET_NUM_POINTS = 25;
const double MAGNET_IMPACT_RAD = 250.0;

body_t *make_magnet(double radius, vector_t center) {
  center.y += radius;
  list_t *circle = list_init(MAGNET_NUM_POINTS, free);
  for (size_t i = 0; i < MAGNET_NUM_POINTS; i++) {
    double angle = 2 * M_PI * i / MAGNET_NUM_POINTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + radius * cos(angle),
                    center.y + radius * sin(angle)};
    list_add(circle, v);
  }
  body_t *magnet =
      body_init_with_info(circle, 1, MAGNET_COLOR, (void *)MAGNET_INFO, NULL);
  return magnet;
}

void magnet_body_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                                   void *aux, double force_const) {
  printf("Collided with magnet!\n");

  state_t *state = (state_t *)aux;
  state->is_magnet_activated = true;
  state->time_elapsed_with_magnet = MAGNET_PERIOD_S;

  bool b1_is_magnet = strcmp(body_get_info(body1), MAGNET_INFO) == 0;
  body_t *magnet = b1_is_magnet ? body1 : body2;
  // The player 'eats' the magnet
  body_remove(magnet);
}

void apply_magnet(state_t *state, double dt) {
  if (state->is_magnet_activated) {
    body_t *player = scene_get_body(state->scene, 0);
    assert(strcmp(body_get_info(player), PLAYER_INFO) == 0);
    vector_t player_centroid = body_get_centroid(player);

    for (size_t i = 0; i < scene_bodies(state->scene); i++) {
      body_t *body = scene_get_body(state->scene, i);
      char *info = body_get_info(body);
      if (info && strcmp(info, COIN_INFO) == 0) {
        vector_t coin_centroid = body_get_centroid(body);
        if (coin_centroid.x < MAX.x) {
          vector_t displacement = vec_subtract(player_centroid, coin_centroid);
          double dist = vec_get_length(displacement);
          printf("Coin dist=%f\n", dist);
          if (dist < MAGNET_IMPACT_RAD) {
            vector_t acceleration =
                vec_multiply(MAGNET_ACCELERATION_S * dt / dist, displacement);
            printf("acceleration.x=%f, acceleration.y=%f\n", acceleration.x,
                   acceleration.y);
            body_set_velocity(body,
                              vec_add(body_get_velocity(body), acceleration));
          }
        }
      }
    }
  }
}

void clean_magnet(state_t *state) {
  if (scene_bodies(state->scene) == 1) {
    return;
  }
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (strcmp(body_get_info(body), MAGNET_INFO) == 0) {
      vector_t magnet_pos = body_get_centroid(body);
      if (magnet_pos.x + MAGNET_RAD < MIN.x) {
        body_remove(body);
      } else {
        body_set_velocity(body, vec_multiply(-1, state->bg.bg_3_building_vel));
      }
    }
  }
}
