#ifndef player_h
#define player_h

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ui.h"

typedef struct Player {
    FILE *player_file;
    char *username;
    char *password;
    char *email;

    

} Player;




#endif palyer_h