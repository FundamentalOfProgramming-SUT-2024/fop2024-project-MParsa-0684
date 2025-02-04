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

void action_game(Game *game, int dir, int time_passed);
void whole_game(Game *game, Floor *floor, bool is_unicode);
bool is_in_map(int y, int x);
void search_map(Game *game);
void foods_menu(Game *game, int time_passed);
void guns_menu(Game *game);
void spells_menu(Game *game, int time_passed);
void password_generator(Game *game, Locked_Door *locked_door, int dir);
void game_lost(Game *game);
void hit_enemy(Game *game, int *save_shot, bool is_a);
void game_won(Game *game, Player *player);
void save_game();
void help_page(Game *game);
void delete_enter(char *s);

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
            for(int i = 0; i < game->floors[game->player_floor].Rooms[j].spell_num; i++) {
                if(game->floors[game->player_floor].Rooms[j].spells[i] != NULL)
                    mvwprintw(whole_map, game->floors[game->player_floor].Rooms[j].spells[i]->location.y, game->floors[game->player_floor].Rooms[j].spells[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[j].spells[i]->unicode);
            }
        
            //spell
            for(int i = 0; i < game->floors[game->player_floor].Rooms[j].spell_num; i++) {
                if(game->floors[game->player_floor].Rooms[j].spells[i] != NULL)
                    mvwprintw(whole_map, game->floors[game->player_floor].Rooms[j].spells[i]->location.y, game->floors[game->player_floor].Rooms[j].spells[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[j].spells[i]->unicode);
            }

            //enemy
            for(int i = 0; i < game->floors[game->player_floor].Rooms[j].enemy_num; i++) {
                if(game->floors[game->player_floor].Rooms[j].enemies[i] != NULL)
                    mvwprintw(whole_map, game->floors[game->player_floor].Rooms[j].enemies[i]->location.y, game->floors[game->player_floor].Rooms[j].enemies[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[j].enemies[i]->unicode);
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
        "  Ordinary\t\tNo\t\t\tNo\t\t\tYes\t\t",
        "  Excellent\t\tYes\t\t\tNo\t\t\tYes\t\t",
        "  Magical\t\tNo\t\t\tYes\t\t\tYes\t\t"
    };
    wchar_t wt[3] = {0x0001F34E, 0x0001F370, 0x0001F347};
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

        for(int i = 0; i < 3; i++) {
            mvwprintw(food_menu, 16 + i, 25, "%lc", wt[i]);
            mvwaddstr(food_menu, 16 + i, 27, str[i]);
        }

        wattroff(food_menu, COLOR_PAIR(9));
        wattron(food_menu, COLOR_PAIR(20));
        mvwprintw(food_menu, 16 + location, 25, "%lc", wt[location]);
        mvwaddstr(food_menu, 16 + location, 27, str[location]);
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
        "  Mace\t\t1\t\t\t5\t\t",
        "  Dagger\t\t5\t\t\t12\t\t",
        "  Magic_Wand\t10\t\t\t15\t\t",
        "  Normal_Arrow\t5\t\t\t5\t\t",
        "  Sword\t\t1\t\t\t10\t\t"
    };
    wchar_t wt[5] = {
        0x0001F531,
        0x0001F528,
        0x0001F4DC,
        0x0001F3F9,
        0x0001F5E1
    };

    char str2[5][100] = {
        " Close Range ",
        "             ",
        " Long Range  ",
        "             ",
        "             "
    };

    int arr[5][2] = {
        {16, 0},
        {17, 4},
        {19, 1},
        {20, 2},
        {21, 3}
    };
    

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
            mvwaddstr(gun_menu, arr[i][0], 20, str2[i]);
            wattroff(gun_menu, COLOR_PAIR(22) | A_BOLD);
            mvwprintw(gun_menu, arr[i][0], 34, "%lc", wt[arr[i][1]]);
            wattron(gun_menu, COLOR_PAIR(19));
            mvwaddstr(gun_menu, arr[i][0], 36, str[arr[i][1]]);
        }
        
        wattroff(gun_menu, COLOR_PAIR(19));
        wattron(gun_menu, COLOR_PAIR(22));
        mvwprintw(gun_menu, arr[location][0], 34, "%lc", wt[arr[location][1]]);
        mvwaddstr(gun_menu, arr[location][0], 36, str[arr[location][1]]);
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
    if(game->gun[arr[location][1]] == NULL) {
        strcpy(mas, "You have no GUN with this Type, Press any key to continue...");
    }
    else {
        game->current_gun = NULL;
        game->current_gun = game->gun[arr[location][1]];
        strcpy(mas, "You have taken GUN with Type {");
        switch(arr[location][1]) {
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
        sprintf(mas + strlen(mas), " with %d Number of it! Press any key to continue...", game->current_gun->counter);
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
        "  Health\t\tNo\t\t\tNo\t\t\tYes\t\t",
        "  Spead\t\tNo\t\t\tYes\t\t\tNo\t\t",
        "  Damage\t\tYes\t\t\tNo\t\t\tNo\t\t"
    };
    wchar_t wt[3] = {0x0001F52E, 0x0001F680, 0x0001F4A5};

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

        for(int i = 0; i < 3; i++) {
            mvwprintw(spell_menu, 16 + i, 25, "%lc", wt[i]);
            mvwaddstr(spell_menu, 16 + i, 27, str[i]);
        }
        wattroff(spell_menu, COLOR_PAIR(12));
        wattron(spell_menu, COLOR_PAIR(21));
        mvwprintw(spell_menu, 16 + location, 25, "%lc", wt[location]);
        mvwaddstr(spell_menu, 16 + location, 27, str[location]);
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

void hit_enemy(Game *game, int *save_shot, bool is_a) {
    // if in raods instead of rooms
    if(game->player_room == -1) {
        move(0, 1);
        attron(COLOR_PAIR(3) | A_BOLD);
        addstr("You can use your GUN just in Rooms! Press any key to continue...");

        getch();
        for(int i = 0; i < 146; i++) {
            move(0, i);
            addch(' ');
        }    

        attroff(COLOR_PAIR(3) | A_BOLD);
        return;  
    }
    bool flag = false;
    
    if(game->current_gun->type == Mace || game->current_gun->type == Sword) {
        for(int i = 0; i < 9; i++) {
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
        int dir = -1;
        if(is_a == true) {
            if(save_shot[1] == -1) {
                move(0, 1);
                attron(COLOR_PAIR(3) | A_BOLD);
                addstr("Your before direction was incorrect! It cann't be repeated! Press any key to continue...");

                getch();
                for(int i = 0; i < 146; i++) {
                    move(0, i);
                    addch(' ');
                }    

                attroff(COLOR_PAIR(3) | A_BOLD); 
                return;       
            }
            else 
                dir = save_shot[1];
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
        }

        bool where = false;
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
            save_shot[1] = dir;
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
                    where = true;
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
                            room->guns[room->gun_num - 1]->unicode = 0x0001F531;
                            break;
                        case Dagger:
                            room->guns[room->gun_num - 1]->damage = 12;
                            room->guns[room->gun_num - 1]->counter = 1;
                            strcpy(room->guns[room->gun_num - 1]->name, "Dagger");
                            room->guns[room->gun_num - 1]->distance = 5;
                            room->guns[room->gun_num - 1]->unicode = 0x0001F528;
                            //unicode
                            break;
                        case Magic_Wand:
                            room->guns[room->gun_num - 1]->damage = 15;
                            room->guns[room->gun_num - 1]->counter = 1;
                            strcpy(room->guns[room->gun_num - 1]->name, "Magic_Wand");
                            room->guns[room->gun_num - 1]->distance = 10;
                            room->guns[room->gun_num - 1]->unicode = 0x0001F4DC;
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
                            room->guns[room->gun_num - 1]->unicode = 0x0001F5E1;
                            //unicode
                            break;
                    }
                    game->current_gun->counter--;
                    break;
                }
            } 

            if(flag == false && where == false) {
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
                        room->guns[room->gun_num - 1]->unicode = 0x0001F531;
                        break;
                    case Dagger:
                        room->guns[room->gun_num - 1]->damage = 12;
                        room->guns[room->gun_num - 1]->counter = 1;
                        strcpy(room->guns[room->gun_num - 1]->name, "Dagger");
                        room->guns[room->gun_num - 1]->distance = 5;
                        room->guns[room->gun_num - 1]->unicode = 0x0001F528;
                        //unicode
                        break;
                    case Magic_Wand:
                        room->guns[room->gun_num - 1]->damage = 15;
                        room->guns[room->gun_num - 1]->counter = 1;
                        strcpy(room->guns[room->gun_num - 1]->name, "Magic_Wand");
                        room->guns[room->gun_num - 1]->distance = 10;
                        room->guns[room->gun_num - 1]->unicode = 0x0001F4DC;
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
                        room->guns[room->gun_num - 1]->unicode = 0x0001F5E1;
                        //unicode
                        break;
                }
                game->current_gun->counter--;
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

void refresh_enemy_following(Game *game) {
    if(game->player_room != -1) {
        for(int i = 0; i < game->floors[game->player_floor].Rooms[game->player_room].enemy_num; i++) {
            if(game->floors[game->player_floor].Rooms[game->player_room].enemies[i] != NULL) {
                switch(game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->type) {
                    case Deamon:
                        game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->following = 0;
                        break;
                    case Monster:
                        game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->following = 0;
                        break;
                    case Giant:
                        game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->following = 5;
                        break;
                    case Snake:
                        game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->following = 1000000;
                        break;
                    case Undeed:
                        game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->following = 5;
                        break;
                }
            }
        }
    }
}

void game_won(Game *game, Player *player) {
    WINDOW *won = newwin(40, 146, 1, 1);
    wattron(won, COLOR_PAIR(11) | A_BOLD);
    wclear(won);
    for(int ii = 0; ii < 3; ii++) {
        for(int i = 0; i < 6; i++){
            mvwaddstr(won, 7 + i, 55 + (ii * 14), HAPPY_FACE[i]);
            mvwaddstr(won, 23 + i, 55 + (ii * 14), HAPPY_FACE[i]);
        }
    }


    for(int i = 14; i < 20; i++) {
        mvwaddstr(won, i, 5, ROGUE[i - 14]);
        mvwaddstr(won, i, 100, GAME[i - 14]);
    }
    char str[100];
    sprintf(str, "You earned %d Scores with %d Golds!", game->total_gold * 40, game->total_gold);
    mvwaddstr(won, 17, 55, "Congratulations! You won the game!");
    mvwaddstr(won, 18, 55, str);    
    mvwaddstr(won, 19, 55, "Press any key to return to Player Menu...");    
    wrefresh(won);
    player->total_gold += game->total_gold;
    player->total_score += game->total_gold * 40;
    player->num_finished++;
    save_game();

    getch();
    game = NULL;


    wattroff(won, COLOR_PAIR(11) | A_BOLD);
    delwin(won);
}

void save_game() {
    unlink(player->file_path);
    char fl[200000];
    snprintf(fl, 1000, "%s\n", player->username);
    // delete_enter(player->username);
    
    snprintf(fl + strlen(fl), 1000, "%s\n", player->password);
    // delete_enter(player->password);
    
    snprintf(fl + strlen(fl), 1000, "%s\n", player->email);
    // delete_enter(player->email);
    
    snprintf(fl + strlen(fl), 1000, "%d\n", (player->total_score));
    snprintf(fl + strlen(fl), 1000, "%d\n", (player->total_gold));
    snprintf(fl + strlen(fl), 1000, "%d\n", (player->num_finished));
    snprintf(fl + strlen(fl), 1000, "%ld\n", (player->time_experience));
    snprintf(fl + strlen(fl), 1000, "%d\n", (player->game_difficulty));
    snprintf(fl + strlen(fl), 1000, "%d\n", (player->color));
    // player->music = (Music *) malloc(sizeof(Music));
    snprintf(fl + strlen(fl), 1000, "%s\n", player->music->music_path);
    // fclose(player_file);
    // delete_enter(player->music->music_path);


    // // char g[100];
    // // fscanf(player_file, "%s", g);
    // if(player->game == NULL) {
    //     snprintf(fl + strlen(fl), 1000, "(null)\n");
    // }
    // else {
    //     FILE *player_file = fopen(player->file_path, "a");
    //     Game *game = player->game;
    //     // game_name
    //     snprintf(fl + strlen(fl), 1000, "%s\n", game->name);
    //     // strcpy(game->name, g);
        
        
    //     // game_floors_calloc
    //     snprintf(fl + strlen(fl), 1000, "%d\n", game->floor_num);
    //     fclose(player_file);
    //     for(int i = 0; i < game->floor_num; i++) {
    //         FILE* player_file2 = fopen(player->file_path, "a");

    //         fprintf(player_file2, "%d\n", game->floors[i].floor_visit);
    //         for(int ii = 0; ii < 40; ii++) {
    //             for(int jj = 0; jj < 146; jj++) 
    //                 fprintf(player_file2, "%d ", game->floors[i].map[ii][jj]);
    //             fprintf(player_file2, "\n");
    //         }

    //         for(int ii = 0; ii < 40; ii++) {
    //             for(int jj = 0; jj < 146; jj++) 
    //                 fprintf(player_file2, "%d ", game->floors[i].visit[ii][jj]);
    //             fprintf(player_file2, "\n");
    //         }

    //         fprintf(player_file2, "%d\n", game->floors[i].room_num);
    //         fprintf(player_file2, "%d\n", game->floors[i].has_gold);
    //         fprintf(player_file2, "%d\n", game->floors[i].staircase_num);
    //         fprintf(player_file2, "%d\n", game->floors[i].master_key_num);

    //         fclose(player_file2);
    //         for(int j = 0; j < game->floors[i].room_num; j++) {
    //             Room *room = &game->floors[i].Rooms[j];

    //             FILE* player_file3 = fopen(player->file_path, "a");

    //             fprintf(player_file3, "%d\n", room->start.y);
    //             fprintf(player_file3, "%d\n", room->start.x);
    //             fprintf(player_file3, "%d\n", room->size.y);
    //             fprintf(player_file3, "%d\n", room->size.x);
    //             fprintf(player_file3, "%d\n", room->type);

    //             // foods
    //             fprintf(player_file3, "%d\n", room->food_num);
    //             for(int z = 0; z < room->food_num; z++) {
    //                 if(room->foods[z] == NULL) {
    //                     fprintf(player_file3, "%d\n", -1);
    //                     continue;
    //                 }

    //                 // room->foods[z]->location.y = is;
    //                 // fprintf(player_file3, "%d\n", &);
    //                 fprintf(player_file3, "%d\n", room->foods[z]->location.y);
    //                 fprintf(player_file3, "%d\n", room->foods[z]->location.x);
    //                 fprintf(player_file3, "%d\n", room->foods[z]->type);
    //                 fprintf(player_file3, "%d\n", room->foods[z]->time_passed);
    //             }

    //             // golds
    //             fprintf(player_file3, "%d\n", room->gold_num);
    //             for(int z = 0; z < room->gold_num; z++) {
    //                 if(room->golds[z] == NULL) {
    //                     fprintf(player_file3, "%d\n", -1);
    //                     continue;
    //                 }

    //                 fprintf(player_file3, "%d\n", room->golds[z]->location.y);
    //                 fprintf(player_file3, "%d\n", room->golds[z]->location.x);
    //                 fprintf(player_file3, "%d\n", room->golds[z]->type);
    //             }
            
    //             // guns
    //             fprintf(player_file3, "%d\n", room->gun_num);
    //             for(int z = 0; z < room->gun_num; z++) {
    //                 if(room->guns[z] == NULL) {
    //                     fprintf(player_file3, "%d\n", -1);
    //                     continue;
    //                 }

    //                 fprintf(player_file3, "%d\n", room->guns[z]->location.y);
    //                 fprintf(player_file3, "%d\n", room->guns[z]->location.x);
    //                 fprintf(player_file3, "%d\n", room->guns[z]->type);
    //                 fprintf(player_file3, "%d\n", room->guns[z]->damage);
    //                 fprintf(player_file3, "%d\n", room->guns[z]->counter);   
    //                 fprintf(player_file3, "%d\n", room->guns[z]->distance);
    //                 // fprintf(player_file3, "%s\n", room->guns[z]->name);
    //             }
            
    //             //  spell
    //             fprintf(player_file3, "%d\n", room->spell_num);
    //             for(int z = 0; z < room->spell_num; z++) {
    //                 if(room->spells[z] == NULL) {
    //                     fprintf(player_file3, "%d\n", -1);
    //                     continue;
    //                 }

    //                 fprintf(player_file3, "%d\n", room->spells[z]->location.y);
    //                 fprintf(player_file3, "%d\n", room->spells[z]->location.x);
    //                 fprintf(player_file3, "%d\n", room->spells[z]->type);
    //             }
            
    //             // trap
    //             fprintf(player_file3, "%d\n", room->trap_num);
    //             for(int z = 0; z < room->trap_num; z++) {
    //                 if(room->traps[z] == NULL) {
    //                     fprintf(player_file3, "%d\n", -1);
    //                     continue;
    //                 }

    //                 fprintf(player_file3, "%d\n", room->traps[z]->location.y);
    //                 fprintf(player_file3, "%d\n", room->traps[z]->location.x); 
    //             }
            
    //             // enemy
    //             fprintf(player_file3, "%d\n", room->enemy_num);
    //             for(int z = 0; z < room->enemy_num; z++) {
    //                 if(room->enemies[z] == NULL) {
    //                     fprintf(player_file3, "%d\n", -1);
    //                     continue;
    //                 }

    //                 fprintf(player_file3, "%d\n", room->enemies[z]->location.y);
    //                 fprintf(player_file3, "%d\n", room->enemies[z]->location.x);
    //                 fprintf(player_file3, "%d\n", room->enemies[z]->type);
    //                 fprintf(player_file3, "%d\n", room->enemies[z]->damage);
    //                 fprintf(player_file3, "%d\n", room->enemies[z]->health);
    //                 fprintf(player_file3, "%d\n", room->enemies[z]->following);
    //                 fprintf(player_file3, "%d\n", room->enemies[z]->is_moving);
    //                 // fprintf(player_file3, "%c\n", room->enemies[z]->chr);
    //             }

    //             // normal_doors
    //             fprintf(player_file3, "%d\n", room->normal_doors_num);
    //             for(int z = 0; z < room->normal_doors_num; z++) {
    //                 fprintf(player_file3, "%d\n", room->normal_doors[z].location.y);
    //                 fprintf(player_file3, "%d\n", room->normal_doors[z].location.x);
    //             }

    //             // secret_doors
    //             fprintf(player_file3, "%d\n", room->secret_doors_num);
    //             for(int z = 0; z < room->secret_doors_num; z++) {
    //                 fprintf(player_file3, "%d\n", room->secret_doors[z].location.y);
    //                 fprintf(player_file3, "%d\n", room->secret_doors[z].location.x);
    //             }
            
    //             // stair_case
    //             if(room->staircase != NULL) {
    //                 fprintf(player_file3, "%d\n", room->staircase->location.y);
    //                 fprintf(player_file3, "%d\n", room->staircase->location.x);
    //             }                
    //             else {
    //                 fprintf(player_file3, "%d\n", -1);
    //             }            

    //             // master_key
    //             if(room->master_key != NULL) {
    //                 fprintf(player_file3, "%d\n", room->master_key->location.y);
    //                 fprintf(player_file3, "%d\n", room->master_key->location.x);
    //             }
    //             else {
    //                 fprintf(player_file3, "%d\n", -1);
    //             }
            

    //             //locked_door
    //             // fprintf(player_file3, "%d\n", &is);
    //             if(room->locked_door != NULL) {
    //                 fprintf(player_file3, "%d\n", room->locked_door->location.y);
    //                 fprintf(player_file3, "%d\n", room->locked_door->location.x);
    //                 fprintf(player_file3, "%d\n", room->locked_door->is_visited);
    //                 fprintf(player_file3, "%d\n", room->locked_door->password_turn);
    //             }
    //             else {
    //                 fprintf(player_file3, "%d\n", -1);
    //             }
    //             fclose(player_file3);
    //         }
    //     }
        
    //     player_file = fopen(player->file_path, "a");
    //     // game_health
    //     snprintf(fl + strlen(fl), 1000, "%d\n", game->Health);
        
    //     // game_food
    //     for(int i = 0; i < 3; i++) {
    //         snprintf(fl + strlen(fl), 1000, "%d\n", game->food_num[i]);
    //         for(int z = 0; z < game->food_num[i]; z++) {
    //             if(game->foods[z][0] == NULL) {
    //                 snprintf(fl + strlen(fl), 1000, "%d\n", -1);
    //                 continue;
    //             }

    //             snprintf(fl + strlen(fl), 1000, "%d\n", game->foods[z][0]->location.y);
    //             snprintf(fl + strlen(fl), 1000, "%d\n", game->foods[z][0]->location.x);
    //             snprintf(fl + strlen(fl), 1000, "%d\n", game->foods[z][0]->type);
    //             snprintf(fl + strlen(fl), 1000, "%d\n", game->foods[z][0]->time_passed);
    //         }
    //     } 

    //     // game_gun
    //     for(int i = 0; i < 5; i++) {
    //         if(game->gun[i] == NULL) {
    //             snprintf(fl + strlen(fl), 1000, "%d\n", -1);
    //             continue;
    //         }
            

    //         snprintf(fl + strlen(fl), 1000, "%d\n", game->gun[i]->location.y);
    //         snprintf(fl + strlen(fl), 1000, "%d\n", game->gun[i]->location.x);
    //         snprintf(fl + strlen(fl), 1000, "%d\n", game->gun[i]->type);
    //         snprintf(fl + strlen(fl), 1000, "%d\n", game->gun[i]->damage);
    //         snprintf(fl + strlen(fl), 1000, "%d\n", game->gun[i]->counter);
    //         snprintf(fl + strlen(fl), 1000, "%d\n", game->gun[i]->distance);
    //         snprintf(fl + strlen(fl), 1000, "%s\n", game->gun[i]->name);

    //     }
        
    //     int ptr = 0;
    //     for(int i = 0; i < 5; i++) {
    //         if(game->gun[i] != NULL && game->current_gun->type == game->gun[i]->type) {
    //             ptr = i;
    //             break;
    //         }
    //     }
    //     snprintf(fl + strlen(fl), 1000, "%d\n", ptr);

    //     // game_spell
    //     for(int i = 0; i < 3; i++) {
    //         snprintf(fl + strlen(fl), 1000, "%d\n", game->spell_num[i]);
    //         for(int z = 0; z < game->spell_num[i]; z++) {
    //             if(game->spell[z][0] == NULL) {
    //                 snprintf(fl + strlen(fl), 1000, "%d\n", -1);
    //                 continue;
    //             }

    //             snprintf(fl + strlen(fl), 1000, "%d\n", game->spell[z][0]->location.y);
    //             snprintf(fl + strlen(fl), 1000, "%d\n", game->spell[z][0]->location.x);
    //             snprintf(fl + strlen(fl), 1000, "%d\n", game->spell[z][0]->type);
    //         }
    //     }

    //     // game_master_key
    //     snprintf(fl + strlen(fl), 1000, "%d\n", game->master_key_num);
    //     for(int i = 0; i < game->master_key_num; i++) {
    //         if(game->master_key[i] == NULL) {
    //             snprintf(fl + strlen(fl), 1000, "%d\n", -1);
    //             continue;
    //         }

    //         snprintf(fl + strlen(fl), 1000, "%d\n", game->master_key[i]->location.y);
    //         snprintf(fl + strlen(fl), 1000, "%d\n", game->master_key[i]->location.x);
    //     }

    //     snprintf(fl + strlen(fl), 1000, "%d\n%d\n%d\n%d\n%d\n", game->total_gold, game->player_location.y, game->player_location.x, game->player_floor, game->player_room);
    //     // snprintf(fl + strlen(fl), 1000, "%d\n", );
    //     // snprintf(fl + strlen(fl), 1000, "%d\n", );
    //     // snprintf(fl + strlen(fl), 1000, "%d\n", );
    //     // snprintf(fl + strlen(fl), 1000, "%d\n", );
    //     // fclose(player_file);
    // }
    
    FILE* player_file = fopen(player->file_path, "w");
    fprintf(player_file, "%s", fl);
    fclose(player_file);
}

void help_page(Game *game) {
    WINDOW *help = newwin(40, 146, 1, 1);
    FILE *help_file = fopen("help.txt", "r");
    
    wattron(help, COLOR_PAIR(1) | A_BOLD);
    for(int i = 0; i < 37; i++) {
        mvwaddstr(help, i, 1, BOX2[i]);
    }

    char a[100];
    for(int i = 0; i < 26; i++) {
        fgets(a, 100, help_file);
        delete_enter(a);
        mvwaddstr(help, 1 + i, 4, a);
    }

    // // fgets(a, 100, help_file);
    for(int i = 0; i < 35; i++) {
        fgets(a, 100, help_file);
        delete_enter(a);
        mvwaddstr(help, 1 + i, 67, a);
    }

    fgets(a, 100, help_file);
    for(int i = 0; i < 12; i++) {
        fgets(a, 100, help_file);
        delete_enter(a);
        mvwaddstr(help, 1 + i, 111, a);
    }


    wattroff(help, COLOR_PAIR(1));

    wattron(help, COLOR_PAIR(8));
    mvwaddstr(help, 38, 57, "Press any key to back to game...");
    wattroff(help, COLOR_PAIR(8) | A_BOLD);

    wrefresh(help);


    int c = getch();

    delwin(help);

}

void delete_enter(char *s) {
    size_t len = strlen(s);
    if(s[len - 1] == '\n')
        s[len - 1] = '\0';
}

#endif


