#ifndef player_h
#define player_h

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

void new_game();
void old_game();
void score_table();
void settings();

typedef struct Game {
    char map[4][40][146];
    int visit[4][40][146];
    //  music
    //  player color
    //  game hard rate
    

} Game;

typedef struct Player {
    char file_path[200];
    char username[100];
    char password[100];
    char email[100];
    int total_score;
    int total_gold;
    int num_finished;
    time_t time_experience;


    Game *old_game;
    Game *current_game;


    

} Player;

Player *player;

void new_game() {

}
void old_game() {

}
void score_table() {

}
void settings() {

}

#endif palyer_h