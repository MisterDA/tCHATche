#ifndef TCHATCHE_H
#define TCHATCHE_H

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define array_size(x)  (sizeof(x) / sizeof((x)[0]))

#define error_exit(s) do { perror(s); exit(EXIT_FAILURE); } while(0)

extern FILE *out;

/* Create a temporary fifo. Returns its path (must be freed). */
char *mktmpfifo_client(void);
char *mktmpfifo_server(void);

#endif
