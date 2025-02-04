#ifndef game_h
#define game_h

#include <stdio.h>
#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <regex.h>
#include <dirent.h>
#include <pthread.h>
#include <locale.h>
#include <wchar.h>
#include <unistd.h>
#include <signal.h>

#include "ui.h"
#include "menus.h"
#include "feature.h"

pid_t afplay_pid = -1;
bool music_changed = false;
pthread_t sound_thread;

void play_game(Game *game);
void create_new_game(Game **game, Music *music, enum Difficulty difficulty, int color);
void create_new_floor(Floor *floor, int floor_num, Game *game);
void create_new_room(Room *room, Floor *floor, int floor_num, int room_num, Game *game);
void in_room_road(Floor *floor, int floor_num);
void paint_floor(Game *game, Floor *floor, WINDOW *game_window, int time_passed, bool is_unicode);
void *play_sound(void *arg);

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
    (*game)->total_gold = 0;
    
    (*game)->foods = (Food ***) calloc(3, sizeof(Food **));
    (*game)->food_num = (int *) calloc(3, sizeof(int));

    (*game)->gun = (Gun **) calloc(5, sizeof(Gun *));
    (*game)->gun[0] = (Gun *) calloc(1, sizeof(Gun));
    strcpy((*game)->gun[0][0].name,"Mace"), (*game)->gun[0][0].type = Mace, (*game)->gun[0][0].counter = 1, (*game)->gun[0][0].damage = 5, (*game)->gun[0][0].distance = 1, (*game)->gun[0][0].unicode = 0x00002694;
    (*game)->current_gun = (*game)->gun[0];

    (*game)->spell = (Spell ***) calloc(3, sizeof(Spell **));
    (*game)->spell_num = (int *) calloc(3, sizeof(int));

    (*game)->master_key = NULL;
    (*game)->master_key_num = 0;

    (*game)->music = (Music *) calloc(1, sizeof(Music));
    strcpy((*game)->music->music_path, music->music_path);
    (*game)->game_difficulty = difficulty;
    (*game)->player_color = color;

    (*game)->time_power = -1;

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 40; j++) {
            for(int k = 0; k < 146; k++)
                (*game)->floors[i].map[j][k] = ' ';
        }
        create_new_floor(&((*game)->floors[i]), i, (*game));
    }


    (*game)->player_unicode  = 0x0001F47E;
    // Player Room and floor and location in first game
    (*game)->player_floor = 0;
    (*game)->floors[0].floor_visit = true;
    bool flag = false;
    while(!flag) {
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
            flag = true; 
    
        for(int i = 0; i < 6; i++) {
            if((*game)->player_location.x >= (*game)->floors[0].Rooms[i].start.x && (*game)->player_location.x < (*game)->floors[0].Rooms[i].start.x + (*game)->floors[0].Rooms[i].size.x && (*game)->player_location.y >= (*game)->floors[0].Rooms[i].start.y && (*game)->player_location.y < (*game)->floors[0].Rooms[i].start.y + (*game)->floors[0].Rooms[i].size.y) {
                if((*game)->floors[0].Rooms[i].type == General)
                    (*game)->player_room = i;
                else
                    flag = false;
            }
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
    floor->Rooms = (Room *) calloc(6, sizeof(Room));
    floor->room_num = 6;
    if(floor_num == 3)
        floor->has_gold = 5;
    else 
        floor->has_gold = -1;
    
    while(true) {
        floor->staircase_num = rand() % 6;
        if(floor->has_gold != floor->staircase_num)
            break;
    }
    
    while(true) {
        floor->master_key_num = rand() % 6;
        if(floor->has_gold != floor->staircase_num)
            break;
    }

    for(int i = 0; i < 6; i++) {
        create_new_room(&(floor->Rooms[i]), floor, floor_num, i, game);
    }

    in_room_road(floor, floor_num);


    // Secret Door configuration
    int size = (floor_num == 3) ? 5 : 6;
    int room = rand() % size;
    floor->Rooms[room].secret_doors_num = floor->Rooms[room].normal_doors_num;
    floor->Rooms[room].secret_doors = (Secret_Door *) calloc(floor->Rooms[room].secret_doors_num, sizeof(Secret_Door));
    for(int i = 0; i < floor->Rooms[room].normal_doors_num; i++) {
        floor->Rooms[room].secret_doors[i].location.y = floor->Rooms[room].normal_doors[i].location.y;
        floor->Rooms[room].secret_doors[i].location.x = floor->Rooms[room].normal_doors[i].location.x;
        floor->map[floor->Rooms[room].secret_doors[i].location.y][floor->Rooms[room].secret_doors[i].location.x] = '*';
    }
    free(floor->Rooms[room].normal_doors);
    floor->Rooms[room].normal_doors_num = 0;



     
}

// Creating new room
void create_new_room(Room *room, Floor *floor, int floor_num, int room_num, Game *game) {

    // Room configuration
    bool flag = true;
    while(flag) {
        room->start.y = (rand() % 39) + 1;
        room->start.x = (rand() % 145) + 1;
        room->size.y = (rand() % 10) + 5;
        room->size.x = (rand() % 14) + 5;

        // Random room placement
        flag = false;
        for(int i = room->start.y; i < room->start.y + room->size.y + 1 && i < 40 && !flag; i++) {
            for(int j = room->start.x; j < room->start.x + room->size.x + 1 && j < 146 && !flag; j++) {
                if(i >= 39 || j >= 145 || floor->map[i][j] == '|' || floor->map[i][j] == '_' || floor->map[i][j] == '.') {
                    flag = true;
                }
            }
        }
    }

    //room chars on floor map
    for(int i = room->start.y + 1; i < room->start.y + room->size.y - 1; i++) {
        for(int j = room->start.x + 1; j < room->start.x + room->size.x - 1; j++) {
            floor->map[i][j] = '.';
        }
    }
    for(int i = room->start.y; i < room->start.y + room->size.y; i++)
        floor->map[i][room->start.x] = '|', floor->map[i][room->start.x + room->size.x - 1] = '|';
    for(int j = room->start.x; j < room->start.x + room->size.x; j++)
        floor->map[room->start.y][j] = '_', floor->map[room->start.y + room->size.y - 1][j] = '_';
    

    //room type specification
    if(floor_num == 3 && room_num == floor->has_gold)
        room->type = Treasure;
    else {
        Room_Type type[9] = {General, Nightmare, General, General, Enchant, General, General, Nightmare, Enchant};
        long long int rtype = rand() % 9;
        room->type = type[rtype];
    }

    // Room random things generator
    switch(room->type) {
        case Treasure:
            room->trap_num = rand() % 4;
            room->gold_num = rand() % 4;
            break;
        
        case Enchant:
            room->spell_num = rand() % 4;
            break;

        case Nightmare:
        case General:
            room->food_num = rand() % (4 - game->game_difficulty);
            room->gold_num = rand() % (4 - game->game_difficulty);
            room->gun_num = rand() % 2;
            room->spell_num = rand() % 2;
            room->trap_num = rand() % (2 + game->game_difficulty);
            room->enemy_num = rand() % (3 + game->game_difficulty);
            break;

    }


    // Food configuration
    room->foods = (Food **) calloc((room->food_num), sizeof(Food *));
    Food_Type ftype[7] = {Magical, Excellent, Ordinary, Magical, Ordinary, Excellent, Ordinary};
    for(int i = 0; i < room->food_num; i++) {
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
                        room->foods[i]->unicode = 0x0001F34E;
                        break;
                    case Excellent:
                        room->foods[i]->unicode = 0x0001F370;
                        break;
                    case Magical:
                        room->foods[i]->unicode = 0x0001F347;
                        break;
                    case Toxic:
                        // 1F344
                        //unicode
                        break;
                }
                room->foods[i]->time_passed = -1;
            }
        }
    }

    // Gold configuration
    room->golds = (Gold **) calloc((room->gold_num), sizeof(Gold *));
    Gold_Type Gtype[7] = {Regular, Regular, Regular, Regular, Black, Black, Regular};
    for(int i = 0; i < room->gold_num; i++) {
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
                        room->golds[i]->unicode = 0x0001F4B0;
                        break;
                    case Black:
                        room->golds[i]->unicode = 0x0001F6E2;
                        break;
                }
            }
        }
    }

    // Gun configuration
    room->guns = (Gun **) calloc((room->gun_num), sizeof(Gun *));
    Gun_Type gtype[4] = {Dagger, Magic_Wand, Normal_Arrow, Sword};
    for(int i = 0; i < room->gun_num; i++) {
        
        int gt = rand() % 4;
        room->guns[i] = (Gun *) calloc(1, sizeof(Gun));
        room->guns[i]->type = gtype[gt];
        bool flag = true;
        while(flag) {
            room->guns[i]->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room->guns[i]->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
            if(floor->map[room->guns[i]->location.y][room->guns[i]->location.x] == '.') {
                flag = false;
                floor->map[room->guns[i]->location.y][room->guns[i]->location.x] = 'U';
                switch(room->guns[i]->type) {
                    case Mace:
                        room->guns[i]->unicode = 0x0001F531;
                        break;
                    case Dagger:
                        room->guns[i]->damage = 12;
                        room->guns[i]->counter = 10;
                        strcpy(room->guns[i]->name, "Dagger");
                        room->guns[i]->distance = 5;
                        room->guns[i]->unicode = 0x0001F528;
                        break;
                    case Magic_Wand:
                        room->guns[i]->damage = 15;
                        room->guns[i]->counter = 8;
                        strcpy(room->guns[i]->name, "Magic_Wand");
                        room->guns[i]->distance = 10;
                        room->guns[i]->unicode = 0x0001F31F;
                        break;
                    case Normal_Arrow:
                        room->guns[i]->damage = 5;
                        room->guns[i]->counter = 20;
                        strcpy(room->guns[i]->name, "Normal_Arrow");
                        room->guns[i]->distance = 5;
                        room->guns[i]->unicode = 0x0001F3F9;
                        break;
                    case Sword:
                        room->guns[i]->damage = 10;
                        room->guns[i]->counter = 1;
                        strcpy(room->guns[i]->name, "Sword");
                        room->guns[i]->distance = 1;
                        room->guns[i]->unicode = 0x0001F5E1;
                        break;
                }
            }
        }
    }
    
    // Spell configuration
    room->spells = (Spell **) calloc((room->spell_num), sizeof(Spell *));
    Spell_Type stype[3] = {Health, Speed, Damage};
    for(int i = 0; i < room->spell_num; i++) {
        int st = rand() % 3;
        room->spells[i] = (Spell *) calloc(1, sizeof(Spell));
        room->spells[i]->type = stype[st];
        bool flag = true;
        while(flag) {
            room->spells[i]->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room->spells[i]->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
            if(floor->map[room->spells[i]->location.y][room->spells[i]->location.x] == '.') {
                flag = false;
                floor->map[room->spells[i]->location.y][room->spells[i]->location.x] = 'S';
                switch(room->spells[i]->type) {
                    case Health:
                        room->spells[i]->unicode = 0x0001F52E;
                        break;
                    case Speed:
                        room->spells[i]->unicode = 0x0001F680;
                        break;
                    case Damage:
                        room->spells[i]->unicode = 0x0001F4A5;
                        break;
                }
            }
        }
    }

    // Trap configuration
    room->traps = (Trap **) calloc((room->trap_num), sizeof(Trap *));
    for(int i = 0; i < room->trap_num; i++) {
        room->traps[i] = (Trap *) calloc(1, sizeof(Trap));
        bool flag = true;
        while(flag) {
            room->traps[i]->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room->traps[i]->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
            if(floor->map[room->traps[i]->location.y][room->traps[i]->location.x] == '.') {
                flag = false;
                floor->map[room->traps[i]->location.y][room->traps[i]->location.x] = '^';
            }
        }
    }

    // Enemy Configuration
    room->enemies = (Enemy **) calloc((room->enemy_num), sizeof(Enemy *));
    Enemy_Type etype[5] = {Deamon, /*Fire Breathing*/Monster, Giant, Snake, Undeed};
    for(int i = 0; i < room->enemy_num; i++) {
        int et = rand() % 5;
        room->enemies[i] = (Enemy *) calloc(1, sizeof(Enemy));
        room->enemies[i]->type = etype[et];
        bool flag = true;
        while(flag) {
            room->enemies[i]->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room->enemies[i]->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
            if(floor->map[room->enemies[i]->location.y][room->enemies[i]->location.x] == '.') {
                flag = false;
                switch(room->enemies[i]->type) {
                    case Deamon:
                        room->enemies[i]->damage = 1;
                        room->enemies[i]->health = 5;
                        room->enemies[i]->following = 0;
                        room->enemies[i]->chr = 'd';
                        room->enemies[i]->unicode = 0x0001F608;
                        break;
                    case Monster:
                        room->enemies[i]->damage = 2;
                        room->enemies[i]->health = 10;
                        room->enemies[i]->following = 0;
                        room->enemies[i]->chr = 'f';
                        room->enemies[i]->unicode = 0x0001F432;
                        break;
                    case Giant:
                        room->enemies[i]->damage = 3;
                        room->enemies[i]->health = 15;
                        room->enemies[i]->following = 5;
                        room->enemies[i]->chr = 'g';
                        room->enemies[i]->unicode = 0x0001F47B;
                        break;
                    case Snake:
                        room->enemies[i]->damage = 4;
                        room->enemies[i]->health = 20;
                        room->enemies[i]->following = 1000000;
                        room->enemies[i]->chr = 's';
                        room->enemies[i]->unicode = 0x0001F40D;
                        break;
                    case Undeed:
                        room->enemies[i]->damage = 6;
                        room->enemies[i]->health = 30;
                        room->enemies[i]->following = 5;
                        room->enemies[i]->chr = 'u';
                        room->enemies[i]->unicode = 0x0001F409;
                        break;
                }
                floor->map[room->enemies[i]->location.y][room->enemies[i]->location.x] = room->enemies[i]->chr;
            }
        }
    }

    // Staircase Configuration
    room->staircase = (Staircase *) calloc(1, sizeof(Staircase));
    if(room_num == floor->staircase_num) {
        bool flag = true;
        while(flag) {
            room->staircase->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room->staircase->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
            if(floor->map[room->staircase->location.y][room->staircase->location.x] == '.') {
                flag = false;
                floor->map[room->staircase->location.y][room->staircase->location.x] = '<';
            }
        }

    }
    else {
        room->staircase = NULL;
    }
    
    // Master Key Configuration
    room->master_key = (Master_Key *) calloc(1, sizeof(Master_Key));
    if(room_num == floor->master_key_num) {
        bool flag = true;
        while(flag) {
            room-> master_key->location.y = rand() % (room->size.y - 1) + room->start.y + 1; 
            room-> master_key->location.x = rand() % (room->size.x - 1) + room->start.x + 1; 
            if(floor->map[room->master_key->location.y][room->master_key->location.x] == '.') {
                flag = false;
                floor->map[room->master_key->location.y][room->master_key->location.x] = '!';
                room->master_key->unicode = 0x0001F511;
            }
        }
    }
    else {
        room->master_key = NULL;
    }

}

