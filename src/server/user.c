#include "user.h"

#include <string.h>
#include <unistd.h>

user *user_from_id(arlist *list, user_id id) {
	for (size_t i=0; i<arlist_size(list); i++) {
		user *u = arlist_get(list,i);
		if (u->id==id)
			return u;
	}
	return NULL;
}

user *user_from_name(arlist *list, char *name) {
	for (size_t i=0; i<arlist_size(list); i++) {
		user *u = arlist_get(list,i);
		if (strcmp(u->name, name)==0)
			return u;
	}
	return NULL;
}

ssize_t send_to(user *u, char *buf, size_t count) {
	return write(u->pipe, buf, count);
}
