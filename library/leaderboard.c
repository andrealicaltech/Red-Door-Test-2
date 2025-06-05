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

body_t *make_scoreboard() {
  list_t *c = list_init(4, free);
  vector_t *v1 = malloc(sizeof(vector_t));
  *v1 = (vector_t){MAX.x, MAX.y};
  list_add(c, v1);

  vector_t *v2 = malloc(sizeof(vector_t));
  *v2 = (vector_t){MAX.x, MAX.y - LEADERBOARD_SIZE.y};
  list_add(c, v2);

  vector_t *v3 = malloc(sizeof(vector_t));
  *v3 = (vector_t){MAX.x - LEADERBOARD_SIZE.x, MAX.y - LEADERBOARD_SIZE.y};
  list_add(c, v3);

  vector_t *v4 = malloc(sizeof(vector_t));
  *v4 = (vector_t){MAX.x - LEADERBOARD_SIZE.x, MAX.y};
  list_add(c, v4);

  vector_t center = (vector_t){MAX.x - LEADERBOARD_SIZE.x / 2,
                       MAX.y - LEADERBOARD_SIZE.y / 2};
  body_t *scoreboard =
      body_init_with_info(c, 1, LEADERBOARD_COLOR, (void *)OBSTACLE_INFO, NULL);
  body_set_centroid(scoreboard, center);
  return scoreboard;
}

