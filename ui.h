#ifndef ui_h
#define ui_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

void hx(const char* hex, short* r, short* g, short* b) {
    int red, green, blue;
    sscanf(hex, "#%02x%02x%02x", &red, &green, &blue);

    *r = (short)((red / 255.0) * 1000);
    *g = (short)((green / 255.0) * 1000);
    *b = (short)((blue / 255.0) * 1000);
}

void clear_space() {
    noecho();
    // cbreak();
    curs_set(FALSE);
    refresh();
    for(int i = 0; i < 42; i++) {
        for(int j = 0; j < 148; j++) {
            move(i, j);
            addch(' ');
        }
    }
}


#define back_color "#c2eefc"
#define font_color "#046d8f"

#endif ui_h