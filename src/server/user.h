#ifndef USER_H
#define USER_H

#include <sys/types.h>
#include "packet.h"
#include "arraylist.h"

#define MAX_USERS 10000

typedef uint32_t user_id;

typedef struct {
    user_id id;
    char *nick;
    int pipe;
} user;

user *user_create(user_id id, char *nick, int pipe);
void user_destroy(void *e);

int compare_users(const void *a, const void *b);
user_id get_available_id(arlist *list);

user *user_from_id(arlist *list, user_id id);
user *user_from_nick(arlist *list, char *nick);

ssize_t send_to(user *u, data data);
void broadcast(arlist *list, data data);


#endif
