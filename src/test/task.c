#include <secretary/test/task.h>
#include <secretary/project.h>
#include <stdlib.h>

static void test_task_create(CuTest *test) {
    Task *task = task_new(1, "Do something" );
    CuAssertStrEquals(test, "Do something", task_get_description(task));

    task_free(task);
}

static void test_task_alter_description(CuTest *test) {
    Task *task = task_new(1, "Do something" );
    CuAssertStrEquals(test, "Do something", task_get_description(task));
    task_set_description(task, "Do something else");
    CuAssertStrEquals(test, "Do something else", task_get_description(task));
    
    task_free(task);
}

static void test_task_remove_from_project(CuTest *test) {
    Task *task = task_new(1, "Do something" );
    Project *project = project_new("libsecretary");
    CuAssertPtrEquals(test, task_get_project(task), NULL);

    task_set_project(task, project);
    CuAssertPtrEquals(test, task_get_project(task), project);
    task_unset_project(task);
    CuAssertPtrEquals(test, task_get_project(task), NULL);

    task_free(task);
    project_free(project);
}

static void test_task_is_in_project(CuTest *test) {
    Task *task = task_new(1, "Do something" );
    Project *project = project_new("libsecretary");
    CuAssertTrue(test, !task_is_in_project(task, project));

    task_set_project(task, project);
    CuAssertTrue(test, task_is_in_project(task, project));

    task_free(task);
    project_free(project);
}

static void test_task_switch_done_status(CuTest *test) {
    Task *task = task_new(1, "Do something" );
    CuAssertTrue(test, !task_is_done(task));
    task_mark_as_done(task);
    CuAssertTrue(test, task_is_done(task));
    task_unmark_as_done(task);
    CuAssertTrue(test, !task_is_done(task));
    task_switch_done_status(task);
    CuAssertTrue(test, task_is_done(task));
    task_switch_done_status(task);
    CuAssertTrue(test, !task_is_done(task));

    task_free(task);
}

static void test_task_is_in_inbox(CuTest *test) {
    Task *task = task_new(1, "Do something" );
    CuAssertTrue(test, task_is_in_inbox(task, false));

    task_mark_as_done(task);
    CuAssertTrue(test, task_is_in_inbox(task, false));

    time_t t = time(NULL);
    task_schedule(task, *localtime(&t));
    CuAssertTrue(test, !task_is_in_inbox(task, false));
    task_unschedule(task);
    CuAssertTrue(test, task_is_in_inbox(task, false));

    Project *project = project_new("libsecretary");
    project_add_task(project, task);
    CuAssertTrue(test, !task_is_in_inbox(task, false));
    project_remove_task(project, task);
    CuAssertTrue(test, task_is_in_inbox(task, false));

    task_archive(task);
    CuAssertTrue(test, !task_is_in_inbox(task, false));

    project_free(project);
    task_free(task);
}

static void test_task_archived_is_in_inbox(CuTest *test) {
    Task *task = task_new(1, "Do something" );
    CuAssertTrue(test, task_is_in_inbox(task, false));

    CuAssertTrue(test, !task_is_in_inbox(task, true));
    task_mark_as_done(task);
    CuAssertTrue(test, !task_is_in_inbox(task, true));
    task_archive(task);
    CuAssertTrue(test, task_is_in_inbox(task, true));

    time_t t = time(NULL);
    task_schedule(task, *localtime(&t));
    CuAssertTrue(test, !task_is_in_inbox(task, true));
    task_unschedule(task);
    CuAssertTrue(test, task_is_in_inbox(task, true));

    Project *project = project_new("libsecretary");
    project_add_task(project, task);
    CuAssertTrue(test, !task_is_in_inbox(task, true));
    project_remove_task(project, task);
    CuAssertTrue(test, task_is_in_inbox(task, true));

    project_free(project);
    task_free(task);
}

static void test_task_archive(CuTest *test) {
    Task *task = task_new(1, "Do something" );
    CuAssertTrue(test, !task_is_archived(task));
    task_archive(task); // does nothing, task is not done
    CuAssertTrue(test, !task_is_archived(task));
    task_mark_as_done(task);
    task_archive(task); // Now it archives
    CuAssertTrue(test, task_is_archived(task));

    task_free(task);
}

CuSuite *test_task_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_task_create);
    SUITE_ADD_TEST(suite, test_task_alter_description);
    SUITE_ADD_TEST(suite, test_task_remove_from_project);
    SUITE_ADD_TEST(suite, test_task_is_in_project);
    SUITE_ADD_TEST(suite, test_task_switch_done_status);
    SUITE_ADD_TEST(suite, test_task_archive);
    SUITE_ADD_TEST(suite, test_task_is_in_inbox);
    SUITE_ADD_TEST(suite, test_task_archived_is_in_inbox);
    return suite;
}
