#ifndef __MOUSE_H__
#define __MOUSE_H__

typedef enum {
  MOUSE_BUTTON_DOWN,
  MOUSE_BUTTON_UP,
  MOUSE_MOVED
} mouse_event_type_t;

// x, y in window to handle mouse position
typedef void (*mouse_handler_t)(double x, double y, mouse_event_type_t type,
                                state_t state);

void mouse_on_event(mouse_handler_t handler);

#endif
