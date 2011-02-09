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
}


CuSuite *test_notebook_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_notebook_create);
    return suite;
}


