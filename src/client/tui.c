#include <stdlib.h>
#include <string.h>
#include "tui.h"

void tui_init_curses(void) {
    initscr();
    cbreak();
    halfdelay(1);
    noecho();
    nonl();

    start_color();
    use_default_colors();

    /* Default (white) foreground */
    init_pair(1, -1, -1);
    init_pair(2, -1, COLOR_BLACK);
    init_pair(3, -1, COLOR_RED);
    init_pair(4, -1, COLOR_GREEN);
    init_pair(5, -1, COLOR_YELLOW);
    init_pair(6, -1, COLOR_BLUE);
    init_pair(7, -1, COLOR_MAGENTA);
    init_pair(8, -1, COLOR_CYAN);
    init_pair(9, -1, COLOR_WHITE);

    /* Text in chat */
    init_pair(10, COLOR_BLACK, COLOR_GREEN);
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
    ui->chat  = newpad(1, main_cols);
    ui->input = newwin(1, main_cols, rows - 1, 0);
    ui->users = newwin(rows, user_cols, 0, main_cols);

    ui->fields[0] = new_field(1, main_cols - 2, 0, 0, 0, 0);
    ui->fields[1] = NULL;
    field_opts_off(ui->fields[0], O_AUTOSKIP);
    field_opts_off(ui->fields[0], O_STATIC);
    set_max_field(ui->fields[0], 9999);
    ui->form = new_form(ui->fields);

    set_form_win(ui->form, ui->input);
    set_form_sub(ui->form, derwin(ui->input, 1, main_cols - 2, 0, 2));
    mvwaddstr(ui->input, 0, 0, "> ");

    ui->chat_row = 0;
    meta(ui->input, true);
    keypad(ui->input, true);
    // nodelay(ui->input, true);

    wnoutrefresh(stdscr);
    post_form(ui->form);
    wnoutrefresh(ui->input);

    wbkgd(ui->info,  COLOR_PAIR(3));
    wbkgd(ui->chat,  COLOR_PAIR(4));
    wbkgd(ui->input, COLOR_PAIR(2));
    set_field_back(ui->fields[0], COLOR_PAIR(2));
    wbkgd(ui->users, COLOR_PAIR(6));
    wbkgd(stdscr, COLOR_PAIR(1));

    return ui;
}

void tui_refresh(tui *ui) {
    wnoutrefresh(stdscr);
    wnoutrefresh(ui->info);
    pnoutrefresh(ui->chat, ui->chat_row, 0, 1, 0, getmaxy(stdscr) - 2, getmaxx(stdscr) - 10);
    wnoutrefresh(ui->users);
    wnoutrefresh(ui->input);
    doupdate();
}

void tui_end(tui *ui) {
    unpost_form(ui->form);
    free_form(ui->form);
    free_field(ui->fields[0]);
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

static int tui_count_lines(tui *ui, char *s, int len, bool *b) {
    int lines = 0, line_len = len;
    for (int i = 0; s[i]; ++i) {
        ++line_len;
        if (s[i] == '\n' || line_len == getmaxx(ui->chat)) {
            ++lines;
            line_len = 0;
        }
    }
    *b = line_len > 0;
    return lines + (*b ? 1 : 0);
}

void tui_add_msg(tui *ui, tui_msg *msg) {
    bool newline;
    char time_buf[6];
    int lines = tui_count_lines(ui, msg->txt, 5 + 4 + strlen(msg->sender), &newline);

    wresize(ui->chat, getmaxy(ui->chat) + lines, getmaxx(ui->chat));
    strftime(time_buf, 6, "%H:%M", localtime(&msg->timestamp));
    wprintw(ui->chat, "%s <%s> %s", time_buf, msg->sender, msg->txt);
    if (newline)
        waddch(ui->chat, '\n');
    if (getcury(ui->chat) == getmaxy(ui->chat) - 1 && getmaxy(ui->chat) > getmaxy(stdscr) - 2)
        ui->chat_row += lines;
}

void tui_add_txt(tui *ui, char *txt) {
    bool newline;
    int lines = tui_count_lines(ui, txt, strlen(txt), &newline);

    wresize(ui->chat, getmaxy(ui->chat) + lines, getmaxx(ui->chat));
    wattron(ui->chat, COLOR_PAIR(10));
    waddstr(ui->chat, txt);
    wattroff(ui->chat, COLOR_PAIR(10));
    if (newline)
        waddch(ui->chat, '\n');
    if (getcury(ui->chat) == getmaxy(ui->chat) - 1 && getmaxy(ui->chat) > getmaxy(stdscr) - 2)
        ui->chat_row += lines;
}
