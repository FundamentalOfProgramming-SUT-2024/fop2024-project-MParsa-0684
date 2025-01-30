#ifndef feature_h
#define feature_h

#include <stdio.h>
#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ui.h"
#include "menus.h"
#include <time.h>
#include <regex.h>
#include <dirent.h>
#include <pthread.h>
#include <locale.h>
#include <wchar.h>
#include <unistd.h>

// Initial defines
int directions[8][2] = {
    {0, -1}, //0
    {1, -1}, //1
    {1, 0},  //2
    {1, 1},  //3
    {0, 1},  //4
    {-1, 1}, //5
    {-1, 0}, //6
    {-1, -1} //7
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
    bool is_visited;

} Secret_Door;

typedef struct Locked_Door {
    Location location;
    int password;
    wchar_t unicode;
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
    Room *Rooms;
    int room_num;
    int has_gold;
    int staircase_num;
    int master_key_num;

} Floor;


typedef struct Game{
    char name[100];
    Floor *floors;
    Location player_location;
    int player_floor;
    int player_room;

    int floor_num;
    int Health;
    Food ***foods;
    Gun **gun;
    Spell ***spell;
    int *food_num;
    int *spell_num;
    Gun *current_gun;
    int total_gold;
    Master_Key **master_key;
    int master_key_num;

    Music *music;
    enum Difficulty game_difficulty;
    int player_color;
    wchar_t player_unicode;

    int time_power;
    bool power_up;
    bool speed_up;
} Game;

void action_game(Game *game, int dir, int time_passed);
void whole_game(Game *game, Floor *floor, bool is_unicode);
bool is_in_map(int y, int x);
void search_map(Game *game);
void foods_menu(Game *game, int time_passed);
void guns_menu(Game *game);
void spells_menu(Game *game, int time_passed);
void password_generator(Game *game, Locked_Door *locked_door, int dir);
void game_lost(Game *game);
void hit_enemy(Game *game);

