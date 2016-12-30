#include "process.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "server.h"
#include "user.h"
#include "request.h"
#include "tchatche.h"

#include <stdio.h> //DEV
#include <unistd.h> //DEV

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

int
pro_client_HELO(char *nick, char *path)
{
	int pipe;
	printf("HELO from {nick: \"%s\"; path: \"%s\"}\n", nick, path);
	if ((pipe = open(path, O_WRONLY)) == -1)
		return 0; /* can't send BADD because there is no pipe */
	if (!is_valid_cred(serv->users, nick, path))
		goto badd;
	user_id id = get_available_id(serv->users);
	if (id > MAX_USERS)
		goto badd;
	user *u = user_create(id, nick, path, pipe);
	arlist_add(serv->users, compare_users, u);
	writedata(pipe, req_server_OKOK(id));
	printf("Added {nick: \"%s\"; id: %d; path: \"%s\"; pipe: \"%d\"}\n",
			nick, id, path, pipe);
	return 0;

	badd:
	printf("Refused {nick: \"%s\"; path: \"%s\"}\n", nick, path);
	writedata(pipe, req_server_BADD());
	return 0;
}

int
pro_client_BYEE(uint32_t id)
{
	size_t i = 0;
	user *u = NULL;
	for (; i < arlist_size(serv->users); ++i) {
		user *v = arlist_get(serv->users, i);
		if (v->id == id) {
			u = v;
			break;
		}
	}
	if (u == NULL) return -1;
	writedata(u->pipe, req_server_BYEE(id));
	arlist_remove(serv->users, i);
	user_destroy(u);
	return 0;
}

int
pro_client_BCST(uint32_t id, char *msg, size_t msglen)
{
	user *u = user_from_id(serv->users, id);
	if (u == NULL) return -1;
	char time_buf[6];
	time_t t = time(NULL);
	strftime(time_buf, 6, "%H:%M", localtime(&t));
	printf("%s <%u:%s> [%zu] %s\n", time_buf, id, u->nick, msglen, msg);
	broadcast(serv->users, req_server_BCST(u->nick, msg, msglen));
	return 0;
}

int
pro_client_PRVT(uint32_t id, char *nick, char *msg, size_t msglen)
{
	//TODO broadcast to single client
	return 0;
}

int
pro_client_LIST(uint32_t id)
{
	user *u, *cl;
	if ((cl = user_from_id(serv->users, id)) == NULL)
		return -1;
	for (size_t i = 0; i < arlist_size(serv->users); ++i) {
		u = arlist_get(serv->users, i);
		writedata(cl->pipe, req_server_LIST(i, u->nick));
	}
	return 0;
}

int
pro_client_SHUT(uint32_t id, char *password)
{
	/* TODO: deal with the password */
	user *u = user_from_id(serv->users, id);
	printf("SHUT from {id: \"%u\"; nick: \"%s\"; pwd: \"%s\"}\n",
			id, u->nick, password);
	broadcast(serv->users, req_server_SHUT(u->nick));
	server_end(serv);
	exit(EXIT_SUCCESS);
	return 0;
}

int
pro_client_DEBG(char *password)
{
	//TODO do something (!)
	return 0;
}

int
pro_client_FILE_announce(uint32_t id, char *nick, uint32_t len, char *filename)
{
	//TODO a lot of things
	return 0;
}

int
pro_client_FILE_transfer(uint32_t serie, uint32_t idtransfer, data buf)
{
	//TODO same here
	return 0;
}

#pragma GCC diagnostic pop
