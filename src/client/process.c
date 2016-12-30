#include "process.h"

#include "client.h"

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
	return 0;
}

int
pro_server_BCST(char *nick, char *msg, __attribute__((unused)) size_t msglen)
{
	tui_add_msg(cl->ui, &(tui_msg){time(NULL), nick, msg});
	return 0;
}

int
pro_server_PRVT(char *nick, char *msg, size_t msglen)
{
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
