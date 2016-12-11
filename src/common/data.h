#ifndef DATA_H
#define DATA_H

#include <sys/types.h>

typedef struct {
	char *ata;
	size_t length;
} data;

#define NODATA (data){NULL,0}

char * malloc_data(data *d, size_t length);
void free_data(data *d);
data empty_data(void);

char * shift_data(data *d, size_t shift);
char * datacpy(data *dest, const data *src);
char * datancpy(data *dest, const data *src, size_t n);
char * malloc_datacpy(data *d, const data *src);
char * malloc_datancpy(data *d, const data *src, size_t n);

data mem2data(data *d, char *mem, size_t length);
data str2data(data *d, char *str);
data * memtodata(data *d, const char *mem, size_t length);
data * strtodata(data *d, const char *str);
char * datatostr(const data *d, char *str, size_t len);

ssize_t writedata(int fd, const data d);

#endif
