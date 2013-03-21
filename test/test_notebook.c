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
    SctNotebook *notebook = sct_notebook_new("nofile");
    SctSecretary *secretary = sct_notebook_get_secretary(notebook);

    CuAssertTrue(test, secretary != NULL);
    CuAssertIntEquals(test, 0, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));

    sct_notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_with_task(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    SctNotebook *notebook = sct_notebook_new("nofile");
    SctSecretary *secretary = sct_notebook_get_secretary(notebook);
    SctTask *task = sct_secretary_create_task(secretary, "Test task creation");
    
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertStrEquals(test, "Test task creation", sct_task_get_description(task));
    CuAssertPtrEquals(test, NULL, sct_task_get_project(task));

    sct_notebook_save(notebook);
    sct_notebook_free(notebook);

    // Retrieving it
    notebook = sct_notebook_new("nofile");
    secretary = sct_notebook_get_secretary(notebook);
    task = sct_secretary_get_task(secretary, 0);

    CuAssertTrue(test, task != NULL);
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertStrEquals(test, "Test task creation", sct_task_get_description(task));
    CuAssertPtrEquals(test, NULL, sct_task_get_project(task));

    sct_notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_many_tasks(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    SctNotebook *notebook = sct_notebook_new("nofile");
    SctSecretary *secretary = sct_notebook_get_secretary(notebook);
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task"),
         *task3 = sct_secretary_create_task(secretary, "Create third task");

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 3, sct_secretary_count_inbox_tasks(secretary, false));

    sct_notebook_save(notebook);
    sct_notebook_free(notebook);
    
    // Retrieving it
    notebook = sct_notebook_new("nofile");
    secretary = sct_notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 3, sct_secretary_count_inbox_tasks(secretary, false));

    task1 = sct_secretary_get_task(secretary, 0);
    task2 = sct_secretary_get_task(secretary, 1);
    task3 = sct_secretary_get_task(secretary, 2);
    CuAssertStrEquals(test, "Create first task", sct_task_get_description(task1));
    CuAssertStrEquals(test, "Create snd task", sct_task_get_description(task2));
    CuAssertStrEquals(test, "Create third task", sct_task_get_description(task3));

    sct_notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_with_project(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    SctNotebook *notebook = sct_notebook_new("nofile");
    SctSecretary *secretary = sct_notebook_get_secretary(notebook);
    SctProject *project = sct_secretary_create_project(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 0, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));

    sct_notebook_save(notebook);
    sct_notebook_free(notebook);

    // Retrieving it
    notebook = sct_notebook_new("nofile");
    secretary = sct_notebook_get_secretary(notebook);
    project = sct_secretary_get_project(secretary, "libsecretary");

    CuAssertTrue(test, project != NULL);
    CuAssertIntEquals(test, 0, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertStrEquals(test, "libsecretary", sct_project_get_name(project));
    CuAssertIntEquals(test, 0, sct_project_count_tasks(project, false));

    sct_notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_with_project_task(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    SctNotebook *notebook = sct_notebook_new("nofile");
    SctSecretary *secretary = sct_notebook_get_secretary(notebook);

    SctProject *project1 = sct_secretary_create_project(secretary, "libsecretary"),
            *project2 = sct_secretary_create_project(secretary, "chocrotary");

    SctTask *task1 = sct_secretary_create_task(secretary, "Test notebook"),
         *task2 = sct_secretary_create_task(secretary, "Create Cocoa interface");
    sct_secretary_create_task(secretary, "Buy coffee");

    sct_secretary_move_task_to_project(secretary, project1, task1);
    sct_secretary_move_task_to_project(secretary, project2, task2);
    
    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));

    sct_notebook_save(notebook);
    sct_notebook_free(notebook);

    // Retrieving it
    notebook = sct_notebook_new("nofile");
    secretary = sct_notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));

    // Who is in inbox?
    SctTask *task = sct_secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertTrue(test, task != NULL);
    CuAssertStrEquals(test, "Buy coffee", sct_task_get_description(task));
    CuAssertPtrEquals(test, NULL, sct_task_get_project(task));
    CuAssertTrue(test, !sct_task_is_scheduled(task));

    // Projects
    SctProject *project = sct_secretary_get_project(secretary, "libsecretary");
    CuAssertTrue(test, project != NULL);
    CuAssertIntEquals(test, 1, sct_project_count_tasks(project, false));
    task = sct_project_get_nth_task(project, 0, false);
    CuAssertTrue(test, task != NULL);
    CuAssertStrEquals(test, "Test notebook", sct_task_get_description(task));
    CuAssertPtrEquals(test, project, sct_task_get_project(task));
    CuAssertTrue(test, !sct_task_is_scheduled(task));

    project = sct_secretary_get_project(secretary, "chocrotary");
    CuAssertTrue(test, project != NULL);
    CuAssertIntEquals(test, 1, sct_project_count_tasks(project, false));
    task = sct_project_get_nth_task(project, 0, false);
    CuAssertTrue(test, task != NULL);
    CuAssertStrEquals(test, "Create Cocoa interface", sct_task_get_description(task));
    CuAssertPtrEquals(test, project, sct_task_get_project(task));
    CuAssertTrue(test, !sct_task_is_scheduled(task));

    sct_notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_scheduled_tasks(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    SctNotebook *notebook = sct_notebook_new("nofile");
    SctSecretary *secretary = sct_notebook_get_secretary(notebook);
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task");
    sct_secretary_create_task(secretary, "Create third task");

    time_t now = time(NULL), future_time = now+60*60*48;
    sct_secretary_schedule_task(secretary, task1, future_time);
    sct_secretary_schedule_task(secretary, task2, now);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));

    sct_notebook_save(notebook);
    sct_notebook_free(notebook);

    // Retrieving it
    notebook = sct_notebook_new("nofile");
    secretary = sct_notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 2, sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            sct_secretary_count_tasks_scheduled_for_today(secretary, false));
    CuAssertIntEquals(test, 2, 
            sct_secretary_count_tasks_scheduled_for(secretary, future_time, false));

    // snd task first because it is early than fst task
    SctTask *task = sct_secretary_get_nth_task_scheduled_for(secretary, future_time, 0, false);
    CuAssertStrEquals(test, "Create snd task", sct_task_get_description(task));
    task = sct_secretary_get_nth_task_scheduled_for(secretary, future_time, 1, false);
    CuAssertStrEquals(test, "Create first task", sct_task_get_description(task));
    // For today
    task = sct_secretary_get_nth_task_scheduled_for_today(secretary, 0, false);
    CuAssertStrEquals(test, "Create snd task", sct_task_get_description(task));
    task = sct_secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertStrEquals(test, "Create third task", sct_task_get_description(task));

    sct_notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_scheduled_tasks_with_projects(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    SctNotebook *notebook = sct_notebook_new("nofile");
    SctSecretary *secretary = sct_notebook_get_secretary(notebook);

    SctProject *project1 = sct_secretary_create_project(secretary, "libsecretary"),
            *project2 = sct_secretary_create_project(secretary, "chocrotary");

    SctTask *task1 = sct_secretary_create_task(secretary, "Test notebook"),
         *task2 = sct_secretary_create_task(secretary, "Create Cocoa interface"),
         *task3 = sct_secretary_create_task(secretary, "Buy pequi");

    time_t now = time(NULL), future_time = now+60*60*48;
    sct_secretary_schedule_task(secretary, task2, future_time);
    sct_secretary_schedule_task(secretary, task3, now);

    sct_secretary_move_task_to_project(secretary, project1, task1);
    sct_secretary_move_task_to_project(secretary, project2, task2);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));

    sct_notebook_save(notebook);
    sct_notebook_free(notebook);

    // Retrieving it
    notebook = sct_notebook_new("nofile");
    secretary = sct_notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 2, sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            sct_secretary_count_tasks_scheduled_for_today(secretary, false));
    // Includes today
    CuAssertIntEquals(test, 2, 
            sct_secretary_count_tasks_scheduled_for(secretary, future_time, false));

    SctTask *task = sct_secretary_get_nth_task_scheduled_for(secretary, future_time, 0, false);
    // Buy pequi comes first because it is scheduled for earlier
    CuAssertStrEquals(test, "Buy pequi", sct_task_get_description(task));
    CuAssertPtrEquals(test, NULL, sct_task_get_project(task));
    task = sct_secretary_get_nth_task_scheduled_for(secretary, future_time, 1, false);
    CuAssertStrEquals(test, "Create Cocoa interface", sct_task_get_description(task));
    CuAssertTrue(test, sct_task_get_project(task) != NULL);
    CuAssertStrEquals(test, "chocrotary", 
            sct_project_get_name(sct_task_get_project(task)));

    // For today
    task = sct_secretary_get_nth_task_scheduled_for_today(secretary, 0, false);
    CuAssertStrEquals(test, "Buy pequi", sct_task_get_description(task));
    CuAssertPtrEquals(test, NULL, sct_task_get_project(task));

    task = sct_secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, NULL, task);

    sct_notebook_free(notebook);
    remove("nofile");
}

