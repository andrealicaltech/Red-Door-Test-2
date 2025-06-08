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
#include "leaderboard.h"
#include "magnet.h"
#include "math_utils.h"
#include "music.h"
#include "obstacle.h"
#include "quesedilla.h"
#include "sdl_wrapper.h"
// moved background positions to background.c

/*
MARK: Emscripten
*/

double delay_time = 0.0;

state_t *emscripten_init() {

  asset_cache_init();
  sdl_init(MIN, MAX);
  TTF_Init();

  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();

  state->started = false;
  state->is_game_over = false;
  state->font = TTF_OpenFont("assets/Roboto-Regular.ttf", 24);
  state->current_game_screen = HOME;

  srand(time(NULL));

  init_screens(state);

  // Needs to be the first one
  body_t *player = init_player(state);

  make_layers(state);

  asset_make_image_with_body(PLAYER_SPRITE_AMUDHAN_PATH, player);
  sdl_on_key((key_handler_t)on_key);
  create_scoreboard(state);

  init_parameters(state);
  state->all_points = list_init(MAX_GAMES, NULL);

  return state;
}

bool emscripten_main(state_t *state) {
  // sdl_clear();
  SDL_Rect viewport = {.x = 0, .y = 0, .w = MAX.x, .h = MAX.y};

  if (!state->started && !state->is_game_over) {
    render_screen("start", &viewport);
    play_music(MENU_MUSIC_PATH, false);
  } // start screen

  if (state->started) {
    double dt = time_since_last_tick();
    play_music(GAME_MUSIC_PATH, false);
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

    // sdl_render_scene(state->scene);
    render_text(state);
    sdl_show();

    state->time_till_next_update -= dt;

    if (state->time_till_next_update <= 0.0) {
      if (state->n_queued_obstacles < MAX_N_QUEUED_OBST) {
        update_obstacles(state);
        bool should_generate_magnet = (rand() % 100) > (MAGNET_FREQUENCY * 100);
        gen_coin_arc(state, should_generate_magnet);
      }
      state->time_till_next_update = mod_d((double)rand(), MAX_TIME_UPDATE);
    }
    if (state->is_magnet_activated) {
      state->time_elapsed_with_magnet -= dt;
    }

    clean_obstacles(state);
    clean_coins(state);
    check_player_falling_off_edge(state);
    apply_magnet(state, dt);

    manipulate_player(state, dt);
    scene_tick(state->scene, dt);

    if (state->is_game_over) {
      state->started = false;
    }
  } // game screen

  if (!state->started && state->is_game_over) {
    sdl_clear();
    render_screen("game over", &viewport);
    sdl_show();

    double time = time_since_last_tick();
    delay_time += time;

    if (delay_time >= 2.0) {
      state->show_shop = true;
      delay_time = 0;
    }
  } // game over screen

  if (state->show_shop) {
    render_screen("shop", &viewport);
  } // shop screen

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