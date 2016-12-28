/* CLIENT */
#include "packet_reception.h"

#include "process.h"
#include "packet.h"

#include "client.h"//DEV

#define OKOK TYPE('O','K','O','K')
#define BADD TYPE('H','E','L','O')
#define BYEE TYPE('B','Y','E','E')
#define BCST TYPE('B','C','S','T')
#define PRVT TYPE('P','R','V','T')
#define LIST TYPE('L','I','S','T')
#define SHUT TYPE('S','H','U','T')
#define FILE TYPE('F','I','L','E')

// TODO: global define
#define NICK_MAX_LENGTH 32
#define PIPE_MAX_LENGTH 256
#define FILE_MAX_LENGTH 64
#define PASS_MAX_LENGTH 64

int
process_packet(data d)
{
	shift_data(&d, SIZEOF_NUM);
	char *t = read_type(&d);
	if (!t) return ERR_INVALID;
	tui_add_txt(cl->ui, d.ata); //DEV
	switch (*(uint32_t *)t) {
		case OKOK:
		{
			uint32_t id = read_num(&d);
			if (id>MAX_NUM) return ERR_INVALID;
			return pro_server_OKOK(id);
		}
		case BADD:
		{
			return pro_server_BADD();
		}
		case BYEE:
		{
			uint32_t id = read_num(&d);
			if (id>MAX_NUM) return ERR_INVALID;
			return pro_server_BYEE(id);
		}
		case BCST:
		{
			char nick[NICK_MAX_LENGTH+1];
			if (!read_str(&d, nick, NICK_MAX_LENGTH+1)) return ERR_INVALID;
			data msg;
			if (!read_mem(&d, &msg)) return ERR_INVALID;
			return pro_server_BCST(nick, msg.ata, msg.length);
		}
		case PRVT:
		{
			char nick[NICK_MAX_LENGTH+1];
			if (!read_str(&d, nick, NICK_MAX_LENGTH+1)) return ERR_INVALID;
			data msg;
			if (!read_mem(&d, &msg)) return ERR_INVALID;
			return pro_server_PRVT(nick, msg.ata, msg.length);
		}
		case LIST:
		{
			uint32_t n = read_num(&d);
			if (n>MAX_NUM) return ERR_INVALID;
			char nick[NICK_MAX_LENGTH+1];
			if (!read_str(&d, nick, NICK_MAX_LENGTH+1)) return ERR_INVALID;
			return pro_server_LIST(n, nick);
		}
		case SHUT:
		{
			data d2 = d;
			uint32_t id = read_num(&d);
			if (id>MAX_NUM) return ERR_INVALID;
			if (d.length==0)
				return pro_server_SHUT(NULL);
			d = d2;
			char nick[NICK_MAX_LENGTH+1];
			if (!read_str(&d, nick, NICK_MAX_LENGTH+1)) return ERR_INVALID;
			return pro_server_SHUT(nick);
		}
		case FILE:
		{
			uint32_t serie = read_num(&d);
			if (serie>MAX_NUM) return ERR_INVALID;
			if (serie == 0) {
				uint32_t idtransfer = read_num(&d);
				if (idtransfer>MAX_NUM) return ERR_INVALID;
				uint32_t len = read_longnum(&d);
				if (len>MAX_LONGNUM) return ERR_INVALID;
				char filename[FILE_MAX_LENGTH+1];
				if (!read_str(&d, filename, FILE_MAX_LENGTH+1)) return ERR_INVALID;
				if (d.length==0)
					return pro_server_FILE_announce(idtransfer, len, filename, NULL);
				char nick[NICK_MAX_LENGTH+1];
				if (!read_str(&d, nick, NICK_MAX_LENGTH+1)) return ERR_INVALID; // accept ?
				return pro_server_FILE_announce(idtransfer, len, filename, nick);
			} else {
				uint32_t idtransfer = read_num(&d);
				if (idtransfer>MAX_NUM) return ERR_INVALID;
				data buf;
				if (!read_mem(&d, &buf)) return ERR_INVALID;
				return pro_server_FILE_transfer(serie, idtransfer, buf);
			}
		}
		default: return ERR_UNKNOWN;
	}
}
