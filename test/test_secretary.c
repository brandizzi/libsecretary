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
#include <secretary/test/secretary.h>
#include <secretary/_internal/secretary.h>
#include <stdlib.h>

void test_secretary_create(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    CuAssertIntEquals(test, 0, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));

    sct_secretary_free(secretary);
}

void test_secretary_create_task(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test task creation");
    
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));

    CuAssertStrEquals(test, "Test task creation", sct_task_get_description(task));
    CuAssertPtrEquals(test, NULL, sct_task_get_project(task));

    sct_secretary_free(secretary);
}

void test_secretary_get_task(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test task creation");

    SctTask *task2 = sct_secretary_get_task(secretary, 0);
    CuAssertPtrEquals(test, task, task2);

    // Get first task
    SctTask *task3 = sct_secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, task, task3);

    // OTOH, should return NULL if no task with given number
    task2 = sct_secretary_get_task(secretary, 32);
    CuAssertPtrEquals(test, NULL, task2);
    task2 = sct_secretary_get_nth_inbox_task(secretary, 2, false);
    CuAssertPtrEquals(test, NULL, task2);
    sct_secretary_free(secretary);
}

void test_secretary_create_project(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctProject *project = sct_secretary_create_project(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 0, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));

    CuAssertStrEquals(test, "libsecretary", sct_project_get_name(project));
    CuAssertIntEquals(test, 0, sct_project_count_tasks(project, false));
    sct_secretary_free(secretary);
}

void test_secretary_archive_project(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctProject *project1 = sct_secretary_create_project(secretary, "libsecretary"),
            *project2 = sct_secretary_create_project(secretary, "secretary-cocoa");

    
    CuAssertIntEquals(test, 2, sct_secretary_count_projects(secretary));
    CuAssertPtrEquals(test, project1, sct_secretary_get_nth_project(secretary, 0));
    CuAssertPtrEquals(test, project2, sct_secretary_get_nth_project(secretary, 1));

    sct_secretary_archive_project(secretary, project2);
    
    CuAssertIntEquals(test, 1, sct_secretary_count_projects(secretary));
    CuAssertPtrEquals(test, project1, sct_secretary_get_nth_project(secretary, 0));
    CuAssertPtrEquals(test, NULL, sct_secretary_get_nth_project(secretary, 1));

    sct_secretary_free(secretary);
}


void test_secretary_get_project(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctProject *project1 = sct_secretary_create_project(secretary, "libsecretary"),
            *project2 = sct_secretary_create_project(secretary, "secretary-cocoa");

    SctProject *p = sct_secretary_get_project(secretary, "libsecretary");
    CuAssertPtrEquals(test, project1, p);
    p = sct_secretary_get_project(secretary, "secretary-cocoa");
    CuAssertPtrEquals(test, project2, p);

    // OTOH, if no project with given name is found, return null
    p = sct_secretary_get_project(secretary, "libiexistent!!");
    CuAssertPtrEquals(test, NULL, p);
    sct_secretary_free(secretary);
}

void test_secretary_get_nth_project(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctProject *project1 = sct_secretary_create_project(secretary, "libsecretary"),
            *project2 = sct_secretary_create_project(secretary, "secretary-cocoa");

    SctProject *p = sct_secretary_get_nth_project(secretary, 0);
    CuAssertPtrEquals(test, project1, p);
    p = sct_secretary_get_nth_project(secretary, 1);
    CuAssertPtrEquals(test, project2, p);

    // OTOH, if no project with given name is found, return null
    p = sct_secretary_get_nth_project(secretary, 3);
    CuAssertPtrEquals(test, NULL, p);

    sct_secretary_delete_project(secretary, project1);
    sct_secretary_delete_project(secretary, project2);
    p = sct_secretary_get_nth_project(secretary, 1);
    CuAssertPtrEquals(test, NULL, p);

    sct_secretary_free(secretary);
}

void test_secretary_move_to_project_task_to_project(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test task transference");
    SctProject *project = sct_secretary_create_project(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_project_count_tasks(project, false));

    sct_secretary_move_task_to_project(secretary, project, task);

    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_projects(secretary));
    // No more in inbox
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));
    // Now in project
    CuAssertIntEquals(test, 1, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, task, sct_project_get_nth_task(project, 0, false));
    CuAssertPtrEquals(test, project, sct_task_get_project(task));
    sct_secretary_free(secretary);
}

void test_secretary_move_to_project_task_from_project(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test task transference");
    SctProject *project = sct_secretary_create_project(secretary, "libsecretary");
    
    sct_secretary_move_task_to_project(secretary, project, task);

    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, task, sct_project_get_nth_task(project, 0, false));

    sct_secretary_remove_task_from_project(secretary, task);

    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, NULL, sct_project_get_nth_task(project, 0, false));

    sct_secretary_free(secretary);
}

