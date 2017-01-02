#ifndef TCHATCHE_H
#define TCHATCHE_H

#include <stdarg.h>
#include <stdio.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define array_size(x)  (sizeof(x) / sizeof((x)[0]))

#define error_exit(s) do { perror(s); exit(EXIT_FAILURE); } while(0)

/* Create a temporary fifo. Returns its path (must be freed). */
char *mktmpfifo_client(void);
char *mktmpfifo_server(void);

void logs_start(FILE *file, const char *motd);
void logs(const char *fmt, ...);
void logs_end(void);

#endif
