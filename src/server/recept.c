/* SERVER */
#include "packet_reception.h"


#include <stdlib.h>
#include "tchatche.h"
#include "server.h"

#include "process.h"
#include "packet.h"


#define HELO TYPE('H','E','L','O')
#define BYEE TYPE('B','Y','E','E')
#define BCST TYPE('B','C','S','T')
#define PRVT TYPE('P','R','V','T')
#define LIST TYPE('L','I','S','T')
#define SHUT TYPE('S','H','U','T')
#define DEBG TYPE('D','E','B','G')
#define FILE TYPE('F','I','L','E')

// TODO: global define
#define NICK_MAX_LENGTH 32
#define PIPE_MAX_LENGTH 256
#define FILE_MAX_LENGTH 64
#define PASS_MAX_LENGTH 64

int
process_packet(data d)
{
	if (show_packets) {
		char *temp = datatostr(&d, NULL, 0);
		logs("\x1B[0;1;35m" "%s" "\x1B[0m" "\n", temp);
		free(temp);
	}

	shift_data(&d, SIZEOF_NUM);
	char *t = read_type(&d);
	if (!t) return ERR_INVALID;
	switch (*(uint32_t *) t) {
		case HELO:
		{
			char *nick_ = NULL;
			char nick[NICK_MAX_LENGTH+1];
			data d_ = d;
			uint32_t nl = read_num(&d_);
			if (nl>MAX_NUM) return ERR_INVALID;
			if (nl>NICK_MAX_LENGTH) {
				shift_data(&d, SIZEOF_NUM+nl);
			} else {
				if (!read_str(&d, nick, NICK_MAX_LENGTH+1)) return ERR_INVALID;
				nick_ = nick;
			}
			char pipe[PIPE_MAX_LENGTH+1];
			if (!read_str(&d, pipe, PIPE_MAX_LENGTH+1)) return ERR_INVALID;
			return pro_client_HELO(nick_, pipe);
		}
		case BYEE:
		{
			uint32_t id = read_num(&d);
			if (id>MAX_NUM) return ERR_INVALID;
			return pro_client_BYEE(id);
		}
		case BCST:
		{
			uint32_t id = read_num(&d);
			if (id>MAX_NUM) return ERR_INVALID;
			data msg;
			if (!read_mem(&d, &msg)) return ERR_INVALID;
			return pro_client_BCST(id, msg.ata, msg.length);
		}
		case PRVT:
		{
			uint32_t id = read_num(&d);
			if (id>MAX_NUM) return ERR_INVALID;
			char nick[NICK_MAX_LENGTH+1];
			if (!read_str(&d, nick, NICK_MAX_LENGTH+1)) return ERR_INVALID;
			data msg;
			if (!read_mem(&d, &msg)) return ERR_INVALID;
			return pro_client_PRVT(id, nick, msg.ata, msg.length);
		}
		case LIST:
		{
			uint32_t id = read_num(&d);
			if (id>MAX_NUM) return ERR_INVALID;
			return pro_client_LIST(id);
		}
		case SHUT:
		{
			uint32_t id = read_num(&d);
			//if (id>MAX_NUM) return ERR_INVALID;
			if (d.length==0)
				return pro_client_SHUT(id, NULL);
			char pass[PASS_MAX_LENGTH+1];
			if (!read_str(&d, pass, PASS_MAX_LENGTH+1)) return ERR_INVALID;
			return pro_client_SHUT(id, pass);
		}
		case DEBG:
		{
			if (d.length==0)
				return pro_client_DEBG(NULL);
			char pass[PASS_MAX_LENGTH+1];
			if (!read_str(&d, pass, PASS_MAX_LENGTH+1)) return ERR_INVALID;
			return pro_client_DEBG(pass);
		}
		case FILE:
		{
			uint32_t serie = read_num(&d);
			if (serie>MAX_NUM) return ERR_INVALID;
			if (serie == 0) {
				uint32_t id = read_num(&d);
				if (id>MAX_NUM) return ERR_INVALID;
				char *nick_ = NULL;
				char nick[NICK_MAX_LENGTH+1];
				data d_ = d;
				uint32_t nl = read_num(&d_);
				if (nl>MAX_NUM) return ERR_INVALID;
				if (nl>NICK_MAX_LENGTH) {
					shift_data(&d, SIZEOF_NUM+nl);
				} else {
					if (!read_str(&d, nick, NICK_MAX_LENGTH+1)) return ERR_INVALID;
					nick_ = nick;
				}
				uint32_t len = read_longnum(&d);
				if (len>MAX_LONGNUM) return ERR_INVALID;
				char filename[FILE_MAX_LENGTH+1];
				if (!read_str(&d, filename, FILE_MAX_LENGTH+1)) return ERR_INVALID;
				return pro_client_FILE_announce(id, nick_, len, filename);
			} else {
				uint32_t idtransfer = read_num(&d);
				if (idtransfer>MAX_NUM) return ERR_INVALID;
				data buf;
				if (!read_mem(&d, &buf)) return ERR_INVALID;
				return pro_client_FILE_transfer(serie, idtransfer, buf);
			}
		}
		default: return ERR_UNKNOWN;
	}
}
