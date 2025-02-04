#ifndef ui_h
#define ui_h

#include <stdio.h>
#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
// #include "ui.h"
// #include "menus.h"
#include <time.h>
#include <regex.h>
#include <dirent.h>
#include <pthread.h>
#include <locale.h>
#include <wchar.h>
#include <unistd.h>

#define back_color "#c2eefc"
#define font_color "#046d8f"
#define Firebrick_color "#B22222"
#define Forestgreen_color "#228B22"
#define Dimgray_color "#696969"
#define Gold_color "#FFD700"
#define Darkorage_color "#FF8C00"
#define Black_black "#000000"
#define COLOR_FIREBRICK 8
#define COLOR_FORESTGREEN 9
#define COLOR_DIMGRAY 10
#define COLOR_GOLD 11
#define COLOR_DARKORANGE 12
#define BLACK_BLACK 13

// Initial defines
int directions[9][2] = {
    {0, -1}, //0
    {1, -1}, //1
    {1, 0},  //2
    {1, 1},  //3
    {0, 1},  //4
    {-1, 1}, //5
    {-1, 0}, //6
    {-1, -1}, //7
    {0, 0} //8
};


// Initial structs
typedef struct Music{
    char music_path[200];
    bool is_playing;
    bool loop;
} Music; 

typedef struct Location{
    int x, y;
} Location;

// Enums
enum Difficulty {
    Easy = 4,
    Normal = 3,
    Hard = 2
};

typedef enum Room_Type {
    General,
    Treasure,
    Enchant,
    Nightmare
} Room_Type;

typedef enum Food_Type {
    Ordinary,
    Excellent, 
    Magical, 
    Toxic
} Food_Type;

typedef enum Gold_Type {
    Regular = 1,
    Black = 10
} Gold_Type;

typedef enum Gun_Type {
    Mace, 
    Dagger,
    Magic_Wand,
    Normal_Arrow,
    Sword
} Gun_Type;

// Mace = 0x2696, 
// Dagger = 0x1F5E1,
// Magic_Wand = 0x1FA84,
// Normal_Arrow = 0x27B3,
// Sword = 0x2694

typedef enum Spell_Type {
    Health, 
    Speed,
    Damage
} Spell_Type;

typedef enum Enemy_Type {
    Deamon,
    /*Fire Breathing*/Monster,
    Giant,
    Snake,
    Undeed,
} Enemy_Type;


// Room contents
typedef struct Normal_Door {
    Location location;


} Normal_Door;

typedef struct Trap {
    Location location;
    wchar_t unicode;
    
    // go to war room
    // win to go back to current room

} Trap;

typedef struct Staircase {
    Location location;
    wchar_t unicode;

} Staircase;

typedef struct Secret_Door {
    Location location;
    wchar_t unicode;

} Secret_Door;

typedef struct Locked_Door {
    Location location;
    int password;
    bool is_visited;
    int password_turn;
    // show when locked with RED, when open with GREEN 
    // 

} Locked_Door;

typedef struct Master_Key {
    Location location;
    wchar_t unicode;

} Master_Key;


// Player contents
typedef struct Food {
    Location location;
    Food_Type type;
    wchar_t unicode;
    int time_passed;

} Food;

typedef struct Gold {
    Location location;
    Gold_Type type;
    wchar_t unicode;
    
} Gold;

typedef struct Gun {
    char name[200];
    Gun_Type type;
    Location location;
    wchar_t unicode;
    int damage;
    int counter;
    int distance;

} Gun;

typedef struct Spell {
    Location location;
    Spell_Type type;
    wchar_t unicode;

} Spell;

typedef struct Enemy {
    Location location;
    Enemy_Type type;
    int damage;
    int health;
    int following;
    bool is_moving;
    char chr;
    wchar_t unicode;

} Enemy;


