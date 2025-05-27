#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "asset.h"
#include "asset_cache.h"
#include "collision.h"
#include "forces.h"
#include "sdl_wrapper.h"

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};

const vector_t START_POS = {100, 30};
const vector_t RESET_POS = {100, 45};
const vector_t BASE_OBJ_VEL = {
    30, 0}; // starting velocity, can multiply to increase difficulty
const double EXTRA_VEL_MULT = 10;
const double VEL_MULT_PROB = 0.2;

const double OUTER_RADIUS = 15;
const double INNER_RADIUS = 15;

//obstacle = table (square 50 x 50) can spawn multiple obstacles in a row
const size_t OBSTACLE_HW = 50;
const vector_t OBS_SPACING = {100, 50, 150};

//pts of player depending on action
const size_t PLAYER_STANDING_PTS = 16;
const size_t PLAYER_RUNNING = 31;
const size_t QUESADILLA_PTS = 20;

//player movements
const double DUCK_INITIAL_VELOCITY = 0;
const double DUCK_ACCELERATION_CHANGE = 0;
const double JUMP_INITIAL_VELOCITY = 0;
const double JUMP_ACCELERATION_CHANGE = 0;

const color_t OBS_COLOR = (color_t){0.2, 0.2, 0.3}; // going to be tables
const color_t QUESADILLA_COLOR = (color_t){1, 1, 0}; // coin


const size_t BODY_ASSETS = 1;
const char *PLAYER_SPRITE_PATH = "assets/frogger.png";

struct state {
  body_t *player;
  bool ducking;
  bool jumping;
  vector_t player_velocity;
  scene_t *scene;
  int16_t points;
};


body_t *make_quesadilla(double outer_radius, double inner_radius, vector_t center) {
  center.y += inner_radius;
  list_t *c = list_init(QUESADILLA_PTS, free);
  for (size_t i = 0; i < QUESADILLA_PTS; i++) {
    double angle = 2 * M_PI * i / QUESADILLA_PTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + inner_radius * cos(angle),
                    center.y + outer_radius * sin(angle)};
    list_add(c, v);
  }
  body_t *quesadilla = body_init(c, 1, QUESADILLA_COLOR);
  return quesadilla;
}


void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
  body_t *player = scene_get_body(state->scene, 0);
  vector_t translation = (vector_t){0, 0};
  if (state->ducking){
    if (state->player_velocity.y > -DUCK_INITIAL_VELOCITY){ //TODO: Replace with if colliding with ground
      state->ducking = false;
      state->player_velocity.y = 0;
    } else{
      state->player_velocity.y += DUCK_ACCELERATION_CHANGE;
    }
  }
  else if (state->jumping){
    if (state->player_velocity.y > -JUMP_INITIAL_VELOCITY){ //TODO: Replace with if colliding with ground
      state->jumping = false;
      state->player_velocity.y = 0;
    } else{
      state->player_velocity.y += JUMP_ACCELERATION_CHANGE;
    }
  }
  else if (type == KEY_PRESSED && type != KEY_RELEASED) {
    switch (key) {
    case LEFT_ARROW:
      translation.x = -H_STEP;
      break;
    case RIGHT_ARROW:
      translation.x = H_STEP;
      break;
    case UP_ARROW:
      state->jumping = true;
      state->player_velocity.y = JUMP_INITIAL_VELOCITY
      //translation.y = V_STEP;
      break;
    case DOWN_ARROW:
      state->ducking = true;
      /*
      if (body_get_centroid(player).y > START_POS.y) {
        translation.y = -V_STEP;
      }
      */
      state->player_velocity.y = DUCK_INITIAL_VELOCITY;
      break;
    }
    vector_t new_centroid = vec_add(body_get_centroid(player), state->player_velocity);
    body_set_centroid(player, new_centroid);
  }
}




state_t *emscripten_init() {
  asset_cache_init();
  sdl_init(MIN, MAX);
  state_t *state = malloc(sizeof(state_t));
  state->points = 0;
  srand(time(NULL));
  state->scene = scene_init();
  state->ducking = false;
  state->jumping = false;
  state->player_velocity = (vector_t){.x=0,.y=0};
  body_t *player = make_frog(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
  body_set_centroid(player, RESET_POS);
  state->player = player;
  scene_add_body(state->scene, player);

  SDL_Rect *rect = malloc(sizeof(SDL_Rect));
  rect->x = MIN.x;
  rect->y = MIN.y;
  rect->w = MAX.x;
  rect->h = MAX.y;

  asset_make_image(BACKGROUND_PATH, *rect);
  asset_make_image_with_body(FROGGER_PATH, player);

  sdl_on_key((key_handler_t)on_key);
  return state;
}

bool emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  player_wrap_edges(state);
  sdl_clear();
  list_t *body_assets = asset_get_asset_list();
  for (size_t i = 0; i < list_size(body_assets); i++) {
    asset_render(list_get(body_assets, i));
  }
  sdl_show();
  scene_tick(state->scene, dt);
  return false;
}

void emscripten_free(state_t *state) {
  list_free(asset_get_asset_list());
  scene_free(state->scene);
  asset_cache_destroy();
  free(state);
}