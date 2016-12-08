#ifndef PACKET_H
#define PACKET_H

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    char *ata;
    size_t length;
} data;
char * malloc_data(data *d, size_t length);
void empty_data(data *d);
ssize_t write_data(int fd, const data d);

#define SIZEOF_NUM 4
#define SIZEOF_LONGNUM 8
#define SIZEOF_TYPE 4
#define SIZEOF_STR(len) (SIZEOF_NUM+(len))       // length-defined
#define SIZEOF_STRING(s) (SIZEOF_NUM+strlen(s))  // null-terminated
#define SIZEOF_DATA(d) (SIZEOF_NUM+(d).length)
#define SIZEOF_REQUEST(len) (SIZEOF_NUM+SIZEOF_TYPE+(len))

#define MIN_NUM 0
#define MAX_NUM 9999
#define MIN_LONGNUM 0
#define MAX_LONGNUM 99999999

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
uint32_t read_num(const char **data);
uint32_t read_longnum(const char **data);
char *   read_str(const char **data, char *str, uint32_t len);
char *   read_string(const char **data, char *str, uint32_t len);

char * write_num(char **data, uint32_t number);
char * write_longnum(char **data, uint32_t number);
char * write_type(char **data, type t);
char * write_str(char **data, char *str, size_t len);
char * write_string(char **data, char *str);

#endif
