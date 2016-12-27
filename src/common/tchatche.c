#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include "tchatche.h"

/* To create a temporary named pipe, we have to create a temporary file with
 * mkstemp, close and remove it, and use mkfifo on the generated name.
 * mkfifo will fail if the file has been recreated. In this case, we try up to
 * 10 times to create a new fifo. */
static char *mktmpfifo(char *path)
{
    int file, ret, i = 0;
    do {
        mode_t omode = umask(0);
        if (mkdir("/tmp/tchatche/", 0777))
            if (errno != EEXIST)
                error_exit("mkdir");
        if ((file = mkstemp(path)) == -1)
            error_exit("mkstemp");
        if (close(file))
            error_exit("close");
        if (unlink(path))
            error_exit("unlink");
        if ((ret = mkfifo(path, 0666)))
            if (errno != EEXIST)
                error_exit(NULL);
        umask(omode);
        ++i;
    } while (ret == -1 && i < 10);
    return ret != -1 ? path : NULL;
}

char *mktmpfifo_client(void)
{
    size_t len = sizeof("/tmp/tchatche/client-XXXXXX");
    char *p = malloc(len);
    memcpy(p, "/tmp/tchatche/client-XXXXXX", len);
    return mktmpfifo(p);
}

char *mktmpfifo_server(void)
{
    size_t len = sizeof("/tmp/tchatche/server-XXXXXX");
    char *p = malloc(len);
    memcpy(p, "/tmp/tchatche/server-XXXXXX", len);
    return mktmpfifo(p);
}


static FILE *log_file = NULL;

void logs_start(char *path) {
    if (log_file) {
        fputs("Logging had already started", stderr);
        exit(EXIT_FAILURE);
    } else if ((log_file = fopen(path, "a")) == NULL) {
        error_exit(path);
    }
    fputs("\n\n", log_file);
    fflush(log_file);
}

void logs(const char *fmt, ...) {
    if (!log_file) {
        fputs("Logging had not started", stderr);
        exit(EXIT_FAILURE);
    }
    va_list varglist;
    va_start(varglist, fmt);
    vfprintf(log_file, fmt, varglist);
    va_end(varglist);
    fflush(log_file);
}

void logs_end(void) {
    if (log_file)
        fclose(log_file);
    log_file = NULL;
}
