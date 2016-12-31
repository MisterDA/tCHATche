#include "process.h"

#include "client.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h> //DEV
#include <unistd.h> //DEV
#include "tchatche.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

int
pro_server_OKOK(uint32_t id)
{
	tui_print_txt(cl->ui, "Nick has been set to <%s>.\n", cl->nick);
	if (!cl->has_id) {
		cl->has_id = true;
		cl->id = id;
	}
	return 0;
}

int
pro_server_BADD()
{
	tui_add_txt(cl->ui, "Invalid nickname.\n");
	return 0;
}

int
pro_server_BYEE(uint32_t id)
{
	if (cl->id != id)
		return -1;
	client_end(cl);
	exit(EXIT_SUCCESS);
	return 0;
}

int
pro_server_BCST(char *nick, char *msg, size_t msglen)
{
	//FIXME msg est un pointeur sur le buff originel, au choix, faire une copie, un malloc ?
	//ATTENTION: nick est aussi un pointeur sur un buff temporaire (mais null-terminated)
	char *message = malloc(msglen+1);
	strncpy(message, msg, msglen);
	message[msglen] = '\0';
	tui_add_msg(cl->ui, &(tui_msg){time(NULL), nick, message});
	free(message); // ou bien ajouter à une structure de donnée ?
	return 0;
}

int
pro_server_PRVT(char *nick, char *msg, size_t msglen)
{
	//TODO apparence de msg privé
	char *message = malloc(msglen+2+1);
	message[0] = '*';
	strncpy(message+1, msg, msglen);
	message[msglen+1] = '*';
	message[msglen+2] = '\0';
	tui_add_msg(cl->ui, &(tui_msg){time(NULL), nick, message});
	free(message);
	return 0;
}

int
pro_server_LIST(uint32_t n, char *nick)
{
	tui_add_user(cl->ui, nick);
	return 0;
}

int
pro_server_SHUT(char *nick)
{
	tui_print_txt(cl->ui, "<%s> has stopped the server !\n", nick);
	client_end(cl);
	exit(EXIT_SUCCESS);
	return 0;
}

int
pro_server_FILE_announce(uint32_t intransfert, uint32_t len, char *filename, char *nick)
{
	return 0;
}

int
pro_server_FILE_transfer(uint32_t serie, uint32_t idtransfer, data buf)
{
	return 0;
}

#pragma GCC diagnostic pop
