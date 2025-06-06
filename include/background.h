#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

// background
extern const vector_t SKY_BACKGROUND;
extern const vector_t SKY_CENTER;
extern const vector_t TREE_BACKGROUND;
extern const vector_t TREE_CENTER;
extern const vector_t BUILD_BACKGROUND;
extern const vector_t BUILD_CENTER;
extern const size_t PANEL_WIDTH;

void background_init(state_t *state);

void update_bg_pos(state_t *state, double dt);

void update_bg_velocity(state_t *state, double dt);

void wrap_backgrounds(state_t *state);

#endif