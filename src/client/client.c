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
#include <getopt.h>
#include <string.h>
#include "tchatche.h"
#include "tui.h"
#include "request.h"
#include "client.h"


/* Client init functions */

client *
client_init(void)
{
    client *cl = malloc(sizeof(client));
    cl->server_path = NULL;
    cl->client_path = NULL;
    cl->ui = NULL;
    cl->has_id = false;
    cl->id = 9999 + 1;
    cl->nick = NULL;
    return cl;
}

void
client_end(client *cl)
{
    close(cl->server_pipe);
    close(cl->client_pipe);
    unlink(cl->client_path);
    free(cl->client_path);
    client_end_tui(cl);
    free(cl);
}

bool
open_server_pipe(client *cl)
{
    bool success = (cl->server_pipe = open(cl->server_path, O_WRONLY)) != -1;
    if (!success)
        perror(cl->server_path);
    return success;
}

bool
open_client_pipe(client *cl)
{
    cl->client_path = mktmpfifo_client();
    if ((cl->client_pipe = open(cl->client_path, O_RDONLY | O_NONBLOCK)) == -1) {
        perror(cl->client_path);
        close(cl->server_pipe);
        free(cl->client_path);
        close(cl->server_pipe);
        return false;
    }
    return true;
}

void
client_init_tui(client *cl)
{
    tui_init_curses();
    cl->ui = tui_init();
    tui_print_info(cl->ui, 0);
    tui_refresh(cl->ui);
}

void
client_end_tui(client *cl)
{
    tui_end(cl->ui);
    tui_end_curses();
}


/* Input handling functions */

cmd_tok *
command_tok(char *buf)
{
    for (size_t i = 0; i < array_size(cmd_toks); ++i)
        if (strcmp(cmd_toks[i].txt, buf) == 0)
            return &cmd_toks[i];
    return NULL; /* CMD_UNKNOWN */
}

void
exec_command(client *cl, char *buf, size_t len)
{
    cmd_tok cmd;
    { /* extract command */
        char *cmd_txt_end = strchrnul(buf, ' ');
        bool was_null = *cmd_txt_end == '\0';
        if (!was_null) *cmd_txt_end = '\0';
        cmd_tok *cmdp = command_tok(buf);
        if (cmdp) {
            cmd = *cmdp;
        } else {
            tui_add_txt(cl->ui, unknown_cmd);
            tui_clear_field(cl->ui);
            return;
        }
        buf = cmd_txt_end;
        if (!was_null) buf[0] = ' ';
    }

    switch (cmd.cmd) {
    case CMD_DEBG:
        writedata(cl->server_pipe, req_client_DEBG(NULL));
        break;
    case CMD_HELP: {
        if (!buf[0]) {
            for (size_t i = 0; i < array_size(cmd_toks); ++i) {
                tui_add_txt(cl->ui, cmd_toks[i].help_txt);
            }
            tui_clear_field(cl->ui);
            return;
        } else {
            for (size_t i = 0; i < array_size(cmd_toks); ++i) {
                if (strcmp(buf + 1, cmd_toks[i].txt) == 0) {
                    tui_add_txt(cl->ui, cmd_toks[i].help_txt);
                    tui_clear_field(cl->ui);
                    return;
                }
            }
        }
        tui_add_txt(cl->ui, invalid_cmd);
        break;
    }
    case CMD_WHO:
        writedata(cl->server_pipe, req_client_LIST(cl->id));;
        break;
    case CMD_MSG:
        writedata(cl->server_pipe, req_client_PRVT(cl->id, "", buf, len));
        break;
    case CMD_NICK: {
        /* We restrict the nickname to 1-11 characters, validated by isgraph */
        writedata(cl->server_pipe, req_client_HELO(buf, cl->client_path));
        break;
    }
    case CMD_QUIT:
        writedata(cl->server_pipe, req_client_BYEE(cl->id));
        break;
    case CMD_SEND:
        /* TODO */
        break;
    case CMD_SHUT:
        writedata(cl->server_pipe, req_client_SHUT(cl->id, ""));
        break;
    }
}

