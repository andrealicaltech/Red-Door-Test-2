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
#include "game_state.h"
#include "kinematics.h"
#include "music.h"
#include "obstacle.h"
#include "sdl_wrapper.h"
#include "utils.h"

const vector_t SKY_BACKGROUND = (vector_t){.x = 1000, .y = 500};
const vector_t SKY_CENTER = (vector_t){.x = 500, .y = 250};
const vector_t TREE_BACKGROUND = (vector_t){.x = 1000, .y = 400};
const vector_t TREE_CENTER = (vector_t){.x = 500, .y = 200};
const vector_t BUILD_BACKGROUND = (vector_t){.x = 1000, .y = 300};
const vector_t BUILD_CENTER = (vector_t){.x = 500, .y = 150};
const size_t PANEL_WIDTH = 4000;

// initialize background
void background_init(state_t *state) {
  state->bg = (background_t){
      .bg_1_sky_vel = INIT_BACKGROUND_1_SKY_VELOCITY,
      .bg_2_tree_vel = INIT_BACKGROUND_2_TREE_VELOCITY,
      .bg_3_building_vel = INIT_BACKGROUND_3_BUILDINGS_VELOCITY,
      .sky_pos = (vector_t){.x = 0, .y = SKY_BACKGROUND.y},
      .tree_pos = (vector_t){.x = 0, .y = TREE_BACKGROUND.y},
      .building_pos = (vector_t){.x = 0, .y = BUILD_BACKGROUND.y}};

  state->bg.building_pos.x = 0;
  state->bg.tree_pos.x = 0;
  state->bg.sky_pos.x = 0;
}

// increase speed of player + backgrounds
void update_bg_velocity(state_t *state, double dt) {
  vector_t add = (vector_t){.x = 2.0 * dt, .y = 0};
  state->bg.bg_1_sky_vel = vec_add(add, state->bg.bg_1_sky_vel);
  state->bg.bg_2_tree_vel = vec_add(add, state->bg.bg_2_tree_vel);
  state->bg.bg_3_building_vel = vec_add(add, state->bg.bg_3_building_vel);
}

// update the position of background for wrapping
void update_bg_pos(state_t *state, double dt) {
  state->bg.sky_pos.x -= state->bg.bg_1_sky_vel.x * dt;
  state->bg.tree_pos.x -= state->bg.bg_2_tree_vel.x * dt;
  state->bg.building_pos.x -= state->bg.bg_3_building_vel.x * dt;
}

// render screens
void render_screen(const char *screen_key, SDL_Rect *viewport) {
  SDL_Texture *screen = asset_cache_lookup(screen_key);
  sdl_render_image(screen, viewport);
}

// render all background layers
void render_layers(SDL_Texture *texture, double *x, SDL_Rect *viewport) {
  *x = fmod(*x, viewport->w);
  if (*x > 0) {
    *x -= viewport->w;
  }

  SDL_Rect dest1 = *viewport;
  dest1.x = (int)(*x);

  SDL_Rect dest2 = *viewport;
  dest2.x = (int)(*x) + viewport->w;

  sdl_render_image(texture, &dest1);
  sdl_render_image(texture, &dest2);
}

// initialize box
body_t *make_object(double w, double h, vector_t center, void *info) {
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
  body_t *object = body_init_with_info(c, 1, SPRITE_COLOR, info, NULL);
  body_set_centroid(object, center);
  return object;
}

body_t *init_player(state_t *state) {
  body_t *player = make_object(PLAYER_DIMS.x, PLAYER_DIMS.y, PLAYER_CENTER_POS,
                               (void *)PLAYER_INFO);
  body_set_centroid(player, PLAYER_CENTER_POS);
  state->player = player;
  scene_add_body(state->scene, player);
  state->player_motion = REGULAR;
  return player;
}

void init_parameters(state_t *state) {
  state->jump_start_y = PLAYER_CENTER_POS.y;
  state->player_motion = REGULAR;
  state->time_till_next_update = FIRST_OBSTACLE_WAIT_TIME;
  state->n_queued_obstacles = 0;
  state->curr_player_obstacle = NULL;
  state->is_magnet_activated = false;
  state->time_elapsed_with_magnet = 0;
  state->n_coins_collected = 0;
  state->points = 0;
  state->current_player_idx = 0;
  state->delay_time_remaining = MAX_DELAY_TIME;
}

void init_screens(state_t *state) {
  SDL_Texture *start =
      asset_cache_obj_get_or_create(ASSET_IMAGE, (char *)START_SCREEN_PATH);
  asset_cache_store_temp("start", start);

  SDL_Texture *game_over =
      asset_cache_obj_get_or_create(ASSET_IMAGE, (char *)GAME_OVER_PATH);
  asset_cache_store_temp("game over", game_over);

  SDL_Texture *shop =
      asset_cache_obj_get_or_create(ASSET_IMAGE, (char *)SHOP_PATH);
  asset_cache_store_temp("shop", shop);
  state->show_shop = false;
}

void make_layers(state_t *state) {
  SDL_Texture *sky =
      asset_cache_obj_get_or_create(ASSET_IMAGE, (char *)SKY_PATH);
  SDL_Texture *tree =
      asset_cache_obj_get_or_create(ASSET_IMAGE, (char *)TREE_PATH);
  SDL_Texture *building =
      asset_cache_obj_get_or_create(ASSET_IMAGE, (char *)BUILDING_PATH);

  background_init(state);

  asset_cache_store_temp("sky", sky);
  asset_cache_store_temp("tree", tree);
  asset_cache_store_temp("building", building);
}

void reset_game(state_t *state) {
  state->started = true;
  state->show_shop = false;
  state->is_game_over = false;
  state->current_game_screen = GAME;
  state->play_music = true;
  halt_music();

  background_init(state);

  asset_remove_body(state->player);

  asset_make_image_with_body(state->sprite_path, state->player);
}