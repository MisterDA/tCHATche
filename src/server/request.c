#include "request.h"

#define BUFFER buffer
#include "packet_edition.h"
static char buffer[MAX_REQUEST_LENGTH+1];

data
req_server_OKOK(uint32_t id)
{
	INIT(d);
	DO write_type(&d, "OKOK");
	DO write_num(&d, id);
	END(d);
}

data
req_server_BADD()
{
	INIT(d);
	DO write_type(&d, "BADD");
	END(d);
}

data
req_server_BYEE(uint32_t id)
{
	INIT(d);
	DO write_type(&d, "BYEE");
	DO write_num(&d, id);
	END(d);
}

data
req_server_BCST(char *nick, char *msg, size_t msglen)
{
	INIT(d);
	DO write_type(&d, "BCST");
	DO write_str(&d, nick);
	DO write_mem(&d, msg, msglen);
	END(d);
}

data
req_server_PRVT(char *nick, char *msg, size_t msglen)
{
	INIT(d);
	DO write_type(&d, "PRVT");
	DO write_str(&d, nick);
	DO write_mem(&d, msg, msglen);
	END(d);
}

data
req_server_LIST(uint32_t n, char *nick)
{
	INIT(d);
	DO write_type(&d, "LIST");
	DO write_num(&d, n);
	DO write_str(&d, nick);
	END(d);
}

data
req_server_SHUT(char *nick)
{
	INIT(d);
	DO write_type(&d, "SHUT");
	DO write_str(&d, nick);
	END(d);
}

data
req_server_FILE_announce(uint32_t idtransfer, uint32_t len, char *filename, char *nick)
{
	INIT(d);
	DO write_type(&d, "FILE");
	DO write_num(&d, 0);
	DO write_num(&d, idtransfer);
	DO write_longnum(&d, len);
	DO write_str(&d, filename);
	if (nick) /* extension du protocole : pseudo de l'expéditeur */
		DO write_str(&d, nick);
	END(d);
}

data
req_server_FILE_transfer(uint32_t serie, uint32_t idtransfer, data buf)
{
	INIT(d);
	DO write_type(&d, "FILE");
	TEST(serie!=0);
	DO write_num(&d, serie);
	DO write_num(&d, idtransfer);
	DO write_data(&d, &buf);
	END(d);
}
