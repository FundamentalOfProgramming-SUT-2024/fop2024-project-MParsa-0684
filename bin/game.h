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
#include <locale.h>
#include <wchar.h>
#include <unistd.h>




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



// Room contents
typedef struct Normal_Door {
    Location location;


} Normal_Door;

typedef struct Trap {
    Location location;
    wchar_t unicode;
    
    // go to war room
    // win to go back to current room
    // in phase 1, just decrease the player's health

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
    Location password_location;
    int password;
    wchar_t unicode;

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

} Food;

typedef struct Gold {
    Location location;
    Gold_Type type;
    wchar_t unicode;
    
} Gold;

typedef struct Gun {
    Gun_Type type;
    Location location;
    wchar_t unicode;

} Gun;

typedef struct Spell {
    Location location;
    Spell_Type type;
    wchar_t unicode;

} Spell;


// Game places
typedef struct Room {
    Location start;
    Location size;
    Room_Type type;
    
    Secret_Door *secret_door;
    Staircase *staircase;
    Locked_Door *locked_door;
    Master_Key *master_key;
    
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

    Normal_Door *normal_doors;
    int normal_doors_num;
    //Enemy ....

} Room;


typedef struct Floor {
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
    int total_score;
    Food **foods;
    Gold **golds;
    Gun ***gun;
    Spell ***spell;
    int food_num;
    int gold_num;
    int *gun_num;
    int *spell_num;

    Music *music;
    enum Difficulty game_difficulty;
    int player_color;

} Game;

void play_game(Game *game);
void create_new_game(Game **game, Music *music, enum Difficulty difficulty, int color);
void create_new_floor(Floor *floor, int floor_num, Game *game);
void create_new_room(Room *room, Floor *floor, int floor_num, int room_num, Game *game);
void in_room_road(Floor *floor);
void paint_floor(Game *game, Floor *floor, WINDOW *game_window);
// void *play_sound(void *arg);


// For creating new game
void create_new_game(Game **game, Music *music, enum Difficulty difficulty, int color) {
    
    // Initial Setup
    echo();
    clear_space();
    refresh();
    srand(time(NULL));
    (*game) = (Game *) calloc(1, sizeof(Game));
    
    move(20, 60);
    attron(A_BOLD | COLOR_PAIR(1));
    addstr("Enter name for new game : ");
    attroff(A_BOLD | COLOR_PAIR(1));
    move(21, 60);
    getstr((*game)->name);
    clear_space();
    refresh();

    // Defines
    (*game)->floors = (Floor *) calloc(4, sizeof(Floor));
    (*game)->floor_num = 4;
    
    (*game)->Health = 100;
    (*game)->total_score = 0;
    
    (*game)->foods = NULL;
    (*game)->food_num = 0;

    (*game)->golds = NULL;
    (*game)->gold_num = 0;

    (*game)->gun = (Gun ***) calloc(5, sizeof(Gun **));
    (*game)->gun_num = (int *) calloc(5, sizeof(int));

    (*game)->spell = (Spell ***) calloc(3, sizeof(Spell **));
    (*game)->spell_num = (int *) calloc(3, sizeof(int));

    (*game)->music = (Music *) calloc(1, sizeof(Music));
    strcpy((*game)->music->music_path, music->music_path);
    (*game)->game_difficulty = difficulty;
    (*game)->player_color = color;


    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 40; j++) {
            for(int k = 0; k < 146; k++)
                (*game)->floors[i].map[j][k] = ' ';
        }
        create_new_floor(&((*game)->floors[i]), i, (*game));
    }


    // Player Room and floor and location in first game
    (*game)->player_floor = 0;
    srand(time(NULL));
    while(true) {
        (*game)->player_location.y = (rand() % 40);
        (*game)->player_location.x = (rand() % 146);
        int i = (*game)->player_location.y;
        int j = (*game)->player_location.x;
        if((*game)->floors->map[i][j] == '.' && 
        !((i >= (*game)->floors[0].Rooms[(*game)->floors[0].staircase_num].start.y 
        && i < ((*game)->floors[0].Rooms[(*game)->floors[0].staircase_num].start.y 
        + (*game)->floors[0].Rooms[(*game)->floors[0].staircase_num].size.y)) 
        && (j >= (*game)->floors[0].Rooms[(*game)->floors[0].staircase_num].start.x 
        && j < ((*game)->floors[0].Rooms[(*game)->floors[0].staircase_num].start.x 
        + (*game)->floors[0].Rooms[(*game)->floors[0].staircase_num].size.x))))
            break; 
    
    }
    
    for(int i = 0; i < 6; i++) {
        if((*game)->player_location.x >= (*game)->floors[0].Rooms[i].start.x && (*game)->player_location.x < (*game)->floors[0].Rooms[i].start.x + (*game)->floors[0].Rooms[i].size.x && (*game)->player_location.y >= (*game)->floors[0].Rooms[i].start.y && (*game)->player_location.y < (*game)->floors[0].Rooms[i].start.y + (*game)->floors[0].Rooms[i].size.y) {
            (*game)->player_room = i;
        }
    }

    // make player's room visited for starting of the game
    for(int i = (*game)->floors[0].Rooms[(*game)->player_room].start.y; i < (*game)->floors[0].Rooms[(*game)->player_room].start.y + (*game)->floors[0].Rooms[(*game)->player_room].size.y; i++) {
        for(int j = (*game)->floors[0].Rooms[(*game)->player_room].start.x; j < (*game)->floors[0].Rooms[(*game)->player_room].start.x + (*game)->floors[0].Rooms[(*game)->player_room].size.x; j++) {
            (*game)->floors[0].visit[i][j] = true;
        }
    }

}

