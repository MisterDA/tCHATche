#include "packet_reception.h"

#include "packet.h"

#define HELO TYPE('H','E','L','O')
#define BYEE TYPE('B','Y','E','E')
#define BCST TYPE('B','C','S','T')
#define PRVT TYPE('P','R','V','T')
#define LIST TYPE('L','I','S','T')
#define SHUT TYPE('S','H','U','T')
#define DEBG TYPE('D','E','B','G')
#define FILE TYPE('F','I','L','E')

int process_packet(data packet) {
	char *t = read_type(&packet);
	if (!t) return -1;
	switch (*(uint32_t *)t) {
		case HELO: return 0;
		case BYEE: return 0;
		case BCST: return 0;
		case PRVT: return 0;
		case LIST: return 0;
		case SHUT: return 0;
		case DEBG: return 0;
		case FILE: return 0;
		default: return -1;
	}
}
