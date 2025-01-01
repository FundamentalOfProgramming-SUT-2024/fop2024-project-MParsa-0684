#ifndef ui_h
#define ui_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "menus.h"

#define back_color "#c2eefc"
#define font_color "#046d8f"

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
    short r, g, b;
    hx(back_color, &r, &g, &b);
    init_color(COLOR_BLACK, r, g, b);
    short rr, gg, bb;
    hx(font_color, &rr, &gg, &bb);
    init_color(COLOR_BLUE, rr, gg, bb);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    attron(COLOR_PAIR(2));
    for(int i = 0; i < 148; i++) {
        move(0, i);
        addch(' ');
    }
    attroff(COLOR_PAIR(2));
}



#endif ui_h