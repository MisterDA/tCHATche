#ifndef PROCESS_CLIENT_H
#define PROCESS_CLIENT_H

#include <stdint.h>
#include "data.h"

int pro_server_OKOK(uint32_t id);
int pro_server_BADD();
int pro_server_BYEE(uint32_t id);
int pro_server_BCST(char *nick, char *msg, size_t msglen);
int pro_server_PRVT(char *nick, char *msg, size_t msglen);
int pro_server_LIST(uint32_t n, char *nick);
int pro_server_SHUT(char *nick);
int pro_server_FILE_announce(uint32_t intransfert, uint32_t len, char *filename, char *nick);
int pro_server_FILE_transfer(uint32_t serie, uint32_t idtransfer, data buf);

#endif
