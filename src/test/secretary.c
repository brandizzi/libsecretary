#include <test/secretary.h>
#include <stdlib.h>

void test_secretary_create(CuTest *test) {
    Secretary *secretary = secretary_new();
    CuAssertIntEquals(test, 0, secretary_task_count(secretary));
    CuAssertIntEquals(test, 0, secretary_project_count(secretary));
    CuAssertIntEquals(test, 0, secretary_inbox_count(secretary));
    CuAssertIntEquals(test, 0, secretary_area_count(secretary));
}

void test_secretary_create_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_appoint(secretary, "Do something");
    
    CuAssertIntEquals(test, 1, secretary_task_count(secretary));
    CuAssertIntEquals(test, 0, secretary_project_count(secretary));
    CuAssertIntEquals(test, 0, secretary_area_count(secretary));
    CuAssertIntEquals(test, 1, secretary_inbox_count(secretary));

    CuAssertIntEquals(test, 1, task_number(task));
    CuAssertStrEquals(test, "Do something", task_description(task));
}

CuSuite *test_secretary_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_secretary_create);
    SUITE_ADD_TEST(suite, test_secretary_create_task);
    return suite;
}
