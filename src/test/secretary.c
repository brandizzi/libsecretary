#include <secretary/test/secretary.h>
#include <stdlib.h>

void test_secretary_create(CuTest *test) {
    Secretary *secretary = secretary_new();
    CuAssertIntEquals(test, 0, secretary_count_task(secretary));
    CuAssertIntEquals(test, 0, secretary_count_project(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 0, secretary_count_area(secretary));

    secretary_free(secretary);
}

void test_secretary_create_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_appoint(secretary, "Test task creation");
    
    CuAssertIntEquals(test, 1, secretary_count_task(secretary));
    CuAssertIntEquals(test, 0, secretary_count_project(secretary));
    CuAssertIntEquals(test, 0, secretary_count_area(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox(secretary));

    CuAssertIntEquals(test, 1, task_get_number(task));
    CuAssertStrEquals(test, "Test task creation", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));

    secretary_free(secretary);
}

void test_secretary_get_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_appoint(secretary, "Test task creation");
    CuAssertIntEquals(test, 1, task_get_number(task));

    Task *task2 = secretary_get_task(secretary, 1);
    CuAssertPtrEquals(test, task, task2);

    // Get first task
    Task *task3 = secretary_get_nth_inbox_task(secretary, 0);
    CuAssertPtrEquals(test, task, task3);

    // OTOH, should return NULL if no task with given number
    task2 = secretary_get_task(secretary, 32);
    CuAssertPtrEquals(test, NULL, task2);
    task2 = secretary_get_nth_inbox_task(secretary, 2);
    CuAssertPtrEquals(test, NULL, task2);
    secretary_free(secretary);
}

void test_secretary_create_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Project *project = secretary_start(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 0, secretary_count_task(secretary));
    CuAssertIntEquals(test, 1, secretary_count_project(secretary));
    CuAssertIntEquals(test, 0, secretary_count_area(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));

    CuAssertStrEquals(test, "libsecretary", project_get_name(project));
    CuAssertIntEquals(test, 0, project_count_task(project));
    secretary_free(secretary);
}

void test_secretary_get_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Project *project1 = secretary_start(secretary, "libsecretary"),
            *project2 = secretary_start(secretary, "secretary-cocoa");

    Project *p = secretary_get_project(secretary, "libsecretary");
    CuAssertPtrEquals(test, project1, p);
    p = secretary_get_project(secretary, "secretary-cocoa");
    CuAssertPtrEquals(test, project2, p);

    // OTOH, if no project with given name is found, return null
    p = secretary_get_project(secretary, "libiexistent!!");
    CuAssertPtrEquals(test, NULL, p);
    secretary_free(secretary);
}

void test_secretary_get_nth_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Project *project1 = secretary_start(secretary, "libsecretary"),
            *project2 = secretary_start(secretary, "secretary-cocoa");

    Project *p = secretary_get_nth_project(secretary, 0);
    CuAssertPtrEquals(test, project1, p);
    p = secretary_get_nth_project(secretary, 1);
    CuAssertPtrEquals(test, project2, p);

    // OTOH, if no project with given name is found, return null
    p = secretary_get_nth_project(secretary, 3);
    CuAssertPtrEquals(test, NULL, p);
    secretary_free(secretary);
}

void test_secretary_move_task_to_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_appoint(secretary, "Test task transference");
    Project *project = secretary_start(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 1, secretary_count_task(secretary));
    CuAssertIntEquals(test, 1, secretary_count_project(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 0, project_count_task(project));

    secretary_move(secretary, task, project);

    CuAssertIntEquals(test, 1, secretary_count_task(secretary));
    CuAssertIntEquals(test, 1, secretary_count_project(secretary));
    // No more in inbox
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));
    // Now in project
    CuAssertIntEquals(test, 1, project_count_task(project));
    CuAssertPtrEquals(test, task, project_get_task(project, 1));
    CuAssertPtrEquals(test, project, task_get_project(task));
    secretary_free(secretary);
}