// Game places
typedef struct Room {
    Location start;
    Location size;
    Room_Type type;
    
    Staircase *staircase;
    Master_Key *master_key;
    Locked_Door *locked_door;
    
    Secret_Door *secret_doors;
    int secret_doors_num;

    Trap **traps;
    int trap_num;

    Food **foods;
    int food_num;
    
    Gold **golds;
    int gold_num;
    
    Gun **guns;
    int gun_num;
    
    Spell **spells;
    int spell_num;

    Enemy **enemies;
    int enemy_num;

    Normal_Door *normal_doors;
    int normal_doors_num;

} Room;


typedef struct Floor {
    bool floor_visit;
    char map[40][146];
    bool visit[40][146];
    int room_num;
    int has_gold;
    int staircase_num;
    int master_key_num;
    Room *Rooms;

} Floor;


typedef struct Game{
    char name[100];

    Floor *floors;
    int floor_num;

    int Health;
    Food ***foods;
    int *food_num;

    Gun **gun;
    Gun *current_gun;

    Spell ***spell;
    int *spell_num;
    
    Master_Key **master_key;
    int master_key_num;

    wchar_t player_unicode;
    int total_gold;
    Music *music;
    enum Difficulty game_difficulty;
    int player_color;

    Location player_location;
    int player_floor;
    int player_room;

    int time_power;
    bool power_up;
    bool speed_up;
} Game;

typedef struct {
    char file_path[200];
    char username[100];
    char password[100];
    char email[100];
    int total_score;
    int total_gold;
    int num_finished;
    time_t time_experience;
    enum Difficulty game_difficulty;
    int color;
    Music *music;

    Game *game;

} Player;

Player *player;

char *ROGUE[] = {
    {"███████╗ ██████╗  ██████╗ ██╗   ██╗███████╗"},
    {"██╔══██║██╔═══██╗██╔════╝ ██║   ██║██╔════╝"},
    {"███████╝██║   ██║██║  ███╗██║   ██║█████╗  "},
    {"██╔═╗██║██║   ██║██║   ██║██║   ██║██╔══╝  "},
    {"██║ ║██║╚██████╔╝╚██████╔╝ ╚█████╝ ███████╗"},
    {"╚═╝ ╚═╝  ╚═════╝  ╚═════╝   ╚═══╝  ╚══════╝"},
};

char *GAME[] = {
    {" ██████╗  █████╗ ███╗   ███╗███████╗"},
    {"██╔════╝ ██╔══██╗████╗ ████║██╔════╝"},
    {"██║  ███╗███████║██╔████╔██║█████╗  "},
    {"██║   ██║██╔══██║██║╚██╔╝██║██╔══╝  "},
    {"╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗"},
    {" ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝"},
};

char *BOX[] = {
    {"╔══════════════════════════════╗"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"╚══════════════════════════════╝"}
};

char *BOX2[] = {
    {"╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝"}
};

char *BOX3[] = {
    {"╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝"}
};


/*



╔═════════════════════════╗
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
╚═════════════════════════╝ 

*/


/*

char *ROGUE[] = {
    {"#######╗ ######╗  ######╗ ##╗   ##╗#######╗\0"},
    {"##╔══##║##╔═══##╗##╔════╝ ##║   ##║##╔════╝\0"},
    {"#######╝##║   ##║##║  ###╗##║   ##║#####╗  \0"},
    {"##╔═╗##║##║   ##║##║   ##║##║   ##║##╔══╝  \0"},
    {"##║ ║##║╚######╔╝╚######╔╝ ╚#####╝ #######╗\0"},
    {"╚═╝ ╚═╝  ╚═════╝  ╚═════╝   ╚═══╝  ╚══════╝\0"},
};

char *GAME[] = {
    {" ######╗  #####╗ ###╗   ###╗#######╗\0"},
    {"##╔════╝ ##╔══##╗####╗ ####║##╔════╝\0"},
    {"##║  ###╗#######║##╔####╔##║#####╗  \0"},
    {"##║   ##║##╔══##║##║╚##╔╝##║##╔══╝  \0"},
    {"╚######╔╝##║  ##║##║ ╚═╝ ##║#######╗\0"},
    {" ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝\0"},
};

*/

