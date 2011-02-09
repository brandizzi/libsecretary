#include <secretary/test/notebook.h>
#include <stdlib.h>
#include <stdio.h>

void test_notebook_create(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    Notebook *notebook = notebook_new("nofile");
    Secretary *secretary = notebook_get_secretary(notebook);

    CuAssertTrue(test, secretary != NULL);
    CuAssertIntEquals(test, 0, secretary_count_task(secretary));
    CuAssertIntEquals(test, 0, secretary_count_project(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 0, secretary_count_area(secretary));

    notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_with_task(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    Notebook *notebook = notebook_new("nofile");
    Secretary *secretary = notebook_get_secretary(notebook);
    Task *task = secretary_appoint(secretary, "Test task creation");
    
    CuAssertIntEquals(test, 1, secretary_count_task(secretary));
    CuAssertIntEquals(test, 0, secretary_count_project(secretary));
    CuAssertIntEquals(test, 0, secretary_count_area(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 1, task_get_number(task));
    CuAssertStrEquals(test, "Test task creation", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));

    notebook_save(notebook);

    // Retrieving it
    notebook = notebook_new("nofile");
    secretary = notebook_get_secretary(notebook);
    task = secretary_get_task(secretary, 1);

    CuAssertTrue(test, task != NULL);
    CuAssertIntEquals(test, 1, secretary_count_task(secretary));
    CuAssertIntEquals(test, 0, secretary_count_project(secretary));
    CuAssertIntEquals(test, 0, secretary_count_area(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 1, task_get_number(task));
    CuAssertStrEquals(test, "Test task creation", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));

    notebook_free(notebook);
    remove("nofile");
}


CuSuite *test_notebook_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_notebook_create);
    SUITE_ADD_TEST(suite, test_notebook_save_with_task);
    return suite;
}