void test_secretary_move_task_from_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_appoint(secretary, "Test task transference");
    Project *project = secretary_start(secretary, "libsecretary");
    
    secretary_move(secretary, task, project);

    CuAssertIntEquals(test, 1, secretary_count_task(secretary));
    CuAssertIntEquals(test, 1, secretary_count_project(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 1, project_count_task(project));
    CuAssertPtrEquals(test, task, project_get_task(project, 1));

    secretary_move_to_inbox(secretary, task);

    CuAssertIntEquals(test, 1, secretary_count_task(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 0, project_count_task(project));
    CuAssertPtrEquals(test, NULL, project_get_task(project, 1));

    secretary_free(secretary);
}

void test_secretary_remove_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_appoint(secretary, "Test task transference");

    CuAssertIntEquals(test, 1, secretary_count_task(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox(secretary));
    CuAssertPtrEquals(test, task, secretary_get_task(secretary, 1));
    CuAssertPtrEquals(test, task, secretary_get_nth_inbox_task(secretary, 0));

    secretary_delete_task(secretary, task);

    CuAssertIntEquals(test, 0, secretary_count_task(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));
    CuAssertPtrEquals(test, NULL, secretary_get_task(secretary, 1));
    CuAssertPtrEquals(test, NULL, secretary_get_nth_inbox_task(secretary, 0));
    secretary_free(secretary);
}

void test_secretary_remove_project_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_appoint(secretary, "Test task transference");
    Project *project = secretary_start(secretary, "libsecretary");

    secretary_move(secretary, task, project);
    
    CuAssertIntEquals(test, 1, secretary_count_task(secretary));
    CuAssertPtrEquals(test, task, secretary_get_task(secretary, 1));
    CuAssertIntEquals(test, 1, project_count_task(project));
    CuAssertPtrEquals(test, task, project_get_task(project, 1));
    CuAssertPtrEquals(test, project, task_get_project(task));

    secretary_delete_task(secretary, task);

    CuAssertIntEquals(test, 0, secretary_count_task(secretary));
    CuAssertPtrEquals(test, NULL, secretary_get_task(secretary, 1));
    CuAssertIntEquals(test, 0, project_count_task(project));
    CuAssertPtrEquals(test, NULL, project_get_task(project, 1));
    secretary_free(secretary);
}

void test_secretary_remove_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Project *project = secretary_start(secretary, "libsecretary");

    CuAssertIntEquals(test, 1, secretary_count_project(secretary));
    CuAssertPtrEquals(test, project, secretary_get_project(secretary, "libsecretary"));

    secretary_delete_project(secretary, project);

    CuAssertIntEquals(test, 0, secretary_count_project(secretary));
    CuAssertPtrEquals(test, NULL, secretary_get_project(secretary, "libsecretary"));

    secretary_free(secretary);
}

void test_secretary_remove_project_with_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_appoint(secretary, "Test task transference");
    Project *project = secretary_start(secretary, "libsecretary");

    secretary_move(secretary, task, project);
    
    CuAssertIntEquals(test, 1, secretary_count_task(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 1, project_count_task(project));
    CuAssertPtrEquals(test, task, project_get_task(project, 1));
    CuAssertPtrEquals(test, project, task_get_project(task));
    CuAssertIntEquals(test, 1, secretary_count_project(secretary));

    secretary_delete_project(secretary, project);

    CuAssertIntEquals(test, 0, secretary_count_project(secretary));
    CuAssertPtrEquals(test, NULL, secretary_get_project(secretary, "libsecretary"));
    CuAssertIntEquals(test, 1, secretary_count_task(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox(secretary));
    CuAssertPtrEquals(test, NULL, task_get_project(task));

    secretary_free(secretary);
}

void test_secretary_remove_task_with_others(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_appoint(secretary, "Create first task"),
         *task2 = secretary_appoint(secretary, "Create snd task"),
         *task3 = secretary_appoint(secretary, "Create thrid task");

    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 3, secretary_count_inbox(secretary));

    Task *f1 = secretary_get_task(secretary, 1),
         *f2 = secretary_get_task(secretary, 2),
         *f3 = secretary_get_task(secretary, 3);
    
    CuAssertPtrEquals(test, task1, f1);
    CuAssertPtrEquals(test, task2, f2);
    CuAssertPtrEquals(test, task3, f3);

    secretary_delete_task(secretary, task2);

    CuAssertIntEquals(test, 2, secretary_count_task(secretary));
    CuAssertIntEquals(test, 2, secretary_count_inbox(secretary));

    f1 = secretary_get_task(secretary, 1);
    f2 = secretary_get_task(secretary, 2);
    f3 = secretary_get_task(secretary, 3);

    CuAssertPtrEquals(test, task1, f1);
    CuAssertPtrEquals(test, NULL, f2);
    CuAssertPtrEquals(test, task3, f3);
    secretary_free(secretary);
}

