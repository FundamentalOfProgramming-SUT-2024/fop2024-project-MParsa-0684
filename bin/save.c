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
#include <time.h>

#include "ui.h"

void save_game(Player *player);

void save_game(Player *player) {
    // unlink(player->file_path);
    // char fl[1000000];
    FILE* player_file = fopen(player->file_path, "w");
    // setvbuf(player_file, fl, _IOFBF, sizeof(fl));
    // setvbuf(player_file, NULL, _IONBF, 0);
    // setvbuf(player_file, fl, _IOFBF, sizeof(fl));
    fprintf(player_file, "%s\n", player->username);
    fflush(player_file);
    // delete_enter(player->username);
    
    fprintf(player_file, "%s\n", player->password);
    fflush(player_file);
    // delete_enter(player->password);
    
    fprintf(player_file, "%s\n", player->email);
    fflush(player_file);
    // delete_enter(player->email);
    
    fprintf(player_file, "%d\n", (player->total_score));
    fprintf(player_file, "%d\n", (player->total_gold));
    fprintf(player_file, "%d\n", (player->num_finished));
    fprintf(player_file, "%ld\n", (player->time_experience));
    fprintf(player_file, "%d\n", (player->game_difficulty));
    fprintf(player_file, "%d\n", (player->color));
    fflush(player_file);
    // player->music = (Music *) malloc(sizeof(Music));
    fprintf(player_file, "%s\n", player->music->music_path);
    fflush(player_file);
    //fflush(player_file);
    // fclose(player_file);
    // delete_enter(player->music->music_path);


    // // char g[100];
    // // fscanf(player_file, "%s", g);
    if(player->game == NULL) {
        fprintf(player_file, "(null)\n");
        fflush(player_file);
    }
    else {
        // FILE *player_file = fopen(player->file_path, "a");
        Game *game = player->game;
        // game_name
        fprintf(player_file, "%s\n", game->name);
        fflush(player_file);
        // strcpy(game->name, g);
        
        
        // game_floors_calloc
        fprintf(player_file, "%d\n", game->floor_num);
        fflush(player_file);
        // fclose(player_file);
        for(int i = 0; i < game->floor_num; i++) {
            // FILE* player_file2 = fopen(player->file_path, "a");

            // fprintf(player_file, "Map\n");
            fprintf(player_file, "%d\n", game->floors[i].floor_visit);
            fflush(player_file);
            for(int ii = 0; ii < 40; ii++) {
                for(int jj = 0; jj < 146; jj++) 
                    fprintf(player_file, "%d ", game->floors[i].map[ii][jj]);
                fprintf(player_file, "\n");
                fflush(player_file);
            }
            //fflush(player_file);

            // fprintf(player_file, "visit\n");
            for(int ii = 0; ii < 40; ii++) {
                for(int jj = 0; jj < 146; jj++) 
                    fprintf(player_file, "%d ", game->floors[i].visit[ii][jj]);
                fprintf(player_file, "\n");
                fflush(player_file);
            }
            //fflush(player_file);

            fprintf(player_file, "%d\n", game->floors[i].room_num);
            fprintf(player_file, "%d\n", game->floors[i].has_gold);
            fprintf(player_file, "%d\n", game->floors[i].staircase_num);
            fprintf(player_file, "%d\n", game->floors[i].master_key_num);
            fflush(player_file);
            //fflush(player_file);

            // fclose(player_file2);
            for(int j = 0; j < game->floors[i].room_num; j++) {
                Room *room = &game->floors[i].Rooms[j];

                // FILE* player_file3 = fopen(player->file_path, "a");

                // fprintf(player_file, "Room setting\n");
                fprintf(player_file, "%d\n", room->start.y);
                fprintf(player_file, "%d\n", room->start.x);
                fprintf(player_file, "%d\n", room->size.y);
                fprintf(player_file, "%d\n", room->size.x);
                fprintf(player_file, "%d\n", room->type);
                fflush(player_file);

                // fprintf(player_file, "Room food\n");
                // foods
                fprintf(player_file, "%d\n", room->food_num);
                for(int z = 0; z < room->food_num; z++) {
                    if(room->foods[z] == NULL) {
                        fprintf(player_file, "%d\n", -1);
                        continue;
                    }

                    // room->foods[z]->location.y = is;
                    // fprintf(player_file, "%d\n", &);
                    fprintf(player_file, "%d\n", room->foods[z]->location.y);
                    fprintf(player_file, "%d\n", room->foods[z]->location.x);
                    fprintf(player_file, "%d\n", room->foods[z]->type);
                    fprintf(player_file, "%d\n", room->foods[z]->time_passed);
                    fflush(player_file);
                }
                //fflush(player_file);

                // fprintf(player_file, "Room Gold\n");
                // golds
                fprintf(player_file, "%d\n", room->gold_num);
                for(int z = 0; z < room->gold_num; z++) {
                    if(room->golds[z] == NULL) {
                        fprintf(player_file, "%d\n", -1);
                        continue;
                    }

                    fprintf(player_file, "%d\n", room->golds[z]->location.y);
                    fprintf(player_file, "%d\n", room->golds[z]->location.x);
                    fprintf(player_file, "%d\n", room->golds[z]->type);
                    fflush(player_file);
                }
                //fflush(player_file);

                // fprintf(player_file, "Room Gun\n");
                // guns
                fprintf(player_file, "%d\n", room->gun_num);
                for(int z = 0; z < room->gun_num; z++) {
                    if(room->guns[z] == NULL) {
                        fprintf(player_file, "%d\n", -1);
                        continue;
                    }

                    fprintf(player_file, "%d\n", room->guns[z]->location.y);
                    fprintf(player_file, "%d\n", room->guns[z]->location.x);
                    fprintf(player_file, "%d\n", room->guns[z]->type);
                    fprintf(player_file, "%d\n", room->guns[z]->damage);
                    fprintf(player_file, "%d\n", room->guns[z]->counter);   
                    fprintf(player_file, "%d\n", room->guns[z]->distance);
                    fflush(player_file);
                    // fprintf(player_file, "%s\n", room->guns[z]->name);
                }
                //fflush(player_file);

                // fprintf(player_file, "Room Spell\n");
                //  spell
                fprintf(player_file, "%d\n", room->spell_num);
                for(int z = 0; z < room->spell_num; z++) {
                    if(room->spells[z] == NULL) {
                        fprintf(player_file, "%d\n", -1);
                        continue;
                    }

                    fprintf(player_file, "%d\n", room->spells[z]->location.y);
                    fprintf(player_file, "%d\n", room->spells[z]->location.x);
                    fprintf(player_file, "%d\n", room->spells[z]->type);
                    fflush(player_file);
                }
                //fflush(player_file);

                // fprintf(player_file, "Room trap\n");
                // trap
                fprintf(player_file, "%d\n", room->trap_num);
                for(int z = 0; z < room->trap_num; z++) {
                    if(room->traps[z] == NULL) {
                        fprintf(player_file, "%d\n", -1);
                        continue;
                    }

                    fprintf(player_file, "%d\n", room->traps[z]->location.y);
                    fprintf(player_file, "%d\n", room->traps[z]->location.x); 
                    fflush(player_file);
                }
                //fflush(player_file);

                // fprintf(player_file, "Room enemy\n");
                // enemy
                fprintf(player_file, "%d\n", room->enemy_num);
                for(int z = 0; z < room->enemy_num; z++) {
                    if(room->enemies[z] == NULL) {
                        fprintf(player_file, "%d\n", -1);
                        continue;
                    }

                    fprintf(player_file, "%d\n", room->enemies[z]->location.y);
                    fprintf(player_file, "%d\n", room->enemies[z]->location.x);
                    fprintf(player_file, "%d\n", room->enemies[z]->type);
                    fprintf(player_file, "%d\n", room->enemies[z]->damage);
                    fprintf(player_file, "%d\n", room->enemies[z]->health);
                    fprintf(player_file, "%d\n", room->enemies[z]->following);
                    fprintf(player_file, "%d\n", room->enemies[z]->is_moving);
                    fflush(player_file);
                    // fprintf(player_file, "%c\n", room->enemies[z]->chr);
                }
                //fflush(player_file);

                // fprintf(player_file, "Room normal_doors\n");
                // normal_doors
                fprintf(player_file, "%d\n", room->normal_doors_num);
                for(int z = 0; z < room->normal_doors_num; z++) {
                    fprintf(player_file, "%d\n", room->normal_doors[z].location.y);
                    fprintf(player_file, "%d\n", room->normal_doors[z].location.x);
                    fflush(player_file);
                }
                //fflush(player_file);

                // fprintf(player_file, "Room Secret_doors\n");
                // secret_doors
                fprintf(player_file, "%d\n", room->secret_doors_num);
                for(int z = 0; z < room->secret_doors_num; z++) {
                    fprintf(player_file, "%d\n", room->secret_doors[z].location.y);
                    fprintf(player_file, "%d\n", room->secret_doors[z].location.x);
                    fflush(player_file);
                }
                //fflush(player_file);

                // fprintf(player_file, "Room Stair_case\n");
                // stair_case
                if(room->staircase != NULL) {
                    fprintf(player_file, "%d\n", room->staircase->location.y);
                    fprintf(player_file, "%d\n", room->staircase->location.x);
                }                
                else {
                    fprintf(player_file, "%d\n", -1);
                }            
                fflush(player_file);

                // fprintf(player_file, "Room master_key\n");
                // master_key
                if(room->master_key != NULL) {
                    fprintf(player_file, "%d\n", room->master_key->location.y);
                    fprintf(player_file, "%d\n", room->master_key->location.x);
                }
                else {
                    fprintf(player_file, "%d\n", -1);
                }
                fflush(player_file);

                // fprintf(player_file, "Room locked_door\n");
                //locked_door
                // fprintf(player_file, "%d\n", &is);
                if(room->locked_door != NULL) {
                    fprintf(player_file, "%d\n", room->locked_door->location.y);
                    fprintf(player_file, "%d\n", room->locked_door->location.x);
                    fprintf(player_file, "%d\n", room->locked_door->is_visited);
                    fprintf(player_file, "%d\n", room->locked_door->password_turn);
                }
                else {
                    fprintf(player_file, "%d\n", -1);
                }
                fflush(player_file);
                // fclose(player_file3);
            }
        }
        
        // player_file = fopen(player->file_path, "a");
        // game_health
        // fprintf(player_file, "Game health\n");
        fprintf(player_file, "%d\n", game->Health);
        fflush(player_file);

        // fprintf(player_file, "Game food\n");
        // game_food
        for(int i = 0; i < 3; i++) {
            fprintf(player_file, "%d\n", game->food_num[i]);
            for(int z = 0; z < game->food_num[i]; z++) {
                if(game->foods[i][z] == NULL) {
                    fprintf(player_file, "%d\n", -1);
                    fflush(player_file);
                    continue;
                }

                fprintf(player_file, "%d\n", game->foods[i][z]->location.y);
                fprintf(player_file, "%d\n", game->foods[i][z]->location.x);
                fprintf(player_file, "%d\n", game->foods[i][z]->type);
                fprintf(player_file, "%d\n", game->foods[i][z]->time_passed);
                fflush(player_file);
            }
        } 
        fflush(player_file);

        // fprintf(player_file, "Game Gun\n");
        // game_gun
        for(int i = 0; i < 5; i++) {
            if(game->gun[i] == NULL) {
                fprintf(player_file, "%d\n", -1);
                fflush(player_file);
                continue;
            }
            

            fprintf(player_file, "%d\n", game->gun[i]->location.y);
            fprintf(player_file, "%d\n", game->gun[i]->location.x);
            fprintf(player_file, "%d\n", game->gun[i]->type);
            fprintf(player_file, "%d\n", game->gun[i]->damage);
            fprintf(player_file, "%d\n", game->gun[i]->counter);
            fprintf(player_file, "%d\n", game->gun[i]->distance);
            // fprintf(player_file, "%s\n", game->gun[i]->name);
            fflush(player_file);
        }
        //fflush(player_file);

        int ptr = 0;
        for(int i = 0; i < 5; i++) {
            if(game->gun[i] != NULL && game->current_gun->type == game->gun[i]->type) {
                ptr = i;
                break;
            }
        }
        fprintf(player_file, "%d\n", ptr);
        fflush(player_file);

        // fprintf(player_file, "Game spell\n");
        // game_spell
        for(int i = 0; i < 3; i++) {
            fprintf(player_file, "%d\n", game->spell_num[i]);
            for(int z = 0; z < game->spell_num[i]; z++) {
                if(game->spell[i][z] == NULL) {
                    fprintf(player_file, "%d\n", -1);
                    continue;
                }

                fprintf(player_file, "%d\n", game->spell[i][z]->location.y);
                fprintf(player_file, "%d\n", game->spell[i][z]->location.x);
                fprintf(player_file, "%d\n", game->spell[i][z]->type);
                fflush(player_file);
            }
        }
        //fflush(player_file);

        // fprintf(player_file, "Game master_key\n");
        // game_master_key
        fprintf(player_file, "%d\n", game->master_key_num);
        for(int i = 0; i < game->master_key_num; i++) {
            if(game->master_key[i] == NULL) {
                fprintf(player_file, "%d\n", -1);
                continue;
            }

            fprintf(player_file, "%d\n", game->master_key[i]->location.y);
            fprintf(player_file, "%d\n", game->master_key[i]->location.x);
            fflush(player_file);
        }
        //fflush(player_file);

        // fprintf(player_file, "Game player\n");
        fprintf(player_file, "%d\n", game->total_gold);
        fflush(player_file);
        fprintf(player_file, "%d\n", game->player_location.y);
        fflush(player_file);
        fprintf(player_file, "%d\n", game->player_location.x);
        fflush(player_file);
        fprintf(player_file, "%d\n", game->player_floor);
        fflush(player_file);
        fprintf(player_file, "%d\n", game->player_room);
        fflush(player_file);
        // fclose(player_file);
    }
    
    // fprintf(player_file, "%s", fl);
    fclose(player_file);
}
