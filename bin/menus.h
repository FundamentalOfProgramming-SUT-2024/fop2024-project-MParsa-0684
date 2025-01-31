#ifndef menus_h
#define menus_h

#include <stdio.h>
#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ui.h"
#include "player.h"
#include "game.h"
#include <regex.h>
#include <time.h>
#include <dirent.h>
#include <locale.h>
#include <wchar.h>


void main_menu();
void create_player();
void login_player();
int check_player();
void guest_login();
void official_login();
int check_login(char *usern, char *passw);
void player_menu();

// Main menu of game
void main_menu() {

    // Initial setups
    // setlocale(LC_ALL, "");
    noecho();
    // cbreak();
    curs_set(FALSE);
    setlocale(LC_ALL, "");
    refresh();

    // Page UI
    start_color();    
    clear_space();


    bkgd(COLOR_PAIR(1));
    
    // Header UI
    move(12 ,69);
    attron(A_BOLD | COLOR_PAIR(1));
    addstr("REGUE GAME");


    // wchar_t gold = 0x0001F312;
    // mvprintw(3, 3, "◌");

    move(13 ,63);
    addstr("<<<<< Main  Menu >>>>>");
    attroff(A_BOLD);

    for(int i = 14; i < 20; i++) {
        move(i, 9);
        addstr(ROGUE[i - 14]);
        move(i, 97);
        addstr(GAME[i - 14]);
    }

    // Menu contents
    char mainMenu[3][100] = {"Create new accout", "Login accout", "Exit"}; 

    for(int i = 0; i < 3; i++) {
        move(16 + i, 65);
        addstr(mainMenu[i]);
    }
    refresh();

    // Selection Part
    move(16, 69);
    void (* menus[2]) () = {&create_player, &login_player};
    int location = 0;
    move(16 + location, 65);
    attron(COLOR_PAIR(2));
    addstr(mainMenu[location]);
    attroff(COLOR_PAIR(2));
    
    while(true) {
        int cursor = getch();

        move(16 + location, 65);
        attron(COLOR_PAIR(1));
        addstr(mainMenu[location]);
        attroff(COLOR_PAIR(1));
        
        switch (cursor) {
            case KEY_ENTER:
                if(location == 2) {
                    endwin();
                    exit(0);
                }
                (* menus[location]) ();
                // create_player();
                break;

            case KEY_DOWN:
                location = (location + 1) % 3;
                break;

            case KEY_UP:
                location = (location - 1) % 3;
                location = (location + 3) % 3;
                break;
        }

        move(16 + location, 65);
        attron(COLOR_PAIR(2));
        addstr(mainMenu[location]);
        attroff(COLOR_PAIR(2));

        refresh();
        // clear_space();
        
    }
    
    
}

// Creating new player
void create_player() {
    // noecho();
    echo();
    refresh();
    
    // username, password, email
    player = (Player *) calloc(1, sizeof(Player));
    while(1) {
        // header UI
        clear_space();
        refresh();
        move(12 ,69);
        attron(A_BOLD | COLOR_PAIR(1));
        addstr("REGUE GAME");
        move(13 , 60);
        addstr("<<<<< Create new accout >>>>>");
        attroff(A_BOLD);
        refresh();
        curs_set(TRUE);        
        int lct = 16;


        // username
        attron(A_BOLD);
        mvaddstr(lct++, 65, "Enter your Username :");
        move(lct++, 65);
        attroff(A_BOLD);
        getstr(player->username);
        refresh();

        // password
        move(0, 1);
        attron(COLOR_PAIR(3) | A_BOLD);
        addstr("To get random password, write 'RANDOM' and press ENTER!!");
        attroff(COLOR_PAIR(3) | A_BOLD);

        attron(A_BOLD);
        mvaddstr(lct++, 65, "Enter your Password :");
        move(lct, 65);
        attroff(A_BOLD);
        getstr(player->password);
        refresh();

        // Getting Random Password
        if(strcmp(player->password, "RANDOM") == 0) {
            char random[11];
            for(int i = 0; i < 10; i++) {
                char ran = rand() % 126;
                ran = (ran + 33) % 126;
                ran = (ran < 33) ? ran + 33 : ran;
                random[i] = ran;
            }
            random[10] = '\0';
            strcpy(player->password, random);
        }
        move(lct, 65);
        addstr(player->password);
        lct++;
        move(0, 1);
        attron(COLOR_PAIR(3) | A_BOLD);
        addstr("                                                        ");
        attroff(COLOR_PAIR(3) | A_BOLD);

        // email
        attron(A_BOLD);
        mvaddstr(lct++, 65, "Enter your E-mail :");
        move(lct++, 65);
        attroff(A_BOLD);
        getstr(player->email);
        refresh();

        curs_set(FALSE);
        if(check_player())
            break;
        
        getch();
    }

    // saving new player
    char file_path[200] = "../players/";
    strcat(file_path, player->username);
    strcat(file_path, ".txt");
    strcpy(player->file_path, file_path);  
    FILE* player_file = fopen(file_path, "w");
    player->total_score = 0;
    player->total_gold = 0;
    player->num_finished = 0;
    player->game_difficulty = Easy;
    player->color = COLOR_FIREBRICK;
    player->music = (Music *) malloc(sizeof(Music));
    strcpy(player->music->music_path, "../music/05 Aen Seidhe.mp3");
    player->time_experience = time(NULL);
    fprintf(player_file, "%s\n%s\n%s\n%d\n%d\n%d\n%ld\n%d\n%d\n%s\n", player->username, player->password, player->email, player->total_score, player->total_gold, player->num_finished, player->time_experience, player->game_difficulty, player->color, player->music->music_path);
    fclose(player_file);

    //adding new details

    player_menu();
}