// Creating new floor
void create_new_floor(Floor *floor, int floor_num, Game *game) {
    srand(time(NULL));
    floor->Rooms = (Room *) calloc(6, sizeof(Room));
    floor->room_num = 6;
    if(floor_num == 3)
        floor->has_gold = rand() % 6;
    else 
        floor->has_gold = -1;
    
    floor->staircase_num = rand() % 6;
    floor->master_key_num = rand() % 6;

    for(int i = 0; i < 6; i++) {
        create_new_room(&(floor->Rooms[i]), floor, floor_num, i, game);
        // adding to the map
    }

    in_room_road(floor);

     
}

// Creating new room
void create_new_room(Room *room, Floor *floor, int floor_num, int room_num, Game *game) {

    // Room configuration
    bool flag = true;
    srand(time(NULL));
    while(flag) {
        room->start.y = (rand() % 39) + 1;
        room->start.x = (rand() % 145) + 1;
        room->size.y = (rand() % 10) + 5;
        room->size.x = (rand() % 14) + 5;

        flag = false;
        for(int i = room->start.y; i < room->start.y + room->size.y + 1 && i < 40; i++) {
            for(int j = room->start.x; j < room->start.x + room->size.x + 1 && j < 146; j++) {
                if(i >= 39 || j >= 145 || floor->map[i][j] == '|' || floor->map[i][j] == '_' || floor->map[i][j] == '.') {
                    flag = true;
                }
            }
        }
    }

    for(int i = room->start.y + 1; i < room->start.y + room->size.y - 1; i++) {
        for(int j = room->start.x + 1; j < room->start.x + room->size.x - 1; j++) {
            floor->map[i][j] = '.';
        }
    }
    for(int i = room->start.y; i < room->start.y + room->size.y; i++)
        floor->map[i][room->start.x] = '|', floor->map[i][room->start.x + room->size.x - 1] = '|';
    for(int j = room->start.x; j < room->start.x + room->size.x; j++)
        floor->map[room->start.y][j] = '_', floor->map[room->start.y + room->size.y - 1][j] = '_';
    
    srand(time(NULL));
    if(floor_num == 3 && room_num == floor->has_gold)
        room->type = Treasure;
    else {
        Room_Type type[9] = {General, Enchant, General, General, Nightmare, General, General, Nightmare, General};
        int rtype = rand() % 9;
        room->type = type[rtype];
    }

    srand(time(NULL));
    switch(room->type) {
        case Treasure:
            srand(time(NULL));
            room->trap_num = rand() % 3;
            srand(time(NULL));
            room->gold_num = rand() % 4;
            break;
        
        case Enchant:
            room->spell_num = rand() % 4;
            break;

        case Nightmare:
        case General:
            srand(time(NULL));
            room->food_num = rand() % (game->game_difficulty);
            srand(time(NULL));
            room->gold_num = rand() % (game->game_difficulty);
            srand(time(NULL));
            room->gun_num = rand() % 2;
            srand(time(NULL));
            room->spell_num = rand() % 2;
            srand(time(NULL));
            room->trap_num = rand() % 2;
            break;

            // Enemy....
    }


    // Food configuration
    room->foods = (Food **) calloc((room->food_num), sizeof(Food *));
    Food_Type ftype[7] = {Ordinary, Excellent, Ordinary, Magical, Toxic, Ordinary, Ordinary};
    for(int i = 0; i < room->food_num; i++) {
        srand(time(NULL));
        int ft = rand() % 7;
        room->foods[i] = (Food *) calloc(1, sizeof(Food));
        room->foods[i]->type = ftype[ft];
        bool flag = true;
        while(flag) {
            room->foods[i]->location.y = (rand() % (room->size.y - 1)) + room->start.y + 1; 
            room->foods[i]->location.x = (rand() % (room->size.x - 1)) + room->start.x + 1; 
            if(floor->map[room->foods[i]->location.y][room->foods[i]->location.x] == '.') {
                flag = false;
                floor->map[room->foods[i]->location.y][room->foods[i]->location.x] = 'F';
                switch(room->foods[i]->type) {
                    case Ordinary:
                        //unicode
                        break;
                    case Excellent:
                        //unicode
                        break;
                    case Magical:
                        //unicode
                        break;
                    case Toxic:
                        //unicode
                        break;
                }
            }
        }
    }

    // Gold configuration
    room->golds = (Gold **) calloc((room->gold_num), sizeof(Gold *));
    Gold_Type Gtype[7] = {Regular, Regular, Regular, Regular, Black, Black, Regular};
    for(int i = 0; i < room->gold_num; i++) {
        srand(time(NULL));
        int Gt = rand() % 7;
        room->golds[i] = (Gold *) calloc(1, sizeof(Gold));
        room->golds[i]->type = Gtype[Gt];
        bool flag = true;
        while(flag) {
            room->golds[i]->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room->golds[i]->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
            if(floor->map[room->golds[i]->location.y][room->golds[i]->location.x] == '.') {
                flag = false;
                floor->map[room->golds[i]->location.y][room->golds[i]->location.x] = 'G';
                switch(room->golds[i]->type) {
                    case Regular:
                        //unicode
                        break;
                    case Black:
                        //unicode
                        break;
                }
            }
        }
    }

    // Gun configuration
    room->guns = (Gun **) calloc((room->gun_num), sizeof(Gun *));
    Gun_Type gtype[4] = {Dagger, Magic_Wand, Normal_Arrow, Sword};
    for(int i = 0; i < room->gun_num; i++) {
        srand(time(NULL));
        int gt = rand() % 4;
        room->guns[i] = (Gun *) calloc(1, sizeof(Gun));
        room->guns[i]->type = gtype[gt];
        bool flag = true;
        while(flag) {
            room->guns[i]->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room->guns[i]->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
            if(floor->map[room->guns[i]->location.y][room->guns[i]->location.x] == '.') {
                flag = false;
                floor->map[room->guns[i]->location.y][room->guns[i]->location.x] = 'g';
                switch(room->guns[i]->type) {
                    case Mace:
                        //unicode
                        break;
                    case Dagger:
                        //unicode
                        break;
                    case Magic_Wand:
                        //unicode
                        break;
                    case Normal_Arrow:
                        //unicode
                        break;
                    case Sword:
                        //unicode
                        break;
                }
            }
        }
    }
    
    // Spell configuration
    room->spells = (Spell **) calloc((room->spell_num), sizeof(Spell *));
    Spell_Type stype[3] = {Health, Speed, Damage};
    for(int i = 0; i < room->spell_num; i++) {
        srand(time(NULL));
        int st = rand() % 3;
        room->spells[i] = (Spell *) calloc(1, sizeof(Spell));
        room->spells[i]->type = stype[st];
        bool flag = true;
        while(flag) {
            room->spells[i]->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room->spells[i]->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
            if(floor->map[room->spells[i]->location.y][room->spells[i]->location.x] == '.') {
                flag = false;
                floor->map[room->spells[i]->location.y][room->spells[i]->location.x] = 's';
                switch(room->spells[i]->type) {
                    case Health:
                        //unicode
                        break;
                    case Speed:
                        //unicode
                        break;
                    case Damage:
                        //unicode
                        break;
                }
            }
        }
    }

    // Trap configuration
    room->traps = (Trap **) calloc((room->trap_num), sizeof(Trap *));
    for(int i = 0; i < room->trap_num; i++) {
        srand(time(NULL));
        room->traps[i] = (Trap *) calloc(1, sizeof(Trap));
        bool flag = true;
        while(flag) {
            room->traps[i]->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room->traps[i]->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
            if(floor->map[room->traps[i]->location.y][room->traps[i]->location.x] == '.') {
                flag = false;
                floor->map[room->traps[i]->location.y][room->traps[i]->location.x] = '^';
                // unicode
            }
        }
    }

    // Normal Door configuration
    // srand(time(NULL));
    // room->normal_door = (Normal_Door *) calloc(1, sizeof(Normal_Door));
    // flag = true;
    // while(flag) {
    //     room->normal_door->location.x = rand() % (room->size.x) + room->start.x;
    //     room->normal_door->location.y = rand() % (room->size.y) + room->start.y;
    //     if(floor->map[room->normal_door->location.y][room->normal_door->location.x] == '|' || floor->map[room->normal_door->location.y][room->normal_door->location.x] == '_') {
    //         flag = false;
    //         floor->map[room->normal_door->location.y][room->normal_door->location.x] = '+';
    //     }
    // }

    // Staircase
    srand(time(NULL));
    room->staircase = (Staircase *) calloc(1, sizeof(Staircase));
    if(room_num == floor->staircase_num) {
        bool flag = true;
        while(flag) {
            room->staircase->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room->staircase->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
            if(floor->map[room->staircase->location.y][room->staircase->location.x] == '.') {
                flag = false;
                floor->map[room->staircase->location.y][room->staircase->location.x] = '<';
                // unicode
            }
        }

    }
    else {
        room->staircase = NULL;
    }
    
    // Master Key
    srand(time(NULL));
    room->master_key = (Master_Key *) calloc(1, sizeof(Master_Key));
    if(room_num == floor->master_key_num) {
        bool flag = true;
        while(flag) {
            room-> master_key->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room-> master_key->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
            if(floor->map[room->master_key->location.y][room->master_key->location.x] == '.') {
                flag = false;
                floor->map[room->master_key->location.y][room->master_key->location.x] = 'M';
                // unicode
            }
        }
    }
    else {
        room->master_key = NULL;
    }

    // Locked, Secret Doors

}

