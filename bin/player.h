#ifndef player_h
#define player_h

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
#include "game.h"
#include <locale.h>
#include <wchar.h>

void new_game();
void old_game();
void score_table();
void settings();
void change_difficulty();
void change_color();
void change_music();
void profile_page();

// Creating and playing new game
void new_game() {
    move(0, 1);
    attron(COLOR_PAIR(3) | A_BOLD);
    addstr("Your old game will be deleted, Press ENTER to continue, Press q to comeback...");
    attroff(COLOR_PAIR(3) | A_BOLD);

    int c = getch();
    while(c != 'q' && c != KEY_ENTER)
        c = getch();

    if(c == 'q')
        return ;
    

    create_new_game(&player->game, player->music, player->game_difficulty, player->color);
    play_game(player->game);

}

// Playing old game
void old_game() {
    play_game(player->game);
}

// Score table 
void score_table() {
    // Initial setups
    noecho();
    curs_set(FALSE);
    clear_space();
    const char *players_dir = "../players/";
    DIR *dir = opendir(players_dir);
    struct dirent *entry;
    Player **p_table = NULL;
    int p_size = 0;
    while((entry = readdir(dir)) != NULL) {
        const char *entry_name = entry->d_name;
        if(strstr(entry_name, ".txt") == NULL)
            continue;

        p_table = (Player **) realloc(p_table, (++p_size) * sizeof(Player *));
        p_table[p_size - 1] = (Player *) malloc(sizeof(Player));

        char file_path[100];
        strcpy(file_path, players_dir);
        strcat(file_path, entry_name);
        // strcat(file_path, '\0');
        // mvaddstr(20, 20, file_path);
        FILE *player_file = fopen(file_path, "r");
        // if(player_file == NULL)
        //     mvaddstr(40, 20, "www  ");
        strcpy(p_table[p_size - 1]->file_path, file_path);

        char sin[200];
        fgets(sin, 100, player_file);
        delete_enter(sin);
        strcpy(p_table[p_size - 1]->username, sin);

        fgets(sin, 100, player_file);
        delete_enter(sin);
        strcpy(p_table[p_size - 1]->password, sin);

        fgets(sin, 100, player_file);
        delete_enter(sin);
        strcpy(p_table[p_size - 1]->email, sin);

        fscanf(player_file, "%d", &(p_table[p_size - 1]->total_score));
        fscanf(player_file, "%d", &(p_table[p_size - 1]->total_gold));
        fscanf(player_file, "%d", &(p_table[p_size - 1]->num_finished));
        fscanf(player_file, "%ld", &(p_table[p_size - 1]->time_experience));
        fclose(player_file);
    }
    closedir(dir);
    int start_table = 0;

    for(int i = 0; i < p_size; i++) {
        for(int j = i + 1; j < p_size; j++) {
            if(p_table[i]->total_score < p_table[j]->total_score) {
                    Player *temp = p_table[i];
                    p_table[i] = p_table[j];
                    p_table[j] = temp;
            }
        }
    }

    // 8 - 39
    bool flag = 1;
    while(flag) {

        // Header UI
        clear_space();
        move(4 ,69);
        attron(A_BOLD | COLOR_PAIR(1));
        addstr("REGUE GAME");
        move(5 ,63);
        addstr("<<<<< Score table >>>>>");
        move(7, 13);
        addstr("    Rank\t\tUsername\t\tTotal Score\t\tTotal Gold\t\tGame Finished\t\tPlayTime\t");
        attroff(A_BOLD);
        for(int i = 0; i < 32 && start_table + i < p_size; i++) {
            
            move(8 + i, 13);
            // current player blinks and bold
            if(strcmp(p_table[start_table + i]->username, player->username) == 0) {
                attron(A_BOLD | A_BLINK);
                if(start_table + i >= 3)
                    addstr("    ");

            }
            // first player is red and bold
            if(start_table + i == 0) {
                //Champion 
                move(8 + i, 10);
                attron(A_BOLD | COLOR_PAIR(4));
                wchar_t gold = 0x0001F947;
                printw("%lc", gold);
                move(8 + i, 13);
                addstr("1st");
                // addstr("<G> ");
            }
            // second player is green
            if(start_table + i == 1) {
                attron(A_BOLD | COLOR_PAIR(5));
                move(8 + i, 10);
                wchar_t silver = 0x0001F948;
                printw("%lc", silver);
                move(8 + i, 13);
                addstr("2nd");
                // addstr("<S> ");
            }
            // third player is yellow
            if(start_table + i == 2) {
                attron(A_BOLD | COLOR_PAIR(6));
                move(8 + i, 10);
                wchar_t bronze = 0x0001F949;
                printw("%lc", bronze);
                move(8 + i, 13);
                addstr("3rd");
                // addstr("<B> ");
            }
            else 
                addstr("    ");

            move(8 + i, 17);
            time_t diff = difftime(time(NULL), p_table[start_table + i]->time_experience);
            int d = diff / 86400;
            int h = (diff % 86400) / 3600;
            int m = (diff % 3600) / 60;
            int s = diff % 60;
            printw("%d\t\t%s\t\t\t%d\t\t\t%d\t\t\t%d\t\t\t%.2d:%.2d:%.2d:%.2d(s)", (start_table + i + 1), p_table[start_table + i]->username, p_table[start_table + i]->total_score, p_table[start_table + i]->total_gold, p_table[start_table + i]->num_finished, d, h, m, s);

            if(strcmp(p_table[start_table + i]->username, player->username) == 0) 
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
                flag = false;
                break;
        }
    }

    // curs_set(true);
    // getch();
}