// Logging in an player
void login_player() {
    noecho();
    refresh();
    clear_space();
    player = (Player *) calloc(1, sizeof(Player));


    // Header UI
    move(12 ,69);
    attron(A_BOLD | COLOR_PAIR(1));
    addstr("REGUE GAME");
    move(13 ,63);
    addstr("<<<<< Login Menu >>>>>");
    attroff(A_BOLD);

    for(int i = 14; i < 20; i++) {
        move(i, 9);
        addstr(ROGUE[i - 14]);
        move(i, 97);
        addstr(GAME[i - 14]);
    }

    // Menu contents
    char loginMenu[3][100] = { "Official Player", "Guest Player", "Go back"}; 

    for(int i = 0; i < 3; i++) {
        move(16 + i, 65);
        addstr(loginMenu[i]);
    }
    refresh();


    // Selection Part
    int location = 0;
    void (* loginmenus[2]) () = {&official_login, &guest_login};
    move(16 + location, 65);
    attron(COLOR_PAIR(2));
    addstr(loginMenu[location]);
    attroff(COLOR_PAIR(2));
    
    while(true) {
        int cursor = getch();

        move(16 + location, 65);
        attron(COLOR_PAIR(1));
        addstr(loginMenu[location]);
        for(int i = 14; i < 20; i++) {
            move(i, 9);
            addstr(ROGUE[i - 14]);
            move(i, 97);
            addstr(GAME[i - 14]);
        }   
        attroff(COLOR_PAIR(1));
        
        switch (cursor) {
            case KEY_ENTER:
                if(location == 2)
                    main_menu();
                (* loginmenus[location]) ();
                break;

            case KEY_DOWN:
                location = (location + 1) % 3;
                break;

            case KEY_UP:
                location = (location - 1) % 3;
                location = (location + 3) % 3;
                break;

            case 'q':
                endwin();
                exit(0);
                break;
        }

        move(16 + location, 65);
        attron(COLOR_PAIR(2));
        addstr(loginMenu[location]);
        attroff(COLOR_PAIR(2));

        refresh();
    }

}

// checking for creating new player
int check_player() {
    
    move(0, 1);
    attron(COLOR_PAIR(3) | A_BOLD);
    char file_path[200] = "../players/";
    strcat(file_path, player->username);
    strcat(file_path, ".txt");
    if(fopen(file_path, "r") != NULL) {
        beep();
        addstr("This Username currently exists! Press ENTER to try again...");
        attroff(COLOR_PAIR(3) | A_BOLD);
        return 0;
    }
    if(strlen(player->password) < 7) {
        beep();
        addstr("Length of Password should be at least 7! Press ENTER to try again...");
        attroff(COLOR_PAIR(3) | A_BOLD);
        return 0;
    }
    
    // a copy of player password
    char ppass[100];
    strcpy(ppass, player->password);
    bool flagnum = false, flagA = false, flaga = false;
    for(int i = 0; i < strlen(ppass); i++) {
        if(flagnum == false && ppass[i] >= '0' && ppass[i] <= '9')
            flagnum = true;
        if(flagA == false && ppass[i] >= 'A' && ppass[i] <= 'Z')
            flagA = true;
        if(flaga == false && ppass[i] >= 'a' && ppass[i] <= 'z')
            flaga = true;
    }

    if(!(flagnum && flagA && flaga)) {
        beep();
        addstr("Password should contains Number, Uppercase and Lowercase letters! Press ENTER to try again...");
        attroff(COLOR_PAIR(3) | A_BOLD);
        return 0;
    }

    regex_t reg;
    const char *email_pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";
    int log = regcomp(&reg, email_pattern, REG_EXTENDED);
    if(log) {
        beep();
        addstr("Could not compile E-mail, Press ENTER to try again...");
        attroff(COLOR_PAIR(3) | A_BOLD);
        return 0;
    }
    log = regexec(&reg, player->email, 0, NULL, 0);
    regfree(&reg);
    if(log) {
        beep();
        addstr("E-mail pattern doesn't match, Press ENTER to try again...");
        attroff(COLOR_PAIR(3) | A_BOLD);
        return 0;
    }

    // Keys to return/ again/ generate random password
    
    // Generating random password
    
    
    attroff(COLOR_PAIR(3) | A_BOLD);
    return 1;

}

