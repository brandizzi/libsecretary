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

static void test_project_remove(CuTest *test) {
    Project *project = project_new("libsecretary");
    Task *task1 = task_new(1, "Create first task"),
         *task2 = task_new(2, "Create snd task"),
         *task3 = task_new(3, "Create thrid task");

    project_add(project, task1);
    project_add(project, task2);
    project_add(project, task3);

    CuAssertIntEquals(test, project_count_task(project), 3);
    CuAssertPtrEquals(test, project_get_nth_task(project, 0), task1);
    CuAssertPtrEquals(test, project_get_nth_task(project, 1), task2);
    CuAssertPtrEquals(test, project_get_nth_task(project, 2), task3);

    project_remove(project, task2);

    CuAssertIntEquals(test, project_count_task(project), 2);
    CuAssertPtrEquals(test, project_get_nth_task(project, 0), task1);
    CuAssertPtrEquals(test, project_get_nth_task(project, 1), task3);

    task_free(task1);
    task_free(task2);
    task_free(task3);
    project_free(project);
}

static void test_project_free_name(CuTest *test) {
    Project *project = project_new("libsecretary");
    free(project->name);
    free(project);
}

CuSuite *test_project_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_project_create);
    SUITE_ADD_TEST(suite, test_project_alter_name);
    SUITE_ADD_TEST(suite, test_project_free_name);
    SUITE_ADD_TEST(suite, test_project_remove);
    return suite;
}
