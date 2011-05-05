#include <secretary/test/secretary.h>
#include <stdlib.h>

void test_secretary_create(CuTest *test) {
    Secretary *secretary = secretary_new();
    CuAssertIntEquals(test, 0, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 0, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary));

    secretary_free(secretary);
}

void test_secretary_create_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task creation");
    
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 0, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary));

    CuAssertIntEquals(test, 1, task_get_number(task));
    CuAssertStrEquals(test, "Test task creation", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));

    secretary_free(secretary);
}

void test_secretary_get_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task creation");
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
    Project *project = secretary_create_project(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 0, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary));

    CuAssertStrEquals(test, "libsecretary", project_get_name(project));
    CuAssertIntEquals(test, 0, project_count_tasks(project));
    secretary_free(secretary);
}

void test_secretary_get_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Project *project1 = secretary_create_project(secretary, "libsecretary"),
            *project2 = secretary_create_project(secretary, "secretary-cocoa");

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
    Project *project1 = secretary_create_project(secretary, "libsecretary"),
            *project2 = secretary_create_project(secretary, "secretary-cocoa");

    Project *p = secretary_get_nth_project(secretary, 0);
    CuAssertPtrEquals(test, project1, p);
    p = secretary_get_nth_project(secretary, 1);
    CuAssertPtrEquals(test, project2, p);

    // OTOH, if no project with given name is found, return null
    p = secretary_get_nth_project(secretary, 3);
    CuAssertPtrEquals(test, NULL, p);

    secretary_delete_project(secretary, project1);
    secretary_delete_project(secretary, project2);
    p = secretary_get_nth_project(secretary, 1);
    CuAssertPtrEquals(test, NULL, p);

    secretary_free(secretary);
}

void test_secretary_move_to_project_task_to_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task transference");
    Project *project = secretary_create_project(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary));
    CuAssertIntEquals(test, 0, project_count_tasks(project));

    project_add_task(project, task);

    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_projects(secretary));
    // No more in inbox
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary));
    // Now in project
    CuAssertIntEquals(test, 1, project_count_tasks(project));
    CuAssertPtrEquals(test, task, project_get_task(project, 1));
    CuAssertPtrEquals(test, project, task_get_project(task));
    secretary_free(secretary);
}

void test_secretary_move_to_project_task_from_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task transference");
    Project *project = secretary_create_project(secretary, "libsecretary");
    
    project_add_task(project, task);

    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary));
    CuAssertIntEquals(test, 1, project_count_tasks(project));
    CuAssertPtrEquals(test, task, project_get_task(project, 1));

    project_remove_task(project, task);

    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary));
    CuAssertIntEquals(test, 0, project_count_tasks(project));
    CuAssertPtrEquals(test, NULL, project_get_task(project, 1));

    secretary_free(secretary);
}

void test_secretary_move_to_project_task_from_project_to_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task transference");
    Project *destination = secretary_create_project(secretary, "Chocrotary");
    Project *origin = secretary_create_project(secretary, "libsecretary");
    
    project_add_task(origin, task);

    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 2, secretary_count_projects(secretary));
    // No more in inbox
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary));
    // Now in project origin
    CuAssertIntEquals(test, 1, project_count_tasks(origin));
    CuAssertIntEquals(test, 0, project_count_tasks(destination));
    CuAssertPtrEquals(test, task, project_get_nth_task(origin, 0));
    CuAssertPtrEquals(test, origin, task_get_project(task));
    // move
    project_add_task(destination, task);
    // Now in project destination
    CuAssertIntEquals(test, 0, project_count_tasks(origin));
    CuAssertIntEquals(test, 1, project_count_tasks(destination));
    CuAssertPtrEquals(test, task, project_get_nth_task(destination, 0));
    CuAssertPtrEquals(test, destination, task_get_project(task));


    secretary_free(secretary);
}


void test_secretary_remove_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task transference");

    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary));
    CuAssertPtrEquals(test, task, secretary_get_task(secretary, 1));
    CuAssertPtrEquals(test, task, secretary_get_nth_inbox_task(secretary, 0));

    secretary_delete_task(secretary, task);

    CuAssertIntEquals(test, 0, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary));
    CuAssertPtrEquals(test, NULL, secretary_get_task(secretary, 1));
    CuAssertPtrEquals(test, NULL, secretary_get_nth_inbox_task(secretary, 0));
    secretary_free(secretary);
}

