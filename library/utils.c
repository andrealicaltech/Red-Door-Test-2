#include <assert.h>
#include <math.h>

#include "body.h"
#include "utils.h"

double mod_d(double a, double b) { return a - (int)(a / b) * b; }

double min_d(double a, double b) { return a < b ? a : b; }

double max_d(double a, double b) { return a > b ? a : b; }

body_t *get_player(state_t *state){
    body_t *player_body = scene_get_body(state->scene, state->current_player_idx);
    assert(strcmp(body_get_info(player_body), PLAYER_INFO) == 0);
    return player_body;
}
