#include "arraylist.h"

void test_arlist_create(void)
{
    arlist *l = arlist_create();
    CU_ASSERT_PTR_NOT_NULL(l);
    CU_ASSERT_EQUAL(l->capacity, MIN_ARLIST);
    CU_ASSERT_EQUAL(l->size, 0);
    CU_ASSERT_EQUAL(arlist_size(l), 0);
    arlist_destroy(l, NULL);
}

void test_arlist_functions(void)
{
    arlist *l = arlist_create();
    size_t size = 0;
    for (size_t i = 0; i < MIN_ARLIST * 8; ++i) {
        size_t *e = malloc(sizeof(size_t));
        *e = i + 50;
        CU_ASSERT_TRUE(arlist_push(l, e));
        size_t *tail = (size_t *)arlist_tail(l);
        CU_ASSERT_PTR_EQUAL(tail, e);
        CU_ASSERT_EQUAL(*tail, *e);
        ++size;
        CU_ASSERT_EQUAL(arlist_size(l), size);
    }
    for (size_t i = MIN_ARLIST * 8; i-- > 0; ) {
        size_t *tail = (size_t *)arlist_pop(l);
        CU_ASSERT_EQUAL(*tail, i + 50);
        size--;
        CU_ASSERT_EQUAL(arlist_size(l), size);
        free(tail);
    }
    arlist_destroy(l, free);
}

CU_TestInfo test_arlist[] = {
    { "test_arlist_create", test_arlist_create },
    { "test_arlist_functions", test_arlist_functions },
    CU_TEST_INFO_NULL
};