void test_notebook_save_done_tasks(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    SctNotebook *notebook = sct_notebook_new("nofile");
    SctSecretary *secretary = sct_notebook_get_secretary(notebook);

    SctProject *project1 = sct_secretary_create_project(secretary, "libsecretary"),
            *project2 = sct_secretary_create_project(secretary, "chocrotary");

    SctTask *task1 = sct_secretary_create_task(secretary, "Test notebook"),
         *task2 = sct_secretary_create_task(secretary, "Create Cocoa interface"),
         *task3 = sct_secretary_create_task(secretary, "Buy pequi");

    time_t now = time(NULL), future_time = now+60*60*48;
    sct_secretary_schedule_task(secretary, task2, future_time);
    sct_secretary_schedule_task(secretary, task3, now);

    sct_secretary_move_task_to_project(secretary, project1, task1);
    sct_secretary_move_task_to_project(secretary, project2, task2);

    sct_task_mark_as_done(task1);
    sct_task_mark_as_done(task2);
    sct_task_mark_as_done(task3);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));

    sct_notebook_save(notebook);
    sct_notebook_free(notebook);

    // Retrieving it
    notebook = sct_notebook_new("nofile");
    secretary = sct_notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 2, 
            sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            sct_secretary_count_tasks_scheduled_for_today(secretary,false));
    CuAssertIntEquals(test, 2, 
            sct_secretary_count_tasks_scheduled_for(secretary, future_time, false));

    SctTask *task = sct_secretary_get_nth_done_task(secretary, 0, false);
    CuAssertTrue(test, sct_task_is_done(task));
    CuAssertStrEquals(test, "Buy pequi", sct_task_get_description(task));
    CuAssertPtrEquals(test, sct_task_get_project(task), NULL);
    CuAssertIntEquals(test, sct_util_beginning_of_day(now), sct_task_get_scheduled_date(task));

    task = sct_secretary_get_nth_done_task(secretary, 1, false);
    CuAssertStrEquals(test, "Create Cocoa interface", 
            sct_task_get_description(task));
    CuAssertTrue(test, sct_task_get_project(task) != NULL);
    CuAssertStrEquals(test, "chocrotary", 
            sct_project_get_name(sct_task_get_project(task)));
    CuAssertIntEquals(test, sct_util_beginning_of_day(future_time),
            sct_task_get_scheduled_date(task));

    task = sct_secretary_get_nth_done_task(secretary, 2, false);
    CuAssertStrEquals(test, "Test notebook", sct_task_get_description(task));
    CuAssertTrue(test, sct_task_get_project(task) != NULL);
    CuAssertStrEquals(test, "libsecretary", 
            sct_project_get_name(sct_task_get_project(task)));
    CuAssertTrue(test, ! sct_task_is_scheduled(task));

    sct_notebook_free(notebook);
    remove("nofile");
}

