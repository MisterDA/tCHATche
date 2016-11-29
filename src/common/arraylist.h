#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MIN_ARLIST 32

typedef struct arlist arlist;

arlist *arlist_create();
void arlist_destroy(arlist *, void (*freefn)(void *));

size_t arlist_size(arlist *);

void *arlist_set(arlist *, size_t, void *);
void *arlist_get(arlist *, size_t);

void *arlist_head(arlist *);
void *arlist_tail(arlist *);

bool arlist_push(arlist *, void *);
void *arlist_pop(arlist *);


#endif
