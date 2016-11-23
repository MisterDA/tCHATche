#include <CUnit/Basic.h>

#define array_size(x)  (sizeof(x) / sizeof((x)[0]))

int main()
{
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    CU_SuiteInfo suites[] = {
        // { "suite", NULL, NULL, NULL, NULL, test_suite },
        CU_SUITE_INFO_NULL,
    };

    if (CUE_SUCCESS != CU_register_suites(suites)) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
