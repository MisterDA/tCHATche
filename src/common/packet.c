#include "packet.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


uint32_t
read_num(data *d)
{
	if (!d) goto err0;
	char *mem = d->ata;
	if (!shift_data(d, SIZEOF_NUM)) goto err0;
	uint32_t n = 0;
	for (int i=SIZEOF_NUM; i>0; i--) {
		if (*mem < '0' || *mem > '9') goto err0;
		n *= 10;
		n += *mem++ - '0';
	}
	return n;
	err0: return MAX_NUM+1;
}

uint32_t
read_longnum(data *d)
{
	if (!d) goto err0;
	char *mem = d->ata;
	if (!shift_data(d, SIZEOF_LONGNUM)) goto err0;
	uint32_t n = 0;
	for (int i=SIZEOF_LONGNUM; i>0; i--) {
		if (*mem < '0' || *mem > '9') goto err0;
		n *= 10;
		n += *mem++ - '0';
	}
	return n;
	err0: return MAX_NUM+1;
}

data * /* aquire a pointer to the original data */
read_mem(data *d, data *mem)
{
	uint32_t l = read_num(d);
	if (l>MAX_NUM) goto err0;
	static data md;
	if (!mem) mem = &md;
	*mem = *d;
	if (!shift_data(d, l)) goto err0;
	mem->length = l;
	return mem;
	err0: return NULL;
}

data * /* copy data into dest (malloc if NULL) */
read_data(data *d, data *dest)
{
	uint32_t l = read_num(d);
	if (l>MAX_NUM) goto err0;
	if (dest) {
		if (!datancpy(dest, d, l)) goto err1;
		dest->length = l;
	} else {
		static data md;
		dest = &md;
		if (!malloc_datancpy(dest, d, l)) goto err1;
	}
	shift_data(d, l);
	return dest;
	err1: shift_data(d, l);
	err0: return NULL;
}

char * /* copy string (malloc if NULL) and return NULL if cannot put a final '\0' */
read_str(data *d, char *str, uint32_t len)
{
	data mem;
	if (!read_mem(d, &mem)) goto err0;
	return datatostr(&mem, str, len);
    err0: return NULL;
}

char * /* aquire a pointer to the original data */
read_type(data *d)
{
	static char *mem;
	mem = d->ata;
	if (!shift_data(d, SIZEOF_TYPE)) goto err0;
	return mem;
	err0: return NULL;
}


char *
write_num(data *d, uint32_t number)
{
	if (!d) goto err0;
	if (number > MAX_NUM) goto err0;
	char *dest;
    if (d) {
    	dest = d->ata;
    	if (!shift_data(d, SIZEOF_NUM)) goto err0;
    } else { /* en fait non, trop dangereux (le code derange pas) */
        dest = malloc(SIZEOF_NUM+1);
        dest[SIZEOF_NUM] = '\0';
   	}
    for (int i = SIZEOF_NUM-1; i >= 0; --i) {
    	dest[i] = '0'+(number%10);
    	number /= 10;
    }
    return dest;
    err0: return NULL;
}

char *
write_longnum(data *d, uint32_t number)
{
	if (!d) goto err0;
	if (number > MAX_LONGNUM) goto err0;
	char *dest;
    if (d) {
    	dest = d->ata;
    	if (!shift_data(d, SIZEOF_LONGNUM)) goto err0;
    } else {
        dest = malloc(SIZEOF_LONGNUM+1);
        dest[SIZEOF_LONGNUM] = '\0';
   	}
    for (int i = SIZEOF_LONGNUM-1; i >= 0; --i) {
    	dest[i] = '0'+(number%10);
    	number /= 10;
    }
    return dest;
    err0: return NULL;
}

char *
write_type(data *d, type t)
{
	if (!d) goto err0;
	char *dest = d->ata;
	if (!shift_data(d, SIZEOF_TYPE)) goto err0;
	for (int i=0; i<SIZEOF_TYPE; i++)
		dest[i] = t[i];
	return dest;
	err0: return NULL;
}

char *
write_mem(data *d, char *mem, size_t len)
{
	if (!mem) goto err0;
	data src = mem2data(NULL, mem, len);
	return write_data(d, &src);
	err0: return NULL;
}

char *
write_data(data *d, data *src) {
	if (!d) goto err0;
	if (!src) goto err0;
	char *dest = write_num(d, src->length);
	if (!dest) goto err0;
	if (!datacpy(d, src)) goto err1;
	if (!shift_data(d, src->length)) goto err0;
	return dest;
	err1: shift_data(d, src->length);
	err0: return NULL;
}

char *
write_str(data *d, char *str)
{
	if (!str) goto err0;
	data src = str2data(NULL, str);
	return write_data(d, &src);
	err0: return NULL;
}

