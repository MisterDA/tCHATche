#include "request.h"

#define BUFFER buffer
#include "packet_edition.h"
static char buffer[MAX_REQUEST_LENGTH+1];

data
req_client_HELO(char *pseudo, char *pipe)
{
	INIT(d);
	DO write_str(&d, pseudo);
	DO write_str(&d, pipe);
	END(d);
}

data
req_client_BYEE(uint32_t id)
{
	INIT(d);
	DO write_type(&d, "BYEE");
	DO write_num(&d, id);
	END(d);
}

data
req_client_BCST(uint32_t id, char *msg, size_t msglen)
{
	INIT(d);
	DO write_type(&d, "BCST");
	DO write_num(&d, id);
	DO write_mem(&d, msg, msglen);
	END(d);
}

data
req_client_PRVT(uint32_t id, char *pseudo, char *msg, size_t msglen)
{
	INIT(d);
	DO write_type(&d, "PRVT");
	DO write_num(&d, id);
	DO write_str(&d, pseudo);
	DO write_mem(&d, msg, msglen);
	END(d);
}

data
req_client_LIST(uint32_t id)
{
	INIT(d);
	DO write_type(&d, "LIST");
	DO write_num(&d, id);
	END(d);
}

data
req_client_SHUT(uint32_t id, char *password)
{
	INIT(d);
	DO write_type(&d, "SHUT");
	DO write_num(&d, id);
	if (password) // extention du protocole : mot de passe admin
		DO write_str(&d, password);
	END(d);
}

data
req_client_DEBG(char *password)
{
	INIT(d);
	DO write_type(&d, "DEBG");
	if (password) // extention du protocole : mot de passe admin
		DO write_str(&d, password);
	END(d);
}

data
req_client_FILE_announce(uint32_t id, char *pseudo, uint32_t len, char *filename)
{
	INIT(d);
	DO write_type(&d, "FILE");
	DO write_num(&d, 0);
	DO write_num(&d, id);
	DO write_str(&d, pseudo);
	DO write_longnum(&d, len);
	DO write_str(&d, filename);
	END(d);
}

data
req_client_FILE_transfer(uint32_t serie, uint32_t idtransfer, data buf)
{
	INIT(d);
	DO write_type(&d, "FILE");
	TEST(serie!=0);
	DO write_num(&d, serie);
	DO write_num(&d, idtransfer);
	DO write_data(&d, &buf);
	END(d);
}

