#include "arraylist.h"

struct arlist {
    void **elements;
    size_t capacity, size;
};

arlist *
arlist_create(void)
{
    arlist *l = malloc(sizeof(arlist));
    l->elements = malloc(sizeof(void*) * MIN_ARLIST);
    l->capacity = MIN_ARLIST;
    l->size = 0;
    return l;
}

void
arlist_destroy(arlist *l, void (*freefn)(void *))
{
    if (freefn)
        for (size_t i = 0; i < l->size; ++i)
            freefn(l->elements[i]);
    free(l->elements);
}

size_t
arlist_size(arlist *l)
{
    return l->size;
}

void *
arlist_set(arlist *l, size_t n, void *e)
{
    if (n >= l->size) {
        arlist_push(l, e);
        return NULL;
    } else {
        void *f = l->elements[n];
        l->elements[n] = e;
        return f;
    }
}

void *
arlist_get(arlist *l, size_t n)
{
    return n >= l->size ? NULL : l->elements[n];
}


void *
arlist_head(arlist *l)
{
    return l->size > 0 ? l->elements[0] : NULL;
}

void *
arlist_tail(arlist *l)
{
    return l->size > 0 ? l->elements[l->size - 1] : NULL;
}

bool
arlist_push(arlist *l, void *e)
{
    if (l->size == SIZE_MAX)
        return false;
    if (l->size == l->capacity) {
        size_t new_capacity = l->capacity >= SIZE_MAX / 2 ? SIZE_MAX : 2 * l->capacity;
        new_capacity *= sizeof(void *);
        void *elements = realloc(l->elements, new_capacity);
        if (elements)
            l->elements = elements;
        else
            return false;
        l->capacity = new_capacity;
    }
    l->elements[(l->size)++] = e;
    return true;
}

void *
arlist_pop(arlist *l)
{
    if (!l->size)
        return NULL;
    void *e = l->elements[--(l->size)];
    if (3 * l->size == l->capacity && l->capacity > MIN_ARLIST) {
        l->capacity /= 2;
        void *elements = realloc(l->elements, l->capacity);
        if (elements)
            l->elements = elements;
    }
    return e;
}
