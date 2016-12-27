#include "arraylist.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

arlist *
arlist_create(void)
{
    arlist *l = malloc(sizeof(arlist));
    l->elements = malloc(MIN_ARLIST * sizeof(void *));
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
    free(l);
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
        void *elements = realloc(l->elements, new_capacity * sizeof(void *));
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
    if (3 * l->size <= l->capacity && l->capacity > MIN_ARLIST) {
        l->capacity /= 2;
        void *elements = realloc(l->elements, l->capacity * sizeof(void *));
        if (elements)
            l->elements = elements;
    }
    return l->elements[--(l->size)];
}

bool
arlist_add(arlist *l, int (*cmp)(const void *, const void *), void *e)
{
    if (l->size == SIZE_MAX)
        return false;
    if (l->size == 0 || cmp(l->elements[l->size - 1], e) < 0)
        return arlist_push(l, e);
    /* TODO: optimize with binary search */
    for (size_t i = 0; i < l->size; ++i) {
        int c = cmp(l->elements[i], e);
        if (c == 0) return false;
        if (c > 0) {
            void *tail = arlist_tail(l);
            memmove(l->elements + i + 1, l->elements + i, l->size - i - 1);
            arlist_push(l, tail);
            l->elements[i] = e;
            return true;
        }
    }
    return false;
}

void *
arlist_remove(arlist *l, size_t n)
{
    if (n >= l->size)
        return NULL;
    if (n == l->size - 1)
        return arlist_pop(l);
    void *e = l->elements[n];
    memmove(l->elements + n, l->elements + n + 1, l->size - n);
    --(l->size);
    return e;
}
