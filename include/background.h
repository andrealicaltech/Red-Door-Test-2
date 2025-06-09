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

void render_screen(const char *screen_key, SDL_Rect *viewport);

void render_layers(SDL_Texture *texture, double *x, SDL_Rect *viewport);

body_t *make_object(double w, double h, vector_t center, void *info);

body_t *init_player(state_t *state);

void init_parameters(state_t *state);

void reset_game(state_t *state);

void set_layers(state_t *state);

void init_screens(state_t *state);

void make_layers(state_t *state);

void set_backgrounds(state_t *state);

#endif