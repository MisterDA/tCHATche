#include "data.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char *
malloc_data(data *d, size_t length)
{
	d->ata = malloc(length * sizeof *d->ata);
	if (d->ata) d->length = length;
	else d->length = 0;
	return d->ata;
}

void
free_data(data *d)
{
	free(d->ata);
	*d = empty_data();
}

data
empty_data(void)
{
	data d;
	malloc_data(&d,0);
	return d;
}


char *
shift_data(data *d, size_t shift)
{
	if (d==NULL) return NULL;
	if (shift>d->length) {
		*d = empty_data();
		return NULL;
	}
	d->ata += shift;
	d->length -= shift;
	return d->ata;
}

char *
datacpy(data *dest, const data *src)
{
	if (!src) return datancpy(dest, src, 0);
	return datancpy(dest, src, src->length);
}

char *
datancpy(data *dest, const data *src, size_t n)
{
	if (!dest) return NULL;
	size_t l = 0;
	if (src) l = src->length;
	if (l>n) l = n;
	size_t m = n<=dest->length?n:dest->length;
	memcpy(dest->ata, src->ata, l<=dest->length?l:dest->length);
	strncpy(dest->ata+l, "", m<=l?0:m-l);
	if (dest->length < n) return NULL;
	return dest->ata;
}

char *
malloc_datacpy(data *d, const data *src)
{
	if (src) return malloc_datancpy(d, src, src->length);
	*d = empty_data();
	return d->ata;
}

char *
malloc_datancpy(data *d, const data *src, size_t length)
{
	malloc_data(d, length);
	char *res = datancpy(d, src, length);
	if (!res) goto err1;
	return res;
	err1: free_data(d);
	return NULL;
}



data
mem2data(data *d, char *mem, size_t length)
{
	data md = {mem,length};
	if (!mem) md.length = 0;
	if (d) *d = md;
	return md;
}

data
str2data(data *d, char *str)
{
	if (!str) return mem2data(d, str, 0);
	return mem2data(d, str, strlen(str));
}

data *
memtodata(data *d,const char *mem,  size_t length)
{
	if (!d)
		if (!(d = malloc(sizeof d))) goto err0;
	data src = mem2data(NULL, (char *)mem, length);
	if (! malloc_datacpy(d, &src) ) goto err1;
	return d;
	err1: free(d);
	err0: return NULL;
}

data *
strtodata(data *d, const char *str)
{
	if (!str) return memtodata(d, str, 0);
	return memtodata(d, str, strlen(str));
}

char *
datatostr(const data *d, char *str, size_t len)
{
	if (!str) {
	    if (!len) len = d->length+1;
	    else if (d->length>=len) goto err0;
	    str = malloc(len*sizeof str);
	} else if (d->length>=len) goto err1;
	strncpy(str, d->ata, d->length);
	strncpy(str+d->length, "", len-d->length);
	return str;
	err1: strncpy(str, d->ata, d->length);
	err0: return NULL;
}



ssize_t
writedata(int fd, const data d)
{
	return write(fd, d.ata, d.length);
}

