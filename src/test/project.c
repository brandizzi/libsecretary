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

static void test_project_remove_task(CuTest *test) {
    Project *project = project_new("libsecretary");
    Task *task1 = task_new(1, "Create first task"),
         *task2 = task_new(2, "Create snd task"),
         *task3 = task_new(3, "Create thrid task");

    project_add_task(project, task1);
    project_add_task(project, task2);
    project_add_task(project, task3);

    CuAssertIntEquals(test, project_count_tasks(project, false), 3);
    CuAssertPtrEquals(test, project_get_nth_task(project, 0, false), task1);
    CuAssertPtrEquals(test, project_get_nth_task(project, 1, false), task2);
    CuAssertPtrEquals(test, project_get_nth_task(project, 2, false), task3);

    project_remove_task(project, task2);

    CuAssertIntEquals(test, project_count_tasks(project, false), 2);
    CuAssertPtrEquals(test, project_get_nth_task(project, 0, false), task1);
    CuAssertPtrEquals(test, project_get_nth_task(project, 1, false), task3);

    task_free(task1);
    task_free(task2);
    task_free(task3);
    project_free(project);
}

void test_project_archived(CuTest *test) {
    Project *project = project_new("libsecretary");
    Task *task1 = task_new(1, "Create first task"),
         *task2 = task_new(2, "Create snd task"),
         *task3 = task_new(3, "Create thrid task");

    project_add_task(project, task1);
    project_add_task(project, task2);
    project_add_task(project, task3);

    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_archive(task2);

    CuAssertIntEquals(test, 2, project_count_tasks(project, false));
    Task *task = project_get_nth_task(project, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = project_get_nth_task(project, 1, false);
    CuAssertPtrEquals(test, task, task3);

    CuAssertIntEquals(test, 1,project_count_tasks(project, true));
    task = project_get_nth_task(project, 0, true);
    CuAssertPtrEquals(test, task, task2);
    
    project_free(project);
}

static void test_project_free_tasks(CuTest *test) {
    Project *project = project_new("libsecretary");
    Task *task1 = task_new(1, "Create first task"),
         *task2 = task_new(2, "Create snd task"),
         *task3 = task_new(3, "Create thrid task");

    project_add_task(project, task1);
    project_add_task(project, task2);
    project_add_task(project, task3);

    CuAssertIntEquals(test, project_count_tasks(project, false), 3);
    CuAssertPtrEquals(test, task_get_project(task1), project);
    CuAssertPtrEquals(test, task_get_project(task2), project);
    CuAssertPtrEquals(test, task_get_project(task3), project);

    project_free(project);

    CuAssertTrue(test, !task_has_project(task1));
    CuAssertTrue(test, !task_has_project(task2));
    CuAssertTrue(test, !task_has_project(task3));

    task_free(task1);
    task_free(task2);
    task_free(task3);
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
    SUITE_ADD_TEST(suite, test_project_free_tasks);
    SUITE_ADD_TEST(suite, test_project_free_name);
    SUITE_ADD_TEST(suite, test_project_remove_task);
    SUITE_ADD_TEST(suite, test_project_archived);
    return suite;
}