void test_secretary_remove_project_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task transference");
    Project *project = secretary_create_project(secretary, "libsecretary");

    project_add_task(project, task);
    
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary));
    CuAssertPtrEquals(test, task, secretary_get_task(secretary, 1));
    CuAssertIntEquals(test, 1, project_count_tasks(project));
    CuAssertPtrEquals(test, task, project_get_task(project, 1));
    CuAssertPtrEquals(test, project, task_get_project(task));

    secretary_delete_task(secretary, task);

    CuAssertIntEquals(test, 0, secretary_count_tasks(secretary));
    CuAssertPtrEquals(test, NULL, secretary_get_task(secretary, 1));
    CuAssertIntEquals(test, 0, project_count_tasks(project));
    CuAssertPtrEquals(test, NULL, project_get_task(project, 1));
    secretary_free(secretary);
}

void test_secretary_remove_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Project *project1 = secretary_create_project(secretary, "libsecretary"),
        *project2 = secretary_create_project(secretary, "Chocrotary"),
        *project3 = secretary_create_project(secretary, "Netuno NG");

    CuAssertIntEquals(test, 3, secretary_count_projects(secretary));
    CuAssertPtrEquals(test, project2, secretary_get_project(secretary, "Chocrotary"));
    CuAssertPtrEquals(test, project1, secretary_get_nth_project(secretary, 0));
    CuAssertPtrEquals(test, project2, secretary_get_nth_project(secretary, 1));
    CuAssertPtrEquals(test, project3, secretary_get_nth_project(secretary, 2));

    secretary_delete_project(secretary, project2);

    CuAssertIntEquals(test, 2, secretary_count_projects(secretary));
    CuAssertPtrEquals(test, NULL, secretary_get_project(secretary, "Chocrotary"));
    CuAssertPtrEquals(test, project1, secretary_get_nth_project(secretary, 0));
    CuAssertPtrEquals(test, project3, secretary_get_nth_project(secretary, 1));

    secretary_free(secretary);
}

void test_secretary_remove_project_with_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task transference");
    Project *project = secretary_create_project(secretary, "libsecretary");

    project_add_task(project, task);
    
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary));
    CuAssertIntEquals(test, 1, project_count_tasks(project));
    CuAssertPtrEquals(test, task, project_get_task(project, 1));
    CuAssertPtrEquals(test, project, task_get_project(task));
    CuAssertIntEquals(test, 1, secretary_count_projects(secretary));

    secretary_delete_project(secretary, project);

    CuAssertIntEquals(test, 0, secretary_count_projects(secretary));
    CuAssertPtrEquals(test, NULL, secretary_get_project(secretary, "libsecretary"));
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary));
    CuAssertPtrEquals(test, NULL, task_get_project(task));

    secretary_free(secretary);
}

void test_secretary_remove_task_with_others(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task"),
         *task3 = secretary_create_task(secretary, "Create thrid task");

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 3, secretary_count_inbox_tasks(secretary));

    Task *f1 = secretary_get_task(secretary, 1),
         *f2 = secretary_get_task(secretary, 2),
         *f3 = secretary_get_task(secretary, 3);
    
    CuAssertPtrEquals(test, task1, f1);
    CuAssertPtrEquals(test, task2, f2);
    CuAssertPtrEquals(test, task3, f3);

    secretary_delete_task(secretary, task2);

    CuAssertIntEquals(test, 2, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 2, secretary_count_inbox_tasks(secretary));

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
    Project *project1 = secretary_create_project(secretary, "Create first project"),
         *project2 = secretary_create_project(secretary, "Create snd project"),
         *project3 = secretary_create_project(secretary, "Create third project");

    CuAssertIntEquals(test, 3, secretary_count_projects(secretary));

    Project *f1 = secretary_get_project(secretary, "Create first project"),
         *f2 = secretary_get_project(secretary, "Create snd project"),
         *f3 = secretary_get_project(secretary, "Create third project");
    
    CuAssertPtrEquals(test, project1, f1);
    CuAssertPtrEquals(test, project2, f2);
    CuAssertPtrEquals(test, project3, f3);

    secretary_delete_project(secretary, project2);

    CuAssertIntEquals(test, 2, secretary_count_projects(secretary));

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
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task");
    secretary_create_task(secretary, "Create thrid task");
         
    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 3, secretary_count_inbox_tasks(secretary));

    time_t now = time(NULL), future_time;
    future_time = now+60*60*48;
    struct tm future_date = *localtime(&future_time);
    task_schedule(task1, future_date);
    task_schedule(task2, *localtime(&now));

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary));

    CuAssertIntEquals(test, 2, secretary_count_tasks_scheduled_for(secretary, future_date));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled_for_today(secretary));

    Task *task = secretary_get_nth_task_scheduled_for(secretary, future_date, 0);
    CuAssertPtrEquals(test, task1, task);

    CuAssertIntEquals(test, secretary_count_tasks_scheduled_for_today(secretary), 1);    
    task = secretary_get_nth_task_scheduled_for_today(secretary, 0);
    CuAssertPtrEquals(test, task2, task);

    secretary_free(secretary);
}

