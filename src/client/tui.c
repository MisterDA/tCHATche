#include "tui.h"

#include <stdlib.h>
#include <string.h>
#include "arraylist.h"

#define PAIR_INFO 1
#define PAIR_TIME 2
#define PAIR_BCST 3
#define PAIR_PRVT 4

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
    init_pair(PAIR_INFO, -1, COLOR_RED); /* ui->info bkgd */
    init_pair(PAIR_TIME, COLOR_CYAN, -1);
    init_pair(PAIR_BCST, COLOR_YELLOW, -1);
    init_pair(PAIR_PRVT, COLOR_MAGENTA, -1);
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
    ui->title = strdup("tCHATche");

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
    if (ui->title)
    	mvwaddstr(ui->info, 0, 0, ui->title);
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
    
    wattron(ui->chat, COLOR_PAIR(PAIR_TIME));
    wprintw(ui->chat, "%s", time_buf);
    wattroff(ui->chat, COLOR_PAIR(PAIR_TIME));
    
    wattron(ui->chat, COLOR_PAIR(PAIR_BCST));
    wattron(ui->chat, A_BOLD);
    wprintw(ui->chat, " <%s> ", msg->sender);
    wattroff(ui->chat, A_BOLD);
    wattroff(ui->chat, COLOR_PAIR(PAIR_BCST));
    
    wprintw(ui->chat, "%s", msg->txt);
    
    if (newline)
        waddch(ui->chat, '\n');
    if (getcury(ui->chat) == getmaxy(ui->chat) - 1 &&
        getmaxy(ui->chat) > LINES - 2)
        ui->chat_row += lines;
}

void
tui_add_prvt_msg(tui *ui, tui_msg *msg, bool to)
{
    bool newline;
    char time_buf[6];
    int lines = tui_count_lines(ui, msg->txt, 5 + 4 + strlen(msg->sender),
                                &newline);

    wresize(ui->chat, getmaxy(ui->chat) + lines, getmaxx(ui->chat));
    strftime(time_buf, 6, "%H:%M", localtime(&msg->timestamp));
    
    wattron(ui->chat, COLOR_PAIR(PAIR_TIME));
    wprintw(ui->chat, "%s", time_buf);
    wattroff(ui->chat, COLOR_PAIR(PAIR_TIME));
    
    wattron(ui->chat, COLOR_PAIR(PAIR_PRVT));
    wattron(ui->chat, A_BOLD);
    if (to) {
    	wprintw(ui->chat, " [");
    	wattron(ui->chat, A_UNDERLINE);
    	wprintw(ui->chat, "me");
    	wattroff(ui->chat, A_UNDERLINE);
    	wprintw(ui->chat, "->%s] ", msg->sender);
    } else {
    	wprintw(ui->chat, " [%s->", msg->sender);
    	wattron(ui->chat, A_UNDERLINE);
    	wprintw(ui->chat, "me");
    	wattroff(ui->chat, A_UNDERLINE);
    	wprintw(ui->chat, "] ");
    }
    wattroff(ui->chat, A_BOLD);
    wattroff(ui->chat, COLOR_PAIR(PAIR_PRVT));
    
    wprintw(ui->chat, "%s", msg->txt);
    
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
    char *buf;
    vasprintf(&buf, fmt, varglist);
    tui_add_txt(ui, buf);
    free(buf);
}

void
tui_add_user(tui *ui, uint32_t n, char *nick)
{
    static uint32_t total = -1;
    static arlist *users = NULL;

    if (total != n) {
        total = n;
        if (users) arlist_destroy(users, free);
        users = arlist_create();
    }

    arlist_push(users, strdup(nick));
    if (n == arlist_size(users)) {
        char *str;
        size_t len;
        for (size_t i = 0; i < arlist_size(users); ++i) {
            len = 0;
            str = arlist_get(users, i);
            len += strlen(str) + 1;
            wresize(ui->chat, getmaxy(ui->chat) + 2, getmaxx(ui->chat));
            if (len > (size_t)COLS) {
                waddch(ui->chat, '\n');
            }
            waddstr(ui->chat, arlist_get(users, i));
            waddch(ui->chat, '\t');
        }
        waddch(ui->chat, '\n');
        arlist_destroy(users, free);
        total = -1;
        users = NULL;
    }
}

void
tui_clear_field(tui *ui)
{
    form_driver(ui->form, REQ_CLR_FIELD);
}
