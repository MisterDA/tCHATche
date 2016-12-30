#ifndef SERVER_H
#define SERVER_H

#include "arraylist.h"

typedef struct server server;
struct server {
    arlist *users;
    int pipe;
    char *path;
    bool symlink_created;
};

server *server_init(void);
void server_end(server *);

extern server *serv;

#endif
