#ifndef TUI_H
#define TUI_H

#include <form.h>
#include <time.h>
#include <stdarg.h>

typedef struct tui tui;
struct tui {
    WINDOW *info, *chat, *input;
    FORM *form;
    FIELD *fields[2];

    int chat_row;
    char *title;
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

void tui_add_msg(tui *, tui_msg *);
void tui_add_prvt_msg(tui *, tui_msg *, bool to);
void tui_add_txt(tui *, const char *);
void tui_print_txt(tui *, const char *, ...);
void tui_vprint_txt(tui *, const char *, va_list);

void tui_add_user(tui *,  uint32_t, char *);

void tui_clear_field(tui *);

#endif