// Creating In_Room roads
void in_room_road(Floor *floor, int floor_num) {
    int room_locked_door = (floor_num == 3) ? (rand() % 5) : rand() % 6;
    for(int i = 1; i < 6; i++) {
        int door_counter = 0;
        if(floor_num == 3 && i == 5) {
            while(true) {
                int room = rand() % 5;
                int y = (rand() % (floor->Rooms[room].size.y - 1)) + floor->Rooms[room].start.y + 1; 
                int x = (rand() % (floor->Rooms[room].size.x - 1)) + floor->Rooms[room].start.x + 1; 
                if(floor->map[y][x] == '.') {
                    floor->map[y][x] = '{';
                    break;
                }
            }
            break;
        }

        Location current ={floor->Rooms[i - 1].start.x + (floor->Rooms[i - 1].size.x / 2), floor->Rooms[i - 1].start.y + (floor->Rooms[i - 1].size.y / 2)};
        Location end = {floor->Rooms[i].start.x + (floor->Rooms[i].size.x / 2), floor->Rooms[i].start.y + (floor->Rooms[i].size.y / 2)};
    

        // & Locked Door configuration
        int dx = end.x - current.x;
        int ctr = 0;
        if(dx > 0) {
            while(current.x < end.x) {
                if(floor->map[current.y][current.x] == ' ' || floor->map[current.y][current.x] == '#') 
                    floor->map[current.y][current.x] = '#';
                else if(((floor->map[current.y][current.x] == '_' || floor->map[current.y][current.x] == '@') && floor->map[current.y][current.x - 1] == '#') || (floor->map[current.y][current.x] == '|')) {
                    if((floor->map[current.y][current.x] == '_' && floor->map[current.y][current.x - 1] == '#')) {
                        if(current.y > 0 && (floor->map[current.y - 1][current.x] == '|' || floor->map[current.y - 1][current.x] == '+'))
                            current.y--, floor->map[current.y][current.x - 1] = '#';
                        else if(current.y < 39 && (floor->map[current.y + 1][current.x] == '|' || floor->map[current.y - 1][current.x] == '+'))
                            current.y++, floor->map[current.y][current.x - 1] = '#';
                    }

                    if(i - 1 == room_locked_door && door_counter == 0) {
                        floor->Rooms[i - 1].locked_door = (Locked_Door *) calloc(1, sizeof(Locked_Door));
                        floor->map[current.y][current.x] = '@';
                        floor->Rooms[i - 1].locked_door->location.y = current.y, floor->Rooms[i - 1].locked_door->location.x = current.x;
                        floor->map[floor->Rooms[i - 1].start.y][floor->Rooms[i - 1].start.x] = '&';
                        floor->Rooms[i - 1].locked_door->password = -1;
                        door_counter++;
                        continue;
                    }

                    floor->map[current.y][current.x] = '+';
                    bool flag = false;
                    for(int j = 0; j < 6 && !flag; j++) {
                        if(current.x >= floor->Rooms[j].start.x && current.x < floor->Rooms[j].start.x + floor->Rooms[j].size.x && current.y >= floor->Rooms[j].start.y && current.y < floor->Rooms[j].start.y + floor->Rooms[j].size.y) {
                            floor->Rooms[j].normal_doors_num++;
                            floor->Rooms[j].normal_doors = (Normal_Door *) realloc(floor->Rooms[j].normal_doors, floor->Rooms[j].normal_doors_num * sizeof(Normal_Door));
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.x = current.x;
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.y = current.y;
                            flag = true;
                        }
                    }
                    door_counter++;
                }
                current.x++;
                ctr++;
                if(ctr % 8 == 0 && floor->map[current.y][current.x - 1] != '+') {
                    
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
                else if(((floor->map[current.y][current.x] == '_' || floor->map[current.y][current.x] == '@') && (floor->map[current.y][current.x + 1] == '#')) || (floor->map[current.y][current.x] == '|'))  {
                    if((floor->map[current.y][current.x] == '_' && floor->map[current.y][current.x + 1] == '#')) {
                        if(current.y > 0 && (floor->map[current.y - 1][current.x] == '|' || floor->map[current.y - 1][current.x] == '+'))
                            current.y--, floor->map[current.y][current.x + 1] = '#';
                        else if(current.y < 39 && (floor->map[current.y - 1][current.x] == '|' || floor->map[current.y - 1][current.x] == '+'))
                            current.y++, floor->map[current.y][current.x + 1] = '#';
                    }

                    if(i - 1 == room_locked_door && door_counter == 0) {
                        floor->Rooms[i - 1].locked_door = (Locked_Door *) calloc(1, sizeof(Locked_Door));
                        floor->map[current.y][current.x] = '@';
                        floor->Rooms[i - 1].locked_door->location.y = current.y, floor->Rooms[i - 1].locked_door->location.x = current.x;
                        floor->map[floor->Rooms[i - 1].start.y][floor->Rooms[i - 1].start.x] = '&';
                        floor->Rooms[i - 1].locked_door->password = -1;
                        door_counter++;
                        continue;
                    }

                    floor->map[current.y][current.x] = '+';
                    bool flag = false;
                    for(int j = 0; j < 6 && !flag; j++) {
                        if(current.x >= floor->Rooms[j].start.x && current.x < floor->Rooms[j].start.x + floor->Rooms[j].size.x && current.y >= floor->Rooms[j].start.y && current.y < floor->Rooms[j].start.y + floor->Rooms[j].size.y) {
                            floor->Rooms[j].normal_doors_num++;
                            floor->Rooms[j].normal_doors = (Normal_Door *) realloc(floor->Rooms[j].normal_doors, floor->Rooms[j].normal_doors_num * sizeof(Normal_Door));
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.x = current.x;
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.y = current.y;
                            flag = true;
                        }
                    }
                    door_counter++;
                }
                current.x--;
                ctr++;
                if(ctr % 8 == 0 && floor->map[current.y][current.x + 1] != '+') {
                    int r = rand() % 2;
                    if(r == 0 && current.y < 39 && floor->map[current.y + 1][current.x] != '_' && floor->map[current.y + 1][current.x] != '|')
                        current.y++;
                    else if(r == 1 && current.y > 0 && floor->map[current.y - 1][current.x] != '_' && floor->map[current.y - 1][current.x] != '|')
                        current.y--;
                    current.x++;
                }
            }
        }

        int dy = end.y - current.y;
        if(dy > 0) {
            while(current.y < end.y) {
                if(floor->map[current.y][current.x] == ' ' || floor->map[current.y][current.x] == '#') 
                    floor->map[current.y][current.x] = '#';
                else if(floor->map[current.y][current.x] == '_' || floor->map[current.y][current.x] == '+') {

                    if(i - 1 == room_locked_door && door_counter == 0) {
                        floor->Rooms[i - 1].locked_door = (Locked_Door *) calloc(1, sizeof(Locked_Door));
                        floor->map[current.y][current.x] = '@';
                        floor->Rooms[i - 1].locked_door->location.y = current.y, floor->Rooms[i - 1].locked_door->location.x = current.x;
                        floor->map[floor->Rooms[i - 1].start.y][floor->Rooms[i - 1].start.x] = '&';
                        floor->Rooms[i - 1].locked_door->password = -1;
                        door_counter++;
                        continue;
                    }

                    floor->map[current.y][current.x] = '+';
                    bool flag = false;
                    for(int j = 0; j < 6 && !flag; j++) {
                        if(current.x >= floor->Rooms[j].start.x && current.x < floor->Rooms[j].start.x + floor->Rooms[j].size.x && current.y >= floor->Rooms[j].start.y && current.y < floor->Rooms[j].start.y + floor->Rooms[j].size.y) {
                            floor->Rooms[j].normal_doors_num++;
                            floor->Rooms[j].normal_doors = (Normal_Door *) realloc(floor->Rooms[j].normal_doors, floor->Rooms[j].normal_doors_num * sizeof(Normal_Door));
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.x = current.x;
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.y = current.y;
                            flag = true;
                        }
                    }
                    door_counter++;
                }
                current.y++;
            }
        }
        else if(dy < 0) {
            while(current.y > end.y) {
                if(floor->map[current.y][current.x] == ' ' || floor->map[current.y][current.x] == '#') 
                    floor->map[current.y][current.x] = '#';
                else if(floor->map[current.y][current.x] == '_' || floor->map[current.y][current.x] == '+') {

                    if(i - 1 == room_locked_door && door_counter == 0) {
                        floor->Rooms[i - 1].locked_door = (Locked_Door *) calloc(1, sizeof(Locked_Door));
                        floor->map[current.y][current.x] = '@';
                        floor->Rooms[i - 1].locked_door->location.y = current.y, floor->Rooms[i - 1].locked_door->location.x = current.x;
                        floor->map[floor->Rooms[i - 1].start.y][floor->Rooms[i - 1].start.x] = '&';
                        floor->Rooms[i - 1].locked_door->password = -1;
                        door_counter++;
                        continue;
                    }

                    floor->map[current.y][current.x] = '+';
                    bool flag = false;
                    for(int j = 0; j < 6 && !flag; j++) {
                        if(current.x >= floor->Rooms[j].start.x && current.x < floor->Rooms[j].start.x + floor->Rooms[j].size.x && current.y >= floor->Rooms[j].start.y && current.y < floor->Rooms[j].start.y + floor->Rooms[j].size.y) {
                            floor->Rooms[j].normal_doors_num++;
                            floor->Rooms[j].normal_doors = (Normal_Door *) realloc(floor->Rooms[j].normal_doors, floor->Rooms[j].normal_doors_num * sizeof(Normal_Door));
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.x = current.x;
                            floor->Rooms[j].normal_doors[floor->Rooms[j].normal_doors_num - 1].location.y = current.y;
                            flag = true;
                        }
                    }
                    door_counter++;
                }
                current.y--;
            }
        }
    }
}

// Playing game
void play_game(Game *game) {

    // game_won(game, player);

    //Initial Setups
    clear_space2();
    WINDOW *game_window = newwin(40, 146, 1, 1);
    int time_passed = 0;
    // srand(time(NULL));

    // Music
    if(strcmp(game->music->music_path, "NULL") != 0) {
        pthread_create(&sound_thread, NULL, play_sound, (void *) game->music->music_path);
    }

    // facets
    bool flag = true;
    bool is_take_element = true;  
    bool is_g = false;  
    bool is_f = false;
    bool is_moved = false;
    bool is_unicode = true;
    int save_shot[2] = {-1, -1};
    int dir = -1;
    while(flag) {
        paint_floor(game, &game->floors[game->player_floor], game_window, time_passed, is_unicode);            

        is_moved = false;
        int c = getch();
        switch(c) {
            // moves
            case 'j':
                if(is_f == true) {
                    while(game->player_location.y > 0 && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != ' ' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != '&') {
                        game->player_location.y--;    
                    }
                    is_f = false;
                }
                else if(game->speed_up == true) {
                    if(game->player_location.y > 0 && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != ' ' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != '&') {
                        game->player_location.y--;    
                    }
                    if(game->player_location.y > 0 && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != ' ' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != '&') {
                        game->player_location.y--;    
                    }
                }
                else if(game->player_location.y > 0 && 
                game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != '_' && 
                game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != '|' && 
                game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x] != ' ') {
                    game->player_location.y--;
                }
                else
                    beep();

                if(is_g == true) {
                    is_g = false;
                    is_take_element = false;
                }
                is_moved = true;
                dir = 0;
                break;

            case 'k':
                if(is_f == true) {
                    while(game->player_location.y < 39 && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != ' ' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '&') {
                        game->player_location.y++;
                    }
                    is_f = false;    
                }
                else if(game->speed_up == true){
                    if(game->player_location.y < 39 && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != ' ' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '&') {
                        game->player_location.y++;
                    }
                    if(game->player_location.y < 39 && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != ' ' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '&') {
                        game->player_location.y++;
                    }
                }
                else if(game->player_location.y < 39 && 
                game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '_' && 
                game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != '|' && 
                game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x] != ' ') {
                    game->player_location.y++;
                }
                else
                    beep();

                if(is_g == true) {
                    is_g = false;
                    is_take_element = false;
                }
                is_moved = true;
                dir = 4;
                break;

            case  'l':
                if(is_f == true) {
                    while(game->player_location.x < 145 && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != ' ' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '&') {
                        game->player_location.x++;
                    }
                    is_f = false;
                }
                else if(game->speed_up == true) {
                    if(game->player_location.x < 145 && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != ' ' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '&') {
                        game->player_location.x++;
                    }
                    if(game->player_location.x < 145 && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != ' ' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '&') {
                        game->player_location.x++;
                    }
                }
                else if(game->player_location.x < 145 && 
                game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '|' && 
                game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != '_' && 
                game->floors[game->player_floor].map[game->player_location.y][game->player_location.x + 1] != ' ') {
                    game->player_location.x++;
                }
                else
                    beep();

                if(is_g == true) {
                    is_g = false;
                    is_take_element = false;
                }
                is_moved = true;
                dir = 2;
                break;
            
            case 'h':
                if(is_f == true) {
                    while(game->player_location.x > 0 && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != ' ' &&
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '&') {
                        game->player_location.x--;
                    }
                    is_f = false;
                }
                else if(game->speed_up == true) {
                    if(game->player_location.x > 0 && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != ' ' &&
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '&') {
                        game->player_location.x--;
                    }
                    if(game->player_location.x > 0 && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != ' ' &&
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '&') {
                        game->player_location.x--;
                    }
                }
                else if(game->player_location.x > 0 && 
                game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '|' && 
                game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != '_' && 
                game->floors[game->player_floor].map[game->player_location.y][game->player_location.x - 1] != ' ') {
                    game->player_location.x--;
                }
                else
                    beep();

                if(is_g == true) {
                    is_g = false;
                    is_take_element = false;
                }
                is_moved = true;
                dir = 6;
                break;

            case 'y':
                if(is_f == true) {
                    while(game->player_location.x > 0 && game->player_location.y > 0 && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != ' ' &&
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '&') {
                        game->player_location.x--, game->player_location.y--;
                    }
                    is_f = false;
                }
                else if(game->speed_up == true) {
                    if(game->player_location.x > 0 && game->player_location.y > 0 && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != ' ' &&
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '&') {
                        game->player_location.x--, game->player_location.y--;
                    }
                    if(game->player_location.x > 0 && game->player_location.y > 0 && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != ' ' &&
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '&') {
                        game->player_location.x--, game->player_location.y--;
                    }
                }
                else if(game->player_location.x > 0 && game->player_location.y > 0 && 
                game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '_' && 
                game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != '|' && 
                game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x - 1] != ' ') {
                    game->player_location.x--, game->player_location.y--;
                }
                else
                    beep();

                if(is_g == true) {
                    is_g = false;
                    is_take_element = false;
                }
                is_moved = true;
                dir = 7;
                break;

            case 'u':
                if(is_f == true) {
                    while(game->player_location.x < 145 && game->player_location.y > 0 && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != ' ' &&
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '&') {
                        game->player_location.x++, game->player_location.y--;
                    }
                    is_f = false;
                }
                else if(game->speed_up == true) {
                    if(game->player_location.x < 145 && game->player_location.y > 0 && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != ' ' &&
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '&') {
                        game->player_location.x++, game->player_location.y--;
                    }
                    if(game->player_location.x < 145 && game->player_location.y > 0 && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != ' ' &&
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '&') {
                        game->player_location.x++, game->player_location.y--;
                    }
                }
                else if(game->player_location.x < 145 && game->player_location.y > 0 && 
                game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '_' && 
                game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != '|' && 
                game->floors[game->player_floor].map[game->player_location.y - 1][game->player_location.x + 1] != ' ') {
                    game->player_location.x++, game->player_location.y--;
                }
                else
                    beep();

                if(is_g == true) {
                    is_g = false;
                    is_take_element = false;
                }
                is_moved = true;
                dir = 1;
                break;
            
            case 'b':
                if(is_f == true) {
                    while(game->player_location.x > 0 && game->player_location.y < 39 && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != ' ' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '&') {
                        game->player_location.x--, game->player_location.y++;
                    }
                    is_f = false;
                }
                else if(game->speed_up == true) {
                    if(game->player_location.x > 0 && game->player_location.y < 39 && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != ' ' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '&') {
                        game->player_location.x--, game->player_location.y++;
                    }
                    if(game->player_location.x > 0 && game->player_location.y < 39 && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != ' ' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '&') {
                        game->player_location.x--, game->player_location.y++;
                    }
                }
                else if(game->player_location.x > 0 && game->player_location.y < 39 && 
                game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '_' && 
                game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != '|' && 
                game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x - 1] != ' ') {
                    game->player_location.x--, game->player_location.y++;
                }
                else
                    beep();

                if(is_g == true) {
                    is_g = false;
                    is_take_element = false;
                }
                is_moved = true;
                dir = 5;
                break;
            
            case 'n':
                if(is_f == true) {
                    while(game->player_location.x < 145 && game->player_location.y < 39 && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != ' ' &&
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '&') {
                        game->player_location.x++, game->player_location.y++;
                    }
                    is_f = false;
                }
                else if(game->speed_up == true) {
                    if(game->player_location.x < 145 && game->player_location.y < 39 && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != ' ' &&
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '&') {
                        game->player_location.x++, game->player_location.y++;
                    }
                    if(game->player_location.x < 145 && game->player_location.y < 39 && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '_' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '|' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != ' ' &&
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '@' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '+' && 
                    game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '&') {
                        game->player_location.x++, game->player_location.y++;
                    }
                }
                else if(game->player_location.x < 145 && game->player_location.y < 39 && 
                game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '_' && 
                game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != '|' && 
                game->floors[game->player_floor].map[game->player_location.y + 1][game->player_location.x + 1] != ' ') {
                    game->player_location.x++, game->player_location.y++;
                }
                else
                    beep();

                if(is_g == true) {
                    is_g = false;
                    is_take_element = false;
                }
                is_moved = true;
                dir = 3;
                break;

            // see whole map
            case 'm':
                whole_game(game, &game->floors[game->player_floor], is_unicode);
                break;

            // move in one directin
            case 'f':
                is_f = true;
                break;
            
            // not taking enchant or gun
            case 'g':
                is_g = true;
                break;

            // search for neighbors for trap or secret door
            case 's':
                search_map(game);
                break;

            // go down stairs
            case '>':
                if(game->floors[game->player_floor].map[game->player_location.y][game->player_location.x] == '<' && game->player_floor < 3) {
                    game->player_floor++;
                    if(game->floors[game->player_floor].floor_visit == true) {
                        game->player_room = game->floors[game->player_floor].staircase_num;
                        game->player_location = game->floors[game->player_floor].Rooms[game->floors[game->player_floor].staircase_num].staircase->location;
                    }
                    else {
                        game->floors[game->player_floor].floor_visit = true;
                        int room;
                        while(true) {
                            if(game->player_floor == 3)
                                room = rand() % 5;
                            else
                                room = rand() % 6;
                            if(room != game->floors[game->player_floor].staircase_num)
                                break;
                        }
                        game->player_room = room;
                        Room *temp_room = &game->floors[game->player_floor].Rooms[game->player_room]; 
                        for(int ii = temp_room->start.y; ii < temp_room->start.y + temp_room->size.y; ii++) {
                            for(int jj = temp_room->start.x; jj < temp_room->start.x + temp_room->size.x; jj++) {
                                game->floors[game->player_floor].visit[ii][jj] = true;
                            }
                        }

                        while(true) {
                            game->player_location.y = rand() % (temp_room->size.y - 1) + temp_room->start.y + 1; 
                            game->player_location.x = rand() % (temp_room->size.x - 1) + temp_room->start.x + 1; 
                            if(game->floors[game->player_floor].map[game->player_location.y][game->player_location.x] == '.') {
                                break;
                            }
                        }
                    }

                    if(game->floors[game->player_floor].Rooms[game->player_room].type == Enchant) {
                        music_changed = true;
                        kill(afplay_pid, SIGTERM);
                        pthread_create(&sound_thread, NULL, play_sound, (void *) "../music/06CommandingtheFury.mp3");
                    }
                    else if(game->floors[game->player_floor].Rooms[game->player_room].type == Nightmare) {
                        music_changed = true;
                        kill(afplay_pid, SIGTERM);
                        pthread_create(&sound_thread, NULL, play_sound, (void *) "../music/12TheNightingale.mp3");
                    }
                    else {
                        if(music_changed == true) {
                            kill(afplay_pid, SIGTERM);
                            if(strcmp(game->music->music_path, "NULL") != 0) {
                                pthread_create(&sound_thread, NULL, play_sound, (void *) game->music->music_path);
                            }
                            music_changed = false;
                        }
                    }
                }
                else
                    beep();
                break;

            // go up stairs
            case '<':
                if(game->floors[game->player_floor].map[game->player_location.y][game->player_location.x] == '<' && game->player_floor > 0) {
                    game->player_floor--;
                    game->player_location = game->floors[game->player_floor].Rooms[game->floors[game->player_floor].staircase_num].staircase->location;
                    game->player_room = game->floors[game->player_floor].staircase_num;

                    if(game->floors[game->player_floor].Rooms[game->player_room].type == Enchant) {
                        music_changed = true;
                        kill(afplay_pid, SIGTERM);
                        pthread_create(&sound_thread, NULL, play_sound, (void *) "../music/06CommandingtheFury.mp3");
                    }
                    else if(game->floors[game->player_floor].Rooms[game->player_room].type == Nightmare) {
                        music_changed = true;
                        kill(afplay_pid, SIGTERM);
                        pthread_create(&sound_thread, NULL, play_sound, (void *) "../music/12TheNightingale.mp3");
                    }
                    else {
                        if(music_changed == true) {
                            kill(afplay_pid, SIGTERM);
                            if(strcmp(game->music->music_path, "NULL") != 0) {
                                pthread_create(&sound_thread, NULL, play_sound, (void *) game->music->music_path);
                            }
                            music_changed = false;
                        }
                    }
                }
                else
                    beep();
                break;

            // menu for foods to eat
            case 'e':
                foods_menu(game, time_passed);
                break;

            // munu for guns to take
            case 'i':
                guns_menu(game);
                break;

            // menu for spells to use
            case 'p':
                spells_menu(game, time_passed);
                break;

            // help page
            case 'x':
                help_page(game);
                break;

            // for using gun
            case ' ':
                // use bool power_up to double the damage
                hit_enemy(game, save_shot, false);
                break;

            // for repeating gun usage again
            case 'a':
                hit_enemy(game, save_shot, true);
                break;

            // for unicode representing
            case 'c':
                is_unicode ^= true;

                break;
            // quit and save
            case 'q':
                flag = false;
                break;
        }

        //! actions after moving 

        if(is_moved == true) {
            if(is_take_element == true)
                action_game(game, dir, time_passed);
            else 
                is_take_element = true;

            Room *temp_room;
            int ii = 1;
            switch(game->floors[game->player_floor].map[game->player_location.y][game->player_location.x]) {
                // roads
                case '#':
                    // expand road for player
                    while(ii < 6) {
                        int y = game->player_location.y + (ii * directions[dir][1]), x = game->player_location.x + (ii * directions[dir][0]);
                        if(is_in_map(y, x) && game->floors[game->player_floor].map[y][x] == '#')
                            game->floors[game->player_floor].visit[y][x] = true;
                        else 
                            ii = 1000;

                        ii++;
                    }

                    // set player_room in road to -1
                    if(game->floors[game->player_floor].map[game->player_location.y - directions[dir][1]][game->player_location.x - directions[dir][0]] == '+' || game->floors[game->player_floor].map[game->player_location.y - directions[dir][1]][game->player_location.x - directions[dir][0]] == '@' || game->floors[game->player_floor].map[game->player_location.y - directions[dir][1]][game->player_location.x - directions[dir][0]] == '*') {
                        game->player_room = -1;
                        if(music_changed == true) {
                            kill(afplay_pid, SIGTERM);
                            if(strcmp(game->music->music_path, "NULL") != 0) {
                                pthread_create(&sound_thread, NULL, play_sound, (void *) game->music->music_path);
                            }
                            music_changed = false;
                        }
                    }
                    break;

                // Normal doors
                case '+':
                    // locate new room for player_room
                    if(game->player_room == -1) {
                        for(int iii = 0; iii < 6 && game->player_room == -1; iii++) {
                            for(int jj = 0; jj < game->floors[game->player_floor].Rooms[iii].normal_doors_num && game->player_room == -1; jj++) {
                                if(game->player_location.y == game->floors[game->player_floor].Rooms[iii].normal_doors[jj].location.y && game->player_location.x == game->floors[game->player_floor].Rooms[iii].normal_doors[jj].location.x) {
                                    game->player_room = iii;
                                }
                            }
                        }
                        refresh_enemy_following(game);
                        if(game->floors[game->player_floor].Rooms[game->player_room].type == Enchant) {
                            music_changed = true;
                            kill(afplay_pid, SIGTERM);
                            pthread_create(&sound_thread, NULL, play_sound, (void *) "../music/06CommandingtheFury.mp3");
                        }
                        else if(game->floors[game->player_floor].Rooms[game->player_room].type == Nightmare) {
                            music_changed = true;
                            kill(afplay_pid, SIGTERM);
                            pthread_create(&sound_thread, NULL, play_sound, (void *) "../music/12TheNightingale.mp3");
                        }
                    }

                    // show new room
                    temp_room = &game->floors[game->player_floor].Rooms[game->player_room]; 
                    for(int iii = temp_room->start.y; iii < temp_room->start.y + temp_room->size.y; iii++) {
                        for(int jj = temp_room->start.x; jj < temp_room->start.x + temp_room->size.x; jj++) {
                            game->floors[game->player_floor].visit[iii][jj] = true;
                        }
                    }
                    break;

                // Secret door
                case '*':
                    // locate new room for player_room
                    if(game->player_room == -1) {
                        for(int iii = 0; iii < 6 && game->player_room == -1; iii++) {
                            for(int jj = 0; jj < game->floors[game->player_floor].Rooms[iii].secret_doors_num && game->player_room == -1; jj++) {
                                if(game->player_location.y == game->floors[game->player_floor].Rooms[iii].secret_doors[jj].location.y && game->player_location.x == game->floors[game->player_floor].Rooms[iii].secret_doors[jj].location.x) {
                                    game->player_room = iii;
                                }
                            }
                        }
                        refresh_enemy_following(game);
                        if(game->floors[game->player_floor].Rooms[game->player_room].type == Enchant) {
                            music_changed = true;
                            kill(afplay_pid, SIGTERM);
                            pthread_create(&sound_thread, NULL, play_sound, (void *) "../music/06CommandingtheFury.mp3");
                        }
                        else if(game->floors[game->player_floor].Rooms[game->player_room].type == Nightmare) {
                            music_changed = true;
                            kill(afplay_pid, SIGTERM);
                            pthread_create(&sound_thread, NULL, play_sound, (void *) "../music/12TheNightingale.mp3");
                        }
                    }

                    // show new room
                    temp_room = &game->floors[game->player_floor].Rooms[game->player_room]; 
                    for(int iii = temp_room->start.y; iii < temp_room->start.y + temp_room->size.y; iii++) {
                        for(int jj = temp_room->start.x; jj < temp_room->start.x + temp_room->size.x; jj++) {
                            game->floors[game->player_floor].visit[iii][jj] = true;
                        }
                    }
                    break;
                
                // Locked door
                case '@':
                    // locate new room for player_room
                    if(game->player_room == -1) {
                        for(int iii = 0; iii < 6 && game->player_room == -1; iii++) {
                            for(int jj = 0; jj < 1 && game->player_room == -1; jj++) {
                                if(game->floors[game->player_floor].Rooms[iii].locked_door != NULL && game->player_location.y == game->floors[game->player_floor].Rooms[iii].locked_door->location.y && game->player_location.x == game->floors[game->player_floor].Rooms[iii].locked_door->location.x) {
                                    game->player_room = iii;
                                }
                            }
                            refresh_enemy_following(game);
                            if(game->floors[game->player_floor].Rooms[game->player_room].type == Enchant) {
                                music_changed = true;
                                kill(afplay_pid, SIGTERM);
                                pthread_create(&sound_thread, NULL, play_sound, (void *) "../music/06CommandingtheFury.mp3");
                            }
                            else if(game->floors[game->player_floor].Rooms[game->player_room].type == Nightmare) {
                                music_changed = true;
                                kill(afplay_pid, SIGTERM);
                                pthread_create(&sound_thread, NULL, play_sound, (void *) "../music/12TheNightingale.mp3");
                            }
                        }

                        // show new room
                        temp_room = &game->floors[game->player_floor].Rooms[game->player_room]; 
                        for(int iii = temp_room->start.y; iii < temp_room->start.y + temp_room->size.y; iii++) {
                            for(int jj = temp_room->start.x; jj < temp_room->start.x + temp_room->size.x; jj++) {
                                game->floors[game->player_floor].visit[iii][jj] = true;
                            }
                        }
                        break;
                    }

                    if(game->floors[game->player_floor].Rooms[game->player_room].locked_door->is_visited == true)
                        break;

                    echo();
                    curs_set(TRUE);
                    game->player_location.y -= directions[dir][1];
                    game->player_location.x -= directions[dir][0];
                    if(game->floors[game->player_floor].Rooms[game->player_room].locked_door->password_turn < 3) {
                        move(0, 1);
                        attron(COLOR_PAIR(2) | A_BOLD);
                        char pass[100];
                        addstr("Please enter your Password: ");
                        attroff(A_BOLD);
                        getstr(pass);
                        int password = strtol(pass, NULL, 10);

                        for(int i = 0; i < 146; i++) {
                            move(0, i);
                            addch(' ');
                        }  
                        attroff(COLOR_PAIR(2));
                        move(0, 1);

                        game->floors[game->player_floor].Rooms[game->player_room].locked_door->password_turn++;
                        if(password == game->floors[game->player_floor].Rooms[game->player_room].locked_door->password) {
                            game->floors[game->player_floor].Rooms[game->player_room].locked_door->is_visited = true;
                            attron(COLOR_PAIR(16) | A_BOLD);
                            addstr("Congratulations! Your Password was correct! The door is unlocked! Press any key to continue...");
                            attroff(COLOR_PAIR(16) | A_BOLD);
                        }
                        else {
                            switch(game->floors[game->player_floor].Rooms[game->player_room].locked_door->password_turn) {
                                case 1:
                                    attron(COLOR_PAIR(18) | A_BOLD);
                                    addstr("Oops... Your Password is incorrect! You have 2 shots left! Press any key to continue...");
                                    attroff(COLOR_PAIR(18) | A_BOLD);
                                    break;

                                case 2:
                                    attron(COLOR_PAIR(17) | A_BOLD);
                                    addstr("Oops... Your Password is incorrect! You have 1 shots left! Press any key to continue...");
                                    attroff(COLOR_PAIR(17) | A_BOLD);
                                    break;

                                case 3:
                                    attron(COLOR_PAIR(3) | A_BOLD);
                                    addstr("Oops... Your Password is incorrect! You have no shots left! The door is locked forever! Press any key to continue...");
                                    attroff(COLOR_PAIR(3) | A_BOLD);
                                    break;
                            }
                        }

                        getch();

                        attron(COLOR_PAIR(2) | A_BOLD);
                        for(int i = 0; i < 146; i++) {
                            move(0, i);
                            addch(' ');
                        }  
                        attroff(COLOR_PAIR(2) | A_BOLD);
                        noecho();
                        curs_set(FALSE);

                    }
                    else {
                        if(game->master_key_num > 0) {
                            move(0, 1);
                            attron(COLOR_PAIR(3) | A_BOLD);
                            addstr("Do you want to use Master_Key to unlock the door?[y/n] ");
                            echo();
                            curs_set(TRUE);
                            int c = getch();
                            for(int i = 0; i < 146; i++) {
                                move(0, i);
                                addch(' ');
                            }  
                            attroff(COLOR_PAIR(3) | A_BOLD);
                            
                            move(0, 1);
                            if(c == 'y') {
                                attron(COLOR_PAIR(16) | A_BOLD);
                                addstr("The door is get unlocked! Press any key to continue...");
                                game->floors[game->player_floor].Rooms[game->player_room].locked_door->is_visited = true;
                                game->master_key_num--;
                                attroff(COLOR_PAIR(16) | A_BOLD);
                            }
                            else {
                                attron(COLOR_PAIR(3) | A_BOLD);
                                addstr("OK! The door is locked! Press any key to continue...");
                                attroff(COLOR_PAIR(3) | A_BOLD);                            
                            }
                            getch();
                            
                            noecho();
                            curs_set(FALSE);
                        }
                        else {
                            move(0, 1);
                            attron(COLOR_PAIR(3) | A_BOLD);
                            addstr("You have tried your 3 shots to unlock the door, Press any key to continue...");

                            getch();

                            for(int i = 0; i < 146; i++) {
                                move(0, i);
                                addch(' ');
                            }    
                            attroff(COLOR_PAIR(3) | A_BOLD);
                        }
                    }
                    break;

                // Password generator
                case '&':
                    game->player_location.y -= directions[dir][1];
                    game->player_location.x -= directions[dir][0];
                    
                    if(game->floors[game->player_floor].Rooms[game->player_room].locked_door->is_visited == true)
                        break;
        
                    if(game->floors[game->player_floor].Rooms[game->player_room].locked_door->password_turn < 3) {
                        password_generator(game, game->floors[game->player_floor].Rooms[game->player_room].locked_door, dir);
                    }
                    else {
                        move(0, 1);
                        attron(COLOR_PAIR(3) | A_BOLD);
                        addstr("You have tried your 3 shots to unlock the door, Press any key to continue...");

                        getch();

                        for(int i = 0; i < 146; i++) {
                            move(0, i);
                            addch(' ');
                        }    
                        attroff(COLOR_PAIR(3) | A_BOLD);
                    }
                    break;

                case '{':
                    game->player_location.x = game->floors[game->player_floor].Rooms[5].start.x + (game->floors[game->player_floor].Rooms[5].size.x / 2);
                    game->player_location.y = game->floors[game->player_floor].Rooms[5].start.y + (game->floors[game->player_floor].Rooms[5].size.y / 2);
                    
                    // show new room
                    temp_room = &game->floors[game->player_floor].Rooms[5]; 
                    for(int iii = temp_room->start.y; iii < temp_room->start.y + temp_room->size.y; iii++) {
                        for(int jj = temp_room->start.x; jj < temp_room->start.x + temp_room->size.x; jj++) {
                            game->floors[game->player_floor].visit[iii][jj] = true;
                        }
                    }
                    game->player_room = 5;
                    break;
                
                // Traps
                case '^':
                    game->Health -= 20;
                    move(0, 1);
                    attron(COLOR_PAIR(3) | A_BOLD);
                    addstr("Trap decreased your Health by 20! Press any key to continue...");

                    getch();
                    for(int i = 0; i < 146; i++) {
                        move(0, i);
                        addch(' ');
                    }    

                    attroff(COLOR_PAIR(3) | A_BOLD);
                    break;
                
                // Master_Key
                case '!':
                    game->master_key_num++;
                    game->master_key = (Master_Key **) realloc(game->master_key, (game->master_key_num) * sizeof(Master_Key *));
                    game->master_key[game->master_key_num - 1] = game->floors[game->player_floor].Rooms[game->player_room].master_key;
                    game->floors[game->player_floor].map[game->player_location.y][game->player_location.x] = '.';
                    game->floors[game->player_floor].Rooms[game->player_room].master_key = NULL;
                    move(0, 1);
                    attron(COLOR_PAIR(18) | A_BOLD);
                    addstr("You've got a Master_Key, Press any key to continue...");

                    int c = getch();
                    for(int i = 0; i < 146; i++) {
                        move(0, i);
                        addch(' ');
                    }    

                    attroff(COLOR_PAIR(18) | A_BOLD);
                    break;
                
                default:
                    break;
            }
        } 

        if(game->Health <= 0) {
            game_lost(game);
            break;
        }
        if(game->player_room == 5 && game->player_floor == 3) {
            bool flag = true;
            for(int i = 0; flag == true && i < game->floors[game->player_floor].Rooms[game->player_room].enemy_num; i++) {
                if(game->floors[game->player_floor].Rooms[game->player_room].enemies[i] != NULL) {
                    flag = false;
                }
            }

            if(flag == true) {
                game_won(game, player);
                break;
            }
        }
        time_passed++;
        
    }

    // Music end
    if(strcmp(game->music->music_path, "NULL") != 0) {
        kill(afplay_pid, SIGTERM);
        afplay_pid = -1;
    }
    // pthread_cancel(sound_thread); 
    // pthread_join(sound_thread, NULL); 

    delwin(game_window);
}

