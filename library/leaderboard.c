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
#include "obstacle.h"
#include "utils.h"

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

void create_reg_text(TTF_Font *font, vector_t top_left, vector_t size,
                     char *text, double count) {
  SDL_Rect *text_rectangle =
      sdl_get_rect(top_left.x, top_left.y, size.x, size.y);
  char *new_text =
      malloc(sizeof(char) * (log10(1 + count) + strlen(text) + 10));
  sprintf(new_text, "%s%d", text, (size_t)(count));
  sdl_render_text(font, TEXT_COLOR, new_text, text_rectangle);
  free(new_text);
}

void render_score_text(state_t *state) {
  vector_t score_rectangle_tl =
      (vector_t){MAX.x - LEADERBOARD_SIZE.x * 0.75, LEADERBOARD_SIZE.y * 0.1};
  vector_t score_size =
      (vector_t){LEADERBOARD_SIZE.x * 0.5, LEADERBOARD_SIZE.y * 0.3};
  create_reg_text(state->font, score_rectangle_tl, score_size,
                  "Score: ", state->points);
  SDL_Rect *high_score_rectangle =
      sdl_get_rect(MAX.x - LEADERBOARD_SIZE.x * 0.9, LEADERBOARD_SIZE.y * 0.6,
                   LEADERBOARD_SIZE.x * 0.8, LEADERBOARD_SIZE.y * 0.3);
  char *high_score_text = malloc(
      sizeof(char) * (log10(1 + state->points) + strlen("High score: ") + 10));
  if (list_size(state->all_points) == 0) {
    sprintf(high_score_text, "High Score: %d", state->points);
  } else {
    sprintf(high_score_text, "High Score: %d",
            (size_t)(max_d(*((double *)(list_get(state->all_points, 0))),
                           state->points)));
  }
  sdl_render_text(state->font, TEXT_COLOR, high_score_text,
                  high_score_rectangle);
  free(high_score_text);
}

void render_coin_text(state_t *state) {
  vector_t coin_rectangle_tl =
      (vector_t){LEADERBOARD_SIZE.x * 0.25, LEADERBOARD_SIZE.y * 0.35};
  vector_t coin_size =
      (vector_t){LEADERBOARD_SIZE.x * 0.5, LEADERBOARD_SIZE.y * 0.3};
  create_reg_text(state->font, coin_rectangle_tl, coin_size,
                  "Coins: ", state->n_coins_collected);
}
void update_score(state_t *state) {
  list_t *lowest_values = list_init(list_size(state->all_points) + 1, NULL);
  double *points_pointer = malloc(sizeof(double));
  *points_pointer = state->points;
  for (int i = list_size(state->all_points) - 1; i >= -1; i--) {
    if (i == -1 ||
        *((double *)(list_get(state->all_points, i))) > state->points) {
      list_add(state->all_points, (void *)points_pointer);
      break;
    } else {
      list_add(lowest_values, list_get(state->all_points, i));
      list_remove(state->all_points, i);
    }
  }
  for (int i = list_size(lowest_values) - 1; i >= 0; i--) {
    void *value = list_get(lowest_values, i);
    list_add(state->all_points, value);
  }
  state->points = 0;
  list_free(lowest_values);
}