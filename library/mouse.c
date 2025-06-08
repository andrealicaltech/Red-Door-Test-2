#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

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

static mouse_handler_t mouse_handler = NULL;

void mouse_on_event(mouse_handler_t handler) { mouse_handler = handler; }

void mouse_process(state_t *state, const SDL_Event *event) {
  if (mouse_handler == NULL) {
    return;
  }

  double x = 0, y = 0;
  mouse_event_type_t type;

  switch (event->type) {
  case SDL_MOUSEBUTTONDOWN:
    type = MOUSE_BUTTON_DOWN;
    x = event->button.x;
    y = event->button.y;
    break;
  case SDL_MOUSEBUTTONUP:
    type = MOUSE_BUTTON_UP;
    x = event->button.x;
    y = event->button.y;
    break;
  case SDL_MOUSEMOTION:
    type = MOUSE_MOVED;
    x = event->motion.x;
    y = event->motion.y;
    break;
  default:
    return;
  }

  mouse_handler(x, y, type, state);
}

void on_mouse(double x, double y, mouse_event_type_t type, state_t *state) {
  if (type != MOUSE_BUTTON_DOWN || !state->show_shop)
    return;

  SDL_Point mouse_pt = {.x = (int)x, .y = (int)y};

  if (SDL_PointInRect(&mouse_pt, &state->andrea_rect)) {
    printf("Andrea selected!\n");
    set_player_texture(state, "Andrea");
  } else if (SDL_PointInRect(&mouse_pt, &state->arjun_rect)) {
    printf("Arjun selected!\n");
    set_player_texture(state, "Arjun");
  } else if (SDL_PointInRect(&mouse_pt, &state->amudhan_rect)) {
    printf("Amudhan selected!\n");
    set_player_texture(state, "Amudhan");
  }
}
