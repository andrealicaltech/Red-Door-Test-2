#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "asset.h"
#include "asset_cache.h"
#include "background.h"
#include "collision.h"
#include "constants.h"
#include "forces.h"
#include "game_state.h"
#include "kinematics.h"
#include "math_utils.h"
#include "obstacle.h"
#include "sdl_wrapper.h"

// moved background positions to background.c

/*
MARK: Player control and kinematics
*/
body_t *make_player_sprite(double outer_radius, double inner_radius,
                           vector_t center) {
  // TODO: Replace with player sprite asset
  center.y += inner_radius;
  list_t *c = list_init(20, free);
  for (size_t i = 0; i < 20; i++) {
    double angle = 2 * M_PI * i / 20;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + inner_radius * cos(angle),
                    center.y + outer_radius * sin(angle)};
    list_add(c, v);
  }
  body_t *player =
      body_init_with_info(c, 1, SPRITE_COLOR, (void *)PLAYER_INFO, NULL);
  return player;
}

void start_game(state_t *state) {
  if (state->is_game_over) {
    state->current_game_screen = HOME;
  } else {
    state->current_game_screen = GAME;
  }

  // TODO: Week 2 - Change state of screen to game
}

void end_game(state_t *state) {
  // TODO: Week 2 - End the game, show the score, and go back to home after
  // GAME_OVER_WAIT_TIME seconds
}

/*
MARK: Coins
*/

void spawn_coins(state_t *state) {
  // TODO: Week 2 - spawn coins at random intervals
}

void clean_elapsed_coins(state_t *state) {
  // TODO: Week 2  - Remove coins after they hit the end of the screen
}

/*
MARK: Emscripten
*/
state_t *emscripten_init() {

  asset_cache_init();
  sdl_init(MIN, MAX);

  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();

  state->current_game_screen = HOME;

  srand(time(NULL));
  state->scene = scene_init();
  state->player_motion = REGULAR;

  // Needs to be the first one
  body_t *player = make_player_sprite(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
  body_set_centroid(player, PLAYER_CENTER_POS);
  state->player = player;
  scene_add_body(state->scene, player);

  // TODO: Initialize all 3 backgrounds
  background_init(state);
  SDL_Rect rect = (SDL_Rect){.x = 0, .y = 0, .w = MAX.x, .h = MAX.y};
  asset_make_image(SKY_PATH, rect);
  asset_make_image(TREE_PATH, rect);
  asset_make_image(BUILDING_PATH, rect);
  asset_make_image_with_body(PLAYER_SPRITE_PATH, player);
  sdl_on_key((key_handler_t)on_key);

  state->jump_start_y = 0.0;

  // Obstacles
  state->time_till_next_obstacle = FIRST_OBSTACLE_WAIT_TIME;
  state->n_queued_obstacles = 0;
  state->curr_player_obstacle = NULL;

  // TODO: Activate
  state->is_revival_activated = false;
  state->is_magnet_activated = false;

  state->points = 0;
  state->all_points = list_init(MAX_GAMES, NULL);
  return state;
}

bool emscripten_main(state_t *state) {
  double dt = time_since_last_tick();

  sdl_clear();
  wrap_backgrounds(state);
  update_bg_velocity(state);

  list_t *body_assets = asset_get_asset_list();
  for (size_t i = 0; i < list_size(body_assets); i++) {
    asset_render(list_get(body_assets, i));
  }
  sdl_render_scene(state->scene);

  state->time_till_next_obstacle -= dt;
  update_obstacles(state);

  clean_obstacles(state);
  check_player_falling_off_edge(state);

  sdl_show();
  scene_tick(state->scene, dt);
  manipulate_player(state, dt);
  return false;
}

void emscripten_free(state_t *state) {
  list_free(asset_get_asset_list());
  list_free(state->all_points);
  scene_free(state->scene);
  asset_cache_destroy();
  free(state);
}