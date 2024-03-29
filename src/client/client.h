#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdbool.h>
#include "arraylist.h"
#include "tui.h"

#define NICK_MAX_LENGTH 32

typedef struct {
	uint32_t id;
	uint32_t series;
	uint32_t len;
	char *filename;
	char *nick;

	int fd;
} transfer;

transfer *transfer_from_id(arlist *list, uint32_t id);
int compare_transfers(const void *a, const void *b);
void destroy_transfer(void *e);

typedef struct {
	int server_pipe, client_pipe;
	char *server_path, *client_path;
	bool client_created;
	arlist *history;
	size_t current_hist;
	tui *ui;

	bool has_id;
	uint32_t id;
	char *nick;

	char last_prvt[NICK_MAX_LENGTH+1];
	transfer *upload;
	arlist *downloads;
	char *download_dir;
} client;

extern client *cl;
extern bool no_wait;

client *client_init(void);
void client_end(client *cl);

bool open_server_pipe(client *cl);
bool open_client_pipe(client *cl);

void client_init_tui(client *cl);
void client_end_tui(client *cl);

typedef enum {
	CMD_DEBG, CMD_HELP, CMD_WHO, CMD_MSG, CMD_RESP, CMD_NICK, CMD_QUIT, CMD_SEND,
	CMD_SHUT, CMD_SHELL, CMD_GOL,
} command;

typedef struct {
	command cmd;
	const char *txt;
	const char *help_txt;
} cmd_tok;

cmd_tok *command_tok(char *buf);
void input_handler(client *cl, char *buf, size_t len);

#endif