void test_secretary_late_scheduled_appears_in_today(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task"),
         *task3 = secretary_create_task(secretary, "Create thrid task");
         
    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 3, secretary_count_inbox_tasks(secretary));

    time_t now = time(NULL), future_time, past_time;
    future_time = now + 60*60*48;
    past_time =   now - 60*60*48;
    struct tm future_date = *localtime(&future_time);
    struct tm past_date = *localtime(&past_time);
    task_schedule(task1, future_date);
    task_schedule(task2, *localtime(&now));
    task_schedule(task3, past_date);

    CuAssertIntEquals(test, 3, secretary_count_tasks_scheduled_for(secretary, future_date));
    CuAssertIntEquals(test, 2, secretary_count_tasks_scheduled_for_today(secretary));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled_for(secretary, past_date));

    Task *task = secretary_get_nth_task_scheduled_for(secretary, future_date, 0);
    CuAssertPtrEquals(test, task1, task);
    task = secretary_get_nth_task_scheduled_for_today(secretary, 0);
    CuAssertPtrEquals(test, task2, task);
    task = secretary_get_nth_task_scheduled_for_today(secretary, 1);
    CuAssertPtrEquals(test, task3, task);

    secretary_free(secretary);
}

void test_secretary_unschedule_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task");
    secretary_create_task(secretary, "Create thrid task");
    Project *project = secretary_create_project(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 3, secretary_count_inbox_tasks(secretary));

    time_t now = time(NULL), future_time = now+60*60*48;
    struct tm date = *localtime(&future_time);

    task_schedule(task1, date);

    // One less in inbox
    CuAssertIntEquals(test, 2, secretary_count_inbox_tasks(secretary));
    task_schedule(task2, *localtime(&now));
    // The SCHEDULE state should be preserved!
    project_add_task(project, task2);

    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary));

    CuAssertIntEquals(test, 2, secretary_count_tasks_scheduled(secretary));
    CuAssertIntEquals(test, 2, secretary_count_tasks_scheduled_for(secretary, date));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled_for_today(secretary));

    Task *task = secretary_get_nth_task_scheduled_for(secretary, date, 0);
    CuAssertPtrEquals(test, task1, task);
    task = secretary_get_nth_task_scheduled(secretary, 0);
    CuAssertPtrEquals(test, task1, task);
    
    task = secretary_get_nth_task_scheduled_for_today(secretary, 0);
    CuAssertPtrEquals(test, task2, task);
    task = secretary_get_nth_task_scheduled(secretary, 1);
    CuAssertPtrEquals(test, task2, task);

    task_unschedule(task1);
    // One more in inbox, since it returns
    CuAssertIntEquals(test, 2, secretary_count_inbox_tasks(secretary));    
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled_for(secretary, date));   

    task_unschedule(task2);
    // No one more, since the task2 was in a project
    CuAssertIntEquals(test, 2, secretary_count_inbox_tasks(secretary)); 
    CuAssertIntEquals(test, 0, secretary_count_tasks_scheduled_for_today(secretary));   

    secretary_free(secretary);
}

