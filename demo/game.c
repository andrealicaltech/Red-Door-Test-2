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
#include "sdl_wrapper.h"
// moved background positions to background.c

/*
MARK: Player control and kinematics
*/
void render_layers(SDL_Texture *texture, double *x, SDL_Rect *viewport) {
  *x = fmod(*x, viewport->w);
  if (*x > 0) {
    *x -= viewport->w;
  }

  printf("Rendering at x = %.2f\n", *x);

  SDL_Rect dest1 = *viewport;
  dest1.x = (int)(*x);

  SDL_Rect dest2 = *viewport;
  dest2.x = (int)(*x) + viewport->w;

  sdl_render_image(texture, &dest1);
  sdl_render_image(texture, &dest2);
}

body_t *make_background(double w, double h, vector_t center) {
  list_t *c = list_init(4, free);
  vector_t *v1 = malloc(sizeof(vector_t));
  *v1 = (vector_t){-w / 2, -h / 2};
  list_add(c, v1);

  vector_t *v2 = malloc(sizeof(vector_t));
  *v2 = (vector_t){w / 2, -h / 2};
  list_add(c, v2);

  vector_t *v3 = malloc(sizeof(vector_t));
  *v3 = (vector_t){w / 2, h / 2};
  list_add(c, v3);

  vector_t *v4 = malloc(sizeof(vector_t));
  *v4 = (vector_t){-w / 2, h / 2};
  list_add(c, v4);
  body_t *obstacle = body_init(c, 1, OBS_COLOR);
  body_set_centroid(obstacle, center);
  return obstacle;
}

body_t *make_frog(double outer_radius, double inner_radius, vector_t center) {
  center.y += inner_radius;
  list_t *c = list_init(4, free);
  for (size_t i = 0; i < 4; i++) {
    double angle = 2 * M_PI * i / 4;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + inner_radius * cos(angle),
                    center.y + outer_radius * sin(angle)};
    list_add(c, v);
  }
  body_t *froggy = body_init(c, 1, SPRITE_COLOR);
  return froggy;
}

body_t *make_player(double w, double h, vector_t center) {
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
  body_t *player = body_init(c, 1, SPRITE_COLOR);
  body_set_centroid(player, center);
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
  printf("Here!\n");
  play_audio();
  printf("Here 2!\n");

  asset_cache_init();
  sdl_init(MIN, MAX);

  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();

  state->bg.building_pos.x = 0;
  state->bg.tree_pos.x = 0;
  state->bg.sky_pos.x = 0;

  state->current_game_screen = HOME;

  srand(time(NULL));
  // state->scene = scene_init();
  state->player_motion = REGULAR;

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
  if (dt < 0.0001) {
    dt = 0.001;
  }
  update_bg_velocity(state, dt);

  update_bg_pos(state, dt);

  sdl_clear();
  SDL_Rect viewport = {.x = 0, .y = 0, .w = MAX.x, .h = MAX.y};
  render_layers(asset_cache_lookup("sky"), &state->bg.sky_pos.x, &viewport);
  render_layers(asset_cache_lookup("tree"), &state->bg.tree_pos.x, &viewport);
  render_layers(asset_cache_lookup("building"), &state->bg.building_pos.x,
                &viewport);

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