void test_secretary_move_to_project_task_from_project_to_project(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test task transference");
    SctProject *destination = sct_secretary_create_project(secretary, "Chocrotary");
    SctProject *origin = sct_secretary_create_project(secretary, "libsecretary");
    
    sct_secretary_move_task_to_project(secretary, origin, task);

    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, sct_secretary_count_projects(secretary));
    // No more in inbox
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));
    // Now in project origin
    CuAssertIntEquals(test, 1, sct_project_count_tasks(origin, false));
    CuAssertIntEquals(test, 0, sct_project_count_tasks(destination, false));
    CuAssertPtrEquals(test, task, sct_project_get_nth_task(origin, 0, false));
    CuAssertPtrEquals(test, origin, sct_task_get_project(task));
    // move
    sct_secretary_move_task_to_project(secretary, destination, task);
    // Now in project destination
    CuAssertIntEquals(test, 0, sct_project_count_tasks(origin, false));
    CuAssertIntEquals(test, 1, sct_project_count_tasks(destination, false));
    CuAssertPtrEquals(test, task, sct_project_get_nth_task(destination, 0, false));
    CuAssertPtrEquals(test, destination, sct_task_get_project(task));


    sct_secretary_free(secretary);
}


//////; NEW IMPL!
void test_secretary_task_set_project_to_project(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test task transference");
    SctProject *project = sct_secretary_create_project(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_project_count_tasks(project, false));

    sct_task_set_project(task, project);

    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_projects(secretary));
    // No more in inbox
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));
    // Now in project
    CuAssertIntEquals(test, 1, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, task, sct_project_get_nth_task(project, 0, false));
    CuAssertPtrEquals(test, project, sct_task_get_project(task));
    sct_secretary_free(secretary);
}

void test_secretary_task_set_project_from_project(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test task transference");
    SctProject *project = sct_secretary_create_project(secretary, "libsecretary");
    
    sct_secretary_move_task_to_project(secretary, project, task);

    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, task, sct_project_get_nth_task(project, 0, false));

    sct_task_unset_project(task);

    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, NULL, sct_project_get_nth_task(project, 0, false));

    sct_secretary_free(secretary);
}

void test_secretary_task_set_project_from_project_to_project(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test task transference");
    SctProject *destination = sct_secretary_create_project(secretary, "Chocrotary");
    SctProject *origin = sct_secretary_create_project(secretary, "libsecretary");
    
    sct_task_set_project(task, origin);

    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, sct_secretary_count_projects(secretary));
    // No more in inbox
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));
    // Now in project origin
    CuAssertIntEquals(test, 1, sct_project_count_tasks(origin, false));
    CuAssertIntEquals(test, 0, sct_project_count_tasks(destination, false));
    CuAssertPtrEquals(test, task, sct_project_get_nth_task(origin, 0, false));
    CuAssertPtrEquals(test, origin, sct_task_get_project(task));
    // move
    sct_task_set_project(task, destination);
    // Now in project destination
    CuAssertIntEquals(test, 0, sct_project_count_tasks(origin, false));
    CuAssertIntEquals(test, 1, sct_project_count_tasks(destination, false));
    CuAssertPtrEquals(test, task, sct_project_get_nth_task(destination, 0, false));
    CuAssertPtrEquals(test, destination, sct_task_get_project(task));


    sct_secretary_free(secretary);
}
//////////////

void test_secretary_remove_task(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test task transference");

    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertPtrEquals(test, task, sct_secretary_get_task(secretary, 0));
    CuAssertPtrEquals(test, task, sct_secretary_get_nth_inbox_task(secretary, 0, false));

    sct_secretary_delete_task(secretary, task);

    CuAssertIntEquals(test, 0, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertPtrEquals(test, NULL, sct_secretary_get_task(secretary, 0));
    CuAssertPtrEquals(test, NULL, sct_secretary_get_nth_inbox_task(secretary, 0, false));
    sct_secretary_free(secretary);
}

void test_secretary_remove_project_task(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test task transference");
    SctProject *project = sct_secretary_create_project(secretary, "libsecretary");

    sct_secretary_move_task_to_project(secretary, project, task);
    
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertPtrEquals(test, task, sct_secretary_get_task(secretary, 0));
    CuAssertIntEquals(test, 1, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, task, sct_project_get_nth_task(project, 0, false));
    CuAssertPtrEquals(test, project, sct_task_get_project(task));

    sct_secretary_delete_task(secretary, task);

    CuAssertIntEquals(test, 0, sct_secretary_count_tasks(secretary, false));
    CuAssertPtrEquals(test, NULL, sct_secretary_get_task(secretary, 0));
    CuAssertIntEquals(test, 0, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, NULL, sct_project_get_nth_task(project, 0, false));
    sct_secretary_free(secretary);
}

void test_secretary_remove_project(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctProject *project1 = sct_secretary_create_project(secretary, "libsecretary"),
        *project2 = sct_secretary_create_project(secretary, "Chocrotary"),
        *project3 = sct_secretary_create_project(secretary, "Netuno NG");

    CuAssertIntEquals(test, 3, sct_secretary_count_projects(secretary));
    CuAssertPtrEquals(test, project2, sct_secretary_get_project(secretary, "Chocrotary"));
    CuAssertPtrEquals(test, project1, sct_secretary_get_nth_project(secretary, 0));
    CuAssertPtrEquals(test, project2, sct_secretary_get_nth_project(secretary, 1));
    CuAssertPtrEquals(test, project3, sct_secretary_get_nth_project(secretary, 2));

    sct_secretary_delete_project(secretary, project2);

    CuAssertIntEquals(test, 2, sct_secretary_count_projects(secretary));
    CuAssertPtrEquals(test, NULL, sct_secretary_get_project(secretary, "Chocrotary"));
    CuAssertPtrEquals(test, project1, sct_secretary_get_nth_project(secretary, 0));
    CuAssertPtrEquals(test, project3, sct_secretary_get_nth_project(secretary, 1));

    sct_secretary_free(secretary);
}

void test_secretary_remove_project_with_task(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test task transference");
    SctProject *project = sct_secretary_create_project(secretary, "libsecretary");

        sct_secretary_move_task_to_project(secretary, project, task);
    
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, task, sct_project_get_nth_task(project, 0, false));
    CuAssertPtrEquals(test, project, sct_task_get_project(task));
    CuAssertIntEquals(test, 1, sct_secretary_count_projects(secretary));

    sct_secretary_delete_project(secretary, project);

    CuAssertIntEquals(test, 0, sct_secretary_count_projects(secretary));
    CuAssertPtrEquals(test, NULL, sct_secretary_get_project(secretary, "libsecretary"));
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertPtrEquals(test, NULL, sct_task_get_project(task));

    sct_secretary_free(secretary);
}

