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
body_t *make_rectangle_body(double width, double height, vector_t center, color_t color) {
  list_t *rect = list_init(4, free);

  vector_t *vec_1 = malloc(sizeof(vector_t));
  *vec_1 = (vector_t){center.x - (width / 2), center.y - (height / 2)};
  list_add(rect, vec_1);

  vector_t *vec_2 = malloc(sizeof(vector_t));
  *vec_2 = (vector_t){center.x + (width / 2), center.y - (height / 2)};
  list_add(rect, vec_2);

  vector_t *vec_3 = malloc(sizeof(vector_t));
  *vec_3 = (vector_t){center.x + (width / 2), center.y + (height / 2)};
  list_add(rect, vec_3);

  vector_t *vec_4 = malloc(sizeof(vector_t));
  *vec_4 = (vector_t){center.x - (width / 2), center.y + (height / 2)};
  list_add(rect, vec_4);

  body_t *rectangle =
      body_init_with_info(rect, 1, color, (void *)PLAYER_INFO, NULL);

  return rectangle;
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
  body_t *player = make_rectangle_body(PLAYER_DIMS.x, PLAYER_DIMS.y, VEC_ZERO, SPRITE_COLOR);
  body_set_centroid(player, PLAYER_CENTER_POS);
  state->player = player;
  scene_add_body(state->scene, player);

  // TODO: Initialize all 3 backgrounds
  background_init(state);
  body_t *sky = make_rectangle_body(SKY_BACKGROUND.x, SKY_BACKGROUND.y, MIN, SPRITE_COLOR);
  body_t *tree = make_rectangle_body(TREE_BACKGROUND.x, TREE_BACKGROUND.y, MIN), SPRITE_COLOR;
  body_t *building =
      make_rectangle_body(BUILD_BACKGROUND.x, BUILD_BACKGROUND.y, MIN, SPRITE_COLOR);
  asset_make_image_with_body(SKY_PATH, sky);
  asset_make_image_with_body(TREE_PATH, tree);
  asset_make_image_with_body(BUILDING_PATH, building);

  state->bg.sky_body = sky;
  state->bg.tree_body = tree;
  state->bg.building_body = building;

  body_set_velocity(state->bg.sky_body, state->bg.bg_1_sky_vel);
  body_set_velocity(state->bg.tree_body, state->bg.bg_2_tree_vel);
  body_set_velocity(state->bg.building_body, state->bg.bg_3_building_vel);

  scene_add_body(state->scene, state->bg.sky_body);
  scene_add_body(state->scene, state->bg.tree_body);
  scene_add_body(state->scene, state->bg.building_body);

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

  update_bg_pos(state, dt);

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