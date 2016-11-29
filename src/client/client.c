#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include "tchatche.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char *path = mktmpfifo_client();
    printf("%s\n", path);
        
    unlink(path);
    free(path);
    
    
    int pipe = open(argv[1], O_WRONLY);
    
    for (int i=0; i<10; i++) {
		
		write(pipe, argv[2], strlen(argv[2]));
		write(pipe, " ", 1);
		
		usleep(2*1000*1000);
    }
    
    close(pipe);
    
    
    return 0;
}
