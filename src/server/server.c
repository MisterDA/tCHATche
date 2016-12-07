#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include "tchatche.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "request.h"

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    char *path = mktmpfifo_server();
    fprintf(stderr,"%s\n",path);
    
    unlink(path);
    free(path);
    
    
    write_data(1, req_server_OKOK(42) );
    write_data(1, req_server_BADD() );
    write_data(1, req_server_BYEE(42) );
    write_data(1, req_server_LIST(1,"JR") );
    write_data(1, req_server_BCST("JR","hello",5) );
    
    fflush(stdout);
    
    
    int pipe = open(argv[1], O_RDWR);
    
    while (1) {
    	
		int buffer = 4;
		char *buff = malloc((buffer+1)*sizeof *buff);
		int r;
		while ((r=read(pipe, buff, buffer))>0) {
			buff[r] = '\0';
			printf("\x1B[35m" "[%d:]" "\x1B[0m" "%s", r, buff); fflush(stdout);
		}
		free(buff);
    	
    	printf("\x1B[35m" "[EOF]" "\x1B[0m"); fflush(stdout);
    	
    }
    
    close(pipe);
    
    
    return 0;
}
