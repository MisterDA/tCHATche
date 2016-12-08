#include "packet.h"

#include <stdio.h>
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
empty_data(data *d)
{
    free(d->ata);
    d->ata = NULL;
    d->length = 0;
}

ssize_t write_data(int fd, const data d)
{
    return write(fd, d.ata, d.length);
}



uint32_t
read_num(const char **data)
{
    uint32_t n = 0;
    for (int i = SIZEOF_NUM; i > 0; --i) {
        if (**data < '0' || **data > '9') {
            *data += i;
            return MAX_NUM+1;
        }
        n *= 10;
        n += *(*data)++ - '0';
    }
    return n;
}

uint32_t
read_longnum(const char **data)
{
    uint32_t n = 0;
    for (int i = SIZEOF_LONGNUM; i > 0; --i) {
        if (**data < '0' || **data > '9') {
            *data += i;
            return MAX_LONGNUM+1;
        }
        n *= 10;
        n += *(*data)++ - '0';
    }
    return n;
}

char *
read_str(const char **data, char *str, uint32_t len)
{
    uint32_t l = read_num(data);
    if (l > MAX_NUM)
        return NULL;
    if (!str) {
        if (!len)
            len = l;
        else if (l>=len)
            return NULL;
        str = malloc(len);
    } else if (l>=len)
        return NULL;
    strncpy(str, *data, l);;
    return str;
}

char *
read_string(const char **data, char *str, uint32_t len)
{
    uint32_t l = read_num(data);
    if (l > MAX_NUM)
        return NULL;
    if (!str) {
        if (!len)
            len = l+1;
        else if (l>=len)
            return NULL;
        str = malloc(len);
    } else if (l>=len)
        return NULL;
    strncpy(str, *data, l);
    str[l] = '\0';
    return str;
}


char *
write_num(char **data, uint32_t number)
{
    if (number > MAX_NUM)
        return NULL;
    char *str;
    if (!data) {
        str = malloc(SIZEOF_NUM+1);
        str[SIZEOF_NUM] = '\0';
    } else {
        str = *data;
        *data += SIZEOF_NUM;
       }
    for (int i = SIZEOF_NUM-1; i >= 0; --i) {
        str[i] = '0'+(number%10);
        number /= 10;
    }
    return str;
}

char *
write_longnum(char **data, uint32_t number)
{
    if (number > MAX_LONGNUM)
        return NULL;
    char *str;
    if (!data) {
        str = malloc(SIZEOF_LONGNUM+1);
        str[SIZEOF_LONGNUM] = '\0';
    } else {
        str = *data;
        *data += SIZEOF_LONGNUM;
       }
    for (int i = SIZEOF_LONGNUM-1; i >= 0; --i) {
        str[i] = '0'+(number%10);
        number /= 10;
    }
    return str;
}

char *
write_type(char **data, type t)
{
    if (!data) return NULL;
    char *s = *data;
    for (int i=0; i<SIZEOF_TYPE; i++)
        *(*data)++ = t[i];
    return s;
}

char *
write_str(char **data, char *str, size_t len)
{
    if (!data) return NULL;
    char *s = write_num(data, len);
    if (!s) return NULL;
    strncpy(*data, str, len);
    *data += len;
    return s;
}

char *
write_string(char **data, char *str)
{
    return write_str(data, str, strlen(str));
}
