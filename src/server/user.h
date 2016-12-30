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
    char *path;
    int pipe;
} user;

user *user_create(user_id id, char *nick, char *path, int pipe);
void user_destroy(void *e);

int compare_users(const void *a, const void *b);
user_id get_available_id(arlist *list);

bool is_valid_cred(arlist *list, char *nick, char *path);

user *user_from_id(arlist *list, user_id id);
user *user_from_nick(arlist *list, char *nick);
user *user_from_pipe_path(arlist *list, char *path)

ssize_t send_to(user *u, data data);
void broadcast(arlist *list, data data);


#endif
