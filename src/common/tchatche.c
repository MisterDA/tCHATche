#include "tchatche.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>

#define TMP "/tmp/tCHATche"

void mktmpdir(void) {
	mode_t omode = umask(0);
	if (mkdir(TMP, 0777))
		if (errno != EEXIST)
			error_exit("mkdir");
	umask(omode);
}

char *mktmpfifo(char *path) {
	mode_t omode = umask(0);
	int ret = mkfifo(path, 0666);
	if (ret)
		error_exit(path);
	umask(omode);
	return strdup(path);
}

/* To create a temporary named pipe, we have to create a temporary file with
 * mkstemp, close and remove it, and use mkfifo on the generated name.
 * mkfifo will fail if the file has been recreated. In this case, we try up to
 * 10 times to create a new fifo. */
static char *mkstmpfifo(char *path)
{
    int file, ret, i = 0;
    do {
        mode_t omode = umask(0);
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
    size_t len = sizeof(TMP"/client-XXXXXX");
    char *p = malloc(len);
    memcpy(p, TMP"/client-XXXXXX", len);
	mktmpdir();
    return mkstmpfifo(p);
}

char *mktmpfifo_server(void)
{
    size_t len = sizeof(TMP"/server-XXXXXX");
    char *p = malloc(len);
    memcpy(p, TMP"/server-XXXXXX", len);
	mktmpdir();
    return mkstmpfifo(p);
}


bool
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
		//printf(">>> %s\n", ent->d_name); //DEV
		ret = false;
		break;
	}
	closedir(dir);
	return ret;
}


static FILE *log_file = NULL;
static const char *log_motd = "";

void logs_start(FILE *file, const char *motd) {
    if (log_file) {
        fputs("Logging had already started", stderr);
        exit(EXIT_FAILURE);
    } else if (file) {
        log_file = file;
    }
    fflush(log_file);
    log_motd = motd;
}

void logs(const char *fmt, ...) {
    if (!log_file) {
        fputs("Logging had not started", stderr);
        exit(EXIT_FAILURE);
    }
    fputs(log_motd, log_file);
    va_list varglist;
    va_start(varglist, fmt);
    vfprintf(log_file, fmt, varglist);
    va_end(varglist);
    fflush(log_file);
}

void logs_end(void) {
    fputs("\n\n", log_file);
    if (log_file)
        fclose(log_file);
    log_file = NULL;
}
