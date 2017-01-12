#include "user.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "server.h"


user *
user_create(user_id id, char *nick, char *path, int pipe)
{
	user *u = malloc(sizeof(*u));
	u->id = id;
	u->nick = strdup(nick);
	u->path = strdup(path);
	u->pipe = pipe;
	return u;
}

void
user_destroy(void *e) {
	user *u = (user *) e;
	free(u->nick);
	close(u->pipe);
	free(u->path);
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

bool
is_valid_cred(arlist *list, char *nick, char *path)
{
	for (size_t i=0; i<arlist_size(list); i++) {
		user *u = arlist_get(list,i);
		if (strcmp(u->nick, nick)==0 || strcmp(u->path, path)==0)
			return false;
	}
	return true;
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

user *
user_from_pipe_path(arlist *list, char *path)
{
	for (size_t i=0; i<arlist_size(list); i++) {
		user *u = arlist_get(list,i);
		if (strcmp(u->path, path)==0)
			return u;
	}
	return NULL;
}

void
remove_user(user *u)
{
	if (!u) return;
	arlist_remove(serv->users, index_of(serv->users, compare_users, u));
	//char *path = strdup(u->path);
	user_destroy(u);
	//unlink(path); // not your job
	//free(path);
}

ssize_t
send_to(user *u, data d)
{
	ssize_t ret;
	if ((ret = writedata(u->pipe, d)) == -1 && errno == EPIPE)
		remove_user(u);
	return ret;
}

void
broadcast(arlist *list, data d)
{
	user *u;
	for (size_t i=0; i<arlist_size(list); i++) {
		u = arlist_get(list, i);
		if (writedata(u->pipe, d) == -1 && errno == EPIPE) { /* closed pipe */
			remove_user(u);
			--i;
		}
	}
}
