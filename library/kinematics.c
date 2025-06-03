#include "body.h"
#include "state.h"


#include "game_state.h"
#include "constants.h"
#include "kinematics.h"

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
  vector_t player_centroid = body_get_centroid(player_body);

  switch (state->player_motion) {
  case JUMP:
    // Applies a force over the current tick
    if (player_centroid.y <= PLAYER_CENTER_POS.y) {
      printf("Changing to regular state\n");
      state->player_motion = REGULAR;
      body_set_velocity(player_body, (vector_t){.x = 0, .y = 0});
    } else {
      vector_t current_player_vel = body_get_velocity(player_body);
      printf("dely=%f\n", Y_GRAV_ACCELERATION_MAG * dt);
      body_set_velocity(
          player_body,
          vec_subtract(current_player_vel,
                       (vector_t){.x = 0, .y = dt * Y_GRAV_ACCELERATION_MAG}));
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