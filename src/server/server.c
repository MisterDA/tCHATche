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
    setlocale(LC_ALL, "");

    char *server_path = mktmpfifo_server();
    bool symlink_created = !symlink(basename(server_path), "/tmp/tchatche/server");

    int server_pipe = open(server_path, O_RDWR);

    write_data(1, req_server_OKOK(42) );
    write_data(1, req_server_BADD() );
    write_data(1, req_server_BYEE(42) );
    write_data(1, req_server_LIST(1,"JR") );
    write_data(1, req_server_BCST("JR","hello",5) );
    fflush(stdout);

    while (1) {
        const int buff_len = 4;
        char *buff = malloc(buff_len + 1);
        int r;
        while ((r = read(server_pipe, buff, buff_len)) > 0) {
            buff[r] = '\0';
            printf("\x1B[35m" "[%d:]" "\x1B[0m" "%s", r, buff);
            fflush(stdout);
        }
        free(buff);
        printf("\x1B[35m" "[EOF]" "\x1B[0m");
        fflush(stdout);
    }

    close(server_pipe);
    unlink(server_path);
    if (symlink_created)
        unlink("/tmp/tchatche/server");
    free(server_path);

    return EXIT_SUCCESS;
}
