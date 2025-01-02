#ifndef player_h
#define player_h

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
#include "game.h"



typedef struct Player {
    char file_path[200];
    char username[100];
    char password[100];
    char email[100];
    int total_score;
    int total_gold;
    int num_finished;
    time_t time_experience;

    Game *old_game;
    Game *current_game;



} Player;

void new_game();
void old_game();
void score_table();
void settings();
void delete_enter(char *s);

Player *player;

void new_game() {
    
}
void old_game() {

}
void score_table() {
    // Initial setups
    noecho();
    curs_set(FALSE);
    clear_space();
    const char *players_dir = "../players/";
    DIR *dir = opendir(players_dir);
    struct dirent *entry;
    Player *p_table = NULL;
    int p_size = 0;
    while((entry = readdir(dir)) != NULL) {
        const char *entry_name = entry->d_name;
        if(strstr(entry_name, ".txt") == NULL)
            continue;

        p_table = (Player *) realloc(p_table, (++p_size) * sizeof(Player));
        // p_table[p_size - 1] = (Player *) malloc(sizeof(Player));

        char file_path[100];
        strcpy(file_path, players_dir);
        strcat(file_path, entry_name);
        // strcat(file_path, '\0');
        // mvaddstr(20, 20, file_path);
        FILE *player_file = fopen(file_path, "r");
        // if(player_file == NULL)
        //     mvaddstr(40, 20, "www  ");
        strcpy(p_table[p_size - 1].file_path, file_path);

        char sin[200];
        fgets(sin, 100, player_file);
        delete_enter(sin);
        strcpy(p_table[p_size - 1].username, sin);

        fgets(sin, 100, player_file);
        delete_enter(sin);
        strcpy(p_table[p_size - 1].password, sin);

        fgets(sin, 100, player_file);
        delete_enter(sin);
        strcpy(p_table[p_size - 1].email, sin);

        fscanf(player_file, "%d", &(p_table[p_size - 1].total_score));
        fscanf(player_file, "%d", &(p_table[p_size - 1].total_gold));
        fscanf(player_file, "%d", &(p_table[p_size - 1].num_finished));
        fscanf(player_file, "%ld", &(p_table[p_size - 1].time_experience));
        fclose(player_file);
    }
    closedir(dir);
    int start_table = 0;


    // 8 - 39
    bool flag = 1;
    while(flag) {

        // Header UI
        clear_space();
        move(4 ,69);
        attron(A_BOLD | COLOR_PAIR(1));
        init_pair(4, COLOR_RED, COLOR_BLACK);
        init_pair(5, COLOR_GREEN, COLOR_BLACK);
        init_pair(6, COLOR_YELLOW, COLOR_BLACK);
        addstr("REGUE GAME");
        move(5 ,63);
        addstr("<<<<< Score table >>>>>");
        move(7, 13);
        addstr("    Rank\t\tUsername\t\tTotal Score\t\tTotal Gold\t\tGame Finished\t\tPlayTime\t\t");
        attroff(A_BOLD);
        for(int i = 0; i < 32 && start_table + i < p_size; i++) {
            
            move(8 + i, 13);
            // current player blinks and bold
            if(strcmp(p_table[start_table + i].username, player->username) == 0) {
                attron(A_BOLD | A_BLINK);
                if(start_table + i >= 3)
                    addstr("    ");

            }
            // first player is red and bold
            if(start_table + i == 0) {
                attron(A_BOLD | COLOR_PAIR(4));
                addstr("<G> ");
            }
            // second player is green
            if(start_table + i == 1) {
                attron(A_BOLD | COLOR_PAIR(5));
                addstr("<S> ");
            }
            // third player is yellow
            if(start_table + i == 2) {
                attron(A_BOLD | COLOR_PAIR(6));
                addstr("<B> ");
            }
            else 
                addstr("    ");

            move(8 + i, 17);
            printw("%d\t\t%s\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%ld(s)", (start_table + i + 1), p_table[start_table + i].username, p_table[start_table + i].total_score, p_table[start_table + i].total_gold, p_table[start_table + i].num_finished, p_table[start_table + i].time_experience);

            if(strcmp(p_table[start_table + i].username, player->username) == 0) 
                attroff(A_BOLD | A_BLINK);
            if(start_table + i == 0)
                attroff(A_BOLD | COLOR_PAIR(4));
            if(start_table + i == 1)
                attroff(A_BOLD | COLOR_PAIR(5));
            if(start_table + i == 2)
                attroff(A_BOLD | COLOR_PAIR(6));
            refresh();
        }

        int c = getch();
        switch(c) {
            case KEY_DOWN:
                if(start_table + 32 < p_size)
                    start_table++;
                break;
            case KEY_UP:
                if(start_table > 0)
                    start_table--;
                break;

            case 'q':
                flag = 0;
                break;
        }
    }

    curs_set(true);
    // getch();

}
void settings() {

}

void delete_enter(char *s) {
    size_t len = strlen(s);
    if(s[len - 1] == '\n')
        s[len - 1] = '\0';
}

#endif palyer_h