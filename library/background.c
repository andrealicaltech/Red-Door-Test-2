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

// initialize background
void background_init(state_t *state) {
  state->bg =
      (background_t){.bg_1_sky_vel = INIT_BACKGROUND_1_SKY_VELOCITY,
                     .bg_2_tree_vel = INIT_BACKGROUND_2_TREE_VELOCITY,
                     .bg_3_building_vel = INIT_BACKGROUND_3_BUILDINGS_VELOCITY,
                     .sky_pos = SKY_BACKGROUND,
                     .tree_pos = TREE_BACKGROUND,
                     .building_pos = BUILD_BACKGROUND};
}

// increase speed of player + backgrounds
void update_bg_velocity(state_t *state) {
  vector_t add = (vector_t){.x = 0.01, .y = 0};
  state->bg.bg_1_sky_vel = vec_add(add, state->bg.bg_1_sky_vel);
  state->bg.bg_2_tree_vel = vec_add(add, state->bg.bg_2_tree_vel);
  state->bg.bg_3_building_vel = vec_add(add, state->bg.bg_3_building_vel);
}

void update_bg_pos(state_t *state, double dt) {
  state->bg.bg_1_sky_vel = vec_add(state->bg.bg_1_sky_vel, vec_multiply(dt, state->bg.bg_1_sky_vel));
  state->bg.bg_2_tree_vel = vec_add(state->bg.bg_2_tree_vel, vec_multiply(dt, state->bg.bg_2_tree_vel));
  state->bg.bg_3_building_vel = vec_add(state->bg.bg_3_building_vel, vec_multiply(dt, state->bg.bg_3_building_vel));

  body_set_centroid(state->bg.building_body, BUILD_CENTER);
  body_set_centroid(state->bg.tree_body, TREE_CENTER);
  body_set_centroid(state->bg.sky_body, SKY_CENTER);
}

// Wrap backgrounds for scrolling effect
void wrap_backgrounds(state_t *state) {
  double limit = -(PANEL_WIDTH - MAX.x);

  if (state->bg.building_pos.x <= limit) {
    state->bg.building_pos.x = 0.0f;
  }
  if (state->bg.tree_pos.x <= limit) {
    state->bg.tree_pos.x = 0.0f;
  }
  if (state->bg.sky_pos.x <= limit) {
    state->bg.sky_pos.x = 0.0f;
  }
}
