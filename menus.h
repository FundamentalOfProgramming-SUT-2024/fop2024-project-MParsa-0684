#ifndef menus_h
#define menus_h

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ui.h"
#include "player.h"

void main_menu();
void create_account();
void login_account();

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
    bkgd(COLOR_PAIR(1));
    
    // Header UI
    move(15 ,69);
    attron(A_BOLD | COLOR_PAIR(1));
    addstr("REGUE GAME");
    attroff(A_BOLD);

    // Menu contents
    char mainMenu[2][100] = {"Create new accout", "Login accout"}; 

    for(int i = 0; i < 2; i++) {
        move(16 + i, 65);
        addstr(mainMenu[i]);
    }
    refresh();

    // Selecting Part
    move(16, 69);
    void (* menus[2]) () = {&create_account, &login_account};
    int location = 0;
    while(true) {
        int cursor = getch();

        move(16 + location, 65);
        attron(COLOR_PAIR(1));
        addstr(mainMenu[location]);
        attroff(COLOR_PAIR(1));
        
        switch (cursor)
        {
        case 'w':
            refresh();
            (* menus[location]) ();
            // create_account();
            break;
        
        case 'j':
            location = (location + 1) % 2;
            break;
        
        case 'k':
            location = (location - 1) % 2;
            location = (location + 2) % 2;
            break;
        }

        move(16 + location, 65);
        attron(COLOR_PAIR(2));
        addstr(mainMenu[location]);
        attroff(COLOR_PAIR(2));

        refresh();
        
    }
    
    
}

// Creating new account
void create_account() {
    clear_space();
    noecho();
    refresh();
    
    // username, password, email
    Player *new_player = malloc(sizeof(Player));
    

    
    main_menu();
}


// Logging in an account
void login_account() {
    clear_space();
    noecho();
    refresh();

    

    main_menu();
}

#endif
