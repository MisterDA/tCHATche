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

server *serv = NULL;
static char *input_fifo = NULL;
static bool input_create = false;
static bool daemonize = false;
bool show_packets = false;


server *
server_init(char *path, bool create)
{
	serv = malloc(sizeof(*serv));
	serv->users = arlist_create();
	if (!path) {
		serv->path = mktmpfifo_server();
		serv->pipe = open(serv->path, O_RDWR);
		serv->symlink_created = !symlink(basename(serv->path), LINK);
	} else if (strcmp(path, "-")==0) {
		serv->path = NULL;
		serv->pipe = 0;
		serv->symlink_created = false;
	} else {
		if (create)
			serv->path = mktmpfifo(path);
		else
			serv->path = NULL;
		serv->pipe = open(path, O_RDWR);
		if (serv->pipe == -1)
			error_exit(path);
		struct stat st;
		if (serv->pipe!=0 && (fstat(serv->pipe, &st) || !S_ISFIFO(st.st_mode))) {
			fprintf(stderr, "%s: Not FIFO\n", path);
			exit(EXIT_FAILURE); // do not close: don't touch the file !
		}
		mktmpdir();
		char *abspath = realpath(path, NULL);
		serv->symlink_created = abspath && !symlink(abspath, LINK);
		free(abspath);
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
		rmdir(TMP);
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
	while ((c = getopt(argc, argv, "df:F:hIPv")) != -1) {
		switch (c) {
		case 'f': input_fifo = optarg; input_create = false; break;
		case 'F': input_fifo = optarg; input_create = true; break;
		case 'I': input_fifo = "-"; input_create = false; break;
		case 'P': show_packets = true; break;
		case 'h': hflag = 1; break;
		case 'v': vflag = 1; break;
		case 'd': daemonize = true; break;
		case '?':
			if (strchr("fF", optopt))
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option '-%c'.\n", optopt);
			else
				fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
			puts("");
		default:
			goto failure;
		}
	}

	if (vflag) {
		puts("tchatche_server dev version\n"
			 "MIT License - "
			 "Copyright (c) 2016 Antonin Décimo, Jean-Raphaël Gaglione");
		if (hflag) puts("");
		else exit(EXIT_SUCCESS);
	}
	if (hflag)
		goto usage;


	if (optind < argc) {
		if (input_fifo) goto failure;
		if (strcmp(argv[optind], "@")!=0) {
			input_fifo = argv[optind];
			input_create = access(input_fifo, F_OK);
		}
	}

	if (++optind < argc)
		goto failure;
	
	
	if (daemonize && input_fifo && strcmp(input_fifo, "-")==0) {
		fprintf (stderr, "Cannot daemonize while reading from stdin.\n");
		puts("");
		goto failure;
	}

	return;
	if (0) usage: status = EXIT_SUCCESS;
	if (0) failure: status = EXIT_FAILURE;
	puts("Usage: tchatche_server [[-f|F] FIFO]\n"
		"\t-d\tdaemonize\n"
		"\t-f FIFO\tuse this pipe as input (if the path is \"-\", similar to -I)\n"
		"\t-F FIFO\tcreate this temporary pipe and use it as input (similar to -f with \"-\")\n"
		"\t-h\thelp\n"
		"\t-I\tuse stdin as input\n"
		"\t-P\tshow raw packets\n"
		"\t-v\tversion\n"
		"Use \"@\" outside of the parameters to use the default value.");
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

	serv = server_init(input_fifo, input_create);
	while (true) {
		if (read_packet(serv->pipe,false)<0)
			break;
	}
	server_end(serv);

	logs_end();
	return EXIT_SUCCESS;
}
