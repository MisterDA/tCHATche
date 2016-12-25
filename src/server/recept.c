#include "packet_reception.h"

#include "packet.h"
#include <stdio.h> //DEV
#include <unistd.h> //DEV

#define HELO TYPE('H','E','L','O')
#define BYEE TYPE('B','Y','E','E')
#define BCST TYPE('B','C','S','T')
#define PRVT TYPE('P','R','V','T')
#define LIST TYPE('L','I','S','T')
#define SHUT TYPE('S','H','U','T')
#define DEBG TYPE('D','E','B','G')
#define FILE TYPE('F','I','L','E')

static int received_HELO(char *pseudo, char *pipe);
static int received_BYEE(uint32_t id);
static int received_BCST(uint32_t id, char *msg, size_t msglen);
static int received_SHUT(uint32_t id, char *password);

int
process_packet(data d)
{
	shift_data(&d, SIZEOF_NUM);
	char *t = read_type(&d);
	if (!t) return -1;
	switch (*(uint32_t *)t) {
		case HELO:
		{
			char pseudo[32+1]; //TODO PSEUDO_MAX_LENGTH
			if (!read_str(&d, pseudo, 32+1)) return ERR_INVALID;
			char pipe[256+1]; //TODO PIPE_MAX_LENGTH
			if (!read_str(&d, pipe, 256+1)) return ERR_INVALID;
			return received_HELO(pseudo, pipe);
		}
		case BYEE:
		{
			int id = read_num(&d);
			if (id>MAX_NUM) return ERR_INVALID;
			return received_BYEE(id);
		}
		case BCST:
		{
			int id = read_num(&d);
			if (id>MAX_NUM) return ERR_INVALID;
			data msg;
			if (!read_mem(&d, &msg)) return ERR_INVALID;
			return received_BCST(id, msg.ata, msg.length);
		}
		case PRVT: return 0;
		case LIST: return 0;
		case SHUT:
		{
			int id = read_num(&d);
			if (id>MAX_NUM) return ERR_INVALID;
			if (d.length==0)
				return received_SHUT(id, NULL);
			char pass[64+1]; //TODO PASSWORD_MAX_LENGTH
			if (!read_str(&d, pass, 64+1)) return ERR_INVALID;
			return received_SHUT(id, pass);
		}
		case DEBG: return 0;
		case FILE: return 0;
		default: return ERR_UNKNOWN;
	}
}


static int
received_HELO(char *pseudo, char *pipe)
{
	printf("[%s] wants to join\n", pseudo);
	//TODO give id to client
	return 0;
}

static int
received_BYEE(uint32_t id)
{
	printf("User%u disconnects\n", id);
	//TODO suppress user
	return 0;
}

static int
received_BCST(uint32_t id, char *msg, size_t msglen)
{
	printf("User%u : ", id); fflush(stdout);
	write(1, msg, msglen);
	putchar('\n');
	//TODO answer to clients
	return 0;
}

static int
received_SHUT(uint32_t id, char *password)
{
	printf("SHUT (%s) from User%u", password, id); fflush(stdout);
	putchar('\n');
	//TODO shutdown
	return 0;
}