void test_secretary_remove_task_with_others(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "Create 1st task"),
         *task2 = sct_secretary_create_task(secretary, "Create 2nd task"),
         *task3 = sct_secretary_create_task(secretary, "Create 3rd task");

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 3, sct_secretary_count_inbox_tasks(secretary, false));

    SctTask *f1 = sct_secretary_get_task(secretary, 0),
         *f2 = sct_secretary_get_task(secretary, 1),
         *f3 = sct_secretary_get_task(secretary, 2);
    CuAssertPtrEquals(test, task1, f1);
    CuAssertPtrEquals(test, task2, f2);
    CuAssertPtrEquals(test, task3, f3);

    sct_secretary_delete_task(secretary, task2);

    CuAssertIntEquals(test, 2, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, sct_secretary_count_inbox_tasks(secretary, false));

    f1 = sct_secretary_get_task(secretary, 0);
    f2 = sct_secretary_get_task(secretary, 1);
    f3 = sct_secretary_get_task(secretary, 2);

    CuAssertPtrEquals(test, task1, f1);
    CuAssertPtrEquals(test, task3, f2);
    CuAssertPtrEquals(test, NULL, f3);
    sct_secretary_free(secretary);
}

void test_secretary_remove_project_with_others(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctProject *project1 = sct_secretary_create_project(secretary, "Create first project"),
         *project2 = sct_secretary_create_project(secretary, "Create snd project"),
         *project3 = sct_secretary_create_project(secretary, "Create third project");

    CuAssertIntEquals(test, 3, sct_secretary_count_projects(secretary));

    SctProject *f1 = sct_secretary_get_project(secretary, "Create first project"),
         *f2 = sct_secretary_get_project(secretary, "Create snd project"),
         *f3 = sct_secretary_get_project(secretary, "Create third project");
    
    CuAssertPtrEquals(test, project1, f1);
    CuAssertPtrEquals(test, project2, f2);
    CuAssertPtrEquals(test, project3, f3);

    sct_secretary_delete_project(secretary, project2);

    CuAssertIntEquals(test, 2, sct_secretary_count_projects(secretary));

    f1 = sct_secretary_get_project(secretary, "Create first project"),
    f2 = sct_secretary_get_project(secretary, "Create snd project"),
    f3 = sct_secretary_get_project(secretary, "Create third project");

    CuAssertPtrEquals(test, project1, f1);
    CuAssertPtrEquals(test, NULL, f2);
    CuAssertPtrEquals(test, project3, f3);
    sct_secretary_free(secretary);
}

void test_secretary_schedule(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task");
    sct_secretary_create_task(secretary, "Create thrid task");
         
    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 3, sct_secretary_count_inbox_tasks(secretary, false));

    time_t now = time(NULL), future_time;
    future_time = now+60*60*48;
    sct_secretary_schedule_task(secretary, task1, future_time);
    sct_secretary_schedule_task(secretary, task2, now);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));

    CuAssertIntEquals(test, 2, sct_secretary_count_tasks_scheduled_for(secretary, future_time, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks_scheduled_for_today(secretary, false));

    SctTask *task = sct_secretary_get_nth_task_scheduled_for(secretary, future_time, 0, false);
    CuAssertPtrEquals(test, task2, task);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, future_time, 1, false);
    CuAssertPtrEquals(test, task1, task);

    CuAssertIntEquals(test, sct_secretary_count_tasks_scheduled_for_today(secretary, false), 1);    
    task = sct_secretary_get_nth_task_scheduled_for_today(secretary, 0, false);
    CuAssertPtrEquals(test, task2, task);

    sct_secretary_free(secretary);
}

