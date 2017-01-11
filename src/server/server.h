#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include "arraylist.h"
#include "user.h"

typedef struct server server;
struct server {
    arlist *users;
    int pipe;
    char *path;
    bool symlink_created;

    arlist *transfers;
};

server *server_init(char *path);
void server_end(server *);

extern server *serv;
extern bool show_packets;

typedef struct transfer transfer;
struct transfer {
    uint32_t id;
    uint32_t len;
    user *sender;
    user *receiver;
};

transfer *transfer_from_id(arlist *list, uint32_t id);
uint32_t get_available_transfer_id(arlist *list);
int compare_transfers(const void *a, const void *b);

#endif
