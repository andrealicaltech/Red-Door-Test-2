#ifndef __MUSIC_H__
#define __MUSIC_H__
/**
 *Plays the music that is sent in to the file
 */
#include <SDL2/SDL_mixer.h>

int play_music(char *music_path);

void halt_music();

#endif