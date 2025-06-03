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

/*
MARK: Constants
*/
const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};

const vector_t PLAYER_DIMS = {10, 60};
const vector_t PLAYER_CENTER_POS = {500, 250};
const char *PLAYER_INFO = "player";

const double OUTER_RADIUS = 15;
const double INNER_RADIUS = 15;

const double MIN_REACTION_TIME_S = 1.5;

// obstacle = table (square 50 x 50) can spawn multiple obstacles in a row
const size_t OBSTACLE_HW = 50;
const size_t MAX_STACKED_OBSTACLES = 5;
const double FIRST_OBSTACLE_WAIT_TIME = 3.0;
const size_t OBSTACLE_CAPACITY = 1024;
const double AVG_TIME_OBSTACLES = 5.0;
const char *OBSTACLE_INFO = "obstacle";

// pts of player depending on action
const size_t PLAYER_STANDING_PTS = 16;
const size_t PLAYER_RUNNING = 31;
const size_t QUESADILLA_PTS = 20;

// player movements
const vector_t DUCK_INITIAL_VELOCITY = (vector_t){.x = 0, .y = 0};
const vector_t JUMP_INITIAL_VELOCITY = (vector_t){.x = 0, .y = 10};
const double Y_GRAV_ACCELERATION_MAG = 50.0;
const double DUCK_ACCELERATION_CHANGE = 0;

// Background and obstacle velocity. Background 3 is the top layer (i.e. the
// velocity of the game)
const double INIT_BACKGROUND_1_SKY_VELOCITY = 50.0;
const double INIT_BACKGROUND_2_TREE_VELOCITY = 100.0;
const double INIT_BACKGROUND_3_BUILDINGS_VELOCITY = 150.0;

// Background positions
vector_t SKY_BACKGROUND = {4000, 500};
vector_t TREE_BACKGROUND = {4000, 400};
vector_t BUILD_BACKGROUND = {4000, 300};
size_t PANEL_WIDTH = 4000;

const color_t SPRITE_COLOR = (color_t){0.0, 0.0, 0.0};
const color_t OBS_COLOR = (color_t){0.2, 0.2, 0.3};  // going to be tables
const color_t QUESADILLA_COLOR = (color_t){1, 1, 0}; // coin

const size_t BODY_ASSETS = 1;
const char *PLAYER_SPRITE_PATH = "assets/frogger.png";
const char *BACKGROUND_PATH = "assets/background.jpg";

const double OBSTACLE_START_WAIT_TIME = 3.0;
const double GAME_OVER_WAIT_TIME = 3.0;

const size_t MAX_GAMES = 1024;

typedef enum {
  // Home includes leaderboard, store, start button
  HOME = 'H',
  GAME = 'G'
} GAME_SCREEN;

typedef enum { REGULAR = 0, JUMP = 1, DUCK = 2 } PLAYER_MOTION;

typedef struct {
  vector_t bg_1_sky_vel;
  vector_t bg_2_tree_vel;
  vector_t bg_3_building_vel;
  vector_t sky_pos;
  vector_t tree_pos;
  vector_t build_pos;
} background_t;

struct state {
  scene_t *scene;

  GAME_SCREEN current_game_screen;
  bool is_game_over;

  background_t bg;
  body_t *player;
  PLAYER_MOTION player_motion;
  vector_t player_velocity;

  
  // Obstacles
  double time_till_next_obstacle;
  size_t n_queued_obstacles;

  // Powerups
  bool is_revival_activated;
  bool is_magnet_activated;

  size_t points;
  list_t *all_points;
};


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

void revert_duck(state_t *state) {
  body_t *player_body = scene_get_body(state->scene, 0);
  vector_t velocity = body_get_velocity(player_body);

  // if (velocity.y >
  //     -DUCK_INITIAL_VELOCITY) { // TODO: Replace with if colliding with
  //     ground
  //   state->player_velocity.y = 0;
  //   state->player_motion = REGULAR;
  // } else {
  //   state->player_velocity.y += DUCK_ACCELERATION_CHANGE;
  // }
}

vector_t get_curr_jump_vel(state_t *state) {
  // TODO: Calculate correct value required to guarantee can clear jump
  return (vector_t){.x = 0, .y = JUMP_INITIAL_VELOCITY.y};
}

void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
  body_t *player_body = scene_get_body(state->scene, 0);
  assert(strcmp(body_get_info(player_body), PLAYER_INFO) == 0);

  if (type == KEY_PRESSED && state->player_motion == REGULAR) {
    switch (key) {
    case UP_ARROW:
      printf("Up key pressed in regular motion!\n");
      body_set_velocity(player_body, get_curr_jump_vel(state));
      state->player_motion = JUMP;
      break;
    case DOWN_ARROW:
      printf("Down key pressed in regular motion");
      body_set_velocity(player_body, DUCK_INITIAL_VELOCITY);
      state->player_motion = DUCK;
      break;
    }
  }
}

