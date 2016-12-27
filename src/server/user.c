#include "user.h"

#include <string.h>
#include <stdlib.h>

user *
user_create(user_id id, char *nick, int pipe)
{
    user *u = malloc(sizeof(*u));
    u->id = id;
    u->nick = strdup(nick);
    u->pipe = pipe;
    return u;
}

void
user_destroy(void *e) {
    user *u = (user *) e;
    free(u->nick);
    free(u);
}

int
compare_users(const void *a, const void *b)
{
    const user *ua = (const user *) a;
    const user *ub = (const user *) b;
    return (ua->id > ub->id) - (ua->id < ub->id);
}

user_id
get_available_id(arlist *list)
{
    if (arlist_size(list) == 0)
        return 0;
    user *e1 = (user *) arlist_get(list, 0);
    user *e2 = e1;
    for (size_t i = 1; i < arlist_size(list); ++i) {
        e2 = (user *) arlist_get(list, i);
        if (e1->id + 1 != e2->id) {
            e2 = e1;
            break;
        }
    }
    return e2->id + 1;
}

user *
user_from_id(arlist *list, user_id id)
{
    for (size_t i=0; i<arlist_size(list); i++) {
        user *u = arlist_get(list,i);
        if (u->id==id)
            return u;
    }
    return NULL;
}

user *
user_from_nick(arlist *list, char *nick)
{
    for (size_t i=0; i<arlist_size(list); i++) {
        user *u = arlist_get(list,i);
        if (strcmp(u->nick, nick)==0)
            return u;
    }
    return NULL;
}

ssize_t
send_to(user *u, data d)
{
    return writedata(u->pipe, d);
}

void
broadcast(arlist *list, data d)
{
    for (size_t i=0; i<arlist_size(list); i++)
        send_to(arlist_get(list,i), d);
}
