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
static char *jr_pipe;
static bool jr_mode = false, daemonize = false;

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
	while ((c = getopt(argc, argv, "dhvj:")) != -1) {
		switch (c) {
		case 'h': hflag = 1; break;
		case 'v': vflag = 1; break;
		case 'd': daemonize = true; break;
		case 'j': jr_mode = true; jr_pipe = optarg; break;
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
		"\t-j <pipe>\tJR mode\n"
		 "\t-h\thelp\n"
		 "\t-v\tversion");
	exit(status);
}

int
main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");
	options_handler(argc, argv);

	if (daemonize && daemon(0, 0) == -1)
		error_exit("daemon");
	if (jr_mode)
		goto here_jr;

	serv = server_init();

	while (true) {
		if (read_packet(serv->pipe,false)<0)
			break;
	}

	server_end(serv);
	return EXIT_SUCCESS;





	here_jr:; /* Code de JR */

	data d = str2data(NULL, "Lorem ipsum etc");

	writedata(1, req_server_OKOK(42) );
	putchar('\n');
	writedata(1, req_server_BADD() );
	putchar('\n');
	writedata(1, req_server_BYEE(42) );
	putchar('\n');
	writedata(1, req_server_LIST(1,"JR") );
	putchar('\n');
	writedata(1, req_server_BCST("JR","blablabla",9) );
	putchar('\n');
	writedata(1, req_server_FILE_announce(961, 65536, "hello.txt", "JR") );
	putchar('\n');
	writedata(1, req_server_FILE_announce(962, 65536, "fractale.png", NULL) );
	putchar('\n');
	writedata(1, req_server_FILE_transfer(1, 962, d) );
	putchar('\n');

	fflush(stdout);


	int pipe = open(jr_pipe, O_RDWR);
	if (pipe<0) {
		printf("\x1B[0;1;31m\"%s\" not accessible\n\x1B[0m", jr_pipe);
		return 1;
	}
	while (1) {

		int buffer = 4;
		char *buff = malloc((buffer+1)*sizeof *buff);
		int r;
		while ((r=read(pipe, buff, buffer))>0) {
			buff[r] = '\0';
			printf("\x1B[0;35m" "[%d:]" "\x1B[0m" "%s", r, buff); fflush(stdout);
		}
		free(buff);

		printf("\x1B[0;35m" "[EOF]" "\x1B[0m"); fflush(stdout);

	}
	close(pipe);

	return 0;
}
