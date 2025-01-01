#ifndef player_h
#define player_h

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ui.h"
#include "menus.h"
#include <regex.h>

typedef struct Player {
    char file_path[200];
    char username[100];
    char password[100];
    char email[100];

    

} Player;

Player *player;


#endif palyer_h