void test_secretary_schedule_only_day_is_relevant(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task"),
         *task3 = sct_secretary_create_task(secretary, "Create third task");
         
    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 3, sct_secretary_count_inbox_tasks(secretary, false));

    time_t today, begining_of_today, end_of_today;
    today = time(NULL);
    begining_of_today = today  -  today % (60*60*24); // Beginning of the day
    end_of_today = begining_of_today + 60*(60*23 + 59) + 59; // End of the day
    sct_secretary_schedule_task(secretary, task1, begining_of_today);
    sct_secretary_schedule_task(secretary, task2, today);
    sct_secretary_schedule_task(secretary, task3, end_of_today);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));

#   warning "It fails for problems with timezones. Repair it"
    CuAssertIntEquals(test, 3, sct_secretary_count_tasks_scheduled_for(secretary, today, false));
    CuAssertIntEquals(test, 3, sct_secretary_count_tasks_scheduled_for_today(secretary, false));

    SctTask *task = sct_secretary_get_nth_task_scheduled_for(secretary, today, 0, false);
    CuAssertPtrEquals(test, task1, task);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, today, 1, false);
    CuAssertPtrEquals(test, task2, task);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, today, 2, false);
    CuAssertPtrEquals(test, task3, task);

    sct_secretary_free(secretary);
}

void test_secretary_late_scheduled_appears_in_today(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task"),
         *task3 = sct_secretary_create_task(secretary, "Create thrid task");
         
    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 3, sct_secretary_count_inbox_tasks(secretary, false));

    time_t now = time(NULL), future_time, past_time;
    future_time = now + 60*60*48;
    past_time =   now - 60*60*48;
    sct_secretary_schedule_task(secretary, task1, future_time);
    sct_secretary_schedule_task(secretary, task2, now);
    sct_secretary_schedule_task(secretary, task3, past_time);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks_scheduled_for(secretary, future_time, false));
    CuAssertIntEquals(test, 2, sct_secretary_count_tasks_scheduled_for_today(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks_scheduled_for(secretary, past_time, false));

    SctTask *task = sct_secretary_get_nth_task_scheduled_for(secretary, future_time, 0, false);
    CuAssertPtrEquals(test, task3, task);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, future_time, 1, false);
    CuAssertPtrEquals(test, task2, task);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, future_time, 2, false);
    CuAssertPtrEquals(test, task1, task);

    task = sct_secretary_get_nth_task_scheduled_for_today(secretary, 0, false);
    CuAssertPtrEquals(test, task3, task);
    task = sct_secretary_get_nth_task_scheduled_for_today(secretary, 1, false);
    CuAssertPtrEquals(test, task2, task);

    sct_secretary_free(secretary);
}

void test_secretary_unschedule_task(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task");
    sct_secretary_create_task(secretary, "Create thrid task");
    SctProject *project = sct_secretary_create_project(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 3, sct_secretary_count_inbox_tasks(secretary, false));

    time_t now = time(NULL), future_time = now+60*60*48;

    sct_secretary_schedule_task(secretary, task1, future_time);

    // One less in inbox
    CuAssertIntEquals(test, 2, sct_secretary_count_inbox_tasks(secretary, false));
    sct_secretary_schedule_task(secretary, task2, now);
    // The SCHEDULE state should be preserved!
        sct_secretary_move_task_to_project(secretary, project, task2);

    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));

    CuAssertIntEquals(test, 2, sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 2, sct_secretary_count_tasks_scheduled_for(secretary, future_time, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks_scheduled_for_today(secretary, false));

    SctTask *task = sct_secretary_get_nth_task_scheduled_for(secretary, future_time, 0, false);
    CuAssertPtrEquals(test, task2, task);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, future_time, 1, false);
    CuAssertPtrEquals(test, task1, task);

    task = sct_secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task2, task);
    task = sct_secretary_get_nth_task_scheduled(secretary, 1, false);
    CuAssertPtrEquals(test, task1, task);
    
    task = sct_secretary_get_nth_task_scheduled_for_today(secretary, 0, false);
    CuAssertPtrEquals(test, task2, task);
    task = sct_secretary_get_nth_task_scheduled(secretary, 1, false);
    CuAssertPtrEquals(test, task1, task);

    sct_secretary_unschedule_task(secretary, task1);
    // One more in inbox, since it returns
    CuAssertIntEquals(test, 2, sct_secretary_count_inbox_tasks(secretary, false));    
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks_scheduled_for(secretary, now, false));   

    sct_secretary_unschedule_task(secretary, task2);
    // No one more, since the task2 was in a project
    CuAssertIntEquals(test, 2, sct_secretary_count_inbox_tasks(secretary, false)); 
    CuAssertIntEquals(test, 0, sct_secretary_count_tasks_scheduled_for_today(secretary, false));   

    sct_secretary_free(secretary);
}

