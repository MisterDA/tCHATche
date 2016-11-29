#ifndef USER_H
#define USER_H

#include <sys/types.h>
#include "packet.h"
#include "arraylist.h"

typedef int user_id;

typedef struct {
	user_id id;
	char *name;
	int pipe;
} user;

user *user_from_id(arlist *list, user_id id);
user *user_from_name(arlist *list, char *name);
ssize_t send_to(user *u, data data);

#endif
