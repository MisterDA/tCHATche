#ifndef PACKET_H
#define PACKET_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MIN_NUMBER 0
#define MAX_NUMBER 9999
#define MAX_LONG_NUMBER (MAX_NUMBER * 1000 + MAX_NUMBER)

/* tCHATche data types
 * number:      integer encoded in decimal in 4 ASCII bytes
 * long number: integer encoded in decimal in 8 ASCII bytes
 * type:        4 ASCII bytes
 * string:      string length as a number then string content except ending '\0'
 * data:        data length as a number then sequence of bytes
 *
 * Length fields include the length itself (4 bytes) and the length of the data.
 */

/* Assuming the buffer contains enough bytes, extracts and converts the data. */
uint32_t extract_number(const char **);
uint32_t extract_long_number(const char **);

char * extract_string(const char **, char *, uint32_t);

char * long_number2str(char *str, uint32_t number);
char * number2str(char *str, uint32_t number);

#endif
