#ifndef REQUEST_SERVER_H
#define REQUEST_SERVER_H

#include <stdint.h>
#include "data.h"

data req_server_OKOK(uint32_t id);
data req_server_BADD();
data req_server_BYEE(uint32_t id);
data req_server_BCST(char *nick, char *msg, size_t msglen);
data req_server_PRVT(char *nick, char *msg, size_t msglen);
data req_server_LIST(uint32_t n, char *nick);
data req_server_SHUT(char *nick);
data req_server_FILE_announce(uint32_t intransfert, uint32_t len, char *filename, char *nick);
data req_server_FILE_transfer(uint32_t serie, uint32_t idtransfer, data buf);

#endif
