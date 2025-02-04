#ifndef ui_h
#define ui_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncursesw/ncurses.h>
#include "menus.h"
#include "ui.h"
#include <locale.h>

#define back_color "#c2eefc"
#define font_color "#046d8f"
#define Firebrick_color "#B22222"
#define Forestgreen_color "#228B22"
#define Dimgray_color "#696969"
#define Gold_color "#FFD700"
#define Darkorage_color "#FF8C00"
#define Black_black "#000000"
#define COLOR_FIREBRICK 8
#define COLOR_FORESTGREEN 9
#define COLOR_DIMGRAY 10
#define COLOR_GOLD 11
#define COLOR_DARKORANGE 12
#define BLACK_BLACK 13

char *ROGUE[] = {
    {"███████╗ ██████╗  ██████╗ ██╗   ██╗███████╗"},
    {"██╔══██║██╔═══██╗██╔════╝ ██║   ██║██╔════╝"},
    {"███████╝██║   ██║██║  ███╗██║   ██║█████╗  "},
    {"██╔═╗██║██║   ██║██║   ██║██║   ██║██╔══╝  "},
    {"██║ ║██║╚██████╔╝╚██████╔╝ ╚█████╝ ███████╗"},
    {"╚═╝ ╚═╝  ╚═════╝  ╚═════╝   ╚═══╝  ╚══════╝"},
};

char *GAME[] = {
    {" ██████╗  █████╗ ███╗   ███╗███████╗"},
    {"██╔════╝ ██╔══██╗████╗ ████║██╔════╝"},
    {"██║  ███╗███████║██╔████╔██║█████╗  "},
    {"██║   ██║██╔══██║██║╚██╔╝██║██╔══╝  "},
    {"╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗"},
    {" ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝"},
};

char *BOX[] = {
    {"╔══════════════════════════════╗"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"║                              ║"},
    {"╚══════════════════════════════╝"}
};

