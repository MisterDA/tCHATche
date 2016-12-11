#ifndef PACKET_H
#define PACKET_H

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "data.h"

#define SIZEOF_NUM 4
#define SIZEOF_LONGNUM 8
#define SIZEOF_TYPE 4
#define SIZEOF_STR(len) (SIZEOF_NUM+(len))       // length-defined
#define SIZEOF_STRING(s) (SIZEOF_NUM+strlen(s))  // null-terminated
#define SIZEOF_DATA(d) (SIZEOF_NUM+(d).length)

#define MIN_NUM 0
#define MAX_NUM 9999
#define MIN_LONGNUM 0
#define MAX_LONGNUM 99999999

#define MAX_REQUEST_LENGTH MAX_NUM

#define DBG(h) do{ printf("[" #h "]\n"); fflush(stdout); }while(0)
#define DBGINT(i) do{ printf("[%d]\n", (i)); fflush(stdout); }while(0)

typedef char type[SIZEOF_TYPE];

/* tCHATche data types
 * number:      integer encoded in decimal in 4 ASCII bytes
 * long number: integer encoded in decimal in 8 ASCII bytes
 * type:        4 ASCII bytes
 * string:      string length as a number then string content except ending '\0'
 * data:        data length as a number then sequence of bytes
 *
 * Message length fields include the length itself (4 bytes) and the length of
 * the data.
 */

/* Assuming the buffer contains enough bytes, extracts and converts the data. */
uint32_t read_num     (data *d);
uint32_t read_longnum (data *d);
data *   read_mem     (data *d, data *mem);
data *   read_data    (data *d, data *dest);
char *   read_str     (data *d, char *str, uint32_t len);

char * write_num     (data *d, uint32_t number);
char * write_longnum (data *d, uint32_t number);
char * write_mem     (data *d, char *mem, size_t len);
char * write_data    (data *d, data *src);
char * write_str     (data *d, char *str);
char * write_type    (data *d, type t);

#endif
