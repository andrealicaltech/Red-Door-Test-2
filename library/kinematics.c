#include "body.h"
#include "state.h"

#include "constants.h"
#include "game_state.h"
#include "kinematics.h"
#include "math_utils.h"


const double JUMP_RESTORE_TOLERANCE = 2.0;

void revert_duck(state_t *state) {
  body_t *player_body = scene_get_body(state->scene, 0);
  // if (velocity.y >
  //     -DUCK_INITIAL_VELOCITY) { // TODO: Replace with if colliding with
  //     ground
  //   state->player_velocity.y = 0;
  //   state->player_motion = REGULAR;
  // } else {
  //   state->player_velocity.y += DUCK_ACCELERATION_CHANGE;
  // }
}

vector_t get_curr_jump_vel(state_t *state) {
  // TODO: Calculate correct value required to guarantee can clear jump
  return (vector_t){.x = 0, .y = JUMP_INITIAL_VELOCITY.y};
}

void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
  body_t *player_body = scene_get_body(state->scene, 0);
  assert(strcmp(body_get_info(player_body), PLAYER_INFO) == 0);

  if (type == KEY_PRESSED && state->player_motion == REGULAR) {
    switch (key) {
    case UP_ARROW:
      printf("Up key pressed in regular motion!\n");
      body_set_velocity(player_body, get_curr_jump_vel(state));
      state->player_motion = JUMP;
      state->jump_start_y = body_get_centroid(player_body).y;
      break;
    case DOWN_ARROW:
      printf("Down key pressed in regular motion");
      body_set_velocity(player_body, DUCK_INITIAL_VELOCITY);
      state->player_motion = DUCK;
      break;
    }
  }
}

void manipulate_player(state_t *state, double dt) {
  body_t *player_body = scene_get_body(state->scene, 0);
  assert(strcmp(body_get_info(player_body), PLAYER_INFO) == 0);

  vector_t player_centroid = body_get_centroid(player_body);
  vector_t player_velocity = body_get_velocity(player_body);

  switch (state->player_motion) {
  case JUMP:
  case FALLING:
  if (player_velocity.y <= 0.0 && abs_d(body_get_centroid(player_body).y - state->jump_start_y) < JUMP_RESTORE_TOLERANCE){
      body_set_velocity(player_body, VEC_ZERO);
      body_set_centroid(player_body, (vector_t) {.x=PLAYER_CENTER_POS.x, .y=state->jump_start_y});
      printf("Stopping jump\n");
      state->player_motion = REGULAR;
    } else {
      double new_y_vel =
          player_velocity.y - (Y_GRAV_ACCELERATION_MAG_PER_S * dt);
      if (player_velocity.y <= 0) {
        new_y_vel = max_d(new_y_vel, -1.0 * JUMP_INITIAL_VELOCITY.y);
      }
      body_set_velocity(player_body, (vector_t){.x = 0, .y = 1.0 * new_y_vel});
    }
    break;
  case DUCK:
    revert_duck(state);
    break;
  case REGULAR:
    sdl_on_key((key_handler_t)on_key);
    break;
  default:
    fprintf(stderr, "Player is not moving in a valid way.");
    exit(2);
  }
}