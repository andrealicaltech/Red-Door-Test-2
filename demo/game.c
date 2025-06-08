#include <SDL2/SDL.h>
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
#include "music.h"
#include "obstacle.h"
#include "quesedilla.h"
#include "sdl_wrapper.h"
// moved background positions to background.c

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
  TTF_Init();

  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();

  state->bg.building_pos.x = 0;
  state->bg.tree_pos.x = 0;
  state->bg.sky_pos.x = 0;

  state->started = false;
  state->is_game_over = false;
  state->font = TTF_OpenFont("assets/Roboto-Regular.ttf", 24);
  state->current_game_screen = HOME;

  srand(time(NULL));
  state->player_motion = REGULAR;

  SDL_Texture *start =
      asset_cache_obj_get_or_create(ASSET_IMAGE, START_SCREEN_PATH);
  asset_cache_store_temp("start", start);

  SDL_Texture *game_over =
      asset_cache_obj_get_or_create(ASSET_IMAGE, GAME_OVER_PATH);
  asset_cache_store_temp("game over", game_over);

  SDL_Texture *shop = asset_cache_obj_get_or_create(ASSET_IMAGE, SHOP_PATH);
  asset_cache_store_temp("shop", shop);
  state->show_shop = false;

  // state->andrea_rect = (SDL_Rect) {.x = 0, .y = 0, .w = 332, .h = 499};
  // state->arjun_rect = (SDL_Rect) {.x = 333, .y = 0, .w = 332, .h = 499};
  // state->amudhan_rect = (SDL_Rect) {.x = 666, .y = 0, .w = 332, .h = 499};

  // SDL_Renderer *rend = sdl_get_renderer();
  // state->rend = rend;
  // SDL_RenderCopy(state->rend,
  // sdl_get_image_texture(PLAYER_SPRITE_ANDREA_PATH), NULL,
  // &state->andrea_rect); SDL_RenderCopy(state->rend,
  // sdl_get_image_texture(PLAYER_SPRITE_ARJUN_PATH), NULL, &state->arjun_rect);
  // SDL_RenderCopy(state->rend,
  // sdl_get_image_texture(PLAYER_SPRITE_AMUDHAN_PATH), NULL,
  // &state->amudhan_rect);

  // Needs to be the first one
  body_t *player = make_player(PLAYER_DIMS.x, PLAYER_DIMS.y, PLAYER_CENTER_POS);
  body_set_centroid(player, PLAYER_CENTER_POS);
  state->player = player;
  scene_add_body(state->scene, player);

  SDL_Rect *rect = malloc(sizeof(SDL_Rect));
  rect->x = MIN.x;
  rect->y = MIN.y;
  rect->w = MAX.x;
  rect->h = MAX.y;

  SDL_Texture *sky = asset_cache_obj_get_or_create(ASSET_IMAGE, SKY_PATH);
  SDL_Texture *tree = asset_cache_obj_get_or_create(ASSET_IMAGE, TREE_PATH);
  SDL_Texture *building =
      asset_cache_obj_get_or_create(ASSET_IMAGE, BUILDING_PATH);

  background_init(state);

  asset_cache_store_temp("sky", sky);
  asset_cache_store_temp("tree", tree);
  asset_cache_store_temp("building", building);

  asset_make_image_with_body(PLAYER_SPRITE_AMUDHAN_PATH, player);
  sdl_on_key((key_handler_t)on_key);

  state->jump_start_y = PLAYER_CENTER_POS.y;

  // Obstacles
  state->time_till_next_update = FIRST_OBSTACLE_WAIT_TIME;
  state->n_queued_obstacles = 0;
  state->curr_player_obstacle = NULL;

  state->is_magnet_activated = false;

  state->points = 0;
  state->all_points = list_init(MAX_GAMES, NULL);
  return state;
}

bool emscripten_main(state_t *state) {
  sdl_clear();
  SDL_Rect viewport = {.x = 0, .y = 0, .w = MAX.x, .h = MAX.y};

  if (!state->started && !state->is_game_over) {
    render_screen("start", &viewport);
    play_music(MENU_MUSIC_PATH);
  } // start screen

  if (state->started) {
    double dt = time_since_last_tick();
    play_music(GAME_MUSIC_PATH);
    update_bg_velocity(state, dt);
    update_bg_pos(state, dt);
    render_layers(asset_cache_lookup("sky"), &state->bg.sky_pos.x, &viewport);
    render_layers(asset_cache_lookup("tree"), &state->bg.tree_pos.x, &viewport);
    render_layers(asset_cache_lookup("building"), &state->bg.building_pos.x,
                  &viewport);
    list_t *body_assets = asset_get_asset_list();

    for (size_t i = 0; i < list_size(body_assets); i++) {
      asset_render(list_get(body_assets, i));
    }

    sdl_render_scene(state->scene);
    state->time_till_next_update -= dt;
    if (state->time_till_next_update <= 0.0) {
      update_obstacles(state);
      state->time_till_next_update = mod_d((double)rand(), MAX_TIME_UPDATE);
      gen_coin_arc(state, false);
    }

    clean_obstacles(state);
    clean_coins(state);
    check_player_falling_off_edge(state);

    manipulate_player(state, dt);
    scene_tick(state->scene, dt);

    if (state->is_game_over) {
      state->started = false;
    }
  } // game screen

  // if (!state->started && state->is_game_over) {
  //   sdl_clear();
  //   render_screen("game over", &viewport);
  //   sdl_show();

  //   double time = time_since_last_tick();
  //   state->delay_time += time;

  //   if (state->delay_time >= 1.0) {
  //     state->show_shop = true;
  //     state->delay_time = 0;
  //   }
  // } // game over screen

  // if (state->show_shop) {
  //   render_screen("shop", &viewport);
  // } // shop screen

  sdl_show();
  return false;
}

void emscripten_free(state_t *state) {
  list_free(asset_get_asset_list());
  list_free(state->all_points);
  scene_free(state->scene);
  asset_cache_destroy();
  free(state);
}