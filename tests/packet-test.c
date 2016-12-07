#include "packet.h"

void test_macros(void)
{
    CU_ASSERT_EQUAL(MIN_NUM, 0);
    CU_ASSERT_EQUAL(MAX_NUM, 9999);
    CU_ASSERT_EQUAL(MAX_LONGNUM, 99999999);
}

static void
next_number(char *number, int len)
{
    for (int i = len - 1; i >= 0; --i) {
        if (number[i] == '9') {
            number[i] = '0';
        } else {
            ++number[i];
            break;
        }
    }
}

void test_read_number(void)
{
    char str[5] = "0000";
    char *cur;
    for (uint32_t i = 0, n = 0; i <= MAX_NUM; ++i) {
        cur = str;
        n = read_num((const char **)&cur);
        CU_ASSERT_EQUAL(n, i);
        CU_ASSERT_PTR_EQUAL(cur, str + 4);
        next_number(str, 4);
    }

    char *strs[] = { "abcd", "0bcd", "a0cd", "ab0c", "abc0" };
    for (uint32_t i = 0, n = 0; i < array_size(strs); ++i) {
        cur = strs[i];
        n = read_num((const char **)&cur);
        CU_ASSERT_TRUE(n > MAX_NUM);
        CU_ASSERT_PTR_EQUAL(cur, strs[i] + 4);
    }
}

CU_TestInfo test_packet[] = {
    { "test_read_number", test_read_number },
    CU_TEST_INFO_NULL
};