// char ROGUE[6][45] = {
//     {"#######| ######|  ######|  ##|  ##| ######|"},
//     {"##|  ##| ##|  ##| ##|      ##|  ##| ##|    "},
//     {"#######| ##|  ##| ##| ###| ##|  ##| #####| "},
//     {"##|  ##| ##|  ##| ##|  ##| ##|  ##| ##|    "},
//     {"##|  ##| ######|  ######|   #####|  ######|"},
//     {"##|  ##| ######|   ####|     ###|   ######|"},
// };

// char GAME[6][38] = {
//     {"######|  ######| ###|  ###| #######|"},
//     {"##|      ##  ##| ####|####| ##|     "},
//     {"##| ###| ######| ##|######| #####|  "},
//     {"##|  ##| ##| ##| ##| ## ##| ##|     "},
//     {"######|  ##| ##| ##|    ##| #######|"},
//     {"######|  ##| ##| ##|    ##| #######|"},
// };

char HAPPY_FACE[6][15] = {
    {"   ______   "},
    {" /  _   _ \\ "},
    {"|   .   .  |"},
    {"|     |    |"},
    {"|   \\___/  |"},
    {" \\________/ "}
};

char SAD_FACE[6][14] = {
    {"   ______   "},
    {" /  _   _ \\ "},
    {"|   .   .  |"},
    {"|     |    |"},
    {"|   _____  |"},
    {" \\________/ "}
};


// char ROGUE[6][45] = {
//     {" ____     _____   _______   _   _    _____ "},
//     {"|  _ \\   |  _  | |  ____|  | | | |  |  ___|"},
//     {"| |_| |  | | | | | | ___   | | | |  | |__  "},
//     {"|  _ /   | | | | | | |_ |  | | | |  |  __| "},
//     {"| | \\ \\  | |_| | | |__| |  | |_| |  | |___ "},
//     {"|_|  \\_\\ |_____| |______/  |_____|  |_____|"},
// };

// char GAME[6][38] = {
//     {" _______  _____   _    _   _____  "},
//     {"|  ____| |  _  | | |  | | |  ___| "},
//     {"| | ___  | |_| | | \\  / | | |__   "},
//     {"| | |_ | |  _  | | |\\/| | |  __|  "},
//     {"| |__| | | | | | | |  | | | |___  "},
//     {"|______/ |_| |_| |_|  |_| |_____|  "},
// };


void hx(const char* hex, short* r, short* g, short* b) {
    int red, green, blue;
    sscanf(hex, "#%02x%02x%02x", &red, &green, &blue);

    *r = (short)((red / 255.0) * 1000);
    *g = (short)((green / 255.0) * 1000);
    *b = (short)((blue / 255.0) * 1000);
}

