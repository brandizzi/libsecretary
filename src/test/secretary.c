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
    CuAssertPtrEquals(test, NULL, task_project(task));

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
    CuAssertIntEquals(test, 0, project_task_count(project));
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

void test_secretary_move_task_to_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_appoint(secretary, "Test task transference");
    Project *project = secretary_start(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 1, secretary_task_count(secretary));
    CuAssertIntEquals(test, 1, secretary_project_count(secretary));
    CuAssertIntEquals(test, 1, secretary_inbox_count(secretary));
    CuAssertIntEquals(test, 0, project_task_count(project));

    secretary_move(secretary, task, project);

    CuAssertIntEquals(test, 1, secretary_task_count(secretary));
    CuAssertIntEquals(test, 1, secretary_project_count(secretary));
    // No more in inbox
    CuAssertIntEquals(test, 0, secretary_inbox_count(secretary));
    // Now in project
    CuAssertIntEquals(test, 1, project_task_count(project));
    CuAssertPtrEquals(test, task, project_get_task(project, 1));
    CuAssertPtrEquals(test, project, task_project(task));
}

void test_secretary_move_task_from_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_appoint(secretary, "Test task transference");
    Project *project = secretary_start(secretary, "libsecretary");
    
    secretary_move(secretary, task, project);

    CuAssertIntEquals(test, 1, secretary_task_count(secretary));
    CuAssertIntEquals(test, 1, secretary_project_count(secretary));
    CuAssertIntEquals(test, 0, secretary_inbox_count(secretary));
    CuAssertIntEquals(test, 1, project_task_count(project));
    CuAssertPtrEquals(test, task, project_get_task(project, 1));

    secretary_move_to_inbox(secretary, task);

    CuAssertIntEquals(test, 1, secretary_task_count(secretary));
    CuAssertIntEquals(test, 1, secretary_inbox_count(secretary));
    CuAssertIntEquals(test, 0, project_task_count(project));
    CuAssertPtrEquals(test, NULL, project_get_task(project, 1));

}

void test_secretary_remove_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_appoint(secretary, "Test task transference");

    CuAssertIntEquals(test, 1, secretary_task_count(secretary));
    CuAssertIntEquals(test, 1, secretary_inbox_count(secretary));
    CuAssertPtrEquals(test, task, secretary_get_task(secretary, 1));
    CuAssertPtrEquals(test, task, secretary_get_inbox_task(secretary, 1));

    secretary_delete_task(secretary, task);

    CuAssertIntEquals(test, 0, secretary_task_count(secretary));
    CuAssertIntEquals(test, 0, secretary_inbox_count(secretary));
    CuAssertPtrEquals(test, NULL, secretary_get_task(secretary, 1));
    CuAssertPtrEquals(test, NULL, secretary_get_inbox_task(secretary, 1));
}

void test_secretary_remove_project_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_appoint(secretary, "Test task transference");
    Project *project = secretary_start(secretary, "libsecretary");

    secretary_move(secretary, task, project);
    
    CuAssertIntEquals(test, 1, secretary_task_count(secretary));
    CuAssertPtrEquals(test, task, secretary_get_task(secretary, 1));
    CuAssertIntEquals(test, 1, project_task_count(project));
    CuAssertPtrEquals(test, task, project_get_task(project, 1));
    CuAssertPtrEquals(test, project, task_project(task));

    secretary_delete_task(secretary, task);

    CuAssertIntEquals(test, 0, secretary_task_count(secretary));
    CuAssertPtrEquals(test, NULL, secretary_get_task(secretary, 1));
    CuAssertIntEquals(test, 0, project_task_count(project));
    CuAssertPtrEquals(test, NULL, project_get_task(project, 1));

}


CuSuite *test_secretary_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_secretary_create);
    SUITE_ADD_TEST(suite, test_secretary_create_task);
    SUITE_ADD_TEST(suite, test_secretary_get_task);
    SUITE_ADD_TEST(suite, test_secretary_create_project);
    SUITE_ADD_TEST(suite, test_secretary_get_project);
    SUITE_ADD_TEST(suite, test_secretary_move_task_to_project);
    SUITE_ADD_TEST(suite, test_secretary_remove_task);
    SUITE_ADD_TEST(suite, test_secretary_remove_project_task);
    return suite;
}
