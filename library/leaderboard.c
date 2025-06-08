#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "asset.h"
#include "asset_cache.h"
#include "collision.h"
#include "constants.h"
#include "forces.h"
#include "game_state.h"
#include "kinematics.h"
#include "math_utils.h"
#include "obstacle.h"

body_t *make_scoreboard(double width, double height, vector_t center,
                        color_t color) {
  list_t *rect = list_init(4, free);

  vector_t *vec_1 = malloc(sizeof(vector_t));
  *vec_1 = (vector_t){center.x + width / 2, center.y + height / 2};
  list_add(rect, vec_1);

  vector_t *vec_2 = malloc(sizeof(vector_t));
  *vec_2 = (vector_t){center.x - width / 2, center.y + height / 2};
  list_add(rect, vec_2);

  vector_t *vec_3 = malloc(sizeof(vector_t));
  *vec_3 = (vector_t){center.x - width / 2, center.y - height / 2};
  list_add(rect, vec_3);
  vector_t *vec_4 = malloc(sizeof(vector_t));
  *vec_4 = (vector_t){center.x + width / 2, center.y - height / 2};
  list_add(rect, vec_4);
  body_t *leaderboard = body_init_with_info(rect, 1, LEADERBOARD_COLOR,
                                            (void *)LEADERBOARD_INFO, NULL);
  return leaderboard;
}

void create_scoreboard(state_t *state) {
  // NOTE: TTF_Init inside emscripten main
  vector_t center = (vector_t){MAX.x - LEADERBOARD_SIZE.x / 2,
                               MAX.y - LEADERBOARD_SIZE.y / 2};
  body_t *scoreboard_bg_rectangle = make_scoreboard(
      LEADERBOARD_SIZE.x, LEADERBOARD_SIZE.y, center, LEADERBOARD_COLOR);
  printf("Scene size: %d\n", scene_bodies(state->scene));
  scene_add_body(state->scene, scoreboard_bg_rectangle);
}

void render_text(state_t *state) {
  SDL_Rect *score_rectangle =
      sdl_get_rect(MAX.x - LEADERBOARD_SIZE.x * 0.75, LEADERBOARD_SIZE.y * 0.1,
                   LEADERBOARD_SIZE.x * 0.5, LEADERBOARD_SIZE.y * 0.3);
  SDL_Rect *high_score_rectangle =
      sdl_get_rect(MAX.x - LEADERBOARD_SIZE.x * 0.9, LEADERBOARD_SIZE.y * 0.6,
                   LEADERBOARD_SIZE.x * 0.8, LEADERBOARD_SIZE.y * 0.3);
  char *high_score_text = malloc(
      sizeof(char) * (log10(1 + state->points) + strlen("High score: ") + 10));
  char *score_text = malloc(
      sizeof(char) * (log10(1 + state->points) + strlen("Score: ") + 10));
  sprintf(score_text, "Score: %d", (size_t)(state->points));
  if (list_size(state->all_points) == 0) {
    sprintf(high_score_text, "High Score: %d", state->points);
  } else {
    printf("%d\n", *((double *)(list_get(state->all_points, 0))));
    sprintf(high_score_text, "High Score: %d",
            (size_t)(max_d(*((double *)(list_get(state->all_points, 0))),
                           state->points)));
  }
  sdl_render_text(state->font, TEXT_COLOR, score_text, score_rectangle);
  sdl_render_text(state->font, TEXT_COLOR, high_score_text,
                  high_score_rectangle);
  free(score_text);
  free(high_score_text);
}