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
#include <unistd.h> 


void main_menu();
void create_player();
void login_player();
int check_player();
void guest_login();
void official_login();
int check_login(char *usern, char *passw);
void player_menu();
void game_intro();
void game_end();

// Main menu of game
void main_menu() {

    // Initial setups
    // setlocale(LC_ALL, "");
    noecho();
    // cbreak();
    curs_set(FALSE);
    // setlocale(LC_ALL, "");
    refresh();

    // Page UI
    start_color();    
    clear_space();


    // Header UI
    attron(A_BOLD | COLOR_PAIR(1));

    for(int i = 0; i < 14; i++) {
        move(10 + i, 58);
        addstr(BOX[i]);
    }

    move(12 ,69);
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

        attron(COLOR_PAIR(1));

        move(16 + location, 65);
        addstr(mainMenu[location]);
        attroff(COLOR_PAIR(1));
        
        switch (cursor) {
            case KEY_ENTER:
                if(location == 2) {
                    game_end();
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
    player->game_difficulty = 0;
    player->color = COLOR_FIREBRICK;
    player->music = (Music *) malloc(sizeof(Music));
    strcpy(player->music->music_path, "../music/05 Aen Seidhe.mp3");
    player->time_experience = time(NULL);
    fprintf(player_file, "%s\n%s\n%s\n%d\n%d\n%d\n%ld\n%d\n%d\n%s\n%s\n", player->username, player->password, player->email, player->total_score, player->total_gold, player->num_finished, player->time_experience, player->game_difficulty, player->color, player->music->music_path, player->game);
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
    attron(A_BOLD | COLOR_PAIR(1));

    for(int i = 0; i < 14; i++) {
        move(10 + i, 58);
        addstr(BOX[i]);
    }

    move(12 ,69);
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
    (player_file, "%s", player->username);
    // delete_enter(player->username);
    
    fscanf(player_file, "%s", player->password);
    // delete_enter(player->password);
    
    fscanf(player_file, "%s", player->email);
    // delete_enter(player->email);
    
    fscanf(player_file, "%d", &(player->total_score));
    fscanf(player_file, "%d", &(player->total_gold));
    fscanf(player_file, "%d", &(player->num_finished));
    fscanf(player_file, "%ld", &(player->time_experience));
    fscanf(player_file, "%d", &(player->game_difficulty));
    fscanf(player_file, "%d", &(player->color));
    player->music = (Music *) malloc(sizeof(Music));
    fscanf(player_file, "%s", player->music->music_path);
    // delete_enter(player->music->music_path);
    
    // char g[100];
    // fscanf(player_file, "%s", g);
    // fclose(player_file);
    // // fclose(player_file);

    // if(strcmp(g, "(null)") == 0) {
    //     player->game = NULL;    
    // }
    // else {
    //     // FILE *player_file = fopen(p)
    //     player->game = (Game *) calloc(1, sizeof(Game));
    //     Game *game = player->game;
    //     // game_name
    //     // fscanf(player_file, "%s", game->name);
    //     strcpy(game->name, g);
        
    //     // game_floors_calloc
    //     fscanf(player_file, "%d", &(game->floor_num));
    //     game->floors = (Floor *) calloc(game->floor_num, sizeof(Floor));
    //     for(int i = 0; i < game->floor_num; i++) {
    //     //     fclose(player_file);
    //     // FILE* player_file = fopen(player->file_path, "r");

    //         fscanf(player_file, "%d", &(game->floors[i].floor_visit));
    //         for(int ii = 0; ii < 40; ii++) {
    //             for(int jj = 0; jj < 146; jj++) 
    //                 fscanf(player_file, "%d ", &(game->floors[i].map[ii][jj]));
    //             // fscanf(player_file, "\n");
    //         }

    //         for(int ii = 0; ii < 40; ii++) {
    //             for(int jj = 0; jj < 146; jj++) 
    //                 fscanf(player_file, "%d", &(game->floors[i].visit[ii][jj]));
    //             // fscanf(player_file, "\n");
    //         }

    //         fscanf(player_file, "%d", &(game->floors[i].room_num));
    //         fscanf(player_file, "%d", &(game->floors[i].has_gold));
    //         fscanf(player_file, "%d", &(game->floors[i].staircase_num));
    //         fscanf(player_file, "%d", &(game->floors[i].master_key_num));

    //         game->floors[i].Rooms = (Room *) calloc(game->floors[i].room_num, sizeof(Room));
    //         for(int j = 0; j < game->floors[i].room_num; j++) {
    //             Room *room = &game->floors[i].Rooms[j];
    //             fscanf(player_file, "%d", &(room->start.y));
    //             fscanf(player_file, "%d", &(room->start.x));
    //             fscanf(player_file, "%d", &(room->size.y));
    //             fscanf(player_file, "%d", &(room->size.x));
    //             fscanf(player_file, "%d", &(room->type));
    //             int is;

    //             // foods
    //             fscanf(player_file, "%d", &(room->food_num));
    //             room->foods = (Food **) calloc(room->food_num, sizeof(Food *));
    //             for(int z = 0; z < room->food_num; z++) {
    //                 fscanf(player_file, "%d", &is);
    //                 if(is == -1)
    //                     continue;

    //                 room->foods[z] = (Food *) calloc(1, sizeof(Food));
    //                 room->foods[z]->location.y = is;
    //                 // fscanf(player_file, "%d", &);
    //                 fscanf(player_file, "%d", &(room->foods[z]->location.x));
    //                 fscanf(player_file, "%d", &(room->foods[z]->type));
    //                 fscanf(player_file, "%d", &(room->foods[z]->time_passed));
    //                 switch(room->foods[z]->type) {
    //                     case Ordinary:
    //                         room->foods[z]->unicode = 0x0001F34E;
    //                         break;
    //                     case Excellent:
    //                         room->foods[z]->unicode = 0x0001F370;
    //                         break;
    //                     case Magical:
    //                         room->foods[z]->unicode = 0x0001F347;
    //                         break;
    //                     case Toxic:
    //                         // 1F344
    //                         //unicode
    //                         break;
    //                 }
    //             }

    //             // golds
    //             fscanf(player_file, "%d", &(room->gold_num));
    //             room->golds = (Gold **) calloc(room->gold_num, sizeof(Gold *));
    //             for(int z = 0; z < room->gold_num; z++) {
    //                 fscanf(player_file, "%d", &is);
    //                 if(is == -1)
    //                     continue;

    //                 room->golds[z] = (Gold *) calloc(1, sizeof(Gold));
    //                 room->golds[z]->location.y = is;
    //                 fscanf(player_file, "%d", &(room->golds[z]->location.x));
    //                 fscanf(player_file, "%d", &(room->golds[z]->type));
    //                 switch(room->golds[z]->type) {
    //                     case Regular:
    //                         room->golds[z]->unicode = 0x0001F4B0;
    //                         break;
    //                     case Black:
    //                         room->golds[z]->unicode = 0x0001F6E2;
    //                         break;
    //                 }
    //             }
            
    //             // guns
    //             fscanf(player_file, "%d", &(room->gun_num));
    //             room->guns = (Gun **) calloc(room->gun_num, sizeof(Gun *));
    //             for(int z = 0; z < room->gun_num; z++) {
    //                 fscanf(player_file, "%d", &is);
    //                 if(is == -1)
    //                     continue;

    //                 room->guns[z] = (Gun *) calloc(1, sizeof(Gun));
    //                 room->guns[z]->location.y = is;
    //                 fscanf(player_file, "%d", &(room->guns[z]->location.x));
    //                 fscanf(player_file, "%d", &(room->guns[z]->type));
    //                 fscanf(player_file, "%d", &(room->guns[z]->damage));
    //                 fscanf(player_file, "%d", &(room->guns[z]->counter));
    //                 fscanf(player_file, "%d", &(room->guns[z]->distance));
    //                 // fscanf(player_file, "%s", room->guns[z]->name);
    //                 switch(room->guns[z]->type) {
    //                     case Mace:
    //                         strcpy(room->guns[z]->name, "Mace");
    //                         room->guns[z]->unicode = 0x0001F531;
    //                         break;
    //                     case Dagger:
    //                         strcpy(room->guns[z]->name, "Dagger");
    //                         room->guns[z]->unicode = 0x0001F528;
    //                         break;
    //                     case Magic_Wand:
    //                         strcpy(room->guns[z]->name, "Magic_Wand");
    //                         room->guns[z]->unicode = 0x0001F31F;
    //                         break;
    //                     case Normal_Arrow:
    //                         strcpy(room->guns[z]->name, "Normal_Arrow");
    //                         room->guns[z]->unicode = 0x0001F3F9;
    //                         break;
    //                     case Sword:
    //                         strcpy(room->guns[z]->name, "Sword");
    //                         room->guns[z]->unicode = 0x0001F5E1;
    //                         break;
    //                 }

    //             }
            
    //             //  spell
    //             fscanf(player_file, "%d", &(room->spell_num));
    //             room->spells = (Spell **) calloc(room->spell_num, sizeof(Spell *));
    //             for(int z = 0; z < room->spell_num; z++) {
    //                 fscanf(player_file, "%d", &is);
    //                 if(is == -1)
    //                     continue;

    //                 room->spells[z] = (Spell *) calloc(1, sizeof(Spell));
    //                 room->spells[z]->location.y = is;
    //                 fscanf(player_file, "%d", &(room->spells[z]->location.x));
    //                 fscanf(player_file, "%d", &(room->spells[z]->type));
    //                 switch(room->spells[z]->type) {
    //                     case Health:
    //                         room->spells[z]->unicode = 0x0001F52E;
    //                         break;
    //                     case Speed:
    //                         room->spells[z]->unicode = 0x0001F680;
    //                         break;
    //                     case Damage:
    //                         room->spells[z]->unicode = 0x0001F4A5;
    //                         break;
    //                 }   
    //             }
            
    //             // trap
    //             fscanf(player_file, "%d", &(room->trap_num));
    //             room->traps = (Trap **) calloc(room->trap_num, sizeof(Trap *));
    //             for(int z = 0; z < room->trap_num; z++) {
    //                 fscanf(player_file, "%d", &is);
    //                 if(is == -1)
    //                     continue;

    //                 room->traps[z] = (Trap *) calloc(1, sizeof(Trap));
    //                 room->traps[z]->location.y = is;
    //                 fscanf(player_file, "%d", &(room->traps[z]->location.x)); 
    //             }
            
    //             // enemy
    //             fscanf(player_file, "%d", &(room->enemy_num));
    //             room->enemies = (Enemy **) calloc(room->enemy_num, sizeof(Enemy *));
    //             for(int z = 0; z < room->enemy_num; z++) {
    //                 fscanf(player_file, "%d", &is);
    //                 if(is == -1)
    //                     continue;

    //                 room->enemies[z] = (Enemy *) calloc(1, sizeof(Enemy));
    //                 room->enemies[z]->location.y = is;
    //                 fscanf(player_file, "%d", &(room->enemies[z]->location.x));
    //                 fscanf(player_file, "%d", &(room->enemies[z]->type));
    //                 fscanf(player_file, "%d", &(room->enemies[z]->damage));
    //                 fscanf(player_file, "%d", &(room->enemies[z]->health));
    //                 fscanf(player_file, "%d", &(room->enemies[z]->following));
    //                 fscanf(player_file, "%d", &(room->enemies[z]->is_moving));
    //                 // fscanf(player_file, "%c", &room->enemies[z]->chr);
    //             switch(room->enemies[z]->type) {
    //                 case Deamon:
    //                     room->enemies[z]->chr = 'd';
    //                     room->enemies[z]->unicode = 0x0001F608;
    //                     break;
    //                 case Monster:
    //                     room->enemies[z]->chr = 'f';
    //                     room->enemies[z]->unicode = 0x0001F432;
    //                     break;
    //                 case Giant:
    //                     room->enemies[z]->chr = 'g';
    //                     room->enemies[z]->unicode = 0x0001F47B;
    //                     break;
    //                 case Snake:
    //                     room->enemies[z]->chr = 's';
    //                     room->enemies[z]->unicode = 0x0001F40D;
    //                     break;
    //                 case Undeed:
    //                     room->enemies[z]->chr = 'u';
    //                     room->enemies[z]->unicode = 0x0001F409;
    //                     break;
    //             }


    //             }

    //             // normal_doors
    //             fscanf(player_file, "%d", &(room->normal_doors_num));
    //             room->normal_doors = (Normal_Door *) calloc(room->normal_doors_num, sizeof(Normal_Door));
    //             for(int z = 0; z < room->normal_doors_num; z++) {
    //                 fscanf(player_file, "%d", &(room->normal_doors[z].location.y));
    //                 fscanf(player_file, "%d", &(room->normal_doors[z].location.x));
    //             }

    //             // secret_doors
    //             fscanf(player_file, "%d", &(room->secret_doors_num));
    //             room->secret_doors = (Secret_Door *) calloc(room->secret_doors_num, sizeof(Secret_Door));
    //             for(int z = 0; z < room->secret_doors_num; z++) {
    //                 fscanf(player_file, "%d", &(room->secret_doors[z].location.y));
    //                 fscanf(player_file, "%d", &(room->secret_doors[z].location.x));
    //             }
            
    //             // stair_case
    //             fscanf(player_file, "%d", &is);
    //             if(is != -1) {
    //                 room->staircase = (Staircase *) calloc(1, sizeof(Staircase));
    //                 room->staircase->location.y = is;
    //                 // fscanf(player_file, "%d", &room->staircase->location.y);
    //                 fscanf(player_file, "%d", &(room->staircase->location.x));
    //             }                
            
    //             // master_key
    //             fscanf(player_file, "%d", &is);
    //             if(is != -1) {
    //                 room->master_key = (Master_Key *) calloc(1, sizeof(Master_Key));
    //                 room->master_key->location.y = is;
    //                 // fscanf(player_file, "%d", &room->master_key->location.y);
    //                 fscanf(player_file, "%d", &(room->master_key->location.x));
    //             }
            

    //             //locked_door
    //             fscanf(player_file, "%d", &is);
    //             if(is != -1) {
    //                 room->locked_door = (Locked_Door *) calloc(1, sizeof(Locked_Door));
    //                 room->locked_door->location.y = is;
    //                 // fscanf(player_file, "%d", &room->locked_door->location.y);
    //                 fscanf(player_file, "%d", &(room->locked_door->location.x));
    //                 room->locked_door->password = -1;
    //                 fscanf(player_file, "%d", &(room->locked_door->is_visited));
    //                 fscanf(player_file, "%d", &(room->locked_door->password_turn));
    //             }
    //         }
    //     }
        
    //     // game_health
    //     int is;
    //     fscanf(player_file, "%d", &(game->Health));
        
    //     // game_food
    //     game->foods = (Food ***) calloc(4, sizeof(Food **));
    //     game->food_num = (int *) calloc(4, sizeof(int));
    //     for(int i = 0; i < 3; i++) {
    //         fscanf(player_file, "%d", &(game->food_num[i]));
    //         game->foods[i] = (Food **) calloc(game->food_num[i], sizeof(Food *));
    //         for(int z = 0; z < game->food_num[i]; z++) {
    //             fscanf(player_file, "%d", &is);
    //             if(is == -1)
    //                 continue;

    //             game->foods[z][0] = (Food *) calloc(1, sizeof(Food));
    //             game->foods[z][0]->location.y = is;
    //             // fscanf(player_file, "%d", &);
    //             fscanf(player_file, "%d", &(game->foods[z][0]->location.x));
    //             fscanf(player_file, "%d", &(game->foods[z][0]->type));
    //             fscanf(player_file, "%d", &(game->foods[z][0]->time_passed));
    //             switch(game->foods[z][0]->type) {
    //                 case Ordinary:
    //                     game->foods[z][0]->unicode = 0x0001F34E;
    //                     break;
    //                 case Excellent:
    //                     game->foods[z][0]->unicode = 0x0001F370;
    //                     break;
    //                 case Magical:
    //                     game->foods[z][0]->unicode = 0x0001F347;
    //                     break;
    //                 case Toxic:
    //                     // 1F344
    //                     //unicode
    //                     break;
    //             }
    //         }
    //     } 

    //     // game_gun
    //     game->gun = (Gun **) calloc(5, sizeof(Gun *));
    //     for(int i = 0; i < 5; i++) {
    //         fscanf(player_file, "%d", &is);
    //         if(is == -1)
    //             continue;
            
    //         game->gun[i] = (Gun *) calloc(1, sizeof(Gun));
    //         game->gun[i]->location.y = is;
    //         fscanf(player_file, "%d", &(game->gun[i]->location.x));
    //         fscanf(player_file, "%d", &(game->gun[i]->type));
    //         fscanf(player_file, "%d", &(game->gun[i]->damage));
    //         fscanf(player_file, "%d", &(game->gun[i]->counter));
    //         fscanf(player_file, "%d", &(game->gun[i]->distance));
    //         // fscanf(player_file, "%s", game->gun[i]->name);
    //         switch(game->gun[i]->type) {
    //             case Mace:
    //                 strcpy(game->gun[i]->name, "Mace");
    //                 game->gun[i]->unicode = 0x0001F531;
    //                 break;
    //             case Dagger:
    //                 strcpy(game->gun[i]->name, "Dagger");
    //                 game->gun[i]->unicode = 0x0001F528;
    //                 break;
    //             case Magic_Wand:
    //                 strcpy(game->gun[i]->name, "Magic_Wand");
    //                 game->gun[i]->unicode = 0x0001F31F;
    //                 break;
    //             case Normal_Arrow:
    //                 strcpy(game->gun[i]->name, "Normal_Arrow");
    //                 game->gun[i]->unicode = 0x0001F3F9;
    //                 break;
    //             case Sword:
    //                 strcpy(game->gun[i]->name, "Sword");
    //                 game->gun[i]->unicode = 0x0001F5E1;
    //                 break;
    //         }
    //     }
    //     fscanf(player_file, "%d", &is);
    //     game->current_gun = game->gun[is];

    //     // game_spell
    //     game->spell = (Spell ***) calloc(3, sizeof(Spell **));
    //     game->spell_num = (int *) calloc(3, sizeof(int));
    //     for(int i = 0; i < 3; i++) {
    //         fscanf(player_file, "%d", &(game->spell_num[i]));
    //         game->spell[i] = (Spell **) calloc(game->spell_num[i], sizeof(Spell *));
    //         for(int z = 0; z < game->spell_num[i]; z++) {
    //             fscanf(player_file, "%d", &is);
    //             if(is == -1)
    //                 continue;

    //             game->spell[z][0] = (Spell *) calloc(1, sizeof(Spell));
    //             game->spell[z][0]->location.y = is;
    //             fscanf(player_file, "%d", &(game->spell[z][0]->location.x));
    //             fscanf(player_file, "%d", &(game->spell[z][0]->type));
    //             switch(game->spell[z][0]->type) {
    //                 case Health:
    //                     game->spell[z][0]->unicode = 0x0001F52E;
    //                     break;
    //                 case Speed:
    //                     game->spell[z][0]->unicode = 0x0001F680;
    //                     break;
    //                 case Damage:
    //                     game->spell[z][0]->unicode = 0x0001F4A5;
    //                     break;
    //             }   
    //         }
    //     }

    //     // game_master_key
    //     fscanf(player_file, "%d", &(game->master_key_num));
    //     game->master_key = (Master_Key **) calloc(game->master_key_num, sizeof(Master_Key *));
    //     for(int i = 0; i < game->master_key_num; i++) {
    //         fscanf(player_file, "%d", &is);
    //         if(is == -1)
    //             continue;

    //         game->master_key[i] =  (Master_Key *) calloc(1, sizeof(Master_Key));
    //         game->master_key[i]->location.y = is;
    //         fscanf(player_file, "%d", &(game->master_key[i]->location.x));
    //     }

    //     game->player_unicode = 0x0001F47E;
    //     fscanf(player_file, "%d", &(game->total_gold));
    //     game->music = (Music *) calloc(1, sizeof(Music));
    //     strcpy(game->music->music_path, player->music->music_path);
    //     game->game_difficulty = player->game_difficulty;
    //     game->player_color = player->color; 

    //     fscanf(player_file, "%d", &(game->player_location.y));
    //     fscanf(player_file, "%d", &(game->player_location.x));
    //     fscanf(player_file, "%d", &(game->player_floor));
    //     fscanf(player_file, "%d", &(game->player_room));

    //     game->time_power = -1;
    // }

    fclose(player_file);
    player_menu();

    
}

// logging in guest player from login menu
void guest_login() {
    char file_path[200] = "../players/";
    strcat(file_path, "guest");
    strcat(file_path, ".txt");
    strcpy(player->file_path, file_path);
    FILE* player_file = fopen(player->file_path, "r");
    fscanf(player_file, "%s", player->username);
    // delete_enter(player->username);
    
    fscanf(player_file, "%s", player->password);
    // delete_enter(player->password);
    
    fscanf(player_file, "%s", player->email);
    // delete_enter(player->email);
    
    fscanf(player_file, "%d", &(player->total_score));
    fscanf(player_file, "%d", &(player->total_gold));
    fscanf(player_file, "%d", &(player->num_finished));
    fscanf(player_file, "%ld", &(player->time_experience));
    fscanf(player_file, "%d", &(player->game_difficulty));
    fscanf(player_file, "%d", &(player->color));
    player->music = (Music *) malloc(sizeof(Music));
    fscanf(player_file, "%s", player->music->music_path);
    // delete_enter(player->music->music_path);
    
    // char g[100];
    // fscanf(player_file, "%s", g);
    // if(strcmp(g, "(null)") == 0) {
    //     player->game = NULL;    
    // }
    // else {
    //     player->game = (Game *) calloc(1, sizeof(Game));
    //     Game *game = player->game;
    //     // game_name
    //     // fscanf(player_file, "%s", game->name);
    //     strcpy(game->name, g);
        
    //     // game_floors_calloc
    //     fscanf(player_file, "%d", &(game->floor_num));
    //     game->floors = (Floor *) calloc(game->floor_num, sizeof(Floor));
    //     for(int i = 0; i < game->floor_num; i++) {
    //         fscanf(player_file, "%d", &(game->floors[i].floor_visit));
    //         for(int ii = 0; ii < 40; ii++) {
    //             for(int jj = 0; jj < 146; jj++) 
    //                 fscanf(player_file, "%d ", &(game->floors[i].map[ii][jj]));
    //             // fscanf(player_file, "\n");
    //         }

    //         for(int ii = 0; ii < 40; ii++) {
    //             for(int jj = 0; jj < 146; jj++) 
    //                 fscanf(player_file, "%d", &(game->floors[i].visit[ii][jj]));
    //             // fscanf(player_file, "\n");
    //         }

    //         fscanf(player_file, "%d", &(game->floors[i].room_num));
    //         fscanf(player_file, "%d", &(game->floors[i].has_gold));
    //         fscanf(player_file, "%d", &(game->floors[i].staircase_num));
    //         fscanf(player_file, "%d", &(game->floors[i].master_key_num));

    //         game->floors[i].Rooms = (Room *) calloc(game->floors[i].room_num, sizeof(Room));
    //         for(int j = 0; j < game->floors[i].room_num; j++) {
    //             Room *room = &game->floors[i].Rooms[j];
    //             fscanf(player_file, "%d", &(room->start.y));
    //             fscanf(player_file, "%d", &(room->start.x));
    //             fscanf(player_file, "%d", &(room->size.y));
    //             fscanf(player_file, "%d", &(room->size.x));
    //             fscanf(player_file, "%d", &(room->type));
    //             int is;

    //             // foods
    //             fscanf(player_file, "%d", &(room->food_num));
    //             room->foods = (Food **) calloc(room->food_num, sizeof(Food *));
    //             for(int z = 0; z < room->food_num; z++) {
    //                 fscanf(player_file, "%d", &is);
    //                 if(is == -1)
    //                     continue;

    //                 room->foods[z] = (Food *) calloc(1, sizeof(Food));
    //                 room->foods[z]->location.y = is;
    //                 // fscanf(player_file, "%d", &);
    //                 fscanf(player_file, "%d", &(room->foods[z]->location.x));
    //                 fscanf(player_file, "%d", &(room->foods[z]->type));
    //                 fscanf(player_file, "%d", &(room->foods[z]->time_passed));
    //                 switch(room->foods[z]->type) {
    //                     case Ordinary:
    //                         room->foods[z]->unicode = 0x0001F34E;
    //                         break;
    //                     case Excellent:
    //                         room->foods[z]->unicode = 0x0001F370;
    //                         break;
    //                     case Magical:
    //                         room->foods[z]->unicode = 0x0001F347;
    //                         break;
    //                     case Toxic:
    //                         // 1F344
    //                         //unicode
    //                         break;
    //                 }
    //             }

    //             // golds
    //             fscanf(player_file, "%d", &(room->gold_num));
    //             room->golds = (Gold **) calloc(room->gold_num, sizeof(Gold *));
    //             for(int z = 0; z < room->gold_num; z++) {
    //                 fscanf(player_file, "%d", &is);
    //                 if(is == -1)
    //                     continue;

    //                 room->golds[z] = (Gold *) calloc(1, sizeof(Gold));
    //                 room->golds[z]->location.y = is;
    //                 fscanf(player_file, "%d", &(room->golds[z]->location.x));
    //                 fscanf(player_file, "%d", &(room->golds[z]->type));
    //                 switch(room->golds[z]->type) {
    //                     case Regular:
    //                         room->golds[z]->unicode = 0x0001F4B0;
    //                         break;
    //                     case Black:
    //                         room->golds[z]->unicode = 0x0001F6E2;
    //                         break;
    //                 }
    //             }
            
    //             // guns
    //             fscanf(player_file, "%d", &(room->gun_num));
    //             room->guns = (Gun **) calloc(room->gun_num, sizeof(Gun *));
    //             for(int z = 0; z < room->gun_num; z++) {
    //                 fscanf(player_file, "%d", &is);
    //                 if(is == -1)
    //                     continue;

    //                 room->guns[z] = (Gun *) calloc(1, sizeof(Gun));
    //                 room->guns[z]->location.y = is;
    //                 fscanf(player_file, "%d", &(room->guns[z]->location.x));
    //                 fscanf(player_file, "%d", &(room->guns[z]->type));
    //                 fscanf(player_file, "%d", &(room->guns[z]->damage));
    //                 fscanf(player_file, "%d", &(room->guns[z]->counter));
    //                 fscanf(player_file, "%d", &(room->guns[z]->distance));
    //                 // fscanf(player_file, "%s", room->guns[z]->name);
    //                 switch(room->guns[z]->type) {
    //                     case Mace:
    //                         strcpy(room->guns[z]->name, "Mace");
    //                         room->guns[z]->unicode = 0x0001F531;
    //                         break;
    //                     case Dagger:
    //                         strcpy(room->guns[z]->name, "Dagger");
    //                         room->guns[z]->unicode = 0x0001F528;
    //                         break;
    //                     case Magic_Wand:
    //                         strcpy(room->guns[z]->name, "Magic_Wand");
    //                         room->guns[z]->unicode = 0x0001F31F;
    //                         break;
    //                     case Normal_Arrow:
    //                         strcpy(room->guns[z]->name, "Normal_Arrow");
    //                         room->guns[z]->unicode = 0x0001F3F9;
    //                         break;
    //                     case Sword:
    //                         strcpy(room->guns[z]->name, "Sword");
    //                         room->guns[z]->unicode = 0x0001F5E1;
    //                         break;
    //                 }

    //             }
            
    //             //  spell
    //             fscanf(player_file, "%d", &(room->spell_num));
    //             room->spells = (Spell **) calloc(room->spell_num, sizeof(Spell *));
    //             for(int z = 0; z < room->spell_num; z++) {
    //                 fscanf(player_file, "%d", &is);
    //                 if(is == -1)
    //                     continue;

    //                 room->spells[z] = (Spell *) calloc(1, sizeof(Spell));
    //                 room->spells[z]->location.y = is;
    //                 fscanf(player_file, "%d", &(room->spells[z]->location.x));
    //                 fscanf(player_file, "%d", &(room->spells[z]->type));
    //                 switch(room->spells[z]->type) {
    //                     case Health:
    //                         room->spells[z]->unicode = 0x0001F52E;
    //                         break;
    //                     case Speed:
    //                         room->spells[z]->unicode = 0x0001F680;
    //                         break;
    //                     case Damage:
    //                         room->spells[z]->unicode = 0x0001F4A5;
    //                         break;
    //                 }   
    //             }
            
    //             // trap
    //             fscanf(player_file, "%d", &(room->trap_num));
    //             room->traps = (Trap **) calloc(room->trap_num, sizeof(Trap *));
    //             for(int z = 0; z < room->trap_num; z++) {
    //                 fscanf(player_file, "%d", &is);
    //                 if(is == -1)
    //                     continue;

    //                 room->traps[z] = (Trap *) calloc(1, sizeof(Trap));
    //                 room->traps[z]->location.y = is;
    //                 fscanf(player_file, "%d", &(room->traps[z]->location.x)); 
    //             }
            
    //             // enemy
    //             fscanf(player_file, "%d", &(room->enemy_num));
    //             room->enemies = (Enemy **) calloc(room->enemy_num, sizeof(Enemy *));
    //             for(int z = 0; z < room->enemy_num; z++) {
    //                 fscanf(player_file, "%d", &is);
    //                 if(is == -1)
    //                     continue;

    //                 room->enemies[z] = (Enemy *) calloc(1, sizeof(Enemy));
    //                 room->enemies[z]->location.y = is;
    //                 fscanf(player_file, "%d", &(room->enemies[z]->location.x));
    //                 fscanf(player_file, "%d", &(room->enemies[z]->type));
    //                 fscanf(player_file, "%d", &(room->enemies[z]->damage));
    //                 fscanf(player_file, "%d", &(room->enemies[z]->health));
    //                 fscanf(player_file, "%d", &(room->enemies[z]->following));
    //                 fscanf(player_file, "%d", &(room->enemies[z]->is_moving));
    //                 // fscanf(player_file, "%c", &room->enemies[z]->chr);
    //             switch(room->enemies[z]->type) {
    //                 case Deamon:
    //                     room->enemies[z]->chr = 'd';
    //                     room->enemies[z]->unicode = 0x0001F608;
    //                     break;
    //                 case Monster:
    //                     room->enemies[z]->chr = 'f';
    //                     room->enemies[z]->unicode = 0x0001F432;
    //                     break;
    //                 case Giant:
    //                     room->enemies[z]->chr = 'g';
    //                     room->enemies[z]->unicode = 0x0001F47B;
    //                     break;
    //                 case Snake:
    //                     room->enemies[z]->chr = 's';
    //                     room->enemies[z]->unicode = 0x0001F40D;
    //                     break;
    //                 case Undeed:
    //                     room->enemies[z]->chr = 'u';
    //                     room->enemies[z]->unicode = 0x0001F409;
    //                     break;
    //             }


    //             }

    //             // normal_doors
    //             fscanf(player_file, "%d", &(room->normal_doors_num));
    //             room->normal_doors = (Normal_Door *) calloc(room->normal_doors_num, sizeof(Normal_Door));
    //             for(int z = 0; z < room->normal_doors_num; z++) {
    //                 fscanf(player_file, "%d", &(room->normal_doors[z].location.y));
    //                 fscanf(player_file, "%d", &(room->normal_doors[z].location.x));
    //             }

    //             // secret_doors
    //             fscanf(player_file, "%d", &(room->secret_doors_num));
    //             room->secret_doors = (Secret_Door *) calloc(room->secret_doors_num, sizeof(Secret_Door));
    //             for(int z = 0; z < room->secret_doors_num; z++) {
    //                 fscanf(player_file, "%d", &(room->secret_doors[z].location.y));
    //                 fscanf(player_file, "%d", &(room->secret_doors[z].location.x));
    //             }
            
    //             // stair_case
    //             fscanf(player_file, "%d", &is);
    //             if(is != -1) {
    //                 room->staircase = (Staircase *) calloc(1, sizeof(Staircase));
    //                 room->staircase->location.y = is;
    //                 // fscanf(player_file, "%d", &room->staircase->location.y);
    //                 fscanf(player_file, "%d", &(room->staircase->location.x));
    //             }                
            
    //             // master_key
    //             fscanf(player_file, "%d", &is);
    //             if(is != -1) {
    //                 room->master_key = (Master_Key *) calloc(1, sizeof(Master_Key));
    //                 room->master_key->location.y = is;
    //                 // fscanf(player_file, "%d", &room->master_key->location.y);
    //                 fscanf(player_file, "%d", &(room->master_key->location.x));
    //             }
            

    //             //locked_door
    //             fscanf(player_file, "%d", &is);
    //             if(is != -1) {
    //                 room->locked_door = (Locked_Door *) calloc(1, sizeof(Locked_Door));
    //                 room->locked_door->location.y = is;
    //                 // fscanf(player_file, "%d", &room->locked_door->location.y);
    //                 fscanf(player_file, "%d", &(room->locked_door->location.x));
    //                 room->locked_door->password = -1;
    //                 fscanf(player_file, "%d", &(room->locked_door->is_visited));
    //                 fscanf(player_file, "%d", &(room->locked_door->password_turn));
    //             }
    //         }
    //     }
        
    //     // game_health
    //     int is;
    //     fscanf(player_file, "%d", &(game->Health));
        
    //     // game_food
    //     game->foods = (Food ***) calloc(4, sizeof(Food **));
    //     game->food_num = (int *) calloc(4, sizeof(int));
    //     for(int i = 0; i < 3; i++) {
    //         fscanf(player_file, "%d", &(game->food_num[i]));
    //         game->foods[i] = (Food **) calloc(game->food_num[i], sizeof(Food *));
    //         for(int z = 0; z < game->food_num[i]; z++) {
    //             fscanf(player_file, "%d", &is);
    //             if(is == -1)
    //                 continue;

    //             game->foods[z][0] = (Food *) calloc(1, sizeof(Food));
    //             game->foods[z][0]->location.y = is;
    //             // fscanf(player_file, "%d", &);
    //             fscanf(player_file, "%d", &(game->foods[z][0]->location.x));
    //             fscanf(player_file, "%d", &(game->foods[z][0]->type));
    //             fscanf(player_file, "%d", &(game->foods[z][0]->time_passed));
    //             switch(game->foods[z][0]->type) {
    //                 case Ordinary:
    //                     game->foods[z][0]->unicode = 0x0001F34E;
    //                     break;
    //                 case Excellent:
    //                     game->foods[z][0]->unicode = 0x0001F370;
    //                     break;
    //                 case Magical:
    //                     game->foods[z][0]->unicode = 0x0001F347;
    //                     break;
    //                 case Toxic:
    //                     // 1F344
    //                     //unicode
    //                     break;
    //             }
    //         }
    //     } 

    //     // game_gun
    //     game->gun = (Gun **) calloc(5, sizeof(Gun *));
    //     for(int i = 0; i < 5; i++) {
    //         fscanf(player_file, "%d", &is);
    //         if(is == -1)
    //             continue;
            
    //         game->gun[i] = (Gun *) calloc(1, sizeof(Gun));
    //         game->gun[i]->location.y = is;
    //         fscanf(player_file, "%d", &(game->gun[i]->location.x));
    //         fscanf(player_file, "%d", &(game->gun[i]->type));
    //         fscanf(player_file, "%d", &(game->gun[i]->damage));
    //         fscanf(player_file, "%d", &(game->gun[i]->counter));
    //         fscanf(player_file, "%d", &(game->gun[i]->distance));
    //         // fscanf(player_file, "%s", game->gun[i]->name);
    //         switch(game->gun[i]->type) {
    //             case Mace:
    //                 strcpy(game->gun[i]->name, "Mace");
    //                 game->gun[i]->unicode = 0x0001F531;
    //                 break;
    //             case Dagger:
    //                 strcpy(game->gun[i]->name, "Dagger");
    //                 game->gun[i]->unicode = 0x0001F528;
    //                 break;
    //             case Magic_Wand:
    //                 strcpy(game->gun[i]->name, "Magic_Wand");
    //                 game->gun[i]->unicode = 0x0001F31F;
    //                 break;
    //             case Normal_Arrow:
    //                 strcpy(game->gun[i]->name, "Normal_Arrow");
    //                 game->gun[i]->unicode = 0x0001F3F9;
    //                 break;
    //             case Sword:
    //                 strcpy(game->gun[i]->name, "Sword");
    //                 game->gun[i]->unicode = 0x0001F5E1;
    //                 break;
    //         }
    //     }
    //     fscanf(player_file, "%d", &is);
    //     game->current_gun = game->gun[is];

    //     // game_spell
    //     game->spell = (Spell ***) calloc(3, sizeof(Spell **));
    //     game->spell_num = (int *) calloc(3, sizeof(int));
    //     for(int i = 0; i < 3; i++) {
    //         fscanf(player_file, "%d", &(game->spell_num[i]));
    //         game->spell[i] = (Spell **) calloc(game->spell_num[i], sizeof(Spell *));
    //         for(int z = 0; z < game->spell_num[i]; z++) {
    //             fscanf(player_file, "%d", &is);
    //             if(is == -1)
    //                 continue;

    //             game->spell[z][0] = (Spell *) calloc(1, sizeof(Spell));
    //             game->spell[z][0]->location.y = is;
    //             fscanf(player_file, "%d", &(game->spell[z][0]->location.x));
    //             fscanf(player_file, "%d", &(game->spell[z][0]->type));
    //             switch(game->spell[z][0]->type) {
    //                 case Health:
    //                     game->spell[z][0]->unicode = 0x0001F52E;
    //                     break;
    //                 case Speed:
    //                     game->spell[z][0]->unicode = 0x0001F680;
    //                     break;
    //                 case Damage:
    //                     game->spell[z][0]->unicode = 0x0001F4A5;
    //                     break;
    //             }   
    //         }
    //     }

    //     // game_master_key
    //     fscanf(player_file, "%d", &(game->master_key_num));
    //     game->master_key = (Master_Key **) calloc(game->master_key_num, sizeof(Master_Key *));
    //     for(int i = 0; i < game->master_key_num; i++) {
    //         fscanf(player_file, "%d", &is);
    //         if(is == -1)
    //             continue;

    //         game->master_key[i] =  (Master_Key *) calloc(1, sizeof(Master_Key));
    //         game->master_key[i]->location.y = is;
    //         fscanf(player_file, "%d", &(game->master_key[i]->location.x));
    //     }

    //     game->player_unicode = 0x0001F47E;
    //     fscanf(player_file, "%d", &(game->total_gold));
    //     game->music = (Music *) calloc(1, sizeof(Music));
    //     strcpy(game->music->music_path, player->music->music_path);
    //     game->game_difficulty = player->game_difficulty;
    //     game->player_color = player->color; 

    //     fscanf(player_file, "%d", &(game->player_location.y));
    //     fscanf(player_file, "%d", &(game->player_location.x));
    //     fscanf(player_file, "%d", &(game->player_floor));
    //     fscanf(player_file, "%d", &(game->player_room));

    //     game->time_power = -1;
    // }


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
    attron(A_BOLD | COLOR_PAIR(1));

    for(int i = 0; i < 14; i++) {
        move(10 + i, 58);
        addstr(BOX[i]);
    }

    move(12 ,69);
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
        attron(A_BOLD | COLOR_PAIR(1));

        for(int i = 0; i < 14; i++) {
            move(10 + i, 58);
            addstr(BOX[i]);
        }
    
        move(12 ,69);
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
                    save_game();
                    main_menu();
                }
                (* pmenus[location]) ();
                break;

            case KEY_DOWN:
                location = (location + 1) % 5;
                if(location == 1 && player->game == NULL)
                    location++;
                break;

            case KEY_UP:
                location = (location - 1) % 5;
                location = (location + 5) % 5;
                if(location == 1 && player->game == NULL)
                    location--;
                break;
        }

        //Every move
        clear_space();
        attron(A_BOLD | COLOR_PAIR(1));

        for(int i = 0; i < 14; i++) {
            move(10 + i, 58);
            addstr(BOX[i]);
        }

        move(12 ,69);
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

void game_intro() {

    WINDOW *intro = newwin(42, 148, 0, 0);

    wattron(intro, COLOR_PAIR(2));
    for(int i = 0; i < 148; i++) {
        mvwaddch(intro, 0, i, ' ');
    }
    for(int i = 0; i < 148; i++) {
        mvwaddch(intro, 41, i, ' ');
    }
    for(int j = 0; j < 42; j++) {
        mvwaddch(intro, j, 0, ' ');
    }
    for(int j = 0; j < 42; j++) {
        mvwaddch(intro, j, 147, ' ');
    }
    wattroff(intro, COLOR_PAIR(2));
    
    wattron(intro, A_BOLD | COLOR_PAIR(1));
    // for(int j = 28; j < 108; j++) {
    //     for(int i = 14; i < 20; i++) {
    //         // move(i, j);
    //         if(j < 71)
    //             mvwprintw(intro, i, j, "%lc", ROGUE[i - 14][j - 28]);
    //         else
    //             mvwprintw(intro, i, j, "%lc", GAME[i - 14][j - 71]);
    //     }
    //     usleep(300 * 1000);
    //     wrefresh(intro);
    // }

    for(int i = 14; i < 20; i++) {
        mvwaddstr(intro, i, 28, ROGUE[i - 14]);
        mvwaddstr(intro, i, 78, GAME[i - 14]);
        usleep(200 * 1000);
        wrefresh(intro);
    }

    for(int j = 0; j < 19; j++) {
        wclear(intro);
        wattron(intro, COLOR_PAIR(2));
        for(int i = 0; i < 148; i++) {
            mvwaddch(intro, 0, i, ' ');
        }
        for(int i = 0; i < 148; i++) {
            mvwaddch(intro, 41, i, ' ');
        }
        for(int j = 0; j < 42; j++) {
            mvwaddch(intro, j, 0, ' ');
        }
        for(int j = 0; j < 42; j++) {
            mvwaddch(intro, j, 147, ' ');
        }
        wattroff(intro, COLOR_PAIR(2));

        wattron(intro, A_BOLD | COLOR_PAIR(1));
        for(int i = 14; i < 20; i++) {
            // move(i, 28 - j);
            mvwaddstr(intro, i, 28 - j, ROGUE[i - 14]);
            // move(i, 78 + j);
            mvwaddstr(intro, i, 78 + j, GAME[i - 14]);
        }
        wrefresh(intro);
        usleep(200 * 1000);
    }

    wattroff(intro, A_BOLD | COLOR_PAIR(1));
    delwin(intro);
}

void game_end() {
    WINDOW *intro = newwin(42, 148, 0, 0);

    wattron(intro, COLOR_PAIR(2));
    for(int i = 0; i < 148; i++) {
        mvwaddch(intro, 0, i, ' ');
    }
    for(int i = 0; i < 148; i++) {
        mvwaddch(intro, 41, i, ' ');
    }
    for(int j = 0; j < 42; j++) {
        mvwaddch(intro, j, 0, ' ');
    }
    for(int j = 0; j < 42; j++) {
        mvwaddch(intro, j, 147, ' ');
    }
    wattroff(intro, COLOR_PAIR(2));
    
    wattron(intro, A_BOLD | COLOR_PAIR(1));
    // for(int j = 28; j < 108; j++) {
    //     for(int i = 14; i < 20; i++) {
    //         // move(i, j);
    //         if(j < 71)
    //             mvwprintw(intro, i, j, "%lc", ROGUE[i - 14][j - 28]);
    //         else
    //             mvwprintw(intro, i, j, "%lc", GAME[i - 14][j - 71]);
    //     }
    //     usleep(300 * 1000);
    //     wrefresh(intro);
    // }

    for(int j = 0; j < 19; j++) {
        wclear(intro);
        wattron(intro, COLOR_PAIR(2));
        for(int i = 0; i < 148; i++) {
            mvwaddch(intro, 0, i, ' ');
        }
        for(int i = 0; i < 148; i++) {
            mvwaddch(intro, 41, i, ' ');
        }
        for(int j = 0; j < 42; j++) {
            mvwaddch(intro, j, 0, ' ');
        }
        for(int j = 0; j < 42; j++) {
            mvwaddch(intro, j, 147, ' ');
        }
        wattroff(intro, COLOR_PAIR(2));

        wattron(intro, A_BOLD | COLOR_PAIR(1));
        for(int i = 14; i < 20; i++) {
            // move(i, 28 - j);
            mvwaddstr(intro, i, 10 + j, ROGUE[i - 14]);
            // move(i, 78 + j);
            mvwaddstr(intro, i, 96 - j, GAME[i - 14]);
        }
        wrefresh(intro);
        usleep(200 * 1000);
    }

    usleep(1000 * 1000);
    wattroff(intro, A_BOLD | COLOR_PAIR(1));
    delwin(intro);
}
/*

    create new game
    continue old game
    score table
    settings

*/


#endif
