/**
 * libsecretary: a C library for managing to-do lists
 * Copyright (C) 2011  Adam Victor Nazareth Brandizzi <brandizzi@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * You can get the latest version of this file at 
 * http://bitbucket.org/brandizzi/libsecretary/
 */
#include <secretary/test/notebook.h>
#include <stdlib.h>
#include <stdio.h>

void test_notebook_create(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    Notebook *notebook = notebook_new("nofile");
    Secretary *secretary = notebook_get_secretary(notebook);

    CuAssertTrue(test, secretary != NULL);
    CuAssertIntEquals(test, 0, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));

    notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_with_task(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    Notebook *notebook = notebook_new("nofile");
    Secretary *secretary = notebook_get_secretary(notebook);
    Task *task = secretary_create_task(secretary, "Test task creation");
    
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));
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
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));
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
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task"),
         *task3 = secretary_create_task(secretary, "Create third task");

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 3, secretary_count_inbox_tasks(secretary, false));

    notebook_save(notebook);
    notebook_free(notebook);
    
    // Retrieving it
    notebook = notebook_new("nofile");
    secretary = notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 3, secretary_count_inbox_tasks(secretary, false));

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
    Project *project = secretary_create_project(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 0, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));

    notebook_save(notebook);
    notebook_free(notebook);

    // Retrieving it
    notebook = notebook_new("nofile");
    secretary = notebook_get_secretary(notebook);
    project = secretary_get_project(secretary, "libsecretary");

    CuAssertTrue(test, project != NULL);
    CuAssertIntEquals(test, 0, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));
    CuAssertStrEquals(test, "libsecretary", project_get_name(project));
    CuAssertIntEquals(test, 0, project_count_tasks(project, false));

    notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_with_project_task(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    Notebook *notebook = notebook_new("nofile");
    Secretary *secretary = notebook_get_secretary(notebook);

    Project *project1 = secretary_create_project(secretary, "libsecretary"),
            *project2 = secretary_create_project(secretary, "chocrotary");

    Task *task1 = secretary_create_task(secretary, "Test notebook"),
         *task2 = secretary_create_task(secretary, "Create Cocoa interface");
    secretary_create_task(secretary, "Buy coffee");

    project_add_task(project1, task1);
    project_add_task(project2, task2);
    
    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));

    notebook_save(notebook);
    notebook_free(notebook);

    // Retrieving it
    notebook = notebook_new("nofile");
    secretary = notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));

    // Who is in inbox?
    Task *task = secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertTrue(test, task != NULL);
    CuAssertStrEquals(test, "Buy coffee", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));
    CuAssertTrue(test, !task_is_scheduled(task));

    // Projects
    Project *project = secretary_get_project(secretary, "libsecretary");
    CuAssertTrue(test, project != NULL);
    CuAssertIntEquals(test, 1, project_count_tasks(project, false));
    task = project_get_nth_task(project, 0, false);
    CuAssertTrue(test, task != NULL);
    CuAssertStrEquals(test, "Test notebook", task_get_description(task));
    CuAssertPtrEquals(test, project, task_get_project(task));
    CuAssertTrue(test, !task_is_scheduled(task));

    project = secretary_get_project(secretary, "chocrotary");
    CuAssertTrue(test, project != NULL);
    CuAssertIntEquals(test, 1, project_count_tasks(project, false));
    task = project_get_nth_task(project, 0, false);
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
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task");
    secretary_create_task(secretary, "Create third task");

    time_t now = time(NULL), future_time = now+60*60*48;
    struct tm date = *localtime(&future_time);
    task_schedule(task1, future_time);
    task_schedule(task2, now);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));

    notebook_save(notebook);
    notebook_free(notebook);

    // Retrieving it
    notebook = notebook_new("nofile");
    secretary = notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 2, secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            secretary_count_tasks_scheduled_for_today(secretary, false));
    CuAssertIntEquals(test, 2, 
            secretary_count_tasks_scheduled_for(secretary, date, false));

    // snd task first because it is early than fst task
    Task *task = secretary_get_nth_task_scheduled_for(secretary, date, 0, false);
    CuAssertStrEquals(test, "Create snd task", task_get_description(task));
    task = secretary_get_nth_task_scheduled_for(secretary, date, 1, false);
    CuAssertStrEquals(test, "Create first task", task_get_description(task));
    // For today
    task = secretary_get_nth_task_scheduled_for_today(secretary, 0, false);
    CuAssertStrEquals(test, "Create snd task", task_get_description(task));
    task = secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertStrEquals(test, "Create third task", task_get_description(task));

    notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_scheduled_tasks_with_projects(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    Notebook *notebook = notebook_new("nofile");
    Secretary *secretary = notebook_get_secretary(notebook);

    Project *project1 = secretary_create_project(secretary, "libsecretary"),
            *project2 = secretary_create_project(secretary, "chocrotary");

    Task *task1 = secretary_create_task(secretary, "Test notebook"),
         *task2 = secretary_create_task(secretary, "Create Cocoa interface"),
         *task3 = secretary_create_task(secretary, "Buy pequi");

    time_t now = time(NULL), future_time = now+60*60*48;
    struct tm date = *localtime(&future_time);
    task_schedule(task2, future_time);
    task_schedule(task3, now);

    project_add_task(project1, task1);
    project_add_task(project2, task2);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));

    notebook_save(notebook);
    notebook_free(notebook);

    // Retrieving it
    notebook = notebook_new("nofile");
    secretary = notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 2, secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            secretary_count_tasks_scheduled_for_today(secretary, false));
    // Includes today
    CuAssertIntEquals(test, 2, 
            secretary_count_tasks_scheduled_for(secretary, date, false));

    Task *task = secretary_get_nth_task_scheduled_for(secretary, date, 0, false);
    // Buy pequi comes first because it is scheduled for earlier
    CuAssertStrEquals(test, "Buy pequi", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));
    task = secretary_get_nth_task_scheduled_for(secretary, date, 1, false);
    CuAssertStrEquals(test, "Create Cocoa interface", task_get_description(task));
    CuAssertTrue(test, task_get_project(task) != NULL);
    CuAssertStrEquals(test, "chocrotary", 
            project_get_name(task_get_project(task)));

    // For today
    task = secretary_get_nth_task_scheduled_for_today(secretary, 0, false);
    CuAssertStrEquals(test, "Buy pequi", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));

    task = secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, NULL, task);

    notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_done_tasks(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    Notebook *notebook = notebook_new("nofile");
    Secretary *secretary = notebook_get_secretary(notebook);

    Project *project1 = secretary_create_project(secretary, "libsecretary"),
            *project2 = secretary_create_project(secretary, "chocrotary");

    Task *task1 = secretary_create_task(secretary, "Test notebook"),
         *task2 = secretary_create_task(secretary, "Create Cocoa interface"),
         *task3 = secretary_create_task(secretary, "Buy pequi");

    time_t now = time(NULL), future_time = now+60*60*48;
    struct tm date = *localtime(&future_time);
    task_schedule(task2, future_time);
    task_schedule(task3, now);

    project_add_task(project1, task1);
    project_add_task(project2, task2);

    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_mark_as_done(task3);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));

    notebook_save(notebook);
    notebook_free(notebook);

    // Retrieving it
    notebook = notebook_new("nofile");
    secretary = notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 2, 
            secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            secretary_count_tasks_scheduled_for_today(secretary,false));
    CuAssertIntEquals(test, 2, 
            secretary_count_tasks_scheduled_for(secretary, date, false));

    Task *task = secretary_get_nth_done_task(secretary, 1, false);
    CuAssertTrue(test, task_is_done(task));
    CuAssertTrue(test, task_get_project(task) != NULL);
    CuAssertStrEquals(test, "chocrotary", 
            project_get_name(task_get_project(task)));
    CuAssertIntEquals(test, future_time, task_get_scheduled_date(task));

    task = secretary_get_nth_done_task(secretary, 2, false);
    CuAssertStrEquals(test, "Buy pequi", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));


    notebook_free(notebook);
    remove("nofile");
}

void test_notebook_update_format(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test notebook");
    task_mark_as_done(task);
    task_archive(task);

    // Opens an nonexistent file.
    remove("nofile");
    FILE *file = fopen("nofile", "w");
    ParserWriterFunction writer = parser_get_writer(1, 1);
    writer(secretary, file);
    fclose(file);
    secretary_free(secretary);

    // Retrieving it
    Notebook *notebook = notebook_new("nofile");
    secretary = notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));

    task = secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertTrue(test, task_is_done(task));
    // v1.1 does not preserver archival
    CuAssertTrue(test, !task_is_archived(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));
    CuAssertStrEquals(test, "Test notebook", task_get_description(task));

    CuAssertIntEquals(test, PARSER_LATEST_MAJOR_VERSION, notebook->major_version);
    CuAssertIntEquals(test, PARSER_LATEST_MINOR_VERSION, notebook->minor_version);

    notebook_free(notebook);
    remove("nofile");
}

void test_notebook_free(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    Notebook *notebook = notebook_new("nofile");

    free(notebook->filename);
    secretary_free(notebook->secretary);
    free(notebook);
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
    SUITE_ADD_TEST(suite, test_notebook_update_format);
    SUITE_ADD_TEST(suite, test_notebook_free);
    return suite;
}


