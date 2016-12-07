#include "tchatche.h"

FILE *out;

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
