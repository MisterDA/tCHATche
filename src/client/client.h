#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>
#include "tui.h"

typedef struct {
    int server_pipe, client_pipe;
    char *server_path, *client_path;
    tui *ui;

    bool has_id;
    uint32_t id;
    char *nick;
} client;

extern client *cl;

client *client_init(void);
void client_end(client *cl);

bool open_server_pipe(client *cl);
bool open_client_pipe(client *cl);

void client_init_tui(client *cl);
void client_end_tui(client *cl);

typedef enum {
    CMD_DEBG, CMD_HELP, CMD_WHO, CMD_MSG, CMD_NICK, CMD_QUIT, CMD_SEND,
    CMD_SHUT,
} command;

typedef struct {
    command cmd;
    const char *txt;
    const char *help_txt;
} cmd_tok;

cmd_tok *command_tok(char *buf);
void input_handler(client *cl, char *buf, size_t len);

#endif
