#ifndef TCHATCHE_H
#define TCHATCHE_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define array_size(x)  (sizeof(x) / sizeof((x)[0]))

#define error_exit(s) do { perror(s); exit(EXIT_FAILURE); } while(0)

/* Create a temporary fifo. Returns its path (must be freed). */
void mktmpdir(void);
char *mktmpfifo(char *path);
char *mktmpfifo_client(void);
char *mktmpfifo_server(void);

bool dir_is_empty(const char *path);

bool valid_filename(char *filename);
int open_new(char *dir, char *basename);

void logs_start(FILE *file, const char *motd);
void logs(const char *fmt, ...);
void logs_end(void);

#endif
