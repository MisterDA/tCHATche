#include "packet.h"

uint32_t
extract_number(const char **data)
{
    uint32_t n = 0;
    for (int i = 4; i > 0; --i) {
        if (**data < '0' || **data > '9') {
            *data += i;
            return MAX_NUMBER + 1;
        }
        n *= 10;
        n += **data - '0';
        ++(*data);
    }
    return n;
}

uint32_t
extract_long_number(const char **data)
{
    uint32_t n = 0;
    for (int i = 8; i > 0; --i) {
        if (**data < '0' || **data > '9') {
            *data += i;
            return MAX_LONG_NUMBER + 1;
        }
        n *= 10;
        n += **data - '0';
        ++(*data);
    }
    return n;
}

char *
extract_string(const char **data, char *str, uint32_t len)
{
    uint32_t l = extract_number(data);
    if (l > MAX_NUMBER)
        return NULL;
    if (!*str) {
        if (!len)
            len = l+1;
        str = malloc(len);
    }
    strncpy(str, *data, len-1);
    str[len-1] = '\0';
    return str;
}

char *
number2str(char *str, uint32_t number)
{
    if (!str) {
        if (number <= MAX_NUMBER)
            str = malloc(4);
        else
            return NULL;
    }
    sprintf(str, "%04d", number);
    return str;
}

char *
long_number2str(char *str, uint32_t number)
{
    if (!str) {
        if (number <= MAX_NUMBER)
            str = malloc(8);
        else
            return NULL;
    }
    sprintf(str, "%08d", number);
    return str;
}
