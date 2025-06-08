#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "asset.h"
#include "asset_cache.h"
#include "collision.h"
#include "constants.h"
#include "list.h"
#include "forces.h"
#include "game_state.h"
#include "obstacle.h"
#include "kinematics.h"

const double COIN_RAD = 10;
const size_t COIN_NUM_POINTS = 20;
const double COIN_SPACING = 12.5;
const size_t MIN_COINS_PER_PATH = 3;
const double PARABOLIC_PATH_PCT = 50.0;
const double MAGNET_TRANSLATION = 15.0;
const char *COIN_INFO = "coin";

double coin_spacing(state_t *state){
    return COIN_SPACING;
}


list_t *flat_path(state_t *state, vector_t min_start_pos, vector_t max_end_pos){
    if (min_start_pos.y != max_end_pos.y) {
        printf("Error: nonmatching y-coords for linear path\n");
        return NULL;
    }
    double delta = (max_end_pos.x - min_start_pos.x);
    if (delta < 0){
        return NULL;
    }
    double space_per_coin = (coin_spacing(state)+(COIN_RAD*2));
    size_t max_packed_coins = (size_t) ((max_end_pos.x - min_start_pos.x) / (coin_spacing(state)+(COIN_RAD*2)));
    printf("delta=%f, space_per_coin=%f, max_packed_coins=%zu\n", delta, space_per_coin, max_packed_coins);
    
    if (max_packed_coins < MIN_COINS_PER_PATH){
        // Bad UX to generate too few coins - return early
        return NULL;
    }

    // If the max packed coins is the preset minimum, just use that number. Otherwise generate a number between present minimum and calculated maximum
    // Require the ternary operator to avoid a div-by-zero 
    size_t packed_coins = max_packed_coins == MIN_COINS_PER_PATH ? max_packed_coins : (rand() % (max_packed_coins-MIN_COINS_PER_PATH)) + MIN_COINS_PER_PATH;
    double empty_space =  (max_end_pos.x - min_start_pos.x) - (
        (coin_spacing(state)+(COIN_RAD*2)) * packed_coins
    );
    double x_offset = min_start_pos.x + (rand() % (int) empty_space);
    printf("packed_coins=%zu, empty_space=%f, x_offset=%f\n", packed_coins, empty_space, x_offset);

    list_t *coin_positions = list_init(packed_coins, free);
    for (size_t i = 0; i < packed_coins; i++){
        vector_t *pos = malloc(sizeof(vector_t));
        *pos = (vector_t) {.x=(
            x_offset + (i * ((COIN_RAD*2) + coin_spacing(state)))
        ), .y=min_start_pos.y};
        list_add(coin_positions, pos);
    }

    return coin_positions;
}

body_t *make_coin(double radius, vector_t center){
  center.y += radius;
  list_t *circle = list_init(COIN_NUM_POINTS, free);
  for (size_t i = 0; i < COIN_NUM_POINTS; i++) {
    double angle = 2 * M_PI * i / COIN_NUM_POINTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + radius * cos(angle),
                    center.y + radius * sin(angle)};
    list_add(circle, v);
  }
  body_t *coin = body_init_with_info(circle, 1, COIN_COLOR, (void *) COIN_INFO, NULL);
  return coin;
}

list_t *parabolic_path(state_t *state, vector_t min_start_pos, vector_t min_end_pos){
    return NULL;
}

/*
Generates an arc between the two last queued obstacles and on top of the last queued obstacle
*/

void gen_coin_arc(state_t *state, bool should_require_powerup){
    double translation = should_require_powerup ? MAGNET_TRANSLATION : 0.0;
    uint8_t roll = rand() % 100;
    // TODO: Remove
    roll = 99;

    // The first obstacle will never have coins on top of it
    if (state->n_queued_obstacles < 2){
        return;
    }

    body_t *slast_obstacle = get_nth_obstacle(state, state->n_queued_obstacles - 2);

    vector_t slast_obst_dim = get_obstacle_dims(slast_obstacle);
    vector_t slast_obst_cent = body_get_centroid(slast_obstacle);
    printf("slast_obst_dim.x=%f, slast_obst_cent.x=%f\n", slast_obst_dim.x, slast_obst_cent.x);
    vector_t slast_obstacle_end = vec_add(body_get_centroid(slast_obstacle), (vector_t) {.x=get_obstacle_dims(slast_obstacle).x * 0.5, .y=0});
    printf("slast_obstacle_end.x=%f, slast_obstacle_end.y=%f\n", slast_obstacle_end.x, slast_obstacle_end.y);

    body_t *last_obstacle = get_nth_obstacle(state, state->n_queued_obstacles - 1);
    vector_t last_obst_dim = get_obstacle_dims(last_obstacle);
    vector_t last_obst_cent = body_get_centroid(last_obstacle);
    printf("last_obst_dim.x=%f, last_obst_cent.x=%f\n", last_obst_dim.x, last_obst_cent.x);
    vector_t last_obstacle_begin = vec_subtract(body_get_centroid(last_obstacle), (vector_t) {.x=get_obstacle_dims(last_obstacle).x * 0.5, .y=0});
    printf("last_obstacle_begin.x=%f, last_obstacle_begin.y=%f\n", last_obstacle_begin.x, last_obstacle_begin.y);

    list_t *points = NULL;
    
    if (roll < PARABOLIC_PATH_PCT){
        // TODO
        points = flat_path(state, slast_obstacle_end, last_obstacle_begin);
    } else {
        printf("Generating linear path\n");
        points = flat_path(state, slast_obstacle_end, last_obstacle_begin);
    }
    if (points){
    for (size_t i = 0; i < list_size(points); i++){
        vector_t *center = list_get(points, i);
        printf("Made coin i=%zu, center.x=%f, center.y=%f\n", i, center->x, center->y);
        body_t *coin = make_coin(COIN_RAD, *center);
        scene_add_body(state->scene, coin);
        body_set_velocity(coin, vec_multiply(-1, state->bg.bg_3_building_vel));
    }
    }
    list_free(points);
}

void clean_coins(state_t *state){
    if (scene_bodies(state->scene) == 1) {
        return;
    }
  for (size_t i = 1; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (strcmp(body_get_info(body), COIN_INFO) == 0) {
      vector_t body_pos = body_get_centroid(body);
      if (body_pos.x + (COIN_RAD * 2) < MIN.x) {
        printf("Removing coin at body_pos.x=%f\n", body_pos.x);
        body_remove(body);
      } else {
        body_set_velocity(body, vec_multiply(-1, state->bg.bg_3_building_vel));
      }
    }
  }
}