void test_notebook_update_format(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test notebook");
    sct_task_mark_as_done(task);
    sct_task_archive(task);

    // Opens an nonexistent file.
    remove("nofile");
    FILE *file = fopen("nofile", "w");
    SctParserWriterFunction writer = parser_get_writer(1, 1);
    writer(secretary, file);
    fclose(file);
    sct_secretary_free(secretary);

    // Retrieving it
    SctNotebook *notebook = sct_notebook_new("nofile");
    secretary = sct_notebook_get_secretary(notebook);

    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));

    task = sct_secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertTrue(test, sct_task_is_done(task));
    // v1.1 does not preserver archival
    CuAssertTrue(test, !sct_task_is_archived(task));
    CuAssertPtrEquals(test, NULL, sct_task_get_project(task));
    CuAssertStrEquals(test, "Test notebook", sct_task_get_description(task));

    CuAssertIntEquals(test, SCT_PARSER_LATEST_MAJOR_VERSION, notebook->major_version);
    CuAssertIntEquals(test, SCT_PARSER_LATEST_MINOR_VERSION, notebook->minor_version);

    sct_notebook_free(notebook);
    remove("nofile");
}

void test_notebook_free(CuTest *test) {
    // Opens an nonexistent file.
    remove("nofile");
    SctNotebook *notebook = sct_notebook_new("nofile");

    free(notebook->filename);
    sct_secretary_free(notebook->secretary);
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


