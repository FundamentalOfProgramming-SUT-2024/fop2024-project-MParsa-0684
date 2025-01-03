#ifndef game_h
#define game_h

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ui.h"
#include "menus.h"
#include <time.h>
#include <regex.h>
#include <dirent.h>
#include <pthread.h>

enum Difficulty {
    Easy,
    Normal,
    Hard
};

typedef struct {
    char music_path[200];
    bool is_playing;
    bool loop;
} Music;

typedef struct {
    char map[4][40][146];
    int visit[4][40][146];
    //  music
    //  player color
    //  game hard rate
    

} Game;

void create_new_game(Game **game);

void create_new_game(Game **game) {

}


#endif game_h