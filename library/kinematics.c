#include "body.h"
#include "music.h"
#include "state.h"

#include "asset.h"
#include "background.h"
#include "constants.h"
#include "game_state.h"
#include "kinematics.h"
#include "utils.h"

void revert_duck(state_t *state) {}

vector_t get_curr_jump_vel(state_t *state) {
  return (vector_t){.x = 0,
                    .y = JUMP_VEL_COMPONENT_RATIO *
                         INIT_BACKGROUND_3_BUILDINGS_VELOCITY.x};
}

vector_t get_curr_gravity(state_t *state) {
  return vec_multiply(GRAV_JUMP_VEL_RATIO, get_curr_jump_vel(state));
}

void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
  if (state->show_shop) {
    switch (key) {
    case SPACE_BAR:
      reset_game(state);
      return;
    case LEFT_ARROW:
      if (state->n_coins_collected >= PRICE) {
        state->sprite_path = (char *)PLAYER_SPRITE_ANDREA_PATH;
        state->n_coins_collected = state->n_coins_collected - PRICE;
      }
      return;
    case UP_ARROW:
      state->sprite_path = (char *)PLAYER_SPRITE_AMUDHAN_PATH;
      return;
    case RIGHT_ARROW:
      if (state->n_coins_collected >= PRICE) {
        state->sprite_path = (char *)PLAYER_SPRITE_ARJUN_PATH;
        state->n_coins_collected = state->n_coins_collected - PRICE;
      }
    }
  }

  if (type == KEY_PRESSED && state->player_motion == REGULAR) {
    if (!state->show_shop && !state->started && key == SPACE_BAR) {
      state->started = true;
      state->current_game_screen = GAME;
      halt_music();
      return;
    }

    if (!state->show_shop && state->started &&
        state->player_motion == REGULAR) {

      switch (key) {
      case UP_ARROW: {
 body_set_velocity(state->player, get_curr_jump_vel(state));
        state->player_motion = JUMP;
        state->jump_start_y = body_get_centroid(state->player).y;
        state->curr_player_obstacle = NULL;
        break;
      }
      case DOWN_ARROW: {
        char *new_path = get_player_sprite_duck_path(state);
        free(state->sprite_path);
        state->sprite_path = new_path;
        asset_remove_body(state->player);
        asset_make_image_with_body(state->sprite_path, state->player);
        state->player_motion = DUCK;
        break;
      }
      }
    }
  }

  if (type == KEY_RELEASED && state->player_motion == DUCK &&
      key == DOWN_ARROW) {
    char *new_path = get_player_sprite_normal_path(state);
    free(state->sprite_path);
    state->sprite_path = new_path;
    asset_remove_body(state->player);
    asset_make_image_with_body(state->sprite_path, state->player);
    state->player_motion = REGULAR;
  }
}

void manipulate_player(state_t *state, double dt) {
  vector_t player_centroid = body_get_centroid(state->player);
  vector_t player_velocity = body_get_velocity(state->player);

  switch (state->player_motion) {
  case JUMP:
  case FALLING:
    if (player_centroid.y < PLAYER_CENTER_POS.y ||
        (player_velocity.y <= 0.0 && player_centroid.y > state->jump_start_y &&
         (player_centroid.y - state->jump_start_y <
          (get_curr_gravity(state).y * dt)))) {
      body_set_velocity(state->player, VEC_ZERO);
      body_set_centroid(state->player, (vector_t){.x = PLAYER_CENTER_POS.x,
                                                  .y = state->jump_start_y});
      if (player_centroid.y == PLAYER_CENTER_POS.y) {
        // Edge case where jump off obstacle before falling off
        state->curr_player_obstacle = NULL;
      }
      state->player_motion = REGULAR;
    } else {
      double new_y_vel = player_velocity.y - (get_curr_gravity(state).y * dt);
      if (player_velocity.y <= 0) {
        new_y_vel = max_d(new_y_vel, -1.0 * JUMP_INITIAL_VELOCITY.y);
      }
      body_set_velocity(state->player,
                        (vector_t){.x = 0, .y = 1.0 * new_y_vel});
    }
    break;
  case DUCK:
    break;
  case REGULAR:
    sdl_on_key((key_handler_t)on_key);
    break;
  default:
    fprintf(stderr, "Player is not moving in a valid way.");
    exit(2);
  }
}

/*
Part of obstacle / quesedilla but abstracted to kinematics
*/
double get_smallest_obst_clearing_dist(state_t *state, double h_player,
                                       double h_obstacle) {
  double u = get_curr_jump_vel(state).y;
  double min_del_h = h_obstacle;
  /*

  Find the time which results in the bottom of the player exactly hitting the
  top edge of the obstacle Solve for t in the y-axis h_o - h_p = ut - 0.5gt^2
  which gives (u + sqrt(u^2 - 2g(h_o-h_p)))/g
  */
  double time =
      (u + sqrt((u * u) - 2 * get_curr_gravity(state).y * min_del_h)) /
      get_curr_gravity(state).y;
  double vx = state->bg.bg_3_building_vel.x;

  // Return the x-distance that will be covered in that time
  return vx * time;
}