void action_game(Game *game, int dir, int time_passed) {
    Room *temp_room;
    char *str;
    Food_Type ft;
    Gold_Type gt;
    Gun_Type ut;
    Spell_Type st;
    int ctr = 0;
            game->floors[game->player_floor].visit[game->player_location.y][game->player_location.x] = true;
            switch(game->floors[game->player_floor].map[game->player_location.y][game->player_location.x]) {
                // Taking food
                case 'F':
                    for(int ii = 0; ii < game->floors[game->player_floor].Rooms[game->player_room].food_num; ii++) {
                        if(game->floors[game->player_floor].Rooms[game->player_room].foods[ii] != NULL && 
                        game->floors[game->player_floor].Rooms[game->player_room].foods[ii]->location.y == game->player_location.y && 
                        game->floors[game->player_floor].Rooms[game->player_room].foods[ii]->location.x == game->player_location.x) {
                            game->food_num[game->floors[game->player_floor].Rooms[game->player_room].foods[ii]->type - Ordinary]++;
                            ft = game->floors[game->player_floor].Rooms[game->player_room].foods[ii]->type;
                            game->foods[game->floors[game->player_floor].Rooms[game->player_room].foods[ii]->type - Ordinary] = (Food **) realloc(game->foods[game->floors[game->player_floor].Rooms[game->player_room].foods[ii]->type - Ordinary], game->food_num[game->floors[game->player_floor].Rooms[game->player_room].foods[ii]->type] * sizeof(Food *));
                            game->foods[game->floors[game->player_floor].Rooms[game->player_room].foods[ii]->type - Ordinary][game->food_num[game->floors[game->player_floor].Rooms[game->player_room].foods[ii]->type - Ordinary] - 1] = game->floors[game->player_floor].Rooms[game->player_room].foods[ii];
                            game->floors[game->player_floor].map[game->floors[game->player_floor].Rooms[game->player_room].foods[ii]->location.y][game->floors[game->player_floor].Rooms[game->player_room].foods[ii]->location.x] = '.';
                            game->foods[game->floors[game->player_floor].Rooms[game->player_room].foods[ii]->type - Ordinary][game->food_num[game->floors[game->player_floor].Rooms[game->player_room].foods[ii]->type - Ordinary] - 1]->time_passed = time_passed;
                            game->floors[game->player_floor].Rooms[game->player_room].foods[ii] = NULL;
                        }
                    }

                    move(0, 1);
                    attron(COLOR_PAIR(16) | A_BOLD);
                    str = (char *) malloc(1000 * sizeof(char));
                    strcpy(str, "You've taken a FOOD with type {");
                    switch(ft) {
                        case Ordinary:
                            strcat(str, "Ordinary");
                            break;
                        case Excellent:
                            strcat(str, "Excellent");
                            break;
                        case Magical:
                            strcat(str, "Magical");
                            break;
                        case Toxic:
                            strcat(str, "Toxic");
                            break;

                    }
                    strcat(str, "}, Press any key to continue...");
                    addstr(str);
                    getch();
                    for(int i = 0; i < 146; i++) {
                        move(0, i);
                        addch(' ');
                    }    
                    attroff(COLOR_PAIR(16) | A_BOLD);
                    free(str);
                    break;

                // Taking gold
                case 'G':
                    str = (char *) malloc(1000 * sizeof(char));
                    strcpy(str, "You've taken a GOLD with type {");
                    for(int ii = 0; ii < game->floors[game->player_floor].Rooms[game->player_room].gold_num; ii++) {
                        if(game->floors[game->player_floor].Rooms[game->player_room].golds[ii] != NULL && 
                        game->floors[game->player_floor].Rooms[game->player_room].golds[ii]->location.y == game->player_location.y && 
                        game->floors[game->player_floor].Rooms[game->player_room].golds[ii]->location.x == game->player_location.x) {
                            gt = game->floors[game->player_floor].Rooms[game->player_room].golds[ii]->type;
                            switch(gt) {
                                case Regular:
                                    strcat(str, "Regular}! You're Total Golds increased by 1! Press any key to continue...");
                                    game->total_gold += gt;
                                    break;
                                case Black:
                                    strcat(str, "Black}! You're Total Golds increased by 10! Press any key to continue...");
                                    game->total_gold += gt;
                                    break;
                            }
                            game->floors[game->player_floor].map[game->floors[game->player_floor].Rooms[game->player_room].golds[ii]->location.y][game->floors[game->player_floor].Rooms[game->player_room].golds[ii]->location.x] = '.';
                            game->floors[game->player_floor].Rooms[game->player_room].golds[ii] = NULL;
                        }
                    }

                    move(0, 1);
                    attron(COLOR_PAIR(18) | A_BOLD);
                    addstr(str);
                    getch();
                    for(int i = 0; i < 146; i++) {
                        move(0, i);
                        addch(' ');
                    }    
                    attroff(COLOR_PAIR(18) | A_BOLD);
                    free(str);
                    break;

                // Taking gun
                case 'U':
                    for(int ii = 0; ii < game->floors[game->player_floor].Rooms[game->player_room].gun_num; ii++) {
                        if(game->floors[game->player_floor].Rooms[game->player_room].guns[ii] != NULL && 
                        game->floors[game->player_floor].Rooms[game->player_room].guns[ii]->location.y == game->player_location.y && 
                        game->floors[game->player_floor].Rooms[game->player_room].guns[ii]->location.x == game->player_location.x) {
                            ut = game->floors[game->player_floor].Rooms[game->player_room].guns[ii]->type;
                            if(!(ut == Sword && game->gun[4] != NULL)) {
                                if(game->gun[game->floors[game->player_floor].Rooms[game->player_room].guns[ii]->type - Mace] == NULL) {
                                    game->gun[game->floors[game->player_floor].Rooms[game->player_room].guns[ii]->type - Mace] = game->floors[game->player_floor].Rooms[game->player_room].guns[ii];
                                }
                                else {
                                    game->gun[game->floors[game->player_floor].Rooms[game->player_room].guns[ii]->type - Mace]->counter += game->floors[game->player_floor].Rooms[game->player_room].guns[ii]->counter;
                                }
                            }
                            ctr = game->floors[game->player_floor].Rooms[game->player_room].guns[ii]->counter;
                            game->floors[game->player_floor].map[game->floors[game->player_floor].Rooms[game->player_room].guns[ii]->location.y][game->floors[game->player_floor].Rooms[game->player_room].guns[ii]->location.x] = '.';
                            game->floors[game->player_floor].Rooms[game->player_room].guns[ii] = NULL;
                        }
                    }

                    move(0, 1);
                    attron(COLOR_PAIR(3) | A_BOLD);
                    str = (char *) malloc(1000 * sizeof(char));
                    strcpy(str, "You've taken ");
                    char *name = (char *) calloc(1000, sizeof(char));
                    switch(ut) {
                        case Mace:
                            break;
                        case Dagger:
                            strcpy(name, "Dagger");
                            break;
                        case Magic_Wand:
                            strcpy(name, "Magic_Wand");
                            break;
                        case Normal_Arrow:
                            strcpy(name, "Normal_Arrow");
                            break;
                        case Sword:
                            strcpy(name, "Sword");
                            break;

                    }

                    sprintf(str + strlen(str), "%d GUN(s) with type {%s}, Press any key to continue...", ctr, name);
                    if(!(ut == Sword && game->gun[4] != NULL)) {
                        addstr(str);
                        getch();
                    }
                    for(int i = 0; i < 146; i++) {
                        move(0, i);
                        addch(' ');
                    }    
                    attroff(COLOR_PAIR(3) | A_BOLD);
                    free(str);
                    break;
                
                // Taking spell
                case 'S':
                    for(int ii = 0; ii < game->floors[game->player_floor].Rooms[game->player_room].spell_num; ii++) {
                        if(game->floors[game->player_floor].Rooms[game->player_room].spells[ii] != NULL && 
                        game->floors[game->player_floor].Rooms[game->player_room].spells[ii]->location.y == game->player_location.y && 
                        game->floors[game->player_floor].Rooms[game->player_room].spells[ii]->location.x == game->player_location.x) {
                            game->spell_num[game->floors[game->player_floor].Rooms[game->player_room].spells[ii]->type - Health]++;
                            st = game->floors[game->player_floor].Rooms[game->player_room].spells[ii]->type;
                            game->spell[game->floors[game->player_floor].Rooms[game->player_room].spells[ii]->type - Health] = (Spell **) realloc(game->spell[game->floors[game->player_floor].Rooms[game->player_room].spells[ii]->type - Health], game->spell_num[game->floors[game->player_floor].Rooms[game->player_room].spells[ii]->type] * sizeof(Spell *));
                            game->spell[game->floors[game->player_floor].Rooms[game->player_room].spells[ii]->type - Health][game->spell_num[game->floors[game->player_floor].Rooms[game->player_room].spells[ii]->type - Health] - 1] = game->floors[game->player_floor].Rooms[game->player_room].spells[ii];
                            game->floors[game->player_floor].map[game->floors[game->player_floor].Rooms[game->player_room].spells[ii]->location.y][game->floors[game->player_floor].Rooms[game->player_room].spells[ii]->location.x] = '.';
                            game->floors[game->player_floor].Rooms[game->player_room].spells[ii] = NULL;
                        }
                    }

                    move(0, 1);
                    attron(COLOR_PAIR(17) | A_BOLD);
                    str = (char *) malloc(1000 * sizeof(char));
                    strcpy(str, "You've taken a SPELL with type {");
                    switch(st) {
                        case Health:
                            strcat(str, "Health");
                            break;
                        case Speed:
                            strcat(str, "Speed");
                            break;
                        case Damage:
                            strcat(str, "Damage");
                            break;

                    }
                    strcat(str, "}, Press any key to continue...");
                    addstr(str);
                    getch();
                    for(int i = 0; i < 146; i++) {
                        move(0, i);
                        addch(' ');
                    }    
                    attroff(COLOR_PAIR(17) | A_BOLD);
                    free(str);
                    break;
                default:

                    break;
            }
}

