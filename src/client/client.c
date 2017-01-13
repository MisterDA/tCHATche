#include "client.h"

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
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include "tchatche.h"
#include "packet_reception.h"
#include "tui.h"
#include "request.h"
#include "gol.h"

#define TMP "/tmp/tCHATche"
#define LINK TMP"/server"

client *cl = NULL;
bool no_wait = false;

static char *invalid_cmd = "Invalid command.";
static char *unknown_cmd = "Unknown command.";

static cmd_tok cmd_toks[] = {
	{CMD_NICK,  "nick",  "/nick <nick>    set your nickname"},
	{CMD_WHO,   "who",   "/who            list users on the server"},
	{CMD_MSG,   "msg",   "/msg <nick> ... send a private message"},
	{CMD_MSG,   "m",     "/m <nick> ...   alias for /msg"},
	{CMD_RESP,  "r",     "/r ...          send a private message to the last correspondant"},
	{CMD_QUIT,  "quit",  "/quit           quit tCHATche client"},
	{CMD_SHUT,  "shut",  "/shut [pwd]     shut down the server"},
	{CMD_SEND,  "send",  "/send <nick> <file>  send a file"},
	{CMD_HELP,  "help",  "/help [cmd]     see more details about a specific command"},
	{CMD_HELP,  "?",     "/? [cmd]        alias for /help"},
	{CMD_SHELL, "shell", "/shell ...      execute a shell command"},
	{CMD_SHELL, "/",     "//...           alias for /shell"},
	{CMD_DEBG,  "debug", "/debug          debug server"},
};

static off_t
fsize(const char *filename) {
	struct stat st;
	if (stat(filename, &st) == 0)
		return st.st_size;
	return -1;
}

/* Client init functions */

client *
client_init(void)
{
	client *cl = malloc(sizeof(client));
	cl->server_path = NULL;
	cl->client_path = NULL;
	cl->ui = NULL;
	cl->history = arlist_create();
	cl->current_hist = 0;
	cl->has_id = false;
	cl->id = MAX_NUM+1;
	cl->nick = NULL;
	memset(cl->last_prvt, '\0', sizeof cl->last_prvt);
	cl->upload = NULL;
	cl->downloads = arlist_create();
	return cl;
}

void
client_end(client *cl)
{
	client_end_tui(cl);
	arlist_destroy(cl->history, free);
	close(cl->server_pipe);
	close(cl->client_pipe);
	if (cl->client_created) {
		unlink(cl->client_path);
		if (dir_is_empty(TMP))
			rmdir(TMP);
	}
	free(cl->server_path);
	free(cl->client_path);
	free(cl->nick);
	arlist_destroy(cl->downloads, destroy_transfer);
	free(cl->download_dir);
	free(cl);
}

bool
open_server_pipe(client *cl)
{
	if (!cl->server_path)
		cl->server_path = strdup(LINK);
	if (strcmp(cl->server_path, "-")==0) {
		cl->server_path = NULL;
		cl->server_pipe = 1;
	} else {
		cl->server_pipe = open(cl->server_path, O_WRONLY);
		if (cl->server_pipe == -1) {
			perror(cl->server_path);
			return false;
		}
		struct stat st;
		if (cl->server_pipe!=1 && (fstat(cl->server_pipe, &st) || !S_ISFIFO(st.st_mode))) {
			fprintf(stderr, "%s: Not FIFO\n", cl->server_path);
			return false;
		}
	}
	return true;


	bool success = (cl->server_pipe = open(cl->server_path, O_WRONLY)) != -1;
	if (!success)
		perror(cl->server_path);

	return success;
}

