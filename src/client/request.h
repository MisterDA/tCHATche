#ifndef REQUEST_CLIENT_H
#define REQUEST_CLIENT_H

#include <stdint.h>
#include "data.h"

data req_client_HELO(char *pseudo, char *pipe);
data req_client_BYEE(uint32_t id);
data req_client_BCST(uint32_t id, char *msg, size_t msglen);
data req_client_PRVT(uint32_t id, char *pseudo, char *msg, size_t msglen);
data req_client_LIST(uint32_t id);
data req_client_SHUT(uint32_t id, char *password);
data req_client_DEBG(char *password);
data req_client_FILE_announce(uint32_t id, char *pseudo, uint32_t len, char *filename);
data req_client_FILE_transfer(uint32_t serie, uint32_t idtransfer, data buf);

#endif