// loggging in official player from login menu
void official_login() {
    echo();
    refresh();
    clear_space();

    // move(0, 1);

    char usern[100], passw[100];
    while(1) {
        // header UI
        clear_space();
        refresh();
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(0, 1, "If you forgot your Password, Enter 'FORGOT' to autofill that!");
        attroff(COLOR_PAIR(3) | A_BOLD);   
        refresh();
        move(12 ,69);
        attron(A_BOLD | COLOR_PAIR(1));
        addstr("REGUE GAME");
        move(13 , 58);
        addstr("<<<<< Login Official accout >>>>>");
        attroff(A_BOLD);
        refresh();
        curs_set(TRUE);        
        int lct = 16;

        // username
        attron(A_BOLD);
        mvaddstr(lct++, 65, "Enter your Username :");
        move(lct++, 65);
        attroff(A_BOLD);
        getstr(usern);
        refresh();
        
        // password
        attron(A_BOLD);
        mvaddstr(lct++, 65, "Enter your Password :");
        move(lct++, 65);
        attroff(A_BOLD);
        getstr(passw);
        refresh();
        
        curs_set(FALSE);
        if(check_login(usern, passw))
            break;
        
        // giving hint for password forget
        getch();
    }

    for(int i = 0; i < 146; i++) {
        move(0, i);
        addch(' ');
    }    

    // entering player arguments
    char file_path[200] = "../players/";
    strcat(file_path, usern);
    strcat(file_path, ".txt");
    strcpy(player->file_path, file_path);
    FILE* player_file = fopen(player->file_path, "r");
    fgets(player->username, 100, player_file);
    delete_enter(player->username);
    
    fgets(player->password, 100, player_file);
    delete_enter(player->password);
    
    fgets(player->email, 100, player_file);
    delete_enter(player->email);
    
    fscanf(player_file, "%d", &(player->total_score));
    fscanf(player_file, "%d", &(player->total_gold));
    fscanf(player_file, "%d", &(player->num_finished));
    fscanf(player_file, "%ld", &(player->time_experience));
    fscanf(player_file, "%d", &(player->game_difficulty));
    fscanf(player_file, "%d", &(player->color));
    player->music = (Music *) malloc(sizeof(Music));
    fgets(player->music->music_path, 100, player_file);
    delete_enter(player->music->music_path);
    fclose(player_file);
    // adding other arguments to player

    player_menu();

    
}

// logging in guest player from login menu
void guest_login() {
    char file_path[200] = "../players/";
    strcat(file_path, "guest");
    strcat(file_path, ".txt");
    strcpy(player->file_path, file_path);
    FILE* player_file = fopen(player->file_path, "r");
    fgets(player->username, 100, player_file);
    delete_enter(player->username);

    fgets(player->password, 100, player_file);
    delete_enter(player->password);
    
    fgets(player->email, 100, player_file);
    delete_enter(player->email);

    fscanf(player_file, "%d", &(player->total_score));
    fscanf(player_file, "%d", &(player->total_gold));
    fscanf(player_file, "%d", &(player->num_finished));
    fscanf(player_file, "%ld", &(player->time_experience));
    fscanf(player_file, "%d", &(player->game_difficulty));
    fscanf(player_file, "%d", &(player->color));
    player->music = (Music *) malloc(sizeof(Music));
    fscanf(player_file, "%s", player->music->music_path);
    // delete_enter(player->music->music_path);
    fclose(player_file);
    // adding other arguments to player

    player_menu();

}

