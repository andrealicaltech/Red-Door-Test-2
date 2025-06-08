#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__

#include <SDL2/SDL.h>
#include <stdlib.h>

#include "constants.h"
#include "scene.h"
#include "state.h"

typedef enum {
  // Home includes leaderboard, store, start button
  HOME = 'H',
  GAME = 'G'
} GAME_SCREEN;

typedef enum { REGULAR = 0, JUMP = 1, FALLING = 2, DUCK = 3 } PLAYER_MOTION;

typedef struct {
  vector_t bg_1_sky_vel;
  vector_t bg_2_tree_vel;
  vector_t bg_3_building_vel;
  vector_t sky_pos;
  vector_t tree_pos;
  vector_t building_pos;
  body_t *sky_body;
  body_t *tree_body;
  body_t *building_body;
} background_t;

struct state {
  scene_t *scene;

  // screens
  GAME_SCREEN current_game_screen;
  bool is_game_over;
  bool started;
  bool show_shop;
  char *sprite_path;

  // background
  background_t bg;
  body_t *player;
  SDL_Renderer *rend;

  PLAYER_MOTION player_motion;
  double jump_start_y;

  // Obstacles
  double time_till_next_update;
  size_t n_queued_obstacles;
  body_t *curr_player_obstacle;

  // Powerups
  bool is_magnet_activated;

  double points;
  size_t n_coins_collected;
  list_t *all_points;
  const TTF_Font *font;
};

#endif