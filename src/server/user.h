#ifndef USER_H
#define USER_H

#include <stddef.h>

typedef int user_id;

typedef struct {
	user_id id;
	char *name;
	int pipe;
} user;

user *user_from_id(arlist *list, user_id id);
user *user_from_name(arlist *list, char *name);
void send(user u, char *message, size_t len);

#endif
