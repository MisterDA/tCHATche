#include "packet_reception.h"

#include <unistd.h>
#include <stdio.h> //DEV
#include "packet.h" //DEV

static char buffer[MAX_REQUEST_LENGTH];
static uint32_t c = 0;
static uint32_t l;

int
read_packet(int fd, bool nb)
{
	int r;
	data buf;
	if (c<4) {
		while (c<4) {
			r = read(fd, buffer+c, 1);
			if (r<0) goto err0;
			if (r<1) {
				if (nb) goto nb;
				else continue;
			}
			if ('0'<=buffer[c] && buffer[c]<='9')
				c++;
			else
				c = 0;
		}
		buf = (data){buffer, 4};
		l = read_num(&buf);
		if (l<4) goto err0;
	}

	while (c<l) {
		r = read(fd, buffer+c, l-c);
		if (r<0) goto err0;
		c += r;
		if (c<l && nb) goto nb;
	}

	buf = (data){buffer, l};
	process_packet(buf);
	// memset(buffer, 0, MAX_REQUEST_LENGTH); /* for debug */
	c = 0;

	nb: return 0;
	err0: return -1;
}
