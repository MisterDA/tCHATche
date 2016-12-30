#include <stdlib.h>
#include <string.h>
#include "tui.h"

void
tui_init_curses(void)
{
    initscr();
    cbreak();
    halfdelay(1);
    noecho();
    nonl();

    start_color();
    use_default_colors();
    init_pair(1, -1, COLOR_RED); /* ui->info bkgd */
}

void
tui_end_curses(void)
{
    endwin();
}

tui *
tui_init(void)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    tui *ui = malloc(sizeof(tui));
    ui->info  = newwin(1, cols, 0, 0);
    ui->chat  = newpad(1, cols);
    ui->input = newwin(1, cols, rows - 1, 0);

    ui->fields[0] = new_field(1, cols - 2, 0, 0, 0, 0);
    ui->fields[1] = NULL;
    field_opts_off(ui->fields[0], O_AUTOSKIP);
    field_opts_off(ui->fields[0], O_STATIC);
    set_max_field(ui->fields[0], 9999);
    ui->form = new_form(ui->fields);

    set_form_win(ui->form, ui->input);
    set_form_sub(ui->form, derwin(ui->input, 1, cols - 2, 0, 2));
    mvwaddstr(ui->input, 0, 0, "> ");

    ui->chat_row = 0;
    meta(ui->input, true);
    keypad(ui->input, true);
    // nodelay(ui->input, true);

    wnoutrefresh(stdscr);
    post_form(ui->form);
    wnoutrefresh(ui->input);

    wbkgd(ui->info, COLOR_PAIR(1));

    return ui;
}

void
tui_refresh(tui *ui)
{
    wnoutrefresh(stdscr);
    wnoutrefresh(ui->info);
    pnoutrefresh(ui->chat, ui->chat_row, 0, 1, 0, LINES - 2, COLS - 10);
    wnoutrefresh(ui->input);
    doupdate();
}

void
tui_end(tui *ui)
{
    unpost_form(ui->form);
    free_form(ui->form);
    free_field(ui->fields[0]);
    delwin(ui->info);
    delwin(ui->chat);
    delwin(ui->input);
    free(ui);
}

void
tui_print_info(tui *ui, int ch)
{
    werase(ui->info);
    waddstr(ui->info, "tCHATche");
    const char *fmt = "%04o - %s        ";
    mvwprintw(ui->info, 0, getmaxx(ui->info) / 2 - sizeof("xxxx -") / 2,
              fmt, ch, keyname(ch));
    mvwaddstr(ui->info, 0, getmaxx(ui->info) - sizeof("Ctrl-D"), "Ctrl-D");
}

static int
tui_count_lines(tui *ui, const char *s, int len, bool *b)
{
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

void
tui_add_msg(tui *ui, tui_msg *msg)
{
    bool newline;
    char time_buf[6];
    int lines = tui_count_lines(ui, msg->txt, 5 + 4 + strlen(msg->sender),
                                &newline);

    wresize(ui->chat, getmaxy(ui->chat) + lines, getmaxx(ui->chat));
    strftime(time_buf, 6, "%H:%M", localtime(&msg->timestamp));
    wprintw(ui->chat, "%s <%s> %s", time_buf, msg->sender, msg->txt);
    if (newline)
        waddch(ui->chat, '\n');
    if (getcury(ui->chat) == getmaxy(ui->chat) - 1 &&
        getmaxy(ui->chat) > LINES - 2)
        ui->chat_row += lines;
}

void
tui_add_txt(tui *ui, const char *txt)
{
    bool newline;
    int lines = tui_count_lines(ui, txt, 0, &newline);

    wresize(ui->chat, getmaxy(ui->chat) + lines, getmaxx(ui->chat));
    wattron(ui->chat, A_DIM);
    waddstr(ui->chat, txt);
    wattroff(ui->chat, A_DIM);
    if (newline)
        waddch(ui->chat, '\n');
    if (getcury(ui->chat) == getmaxy(ui->chat) - 1 &&
        getmaxy(ui->chat) > LINES - 2)
        ui->chat_row += lines;
}

void
tui_print_txt(tui *ui, const char *fmt, ...)
{
    va_list varglist;
    va_start(varglist, fmt);
    tui_vprint_txt(ui, fmt, varglist);
    va_end(varglist);
}

void
tui_vprint_txt(tui *ui, const char *fmt, va_list varglist)
{
    wattron(ui->chat, A_DIM);
    vwprintw(ui->chat, fmt, varglist);
    wattroff(ui->chat, A_DIM);
}

void
tui_add_user(tui *ui, char *nick)
{
    waddstr(ui->chat, nick);
}

void
tui_clear_field(tui *ui)
{
    form_driver(ui->form, REQ_CLR_FIELD);
}
