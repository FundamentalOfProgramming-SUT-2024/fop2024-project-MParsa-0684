#ifndef menus_h
#define menus_h

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ui.h"
#include "player.h"
#include <regex.h>
#include <time.h>
#include <dirent.h>

void main_menu();
void create_player();
void login_player();
int check_player();
void guest_login();
void official_login();
int check_login(char *usern, char *passw);
void player_menu();
void delete_enter(char *s);

// Main menu of game
void main_menu() {

    // Initial setups
    clear_space();
    noecho();
    // cbreak();
    curs_set(FALSE);
    refresh();

    // Page UI
    start_color();
    short r, g, b;
    hx(back_color, &r, &g, &b);
    init_color(COLOR_BLACK, r, g, b);
    short rr, gg, bb;
    hx(font_color, &rr, &gg, &bb);
    init_color(COLOR_BLUE, rr, gg, bb);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    init_pair(3, COLOR_RED, COLOR_BLUE);
    bkgd(COLOR_PAIR(1));
    
    // Header UI
    move(12 ,69);
    attron(A_BOLD | COLOR_PAIR(1));
    addstr("REGUE GAME");
    move(13 ,63);
    addstr("<<<<< Main  Menu >>>>>");
    attroff(A_BOLD);

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
        attron(A_BOLD);
        mvaddstr(lct++, 65, "Enter your Password :");
        move(lct++, 65);
        attroff(A_BOLD);
        getstr(player->password);
        refresh();
        
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
    fprintf(player_file, "%s\n%s\n%s", player->username, player->password, player->email);
    fclose(player_file);

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
        addstr("This Username currently exists! Press ENTER to try again...");
        attroff(COLOR_PAIR(3) | A_BOLD);
        return 0;
    }
    if(strlen(player->password) < 7) {
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
        addstr("Password should contains Number, Uppercase and Lowercase letters! Press ENTER to try again...");
        attroff(COLOR_PAIR(3) | A_BOLD);
        return 0;
    }

    regex_t reg;
    const char *email_pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";
    int log = regcomp(&reg, email_pattern, REG_EXTENDED);
    if(log) {
        addstr("Could not compile E-mail, Press ENTER to try again...");
        attroff(COLOR_PAIR(3) | A_BOLD);
        return 0;
    }
    log = regexec(&reg, player->email, 0, NULL, 0);
    regfree(&reg);
    if(log) {
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
    
    char usern[100], passw[100];
    while(1) {
        // header UI
        clear_space();
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

    // entering player arguments
    char file_path[200] = "../players/";
    strcat(file_path, usern);
    strcat(file_path, ".txt");
    strcpy(player->file_path, file_path);
    FILE* player_file = fopen(player->file_path, "r");
    fgets(player->username, 100, player_file);
    fgets(player->password, 100, player_file);
    fgets(player->email, 100, player_file);
    delete_enter(player->username);
    delete_enter(player->password);
    delete_enter(player->email);
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
    fgets(player->password, 100, player_file);
    fgets(player->email, 100, player_file);
    delete_enter(player->username);
    delete_enter(player->password);
    delete_enter(player->email);
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
    if(strcmp(passw, playerpass) != 0) {
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

        move(16 + location, 65);
        attron(COLOR_PAIR(1));
        addstr(playerMenu[location]);
        attroff(COLOR_PAIR(1));
        for(int i = 0; i < 5; i++) {
            move(16 + i, 65);
            addstr(playerMenu[i]);
        }
        refresh();
        
        switch (cursor) {
            case KEY_ENTER:
                if(location == 4) {
                    main_menu();
                }
                getch();
                (* pmenus[location]) ();
                // create_player();
                break;

            case KEY_DOWN:
                location = (location + 1) % 5;
                break;

            case KEY_UP:
                location = (location - 1) % 5;
                location = (location + 5) % 5;
                break;
        }

        move(16 + location, 65);
        attron(COLOR_PAIR(2));
        addstr(playerMenu[location]);
        attroff(COLOR_PAIR(2));

        refresh();
        // clear_space();
        
    }


}

void delete_enter(char *s) {
    size_t len = strlen(s);
    if(s[len - 1] == '\n')
        s[len - 1] = '\0';
}

/*

    create new game
    continue old game
    score table
    settings

*/

#endif menus_h
