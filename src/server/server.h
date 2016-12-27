#ifndef SERVER_H
#define SERVER_H

#include "arraylist.h"

typedef struct server server;
struct server {
    arlist *users;
    int pipe;
};

extern server *serv;

#endif
