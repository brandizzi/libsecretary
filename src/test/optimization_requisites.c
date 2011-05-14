#include <secretary/test/optimization_requisites.h>
#include <secretary/secretary.h>

void test_optimization_requisites_task_points_secretary(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task creation");
    
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 0, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));

    CuAssertIntEquals(test, 1, task_get_number(task));
    CuAssertStrEquals(test, "Test task creation", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));
    CuAssertPtrEquals(test, secretary, task->secretary);
    
    secretary_free(secretary);
}

CuSuite *test_optimization_requisites_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_optimization_requisites_task_points_secretary);
    return suite;
}
