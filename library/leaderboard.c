#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "asset.h"
#include "asset_cache.h"
#include "collision.h"
#include "constants.h"
#include "forces.h"
#include "game_state.h"
#include "kinematics.h"
#include "obstacle.h"

body_t *render_scoreboard(state_t *state) {
  // NOTE: TTF_Init inside emscripten main
  vector_t center = (vector_t){MAX.x - LEADERBOARD_SIZE.x / 2,
                               MAX.y - LEADERBOARD_SIZE.y / 2};

  body_t *scoreboard_bg_rectangle = make_rectangle_body(
      LEADERBOARD_SIZE.x, LEADERBOARD_SIZE.y, center, LEADERBOARD_COLOR);
  body_t *text_rectangle = sdl_get_rect(
      MAX.x - LEADERBOARD_SIZE.x * 0.75, MAX.y - LEADERBOARD_SIZE.y * 0.75,
      LEADERBOARD_SIZE.x * 0.5, LEADERBOARD_SIZE.y * 0.5);
}