void test_secretary_mark_task_as_done(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctProject *project = sct_secretary_create_project(secretary, "libsecretary");
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task"),
         *task3 = sct_secretary_create_task(secretary, "Create thrid task");
    
    struct tm date;
    date.tm_hour = 0;
    date.tm_mday = 30;
    date.tm_mon = 4;
    date.tm_year = 2002-1900;
    time_t t = timegm(&date);
    sct_secretary_schedule_task(secretary, task1, timegm(&date));
    sct_secretary_move_task_to_project(secretary, project, task2);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks_scheduled_for(secretary, t, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_done_tasks(secretary, false));


    sct_task_mark_as_done(task1);
    sct_task_mark_as_done(task2);
    sct_task_mark_as_done(task3);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks_scheduled_for(secretary, t, false));
    CuAssertIntEquals(test, 3, sct_secretary_count_done_tasks(secretary, false));

    // Should be found in their places
    SctTask *task = sct_secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, task3, task);
    // Should be found as scheduled
    task = sct_secretary_get_nth_task_scheduled_for(secretary, t, 0, false);
    CuAssertPtrEquals(test, task1, task);
    task = sct_secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task1, task);
    // Should be found in project
    task = sct_project_get_nth_task(project, 0, false);
    CuAssertPtrEquals(test, task2, task);

    // Getting the things done :)
    task = sct_secretary_get_nth_done_task(secretary, 0, false);
    CuAssertPtrEquals(test, task1, task);
    task = sct_secretary_get_nth_done_task(secretary, 1, false);
    CuAssertPtrEquals(test, task2, task);
    task = sct_secretary_get_nth_done_task(secretary, 2, false);
    CuAssertPtrEquals(test, task3, task);
    task = sct_secretary_get_nth_done_task(secretary, 3, false);
    CuAssertPtrEquals(test, NULL, task);
    sct_secretary_free(secretary);
}

void test_secretary_unmark_task_as_done(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctProject *project = sct_secretary_create_project(secretary, "libsecretary");
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task"),
         *task3 = sct_secretary_create_task(secretary, "Create thrid task");
    
    struct tm date;
    date.tm_hour = 0;
    date.tm_mday = 30;
    date.tm_mon = 4;
    date.tm_year = 2002-1900;
    time_t t = timegm(&date);
    sct_secretary_schedule_task(secretary, task1, t);
    sct_secretary_move_task_to_project(secretary, project, task2);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, 
            sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            sct_secretary_count_tasks_scheduled_for(secretary, t, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_done_tasks(secretary, false));


    sct_task_mark_as_done(task1);
    sct_task_mark_as_done(task2);
    sct_task_mark_as_done(task3);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, 
            sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            sct_secretary_count_tasks_scheduled_for(secretary, t, false));
    CuAssertIntEquals(test, 3, sct_secretary_count_done_tasks(secretary, false));

    // Should be found in inbox
    SctTask *task = sct_secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, task3, task);
    // Should be found as scheduled
    task = sct_secretary_get_nth_task_scheduled_for(secretary, t, 0, false);
    CuAssertPtrEquals(test, task1, task);
    task = sct_secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task1, task);
    // Should be found in project
    task = sct_project_get_nth_task(project, 0, false);
    CuAssertPtrEquals(test, task2, task);

    // UNDOING
    sct_task_unmark_as_done(task1);
    sct_task_unmark_as_done(task2);
    sct_task_unmark_as_done(task3);

    // found as scheduled
    task = sct_secretary_get_nth_task_scheduled_for(secretary, t, 0, false);
    CuAssertPtrEquals(test, task1, task);
    task = sct_secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task1, task);
    // found in project
    task = sct_project_get_nth_task(project, 0, false);
    CuAssertPtrEquals(test, task2, task);
    // Found in inbox
    task = sct_secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, task3, task);

    CuAssertIntEquals(test, 0, 
            sct_secretary_count_done_tasks(secretary, false));
    CuAssertPtrEquals(test, NULL, 
            sct_secretary_get_nth_done_task(secretary, 0, false));
}

void test_secretary_archived_inbox(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task"),
         *task3 = sct_secretary_create_task(secretary, "Create thrid task");

    sct_task_mark_as_done(task1);
    sct_task_mark_as_done(task2);
    sct_secretary_archive_task(secretary, task2);

    CuAssertIntEquals(test, 2, sct_secretary_count_inbox_tasks(secretary, false));
    SctTask *task = sct_secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = sct_secretary_get_nth_inbox_task(secretary, 1, false);
    CuAssertPtrEquals(test, task, task3);

    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, true));
    task = sct_secretary_get_nth_inbox_task(secretary, 0, true);
    CuAssertPtrEquals(test, task, task2);
    
    sct_secretary_free(secretary);
}