void input_handler(client *cl, char *buf, size_t len) {
    logs("input_handler: %s %ld\n", buf, len);
    if (buf[0] == '/') {
        exec_command(cl, buf + 1, len - 1);
    } else if (!cl->has_id) {
        tui_add_txt(cl->ui, "No id ! /nick");
    } else {
        writedata(cl->server_pipe, req_client_BCST(cl->id, buf, len));
    }
}

static char *
trim(char *str)
{
    while (isspace((unsigned char)*str)) ++str;
    if (*str == '\0')
        return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) --end;
    *(end+1) = 0;
    return str;
}


/* Main functions */

static void
options_handler(int argc, char *argv[], client *cl)
{
    opterr = 0;
    int hflag = 0, vflag = 0, status, c;
    while ((c = getopt(argc, argv, "hv")) != -1) {
        switch (c) {
        case 'h': hflag = 1; break;
        case 'v': vflag = 1; break;
        case '?':
            if (isprint(optopt))
                fprintf(stderr, "Unknown option '-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
        default:
            status = EXIT_FAILURE;
            goto usage;
        }
    }

    if (vflag) {
        puts("tchatche dev version\n"
             "MIT License - "
             "Copyright (c) 2016 Antonin Décimo, Jean-Raphaël Gaglione");
        exit(EXIT_SUCCESS);
    } else if (hflag) {
        status = EXIT_SUCCESS;
        goto usage;
    }

    if (optind == argc) {
        cl->server_path = "/tmp/tchatche/server";
    } else if (optind == argc - 1) {
        cl->server_path = argv[optind];
    } else {
        status = EXIT_FAILURE;
        goto usage;
    }

    return;
    usage:
    puts("Usage: tchatche [server_pipe]\n"
         "\t-h\thelp\n"
         "\t-v\tversion");
    exit(status);
}


int
main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");

    logs_start("/dev/pts/2");

    /* Init client */
    client *cl = client_init();
    options_handler(argc, argv, cl);
    if (!open_server_pipe(cl) || !open_client_pipe(cl))
        exit(EXIT_FAILURE);
    client_init_tui(cl);
    tui *ui = cl->ui;

    /* Event loop */
    size_t current_msg = 0;
    char buffer[9999] = {0};
    int ch, r;
    bool run = true;
    while (run) {
        ch = wgetch(ui->input);

        /* Messages handler */
        if ((r = read(cl->client_pipe, buffer, sizeof(buffer) - 1))) {
            buffer[r] = '\0';
            tui_add_txt(ui, buffer);
            tui_refresh(ui);
        }

        /* Input handler */
        if (ch == ERR) continue;
        tui_print_info(ui, ch);
        tui_refresh(ui);
        switch (ch) {
        case KEY_UP:
            if (ui->chat_row > 0) {
                --(ui->chat_row);
                tui_refresh(ui);
            }
            break;
        case KEY_DOWN:
            if (ui->chat_row < getmaxy(ui->chat) - getmaxy(stdscr) - 1) {
                ++(ui->chat_row);
                tui_refresh(ui);
            }
            break;
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
        case '\004': /* EOT - Ctrl-D */
            run = false;
            break;
        case KEY_F(2):
            // tui_add_msg(ui, messages + (current_msg++) % array_size(messages));
            // tui_print_info(ui, ch);
            tui_refresh(ui);
            break;
        case KEY_F(3): {
            form_driver(ui->form, REQ_VALIDATION);
            char *buf = trim(field_buffer(ui->fields[0], 0));
            const size_t len = strlen(buf);
            tui_msg msg = {time(NULL), "Antonin", buf};
            if (len > 0) {
                tui_add_msg(ui, &msg);
                tui_refresh(ui);
                tui_clear_field(ui);
            }
            break;
        }
        case '\r':
        case '\n': {
            form_driver(ui->form, REQ_VALIDATION);
            char *buf = trim(field_buffer(ui->fields[0], 0));
            const size_t len = strlen(buf);
            if (len > 0) {
                input_handler(cl, buf, len);
                tui_refresh(ui);
            }
            break;
        }
        default:
            form_driver(ui->form, ch);
            break;
        }
    }

    logs_end();
    client_end(cl);
    return EXIT_SUCCESS;
}
