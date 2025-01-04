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
    Room Rooms[6];

} Floor;


typedef struct Game{
    char name[100];
    Floor floors[4];
    Location player;
    int floor_num;
    int Health;
    int total_score;
    Food **foods;
    Gold **golds;
    Gun **gun;
    Spell **spell;
    int food_num;
    int gold_num;
    int gun_num;
    int spell_num;

    Music *music;
    enum Difficulty game_difficulty;
    int player_color;
    

} Game;

void create_new_game(Game **game);
void play_game(Game *game);


void create_new_game(Game **game) {
    (*game) = (Game *) malloc(sizeof(Game));
    

}

void play_game(Game *game) {

}


#endif game_h