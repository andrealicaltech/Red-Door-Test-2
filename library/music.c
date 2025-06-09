#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "constants.h"

const size_t AUDIO_PARAM_1 = 22050;
const size_t AUDIO_PARAM_2 = 2;
const size_t AUDIO_PARAM_3 = 4096;

// Our wave file
// Mix_Chunk *wave = NULL;
// Our music file
int play_music(char *music_path, bool side_by_side, Mix_Music *music,
               int value) {

  // Initialize SDL.
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    return -1;
  }
  // Initialize SDL_mixer
  if (Mix_OpenAudio(AUDIO_PARAM_1, MIX_DEFAULT_FORMAT, value, AUDIO_PARAM_3) ==
      -1) {
    return -1;
  }

  // Load our sound effect
  /*
      wave = Mix_LoadWAV(WAV_PATH);
      if (wave == NULL){
              return -1;
  }
  */

  // Load our music
  music = Mix_LoadMUS(music_path);
  if (music == NULL)
    return -1;
  // if ( Mix_PlayChannel(-1, wave, 0) == -1 )
  // return -1;

  if (!side_by_side && Mix_PlayingMusic()) {
    return -1;
  }
  if (Mix_PlayMusic(music, 1) == -1) {
    return -1;
  }
  // while ( Mix_PlayingMusic() ) ;

  // clean up our resources
  // Mix_FreeChunk(wave);
  // Mix_FreeMusic(music);

  // quit SDL_mixer
  // Mix_CloseAudio();

  return 0;
}

void halt_music() { Mix_HaltMusic(); }