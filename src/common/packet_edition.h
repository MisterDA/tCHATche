#ifndef PACKET_EDITION_H
#define PACKET_EDITION_H

/* A constant BUFFER is needed ; must designate a
 *   static char []
 * Array length must be > MAX_LENGTH (to detect overflow).
 * example of use:
 * 
 * data
 * req_server_BCST(char *pseudo, char *msg, size_t msglen)
 * {
 *     INIT(d);
 *     TEST(msglen>0)
 *     DO write_type(&d, "BCST");
 *     DO write_str(&d, pseudo);
 *     DO write_mem(&d, msg, msglen);
 *     END(d);
 * }
 * 
 */

#include "packet.h"

#define INIT(d) \
	data buf_ = mem2data(NULL, BUFFER, sizeof BUFFER / sizeof *BUFFER); \
	data d = buf_; \
	int err_ = 0; \
	DO shift_data(&d, SIZEOF_NUM);

#define DO err_ |= NULL==
#define TEST(cond) err_ |= !(cond)

#define END(d) \
	buf_.length -= d.length; \
	d = buf_; \
	DO write_num(&d, buf_.length); \
	if (err_) return NODATA; \
	return buf_;

#endif