void test_secretary_remove_project_with_others(CuTest *test) {
    Secretary *secretary = secretary_new();
    Project *project1 = secretary_start(secretary, "Create first project"),
         *project2 = secretary_start(secretary, "Create snd project"),
         *project3 = secretary_start(secretary, "Create third project");

    CuAssertIntEquals(test, 3, secretary_count_project(secretary));

    Project *f1 = secretary_get_project(secretary, "Create first project"),
         *f2 = secretary_get_project(secretary, "Create snd project"),
         *f3 = secretary_get_project(secretary, "Create third project");
    
    CuAssertPtrEquals(test, project1, f1);
    CuAssertPtrEquals(test, project2, f2);
    CuAssertPtrEquals(test, project3, f3);

    secretary_delete_project(secretary, project2);

    CuAssertIntEquals(test, 2, secretary_count_project(secretary));

    f1 = secretary_get_project(secretary, "Create first project"),
    f2 = secretary_get_project(secretary, "Create snd project"),
    f3 = secretary_get_project(secretary, "Create third project");

    CuAssertPtrEquals(test, project1, f1);
    CuAssertPtrEquals(test, NULL, f2);
    CuAssertPtrEquals(test, project3, f3);
    secretary_free(secretary);
}

void test_secretary_schedule(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_appoint(secretary, "Create first task"),
         *task2 = secretary_appoint(secretary, "Create snd task");
    secretary_appoint(secretary, "Create thrid task");
         
    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 3, secretary_count_inbox(secretary));

    struct tm date;
    date.tm_hour = 0;
    date.tm_mday = 30;
    date.tm_mon = 4;
    date.tm_year = 2002-1900;
    secretary_schedule(secretary, task1, date);

    time_t now = time(NULL);
    secretary_schedule(secretary, task2, *localtime(&now));

    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox(secretary));

    CuAssertIntEquals(test, 1, secretary_count_scheduled_for(secretary, date));
    CuAssertIntEquals(test, 1, secretary_count_scheduled_for_today(secretary));

    CuAssertIntEquals(test, secretary_count_scheduled_for(secretary, date), 1);
    Task *task = secretary_get_nth_scheduled_for(secretary, date, 0);
    CuAssertPtrEquals(test, task1, task);

    CuAssertIntEquals(test, secretary_count_scheduled_for_today(secretary), 1);    
    task = secretary_get_nth_scheduled_for_today(secretary, 0);
    CuAssertPtrEquals(test, task2, task);

    secretary_free(secretary);
}

