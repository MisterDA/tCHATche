#include "packet_reception.h"

#include "packet.h"
#include "client.h"

#define OKOK TYPE('O','K','O','K')
#define BADD TYPE('H','E','L','O')
#define BYEE TYPE('B','Y','E','E')
#define BCST TYPE('B','C','S','T')
#define PRVT TYPE('P','R','V','T')
#define LIST TYPE('L','I','S','T')
#define SHUT TYPE('S','H','U','T')
#define FILE TYPE('F','I','L','E')

int
process_packet(data packet)
{
	char *t = read_type(&packet);
	if (!t) return -1;
	tui_add_txt(cl->ui, packet.ata);
	switch (*(uint32_t *)t) {
		case OKOK: return 0;
		case BADD: return 0;
		case BYEE: return 0;
		case BCST: return 0;
		case PRVT: return 0;
		case LIST: return 0;
		case SHUT: return 0;
		case FILE: return 0;
		default: return -1;
	}
}