void clear_space() {
    
    clear();
    // Page UI
    start_color();
    bkgd(COLOR_PAIR(1));

    short r, g, b;
    hx(back_color, &r, &g, &b);
    init_color(COLOR_BLACK, r, g, b);
    short rr, gg, bb;
    hx(font_color, &rr, &gg, &bb);
    init_color(COLOR_BLUE, rr, gg, bb);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    init_pair(3, COLOR_RED, COLOR_BLUE);

    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);

    hx(Firebrick_color, &r, &g, &b);
    init_color(COLOR_FIREBRICK, r, g, b);
    init_pair(8, COLOR_FIREBRICK, COLOR_BLACK);
    
    hx(Forestgreen_color, &r, &g, &b);
    init_color(COLOR_FORESTGREEN, r, g, b);
    init_pair(9, COLOR_FORESTGREEN, COLOR_BLACK);
    
    hx(Dimgray_color, &r, &g, &b);
    init_color(COLOR_DIMGRAY, r, g, b);
    init_pair(10, COLOR_DIMGRAY, COLOR_BLACK);
    
    hx(Gold_color, &r, &g, &b);
    init_color(COLOR_GOLD, r, g, b);
    init_pair(11, COLOR_GOLD, COLOR_BLACK);
    
    hx(Darkorage_color, &r, &g, &b);
    init_color(COLOR_DARKORANGE, r, g, b);
    init_pair(12, COLOR_DARKORANGE, COLOR_BLACK);
    
    init_pair(13, COLOR_RED, COLOR_BLACK);
    init_pair(14, COLOR_GREEN, COLOR_BLACK);
    init_pair(15, COLOR_RED, COLOR_BLUE);
    init_pair(16, COLOR_GREEN, COLOR_BLUE);
    init_pair(17, COLOR_DARKORANGE, COLOR_BLUE);
    init_pair(18, COLOR_GOLD, COLOR_BLUE);

    hx(Black_black, &r, &g, &b);
    init_color(BLACK_BLACK, r, g, b);
    init_pair(19, BLACK_BLACK, COLOR_BLACK);
    init_pair(20, COLOR_BLACK, COLOR_FORESTGREEN);
    init_pair(21, COLOR_BLACK, COLOR_DARKORANGE);
    init_pair(22, COLOR_BLACK, BLACK_BLACK);
    /*
#define FIREBRICK 8
#define FORESTGREEN 9
#define DIMGRAY 10
#define GOLD 11
#define DARKORANGE 12
    
    */
    attron(COLOR_PAIR(2));
    for(int i = 0; i < 148; i++) {
        move(0, i);
        addch(' ');
    }
    for(int i = 0; i < 148; i++) {
        move(41, i);
        addch(' ');
    }
    for(int j = 0; j < 42; j++) {
        move(j, 0);
        addch(' ');
    }
    for(int j = 0; j < 42; j++) {
        move(j, 147);
        addch(' ');
    }
    attroff(COLOR_PAIR(2));
}

void clear_space2() {

    short r, g, b;
    hx(back_color, &r, &g, &b);
    init_color(COLOR_BLACK, r, g, b);
    short rr, gg, bb;
    hx(font_color, &rr, &gg, &bb);
    init_color(COLOR_BLUE, rr, gg, bb);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    init_pair(3, COLOR_RED, COLOR_BLUE);

    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);


    hx(Firebrick_color, &r, &g, &b);
    init_color(COLOR_FIREBRICK, r, g, b);
    init_pair(8, COLOR_FIREBRICK, COLOR_BLACK);
    
    hx(Forestgreen_color, &r, &g, &b);
    init_color(COLOR_FORESTGREEN, r, g, b);
    init_pair(9, COLOR_FORESTGREEN, COLOR_BLACK);
    
    hx(Dimgray_color, &r, &g, &b);
    init_color(COLOR_DIMGRAY, r, g, b);
    init_pair(10, COLOR_DIMGRAY, COLOR_BLACK);
    
    hx(Gold_color, &r, &g, &b);
    init_color(COLOR_GOLD, r, g, b);
    init_pair(11, COLOR_GOLD, COLOR_BLACK);
    
    hx(Darkorage_color, &r, &g, &b);
    init_color(COLOR_DARKORANGE, r, g, b);
    init_pair(12, COLOR_DARKORANGE, COLOR_BLACK);

    init_pair(13, COLOR_RED, COLOR_BLACK);
    init_pair(14, COLOR_GREEN, COLOR_BLACK);
    init_pair(15, COLOR_RED, COLOR_BLUE);
    init_pair(16, COLOR_GREEN, COLOR_BLUE);
    init_pair(17, COLOR_DARKORANGE, COLOR_BLUE);
    init_pair(18, COLOR_GOLD, COLOR_BLUE);

    hx(Black_black, &r, &g, &b);
    init_color(BLACK_BLACK, r, g, b);
    init_pair(19, BLACK_BLACK, COLOR_BLACK);
    init_pair(20, COLOR_BLACK, COLOR_FORESTGREEN);
}



#endif