// Creating In_Room roads
void in_room_road(Floor *floor) {

    for(int i = 1; i < 6; i++) {
        Location current ={floor->Rooms[i - 1].start.x + (floor->Rooms[i - 1].size.x / 2), floor->Rooms[i - 1].start.y + (floor->Rooms[i - 1].size.y / 2)};
        Location end = {floor->Rooms[i].start.x + (floor->Rooms[i].size.x / 2), floor->Rooms[i].start.y + (floor->Rooms[i].size.y / 2)};
    
        int dx = end.x - current.x;
        int ctr = 0;
        if(dx > 0) {
            while(current.x < end.x) {
                if(floor->map[current.y][current.x] == ' ' || floor->map[current.y][current.x] == '#') 
                    floor->map[current.y][current.x] = '#';
                else if((floor->map[current.y][current.x] == '_' && floor->map[current.y][current.x - 1] == '#') || (floor->map[current.y][current.x] == '|')) {
                    if((floor->map[current.y][current.x] == '_' && floor->map[current.y][current.x - 1] == '#')) {
                        if(current.y > 0 && (floor->map[current.y - 1][current.x] == '|' || floor->map[current.y - 1][current.x] == '+'))
                            current.y--, floor->map[current.y][current.x - 1] = '#';
                        else if(current.y < 39 && (floor->map[current.y + 1][current.x] == '|' || floor->map[current.y - 1][current.x] == '+'))
                            current.y++, floor->map[current.y][current.x - 1] = '#';
                    }

                    floor->map[current.y][current.x] = '+';
                    bool flag = false;
                    for(int j = 0; j < 6 && !flag; j++) {
                        if(current.x >= floor->Rooms[j].size.x && current.x < floor->Rooms[j].start.x + floor->Rooms[j].size.x && current.y >= floor->Rooms[j].size.y && current.y < floor->Rooms[j].start.y + floor->Rooms[j].size.y) {
                            floor->Rooms[j].normal_doors_num++;
                            floor->Rooms[j].normal_doors = (Normal_Door *) realloc(floor->Rooms[j].normal_doors, floor->Rooms[j].normal_doors_num * sizeof(Normal_Door));
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.x = current.x;
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.y = current.y;
                            flag = true;
                        }
                    }
    // WINDOW *game_window = newwin(40, 146, 1, 1);
    // wclear(game_window);
    // for(int i = 0; i < 40; i++) {
    //     // move(i + 1, 1);
    //     printf(" ");
    //     for(int j = 0; j < 146; j++) {
    //         wattron(game_window, COLOR_PAIR(1));
    //         // wmove(game_window, i + 1, j + 1);
    //         mvwaddch(game_window, i, j, floor->map[i][j]);
    //         wattroff(game_window, COLOR_PAIR(1));
    //         wrefresh(game_window);
    //     }
    //     printf(" ");
    // }

    // delwin(game_window);
    // sleep(1);
                }
                current.x++;
                ctr++;
                if(ctr % 8 == 0 && floor->map[current.y][current.x - 1] != '+') {
                    srand(time(NULL));
                    int r = rand() % 2;
                    if(r == 0 && current.y < 39 && floor->map[current.y + 1][current.x] != '_' && floor->map[current.y + 1][current.x] != '|')
                        current.y++;
                    else if(r == 1 && current.y > 0 && floor->map[current.y - 1][current.x] != '_' && floor->map[current.y - 1][current.x] != '|')
                        current.y--;
                    current.x--;
                }
            }
        }
        else if (dx < 0){
            while(current.x > end.x) {
                if(floor->map[current.y][current.x] == ' ' || floor->map[current.y][current.x] == '#')
                    floor->map[current.y][current.x] = '#';
                else if((floor->map[current.y][current.x] == '_' && (floor->map[current.y][current.x + 1] == '#')) || (floor->map[current.y][current.x] == '|'))  {
                    if((floor->map[current.y][current.x] == '_' && floor->map[current.y][current.x + 1] == '#')) {
                        if(current.y > 0 && (floor->map[current.y - 1][current.x] == '|' || floor->map[current.y - 1][current.x] == '+'))
                            current.y--, floor->map[current.y][current.x + 1] = '#';
                        else if(current.y < 39 && (floor->map[current.y - 1][current.x] == '|' || floor->map[current.y - 1][current.x] == '+'))
                            current.y++, floor->map[current.y][current.x + 1] = '#';
                    }

                    floor->map[current.y][current.x] = '+';
                    bool flag = false;
                    for(int j = 0; j < 6 && !flag; j++) {
                        if(current.x >= floor->Rooms[j].size.x && current.x < floor->Rooms[j].start.x + floor->Rooms[j].size.x && current.y >= floor->Rooms[j].size.y && current.y < floor->Rooms[j].start.y + floor->Rooms[j].size.y) {
                            floor->Rooms[j].normal_doors_num++;
                            floor->Rooms[j].normal_doors = (Normal_Door *) realloc(floor->Rooms[j].normal_doors, floor->Rooms[j].normal_doors_num * sizeof(Normal_Door));
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.x = current.x;
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.y = current.y;
                            flag = true;
                        }
                    }

                }
                current.x--;
                ctr++;
                if(ctr % 8 == 0 && floor->map[current.y][current.x + 1] != '+') {
                    srand(time(NULL));
                    int r = rand() % 2;
                    if(r == 0 && current.y < 39 && floor->map[current.y + 1][current.x] != '_' && floor->map[current.y + 1][current.x] != '|')
                        current.y++;
                    else if(r == 1 && current.y > 0 && floor->map[current.y - 1][current.x] != '_' && floor->map[current.y - 1][current.x] != '|')
                        current.y--;
                    current.x++;
                }
    //                 WINDOW *game_window = newwin(40, 146, 1, 1);
    // wclear(game_window);
    // for(int i = 0; i < 40; i++) {
    //     // move(i + 1, 1);
    //     printf(" ");
    //     for(int j = 0; j < 146; j++) {
    //         wattron(game_window, COLOR_PAIR(1));
    //         // wmove(game_window, i + 1, j + 1);
    //         mvwaddch(game_window, i, j, floor->map[i][j]);
    //         wattroff(game_window, COLOR_PAIR(1));
    //         wrefresh(game_window);
    //     }
    //     printf(" ");
    // }

    // delwin(game_window);
    // sleep(1);
            }
        }


        int dy = end.y - current.y;
        if(dy > 0) {
            while(current.y < end.y) {
                if(floor->map[current.y][current.x] == ' ' || floor->map[current.y][current.x] == '#') 
                    floor->map[current.y][current.x] = '#';
                else if(floor->map[current.y][current.x] == '_' || floor->map[current.y][current.x] == '+') {
                    floor->map[current.y][current.x] = '+';
                    bool flag = false;
                    for(int j = 0; j < 6 && !flag; j++) {
                        if(current.x >= floor->Rooms[j].size.x && current.x < floor->Rooms[j].start.x + floor->Rooms[j].size.x && current.y >= floor->Rooms[j].size.y && current.y < floor->Rooms[j].start.y + floor->Rooms[j].size.y) {
                            floor->Rooms[j].normal_doors_num++;
                            floor->Rooms[j].normal_doors = (Normal_Door *) realloc(floor->Rooms[j].normal_doors, floor->Rooms[j].normal_doors_num * sizeof(Normal_Door));
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.x = current.x;
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.y = current.y;
                            flag = true;
                        }
                    }
                }
                current.y++;
    //                 WINDOW *game_window = newwin(40, 146, 1, 1);
    // wclear(game_window);
    // for(int i = 0; i < 40; i++) {
    //     // move(i + 1, 1);
    //     printf(" ");
    //     for(int j = 0; j < 146; j++) {
    //         wattron(game_window, COLOR_PAIR(1));
    //         // wmove(game_window, i + 1, j + 1);
    //         mvwaddch(game_window, i, j, floor->map[i][j]);
    //         wattroff(game_window, COLOR_PAIR(1));
    //         wrefresh(game_window);
    //     }
    //     printf(" ");
    // }

    // delwin(game_window);
    // sleep(1);
            }
        }
        else if(dy < 0) {
            while(current.y > end.y) {
                if(floor->map[current.y][current.x] == ' ' || floor->map[current.y][current.x] == '#') 
                    floor->map[current.y][current.x] = '#';
                else if(floor->map[current.y][current.x] == '_' || floor->map[current.y][current.x] == '+') {
                    floor->map[current.y][current.x] = '+';
                    bool flag = false;
                    for(int j = 0; j < 6 && !flag; j++) {
                        if(current.x >= floor->Rooms[j].size.x && current.x < floor->Rooms[j].start.x + floor->Rooms[j].size.x && current.y >= floor->Rooms[j].size.y && current.y < floor->Rooms[j].start.y + floor->Rooms[j].size.y) {
                            floor->Rooms[j].normal_doors_num++;
                            floor->Rooms[j].normal_doors = (Normal_Door *) realloc(floor->Rooms[j].normal_doors, floor->Rooms[j].normal_doors_num * sizeof(Normal_Door));
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.x = current.x;
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.y = current.y;
                            flag = true;
                        }
                    }
                }
                current.y--;
    //                 WINDOW *game_window = newwin(40, 146, 1, 1);
    // wclear(game_window);
    // for(int i = 0; i < 40; i++) {
    //     // move(i + 1, 1);
    //     printf(" ");
    //     for(int j = 0; j < 146; j++) {
    //         wattron(game_window, COLOR_PAIR(1));
    //         // wmove(game_window, i + 1, j + 1);
    //         mvwaddch(game_window, i, j, floor->map[i][j]);
    //         wattroff(game_window, COLOR_PAIR(1));
    //         wrefresh(game_window);
    //     }
    //     printf(" ");
    // }

    // delwin(game_window);
    // sleep(1);
            }
        }
    }



}