void manipulate_player(state_t *state, double dt) {
  body_t *player_body = scene_get_body(state->scene, 0);
  vector_t player_centroid = body_get_centroid(player_body);

  switch (state->player_motion) {
  case JUMP:
    // Applies a force over the current tick
    if (player_centroid.y <= PLAYER_CENTER_POS.y) {
      printf("Changing to regular state\n");
      state->player_motion = REGULAR;
      body_set_velocity(player_body, (vector_t){.x = 0, .y = 0});
    } else {
      vector_t current_player_vel = body_get_velocity(player_body);
      printf("dely=%f\n", Y_GRAV_ACCELERATION_MAG * dt);
      body_set_velocity(
          player_body,
          vec_subtract(current_player_vel,
                       (vector_t){.x = 0, .y = dt * Y_GRAV_ACCELERATION_MAG}));
    }
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
}

void start_game(state_t *state) {
  // TODO: Week 2 - Change state of screen to game
}

void end_game(state_t *state) {
  // TODO: Week 2 - End the game, show the score, and go back to home after
  // GAME_OVER_WAIT_TIME seconds
}

/*
MARK: Obstacle code
TODO: Move all of these to obstacles.h file in the future
*/
body_t *make_obstacle(size_t w, size_t h, vector_t center) {
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
  body_t *obstacle = body_init(c, 1, OBS_COLOR);
  body_set_centroid(obstacle, center);
  return obstacle;
}

double mod_d(double a, double b) {
  /*
  Equivalent to a % b. Cast (a/b) to an int and multiply that by b. Return
  difference from a
  */
  return a - (int)(a / b) * b;
}

double max_d(double a, double d) { return a > b ? a : b; }

vector_t get_obstacle_dims(body_t *obstacle) {
  assert(strcmp(body_get_info(obstacle), OBSTACLE_INFO) == 0);
  return *((vector_t *)list_get(body_get_shape(obstacle), 3));
}

/*
  Kinematics-based calculation of the smallest distance before an obstacle
  at which the player can currently jump without colliding with the obstacle's
  vertical edge

  `h_player`: distance between ground and player centroid
  `h_obstacle`: distance between ground and top of obstacle
*/
double get_smallest_obst_clearing_dist(state_t *state, double h_player,
                                       double h_obstacle) {
  double u = get_curr_jump_vel(state).y;

  double min_del_h = h_obstacle - h_player;
  /*
  Solve for t in the y-axis
  h_o - h_p = ut - 0.5gt^2
  which gives (u + sqrt(u^2 - 2g(h_o-h_p)))/g
  */
  double time = (u + sqrt(u * u - 2 * Y_GRAV_ACCELERATION_MAG * min_del_h)) /
                Y_GRAV_ACCELERATION_MAG;
  double vx = state->bg.bg_3_building_vel.x;
  return vx / time;
}

double next_obst_x(state_t *state, body_t *last_obstacle) {
  /*
  Guarrantee that if the player jumps from the latest possible point to clear
  the last obstacle in the queue, there is sufficient space before the next
  obstacle for them to jump at the earliest possible point and clear the
  obstacle.
  */

  vector_t last_obstacle_dims = get_obstacle_dims(last_obstacle);
  vector_t curr_obst_speed = state->bg.bg_3_building_vel;

  double expected_x_dist_with_jump =
      (2 * get_curr_jump_vel(state).y / Y_GRAV_ACCELERATION_MAG) *
      curr_obst_speed.x;
  double furthest_poss_x =
      (body_get_centroid(last_obstacle).x - last_obstacle_dims.x -
       get_smallest_obst_clearing_dist(state, PLAYER_DIMS.y, last_obstacle.y)) +
      expected_x_dist_with_jump;

  // Additional random spacing between obstacles
  double running_space = rand() % ((int)MAX.x);
  // if running space is low, need to guarantee that we give the player enough
  // distance to jump such that they clear the height of the obstacle
  double clearing_space =
      get_smallest_obst_clearing_dist(state, PLAYER_DIMS.y, OBSTACLE_HW);

  // It is possible that this sum is small enough that it doesn't given
  // reasonable reaction time for a player
  return max_d(furthest_poss_x + running_space + clearing_space,
               MIN_REACTION_TIME_S * curr_obst_speed.x);
}

void update_obstacles(state_t *state) {
  /*
  Called in the main loop.
  Check if the timer to spawn the next obstacle has elapsed.
  If so, add an obstacle to the edge of the far left of the screen and reset a
  random timer
  */
  if (state -> time_till_next_obstacle <= 0.0){
    // TODO: Create an obstacle
    state->time_till_next_obstacle =  mod_d((double)rand(), AVG_TIME_BULLETS);
  }
}

// Arjun
void clean_obstacles(state_t *state) {
  /*
  Removes obstacles after they exit the viewport
  */

  // First obstacle is always
  for (size_t i = 1; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(scene, i);
    if (strcmp(body_get_info(body), OBSTACLE_INFO) == 0) {
      vector_t obst_pos = body_get_centroid(body);
      if (obst_pos.x > MAX.x) {
        body_free(body);
        state->n_queued_obstacles -= 1;
      }
    }
  }
}

/*
MARK: Backgrounds
*/

// Andrea
void update_bg_velocity(state_t *state) {
  // TODO: Week 2 - Update velocity
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
  // TODO: Initialize state-bg here?
  SDL_Rect rect = (SDL_Rect){.x = 0, .y = 0, .w = MAX.x, .h = MAX.y};
  asset_make_image(BACKGROUND_PATH, rect);
  asset_make_image_with_body(PLAYER_SPRITE_PATH, player);
  sdl_on_key((key_handler_t)on_key);

  // Obstacles
  state->time_till_next_obstacle = FIRST_OBSTACLE_WAIT_TIME;
  state->n_queued_obstacles = 0;

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