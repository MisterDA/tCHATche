#ifndef PACKET_RECEPTION_H
#define PACKET_RECEPTION_H

#include "data.h"

//#define TYPE(t) (*(uint32_t *)(t))
//#define SETTYPE(var) var = TYPE(#var)
#define TYPE(c1,c2,c3,c4) \
	(((uint32_t)c1 << 000) | \
	 ((uint32_t)c2 << 010) | \
	 ((uint32_t)c3 << 020) | \
	 ((uint32_t)c4 << 030))

#define ERR_UNKNOWN (-1)
#define ERR_INVALID (-2)

int process_packet(data packet);

/* A function process_packet(data) is needed
 * exemple of use:
 *
 * void process_packet(data packet) {
 *     type t = read_type(&packet);
 *     if (!t) return -1;
 *     switch (TYPE(t)) {
 *         case TYPE("HELO"): ...
 *         case TYPE("BCST"): ...
 *         case TYPE("PRVT"): ...
 *         default: return -1;
 *     }
 * }
 */

#include "packet.h"

int read_packet(int fd, bool bl);

#endif
