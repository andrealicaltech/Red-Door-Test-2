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

body_t *make_scoreboard(double width, double height, vector_t center, color_t color) {
    list_t *rect = list_init(4, free);
  
    vector_t *vec_1 = malloc(sizeof(vector_t));
    *vec_1 = (vector_t){0, 0};
    list_add(rect, vec_1);
  
    vector_t *vec_2 = malloc(sizeof(vector_t));
    *vec_2 = (vector_t){width, 0};
    list_add(rect, vec_2);
  
    vector_t *vec_3 = malloc(sizeof(vector_t));
    *vec_3 = (vector_t){width, height};
    list_add(rect, vec_3);
  
    vector_t *vec_4 = malloc(sizeof(vector_t));
    *vec_4 = (vector_t){0, height};
    list_add(rect, vec_4);
    body_t *leaderboard = body_init(rect, 1, color);
    return leaderboard;
  }

body_t *render_scoreboard(state_t *state) {
  // NOTE: TTF_Init inside emscripten main
  vector_t center = (vector_t){MAX.x - LEADERBOARD_SIZE.x / 2,
                               MAX.y - LEADERBOARD_SIZE.y / 2};

  body_t *scoreboard_bg_rectangle = make_scoreboard(
      LEADERBOARD_SIZE.x, LEADERBOARD_SIZE.y, center, LEADERBOARD_COLOR);
  body_t *text_rectangle = sdl_get_rect(
      MAX.x - LEADERBOARD_SIZE.x * 0.75, MAX.y - LEADERBOARD_SIZE.y * 0.75,
      LEADERBOARD_SIZE.x * 0.5, LEADERBOARD_SIZE.y * 0.5);
}