void test_secretary_unschedule(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_appoint(secretary, "Create first task"),
         *task2 = secretary_appoint(secretary, "Create snd task");
    secretary_appoint(secretary, "Create thrid task");
    Project *project = secretary_start(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 3, secretary_count_inbox(secretary));

    struct tm date;
    date.tm_hour = 0;
    date.tm_mday = 30;
    date.tm_mon = 4;
    date.tm_year = 2002-1900;
    secretary_schedule(secretary, task1, date);

    // One less in inbox
    CuAssertIntEquals(test, 2, secretary_count_inbox(secretary));

    time_t now = time(NULL);
    secretary_schedule(secretary, task2, *localtime(&now));
    // The SCHEDULE state should be preserved!
    secretary_move(secretary, task2, project);

    CuAssertIntEquals(test, 1, secretary_count_inbox(secretary));

    CuAssertIntEquals(test, 2, secretary_count_scheduled(secretary));
    CuAssertIntEquals(test, 1, secretary_count_scheduled_for(secretary, date));
    CuAssertIntEquals(test, 1, secretary_count_scheduled_for_today(secretary));

    Task *task = secretary_get_nth_scheduled_for(secretary, date, 0);
    CuAssertPtrEquals(test, task1, task);
    task = secretary_get_nth_scheduled(secretary, 0);
    CuAssertPtrEquals(test, task1, task);
    
    task = secretary_get_nth_scheduled_for_today(secretary, 0);
    CuAssertPtrEquals(test, task2, task);
    task = secretary_get_nth_scheduled(secretary, 1);
    CuAssertPtrEquals(test, task2, task);

    secretary_unschedule(secretary, task1);
    // One more in inbox, since it returns
    CuAssertIntEquals(test, 2, secretary_count_inbox(secretary));    
    CuAssertIntEquals(test, 0, secretary_count_scheduled_for(secretary, date));   

    secretary_unschedule(secretary, task2);
    // No one more, since the task2 was in a project
    CuAssertIntEquals(test, 2, secretary_count_inbox(secretary)); 
    CuAssertIntEquals(test, 0, secretary_count_scheduled_for_today(secretary));   

    secretary_free(secretary);
}

void test_secretary_do(CuTest *test) {
    Secretary *secretary = secretary_new();
    Project *project = secretary_start(secretary, "libsecretary");
    Task *task1 = secretary_appoint(secretary, "Create first task"),
         *task2 = secretary_appoint(secretary, "Create snd task"),
         *task3 = secretary_appoint(secretary, "Create thrid task");
    
    struct tm date;
    date.tm_hour = 0;
    date.tm_mday = 30;
    date.tm_mon = 4;
    date.tm_year = 2002-1900;
    secretary_schedule(secretary, task1, date);
    secretary_move(secretary, task2, project);

    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 1, secretary_count_scheduled(secretary));
    CuAssertIntEquals(test, 1, secretary_count_scheduled_for(secretary, date));
    CuAssertIntEquals(test, 0, secretary_count_done_tasks(secretary));


    secretary_do(secretary, task1);
    secretary_do(secretary, task2);
    secretary_do(secretary, task3);

    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 0, secretary_count_scheduled(secretary));
    CuAssertIntEquals(test, 0, secretary_count_scheduled_for(secretary, date));
    CuAssertIntEquals(test, 3, secretary_count_done_tasks(secretary));

    // Not found in inbox
    Task *task = secretary_get_nth_inbox_task(secretary, 0);
    CuAssertPtrEquals(test, NULL, task);
    // Not found as scheduled
    task = secretary_get_nth_scheduled_for(secretary, date, 0);
    CuAssertPtrEquals(test, NULL, task);
    task = secretary_get_nth_scheduled(secretary, 0);
    CuAssertPtrEquals(test, NULL, task);
    // Not found in project
    task = project_get_nth_task(project, 0);
    CuAssertPtrEquals(test, NULL, task);

    // Getting the things done :)
    task = secretary_get_nth_done_task(secretary, 0);
    CuAssertPtrEquals(test, task1, task);
    task = secretary_get_nth_done_task(secretary, 1);
    CuAssertPtrEquals(test, task2, task);
    task = secretary_get_nth_done_task(secretary, 2);
    CuAssertPtrEquals(test, task3, task);
    task = secretary_get_nth_done_task(secretary, 3);
    CuAssertPtrEquals(test, NULL, task);
    secretary_free(secretary);
}