// paint_floor_lapse
void paint_floor(Game *game, Floor *floor, WINDOW *game_window, int time_passed, bool is_unicode) {
    noecho();
    curs_set(FALSE);
    clear_space2();
    wclear(game_window);

    // Logics
    if(time_passed % 15 == 0 && time_passed != 0)
        game->Health -= 1;
    if(game->time_power != -1 && (time_passed - game->time_power) > 10) {
        game->power_up = false;
        game->speed_up = false;
        game->time_power = -1;
    }

    // Enemy Movement
    if(game->player_room != -1) {
        for(int i = 0; i < game->floors[game->player_floor].Rooms[game->player_room].enemy_num; i++) {
            if(game->floors[game->player_floor].Rooms[game->player_room].enemies[i] != NULL) {
                int dy = game->player_location.y - game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.y;
                int dx = game->player_location.x - game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.x;

                if(abs(dy) <= 1 && abs(dx) <= 1) {
                    game->Health -= game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->damage;
                }
                else if(game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->following > 0) {
                    game->floors[game->player_floor].map[game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.y][game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.x] = '.';
                    if(abs(dy) > 1) {
                        if(dy > 1) {
                            while(true) {
                                game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.y++;
                                if(game->floors[game->player_floor].map[game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.y][game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.x] == '.')
                                    break;
                            }
                        }
                        else    
                            while(true) {
                                game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.y--;
                                if(game->floors[game->player_floor].map[game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.y][game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.x] == '.')
                                    break;
                            }
                    }
                    else {
                        if(dx > 1)
                            while(true) {
                                game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.x++;
                                if(game->floors[game->player_floor].map[game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.y][game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.x] == '.')
                                    break;
                            }
                        else
                            while(true) {
                                game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.x--;
                                if(game->floors[game->player_floor].map[game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.y][game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.x] == '.')
                                    break;
                            }
                    }
                    game->floors[game->player_floor].map[game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.y][game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->location.x] = game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->chr;
                    game->floors[game->player_floor].Rooms[game->player_room].enemies[i]->following--;
                }
            }
        }
    }

    for(int i = 0; i < 40; i++) {
        for(int j = 0; j < 146; j++) {
            int color_num;
            if(floor->visit[i][j] == true) {
                bool flag = false;
                char index = floor->map[i][j];
                if(floor->map[i][j] == '#') {
                    wattron(game_window, COLOR_PAIR(1));
                    color_num = 1;
                    flag = true;
                }
                if(!flag && index == '@') {
                    for(int i = 0; i < 6; i++) {
                        if(floor->Rooms[i].locked_door != NULL) {
                            if(floor->Rooms[i].locked_door->is_visited == true) {
                                wattron(game_window, COLOR_PAIR(14));
                                color_num = 14;
                            }
                            else {
                                wattron(game_window, COLOR_PAIR(13));
                                color_num = 13;
                            }
                            break;
                        }
                    }
                    flag = true;
                }
                if(!flag && index == 'd' || index == 'f' || index == 'g' || index == 's' || index == 'u') {
                    wattron(game_window, COLOR_PAIR(19) | A_BOLD);
                    color_num = 19;
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
                                wattron(game_window, COLOR_PAIR(11));
                                color_num = 11;
                                break;
                            case Enchant:
                                wattron(game_window, COLOR_PAIR(9));
                                color_num = 9;
                                break;
                            case Nightmare:
                                wattron(game_window, COLOR_PAIR(12));
                                color_num = 12;
                                break;
                        }
                        flag = true;
                    } 
                }

                if(index == '^')
                    index = '.';
                if(index == '*') {
                    if((j < 145 && floor->map[i][j + 1] == '_') || (j > 0 && floor->map[i][j - 1] == '_'))
                        index = '_';
                    else
                        index = '|';
                }

                mvwaddch(game_window, i, j, index);
                wrefresh(game_window);
                wattroff(game_window, COLOR_PAIR(color_num));
                if(index == 'd' || index == 'f' || index == 'g' || index == 's' || index == 'u')
                    wattroff(game_window, A_BOLD);
            }
        }

    }

    wattron(game_window, COLOR_PAIR(game->player_color) | A_BOLD);
    // wchar_t player = 0x0001F47E;
    // printw("%lc", player);
    mvwaddch(game_window, game->player_location.y, game->player_location.x, 'P');
    wrefresh(game_window);
    wattroff(game_window, COLOR_PAIR(game->player_color) | A_BOLD);

    attron(COLOR_PAIR(3) | A_BOLD);
    for(int i = 0; i < 148; i++) {
        move(0, i);
        addch(' ');
        move(41, i);
        addch(' ');
    }    
    attroff(COLOR_PAIR(3) | A_BOLD);
    if(game->floors[game->player_floor].map[game->player_location.y][game->player_location.x] == '<') {
        attron(COLOR_PAIR(3) | A_BOLD);
        move(0, 1);
        addstr("PRESS >/< to go up/down stair...");
        attroff(COLOR_PAIR(3) | A_BOLD);
    }
    


    attron(COLOR_PAIR(2) | A_BOLD);
    move(41, 0);
    printw(" Game_Name: %s\tHealth: %d%%\tGun: %s,%d \tGold: %d\t\tMaster_Key: %d\tFloor: %d\tPress 'q' to Save & Quit the game!", game->name, game->Health, game->current_gun->name, game->current_gun->counter, game->total_gold, game->master_key_num, (game->player_floor + 1));
    attroff(COLOR_PAIR(2) | A_BOLD);
    
    if(is_unicode == true) {
        //food
        for(int j = 0; j < 6; j++) {
            if(game->floors[game->player_floor].visit[game->floors[game->player_floor].Rooms[j].start.y][game->floors[game->player_floor].Rooms[j].start.x] != true)
                continue;

            for(int i = 0; i < game->floors[game->player_floor].Rooms[j].food_num; i++) {
                if(game->floors[game->player_floor].Rooms[j].foods[i] != NULL)
                    mvwprintw(game_window, game->floors[game->player_floor].Rooms[j].foods[i]->location.y, game->floors[game->player_floor].Rooms[j].foods[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[j].foods[i]->unicode);
            }

            //gold
            for(int i = 0; i < game->floors[game->player_floor].Rooms[j].gold_num; i++) {
                if(game->floors[game->player_floor].Rooms[j].golds[i] != NULL)
                    mvwprintw(game_window, game->floors[game->player_floor].Rooms[j].golds[i]->location.y, game->floors[game->player_floor].Rooms[j].golds[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[j].golds[i]->unicode);
            }

            //gun
            for(int i = 0; i < game->floors[game->player_floor].Rooms[j].gun_num; i++) {
                if(game->floors[game->player_floor].Rooms[j].guns[i] != NULL)
                    mvwprintw(game_window, game->floors[game->player_floor].Rooms[j].guns[i]->location.y, game->floors[game->player_floor].Rooms[j].guns[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[j].guns[i]->unicode);
            }

            //spell
            for(int i = 0; i < game->floors[game->player_floor].Rooms[j].spell_num; i++) {
                if(game->floors[game->player_floor].Rooms[j].spells[i] != NULL)
                    mvwprintw(game_window, game->floors[game->player_floor].Rooms[j].spells[i]->location.y, game->floors[game->player_floor].Rooms[j].spells[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[j].spells[i]->unicode);
            }
        
            //spell
            for(int i = 0; i < game->floors[game->player_floor].Rooms[j].spell_num; i++) {
                if(game->floors[game->player_floor].Rooms[j].spells[i] != NULL)
                    mvwprintw(game_window, game->floors[game->player_floor].Rooms[j].spells[i]->location.y, game->floors[game->player_floor].Rooms[j].spells[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[j].spells[i]->unicode);
            }

            //enemy
            for(int i = 0; i < game->floors[game->player_floor].Rooms[j].enemy_num; i++) {
                if(game->floors[game->player_floor].Rooms[j].enemies[i] != NULL)
                    mvwprintw(game_window, game->floors[game->player_floor].Rooms[j].enemies[i]->location.y, game->floors[game->player_floor].Rooms[j].enemies[i]->location.x, "%lc", game->floors[game->player_floor].Rooms[j].enemies[i]->unicode);
            }

            // master key
            if(game->floors[game->player_floor].Rooms[j].master_key != NULL)
                mvwprintw(game_window, game->floors[game->player_floor].Rooms[j].master_key->location.y, game->floors[game->player_floor].Rooms[j].master_key->location.x, "%lc", game->floors[game->player_floor].Rooms[j].master_key->unicode);
            
            mvwprintw(game_window, game->player_location.y, game->player_location.x, "%lc" ,game->player_unicode);
            wrefresh(game_window);
        }

    
    }


    
    refresh();
}

void *play_sound(void *arg) {
    const char *sound_file = (const char *)arg;

    while (1) { 
        afplay_pid = fork();  

        if (afplay_pid < 0) {
            perror("Fork failed");
            exit(1);
        } else if (afplay_pid == 0) {
            execlp("afplay", "afplay", sound_file, (char *)NULL);
            perror("Error executing afplay"); // If execlp fails
            exit(1);
        }

        int status;
        waitpid(afplay_pid, &status, 0);  

        sleep(1);
    }

    return NULL;
}

#endif



// void *play_sound(void *arg) {
//     const char *sound_file = (const char *)arg;
//     char command[256];

//     snprintf(command, sizeof(command), "afplay '%s'", sound_file);

//     while (1) {
//         system(command);
//         sleep(1);
//     }

//     return NULL;
// }