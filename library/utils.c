#include <assert.h>
#include <math.h>

#include "constants.h"
#include "utils.h"
double mod_d(double a, double b) { return a - (int)(a / b) * b; }

double min_d(double a, double b) { return a < b ? a : b; }

double max_d(double a, double b) { return a > b ? a : b; }

char *get_player_sprite_duck_path(state_t *state) {
  char *ext = (char *)PLAYER_SPRITE_EXT;
  char *suffix = (char *)PLAYER_SPRITE_DUCK_PATH_SUFFIX;

  size_t path_len = strlen(state->sprite_path);
  size_t ext_len = strlen(ext);
  size_t suffix_len = strlen(suffix);

  size_t base_len = path_len - ext_len;
  size_t total_len = base_len + suffix_len + ext_len;

  char *sprite_name = malloc(total_len + 1);

  // Copy over without null terminator - then manually add in
  memcpy(sprite_name, state->sprite_path, base_len);
  sprite_name[base_len] = '\0';

  strcat(sprite_name, suffix);
  strcat(sprite_name, ext);
  return sprite_name;
}

char *get_player_sprite_normal_path(state_t *state) {
  size_t path_len = strlen(state->sprite_path);
  size_t ext_len = strlen(PLAYER_SPRITE_EXT);
  size_t suffix_len = strlen(PLAYER_SPRITE_DUCK_PATH_SUFFIX);

  size_t base_len = path_len - ext_len - suffix_len;
  size_t total_len = base_len + ext_len;

  char *normal_name = malloc(total_len + 1);

  memcpy(normal_name, state->sprite_path, base_len);
  normal_name[base_len] memcpy(normal_name + base_len, PLAYER_SPRITE_EXT,
                               ext_len);
  normal_name[total_len] = '\0';

  return normal_name;
}
