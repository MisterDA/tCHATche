#include <stdlib.h>
#include "tui.h"

void tui_init_curses(void) {
    initscr();
    // halfdelay(127);
    cbreak();
    keypad(stdscr, true);
    noecho();
    nonl();
    // scrollok(stdscr, true);
    // leaveok(stdscr, false);

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_GREEN);
    init_pair(4, COLOR_WHITE, COLOR_YELLOW);
    init_pair(5, COLOR_WHITE, COLOR_BLUE);
    init_pair(6, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(7, COLOR_WHITE, COLOR_CYAN);
    init_pair(8, COLOR_WHITE, COLOR_WHITE);
}

void tui_end_curses(void) {
    endwin();
}

tui *tui_init(void) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int user_cols = 10;
    int main_cols = cols - user_cols;

    tui *ui = malloc(sizeof(tui));
    ui->info  = newwin(1, main_cols, 0, 0);
    ui->chat  = newwin(rows - 2, main_cols, 1, 0);
    ui->input = newwin(1, main_cols, rows - 1, 0);
    ui->users = newwin(rows, user_cols, 0, main_cols);

    ui->fields[0] = new_field(1, main_cols, 0, 0, 0, 0);
    ui->fields[1] = NULL;
    field_opts_off(ui->fields[0], O_AUTOSKIP);
    field_opts_off(ui->fields[0], O_STATIC);
    set_max_field(ui->fields[0], 9999);
    ui->form = new_form(ui->fields);

    set_form_win(ui->form, ui->input);
    set_form_sub(ui->form, derwin(ui->input, 1, main_cols, 0, 0));

    keypad(ui->input, true);

    wnoutrefresh(stdscr);
    post_form(ui->form);
    wnoutrefresh(ui->input);

    wbkgd(ui->info,  COLOR_PAIR(2));
    wbkgd(ui->chat,  COLOR_PAIR(3));
    wbkgd(ui->input, COLOR_PAIR(4));
    wbkgd(ui->users, COLOR_PAIR(5));

    return ui;
}

void tui_refresh(tui *ui) {
    wnoutrefresh(stdscr);
    wnoutrefresh(ui->info);
    wnoutrefresh(ui->chat);
    wnoutrefresh(ui->users);
    wnoutrefresh(ui->input);
    doupdate();
}

void tui_end(tui *ui) {
    delwin(ui->info);
    delwin(ui->chat);
    delwin(ui->input);
    delwin(ui->users);
    free(ui);
}

void tui_print_info(tui *ui, int ch) {
    werase(ui->info);
    waddstr(ui->info, "tCHATche");
    mvwprintw(ui->info, 0, 40, "%04o - %s        ", ch, keyname(ch));
    mvwaddstr(ui->info, 0, 61, keyname(TUI_QUIT));
}
