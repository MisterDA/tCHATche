#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include <stdbool.h>
#include <sys/types.h>

#define MIN_ARLIST 32

typedef struct arlist arlist;
struct arlist {
    void **elements;
    size_t capacity, size;
};

arlist *arlist_create(void);
void arlist_destroy(arlist *, void (*freefn)(void *));

size_t arlist_size(arlist *);

void *arlist_set(arlist *, size_t, void *);
void *arlist_get(arlist *, size_t);

void *arlist_head(arlist *);
void *arlist_tail(arlist *);

bool arlist_push(arlist *, void *);
void *arlist_pop(arlist *);

#endif
