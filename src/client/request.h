#ifndef REQUEST_SERVER_H
#define REQUEST_SERVER_H

#include <stdint.h>
#include "data.h"

data req_server_HELO(char *pseudo, char *pipe);
data req_server_BYEE(uint32_t id);
data req_server_BCST(uint32_t id, char *msg, size_t msglen);
data req_server_PRVT(uint32_t id, char *pseudo, char *msg, size_t msglen);
data req_server_LIST(uint32_t id);
data req_server_SHUT(uint32_t id, char *password);
data req_server_DEBG(char *password);
data req_server_FILE_announce(uint32_t id, char *pseudo, uint32_t len, char *filename);
data req_server_FILE_transfer(uint32_t serie, uint32_t idtransfer, data buf);

#endif
