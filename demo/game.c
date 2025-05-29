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

// obstacle = table (square 50 x 50) can spawn multiple obstacles in a row
const size_t OBSTACLE_HW = 50;

// TODO: Make sure x-distance covered by jump is less than 50
const size_t OBS_SPACING[4] = {0, 50, 100, 150};
const size_t MAX_CONSEC_OBSTACLES = 5;

// pts of player depending on action
const size_t PLAYER_STANDING_PTS = 16;
const size_t PLAYER_RUNNING = 31;
const size_t QUESADILLA_PTS = 20;

// player movements
const double DUCK_INITIAL_VELOCITY = 0;
const double DUCK_ACCELERATION_CHANGE = 0;
const double JUMP_INITIAL_VELOCITY = 10;
const double Y_GRAVITY_ACCELERATION = -0.5;

// Background and obstacle velocity
const double INIT_BACKGROUND_1_SKY_VELOCITY = 50.0;
const double INIT_BACKGROUND_2_TREE_VELOCITY = 100.0;
const double INIT_BACKGROUND_3_BUILDINGS_VELOCITY = 150.0;

const color_t SPRITE_COLOR = (color_t){0.0, 0.0, 0.0};
const color_t OBS_COLOR = (color_t){0.2, 0.2, 0.3};  // going to be tables
const color_t QUESADILLA_COLOR = (color_t){1, 1, 0}; // coin

const size_t BODY_ASSETS = 1;
const char *PLAYER_SPRITE_PATH = "assets/frogger.png";
const char *BACKGROUND_PATH = "assets/frogger-background.png";

const double OBSTACLE_START_WAIT_TIME = 3.0;
const double GAME_OVER_WAIT_TIME = 3.0;

const size_t MAX_GAMES = 1024;

typedef enum {
  // Home includes leaderboard, store, start button
  HOME = 'H',
  GAME = 'G'
} GAME_SCREEN;

typedef enum {
  REGULAR = 0,
  JUMP = 1,
  DUCK = 2
} PLAYER_MOTION;

struct state {
  GAME_SCREEN current_game_screen;

  body_t *player;
  PLAYER_MOTION player_motion;
  vector_t player_velocity;
  scene_t *scene;

  vector_t bg_1_sky_vel;
  vector_t bg_2_tree_vel;
  vector_t bg_3_building_vel;

  bool is_game_over;
  // Powerups
  bool is_revival_activated;
  bool is_magnet_activated;

  size_t points;
  list_t *all_points;
};

body_t *make_obstacle(double outer_radius, double inner_radius,
                      vector_t center) {
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

void revert_jump(state_t* state){
  if (state->player_velocity.y <=
      -JUMP_INITIAL_VELOCITY) { // TODO: Replace with if colliding with ground
    state->player_velocity.y = 0;
    state->player_motion = REGULAR;
  } else {
    printf("I'm here!\n");
    state->player_velocity.y += Y_GRAVITY_ACCELERATION;
  }
}

void revert_duck(state_t* state){
  if (state->player_velocity.y >
      -DUCK_INITIAL_VELOCITY) { // TODO: Replace with if colliding with ground
    state->player_velocity.y = 0;
    state->player_motion = REGULAR;
  } else {
    state->player_velocity.y += DUCK_ACCELERATION_CHANGE;
  }
}

void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
  // vector_t translation = (vector_t){0, 0};
  if (type == KEY_PRESSED && state->player_motion == REGULAR) {
    switch (key) {
    case UP_ARROW:
      printf("Uppies!\n");
      state->player_velocity.y = JUMP_INITIAL_VELOCITY;
      state->player_motion = JUMP;
      break;
    case DOWN_ARROW:
      state->player_velocity.y = DUCK_INITIAL_VELOCITY;
      state->player_motion = DUCK;
      break;
    }
  }
}
void manipulate_player(state_t* state){
  switch (state->player_motion){
    case JUMP:
      revert_jump(state);
      break;
    case DUCK:
      revert_duck(state);
      break;
    case REGULAR:
      sdl_on_key((key_handler_t)on_key);
      break;
    default:
      fprintf(stderr, "Player is not moving in a valid way.");
      exit(2);
  }
  body_t *player = scene_get_body(state->scene, 0);
  vector_t new_centroid =
        vec_add(body_get_centroid(player), state->player_velocity);
  body_set_centroid(player, new_centroid);
}
void start_game(state_t *state) {
  // TODO: Week 2 - Change state of screen to game
}

void end_game(state_t *state) {
  // TODO: Week 2 - End the game, show the score, and go back to home after
  // GAME_OVER_WAIT_TIME seconds
}

// Arjun
void spawn_obstacles(state_t *state) {
  // TODO: Week 1 - Spawn obstacles at a random interval. The right end should
  // be at the far left of the screen
}

// Arjun
void clean_elapsed_obstacles(state_t *state) {
  // TODO: Week 1 - Free tables after they exit the viewport
}

// Andrea
void update_bg_velocity(state_t *state) {
  // TODO: Week 2 - Update velocity
}

// Andrea
void wrap_backgrounds(state_t *state) {
  // TODO: Week 1 - Wrap backgrounds
}

void spawn_coins(state_t *state) {
  // TODO: Week 2 - spawn coins at random intervals
}

void clean_elapsed_coins(state_t *state) {
  // TODO: Week 2  - Remove coins after they hit the end of the screen
}

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
  body_t *froggy = body_init(c, 1, SPRITE_COLOR);
  return froggy;
}


state_t *emscripten_init() {

  asset_cache_init();
  sdl_init(MIN, MAX);

  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();
  state->current_game_screen = HOME;

  srand(time(NULL));
  state->scene = scene_init();
  state->player_motion = REGULAR;
  state->player_velocity = (vector_t){.x = 0, .y = 0};

  body_t *player = make_player_sprite(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
  body_set_centroid(player, RESET_POS);
  state->player = player;
  scene_add_body(state->scene, player);

  // TODO: Initialize all 3 backgrounds
  SDL_Rect rect = (SDL_Rect){.x = 0, .y = 0, .w = MAX.x, .h = MAX.y};
  asset_make_image(BACKGROUND_PATH, rect);
  asset_make_image_with_body(PLAYER_SPRITE_PATH, player);
  sdl_on_key((key_handler_t)on_key);

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

  // wrap_backgrounds(state);
  // update_bg_velocity(state);
  
  list_t *body_assets = asset_get_asset_list();
  for (size_t i = 0; i < list_size(body_assets); i++) {
    asset_render(list_get(body_assets, i));
  }
  // spawn_obstacles(state);
  // clean_elapsed_obstacles(state);

  sdl_show();
  scene_tick(state->scene, dt);
  manipulate_player(state);
  return false;
}

void emscripten_free(state_t *state) {
  list_free(asset_get_asset_list());
  list_free(state->all_points);
  scene_free(state->scene);
  asset_cache_destroy();
  free(state);
}