#ifndef PROCESS_SERVER_H
#define PROCESS_SERVER_H

#include <stdint.h>
#include "data.h"

int pro_client_HELO(char *nick, char *pipe);
int pro_client_BYEE(uint32_t id);
int pro_client_BCST(uint32_t id, char *msg, size_t msglen);
int pro_client_PRVT(uint32_t id, char *nick, char *msg, size_t msglen);
int pro_client_LIST(uint32_t id);
int pro_client_SHUT(uint32_t id, char *password);
int pro_client_DEBG(char *password);
int pro_client_FILE_announce(uint32_t id, char *nick, uint32_t len, char *filename);
int pro_client_FILE_transfer(uint32_t serie, uint32_t idtransfer, data buf);

#endif
