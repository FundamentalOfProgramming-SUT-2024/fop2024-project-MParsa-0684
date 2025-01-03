/*
    ROGUE PROJECT
    Author : MohammadParsa MahmoudabadiArani / 403106679 / 2024-2025

 */

#include <stdio.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "menus.h"
#include <time.h>
#include <dirent.h>

void init_game();

int main() {

    initscr();
    // clear_space();
    noecho();
    cbreak();
    // curs_set(FALSE);
    keypad(stdscr, true);
    refresh();

    // Error for not supporting colors
    if(has_colors() == false) {
        move(21, 56);
        attron(A_BOLD);
        addstr("YOUR DEVICE DOESN'T SUPPORT COLORS!");
        move(22, 59);
        addstr("PRESS ANY KEY TO EXIT...");
        attroff(A_BOLD);
        move(23, 67);
        getch();
        refresh();
        endwin();
        return 0;

    }

    // Starting game
    refresh();
    init_game();

    endwin();
    return 0;
}

void init_game(){
    while (true)
    {
        main_menu();
    }
}