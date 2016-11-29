#ifndef REQUEST_SERVER_H
#define REQUEST_SERVER_H

#include "packet.h"
//#include "user.h" //attention (cf user_id)

data req_server_OKOK(uint32_t id);
data req_server_BADD();
data req_server_BYEE(uint32_t id);
data req_server_BCST(char *pseudo, char *msg, size_t msglen);
data req_server_PRVT(char *pseudo, char *msg, size_t msglen);
data req_server_LIST(uint32_t n, char *pseudo);
data req_server_SHUT(char *pseudo);
data req_server_FILE_announce(uint32_t intransfert, uint32_t len, char *filename, char *pseudo);


#endif
