#include "process.h"

#include <sys/stat.h>
#include <fcntl.h>
#include "server.h"
#include "user.h"
#include "request.h"
#include "tchatche.h"

#include <stdio.h> //DEV
#include <unistd.h> //DEV


int
pro_client_HELO(char *nick, char *path)
{
	printf("[%s] wants to join\n", nick); //DEV
	
	int pipe = open(path, O_WRONLY);
	if (pipe == -1)
		return 0; // on peut pas envoyer de BADD vu qu'y a pas de pipe
	if (user_from_nick(serv->users, nick) != NULL)
		goto badd;
	user_id id = get_available_id(serv->users);
	if (id > MAX_USERS)
		goto badd;
	user *u = user_create(id, nick, pipe);
	arlist_add(serv->users, compare_users, u);
	writedata(pipe, req_server_OKOK(id));
	return 0;

	badd:
	writedata(pipe, req_server_BADD()); // pipe c'est quelle variable ? une statique ?
	return 0;
}

int
pro_client_BYEE(uint32_t id)
{
	printf("User%u disconnects\n", id);
	//TODO suppress user
	return 0;
}

int
pro_client_BCST(uint32_t id, char *msg, size_t msglen)
{
	printf("User%u : ", id); fflush(stdout);
	write(1, msg, msglen);
	putchar('\n');
	//TODO broadcast to every client
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
	//TODO send list
	return 0;
}

int
pro_client_SHUT(uint32_t id, char *password)
{
	printf("SHUT (%s) from User%u", password, id); fflush(stdout);
	putchar('\n');
	//TODO shutdown
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

