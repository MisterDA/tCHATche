#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <ncurses.h>
#include <ctype.h>
#include <assert.h>
#include "tchatche.h"
#include "tui.h"

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    tui_init_curses();
    tui *ui = tui_init();

    tui_refresh(ui);

    int ch = 0;
    while ((ch = wgetch(ui->input)) != TUI_QUIT) {
        tui_print_info(ui, ch);
        tui_refresh(ui);
        switch (ch) {
        case KEY_LEFT:
            form_driver(ui->form, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT:
            form_driver(ui->form, REQ_NEXT_CHAR);
            break;
        case '\177': /* DEL */
            form_driver(ui->form, REQ_DEL_PREV);
            if (data_behind(ui->form)) {
                if (getcurx(ui->input) > 0) {
                    form_driver(ui->form, REQ_SCR_BCHAR);
                }
                form_driver(ui->form, REQ_NEXT_CHAR);
            } else if (getcurx(ui->input) == 0) {
                form_driver(ui->form, REQ_DEL_CHAR);
            }
            break;
        default:
            form_driver(ui->form, ch);
            break;
        }
    }

    tui_end(ui);
    tui_end_curses();
    return EXIT_SUCCESS;
}
