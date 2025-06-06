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
}

// increase speed of player + backgrounds
void update_bg_velocity(state_t *state) {
  vector_t add = (vector_t){.x = 0.01, .y = 0};
  state->bg.bg_1_sky_vel = vec_add(add, state->bg.bg_1_sky_vel);
  state->bg.bg_2_tree_vel = vec_add(add, state->bg.bg_2_tree_vel);
  state->bg.bg_3_building_vel = vec_add(add, state->bg.bg_3_building_vel);
}

void update_bg_pos(state_t *state, double dt) {
  state->bg.sky_pos.x -= state->bg.bg_1_sky_vel.x * dt;
  state->bg.tree_pos.x -= state->bg.bg_2_tree_vel.x * dt;
  state->bg.building_pos.x -= state->bg.bg_3_building_vel.x * dt;
}