//player settings 
void settings() {
    
    // Initial setups
    noecho();
    curs_set(FALSE);
    clear_space();

    attron(A_BOLD | COLOR_PAIR(1));
    profile_page();
    move(18 ,60);
    addstr("<<<<< Player Settings >>>>>");
    attroff(A_BOLD);

    // Menu contents
    char settingsMenu[4][100] = {"Change game difficulty", "Change player color", "Change game music", "Exit"}; 

    for(int i = 0; i < 4; i++) {
        move(21 + i, 63);
        addstr(settingsMenu[i]);
    }
    refresh();

    // Selection Part
    move(21, 69);
    void (* smenus[3]) () = {&change_difficulty, &change_color, &change_music};
    int location = 0;
    move(21 + location, 63);
    attron(COLOR_PAIR(2));
    addstr(settingsMenu[location]);
    attroff(COLOR_PAIR(2));

    bool flag = true;
    while(flag) {
        int cursor = getch();

        //Every Move
        // Header UI
        clear_space();
        // move(17 ,69);
        attron(A_BOLD | COLOR_PAIR(1));
        profile_page();
        // addstr("REGUE GAME");
        move(18 ,60);
        addstr("<<<<< Player Settings >>>>>");
        attroff(A_BOLD);

        //Page content
        move(21 + location, 63);
        attron(COLOR_PAIR(1));
        addstr(settingsMenu[location]);
        attroff(COLOR_PAIR(1));
        for(int i = 0; i < 4; i++) {
            move(21 + i, 63);
            addstr(settingsMenu[i]);
        }
        refresh();

        switch(cursor) {
            case KEY_ENTER:
                if(location == 3){
                    flag = false;
                    break;
                }
                (* smenus[location]) ();
                break;

            case KEY_DOWN:
                location = (location + 1) % 4;
                break;

            case KEY_UP:
                location = (location - 1) % 4;
                location = (location + 4) % 4;
                break;
        }

        move(21 + location, 63);
        attron(COLOR_PAIR(2));
        addstr(settingsMenu[location]);
        attroff(COLOR_PAIR(2));

        refresh();
    }

}


