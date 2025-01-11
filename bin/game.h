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
    Mace = 0x2696, 
    Dagger = 0x1F5E1,
    Magic_Wand = 0x1FA84,
    Normal_Arrow = 0x27B3,
    Sword = 0x2694
} Gun_Type;

typedef enum Spell_Type {
    Health, 
    Speed,
    Damage
} Spell_Type;



// Room contents
typedef struct Normal_Door {
    Location Location;


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
    
    Normal_Door *normal_doors;
    Staircase *staircase;
    Secret_Door *secret_doors;
    Locked_Door *locked_doors;
    
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

    //Enemy ....

} Room;


typedef struct Floor {
    char map[40][146];
    char visit[40][146];
    Room *Rooms;
    int room_num;
    int has_gold;

} Floor;


typedef struct Game{
    char name[100];
    Floor *floors;
    Location location;
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


// For creating new game
void create_new_game(Game **game, Music *music, enum Difficulty difficulty, int color) {
    
    // Initial Setup
    echo();
    clear_space();
    refresh();
    (*game) = (Game *) malloc(sizeof(Game));
    
    move(20, 60);
    attron(A_BOLD | COLOR_PAIR(1));
    addstr("Enter name for new game : ");
    attroff(A_BOLD | COLOR_PAIR(1));
    move(21, 60);
    getstr((*game)->name);
    clear_space();
    refresh();

    // Defines
    (*game)->floors = (Floor *) malloc(4 * sizeof(Floor));
    (*game)->floor_num = 4;
    
    (*game)->Health = 100;
    (*game)->total_score = 0;
    
    (*game)->foods = NULL;
    (*game)->food_num = 0;

    (*game)->golds = NULL;
    (*game)->gold_num = 0;

    (*game)->gun = (Gun ***) malloc(5 * sizeof(Gun **));
    (*game)->gun_num = (int *) calloc(5, sizeof(int));

    (*game)->spell = (Spell ***) malloc(3 * sizeof(Spell **));
    (*game)->spell_num = (int *) calloc(3, sizeof(int));

    (*game)->music = (Music *) malloc(sizeof(Music));
    strcpy((*game)->music->music_path, music->music_path);
    (*game)->game_difficulty = difficulty;
    (*game)->player_color = color;


    for(int i = 0; i < 4; i++)
        create_new_floor(&((*game)->floors[i]), i, (*game));

    while(true) {
        (*game)->location.y = (rand() % 40) + 1;
        (*game)->location.x = (rand() % 146) + 1;
        if((*game)->floors->map[(*game)->location.y][(*game)->location.x] == '.')
            break; 
    }



}


// Creating new floor
void create_new_floor(Floor *floor, int floor_num, Game *game) {
    floor->Rooms = (Room *) malloc(6 * sizeof(Room));
    floor->room_num = 6;
    if(floor_num == 3)
        floor->has_gold = rand() % 6;
    

    for(int i = 0; i < 6; i++) {
        create_new_room(&(floor->Rooms[i]), floor, floor_num, i, game);
        // adding to the map
    }
}


// Creating new room
void create_new_room(Room *room, Floor *floor, int floor_num, int room_num, Game *game) {


    // Room configuration
    bool flag = true;
    while(flag) {
        room->start.y = (rand() % 40) + 1;
        room->start.x = (rand() % 146) + 1;
        room->size.y = (rand() % 5) + 4;
        room->size.x = (rand() % 7) + 4;

        flag = false;
        for(int i = room->start.y; i < room->start.y + room->size.y; i++) {
            for(int j = room->start.x; j < room->start.x + room->size.x; j++) {
                if(floor->map[i][j] == '|' || floor->map[i][j] == '_' || floor->map[i][j] == '.' || i > 146 || j > 40) {
                    flag = true;
                    goto brk;
                }
            }
        }
        brk:
    }

    for(int i = room->start.y + 1; i < room->start.y + room->size.y - 1; i++) {
        for(int j = room->start.x + 1; j < room->start.x + room->size.x - 1; j++) {
            floor->map[i][j] = '.';
        }
    }
    for(int i = room->start.y; i < room->start.y + room->size.y; i++)
        floor->map[i][room->start.x] = '|', floor->map[i][room->start.x + room->size.x - 1] = '|';
    for(int j = room->start.x; j < room->start.x + room->size.x; j++)
        floor->map[j][room->start.y] = '_', floor->map[j][room->start.y + room->size.y - 1] = '_';
    
    if(floor_num == 3 && room_num == floor->has_gold)
        room->type = Treasure;
    else {
        Room_Type type[9] = {General, Enchant, General, General, Nightmare, General, General, Nightmare, Enchant};
        int rtype = rand() % 9;
        room->type = type[rtype];
    }

    switch(room->type) {
        case Treasure:
            room->trap_num = rand() % 3;
            room->gold_num = rand() % 4;
            break;
        
        case Enchant:
            room->spell_num = rand() % 4;
            break;

        case Nightmare:
        case General:
            room->food_num = rand() % (game->game_difficulty);
            room->gold_num = rand() % (game->game_difficulty);
            room->gun_num = rand() % 2;
            room->spell_num = rand() % 2;
            room->trap_num = rand() % 2;
            break;

            // Enemy....
    }


    // Food configuration
    room->foods = (Food **) malloc((room->food_num) * sizeof(Food *));
    Food_Type ftype[7] = {Ordinary, Excellent, Ordinary, Magical, Toxic, Ordinary, Ordinary};
    for(int i = 0; i < room->food_num; i++) {
        int ft = rand() % 7;
        room->foods[i]->type = ftype[ft];
        bool flag = true;
        while(flag) {
            room->foods[i]->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room->foods[i]->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
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
    room->golds = (Gold **) malloc((room->gold_num) * sizeof(Gold *));
    Gold_Type Gtype[7] = {Regular, Regular, Regular, Regular, Black, Black, Regular};
    for(int i = 0; i < room->gold_num; i++) {
        int Gt = rand() % 7;
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
    room->guns = (Gun **) malloc((room->gun_num) * sizeof(Gun *));
    Gun_Type gtype[4] = {Dagger, Magic_Wand, Normal_Arrow, Sword};
    for(int i = 0; i < room->gun_num; i++) {
        int gt = rand() % 4;
        room->guns[i]->type = gtype[gt];
        bool flag = true;
        while(flag) {
            room->guns[i]->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room->guns[i]->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
            if(floor->map[room->guns[i]->location.y][room->guns[i]->location.x] == '.') {
                flag = false;
                floor->map[room->guns[i]->location.y][room->guns[i]->location.x] = 'g';
                switch(room->guns[i]->type) {
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
    room->spells = (Spell **) malloc((room->spell_num) * sizeof(Spell *));
    Spell_Type stype[3] = {Health, Speed, Damage};
    for(int i = 0; i < room->spell_num; i++) {
        int st = rand() % 3;
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
    room->traps = (Trap **) malloc((room->trap_num) * sizeof(Trap *));
    for(int i = 0; i < room->trap_num; i++) {
        bool flag = true;
        while(flag) {
            room->traps[i]->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room->traps[i]->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
            if(floor->map[room->traps[i]->location.y][room->traps[i]->location.x] == '.') {
                flag = false;
                floor->map[room->traps[i]->location.y][room->traps[i]->location.x] = 't';
                // unicode
            }
        }
    }


    // Normal Door configuration
    

}


// Playing game
void play_game(Game *game) {

}


#endif game_h