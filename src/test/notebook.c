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
    notebook_free(notebook);

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

void test_notebook_save_many_tasks(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    Notebook *notebook = notebook_new("nofile");
    Secretary *secretary = notebook_get_secretary(notebook);
    Task *task1 = secretary_appoint(secretary, "Create first task"),
         *task2 = secretary_appoint(secretary, "Create snd task"),
         *task3 = secretary_appoint(secretary, "Create third task");

    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 3, secretary_count_inbox(secretary));

    notebook_save(notebook);
    notebook_free(notebook);
    
    // Retrieving it
    notebook = notebook_new("nofile");
    secretary = notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 3, secretary_count_inbox(secretary));

    task1 = secretary_get_task(secretary, 1);
    task2 = secretary_get_task(secretary, 2);
    task3 = secretary_get_task(secretary, 3);
    CuAssertStrEquals(test, "Create first task", task_get_description(task1));
    CuAssertStrEquals(test, "Create snd task", task_get_description(task2));
    CuAssertStrEquals(test, "Create third task", task_get_description(task3));

    notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_with_project(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    Notebook *notebook = notebook_new("nofile");
    Secretary *secretary = notebook_get_secretary(notebook);
    Project *project = secretary_start(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 0, secretary_count_task(secretary));
    CuAssertIntEquals(test, 1, secretary_count_project(secretary));
    CuAssertIntEquals(test, 0, secretary_count_area(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));

    notebook_save(notebook);
    notebook_free(notebook);

    // Retrieving it
    notebook = notebook_new("nofile");
    secretary = notebook_get_secretary(notebook);
    project = secretary_get_project(secretary, "libsecretary");

    CuAssertTrue(test, project != NULL);
    CuAssertIntEquals(test, 0, secretary_count_task(secretary));
    CuAssertIntEquals(test, 1, secretary_count_project(secretary));
    CuAssertIntEquals(test, 0, secretary_count_area(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));
    CuAssertStrEquals(test, "libsecretary", project_get_name(project));
    CuAssertIntEquals(test, 0, project_count_task(project));

    notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_with_project_task(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    Notebook *notebook = notebook_new("nofile");
    Secretary *secretary = notebook_get_secretary(notebook);

    Project *project1 = secretary_start(secretary, "libsecretary"),
            *project2 = secretary_start(secretary, "chocrotary");

    Task *task1 = secretary_appoint(secretary, "Test notebook"),
         *task2 = secretary_appoint(secretary, "Create Cocoa interface");
    secretary_appoint(secretary, "Buy coffee");

    secretary_move(secretary, task1, project1);
    secretary_move(secretary, task2, project2);
    
    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 2, secretary_count_project(secretary));
    CuAssertIntEquals(test, 0, secretary_count_area(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox(secretary));

    notebook_save(notebook);
    notebook_free(notebook);

    // Retrieving it
    notebook = notebook_new("nofile");
    secretary = notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 2, secretary_count_project(secretary));
    CuAssertIntEquals(test, 0, secretary_count_area(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox(secretary));

    // Who is in inbox?
    Task *task = secretary_get_nth_inbox_task(secretary, 0);
    CuAssertTrue(test, task != NULL);
    CuAssertStrEquals(test, "Buy coffee", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));
    CuAssertTrue(test, !task_is_scheduled(task));

    // Projects
    Project *project = secretary_get_project(secretary, "libsecretary");
    CuAssertTrue(test, project != NULL);
    CuAssertIntEquals(test, 1, project_count_task(project));
    task = project_get_nth_task(project, 0);
    CuAssertTrue(test, task != NULL);
    CuAssertStrEquals(test, "Test notebook", task_get_description(task));
    CuAssertPtrEquals(test, project, task_get_project(task));
    CuAssertTrue(test, !task_is_scheduled(task));

    project = secretary_get_project(secretary, "chocrotary");
    CuAssertTrue(test, project != NULL);
    CuAssertIntEquals(test, 1, project_count_task(project));
    task = project_get_nth_task(project, 0);
    CuAssertTrue(test, task != NULL);
    CuAssertStrEquals(test, "Create Cocoa interface", task_get_description(task));
    CuAssertPtrEquals(test, project, task_get_project(task));
    CuAssertTrue(test, !task_is_scheduled(task));

    notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_scheduled_tasks(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    Notebook *notebook = notebook_new("nofile");
    Secretary *secretary = notebook_get_secretary(notebook);
    Task *task1 = secretary_appoint(secretary, "Create first task"),
         *task2 = secretary_appoint(secretary, "Create snd task");
    secretary_appoint(secretary, "Create third task");

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

    notebook_save(notebook);
    notebook_free(notebook);

    // Retrieving it
    notebook = notebook_new("nofile");
    secretary = notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 2, secretary_count_scheduled(secretary));
    CuAssertIntEquals(test, 1, secretary_count_scheduled_for_today(secretary));
    CuAssertIntEquals(test, 1, secretary_count_scheduled_for(secretary, date));

    Task *task = secretary_get_nth_scheduled_for(secretary, date, 0);
    CuAssertStrEquals(test, "Create first task", task_get_description(task));
    task = secretary_get_nth_scheduled_for_today(secretary, 0);
    CuAssertStrEquals(test, "Create snd task", task_get_description(task));
    task = secretary_get_nth_inbox_task(secretary, 0);
    CuAssertStrEquals(test, "Create third task", task_get_description(task));

    notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_scheduled_tasks_with_projects(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    Notebook *notebook = notebook_new("nofile");
    Secretary *secretary = notebook_get_secretary(notebook);

    Project *project1 = secretary_start(secretary, "libsecretary"),
            *project2 = secretary_start(secretary, "chocrotary");

    Task *task1 = secretary_appoint(secretary, "Test notebook"),
         *task2 = secretary_appoint(secretary, "Create Cocoa interface"),
         *task3 = secretary_appoint(secretary, "Buy pequi");

    struct tm date;
    date.tm_hour = 0;
    date.tm_mday = 30;
    date.tm_mon = 4;
    date.tm_year = 2002-1900;
    secretary_schedule(secretary, task2, date);

    time_t now = time(NULL);
    secretary_schedule(secretary, task3, *localtime(&now));

    secretary_move(secretary, task1, project1);
    secretary_move(secretary, task2, project2);

    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 2, secretary_count_scheduled(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));

    notebook_save(notebook);
    notebook_free(notebook);

    // Retrieving it
    notebook = notebook_new("nofile");
    secretary = notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 2, secretary_count_scheduled(secretary));
    CuAssertIntEquals(test, 1, secretary_count_scheduled_for_today(secretary));
    CuAssertIntEquals(test, 1, secretary_count_scheduled_for(secretary, date));

    Task *task = secretary_get_nth_scheduled_for(secretary, date, 0);
    CuAssertStrEquals(test, "Create Cocoa interface", task_get_description(task));
    CuAssertTrue(test, task_get_project(task) != NULL);
    CuAssertStrEquals(test, "chocrotary", 
            project_get_name(task_get_project(task)));
    task = secretary_get_nth_scheduled_for_today(secretary, 0);
    CuAssertStrEquals(test, "Buy pequi", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));

    task = secretary_get_nth_inbox_task(secretary, 0);
    CuAssertPtrEquals(test, NULL, task);

    notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_done_tasks(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    Notebook *notebook = notebook_new("nofile");
    Secretary *secretary = notebook_get_secretary(notebook);

    Project *project1 = secretary_start(secretary, "libsecretary"),
            *project2 = secretary_start(secretary, "chocrotary");

    Task *task1 = secretary_appoint(secretary, "Test notebook"),
         *task2 = secretary_appoint(secretary, "Create Cocoa interface"),
         *task3 = secretary_appoint(secretary, "Buy pequi");

    struct tm date;
    date.tm_hour = 0;
    date.tm_mday = 30;
    date.tm_mon = 4;
    date.tm_year = 2002-1900;
    secretary_schedule(secretary, task2, date);

    time_t now = time(NULL);
    secretary_schedule(secretary, task3, *localtime(&now));

    secretary_move(secretary, task1, project1);
    secretary_move(secretary, task2, project2);

    secretary_do(secretary, task1);
    secretary_do(secretary, task2);
    secretary_do(secretary, task3);

    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 0, secretary_count_scheduled(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));

    notebook_save(notebook);
    notebook_free(notebook);

    // Retrieving it
    notebook = notebook_new("nofile");
    secretary = notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, secretary_count_task(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox(secretary));
    CuAssertIntEquals(test, 0, secretary_count_scheduled(secretary));
    CuAssertIntEquals(test, 0, secretary_count_scheduled_for_today(secretary));
    CuAssertIntEquals(test, 0, secretary_count_scheduled_for(secretary, date));

    Task *task = secretary_get_nth_done_task(secretary, 1);
    CuAssertTrue(test, task_is_done(task));
    CuAssertTrue(test, task_get_project(task) != NULL);
    CuAssertStrEquals(test, "chocrotary", 
            project_get_name(task_get_project(task)));
    CuAssertIntEquals(task, date.tm_mday, task_get_scheduled_date(task).tm_mday);
    CuAssertIntEquals(task, date.tm_mday, task_get_scheduled_date(task).tm_mday);

    task = secretary_get_nth_done_task(secretary, 2);
    CuAssertStrEquals(test, "Buy pequi", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));


    notebook_free(notebook);
    remove("nofile");
}

CuSuite *test_notebook_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_notebook_create);
    SUITE_ADD_TEST(suite, test_notebook_save_with_task);
    SUITE_ADD_TEST(suite, test_notebook_save_many_tasks);
    SUITE_ADD_TEST(suite, test_notebook_save_with_project);
    SUITE_ADD_TEST(suite, test_notebook_save_with_project_task);
    SUITE_ADD_TEST(suite, test_notebook_save_scheduled_tasks);
    SUITE_ADD_TEST(suite, test_notebook_save_scheduled_tasks_with_projects);
    SUITE_ADD_TEST(suite, test_notebook_save_done_tasks);
    return suite;
}


