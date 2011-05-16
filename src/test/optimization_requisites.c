#include <secretary/test/optimization_requisites.h>
#include <secretary/_internal/secretary.h>

/**
 * Ensures that each task knows its secretary.
 */
static void test_optimization_requisites_task_points_secretary(CuTest *test) {
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

/**
 * Ensures that a secretary has some lists to store inbox and scheduled tasks.
 * It will ease the process of efficiently retrieve some of the tasks in inbox
 * and scheduled ones.
 */
static void test_optimization_requisites_secretary_perspectives(CuTest *test) {
    Secretary *secretary = secretary_new();

    CuAssertPtrNotNull(test, secretary->inbox_perspective.visible_tasks);
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrNotNull(test, secretary->inbox_perspective.archived_tasks);
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.archived_tasks));
    CuAssertPtrNotNull(test, secretary->scheduled_perspective.visible_tasks);
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));
    CuAssertPtrNotNull(test, secretary->scheduled_perspective.archived_tasks);
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.archived_tasks));

    secretary_free(secretary);
}

/**
 * Ensures that a secretary has some lists to store inbox and scheduled tasks.
 * It will ease the process of efficiently retrieve some of the tasks in inbox
 * and scheduled ones.
 */
static void test_optimization_requisites_inbox_perspective(CuTest *test) {
    Secretary *secretary = secretary_new();

    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.archived_tasks));

    Task *task1 = secretary_create_task(secretary, "task 1"),
        *task2 = secretary_create_task(secretary, "task 2");
    // All in inbox
    CuAssertIntEquals(test, 2, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrEquals(test, task1, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 0));
    CuAssertPtrEquals(test, task2, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 1));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.archived_tasks));
    // Scheduled task not in inbox anymore
    time_t now = time(NULL);
    task_schedule(task1, *localtime(&now));
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrEquals(test, task2, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 0));
    // Project-related task not in inbox anymore
    Project *project = secretary_create_project(secretary, "project");
    project_add_task(project, task2);
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    // Removed from project - returns to inbox
    project_remove_task(project, task2);
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrEquals(test, task2, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 0));


    task_unschedule(task1);
    CuAssertIntEquals(test, 2, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrEquals(test, task2, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 0));
    CuAssertPtrEquals(test, task1, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 1));

    secretary_free(secretary);
}

/**
 * Ensures that a project-related scheduled secretary does not go to inbox
 * perspective if it is either unscheduled or removed from project, but not
 * both.
 */
static void test_optimization_requisites_do_not_go_to_inbox(CuTest *test) {
    Secretary *secretary = secretary_new();

    Task *task = secretary_create_task(secretary, "task 1");
    // All in inbox
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrEquals(test, task, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 0));

    // Schedule task...
    time_t now = time(NULL);
    task_schedule(task, *localtime(&now));
    // ...and put it in project
    Project *project = secretary_create_project(secretary, "project");
    project_add_task(project, task);

    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));

    // Removed from project - does not return to inbox, since is scheduled
    project_remove_task(project, task);
    CuAssertIntEquals(test,0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));

    // Not it is unscheduled - returns to inbox
    task_unschedule(task);
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrEquals(test, task, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 0));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));

    // Reschedule and put in project again
    task_schedule(task, *localtime(&now));
    project_add_task(project, task);

    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));

    // Unschedule it again: should not return to inbox, since is in project
    task_unschedule(task);

    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));

    // Remove from project - returns to inbox
    project_remove_task(project, task);
    CuAssertIntEquals(test,1, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));

    secretary_free(secretary);
}


static void test_optimization_requisites_register_in_inbox(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = task_new(0, "task");
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));

    _secretary_register_in_inbox(secretary, task);

    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrEquals(test, task, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 0));

    _secretary_unregister_from_inbox(secretary, task);
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));

    time_t now;
    task->scheduled_for = *localtime(&now);
    task->scheduled = true;

    // Should not register to inbox because is scheduled
    _secretary_register_in_inbox(secretary, task);
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));

    task->scheduled = false;
    // Should not be registered in inbox - has project
    Project *project = project_new("project");
    project_add_task(project, task);

    _secretary_register_in_inbox(secretary, task);
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));


    secretary_free(secretary);
}

static void test_optimization_requisites_register_archived_in_inbox(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = task_new(0, "task");
    task_mark_as_done(task);
    task_archive(task);

    _secretary_register_in_inbox(secretary, task);

    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->inbox_perspective.archived_tasks));

    CuAssertPtrEquals(test, task, 
            list_get_nth_item(secretary->inbox_perspective.archived_tasks, 0));

    _secretary_unregister_from_inbox(secretary, task);
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.archived_tasks));
            

    time_t now;
    task->scheduled_for = *localtime(&now);
    task->scheduled = true;

    // Should not register to inbox because is scheduled
    _secretary_register_in_inbox(secretary, task);
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.archived_tasks));

    task->scheduled = false;
    // Should not be registered in inbox - has project
    Project *project = project_new("project");
    project_add_task(project, task);

    _secretary_register_in_inbox(secretary, task);
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.archived_tasks));


    secretary_free(secretary);
}

static void test_optimization_requisites_register_in_scheduled(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "task");
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrEquals(test, task, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 0));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));

    time_t now;
    task->scheduled_for = *localtime(&now);
    task->scheduled = true;

    // Should  register in scheduled
    _secretary_register_in_scheduled(secretary, task);
    
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));
    CuAssertPtrEquals(test, task, 
            list_get_nth_item(secretary->scheduled_perspective.visible_tasks, 0));

    _secretary_unregister_from_scheduled(secretary, task);

    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));

    task->scheduled = false;
    // Should not register - task is not scheduled
    _secretary_register_in_scheduled(secretary, task);
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));

    secretary_free(secretary);
}

static void test_optimization_requisites_register_archived_in_scheduled(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = task_new(0, "task");
    time_t now;
    task->scheduled_for = *localtime(&now);
    task->scheduled = true;

    task->done = task->archived = true;

    // Should  register in scheduled
    _secretary_register_in_scheduled(secretary, task);

    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->scheduled_perspective.archived_tasks));
    CuAssertPtrEquals(test, task, 
            list_get_nth_item(secretary->scheduled_perspective.archived_tasks, 0));

    _secretary_unregister_from_scheduled(secretary, task);

    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.archived_tasks));

    task->scheduled = false;
    // Should not register - task is not scheduled
    _secretary_register_in_scheduled(secretary, task);
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.archived_tasks));

    secretary_free(secretary);
}

CuSuite *test_optimization_requisites_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_optimization_requisites_task_points_secretary);
    SUITE_ADD_TEST(suite, test_optimization_requisites_secretary_perspectives);
    SUITE_ADD_TEST(suite, test_optimization_requisites_inbox_perspective);
    SUITE_ADD_TEST(suite, test_optimization_requisites_register_in_inbox);
    SUITE_ADD_TEST(suite, test_optimization_requisites_register_archived_in_inbox);
    SUITE_ADD_TEST(suite, test_optimization_requisites_do_not_go_to_inbox);
    SUITE_ADD_TEST(suite, test_optimization_requisites_register_in_scheduled);
    SUITE_ADD_TEST(suite, test_optimization_requisites_register_archived_in_scheduled);
    return suite;
}
