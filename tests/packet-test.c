#include "packet.h"

void test_macros(void)
{
    CU_ASSERT_EQUAL(MIN_NUMBER, 0);
    CU_ASSERT_EQUAL(MAX_NUMBER, 9999);
    CU_ASSERT_EQUAL(MAX_LONG_NUMBER, 99999999);
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

void test_extract_number(void)
{
    char str[5] = "0000";
    char *cur;
    for (uint32_t i = 0, n = 0; i <= MAX_NUMBER; ++i) {
        cur = str;
        n = extract_number((const char **)&cur);
        CU_ASSERT_EQUAL(n, i);
        CU_ASSERT_PTR_EQUAL(cur, str + 4);
        next_number(str, 4);
    }

    char *strs[] = { "abcd", "0bcd", "a0cd", "ab0c", "abc0" };
    for (uint32_t i = 0, n = 0; i < array_size(strs); ++i) {
        cur = strs[i];
        n = extract_number((const char **)&cur);
        CU_ASSERT_TRUE(n > MAX_NUMBER);
        CU_ASSERT_PTR_EQUAL(cur, strs[i] + 4);
    }
}

CU_TestInfo test_packet[] = {
    { "test_extract_number", test_extract_number },
    CU_TEST_INFO_NULL
};