void test_secretary_archived_scheduled(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task"),
         *task3 = sct_secretary_create_task(secretary, "Create thrid task");

    time_t date = time(NULL);

    sct_secretary_schedule_task(secretary, task1, date);
    sct_secretary_schedule_task(secretary, task2, date);
    sct_secretary_schedule_task(secretary, task3, date);

    sct_task_mark_as_done(task1);
    sct_task_mark_as_done(task2);
    sct_secretary_archive_task(secretary, task2);

    CuAssertIntEquals(test, 2, 
            sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 2, 
            sct_secretary_count_tasks_scheduled_for(secretary, date, false));
    CuAssertIntEquals(test, 2,
            sct_secretary_count_tasks_scheduled_for_today(secretary, false));

    SctTask *task = sct_secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, date, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = sct_secretary_get_nth_task_scheduled_for_today(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);

    task = sct_secretary_get_nth_task_scheduled(secretary, 1, false);
    CuAssertPtrEquals(test, task, task3);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, date, 1, false);
    CuAssertPtrEquals(test, task, task3);
    task = sct_secretary_get_nth_task_scheduled_for_today(secretary, 1, false);
    CuAssertPtrEquals(test, task, task3);

    CuAssertIntEquals(test, 1, 
            sct_secretary_count_tasks_scheduled(secretary, true));
    CuAssertIntEquals(test, 1, 
            sct_secretary_count_tasks_scheduled_for(secretary, date, true));
    CuAssertIntEquals(test, 1,
            sct_secretary_count_tasks_scheduled_for_today(secretary, true));

    task = sct_secretary_get_nth_task_scheduled(secretary, 0, true);
    CuAssertPtrEquals(test, task, task2);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, date, 0, true);
    CuAssertPtrEquals(test, task, task2);
    task = sct_secretary_get_nth_task_scheduled_for_today(secretary, 0, true);
    CuAssertPtrEquals(test, task, task2);
    
    sct_secretary_free(secretary);
}

void test_secretary_archived_done(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task"),
         *task3 = sct_secretary_create_task(secretary, "Create thrid task");

    sct_task_mark_as_done(task1);
    sct_task_mark_as_done(task2);
    sct_task_mark_as_done(task3);
    sct_task_archive(task2);

    CuAssertIntEquals(test, 2, sct_secretary_count_done_tasks(secretary, false));
    SctTask *task = sct_secretary_get_nth_done_task(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = sct_secretary_get_nth_done_task(secretary, 1, false);
    CuAssertPtrEquals(test, task, task3);

    CuAssertIntEquals(test, 1, sct_secretary_count_done_tasks(secretary, true));
    task = sct_secretary_get_nth_done_task(secretary, 0, true);
    CuAssertPtrEquals(test, task, task2);
    
    sct_secretary_free(secretary);
}

void test_secretary_archive_inbox_tasks(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task"),
         *task3 = sct_secretary_create_task(secretary, "Create thrid task");

    sct_task_mark_as_done(task2);

    CuAssertIntEquals(test, 3, sct_secretary_count_inbox_tasks(secretary, false));
    SctTask *task = sct_secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, task1, task);
    task = sct_secretary_get_nth_inbox_task(secretary, 1, false);
    CuAssertPtrEquals(test, task2, task);
    task = sct_secretary_get_nth_inbox_task(secretary, 2, false);
    CuAssertPtrEquals(test, task3, task);

    sct_secretary_archive_inbox_tasks(secretary);

    CuAssertIntEquals(test, 2, sct_secretary_count_inbox_tasks(secretary, false));
    task = sct_secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = sct_secretary_get_nth_inbox_task(secretary, 1, false);
    CuAssertPtrEquals(test, task, task3);

    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, true));
    task = sct_secretary_get_nth_inbox_task(secretary, 0, true);
    CuAssertPtrEquals(test, task, task2);
    
    sct_secretary_free(secretary);
}

void test_secretary_archive_lots_of_inbox_tasks(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *tasks[] = {
        sct_secretary_create_task(secretary, "Create first task"),
        sct_secretary_create_task(secretary, "Create snd task"),
        sct_secretary_create_task(secretary, "Create thrid task"),
        sct_secretary_create_task(secretary, "Create fourth task"),
        sct_secretary_create_task(secretary, "Create fifth task"),
        sct_secretary_create_task(secretary, "Create sixth task"),
        sct_secretary_create_task(secretary, "Create seventh task"),
        sct_secretary_create_task(secretary, "Create eighth task"),
        sct_secretary_create_task(secretary, "Create nineth task"),
        sct_secretary_create_task(secretary, "Create tenth task")
    };

    for (int i = 0; i < 10; i+= 2) {
        sct_task_mark_as_done(tasks[i]);
    }

    sct_secretary_archive_inbox_tasks(secretary);

    CuAssertIntEquals(test, 5, sct_secretary_count_inbox_tasks(secretary, false));
    for (int i = 0; i < 5; i++) {
        SctTask *task = sct_secretary_get_nth_inbox_task(secretary, i, false);
        CuAssertPtrEquals(test, task, tasks[i*2+1]);
        
    }

    CuAssertIntEquals(test, 5, sct_secretary_count_inbox_tasks(secretary, true));
    for (int i = 0; i < 5; i++) {
        SctTask *task = sct_secretary_get_nth_inbox_task(secretary, i, true);
        CuAssertPtrEquals(test, task, tasks[i*2]);
        
    }

    sct_secretary_free(secretary);
}


