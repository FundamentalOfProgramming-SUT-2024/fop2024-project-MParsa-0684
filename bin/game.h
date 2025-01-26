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

#include "ui.h"
#include "menus.h"
#include "feature.h"


void play_game(Game *game);
void create_new_game(Game **game, Music *music, enum Difficulty difficulty, int color);
void create_new_floor(Floor *floor, int floor_num, Game *game);
void create_new_room(Room *room, Floor *floor, int floor_num, int room_num, Game *game);
void in_room_road(Floor *floor, int floor_num);
void paint_floor(Game *game, Floor *floor, WINDOW *game_window, int time_passed);
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
    (*game)->floor_num = 0;
    
    (*game)->Health = 100;
    (*game)->total_gold = 0;
    
    (*game)->foods = (Food ***) calloc(4, sizeof(Food **));
    (*game)->food_num = (int *) calloc(4, sizeof(int));

    (*game)->gun = (Gun ***) calloc(5, sizeof(Gun **));
    (*game)->gun_num = (int *) calloc(5, sizeof(int));
    
    (*game)->gun[0] = (Gun **) calloc(1, sizeof(Gun *));
    (*game)->gun[0][0] = (Gun *) calloc(1, sizeof(Gun));
    strcpy((*game)->gun[0][0][0].name,"Mace"), (*game)->gun[0][0][0].type = Mace;
    (*game)->gun_num[0]++;
    (*game)->current_gun = (*game)->gun[0][0];

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
            room->food_num = rand() % (game->game_difficulty);
            room->gold_num = rand() % (game->game_difficulty);
            room->gun_num = rand() % 2;
            room->spell_num = rand() % 2;
            room->trap_num = rand() % 2;
            room->enemy_num = rand() % 3;
            break;

    }


    // Food configuration
    room->foods = (Food **) calloc((room->food_num), sizeof(Food *));
    Food_Type ftype[7] = {Ordinary, Excellent, Ordinary, Magical, Toxic, Ordinary, Ordinary};
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
                        room->guns[i]->damage = 5;
                        //unicode
                        break;
                    case Dagger:
                        room->guns[i]->damage = 12;
                        //unicode
                        break;
                    case Magic_Wand:
                        room->guns[i]->damage = 15;
                        //unicode
                        break;
                    case Normal_Arrow:
                        room->guns[i]->damage = 5;
                        //unicode
                        break;
                    case Sword:
                        room->guns[i]->damage = 10;
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
                        //unicode
                        break;
                    case Monster:
                        room->enemies[i]->damage = 1;
                        room->enemies[i]->health = 10;
                        room->enemies[i]->following = 0;
                        room->enemies[i]->chr = 'f';
                        //unicode
                        break;
                    case Giant:
                        room->enemies[i]->damage = 1;
                        room->enemies[i]->health = 15;
                        room->enemies[i]->following = 5;
                        room->enemies[i]->chr = 'g';
                        //unicode
                        break;
                    case Snake:
                        room->enemies[i]->damage = 1;
                        room->enemies[i]->health = 20;
                        room->enemies[i]->following = 1000000;
                        room->enemies[i]->chr = 's';
                        //unicode
                        break;
                    case Undeed:
                        room->enemies[i]->damage = 1;
                        room->enemies[i]->health = 30;
                        room->enemies[i]->following = 5;
                        room->enemies[i]->chr = 'u';
                        //unicode
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
                // unicode
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
                // unicode
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

    //Initial Setups
    clear_space2();
    setlocale(LC_ALL, "");
    WINDOW *game_window = newwin(40, 146, 1, 1);
    int time_passed = 0;
    // srand(time(NULL));

    // Music
    // pthread_t sound_thread;
    // pthread_create(&sound_thread, NULL, play_sound, (void *) game->music->music_path);

    // facets
    bool flag = true;
    bool is_take_element = true;  
    bool is_g = false;  
    bool is_f = false;
    bool is_moved = false;
    int dir = -1;
    while(flag) {
        paint_floor(game, &game->floors[game->player_floor], game_window, time_passed);
        
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
                whole_game(game, &game->floors[game->player_floor]);
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
                }
                else
                    beep();
                break;

            // menu for foods to eat
            case 'e':
                foods_menu(game);
                break;

            // munu for guns to take
            case 'i':
                guns_menu(game);
                break;

            // menu for spells to use
            case 'p':
                spells_menu(game);
                break;

            // for using gun
            case ' ':

                break;

            // for repeating gun usage again
            case 'a':

                break;

            // quit and save
            case 'q':
                flag = false;
                break;
        }

        //! actions after moving 

        if(is_moved == true && is_take_element == true) {
            action_game(game, dir, time_passed);
        } 
        else if(is_moved == true){
            is_take_element = true;
        }
        time_passed++;
        
    }

    // Music end
    // pthread_cancel(sound_thread); 
    // pthread_join(sound_thread, NULL); 

    delwin(game_window);
}

void paint_floor(Game *game, Floor *floor, WINDOW *game_window, int time_passed) {
    noecho();
    curs_set(FALSE);
    clear_space2();
    wclear(game_window);

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
                if(index == '@') {
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
                    if((i < 39 && floor->map[i + 1][j] == '#') || (i > 0 && floor->map[i - 1][j] == '#'))
                        index = '_';
                    else
                        index = '|';
                }

                mvwaddch(game_window, i, j, index);
                wrefresh(game_window);
                wattroff(game_window, COLOR_PAIR(color_num));
            }
        }

    }

    wattron(game_window, COLOR_PAIR(game->player_color) | A_BOLD);
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

    if(time_passed % 1 == 0)
        game->Health -= 2;


    attron(COLOR_PAIR(2) | A_BOLD);
    move(41, 0);
    printw(" \t\tHealth: %d%%\t\tGun: %s\tGold: %d\t\tFloor: %d\tPress 'q' to Save & Quit the game!", game->Health, game->current_gun->name, game->total_gold, (game->player_floor + 1));
    attroff(COLOR_PAIR(2) | A_BOLD);
        
    
    refresh();
}

void *play_sound(void *arg) {
    const char *sound_file = (const char *)arg;

    // Fork a child process
    pid_t pid = fork();
    if (pid == 0) {
        // Child process runs afplay
        execlp("afplay", "afplay", sound_file, (char *)NULL);
        perror("Error executing afplay"); // If execlp fails
        exit(1);
    }

    // Parent process continues (does nothing here for sound)
    return NULL;
}


#endif



// void *play_sound(void *arg) {
//     const char *sound_file = (const char *)arg;
//     char command[256];

//     snprintf(command, sizeof(command), "setsid afplay '%s' &", sound_file);

//     while (1) {
//         system(command);
//         sleep(1);
//     }

//     return NULL;
// }