void test_secretary_undo(CuTest *test) {
    Secretary *secretary = secretary_new();
    Project *project = secretary_start(secretary, "libsecretary");
    Task *task1 = secretary_appoint(secretary, "Create first task"),
         *task2 = secretary_appoint(secretary, "Create snd task"),
         *task3 = secretary_appoint(secretary, "Create thrid task");
    
    struct tm date;
    date.tm_hour = 0;
    date.tm_mday = 30;
    date.tm_mon = 4;
    date.tm_year = 2002-1900;
    secretary_schedule(secretary, task1, date);
    secretary_move(secretary, task2, project);

    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 1, secretary_count_scheduled(secretary));
    CuAssertIntEquals(test, 1, secretary_count_scheduled_for(secretary, date));
    CuAssertIntEquals(test, 0, secretary_count_done_tasks(secretary));


    secretary_do(secretary, task1);
    secretary_do(secretary, task2);
    secretary_do(secretary, task3);

    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 0, secretary_count_scheduled(secretary));
    CuAssertIntEquals(test, 0, secretary_count_scheduled_for(secretary, date));
    CuAssertIntEquals(test, 3, secretary_count_done_tasks(secretary));

    // Not found in inbox
    Task *task = secretary_get_nth_inbox_task(secretary, 0);
    CuAssertPtrEquals(test, NULL, task);
    // Not found as scheduled
    task = secretary_get_nth_scheduled_for(secretary, date, 0);
    CuAssertPtrEquals(test, NULL, task);
    task = secretary_get_nth_scheduled(secretary, 0);
    CuAssertPtrEquals(test, NULL, task);
    // Not found in project
    task = project_get_nth_task(project, 0);
    CuAssertPtrEquals(test, NULL, task);

    // UNDOING
    secretary_undo(secretary, task1);
    secretary_undo(secretary, task2);
    secretary_undo(secretary, task3);

    // found as scheduled
    task = secretary_get_nth_scheduled_for(secretary, date, 0);
    CuAssertPtrEquals(test, task1, task);
    task = secretary_get_nth_scheduled(secretary, 0);
    CuAssertPtrEquals(test, task1, task);
    // found in project
    task = project_get_nth_task(project, 0);
    CuAssertPtrEquals(test, task2, task);
    // Found in inbox
    task = secretary_get_nth_inbox_task(secretary, 0);
    CuAssertPtrEquals(test, task3, task);

    CuAssertIntEquals(test, 0, secretary_count_done_tasks(secretary));
    CuAssertPtrEquals(test, NULL, secretary_get_nth_done_task(secretary, 0));
}

CuSuite *test_secretary_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_secretary_create);
    SUITE_ADD_TEST(suite, test_secretary_create_task);
    SUITE_ADD_TEST(suite, test_secretary_get_task);
    SUITE_ADD_TEST(suite, test_secretary_create_project);
    SUITE_ADD_TEST(suite, test_secretary_get_project);
    SUITE_ADD_TEST(suite, test_secretary_get_nth_project);
    SUITE_ADD_TEST(suite, test_secretary_move_task_to_project);
    SUITE_ADD_TEST(suite, test_secretary_remove_task);
    SUITE_ADD_TEST(suite, test_secretary_remove_project);
    SUITE_ADD_TEST(suite, test_secretary_remove_project_task);
    SUITE_ADD_TEST(suite, test_secretary_remove_task_with_others);
    SUITE_ADD_TEST(suite, test_secretary_remove_project_with_others);
    SUITE_ADD_TEST(suite, test_secretary_schedule);
    SUITE_ADD_TEST(suite, test_secretary_unschedule);
    SUITE_ADD_TEST(suite, test_secretary_do);
    SUITE_ADD_TEST(suite, test_secretary_undo);
    return suite;
}


