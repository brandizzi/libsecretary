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
    Task *task = secretary_appoint(secretary, "Test task creation");
    
    CuAssertIntEquals(test, 1, secretary_task_count(secretary));
    CuAssertIntEquals(test, 0, secretary_project_count(secretary));
    CuAssertIntEquals(test, 0, secretary_area_count(secretary));
    CuAssertIntEquals(test, 1, secretary_inbox_count(secretary));

    CuAssertIntEquals(test, 1, task_number(task));
    CuAssertStrEquals(test, "Test task creation", task_description(task));

}

void test_secretary_get_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_appoint(secretary, "Test task creation");
    CuAssertIntEquals(test, 1, task_number(task));

    Task *task2 = secretary_get_task(secretary, 1);
    CuAssertPtrEquals(test, task, task2);

    // Should be stored in inbox
    Task *task3 = secretary_get_inbox_task(secretary, 1);
    CuAssertPtrEquals(test, task, task3);

    // OTOH, should return NULL if no task with given number
    task2 = secretary_get_task(secretary, 32);
    CuAssertPtrEquals(test, NULL, task2);
    task2 = secretary_get_inbox_task(secretary, 32);
    CuAssertPtrEquals(test, NULL, task2);
}

void test_secretary_create_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Project *project = secretary_start(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 0, secretary_task_count(secretary));
    CuAssertIntEquals(test, 1, secretary_project_count(secretary));
    CuAssertIntEquals(test, 0, secretary_area_count(secretary));
    CuAssertIntEquals(test, 0, secretary_inbox_count(secretary));

    CuAssertStrEquals(test, "libsecretary", project_name(project));
}

void test_secretary_get_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Project *project = secretary_start(secretary, "libsecretary");
    CuAssertStrEquals(test, "libsecretary", project_name(project));

    Project *project2 = secretary_get_project(secretary, "libsecretary");
    CuAssertPtrEquals(test, project, project2);

    // OTOH, if no project with given name is found, return null
    project2 = secretary_get_project(secretary, "libiexistent!!");
    CuAssertPtrEquals(test, NULL, project2);
}


CuSuite *test_secretary_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_secretary_create);
    SUITE_ADD_TEST(suite, test_secretary_create_task);
    SUITE_ADD_TEST(suite, test_secretary_get_task);
    SUITE_ADD_TEST(suite, test_secretary_create_project);
    SUITE_ADD_TEST(suite, test_secretary_get_project);
    return suite;
}
