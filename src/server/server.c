#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include "tchatche.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>

#include "request.h"

int main(int argc, char *argv[])
{
	if (argc>1) goto here;


	setlocale(LC_ALL, "");

	char *server_path = mktmpfifo_server();
	bool symlink_created = !symlink(basename(server_path), "/tmp/tchatche/server");

	int server_pipe = open(server_path, O_RDWR);

	int r;
	bool quit = false;
	const int buf_len = 127;
	char *buf = malloc(buf_len + 1);
	while (!quit) {
		while ((r = read(server_pipe, buf, buf_len)) > 0) {
	 		buf[r] = '\0';
			puts(buf);
			fflush(stdout);
			if (strcmp(buf, "quit") == 0) {
				quit = true;
				break;
			}
		}
	}
	
	
	close(server_pipe);
	unlink(server_path);
	if (symlink_created)
		unlink("/tmp/tchatche/server");
	free(server_path);

	return EXIT_SUCCESS;
	
	
	
	
	return 0;
	here:;// CODE A JR
	
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
	
	if (argc<=1)
	return 0;
	
	int pipe = open(argv[1], O_RDWR);
	if (pipe<0) {
		printf("\x1B[0;1;31m\"%s\" not accessible\n\x1B[0m", argv[1]);
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