// Playing game
void play_game(Game *game) {

    //Initial Setups
    clear_space2();
    setlocale(LC_ALL, "");
    WINDOW *game_window = newwin(40, 146, 1, 1);

    // Music
    // pthread_t sound_thread;
    // pthread_create(&sound_thread, NULL, play_sound, (void *) game->music->music_path);

    while(true) {
        paint_floor(game, &game->floors[game->player_floor], game_window);
        sleep(5);
        break;

    }

    // // Music end
    // pthread_cancel(sound_thread); 
    // pthread_join(sound_thread, NULL); 

    delwin(game_window);
}

void paint_floor(Game *game, Floor *floor, WINDOW *game_window) {
    clear_space2();
    wclear(game_window);

    for(int i = 0; i < 40; i++) {
        for(int j = 0; j < 146; j++) {
            int color_num;
            if(floor->visit[i][j] == true) {
                bool flag = false;
                if(floor->map[i][j] == '#') {
                    wattron(game_window, COLOR_PAIR(1));
                    color_num = 1;
                    flag = true;
                }
                for(int ii = 0; ii < 6 && !flag; ii++) {
                    if(i >= floor->Rooms[ii].start.y && j >= floor->Rooms[ii].start.x && i < (floor->Rooms[ii].start.y + floor->Rooms[ii].size.y) && j < (floor->Rooms[ii].start.x + floor->Rooms[ii].size.x)) {
                        switch(floor->Rooms[ii].type) {
                            case General:
                                wattron(game_window, COLOR_PAIR(1));
                                color_num = 1;
                                break;
                            case Treasure:
                                wattron(game_window, COLOR_PAIR(4));
                                color_num = 4;
                                break;
                            case Enchant:
                                wattron(game_window, COLOR_PAIR(5));
                                color_num = 5;
                                break;
                            case Nightmare:
                                wattron(game_window, COLOR_PAIR(6));
                                color_num = 6;
                                break;
                        }
                        flag = true;
                    } 
                }
                
                mvwaddch(game_window, i, j, floor->map[i][j]);
                wrefresh(game_window);

                wattroff(game_window, COLOR_PAIR(color_num));
            }
        }
    }

    wattron(game_window, COLOR_PAIR(game->player_color) | A_BLINK | A_BOLD);
    mvwaddch(game_window, game->player_location.y, game->player_location.x, 'P');
    wrefresh(game_window);
    wattroff(game_window, COLOR_PAIR(game->player_color) | A_BLINK | A_BOLD);

}

// void *play_sound(void *arg) {
    // const char *sound_file = (const char *)arg;
    // char command[256];
// 
    // snprintf(command, sizeof(command), "afplay %s", sound_file);
// 
    // while (1) {
        // system(command);
        // sleep(1);
    // }
// 
    // return NULL;
// }

#endif