// checking username & password for logging in offical player
int check_login(char *usern, char *passw) {
    
    move(0, 1);
    attron(COLOR_PAIR(3) | A_BOLD);

    // check for player username
    char file_path[200] = "../players/";
    strcat(file_path, usern);
    strcat(file_path, ".txt");
    if(fopen(file_path, "r") == NULL) {
        beep();
        addstr("Player with this Username doesn't exist! Press ENTER to try again...");
        attroff(COLOR_PAIR(3) | A_BOLD);
        return 0;
    }

    // check for player password
    FILE* login = fopen(file_path, "r");
    char temp[100];
    char playerpass[100];
    fgets(temp, 100, login);
    fgets(playerpass, 100, login);
    size_t len = strlen(playerpass);
    if(playerpass[len - 1] == '\n')
        playerpass[len - 1] = '\0';
    
    if(strcmp(passw, "FORGOT") == 0) {
        strcpy(passw, playerpass);
    }

    if(strcmp(passw, playerpass) != 0) {
        beep();
        addstr("Password is not correct! Press ENTER to try again...");
        attroff(COLOR_PAIR(3) | A_BOLD);
        fclose(login);
        return 0;
    }

    fclose(login);
    attroff(COLOR_PAIR(3) | A_BOLD);
    return 1;
}

// player menu after logging in player
void player_menu() {

    // Initial setups
    clear_space();
    noecho();
    curs_set(FALSE);
    refresh();

    // Header UI
    move(12 ,69);
    attron(A_BOLD | COLOR_PAIR(1));
    addstr("REGUE GAME");
    move(13 ,64 - ((int) strlen(player->username) / 2));
    printw("<<<<< Welcome %s >>>>>", player->username);
    attroff(A_BOLD);

    for(int i = 14; i < 20; i++) {
        move(i, 9);
        addstr(ROGUE[i - 14]);
        move(i, 97);
        addstr(GAME[i - 14]);
    }

    // Menu contents
    char playerMenu[5][100] = {"Create new game", "Continue old game", "Score table", "Settings", "Exit"}; 

    for(int i = 0; i < 5; i++) {
        move(16 + i, 65);
        addstr(playerMenu[i]);
    }
    refresh();

    // Selection Part
    move(16, 69);
    void (* pmenus[4]) () = {&new_game, &old_game, &score_table, &settings};
    int location = 0;
    move(16 + location, 65);
    attron(COLOR_PAIR(2));
    addstr(playerMenu[location]);
    attroff(COLOR_PAIR(2));
    
    while(true) {
        int cursor = getch();

        //Every move
        clear_space();
        move(12 ,69);
        attron(A_BOLD | COLOR_PAIR(1));
        addstr("REGUE GAME");
        move(13 ,64 - ((int) strlen(player->username) / 2));
        printw("<<<<< Welcome %s >>>>>", player->username);
        attroff(A_BOLD);
        move(16 + location, 65);
        attron(COLOR_PAIR(1));
        addstr(playerMenu[location]);
        attroff(COLOR_PAIR(1));
        for(int i = 0; i < 5; i++) {
            move(16 + i, 65);
            addstr(playerMenu[i]);
        }
        for(int i = 14; i < 20; i++) {
            move(i, 9);
            addstr(ROGUE[i - 14]);
            move(i, 97);
            addstr(GAME[i - 14]);
        }

        refresh();
        
        switch (cursor) {
            case KEY_ENTER:
                if(location == 4) {
                    main_menu();
                }
                (* pmenus[location]) ();
                break;

            case KEY_DOWN:
                location = (location + 1) % 5;
                break;

            case KEY_UP:
                location = (location - 1) % 5;
                location = (location + 5) % 5;
                break;
        }

        //Every move
        clear_space();
        move(12 ,69);
        attron(A_BOLD | COLOR_PAIR(1));
        addstr("REGUE GAME");
        move(13 ,64 - ((int) strlen(player->username) / 2));
        printw("<<<<< Welcome %s >>>>>", player->username);
        attroff(A_BOLD);
        move(16 + location, 65);
        for(int i = 0; i < 5; i++) {
            move(16 + i, 65);
            addstr(playerMenu[i]);
        }
        for(int i = 14; i < 20; i++) {
            move(i, 9);
            addstr(ROGUE[i - 14]);
            move(i, 97);
            addstr(GAME[i - 14]);
        }
        refresh();
        move(16 + location, 65);
        attron(COLOR_PAIR(2));
        addstr(playerMenu[location]);
        attroff(COLOR_PAIR(2));

        refresh();
        // clear_space();
        
    }
}


/*

    create new game
    continue old game
    score table
    settings

*/

#endif
