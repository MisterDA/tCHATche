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
	char *err_mes = NULL;
	logs("HELO from {nick: \"%s\"; path: \"%s\"}\n", nick, path);
	if ((pipe = open(path, O_WRONLY)) == -1) {
		logs("Refused {nick: \"%s\"; path: \"%s\"} (invalid path)\n", nick, path);
		return 0; /* can't send BADD because there is no pipe */
	}
	if (!is_valid_cred(serv->users, nick, path)) {
		err_mes = "this nick is already used";
		goto badd;
	}
	user_id id = get_available_id(serv->users);
	if (id > MAX_USERS) {
		err_mes = "too many people here";
		goto badd;
	}
	if (strcmp(nick, "SERVER")==0) goto badd_nick;
	if (strcmp(nick, "")==0) goto badd_nick;
	user *u = user_create(id, nick, path, pipe);
	arlist_add(serv->users, compare_users, u);
	send_to(u, req_server_OKOK(id));
	logs("Added {nick: \"%s\"; id: %d; path: \"%s\"; pipe: \"%d\"}\n",
			nick, id, path, pipe);
	return 0;

	badd_nick:
	err_mes = "forbidden nick";
	badd:
	logs("Refused {nick: \"%s\"; path: \"%s\"}\n", nick, path);
	writedata(pipe, req_server_BADD());
	if (err_mes)
		writedata(pipe, req_server_BCST("SERVER", err_mes, strlen(err_mes)));
	return 0;
}

int
pro_client_BYEE(uint32_t id)
{
	user *u = user_from_id(serv->users, id);
	if (u == NULL) return -1;
	logs("BYEE from {id: %u; nick: \"%s\"}\n", id, u->nick);
	send_to(u, req_server_BYEE(id));
	remove_user(user_from_id(serv->users, id));
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
	
	char *temp = strndup(msg,msglen);
	logs("%s <%04u:%s> %s\n", time_buf, id, u->nick, temp);
	free(temp);
	
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
	char *temp = strndup(msg,msglen);
	
	if (cl)
		logs("%s <%04u:%s->%04u:%s> %s\n", time_buf, id, u->nick, cl->id, nick, temp);
	else
		logs("%s <%04u:%s->????:%s> %s\n", time_buf, id, u->nick, nick, temp);
	free(temp);
	
	if (cl)
		send_to(cl, req_server_PRVT(u->nick, msg, msglen));
	else {
		char *msg;
		asprintf(&msg, "%s is not online", nick);
		send_to(u, req_server_BCST("SERVER", msg, strlen(msg))); // error message
		free (msg);
	}
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
		if (password)
			logs("SHUT from {id: %u; nick: \"%s\"; passwd: \"%s\"}\n",
				id, u->nick, password);
		else
			logs("SHUT from {id: %u; nick: \"%s\"}\n",
				id, u->nick);
		broadcast(serv->users, req_server_SHUT(u->nick));
	} else {
		if (password)
			logs("SHUT {passwd: \"%s\"}\n", password);
		else
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
	logs("\x1B[0;1;31m" "I'm a turtle!");
	if (password)
		logs(" (%s)", password);
	logs("\x1B[0m" "\n");
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