void test_secretary_mark_task_as_done(CuTest *test) {
    Secretary *secretary = secretary_new();
    Project *project = secretary_create_project(secretary, "libsecretary");
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task"),
         *task3 = secretary_create_task(secretary, "Create thrid task");
    
    struct tm date;
    date.tm_hour = 0;
    date.tm_mday = 30;
    date.tm_mon = 4;
    date.tm_year = 2002-1900;
    task_schedule(task1, date);
    project_add_task(project, task2);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled(secretary));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled_for(secretary, date));
    CuAssertIntEquals(test, 0, secretary_count_done_tasks(secretary));


    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_mark_as_done(task3);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled(secretary));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled_for(secretary, date));
    CuAssertIntEquals(test, 3, secretary_count_done_tasks(secretary));

    // Should be found in their places
    Task *task = secretary_get_nth_inbox_task(secretary, 0);
    CuAssertPtrEquals(test, task3, task);
    // Should be found as scheduled
    task = secretary_get_nth_task_scheduled_for(secretary, date, 0);
    CuAssertPtrEquals(test, task1, task);
    task = secretary_get_nth_task_scheduled(secretary, 0);
    CuAssertPtrEquals(test, task1, task);
    // Should be found in project
    task = project_get_nth_task(project, 0);
    CuAssertPtrEquals(test, task2, task);

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

void test_secretary_unmark_task_as_done(CuTest *test) {
    Secretary *secretary = secretary_new();
    Project *project = secretary_create_project(secretary, "libsecretary");
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task"),
         *task3 = secretary_create_task(secretary, "Create thrid task");
    
    struct tm date;
    date.tm_hour = 0;
    date.tm_mday = 30;
    date.tm_mon = 4;
    date.tm_year = 2002-1900;
    task_schedule(task1, date);
    project_add_task(project, task2);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled(secretary));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled_for(secretary, date));
    CuAssertIntEquals(test, 0, secretary_count_done_tasks(secretary));


    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_mark_as_done(task3);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled(secretary));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled_for(secretary, date));
    CuAssertIntEquals(test, 3, secretary_count_done_tasks(secretary));

    // Should be found in inbox
    Task *task = secretary_get_nth_inbox_task(secretary, 0);
    CuAssertPtrEquals(test, task3, task);
    // Should be found as scheduled
    task = secretary_get_nth_task_scheduled_for(secretary, date, 0);
    CuAssertPtrEquals(test, task1, task);
    task = secretary_get_nth_task_scheduled(secretary, 0);
    CuAssertPtrEquals(test, task1, task);
    // Should be found in project
    task = project_get_nth_task(project, 0);
    CuAssertPtrEquals(test, task2, task);

    // UNDOING
    task_unmark_as_done(task1);
    task_unmark_as_done(task2);
    task_unmark_as_done(task3);

    // found as scheduled
    task = secretary_get_nth_task_scheduled_for(secretary, date, 0);
    CuAssertPtrEquals(test, task1, task);
    task = secretary_get_nth_task_scheduled(secretary, 0);
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

void test_secretary_archived_inbox(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task"),
         *task3 = secretary_create_task(secretary, "Create thrid task");

    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_archive(task1);

    CuAssertIntEquals(test, 2, secretary_count_inbox_tasks(secretary));

    secretary_free(secretary);
}

CuSuite *test_secretary_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_secretary_create);
    SUITE_ADD_TEST(suite, test_secretary_create_task);
    SUITE_ADD_TEST(suite, test_secretary_get_task);
    SUITE_ADD_TEST(suite, test_secretary_create_project);
    SUITE_ADD_TEST(suite, test_secretary_get_project);
    SUITE_ADD_TEST(suite, test_secretary_get_nth_project);
    SUITE_ADD_TEST(suite, test_secretary_move_to_project_task_to_project);
    SUITE_ADD_TEST(suite, test_secretary_move_to_project_task_from_project_to_project);
    SUITE_ADD_TEST(suite, test_secretary_move_to_project_task_from_project);
    SUITE_ADD_TEST(suite, test_secretary_remove_task);
    SUITE_ADD_TEST(suite, test_secretary_remove_project);
    SUITE_ADD_TEST(suite, test_secretary_remove_project_task);
    SUITE_ADD_TEST(suite, test_secretary_remove_task_with_others);
    SUITE_ADD_TEST(suite, test_secretary_remove_project_with_others);
    SUITE_ADD_TEST(suite, test_secretary_schedule);
    SUITE_ADD_TEST(suite, test_secretary_late_scheduled_appears_in_today);
    SUITE_ADD_TEST(suite, test_secretary_unschedule_task);
    SUITE_ADD_TEST(suite, test_secretary_mark_task_as_done);
    SUITE_ADD_TEST(suite, test_secretary_unmark_task_as_done);
    SUITE_ADD_TEST(suite, test_secretary_archived_inbox);
    return suite;
}