void test_secretary_archive_scheduled_task(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task"),
         *task3 = sct_secretary_create_task(secretary, "Create thrid task");

    time_t date1 = time(NULL);
    sct_secretary_schedule_task(secretary, task1, date1);
    time_t date2 = date1 + 24*60*60*30;
    sct_secretary_schedule_task(secretary, task2, date2);
    time_t date3 = date2 + 24*60*60*30;
    sct_secretary_schedule_task(secretary, task3, date3);

    sct_task_mark_as_done(task1);
    sct_task_mark_as_done(task2);
    sct_task_mark_as_done(task3);

    CuAssertIntEquals(test, 3, 
            sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            sct_secretary_count_tasks_scheduled_for(secretary, date1, false));
    CuAssertIntEquals(test, 2, 
            sct_secretary_count_tasks_scheduled_for(secretary, date2, false));
    CuAssertIntEquals(test, 3, 
            sct_secretary_count_tasks_scheduled_for(secretary, date3, false));
    CuAssertIntEquals(test, 1,
            sct_secretary_count_tasks_scheduled_for_today(secretary, false));

    SctTask *task = sct_secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, date1, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = sct_secretary_get_nth_task_scheduled_for_today(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);

    task = sct_secretary_get_nth_task_scheduled(secretary, 1, false);
    CuAssertPtrEquals(test, task, task2);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, date2, 1, false);
    CuAssertPtrEquals(test, task, task2);

    task = sct_secretary_get_nth_task_scheduled(secretary, 2, false);
    CuAssertPtrEquals(test, task, task3);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, date3, 2, false);
    CuAssertPtrEquals(test, task, task3);

    sct_secretary_archive_tasks_scheduled_for_today(secretary);

    CuAssertIntEquals(test, 2, 
            sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 0, 
            sct_secretary_count_tasks_scheduled_for(secretary, date1, false));
    CuAssertIntEquals(test, 0,
            sct_secretary_count_tasks_scheduled_for_today(secretary, false));

    task = sct_secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task, task2);
    task = sct_secretary_get_nth_task_scheduled(secretary, 1, false);
    CuAssertPtrEquals(test, task, task3);
    task = sct_secretary_get_nth_task_scheduled(secretary, 0, true);
    CuAssertPtrEquals(test, task, task1);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, date1, 0, true);
    CuAssertPtrEquals(test, task, task1);
    task = sct_secretary_get_nth_task_scheduled_for_today(secretary, 0, true);
    CuAssertPtrEquals(test, task, task1);

    sct_secretary_archive_tasks_scheduled_for(secretary, date2);

    CuAssertIntEquals(test, 1, 
            sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 0, 
            sct_secretary_count_tasks_scheduled_for(secretary, date2, false));

    task = sct_secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task, task3);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, date2, 0, true);
    CuAssertPtrEquals(test, task, task1);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, date2, 1, true);
    CuAssertPtrEquals(test, task, task2);

    sct_secretary_archive_scheduled_tasks(secretary);

    CuAssertIntEquals(test, 0, 
            sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 0, 
            sct_secretary_count_tasks_scheduled_for(secretary, date3, false));

    task = sct_secretary_get_nth_task_scheduled_for(secretary, date3, 0, true);
    CuAssertPtrEquals(test, task, task1);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, date3, 1, true);
    CuAssertPtrEquals(test, task, task2);
    task = sct_secretary_get_nth_task_scheduled_for(secretary, date3, 2, true);
    CuAssertPtrEquals(test, task, task3);


    sct_secretary_free(secretary);
}

void test_secretary_count_tasks_archived(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task");
         /**task3 = */sct_secretary_create_task(secretary, "Create thrid task");

    sct_task_mark_as_done(task1);
    sct_task_mark_as_done(task2);
    sct_secretary_archive_task(secretary, task2);

    CuAssertIntEquals(test, 2, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, true));
        
    sct_secretary_free(secretary);
}

void test_secretary_count_all_tasks(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "Create first task"),
         *task2 = sct_secretary_create_task(secretary, "Create snd task");
         /**task3 = */sct_secretary_create_task(secretary, "Create thrid task");

    sct_task_mark_as_done(task1);
    sct_task_mark_as_done(task2);
    sct_secretary_archive_task(secretary, task2);

    CuAssertIntEquals(test, 2, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, true));

    CuAssertIntEquals(test, 3, sct_secretary_count_all_tasks(secretary));

    sct_secretary_free(secretary);
}

static void test_secretary_remove_from_project(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "task 1"),
        *task2 = sct_secretary_create_task(secretary, "task 2"),
        *task3 = sct_secretary_create_task(secretary, "task 3");

    SctProject *project = sct_secretary_create_project(secretary, "project");
    sct_secretary_move_task_to_project(secretary, project, task1);
    sct_secretary_move_task_to_project(secretary, project, task3);

    CuAssertIntEquals(test, 2, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, task1, sct_project_get_nth_task(project, 0, false));
    CuAssertPtrEquals(test, task3, sct_project_get_nth_task(project, 1, false));

    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertPtrEquals(test, task2, 
            sct_secretary_get_nth_inbox_task(secretary, 0, false));

    sct_secretary_remove_task_from_project(secretary, task1);

    CuAssertIntEquals(test, 1, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, task3, sct_project_get_nth_task(project, 0, false));

    CuAssertIntEquals(test, 2, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertPtrEquals(test, task1, 
            sct_secretary_get_nth_inbox_task(secretary, 0, false));
    CuAssertPtrEquals(test, task2, 
            sct_secretary_get_nth_inbox_task(secretary, 1, false));

    sct_secretary_free(secretary);
}

