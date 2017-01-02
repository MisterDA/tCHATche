#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include "tchatche.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <stdbool.h>
#include <ctype.h>
#include <getopt.h>
#include <dirent.h>

#include "server.h"
#include "request.h"
#include "packet_reception.h"
#include "user.h"

server *serv = NULL;
static bool daemonize = false;

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
server_init(void)
{
	serv = malloc(sizeof(*serv));
	serv->users = arlist_create();
	serv->path = mktmpfifo_server();
	serv->pipe = open(serv->path, O_RDWR);
	serv->symlink_created = !symlink(basename(serv->path), "/tmp/tchatche/server");
	return serv;
}

void
server_end(server *serv)
{
	arlist_destroy(serv->users, user_destroy);
	close(serv->pipe);
	unlink(serv->path);
	if (serv->symlink_created)
		unlink("/tmp/tchatche/server");
	free(serv->path);
	free(serv);
	if (dir_is_empty("/tmp/tchatche"))
		unlink("/tmp/tchatche");
}


static void
options_handler(int argc, char *argv[])
{
	opterr = 0;
	int hflag = 0, vflag = 0, status, c;
	while ((c = getopt(argc, argv, "dhv")) != -1) {
		switch (c) {
		case 'h': hflag = 1; break;
		case 'v': vflag = 1; break;
		case 'd': daemonize = true; break;
		case '?':
			if (optopt == 'j')
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
		exit(EXIT_SUCCESS);
	} else if (hflag) {
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
		"\t-h\thelp\n"
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

	serv = server_init();
	while (true) {
		if (read_packet(serv->pipe,false)<0)
			break;
	}
	server_end(serv);

	logs_end();
	return EXIT_SUCCESS;
}
