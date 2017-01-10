#include "process.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "server.h"
#include "user.h"
#include "request.h"
#include "tchatche.h"


int
pro_client_HELO(char *nick, char *path)
{
	int pipe;
	logs("HELO from {nick: \"%s\"; path: \"%s\"}\n", nick, path);
	if ((pipe = open(path, O_WRONLY)) == -1)
		return 0; /* can't send BADD because there is no pipe */
	if (!is_valid_cred(serv->users, nick, path))
		goto badd;
	user_id id = get_available_id(serv->users);
	if (id > MAX_USERS)
		goto badd;
	user *u = user_create(id, nick, path, pipe);
	arlist_add(serv->users, compare_users, u);
	send_to(u, req_server_OKOK(id));
	logs("Added {nick: \"%s\"; id: %d; path: \"%s\"; pipe: \"%d\"}\n",
			nick, id, path, pipe);
	return 0;

	badd:
	logs("Refused {nick: \"%s\"; path: \"%s\"}\n", nick, path);
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
	send_to(u, req_server_BYEE(id));
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
	logs("%s <%u:%s> [%zu] %s\n", time_buf, id, u->nick, msglen, msg);
	broadcast(serv->users, req_server_BCST(u->nick, msg, msglen));
	return 0;
}

int
pro_client_PRVT(uint32_t id, char *nick, char *msg, size_t msglen)
{
	user *u = user_from_id(serv->users, id);
	user *cl = user_from_nick(serv->users, nick);
	if (u == NULL) return -1;
	char time_buf[6];
	time_t t = time(NULL);
	strftime(time_buf, 6, "%H:%M", localtime(&t));
	logs("%s <%u:%s->%s> [%zu] %s\n", time_buf, id, u->nick, nick, msglen, msg);
	if (cl)
		send_to(cl, req_server_PRVT(u->nick, msg, msglen));
	else
		send_to(u, req_server_BCST("SERVER", "unkown user", 11)); // error message
	return 0;
}

int
pro_client_LIST(uint32_t id)
{
	user *u, *cl;
	if ((u = user_from_id(serv->users, id)) == NULL)
		return -1;
	for (size_t i = 0; i < arlist_size(serv->users); ++i) {
		cl = arlist_get(serv->users, i);
		send_to(u, req_server_LIST(arlist_size(serv->users), cl->nick));
	}
	return 0;
}

int
pro_client_SHUT(uint32_t id, char *password)
{
	/* TODO: deal with the password */
	user *u = user_from_id(serv->users, id);
	if (u) {
		logs("SHUT from {id: \"%u\"; nick: \"%s\"; passwd: \"%s\"}\n",
			id, u->nick, password);
		broadcast(serv->users, req_server_SHUT(u->nick));
	} else {
		logs("SHUT\n");
		broadcast(serv->users, req_server_SHUT(NULL));
	}
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
	logs("FILE announce: {id: \"%u\"; nick: \"%s\"; len: \"%d\"; filename: \"%s\"}\n",
		id, nick, len, filename);
	user *sender = user_from_id(serv->users, id);
	user *receiver = user_from_nick(serv->users, nick);
	if (sender == NULL) return -1;
	if (receiver == NULL || sender == receiver) {
		send_to(sender, req_server_BADD());
		return 0;
	}

	transfer *t = malloc(sizeof(*t));
	t->id = get_available_transfer_id(serv->transfers);
	t->sender = sender;
	t->receiver = receiver;
	t->len = len;
	arlist_add(serv->transfers, compare_transfers, t);

	send_to(receiver, req_server_FILE_announce(t->id, len, filename, sender->nick));
	send_to(sender, req_server_OKOK(t->id));
	return 0;
}

int
pro_client_FILE_transfer(uint32_t serie, uint32_t idtransfer, data buf)
{
	logs("FILE transfert: {id: \"%u\"; serie: \"%u\"}\n", idtransfer, serie);
	transfer *t = transfer_from_id(serv->transfers, idtransfer);
	if (!t) return -1;
	send_to(t->receiver, req_server_FILE_transfer(serie, idtransfer, buf));
	if (serie * 256 >= t->len)
		arlist_remove(serv->transfers, index_of(serv->transfers, compare_transfers, t));
	return 0;
}
