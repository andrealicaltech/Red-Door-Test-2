#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "asset.h"
#include "asset_cache.h"
#include "collision.h"
#include "constants.h"
#include "forces.h"
#include "game_state.h"
#include "kinematics.h"
#include "math_utils.h"
#include "obstacle.h"
#include "sdl_wrapper.h"

// Background positions
const vector_t SKY_BACKGROUND = (vector_t){.x = 4000, .y = 500};
const vector_t TREE_BACKGROUND = (vector_t){.x = 4000, .y = 400};
const vector_t BUILD_BACKGROUND = (vector_t){.x = 4000, .y = 300};
const size_t PANEL_WIDTH = 4000;


void wrap_edges(body_t *body) {
  vector_t centroid = body_get_centroid(body);
  if (centroid.x > MAX.x) {
    body_set_centroid(body, (vector_t){MIN.x, centroid.y});
  } else if (centroid.x < MIN.x) {
    body_set_centroid(body, (vector_t){MAX.x, centroid.y});
  } else if (centroid.y > MAX.y) {
    body_set_centroid(body, (vector_t){centroid.x, MIN.y});
  } else if (centroid.y < MIN.y) {
    body_set_centroid(body, (vector_t){centroid.x, MAX.y});
  }
}

// TODO - Amudhan

void revert_jump(state_t *state) {
  if (state->player_velocity.y <=
      -JUMP_INITIAL_VELOCITY) { // TODO: Replace with if colliding with ground
    state->player_velocity.y = 0;
    state->player_motion = REGULAR;
  } else {
    printf("I'm here!\n");
    state->player_velocity.y += Y_GRAVITY_ACCELERATION;
  }
}

void start_game(state_t *state) {
  // TODO: Week 2 - Change state of screen to game
}

void end_game(state_t *state) {
  // TODO: Week 2 - End the game, show the score, and go back to home after
  // GAME_OVER_WAIT_TIME seconds
}

/*
MARK: Backgrounds
*/

// Andrea
void update_bg_velocity(state_t *state) {
  // TODO: Week 2 - Update velocity
  state->bg.bg_1_sky_vel = {20, 0};
  state->bg.bg_2_tree_vel = {30, 0};
  state->bg.bg_3_building_vel = {50, 0};
}

// Wrap backgrounds for scrolling effect
void wrap_backgrounds(state_t *state) {
  ssize_t limit = -(PANEL_WIDTH - MAX.x);
  if (state->bg.sky_pos.x <= limit) {
    state->bg.sky_pos.x = 0.0f;
  }
  if (state->bg.tree_pos.x <= limit) {
    state->bg.tree_pos.x = 0.0f;
  }
  if (state->bg.build_pos.x <= limit) {
    state->bg.build_pos.x = 0.0f;
  }
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
  state->bg =
      (background_t){.bg_1_sky_vel = INIT_BACKGROUND_1_SKY_VELOCITY,
                     .bg_2_tree_vel = INIT_BACKGROUND_2_TREE_VELOCITY,
                     .bg_3_building_vel = INIT_BACKGROUND_3_BUILDINGS_VELOCITY,
                     .sky_pos = SKY_BACKGROUND,
                     .tree_pos = TREE_BACKGROUND,
                     .build_pos = BUILD_BACKGROUND};
  SDL_Rect rect = (SDL_Rect){.x = 0, .y = 0, .w = MAX.x, .h = MAX.y};
  asset_make_image(BACKGROUND_PATH, rect);
  asset_make_image_with_body(PLAYER_SPRITE_PATH, player);
  sdl_on_key((key_handler_t)on_key);

  state->jump_start_y = 0.0;

  // Obstacles
  state->time_till_next_obstacle = FIRST_OBSTACLE_WAIT_TIME;
  state->n_queued_obstacles = 0;
  state->player_running_on_obst = false;

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

  state->time_till_next_obstacle -= dt;
  update_obstacles(state);
  clean_obstacles(state);

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