// Change difficluty
void change_difficulty() {
    // Initial setups
    noecho();
    curs_set(FALSE);

    // Menu contents
    char dMenu[3][100] = {"Easy", "Normal", "Hard"}; 

    for(int i = 0; i < 3; i++) {
        move(21 + i, 88);
        addstr(dMenu[i]);
    }
    refresh();

    //25
    // Selection Part
    int arr[3] = {Easy, Normal, Hard};
    int location = 0;
    move(21 + location, 88);
    attron(COLOR_PAIR(2));
    addstr(dMenu[location]);
    attroff(COLOR_PAIR(2));

    bool flag = true;
    while(flag) {
        int cursor = getch();

        //Every Move
        //Page content
        move(21 + location, 88);
        attron(COLOR_PAIR(1));
        addstr(dMenu[location]);
        attroff(COLOR_PAIR(1));
        for(int i = 0; i < 3; i++) {
            move(21 + i, 88);
            addstr(dMenu[i]);
        }
        refresh();

        switch(cursor) {
            case KEY_ENTER:
                player->game_difficulty = location;
                flag = false;
                break;

            case KEY_DOWN:
                location = (location + 1) % 3;
                break;

            case KEY_UP:
                location = (location - 1) % 3;
                location = (location + 3) % 3;
                break;
        }

        move(21 + location, 88);
        attron(COLOR_PAIR(1));
        addstr(dMenu[location]);
        attroff(COLOR_PAIR(1));
        for(int i = 0; i < 3; i++) {
            move(21 + i, 88);
            addstr(dMenu[i]);
        }
        move(21 + location, 88);
        attron(COLOR_PAIR(2));
        addstr(dMenu[location]);
        attroff(COLOR_PAIR(2));

        refresh();
    }



}

// Change player color
void change_color() {
    // Initial setups
    noecho();
    curs_set(FALSE);

    // Menu contents
    char colMenu[5][100] = {"Firebrick", "Forestgreen", "Dimgray", "Gold", "Darkorage"}; 
    int arr[5] = {COLOR_FIREBRICK, COLOR_FORESTGREEN, COLOR_DIMGRAY, COLOR_GOLD, COLOR_DARKORANGE};

    for(int i = 0; i < 5; i++) {
        attron(COLOR_PAIR(arr[i]));
        move(22 + i, 88);
        addstr(colMenu[i]);
        attroff(COLOR_PAIR(arr[i]));
    }
    refresh();

    //25
    // Selection Part
    int location = 0;
    move(22 + location, 88);
    attron(COLOR_PAIR(2));
    addstr(colMenu[location]);
    attroff(COLOR_PAIR(2));

    bool flag = true;
    while(flag) {
        int cursor = getch();

        //Every Move
        //Page content
        // move(22 + location, 88);
        // attron(COLOR_PAIR(arr[location]));
        // addstr(colMenu[location]);
        // attroff(COLOR_PAIR(arr[location]));
        for(int i = 0; i < 5; i++) {
            attron(COLOR_PAIR(arr[i]));
            move(22 + i, 88);
            addstr(colMenu[i]);
            attroff(COLOR_PAIR(arr[i]));
        }
        refresh();

        switch(cursor) {
            case KEY_ENTER:
                player->color = arr[location];
                flag = false;
                break;

            case KEY_DOWN:
                location = (location + 1) % 5;
                break;

            case KEY_UP:
                location = (location - 1) % 5;
                location = (location + 5) % 5;
                break;
        }

        // move(22 + location, 88);
        // attron(COLOR_PAIR(arr[location]));
        // addstr(colMenu[location]);
        // attroff(COLOR_PAIR(arr[location]));
        for(int i = 0; i < 5; i++) {
            attron(COLOR_PAIR(arr[i]));
            move(22 + i, 88);
            addstr(colMenu[i]);
            attroff(COLOR_PAIR(arr[i]));
        }
        move(22 + location, 88);
        attron(COLOR_PAIR(2));
        addstr(colMenu[location]);
        attroff(COLOR_PAIR(2));

        refresh();
    }

}

