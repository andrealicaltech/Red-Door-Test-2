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

// initialize background
void background_init(state_t *state) {
  state->bg =
      (background_t){.bg_1_sky_vel = INIT_BACKGROUND_1_SKY_VELOCITY,
                     .bg_2_tree_vel = INIT_BACKGROUND_2_TREE_VELOCITY,
                     .bg_3_building_vel = INIT_BACKGROUND_3_BUILDINGS_VELOCITY,
                     .sky_pos = SKY_BACKGROUND,
                     .tree_pos = TREE_BACKGROUND,
                     .build_pos = BUILD_BACKGROUND};
}

void update_bg_velocity(state_t *state) {
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
