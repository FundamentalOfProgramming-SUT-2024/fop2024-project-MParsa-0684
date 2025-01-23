#inlcude<stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

void play_sound(const char *file) {
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Music *music = Mix_LoadMUS(file);
    Mix_PlayMusic(music, -1); // Loop indefinitely
    SDL_Delay(5000); // Keep playing for 5 seconds
    Mix_FreeMusic(music);
    Mix_CloseAudio();
}

int main() {
    
}