char *BOX2[] = {
    {"╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"║                                                               ║                                          ║                                   ║"},
    {"╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝"}
};

char *BOX3[] = {
    {"╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"║                                                                                                                                              ║"},
    {"╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝"}
};


/*



╔═════════════════════════╗
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
║                         ║
╚═════════════════════════╝ 

*/


/*

char *ROGUE[] = {
    {"#######╗ ######╗  ######╗ ##╗   ##╗#######╗\0"},
    {"##╔══##║##╔═══##╗##╔════╝ ##║   ##║##╔════╝\0"},
    {"#######╝##║   ##║##║  ###╗##║   ##║#####╗  \0"},
    {"##╔═╗##║##║   ##║##║   ##║##║   ##║##╔══╝  \0"},
    {"##║ ║##║╚######╔╝╚######╔╝ ╚#####╝ #######╗\0"},
    {"╚═╝ ╚═╝  ╚═════╝  ╚═════╝   ╚═══╝  ╚══════╝\0"},
};

char *GAME[] = {
    {" ######╗  #####╗ ###╗   ###╗#######╗\0"},
    {"##╔════╝ ##╔══##╗####╗ ####║##╔════╝\0"},
    {"##║  ###╗#######║##╔####╔##║#####╗  \0"},
    {"##║   ##║##╔══##║##║╚##╔╝##║##╔══╝  \0"},
    {"╚######╔╝##║  ##║##║ ╚═╝ ##║#######╗\0"},
    {" ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝\0"},
};

*/

// char ROGUE[6][45] = {
//     {"#######| ######|  ######|  ##|  ##| ######|"},
//     {"##|  ##| ##|  ##| ##|      ##|  ##| ##|    "},
//     {"#######| ##|  ##| ##| ###| ##|  ##| #####| "},
//     {"##|  ##| ##|  ##| ##|  ##| ##|  ##| ##|    "},
//     {"##|  ##| ######|  ######|   #####|  ######|"},
//     {"##|  ##| ######|   ####|     ###|   ######|"},
// };

// char GAME[6][38] = {
//     {"######|  ######| ###|  ###| #######|"},
//     {"##|      ##  ##| ####|####| ##|     "},
//     {"##| ###| ######| ##|######| #####|  "},
//     {"##|  ##| ##| ##| ##| ## ##| ##|     "},
//     {"######|  ##| ##| ##|    ##| #######|"},
//     {"######|  ##| ##| ##|    ##| #######|"},
// };

char HAPPY_FACE[6][15] = {
    {"   ______   "},
    {" /  _   _ \\ "},
    {"|   .   .  |"},
    {"|     |    |"},
    {"|   \\___/  |"},
    {" \\________/ "}
};

char SAD_FACE[6][14] = {
    {"   ______   "},
    {" /  _   _ \\ "},
    {"|   .   .  |"},
    {"|     |    |"},
    {"|   _____  |"},
    {" \\________/ "}
};


// char ROGUE[6][45] = {
//     {" ____     _____   _______   _   _    _____ "},
//     {"|  _ \\   |  _  | |  ____|  | | | |  |  ___|"},
//     {"| |_| |  | | | | | | ___   | | | |  | |__  "},
//     {"|  _ /   | | | | | | |_ |  | | | |  |  __| "},
//     {"| | \\ \\  | |_| | | |__| |  | |_| |  | |___ "},
//     {"|_|  \\_\\ |_____| |______/  |_____|  |_____|"},
// };

// char GAME[6][38] = {
//     {" _______  _____   _    _   _____  "},
//     {"|  ____| |  _  | | |  | | |  ___| "},
//     {"| | ___  | |_| | | \\  / | | |__   "},
//     {"| | |_ | |  _  | | |\\/| | |  __|  "},
//     {"| |__| | | | | | | |  | | | |___  "},
//     {"|______/ |_| |_| |_|  |_| |_____|  "},
// };


void hx(const char* hex, short* r, short* g, short* b) {
    int red, green, blue;
    sscanf(hex, "#%02x%02x%02x", &red, &green, &blue);

    *r = (short)((red / 255.0) * 1000);
    *g = (short)((green / 255.0) * 1000);
    *b = (short)((blue / 255.0) * 1000);
}

void clear_space() {
    
    clear();
    // Page UI
    start_color();
    bkgd(COLOR_PAIR(1));

    short r, g, b;
    hx(back_color, &r, &g, &b);
    init_color(COLOR_BLACK, r, g, b);
    short rr, gg, bb;
    hx(font_color, &rr, &gg, &bb);
    init_color(COLOR_BLUE, rr, gg, bb);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    init_pair(3, COLOR_RED, COLOR_BLUE);

    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);

    hx(Firebrick_color, &r, &g, &b);
    init_color(COLOR_FIREBRICK, r, g, b);
    init_pair(8, COLOR_FIREBRICK, COLOR_BLACK);
    
    hx(Forestgreen_color, &r, &g, &b);
    init_color(COLOR_FORESTGREEN, r, g, b);
    init_pair(9, COLOR_FORESTGREEN, COLOR_BLACK);
    
    hx(Dimgray_color, &r, &g, &b);
    init_color(COLOR_DIMGRAY, r, g, b);
    init_pair(10, COLOR_DIMGRAY, COLOR_BLACK);
    
    hx(Gold_color, &r, &g, &b);
    init_color(COLOR_GOLD, r, g, b);
    init_pair(11, COLOR_GOLD, COLOR_BLACK);
    
    hx(Darkorage_color, &r, &g, &b);
    init_color(COLOR_DARKORANGE, r, g, b);
    init_pair(12, COLOR_DARKORANGE, COLOR_BLACK);
    
    init_pair(13, COLOR_RED, COLOR_BLACK);
    init_pair(14, COLOR_GREEN, COLOR_BLACK);
    init_pair(15, COLOR_RED, COLOR_BLUE);
    init_pair(16, COLOR_GREEN, COLOR_BLUE);
    init_pair(17, COLOR_DARKORANGE, COLOR_BLUE);
    init_pair(18, COLOR_GOLD, COLOR_BLUE);

    hx(Black_black, &r, &g, &b);
    init_color(BLACK_BLACK, r, g, b);
    init_pair(19, BLACK_BLACK, COLOR_BLACK);
    init_pair(20, COLOR_BLACK, COLOR_FORESTGREEN);
    init_pair(21, COLOR_BLACK, COLOR_DARKORANGE);
    init_pair(22, COLOR_BLACK, BLACK_BLACK);
    /*
#define FIREBRICK 8
#define FORESTGREEN 9
#define DIMGRAY 10
#define GOLD 11
#define DARKORANGE 12
    
    */
    attron(COLOR_PAIR(2));
    for(int i = 0; i < 148; i++) {
        move(0, i);
        addch(' ');
    }
    for(int i = 0; i < 148; i++) {
        move(41, i);
        addch(' ');
    }
    for(int j = 0; j < 42; j++) {
        move(j, 0);
        addch(' ');
    }
    for(int j = 0; j < 42; j++) {
        move(j, 147);
        addch(' ');
    }
    attroff(COLOR_PAIR(2));
}

void clear_space2() {

    short r, g, b;
    hx(back_color, &r, &g, &b);
    init_color(COLOR_BLACK, r, g, b);
    short rr, gg, bb;
    hx(font_color, &rr, &gg, &bb);
    init_color(COLOR_BLUE, rr, gg, bb);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    init_pair(3, COLOR_RED, COLOR_BLUE);

    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);


    hx(Firebrick_color, &r, &g, &b);
    init_color(COLOR_FIREBRICK, r, g, b);
    init_pair(8, COLOR_FIREBRICK, COLOR_BLACK);
    
    hx(Forestgreen_color, &r, &g, &b);
    init_color(COLOR_FORESTGREEN, r, g, b);
    init_pair(9, COLOR_FORESTGREEN, COLOR_BLACK);
    
    hx(Dimgray_color, &r, &g, &b);
    init_color(COLOR_DIMGRAY, r, g, b);
    init_pair(10, COLOR_DIMGRAY, COLOR_BLACK);
    
    hx(Gold_color, &r, &g, &b);
    init_color(COLOR_GOLD, r, g, b);
    init_pair(11, COLOR_GOLD, COLOR_BLACK);
    
    hx(Darkorage_color, &r, &g, &b);
    init_color(COLOR_DARKORANGE, r, g, b);
    init_pair(12, COLOR_DARKORANGE, COLOR_BLACK);

    init_pair(13, COLOR_RED, COLOR_BLACK);
    init_pair(14, COLOR_GREEN, COLOR_BLACK);
    init_pair(15, COLOR_RED, COLOR_BLUE);
    init_pair(16, COLOR_GREEN, COLOR_BLUE);
    init_pair(17, COLOR_DARKORANGE, COLOR_BLUE);
    init_pair(18, COLOR_GOLD, COLOR_BLUE);

    hx(Black_black, &r, &g, &b);
    init_color(BLACK_BLACK, r, g, b);
    init_pair(19, BLACK_BLACK, COLOR_BLACK);
    init_pair(20, COLOR_BLACK, COLOR_FORESTGREEN);
}



#endif