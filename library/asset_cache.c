#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset_cache.h"
#include "list.h"
#include "sdl_wrapper.h"

static list_t *ASSET_CACHE;
static list_t *TEMP_CACHE;

const size_t FONT_SIZE = 18;
const size_t INITIAL_CAPACITY = 5;

typedef struct {
  asset_type_t type;
  const char *filepath;
  void *obj;
} entry_t;

static void asset_cache_free_entry(entry_t *entry) {
  if (entry->type == ASSET_IMAGE) {
    SDL_DestroyTexture(entry->obj);
  } else if (entry->type == ASSET_TEXT) {
    TTF_CloseFont(entry->obj);
  }
  free(entry);
}

void asset_cache_init() {
  ASSET_CACHE =
      list_init(INITIAL_CAPACITY, (free_func_t)asset_cache_free_entry);
  TEMP_CACHE = list_init(INITIAL_CAPACITY, (free_func_t)asset_cache_free_entry);
}

void asset_cache_destroy() {
  list_free(ASSET_CACHE);
  list_free(TEMP_CACHE);
}

void asset_cache_reset_temp() {
  if (TEMP_CACHE != NULL) {
    list_free(TEMP_CACHE);
  }
  TEMP_CACHE = list_init(INITIAL_CAPACITY, (free_func_t)asset_cache_free_entry);
}

entry_t *helper_asset_cache(asset_type_t ty, const char *filepath) {
  for (size_t i = 0; i < list_size(ASSET_CACHE); i++) {
    entry_t *entry = (entry_t *)list_get(ASSET_CACHE, i);
    if (entry && entry->type == ty &&
        (strcmp(entry->filepath, filepath) == 0)) {
      return entry;
    }
  }
  return NULL;
}

void *asset_cache_obj_get_or_create(asset_type_t ty, const char *filepath) {
  entry_t *content = helper_asset_cache(ty, filepath);
  if (content == NULL) {
    entry_t *new_entry = (entry_t *)malloc(sizeof(entry_t));

    new_entry->type = ty;
    new_entry->filepath = filepath;

    if (new_entry->type == ASSET_IMAGE) {
      new_entry->obj = sdl_get_image_texture(new_entry->filepath);
    } else if (new_entry->type == ASSET_TEXT) {
      new_entry->obj = TTF_OpenFont(new_entry->filepath, FONT_SIZE);
    }

    list_add(ASSET_CACHE, new_entry);
    return new_entry->obj;
  }
  return content->obj;
}

// ADDED
void asset_cache_store_temp(const char *key, SDL_Texture *tex) {
  entry_t *new_entry = malloc(sizeof(entry_t));
  new_entry->type = ASSET_IMAGE;
  new_entry->filepath = key; // Using the key as identifier
  new_entry->obj = tex;
  list_add(TEMP_CACHE, new_entry);
}

SDL_Texture *asset_cache_lookup(const char *key) {
  for (size_t i = 0; i < list_size(TEMP_CACHE); i++) {
    entry_t *entry = list_get(TEMP_CACHE, i);
    if (entry && strcmp(entry->filepath, key) == 0) {
      return entry->obj;
    }
  }
  return NULL;
}
