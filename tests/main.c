#include <CUnit/Basic.h>

#include "tchatche.h"

#include "packet-test.c"
#include "arraylist-test.c"

int main()
{
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    CU_SuiteInfo suites[] = {
        { "packet.h", NULL, NULL, NULL, NULL, test_packet },
        { "arlist.h", NULL, NULL, NULL, NULL, test_arlist },
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