static void test_secretary_archive_tasks_from_project(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "task 1"),
        *task2 = sct_secretary_create_task(secretary, "task 2"),
        *task3 = sct_secretary_create_task(secretary, "task 3");

    SctProject *project = sct_secretary_create_project(secretary, "project");
    sct_secretary_move_task_to_project(secretary, project, task1);
    sct_secretary_move_task_to_project(secretary, project, task2);
    sct_secretary_move_task_to_project(secretary, project, task3);

    CuAssertIntEquals(test, 3, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, task1, sct_project_get_nth_task(project, 0, false));
    CuAssertPtrEquals(test, task2, sct_project_get_nth_task(project, 1, false));
    CuAssertPtrEquals(test, task3, sct_project_get_nth_task(project, 2, false));
    CuAssertIntEquals(test, 0, sct_project_count_tasks(project, true));

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertPtrEquals(test, task1, sct_secretary_get_nth_task(secretary, 0));
    CuAssertPtrEquals(test, task2, sct_secretary_get_nth_task(secretary, 1));
    CuAssertPtrEquals(test, task3, sct_secretary_get_nth_task(secretary, 2));
    CuAssertIntEquals(test, 0, sct_secretary_count_tasks(secretary, true));

    sct_task_mark_as_done(task2);
    sct_secretary_archive_tasks_from_project(secretary, project);

    CuAssertIntEquals(test, 2, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, task1, sct_project_get_nth_task(project, 0, false));
    CuAssertPtrEquals(test, task3, sct_project_get_nth_task(project, 1, false));
    CuAssertIntEquals(test, 1, sct_project_count_tasks(project, true));
    CuAssertPtrEquals(test, task2, sct_project_get_nth_task(project, 0, true));

    CuAssertIntEquals(test, 2, sct_secretary_count_tasks(secretary, false));
    CuAssertPtrEquals(test, task1, sct_secretary_get_nth_task(secretary, 0));
    CuAssertPtrEquals(test, task3, sct_secretary_get_nth_task(secretary, 1));
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, true));
    // It should be the third one because of optimization ordering
    CuAssertPtrEquals(test, task2, sct_secretary_get_nth_task(secretary, 2));

    sct_secretary_free(secretary);
}

void test_secretary_add_tasks(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test task creation");
    
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, false));

    CuAssertStrEquals(test, "Test task creation", sct_task_get_description(task));
    CuAssertPtrEquals(test, NULL, sct_task_get_project(task));

    sct_secretary_free(secretary);
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
    SUITE_ADD_TEST(suite, test_secretary_task_set_project_to_project);
    SUITE_ADD_TEST(suite, test_secretary_task_set_project_from_project_to_project);
    SUITE_ADD_TEST(suite, test_secretary_task_set_project_from_project);
    SUITE_ADD_TEST(suite, test_secretary_remove_task);
    SUITE_ADD_TEST(suite, test_secretary_remove_project);
    SUITE_ADD_TEST(suite, test_secretary_remove_project_with_task);
    SUITE_ADD_TEST(suite, test_secretary_remove_project_task);
    SUITE_ADD_TEST(suite, test_secretary_remove_task_with_others);
    SUITE_ADD_TEST(suite, test_secretary_remove_project_with_others);
    SUITE_ADD_TEST(suite, test_secretary_schedule);
    SUITE_ADD_TEST(suite, test_secretary_schedule_only_day_is_relevant);
    SUITE_ADD_TEST(suite, test_secretary_late_scheduled_appears_in_today);
    SUITE_ADD_TEST(suite, test_secretary_unschedule_task);
    SUITE_ADD_TEST(suite, test_secretary_mark_task_as_done);
    SUITE_ADD_TEST(suite, test_secretary_unmark_task_as_done);
    SUITE_ADD_TEST(suite, test_secretary_archived_inbox);
    SUITE_ADD_TEST(suite, test_secretary_archived_scheduled);
    SUITE_ADD_TEST(suite, test_secretary_archived_done);
    SUITE_ADD_TEST(suite, test_secretary_archive_inbox_tasks);
    SUITE_ADD_TEST(suite, test_secretary_archive_lots_of_inbox_tasks);
    SUITE_ADD_TEST(suite, test_secretary_archive_scheduled_task);
    SUITE_ADD_TEST(suite, test_secretary_count_tasks_archived);
    SUITE_ADD_TEST(suite, test_secretary_count_all_tasks);
    SUITE_ADD_TEST(suite, test_secretary_remove_from_project);
    SUITE_ADD_TEST(suite, test_secretary_archive_tasks_from_project);
    SUITE_ADD_TEST(suite, test_secretary_archive_project);
    return suite;
}