bool
open_client_pipe(client *cl)
{
	if (!cl->client_path) {
		cl->client_path = mktmpfifo_client();
		cl->client_created = true;
	} else if (strcmp(cl->client_path, "-")==0) {
		fprintf(stderr, "input: Must be a physical FIFO, not stdin\n");
		goto err0;
	} else if (cl->client_created) {
		mktmpfifo(cl->client_path);
	}
	char *abspath = realpath(cl->client_path, NULL);
	if (!abspath) {
		perror(cl->client_path);
		goto err0;
	}
	free(cl->client_path);
	cl->client_path = abspath;
	cl->client_pipe = open(cl->client_path, O_RDONLY | O_NONBLOCK);
	struct stat st;
	if (cl->client_pipe == -1) {
		perror(cl->client_path);
		goto err0;
	}
	if (fstat(cl->client_pipe, &st) || !S_ISFIFO(st.st_mode)) {
		fprintf(stderr, "%s: Not FIFO\n", cl->client_path);
		goto err1;
	}
	return true;
	err1: close(cl->client_pipe);
	err0: free(cl->client_path);
	close(cl->server_pipe);
	return false;
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

transfer *
transfer_from_id(arlist *list, uint32_t id)
{
	for (size_t i=0; i<arlist_size(list); i++) {
		transfer *t = arlist_get(list,i);
		if (t->id==id)
			return t;
	}
	return NULL;
}

int
compare_transfers(const void *a, const void *b)
{
	const transfer *ta = (const transfer *) a;
	const transfer *tb = (const transfer *) b;
	return (ta->id > tb->id) - (ta->id < tb->id);
}

void
destroy_transfer(void *e)
{
	transfer *t = (transfer *)e;
	if (t->fd != -1) close(t->fd);
	free(t->filename);
	free(t->nick);
	free(t);
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


static void
exec_command_HELP(client *cl, char *buf, size_t len) {
	if (!buf[0]) {
		for (size_t i = 0; i < array_size(cmd_toks); ++i) {
			tui_add_txt(cl->ui, cmd_toks[i].help_txt);
		}
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
	tui_print_txt(cl->ui, "/%-13s  Sorry ! Not yet implemented.", buf+1);
}

void
exec_command(client *cl, char *buf, size_t len)
{
	cmd_tok cmd;
	size_t cmd_len;
	char * cmd_txt_end;
	{ /* extract command */
		cmd_tok *cmdp;
		bool was_null = false;
		if (buf[0] == '/') {
			cmd_txt_end = buf;
			cmdp = command_tok("/");
		} else {
			cmd_txt_end = strchrnul(buf, ' ');
			was_null = *cmd_txt_end == '\0';
			if (!was_null) *cmd_txt_end = '\0';
			cmdp = command_tok(buf);
		}
		if (cmdp) {
			cmd = *cmdp;
		} else {
			tui_add_txt(cl->ui, unknown_cmd);
			return;
		}
		cmd_len = cmd_txt_end - buf;
		buf = cmd_txt_end;
		if (!was_null) buf[0] = ' ';
	}

	switch (cmd.cmd) {
		case CMD_SHELL: {
			//tui_print_txt(cl->ui, "$ %s", buf+1);
			char *msg;
			asprintf(&msg, "$ %s", buf+1);
			writedata(cl->server_pipe, req_client_BCST(cl->id, msg, strlen(msg)));
			if (no_wait)
				tui_add_msg(cl->ui, &(tui_msg){time(NULL), cl->nick, msg});
			free(msg);

			FILE *in = popen(buf+1, "r");
			char *line = NULL;
			size_t len = 0;
			ssize_t read;

			while ((read = getline(&line, &len, in)) != -1) {
				//line[read-1] = '\0';
				//tui_add_txt(cl->ui, line);
				asprintf(&msg, "  %s", line);
				writedata(cl->server_pipe, req_client_BCST(cl->id, msg, strlen(msg)));
				if (no_wait)
					tui_add_msg(cl->ui, &(tui_msg){time(NULL), cl->nick, msg});
				free(msg);
			}

			free(line);
			fclose(in);
			break;
		}
		case CMD_HELP: {
			exec_command_HELP(cl, buf, len);
			break;
		}
		default: goto next;
	}
	return; next:

	if (!cl->has_id) {

		switch (cmd.cmd) {
		case CMD_NICK: {
			if (cmd_len >= len) {
				tui_add_txt(cl->ui, invalid_cmd);
				break;
			}
			free(cl->nick);
			cl->nick = strdup(buf+1);
			writedata(cl->server_pipe, req_client_HELO(buf+1, cl->client_path));
			break;
		}
		case CMD_QUIT: {
			client_end(cl);
			exit(EXIT_SUCCESS);
			break;
		}
		case CMD_DEBG: {
			writedata(cl->server_pipe, req_client_DEBG(NULL));
			break;
		}
		default:
			tui_print_txt(cl->ui, "%s Use \"/nick\" first.", invalid_cmd);
		}

	} else {

		switch (cmd.cmd) {
		case CMD_DEBG: {
			writedata(cl->server_pipe, req_client_DEBG(NULL));
			break;
		}
		case CMD_WHO:
			writedata(cl->server_pipe, req_client_LIST(cl->id));
			break;
		case CMD_MSG: {
			char *nick_end = strchrnul(cmd_txt_end + 1, ' ');
			size_t nick_len = nick_end - cmd_txt_end;
			if (*nick_end == '\0' || nick_len + cmd_len >= len) {
				tui_add_txt(cl->ui, invalid_cmd);
				break;
			}
			*nick_end = '\0';
			writedata(cl->server_pipe,
					  req_client_PRVT(cl->id, cmd_txt_end + 1, nick_end + 1, len));
			tui_add_prvt_msg(cl->ui, &(tui_msg){time(NULL), cmd_txt_end+1, nick_end+1}, true);
			strcpy(cl->last_prvt, cmd_txt_end+1);
			break;
		}
		case CMD_RESP: {
			if (cmd_len >= len) {
				tui_add_txt(cl->ui, invalid_cmd);
				break;
			}
			if (strcmp(cl->last_prvt, "")==0) {
				tui_print_txt(cl->ui, "%s You haven't talk to anyone !", invalid_cmd);
			}
			writedata(cl->server_pipe,
					  req_client_PRVT(cl->id, cl->last_prvt, cmd_txt_end + 1, len));
			tui_add_prvt_msg(cl->ui, &(tui_msg){time(NULL), cl->last_prvt, cmd_txt_end+1}, true);
			break;
		}
		case CMD_NICK: {
			tui_print_txt(cl->ui, "%s You already have a nick !", invalid_cmd);
			break;
		}
		case CMD_QUIT: {
			writedata(cl->server_pipe, req_client_BYEE(cl->id));
			break;
		}
		case CMD_SEND: {
			char *nick_end = strchrnul(cmd_txt_end + 1, ' ');
			size_t nick_len = nick_end - cmd_txt_end-1 + 1;
			if (*nick_end == '\0' || nick_len + cmd_len >= len) {
				tui_add_txt(cl->ui, invalid_cmd);
				break;
			}
			*nick_end = '\0';

			char *path = nick_end + 1;
			if (access(path, R_OK)) {
				tui_print_txt(cl->ui, "Invalid path: %s", strerror(errno));
				break;
			}
			off_t len = fsize(path);
			if (len < 0 ) {
				tui_add_txt(cl->ui, "Invalid file length.");
				break;
			//} else if (len == 0) {
			//	tui_add_txt(cl->ui, "Empty file.");
			//	break;
			} else if ((uint32_t)len > 9999u * 256u) {
				tui_add_txt(cl->ui, "The file is too big to be send.");
				break;
			}
			int fd = open(path, O_RDONLY);
			if (fd == -1) {
				tui_print_txt(cl->ui, "Invalid path: %s", strerror(errno));
				break;
			}

			transfer *t = malloc(sizeof(*t));
			t->id = 0;
			t->series = 0;
			t->len = len;
			t->fd = fd;
			t->filename = strdup(basename(path));
			t->nick = strdup(cmd_txt_end + 1);
			cl->upload = t;

			writedata(cl->server_pipe,
				req_client_FILE_announce(cl->id, t->nick, t->len, t->filename));
			break;
		}
		case CMD_SHUT: {
			if (cmd_len >= len)
				writedata(cl->server_pipe, req_client_SHUT(cl->id, NULL));
			else
				writedata(cl->server_pipe, req_client_SHUT(cl->id, buf+1));
			break;
		}
		case CMD_GOL: {
			gol *g = gol_init(getmaxx(stdscr), getmaxy(stdscr) - 3);
			gol_rand(g);
			while (true) {
				gol_iter(g);
				gol_draw(stdscr, g);
				refresh();
				usleep(100);
			}
			gol_free(g);
		}
		default:
			tui_print_txt(cl->ui, "%s Already connected.", invalid_cmd);
		}

	}
}

void input_handler(client *cl, char *buf, size_t len) {
	arlist_push(cl->history, strdup(buf));
	cl->current_hist = arlist_size(cl->history);
	if (buf[0] == '/') {
		exec_command(cl, buf + 1, len - 1);
	} else if (!cl->has_id) {
		tui_add_txt(cl->ui, "Use /nick to set your nickname.");
	} else {
		writedata(cl->server_pipe, req_client_BCST(cl->id, buf, len));
		if (no_wait)
			tui_add_msg(cl->ui, &(tui_msg){time(NULL), cl->nick, buf});
	}
	tui_clear_field(cl->ui);
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
	cl->server_path = NULL;
	cl->client_path = NULL;
	cl->client_created = false;
	cl->nick = NULL;
	cl->download_dir = ".";

	opterr = 0;
	int hflag = 0, vflag = 0, status, c;
	while ((c = getopt(argc, argv, "D:f:F:hmMn:Os:v")) != -1) {
		switch (c) {
		case 'f': cl->client_path = optarg; cl->client_created = false; break;
		case 'F': cl->client_path = optarg; cl->client_created = true; break;
		case 's': cl->server_path = optarg; break;
		case 'O': cl->server_path = "-"; break;
		case 'n': cl->nick = optarg; break;
		case 'm': no_wait = false; break;
		case 'M': no_wait = true; break;
		case 'D': cl->download_dir = optarg; break;
		case 'h': hflag = 1; break;
		case 'v': vflag = 1; break;
		case '?':
			if (strchr("DfFns", optopt))
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			if (isprint(optopt))
				fprintf(stderr, "Unknown option '-%c'.\n", optopt);
			else
				fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
			puts("");
		default:
			goto failure;
		}
	}

	if (vflag) {
		puts("tchatche dev version\n"
			 "MIT License - "
			 "Copyright (c) 2016 Antonin Décimo, Jean-Raphaël Gaglione");
		if (hflag) puts("");
		else exit(EXIT_SUCCESS);
	}
	if (hflag)
		goto usage;


	if (optind < argc) {
		if (cl->server_path) goto failure;
		if (strcmp(argv[optind], "@")!=0) {
			cl->server_path = argv[optind];
		}
	}
	if (cl->server_path)
		cl->server_path = strdup(cl->server_path);

	if (++optind < argc) {
		if (cl->client_path) goto failure;
		if (strcmp(argv[optind], "@")!=0) {
			cl->client_path = argv[optind];
			cl->client_created = access(cl->client_path, F_OK);
		}
	}
	if (cl->client_path)
		cl->client_path = strdup(cl->client_path);

	if (++optind < argc)
		goto failure;

	if (cl->nick)
		cl->nick = strdup(cl->nick);

	if (cl->download_dir)
		cl->download_dir = strdup(cl->download_dir);


	return;
	if (0) usage: status = EXIT_SUCCESS;
	if (0) failure: status = EXIT_FAILURE;
	puts("Usage: tchatche [[-s] SERV_FIFO] [[-f|F] FIFO]\n"
		"\t-D DIR\tset downloads directory (default is the current working directory)\n"
		"\t-f FIFO\tuse this pipe as input (\"-\" is not allowed)\n"
		"\t-F FIFO\tcreate this temporary pipe and use it as input (\"-\" is not allowed)\n"
		"\t-h\thelp\n"
		"\t-m\twait server broadcast to show user message (default)\n"
		"\t-M\tshow user messages when sending and ignore server response\n"
		"\t-n NICK\tinitialize the tchat session with this nick\n"
		"\t-O\tuse stdout as output (useful whith redirected output)\n"
		"\t-s FIFO\tuse this pipe as output (if the path is \"-\", similar to -O)\n"
		"\t-v\tversion\n"
		"Use \"@\" outside of the parameters to use the default value.");
	exit(status);
}


int
main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");

	/*
	char *log_path = "/dev/pts/2";
	FILE *log_file;
	if ((log_file = fopen(log_path, "a")) == NULL)
		error_exit(log_path);
	logs_start(log_file, "");
	*/

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		perror("signal");

	/* Init client */
	cl = client_init();
	options_handler(argc, argv, cl);
	if (!open_server_pipe(cl) || !open_client_pipe(cl))
		exit(EXIT_FAILURE);

	if (cl->nick) {
		writedata(cl->server_pipe, req_client_HELO(cl->nick, cl->client_path));
		//TODO: peut mieux faire:
		// start l'ui uniquement une fois le OKOK reçu
		// quitter le programme si BADD
	}

	client_init_tui(cl);
	tui *ui = cl->ui;

	/* Event loop */
	int ch;
	bool run = true;
	while (run) {
		ch = wgetch(ui->input);

		/* Packet handler */
		read_packet(cl->client_pipe, true);
		tui_refresh(ui);

		/* Input handler */
		if (ch == ERR) continue;;
		tui_print_info(ui, ch);
		tui_refresh(ui);
		switch (ch) {
		case KEY_PPAGE:
			if (ui->chat_row > 0) {
				--(ui->chat_row);
				tui_refresh(ui);
			}
			break;
		case KEY_NPAGE:
			if (ui->chat_row < getmaxy(ui->chat) - getmaxy(stdscr) + 2) {
				++(ui->chat_row);
				tui_refresh(ui);
			}
			break;
		case KEY_UP:
			if (cl->current_hist > 0) {
				--(cl->current_hist);
				set_field_buffer(cl->ui->fields[0], 0, arlist_get(cl->history, cl->current_hist));
				form_driver(cl->ui->form, REQ_END_LINE);
			}
			break;
		case KEY_DOWN:
			if (cl->current_hist < arlist_size(cl->history) - 1) {
				++(cl->current_hist);
				set_field_buffer(cl->ui->fields[0], 0, arlist_get(cl->history, cl->current_hist));
				form_driver(cl->ui->form, REQ_END_LINE);
			} else if (cl->current_hist == arlist_size(cl->history) - 1) {
				++(cl->current_hist);
				form_driver(cl->ui->form, REQ_CLR_FIELD);
			}
			break;
		case KEY_LEFT:
			form_driver(ui->form, REQ_PREV_CHAR);
			break;
		case KEY_RIGHT:
			form_driver(ui->form, REQ_NEXT_CHAR);
			break;
		case '\177': /* DEL */
		case KEY_BACKSPACE:
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
		case KEY_DC: /* Suppr. */
			form_driver(ui->form, REQ_DEL_CHAR);
			break;
		case '\004': /* EOT - Ctrl-D */
			run = false;
			writedata(cl->server_pipe, req_client_BYEE(cl->id));
			break;
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

	client_end(cl);
	/* logs_end(); */
	return EXIT_SUCCESS;
}