void whole_game(Game *game, Floor *floor, bool is_unicode) {
    WINDOW *whole_map = newwin(40, 146, 1, 1);
    for(int i = 0; i < 40; i++) {
        for(int j = 0; j < 146; j++) {
            int color_num;
            if(true) {
                bool flag = false;
                char index = floor->map[i][j];
                if(index== '#') {
                    wattron(whole_map, COLOR_PAIR(1));
                    color_num = 1;
                    flag = true;
                }
                if(index == '@') {
                    for(int i = 0; i < 6; i++) {
                        if(floor->Rooms[i].locked_door != NULL) {
                            if(floor->Rooms[i].locked_door->is_visited == true) {
                                wattron(whole_map, COLOR_PAIR(14));
                                color_num = 14;
                            }
                            else {
                                wattron(whole_map, COLOR_PAIR(13));
                                color_num = 13;
                            }
                            break;
                        }
                    }
                    flag = true;
                }
                if(!flag && index == 'd' || index == 'f' || index == 'g' || index == 's' || index == 'u') {
                    wattron(whole_map, COLOR_PAIR(19) | A_BOLD);
                    color_num = 19;
                    flag = true;
                }

                for(int ii = 0; ii < 6 && !flag; ii++) {
                    if(i >= floor->Rooms[ii].start.y && j >= floor->Rooms[ii].start.x && i < (floor->Rooms[ii].start.y + floor->Rooms[ii].size.y) && j < (floor->Rooms[ii].start.x + floor->Rooms[ii].size.x)) {
                        switch(floor->Rooms[ii].type) {
                            case General:
                                wattron(whole_map, COLOR_PAIR(1));
                                color_num = 1;
                                break;
                            case Treasure:
                                wattron(whole_map, COLOR_PAIR(11));
                                color_num = 11;
                                break;
                            case Enchant:
                                wattron(whole_map, COLOR_PAIR(9));
                                color_num = 9;
                                break;
                            case Nightmare:
                                wattron(whole_map, COLOR_PAIR(12));
                                color_num = 12;
                                break;
                        }
                        flag = true;
                    } 
                }

                if(index == '^')
                    index = '.';
                if(index == '*') {
                    if((i < 39 && floor->map[i + 1][j] == '#') || (i > 0 && floor->map[i - 1][j] == '#'))
                        index = '_';
                    else
                        index = '|';
                }

                mvwaddch(whole_map, i, j, index);
                wrefresh(whole_map);
                wattroff(whole_map, COLOR_PAIR(color_num));
                if(index == 'd' || index == 'f' || index == 'g' || index == 's' || index == 'u')
                    wattroff(whole_map, A_BOLD);
            
            }
        }
    }

    wattron(whole_map, COLOR_PAIR(game->player_color) | A_BOLD);
    mvwaddch(whole_map, game->player_location.y, game->player_location.x, 'P');
    wrefresh(whole_map);
    wattroff(whole_map, COLOR_PAIR(game->player_color) | A_BOLD);

    if(is_unicode == true) {
            //food
            for(int j = 0; j < 6; j++) {
            for(int i = 0; i < game->floors[game->player_floor].Rooms[j].food_num; i++) {
                if(game->floors[game->player_floor].Rooms[j].foods[i] != NULL)
                    mvwprintw(whole_map, game->floors[game->player_floor].Rooms[j].foods[i]->location.y, game->floors[game->player_floor].Rooms[j].foods[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[j].foods[i]->unicode);
            }

            //gold
            for(int i = 0; i < game->floors[game->player_floor].Rooms[j].gold_num; i++) {
                if(game->floors[game->player_floor].Rooms[j].golds[i] != NULL)
                    mvwprintw(whole_map, game->floors[game->player_floor].Rooms[j].golds[i]->location.y, game->floors[game->player_floor].Rooms[j].golds[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[j].golds[i]->unicode);
            }

            //gun
            for(int i = 0; i < game->floors[game->player_floor].Rooms[j].gun_num; i++) {
                if(game->floors[game->player_floor].Rooms[j].guns[i] != NULL)
                    mvwprintw(whole_map, game->floors[game->player_floor].Rooms[j].guns[i]->location.y, game->floors[game->player_floor].Rooms[j].guns[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[j].guns[i]->unicode);
            }

            //spell
            for(int i = 0; i < game->floors[game->player_floor].Rooms[game->player_room].spell_num; i++) {
                if(game->floors[game->player_floor].Rooms[game->player_room].spells[i] != NULL)
                    mvwprintw(whole_map, game->floors[game->player_floor].Rooms[game->player_room].spells[i]->location.y, game->floors[game->player_floor].Rooms[game->player_room].spells[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[game->player_room].spells[i]->unicode);
            }
        
            //spell
            for(int i = 0; i < game->floors[game->player_floor].Rooms[game->player_room].spell_num; i++) {
                if(game->floors[game->player_floor].Rooms[game->player_room].spells[i] != NULL)
                    mvwprintw(whole_map, game->floors[game->player_floor].Rooms[game->player_room].spells[i]->location.y, game->floors[game->player_floor].Rooms[game->player_room].spells[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[game->player_room].spells[i]->unicode);
            }

            //enemy
            for(int i = 0; i < game->floors[game->player_floor].Rooms[game->player_room].enemy_num; i++) {
                if(game->floors[game->player_floor].Rooms[game->player_room].enemies[i] != NULL)
                    mvwprintw(whole_map, game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.y, game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->unicode);
            }

            // master key
            if(game->floors[game->player_floor].Rooms[j].master_key != NULL)
                mvwprintw(whole_map, game->floors[game->player_floor].Rooms[j].master_key->location.y, game->floors[game->player_floor].Rooms[j].master_key->location.x, "%lc", game->floors[game->player_floor].Rooms[j].master_key->unicode);
            
            mvwprintw(whole_map, game->player_location.y, game->player_location.x, "%lc", game->player_unicode);
            wrefresh(whole_map);
        }
    }

    move(0, 1);
    attron(COLOR_PAIR(3) | A_BOLD);
    addstr("PRESS 'm' key to continue game...");

    while(true) {
        int c = getch();
        if(c == 'm')
            break;
    }

    for(int i = 0; i < 146; i++) {
        move(0, i);
        addch(' ');
    }    
    attroff(COLOR_PAIR(3) | A_BOLD);

    delwin(whole_map);
}

bool is_in_map(int y, int x) {
    if(x >= 0 && y >= 0 && x < 146 && y < 40)
        return true;
    else
        return false;
}

void search_map(Game *game) {
    WINDOW *s_window = newwin(3, 3, game->player_location.y, game->player_location.x);
    wattron(s_window, COLOR_PAIR(2));
    for(int ii = 0; ii < 8; ii++) {
        mvwaddch(s_window, 1 + directions[ii][1], 1 + directions[ii][0], game->floors[game->player_floor].map[game->player_location.y + directions[ii][1]][game->player_location.x + directions[ii][0]]);
    }
    wattroff(s_window, COLOR_PAIR(2));

    wattron(s_window, COLOR_PAIR(3) | A_BOLD);
    mvwaddch(s_window, 1, 1, 'P');
    wrefresh(s_window);
    wattroff(s_window, COLOR_PAIR(3) | A_BOLD);
    wrefresh(s_window);

    move(0, 1);
    attron(COLOR_PAIR(3) | A_BOLD);
    addstr("PRESS 's' key to continue game...");
    
    while(true) {
        int c = getch();
        if(c == 's')
            break;
    }

    for(int i = 0; i < 146; i++) {
        move(0, i);
        addch(' ');
    }    
    attroff(COLOR_PAIR(3) | A_BOLD);

    delwin(s_window);
}

void foods_menu(Game *game, int time_passed) {
    WINDOW *food_menu = newwin(40, 146, 1, 1);
    char str[3][100] = {
        "    Ordinary\t\tNo\t\t\tNo\t\t\tYes\t\t",
        "    Excellent\t\tYes\t\t\tNo\t\t\tYes\t\t",
        "    Magical\t\tNo\t\t\tYes\t\t\tYes\t\t"
    };
    for(int i = 0; i < 3; i++) {
        sprintf(str[i] + strlen(str[i]), "%d\t", game->food_num[i]);
    }
    int location = 0;

    bool flag = true;
    while(flag) {
        wclear(food_menu);
        wattron(food_menu, COLOR_PAIR(9) | A_BOLD);
        mvwaddstr(food_menu, 12, 62, "<<<<< FOOD MENU >>>>>");
        mvwaddstr(food_menu, 14, 25, "    Type\t\tPower_up\t\tSpeed_up\t\tHealth\t\tNumber");
        mvwaddstr(food_menu, 15, 25, "------------------------------------------------------------------------------------------------");
        wattron(food_menu, A_BOLD);

        for(int i = 0; i < 3; i++) 
        mvwaddstr(food_menu, 16 + i, 25, str[i]);
        wattroff(food_menu, COLOR_PAIR(9));
        wattron(food_menu, COLOR_PAIR(20));
        mvwaddstr(food_menu, 16 + location, 25, str[location]);
        wattroff(food_menu, COLOR_PAIR(20));
        wrefresh(food_menu);

        int c = getch();
        switch(c) {
            case 'k':
                location = (location + 1) % 3;
                break;
            case 'j':
                location = (location - 1) % 3;
                location = (location + 3) % 3;
                break;
            case 'e':
                delwin(food_menu);
                return;
                break;
            case KEY_ENTER:
                flag = false;
                break;
        }
    }

    char *mas = (char *) malloc(200 * sizeof(char));
    if(game->food_num[location] == 0) {
        strcpy(mas, "There is no FOOD with this Type, Press any key to continue...");
    }
    else {
        Food *food = game->foods[location][game->food_num[location] - 1];
        if((time_passed - food->time_passed) > 180) {
            strcpy(mas, "The Food's Expired time is passed! ");
            if(food->type == Excellent || food->type == Magical) {
                game->Health = 100;
                strcat(mas, "Excellent/Magical Food transforms to Ordinary Food! Your Health is 100%%! Press any key to continue...");
            }
            else {
                game->Health -= 10;
                strcat(mas, "Ordinary Food transforms to Toxic Food! Your Health decreased by 10%%! Press any key to continue...");
            }
        }
        else {
            strcpy(mas, "You used FOOD with Type {");
            switch(food->type) {
                case Ordinary:
                    game->Health = 100;
                    game->power_up = false;
                    game->speed_up = false;
                    game->time_power = -1;
                    strcat(mas, "Ordinary}");
                    break;
                case Excellent:
                    game->Health = 100;
                    game->power_up = true;
                    game->speed_up = false;
                    game->time_power = time_passed;
                    strcat(mas, "Excellent}");
                    break;
                case Magical:
                    game->Health = 100;
                    game->power_up = false;
                    game->speed_up = true;
                    game->time_power = time_passed;
                    strcat(mas, "Magical}");
                    break;
                case Toxic:
                    break;
            }
            strcat(mas, ", Effects of before FOOD (if lasts) will disapear! Press any key to continue...");
        }
        game->foods[location][game->food_num[location] - 1] = NULL;
        game->food_num[location]--;
        free(food);
    }

    move(0, 1);
    attron(COLOR_PAIR(3) | A_BOLD);
    addstr(mas);

    int c = getch();
    for(int i = 0; i < 146; i++) {
        move(0, i);
        addch(' ');
    }    

    attroff(COLOR_PAIR(3) | A_BOLD);
    free(mas);

    delwin(food_menu);


}

void guns_menu(Game *game) {
    WINDOW *gun_menu = newwin(40, 146, 1, 1);
    char str[5][100] = {
        "    Mace\t\t1\t\t\t5\t\t",
        "    Dagger\t\t5\t\t\t12\t\t",
        "    Magic_Wand\t10\t\t\t15\t\t",
        "    Normal_Arrow\t5\t\t\t5\t\t",
        "    Sword\t\t1\t\t\t10\t\t"
    };
    char str2[5][100] = {
        " Close Range ",
        "             ",
        " Long Range  ",
        "             ",
        "             "
    };

    int arr[5] = {16,17,19,20,21};
    

    for(int i = 0; i < 5; i++) {
        int ctr = 0;
        if(game->gun[i] != NULL)
            ctr = game->gun[i]->counter;

        sprintf(str[i] + strlen(str[i]), "%d\t    ", ctr);
    }
    int location = 0;

    bool flag = true;
    while(flag) {
        wclear(gun_menu);
        wattron(gun_menu, COLOR_PAIR(19) | A_BOLD);
        mvwaddstr(gun_menu, 12, 61, "<<<<< GUN MENU >>>>>");
        mvwaddstr(gun_menu, 14, 34, "    Type\t\tDistance\t\tPower\t\tNumber");
        mvwaddstr(gun_menu, 15, 34, "--------------------------------------------------------------------------");
        wattron(gun_menu, A_BOLD);

        for(int i = 0; i < 5; i++) {
            wattron(gun_menu, COLOR_PAIR(22) | A_BOLD);
            mvwaddstr(gun_menu, arr[i], 20, str2[i]);
            wattroff(gun_menu, COLOR_PAIR(22) | A_BOLD);
            wattron(gun_menu, COLOR_PAIR(19));
            mvwaddstr(gun_menu, arr[i], 34, str[i]);
        }
        
        wattroff(gun_menu, COLOR_PAIR(19));
        wattron(gun_menu, COLOR_PAIR(22));
        mvwaddstr(gun_menu, arr[location], 34, str[location]);
        wattroff(gun_menu, COLOR_PAIR(22));
        wrefresh(gun_menu);

        int c = getch();
        switch(c) {
            case 'k':
                location = (location + 1) % 5;
                break;
            case 'j':
                location = (location - 1) % 5;
                location = (location + 5) % 5;
                break;
            case 'i':
                delwin(gun_menu);
                return;
                break;
            case KEY_ENTER:
                flag = false;
                break;
        }
    }

    char *mas = (char *) malloc(200 * sizeof(char));
    if(game->gun[location] == NULL) {
        strcpy(mas, "You have no GUN with this Type, Press any key to continue...");
    }
    else {
        game->current_gun = NULL;
        game->current_gun = game->gun[location];
        strcpy(mas, "You have taken GUN with Type{");
        switch(location) {
            case 0:
                strcat(mas, "Mace}");
                break;

            case 1:
                strcat(mas, "Dagger}");
                break;

            case 2:
                strcat(mas, "Magic_Wand}");
                break;
            
            case 3:
                strcat(mas, "Normal_Arrow}");
                break;

            case 4:
                strcat(mas, "Sword}");
                break;
        }
        sprintf(mas + strlen(mas), "with %d Number of it! Press any key to continue...", game->current_gun->counter);
    }

    move(0, 1);
    attron(COLOR_PAIR(3) | A_BOLD);
    addstr(mas);

    int c = getch();
    for(int i = 0; i < 146; i++) {
        move(0, i);
        addch(' ');
    }    

    attroff(COLOR_PAIR(3) | A_BOLD);
    free(mas);

    delwin(gun_menu);

}

void spells_menu(Game *game, int time_passed) {
    WINDOW *spell_menu = newwin(40, 146, 1, 1);
    char str[3][100] = {
        "    Health\t\tNo\t\t\tNo\t\t\tYes\t\t",
        "    Spead\t\tNo\t\t\tYes\t\t\tNo\t\t",
        "    Damage\t\tYes\t\t\tNo\t\t\tNo\t\t"
    };
    for(int i = 0; i < 3; i++) {
        sprintf(str[i] + strlen(str[i]), "%d\t", game->spell_num[i]);
    }
    int location = 0;

    bool flag = true;
    while(flag) {
        wclear(spell_menu);
        wattron(spell_menu, COLOR_PAIR(12) | A_BOLD);
        mvwaddstr(spell_menu, 12, 60, "<<<<< SPELL MENU >>>>>");
        mvwaddstr(spell_menu, 14, 25, "    Type\t\tPower_up\t\tSpeed_up\t\tHealth\t\tNumber");
        mvwaddstr(spell_menu, 15, 25, "------------------------------------------------------------------------------------------------");
        wattron(spell_menu, A_BOLD);

        for(int i = 0; i < 3; i++) 
        mvwaddstr(spell_menu, 16 + i, 25, str[i]);
        wattroff(spell_menu, COLOR_PAIR(12));
        wattron(spell_menu, COLOR_PAIR(21));
        mvwaddstr(spell_menu, 16 + location, 25, str[location]);
        wattroff(spell_menu, COLOR_PAIR(21));
        wrefresh(spell_menu);

        int c = getch();
        switch(c) {
            case 'k':
                location = (location + 1) % 3;
                break;
            case 'j':
                location = (location - 1) % 3;
                location = (location + 3) % 3;
                break;
            case 'p':
                delwin(spell_menu);
                return;
                break;
            case KEY_ENTER:
                flag = false;
                break;
        }
    }

    char *mas = (char *) malloc(200 * sizeof(char));
    if(game->spell_num[location] == 0) {
        strcpy(mas, "There is no SPELL with this Type, Press any key to continue...");
    }
    else {
        Spell *spell = game->spell[location][game->spell_num[location] - 1];
        strcpy(mas, "You used SPELL with Type {");
        switch(spell->type) {
            case Health:
                game->Health = 100;
                game->power_up = false;
                game->speed_up = false;
                game->time_power = -1;
                strcat(mas, "Health}");
                break;
            case Speed:
                game->power_up = false;
                game->speed_up = true;
                game->time_power = time_passed;
                strcat(mas, "Speed}");
                break;
            case Damage:
                game->power_up = true;
                game->speed_up = false;
                game->time_power = time_passed;
                strcat(mas, "Damage}");
                break;
        }
        strcat(mas, ", Effects of before SPELL (if lasts) will disapear! Press any key to continue...");
        game->spell[location][game->spell_num[location] - 1] = NULL;
        game->spell_num[location]--;
        free(spell);
    }

    move(0, 1);
    attron(COLOR_PAIR(3) | A_BOLD);
    addstr(mas);

    int c = getch();
    for(int i = 0; i < 146; i++) {
        move(0, i);
        addch(' ');
    }    

    attroff(COLOR_PAIR(3) | A_BOLD);
    free(mas);

    delwin(spell_menu);

}

void password_generator(Game *game, Locked_Door *locked_door, int dir) {
    locked_door->password = rand() % 10000;
    

    WINDOW *alert = newwin(1, 72, 0, 0);
    wattron(alert, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(alert, 0, 0," Your Password is {%.4d}; This Password will disapear within 5 Second...", locked_door->password);
    wrefresh(alert);
    sleep(5);

    wattroff(alert, COLOR_PAIR(3) | A_BOLD);
    delwin(alert);
}

void game_lost(Game *game) {
    WINDOW *lost = newwin(40, 146, 1, 1);
    wattron(lost, COLOR_PAIR(8) | A_BOLD);
    wclear(lost);
    for(int ii = 0; ii < 3; ii++) {
        for(int i = 0; i < 6; i++){
            mvwaddstr(lost, 7 + i, 55 + (ii * 14), SAD_FACE[i]);
            mvwaddstr(lost, 23 + i, 55 + (ii * 14), SAD_FACE[i]);
        }
    }


    for(int i = 14; i < 20; i++) {
        mvwaddstr(lost, i, 5, ROGUE[i - 14]);
        mvwaddstr(lost, i, 100, GAME[i - 14]);
    }
    mvwaddstr(lost, 17, 55, "Unfortunatelly, You lost the game!");
    mvwaddstr(lost, 18, 55, "Press any key to return to Player Menu...");    
    wrefresh(lost);

    getch();
    game = NULL;

    wattroff(lost, COLOR_PAIR(8) | A_BOLD);
    delwin(lost);
}

void hit_enemy(Game *game) {
    if(game->player_room == -1) {
        move(0, 1);
        attron(COLOR_PAIR(3) | A_BOLD);
        addstr("You can use your GUN just in Rooms! Press any key to continue...");

        int c = getch();
        for(int i = 0; i < 146; i++) {
            move(0, i);
            addch(' ');
        }    

        attroff(COLOR_PAIR(3) | A_BOLD);
        return;  
    }
    bool flag = false;
    if(game->current_gun->type == Mace || game->current_gun->type == Sword) {
        for(int i = 0; i < 8; i++) {
            int y = game->player_location.y + directions[i][1], x = game->player_location.x + directions[i][0];
            if(y >= 0 && x >= 0 && y < 40 && x < 146 && 
            (game->floors[game->player_floor].map[y][x] == 'd' || 
            game->floors[game->player_floor].map[y][x] == 'f' || 
            game->floors[game->player_floor].map[y][x] == 'g' || 
            game->floors[game->player_floor].map[y][x] == 's' || 
            game->floors[game->player_floor].map[y][x] == 'u')) {
                for(int ii = 0; ii < game->floors[game->player_floor].Rooms[game->player_room].enemy_num; ii++) {
                    if(game->floors[game->player_floor].Rooms[game->player_room].enemies[ii] != NULL && game->floors[game->player_floor].Rooms[game->player_room].enemies[ii]->location.y == y && 
                    game->floors[game->player_floor].Rooms[game->player_room].enemies[ii]->location.x == x) {
                        if(game->current_gun->damage >= game->floors[game->player_floor].Rooms[game->player_room].enemies[ii]->health) {
                                game->floors[game->player_floor].map[y][x] = '.';
                                game->floors[game->player_floor].Rooms[game->player_room].enemies[ii] = NULL;
                        }
                        else 
                            game->floors[game->player_floor].Rooms[game->player_room].enemies[ii]->health -= game->current_gun->damage;
                        flag = true;
                    }
                }
            }
        }
    }
    else {
        move(0, 1);
        attron(COLOR_PAIR(3) | A_BOLD);
        addstr("Please select a Direction to shoot the gun[j/k/h/l/y/u/b/n]...");

        int c = getch();
        for(int i = 0; i < 146; i++) {
            move(0, i);
            addch(' ');
        }    

        attroff(COLOR_PAIR(3) | A_BOLD);        

        int dir = -1;
        switch (c)
        {
        case 'j':
            dir = 0;
            break;
        
        case 'k':
            dir = 4;
            break;
        
        case 'l':
            dir = 2;
            break;
        
        case 'h':
            dir = 6;
            break;

        case 'y':
            dir = 7;
            break;
        
        case 'u':
            dir = 1;
            break;
        
        case 'b':
            dir = 5;
            break;
        
        case 'n':
            dir = 3;
            break;
        
        default:
            break;
        }

        if(dir == -1) {
            move(0, 1);
            attron(COLOR_PAIR(3) | A_BOLD);
            addstr("You've chosen wrong key! Press any key to continue...");

            getch();
            for(int i = 0; i < 146; i++) {
                move(0, i);
                addch(' ');
            }    

            attroff(COLOR_PAIR(3) | A_BOLD); 
            return; 
        }
        else {
            int y = game->player_location.y, x = game->player_location.x;
            for(int iii = 0; iii <= game->current_gun->distance; iii++) {
                if(game->floors[game->player_floor].map[y][x] == '.' ||
                    game->floors[game->player_floor].map[y][x] == 'd' ||
                    game->floors[game->player_floor].map[y][x] == 'f' || 
                    game->floors[game->player_floor].map[y][x] == 'g' || 
                    game->floors[game->player_floor].map[y][x] == 's' || 
                    game->floors[game->player_floor].map[y][x] == 'u') {
                        if(game->floors[game->player_floor].map[y][x] == 'd' ||
                        game->floors[game->player_floor].map[y][x] == 'f' || 
                        game->floors[game->player_floor].map[y][x] == 'g' || 
                        game->floors[game->player_floor].map[y][x] == 's' || 
                        game->floors[game->player_floor].map[y][x] == 'u') {
                            // checked
                            for(int ii = 0; ii < game->floors[game->player_floor].Rooms[game->player_room].enemy_num; ii++) {
                                if(game->floors[game->player_floor].Rooms[game->player_room].enemies[ii] != NULL && 
                                game->floors[game->player_floor].Rooms[game->player_room].enemies[ii]->location.y == y && 
                                game->floors[game->player_floor].Rooms[game->player_room].enemies[ii]->location.x == x) {
                                    if(game->current_gun->damage >= game->floors[game->player_floor].Rooms[game->player_room].enemies[ii]->health) {
                                            game->floors[game->player_floor].map[y][x] = '.';
                                            game->floors[game->player_floor].Rooms[game->player_room].enemies[ii] = NULL;
                                    }
                                    else 
                                        game->floors[game->player_floor].Rooms[game->player_room].enemies[ii]->health -= game->current_gun->damage;
                                    
                                    game->current_gun->counter--;
                                    flag = true;
                                    break;
                                }
                            }
                            break;
                        }
                        else {
                            y += directions[dir][1];
                            x += directions[dir][0];
                        }
                }
                else {
                    if(iii > 0)
                        y -= directions[dir][1];
                        x -= directions[dir][0];
                
                    Room *room = &game->floors[game->player_floor].Rooms[game->player_room];
                    room->gun_num++;
                    room->guns = (Gun **) realloc(room->guns, room->gun_num * sizeof(Gun *));
                    room->guns[room->gun_num - 1] = (Gun *) malloc(sizeof(Gun));
                    room->guns[room->gun_num - 1]->type = game->current_gun->type;
                    room->guns[room->gun_num - 1]->location.y = y;
                    room->guns[room->gun_num - 1]->location.x = x;
                    game->floors[game->player_floor].map[room->guns[room->gun_num - 1]->location.y][room->guns[room->gun_num - 1]->location.x] = 'U';
                    switch(room->guns[room->gun_num - 1]->type) {
                        case Mace:
                            room->guns[room->gun_num - 1]->unicode = 0x00002694;
                            break;
                        case Dagger:
                            room->guns[room->gun_num - 1]->damage = 12;
                            room->guns[room->gun_num - 1]->counter = 1;
                            strcpy(room->guns[room->gun_num - 1]->name, "Dagger");
                            room->guns[room->gun_num - 1]->distance = 5;
                            room->guns[room->gun_num - 1]->unicode = 0x0001F5E1;
                            //unicode
                            break;
                        case Magic_Wand:
                            room->guns[room->gun_num - 1]->damage = 15;
                            room->guns[room->gun_num - 1]->counter = 1;
                            strcpy(room->guns[room->gun_num - 1]->name, "Magic_Wand");
                            room->guns[room->gun_num - 1]->distance = 10;
                            room->guns[room->gun_num - 1]->unicode = 0x0001FA84;
                            //unicode
                            break;
                        case Normal_Arrow:
                            room->guns[room->gun_num - 1]->damage = 5;
                            room->guns[room->gun_num - 1]->counter = 1;
                            strcpy(room->guns[room->gun_num - 1]->name, "Normal_Arrow");
                            room->guns[room->gun_num - 1]->distance = 5;
                            room->guns[room->gun_num - 1]->unicode = 0x0001F3F9;
                            //unicode
                            break;
                        case Sword:
                            room->guns[room->gun_num - 1]->damage = 10;
                            room->guns[room->gun_num - 1]->counter = 1;
                            strcpy(room->guns[room->gun_num - 1]->name, "Sword");
                            room->guns[room->gun_num - 1]->distance = 1;
                            room->guns[room->gun_num - 1]->unicode = 0x0001FA93;
                            //unicode
                            break;
                    }
                    game->current_gun->counter--;
                    break;
                }
            } 
        }
    }
    
    if(flag == true) {
        move(0, 1);
        attron(COLOR_PAIR(3) | A_BOLD);
        addstr("You've hit the Enemy with your GUN, Press any key to continue...");
        getch();
        for(int i = 0; i < 146; i++) {
            move(0, i);
            addch(' ');
        }    
        attroff(COLOR_PAIR(3) | A_BOLD); 
    }

    if(game->current_gun->counter == 0) {
        game->current_gun = game->gun[0];
        for(int i = 0; i < 5; i++) {
            if(game->gun[i] != NULL && game->gun[i]->counter == 0)
                game->gun[i] = NULL;
        }
    }
}

#endif