// Chnage game music
void change_music() {
    


    // Initial setups
    noecho();
    curs_set(FALSE);

    // Menu contents
    const char *players_dir = "../music/";
    DIR *dir = opendir(players_dir);
    struct dirent *entry;
    char **musics = NULL;
    int m_size = 0;
    char **muMenu = NULL;
    char **arr = NULL;
    while((entry = readdir(dir)) != NULL) {
        const char *entry_name = entry->d_name;
        if(strstr(entry_name, ".mp3") == NULL)
            continue;

        m_size++;
        muMenu = (char **) realloc(muMenu, m_size * sizeof(char *));
        muMenu[m_size - 1] = (char *) malloc(100 * sizeof(char));
        strcpy(muMenu[m_size - 1], entry_name);


        arr = (char **) realloc(arr, m_size * sizeof(char *));
        arr[m_size - 1] = (char *) malloc(100 * sizeof(char));
        strcpy(arr[m_size - 1], players_dir);
        strcat(arr[m_size - 1], entry_name);
    }
    closedir(dir);
    m_size++;
    muMenu = (char **) realloc(muMenu, m_size * sizeof(char *));
    muMenu[m_size - 1] = (char *) malloc(100 * sizeof(char));
    strcpy(muMenu[m_size - 1], "None"); 

    for(int i = 0; i < m_size; i++) {
        move(23 + i, 88);
        addstr(muMenu[i]);
    }
    refresh();

    //25
    // Selection Part
    int location = 0;
    move(23 + location, 88);
    attron(COLOR_PAIR(2));
    addstr(muMenu[location]);
    attroff(COLOR_PAIR(2));

    bool flag = true;
    while(flag) {
        int cursor = getch();

        //Every Move
        //Page content
        move(23 + location, 88);
        attron(COLOR_PAIR(1));
        addstr(muMenu[location]);
        attroff(COLOR_PAIR(1));
        for(int i = 0; i < m_size; i++) {
            move(23 + i, 88);
            addstr(muMenu[i]);
        }
        refresh();

        switch(cursor) {
            case KEY_ENTER:
                if(location == m_size - 1) 
                    strcpy(player->music->music_path, "NULL");
                else 
                    strcpy(player->music->music_path,arr[location]);
                flag = false;
                break;

            case KEY_DOWN:
                location = (location + 1) % m_size;
                break;

            case KEY_UP:
                location = (location - 1) % m_size;
                location = (location + m_size) % m_size;
                break;
        }

        move(23 + location, 88);
        attron(COLOR_PAIR(1));
        addstr(muMenu[location]);
        attroff(COLOR_PAIR(1));
        for(int i = 0; i < m_size; i++) {
            move(23 + i, 88);
            addstr(muMenu[i]);
        }
        move(23 + location, 88);
        attron(COLOR_PAIR(2));
        addstr(muMenu[location]);
        attroff(COLOR_PAIR(2));

        refresh();
    }


}

void profile_page() {

    for(int i = 0; i < 15; i++) 
    mvaddstr(i + 1, 2, BOX3[i]);

    for(int i = 0; i < 6; i++) 
        mvaddstr(i + 5 + 1, 10, HAPPY_FACE[i]);
    for(int i = 0; i < 6; i++) 
        mvaddstr(i + 5 + 1, 125, HAPPY_FACE[i]);

    move(3 ,60);
    addstr("<<<<< Player Profile >>>>>");

    attroff(A_BOLD);
    char str[100];
    sprintf(str, "Username: %s", player->username);
    mvaddstr(5 + 1, 27, str);
    sprintf(str, "Password: %s", player->password);
    mvaddstr(8 + 1, 27, str);
    sprintf(str, "Email: %s", player->email);
    mvaddstr(11 + 1, 27, str);

    sprintf(str, "Total Score: %d", player->total_score);
    mvaddstr(5 + 1, 51, str);
    sprintf(str, "Total Gold: %d", player->total_gold);
    mvaddstr(8 + 1, 51, str);
    sprintf(str, "Number of Finished Games: %d", player->num_finished);
    mvaddstr(11 + 1, 51, str);

    time_t diff = difftime(time(NULL), player->time_experience);
    int d = diff / 86400;
    int h = (diff % 86400) / 3600;
    int m = (diff % 3600) / 60;
    int s = diff % 60;
    char dMenu[3][100] = {"Easy", "Normal", "Hard"}; 
    char colMenu[5][100] = {"Firebrick", "Forestgreen", "Dimgray", "Gold", "Darkorage"}; 
    char *ptr = strrchr(player->music, '/');
    ptr++;

    sprintf(str, "Experienced Time: %.2d:%.2d:%.2d:%.2d(s)", d, h, m, s);
    mvaddstr(5 + 1,85, str);
    sprintf(str, "Game Difficulty: %s", dMenu[player->game_difficulty]);
    mvaddstr(7 + 1, 85, str);
    sprintf(str, "Player Color: %s", colMenu[player->color - 8]);
    mvaddstr(9 + 1, 85, str);
    sprintf(str, "Game Music: %s", ptr);
    mvaddstr(11 + 1, 85, str);
    

    refresh();

    // Header UI
    // clear_space();
    // move(17 ,69);
    // addstr("REGUE GAME");
    attron(A_BOLD);

}

#endif