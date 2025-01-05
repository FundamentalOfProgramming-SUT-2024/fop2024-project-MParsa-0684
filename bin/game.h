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
    Easy,
    Normal,
    Hard
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
    health, 
    Speed,
    Damage
} Spell_Type;



// Room contents
typedef struct Normal_Door {
    Location Location;


} Normal_Door;

typedef struct Trap {
    Location location;
    
    // go to war room
    // win to go back to current room
    // in phase 1, just decrease the player's health

} Trap;

typedef struct Staircase {
    Location location;

} Staircase;

typedef struct Secret_Door {
    Location location;

} Secret_Door;

typedef struct Locked_Door {
    Location location;
    int password;

    // show when locked with RED, when open with GREEN 
    // 

} Locked_Door;

typedef struct Master_Key {
    Location location;

} Master_Key;



// Player contents
typedef struct Food {
    Location location;
    Food_Type type;

} Food;

typedef struct Gold {
    Location location;
    Gold_Type type;
    
} Gold;

typedef struct Gun {
    Gun_Type type;
    Location location;
    wchar_t unicode;

} Gun;

typedef struct Spell {
    Location location;
    Spell_Type type;

} Spell;


// Game places
typedef struct Room {
    Location start;
    Location size;
    Room_Type type;
    Normal_Door *normal_doors;
    Trap *traps;
    Staircase *staircase;
    Secret_Door *secret_doors;
    Locked_Door *locked_doors;
    Food **foods;
    Gold **golds;
    Gun **gun;
    Spell **spell;

} Room;


typedef struct Floor {
    char map[40][146];
    char visit[40][146];
    Room *Rooms;
    int room_num;

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
void create_new_floor(Floor *floor);
void create_new_room(Room *room, Floor *floor);


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
        create_new_floor(&((*game)->floors[i]));

    while(true) {
        (*game)->location.y = (rand() % 40) + 1;
        (*game)->location.x = (rand() % 146) + 1;
        if((*game)->floors->map[(*game)->location.y][(*game)->location.x] == '.')
            break; 
    }



}


// Creating new floor
void create_new_floor(Floor *floor) {
    floor->Rooms = (Room *) malloc(6 * sizeof(Room));
    floor->room_num = 6;

    for(int i = 0; i < 6; i++) {
        create_new_room(&(floor->Rooms[i]), floor);
        // adding to the map
    }
}


// Creating new room
void create_new_room(Room *room, Floor *floor) {

    bool flag = true;
    while(flag) {
        room->start.y = (rand() % 40) + 1;
        room->start.x = (rand() % 146) + 1;
        room->size.y = (rand() % 5) + 4;
        room->size.x = (rand() % 7) + 4;

        flag = false;
        for(int i = room->start.y; i < room->start.y + room->size.y; i++) {
            for(int j = room->start.x; j < room->start.x + room->size.x; j++) {
                if(floor->map[i][j] == '|' || floor->map[i][j] == '_' || floor->map[i][j] == '.') {
                    flag = true;
                    goto brk;
                }
            }
        }
        brk:
    }

    
}


// Playing game
void play_game(Game *game) {

}


#endif game_h