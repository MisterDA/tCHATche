#ifndef TUI_H
#define TUI_H

#include <ncurses.h>
#include <form.h>
#include <time.h>

#define TUI_QUIT KEY_F(1)

typedef struct tui tui;
struct tui {
    WINDOW *info, *chat, *input, *users;
    FORM *form;
    FIELD *fields[2];

    int chat_row;
};

typedef struct tui_msg tui_msg;
struct tui_msg {
    time_t timestamp;
    char *sender;
    char *txt;
};

void tui_init_curses(void);
void tui_end_curses(void);

tui *tui_init(void);
void tui_end(tui *);

void tui_refresh(tui *);

void tui_print_info(tui *, int);
void tui_add_msg(tui *, tui_msg*);

#endif
