#include "server.h"

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <stdbool.h>
#include <ctype.h>
#include <getopt.h>
#include <dirent.h>
#include <signal.h>

#include "tchatche.h"
#include "request.h"
#include "packet_reception.h"
#include "user.h"

#define TMP "/tmp/tCHATche"
#define LINK TMP"/server"
#define PIPE_MAX_LENGTH 256

server *serv = NULL;
static char *input_fifo = NULL;
static bool daemonize = false;
bool show_packets = false;

static bool
dir_is_empty(const char *path)
{
	DIR *dir;
	bool ret = true;
	struct dirent *ent;
	if ((dir = opendir(path)) == NULL)
		return false;
	while ((ent = readdir(dir))) {
		if (!strcmp(ent->d_name, ".") || !(strcmp(ent->d_name, "..")))
			continue;
		ret = false;
		break;
	}
	closedir(dir);
	return ret;
}

server *
server_init(char *path)
{
	serv = malloc(sizeof(*serv));
	serv->users = arlist_create();
	//fprintf(stderr, "input is %s\n", path);
	if (!path) {
		serv->path = mktmpfifo_server();
		serv->pipe = open(serv->path, O_RDWR);
		serv->symlink_created = !symlink(basename(serv->path), LINK);
	} else if (strcmp(path, "-")==0) {
		serv->path = NULL;
		serv->pipe = 0;
		serv->symlink_created = false;
	} else {
		serv->path = NULL;
		serv->pipe = open(path, O_RDWR);
		if (serv->pipe < 0)
			error_exit("input");
		struct stat st;
		if (serv->pipe!=0 && (fstat(serv->pipe, &st) || !S_ISFIFO(st.st_mode))) {
			fprintf(stderr, "input: must be FIFO\n");
			exit(EXIT_FAILURE);
		}
		char abspath[PIPE_MAX_LENGTH];
		realpath(path, abspath);
		serv->symlink_created = !symlink(abspath, LINK);
	}
	serv->transfers = arlist_create();
	return serv;
}

void
server_end(server *serv)
{
	arlist_destroy(serv->users, user_destroy);
	arlist_destroy(serv->transfers, free);
	close(serv->pipe);
	if (serv->path)
		unlink(serv->path);
	if (serv->symlink_created)
		unlink(LINK);
	free(serv->path);
	free(serv);
	if (dir_is_empty(TMP))
		unlink("TMP");
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

uint32_t
get_available_transfer_id(arlist *list)
{
    if (arlist_size(list) == 0)
        return 0;
    transfer *t1 = (transfer *) arlist_get(list, 0);
    transfer *t2 = t1;
    for (size_t i = 1; i < arlist_size(list); ++i) {
        t2 = (transfer *) arlist_get(list, i);
        if (t1->id + 1 != t2->id) {
            t2 = t1;
            break;
        }
    }
    return t2->id + 1;
}

int
compare_transfers(const void *a, const void *b)
{
    const transfer *ta = (const transfer *) a;
    const transfer *tb = (const transfer *) b;
    return (ta->id > tb->id) - (ta->id < tb->id);
}

static void
options_handler(int argc, char *argv[])
{
	opterr = 0;
	int hflag = 0, vflag = 0, status, c;
	while ((c = getopt(argc, argv, "df:hIPv")) != -1) {
		switch (c) {
		case 'P': show_packets = true; break;
		case 'f': input_fifo = optarg; break;
		case 'I': input_fifo = "-"; break;
		case 'h': hflag = 1; break;
		case 'v': vflag = 1; break;
		case 'd': daemonize = true; break;
		case '?':
			if (optopt == 'f')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option '-%c'.\n", optopt);
			else
				fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
		default:
			status = EXIT_FAILURE;
			goto usage;
		}
	}

	if (vflag) {
		puts("tchatche_server dev version\n"
			 "MIT License - "
			 "Copyright (c) 2016 Antonin Décimo, Jean-Raphaël Gaglione");
		if (hflag) {
			puts("");
			status = EXIT_SUCCESS;
			goto usage;
		}
		exit(EXIT_SUCCESS);
	}
	if (hflag) {
		status = EXIT_SUCCESS;
		goto usage;
	}

	if (optind != argc) {
		status = EXIT_FAILURE;
		goto usage;
	}

	return;
	usage:
	puts("Usage: tchatche_server\n"
		"\t-d\tdaemonize\n"
		"\t-f FIFO\tuse this pipe as input (if the path is \"-\", similar to -I)\n"
		"\t-h\thelp\n"
		"\t-I\tuse stdin as input\n"
		"\t-P\tshow raw packets\n"
		"\t-v\tversion");
	exit(status);
}

int
main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");
	logs_start(stdout, "");
	options_handler(argc, argv);

	if (daemonize && daemon(0, 0) == -1)
		error_exit("daemon");

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		perror("signal");

	serv = server_init(input_fifo);
	while (true) {
		if (read_packet(serv->pipe,false)<0)
			break;
	}
	server_end(serv);

	logs_end();
	return EXIT_SUCCESS;
}
