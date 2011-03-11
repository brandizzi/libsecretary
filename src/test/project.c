#include <secretary/test/project.h>
#include <stdlib.h>

static void test_project_create(CuTest *test) {
    Project *project = project_new("libsecretary");
    CuAssertStrEquals(test, "libsecretary", project_get_name(project));

    project_free(project);
}

static void test_project_alter_name(CuTest *test) {
    Project *project = project_new("libsecretary");
    CuAssertStrEquals(test, "libsecretary", project_get_name(project));

    project_set_name(project, "chocrotary");
    CuAssertStrEquals(test, "chocrotary", project_get_name(project));
    
    project_free(project);
}

CuSuite *test_project_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_project_create);
    SUITE_ADD_TEST(suite, test_project_alter_name);
    return suite;
}
