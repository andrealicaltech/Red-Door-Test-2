#include "assert.h"
#include "asset.h"
#include "asset_cache.h"
#include "collision.h"
#include "constants.h"
#include "forces.h"
#include "game_state.h"
#include "list.h"

const double MAGNET_ACCELERATION_S = 250.0;
const double MAGNET_RADIUS = 100.0;
const size_t MAGNET_NUM_POINTS = 25;

body_t *make_magnet(vector_t center){
  center.y += MAGNET_RADIUS;
  list_t *circle = list_init(MAGNET_NUM_POINTS, free);
  for (size_t i = 0; i < MAGNET_NUM_POINTS; i++) {
    double angle = 2 * M_PI * i / MAGNET_NUM_POINTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + MAGNET_RADIUS * cos(angle),
                    center.y + MAGNET_RADIUS * sin(angle)};
    list_add(circle, v);
  }
  body_t *magnet =
      body_init_with_info(circle, 1, MAGNET_COLOR, (void *)MAGNET_INFO, NULL);
  return magnet;
}

void magnet_body_collision_handler(body_t *body1, body_t *body2, vector_t axis, void *aux, double force_const){
    state_t *state = (state_t *) aux;
    state->is_magnet_activated = true;
    state->time_elapsed_with_magnet = MAGNET_PERIOD_S;

    bool b1_is_magnet = strcmp(body_get_info(body1), MAGNET_INFO) == 0;
    body_t *magnet = b1_is_magnet ? body1 : body2;

    // The player 'eats' the magnet
    body_remove(magnet);
}

void apply_magnet(state_t *state, double dt){
    if (state -> is_magnet_activated){
    body_t *player = scene_get_body(state->scene, 0);
    assert(strcmp(body_get_info(player), PLAYER_INFO) == 0);
    vector_t player_centroid = body_get_centroid(player);

    for (size_t i = 0; i < scene_bodies(state->scene); i++){
        body_t *body = scene_get_body(state->scene, i);
        char *info = body_get_info(body);
        if (info && strcmp(info, MAGNET_INFO) == 0){
            vector_t coin_centroid = body_get_centroid(body);
            vector_t displacement = vec_subtract(player_centroid, coin_centroid);
            double dist = vec_get_length(displacement);
            if (dist < MAGNET_RADIUS){
                vector_t translation = vec_multiply(MAGNET_ACCELERATION_S*dt/dist, displacement);
                body_set_centroid(player, translation);
            }
        }
    }
    }
}

