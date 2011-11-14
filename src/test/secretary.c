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
    Secretary *secretary = secretary_new();
    CuAssertIntEquals(test, 0, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));

    secretary_free(secretary);
}

void test_secretary_create_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task creation");
    
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));

    CuAssertStrEquals(test, "Test task creation", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));

    secretary_free(secretary);
}

void test_secretary_get_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task creation");

    Task *task2 = secretary_get_task(secretary, 0);
    CuAssertPtrEquals(test, task, task2);

    // Get first task
    Task *task3 = secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, task, task3);

    // OTOH, should return NULL if no task with given number
    task2 = secretary_get_task(secretary, 32);
    CuAssertPtrEquals(test, NULL, task2);
    task2 = secretary_get_nth_inbox_task(secretary, 2, false);
    CuAssertPtrEquals(test, NULL, task2);
    secretary_free(secretary);
}

void test_secretary_create_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Project *project = secretary_create_project(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 0, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));

    CuAssertStrEquals(test, "libsecretary", project_get_name(project));
    CuAssertIntEquals(test, 0, project_count_tasks(project, false));
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
    
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 0, project_count_tasks(project, false));

    project_add_task(project, task);

    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_projects(secretary));
    // No more in inbox
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));
    // Now in project
    CuAssertIntEquals(test, 1, project_count_tasks(project, false));
    CuAssertPtrEquals(test, task, project_get_nth_task(project, 0, false));
    CuAssertPtrEquals(test, project, task_get_project(task));
    secretary_free(secretary);
}

void test_secretary_move_to_project_task_from_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task transference");
    Project *project = secretary_create_project(secretary, "libsecretary");
    
    project_add_task(project, task);

    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, project_count_tasks(project, false));
    CuAssertPtrEquals(test, task, project_get_nth_task(project, 0, false));

    project_remove_task(project, task);

    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 0, project_count_tasks(project, false));
    CuAssertPtrEquals(test, NULL, project_get_nth_task(project, 0, false));

    secretary_free(secretary);
}

void test_secretary_move_to_project_task_from_project_to_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task transference");
    Project *destination = secretary_create_project(secretary, "Chocrotary");
    Project *origin = secretary_create_project(secretary, "libsecretary");
    
    project_add_task(origin, task);

    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, secretary_count_projects(secretary));
    // No more in inbox
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));
    // Now in project origin
    CuAssertIntEquals(test, 1, project_count_tasks(origin, false));
    CuAssertIntEquals(test, 0, project_count_tasks(destination, false));
    CuAssertPtrEquals(test, task, project_get_nth_task(origin, 0, false));
    CuAssertPtrEquals(test, origin, task_get_project(task));
    // move
    project_add_task(destination, task);
    // Now in project destination
    CuAssertIntEquals(test, 0, project_count_tasks(origin, false));
    CuAssertIntEquals(test, 1, project_count_tasks(destination, false));
    CuAssertPtrEquals(test, task, project_get_nth_task(destination, 0, false));
    CuAssertPtrEquals(test, destination, task_get_project(task));


    secretary_free(secretary);
}


void test_secretary_remove_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task transference");

    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));
    CuAssertPtrEquals(test, task, secretary_get_task(secretary, 0));
    CuAssertPtrEquals(test, task, secretary_get_nth_inbox_task(secretary, 0, false));

    secretary_delete_task(secretary, task);

    CuAssertIntEquals(test, 0, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));
    CuAssertPtrEquals(test, NULL, secretary_get_task(secretary, 0));
    CuAssertPtrEquals(test, NULL, secretary_get_nth_inbox_task(secretary, 0, false));
    secretary_free(secretary);
}

void test_secretary_remove_project_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test task transference");
    Project *project = secretary_create_project(secretary, "libsecretary");

    project_add_task(project, task);
    
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, false));
    CuAssertPtrEquals(test, task, secretary_get_task(secretary, 0));
    CuAssertIntEquals(test, 1, project_count_tasks(project, false));
    CuAssertPtrEquals(test, task, project_get_nth_task(project, 0, false));
    CuAssertPtrEquals(test, project, task_get_project(task));

    secretary_delete_task(secretary, task);

    CuAssertIntEquals(test, 0, secretary_count_tasks(secretary, false));
    CuAssertPtrEquals(test, NULL, secretary_get_task(secretary, 0));
    CuAssertIntEquals(test, 0, project_count_tasks(project, false));
    CuAssertPtrEquals(test, NULL, project_get_nth_task(project, 0, false));
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
    
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, project_count_tasks(project, false));
    CuAssertPtrEquals(test, task, project_get_nth_task(project, 0, false));
    CuAssertPtrEquals(test, project, task_get_project(task));
    CuAssertIntEquals(test, 1, secretary_count_projects(secretary));

    secretary_delete_project(secretary, project);

    CuAssertIntEquals(test, 0, secretary_count_projects(secretary));
    CuAssertPtrEquals(test, NULL, secretary_get_project(secretary, "libsecretary"));
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));
    CuAssertPtrEquals(test, NULL, task_get_project(task));

    secretary_free(secretary);
}

void test_secretary_remove_task_with_others(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create 1st task"),
         *task2 = secretary_create_task(secretary, "Create 2nd task"),
         *task3 = secretary_create_task(secretary, "Create 3rd task");

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 3, secretary_count_inbox_tasks(secretary, false));

    Task *f1 = secretary_get_task(secretary, 0),
         *f2 = secretary_get_task(secretary, 1),
         *f3 = secretary_get_task(secretary, 2);
    CuAssertPtrEquals(test, task1, f1);
    CuAssertPtrEquals(test, task2, f2);
    CuAssertPtrEquals(test, task3, f3);

    secretary_delete_task(secretary, task2);

    CuAssertIntEquals(test, 2, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, secretary_count_inbox_tasks(secretary, false));

    f1 = secretary_get_task(secretary, 0);
    f2 = secretary_get_task(secretary, 1);
    f3 = secretary_get_task(secretary, 2);

    CuAssertPtrEquals(test, task1, f1);
    CuAssertPtrEquals(test, task3, f2);
    CuAssertPtrEquals(test, NULL, f3);
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
         
    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 3, secretary_count_inbox_tasks(secretary, false));

    time_t now = time(NULL), future_time;
    future_time = now+60*60*48;
    secretary_schedule_task(secretary, task1, future_time);
    secretary_schedule_task(secretary, task2, now);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));

    CuAssertIntEquals(test, 2, secretary_count_tasks_scheduled_for(secretary, future_time, false));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled_for_today(secretary, false));

    Task *task = secretary_get_nth_task_scheduled_for(secretary, future_time, 0, false);
    CuAssertPtrEquals(test, task2, task);
    task = secretary_get_nth_task_scheduled_for(secretary, future_time, 1, false);
    CuAssertPtrEquals(test, task1, task);

    CuAssertIntEquals(test, secretary_count_tasks_scheduled_for_today(secretary, false), 1);    
    task = secretary_get_nth_task_scheduled_for_today(secretary, 0, false);
    CuAssertPtrEquals(test, task2, task);

    secretary_free(secretary);
}

void test_secretary_schedule_only_day_is_relevant(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task"),
         *task3 = secretary_create_task(secretary, "Create third task");
         
    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 3, secretary_count_inbox_tasks(secretary, false));

    time_t today, begining_of_today, end_of_today;
    today = time(NULL);
    begining_of_today = today  -  today % (60*60*24); // Beginning of the day
    end_of_today = begining_of_today + 60*(60*23 + 59) + 59; // End of the day
    secretary_schedule_task(secretary, task1, begining_of_today);
    secretary_schedule_task(secretary, task2, today);
    secretary_schedule_task(secretary, task3, end_of_today);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));

    CuAssertIntEquals(test, 3, secretary_count_tasks_scheduled_for(secretary, today, false));
    CuAssertIntEquals(test, 3, secretary_count_tasks_scheduled_for_today(secretary, false));

    Task *task = secretary_get_nth_task_scheduled_for(secretary, today, 0, false);
    CuAssertPtrEquals(test, task1, task);
    task = secretary_get_nth_task_scheduled_for(secretary, today, 1, false);
    CuAssertPtrEquals(test, task2, task);
    task = secretary_get_nth_task_scheduled_for(secretary, today, 2, false);
    CuAssertPtrEquals(test, task3, task);

    secretary_free(secretary);
}

void test_secretary_late_scheduled_appears_in_today(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task"),
         *task3 = secretary_create_task(secretary, "Create thrid task");
         
    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 3, secretary_count_inbox_tasks(secretary, false));

    time_t now = time(NULL), future_time, past_time;
    future_time = now + 60*60*48;
    past_time =   now - 60*60*48;
    secretary_schedule_task(secretary, task1, future_time);
    secretary_schedule_task(secretary, task2, now);
    secretary_schedule_task(secretary, task3, past_time);

    CuAssertIntEquals(test, 3, secretary_count_tasks_scheduled_for(secretary, future_time, false));
    CuAssertIntEquals(test, 2, secretary_count_tasks_scheduled_for_today(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled_for(secretary, past_time, false));

    Task *task = secretary_get_nth_task_scheduled_for(secretary, future_time, 0, false);
    CuAssertPtrEquals(test, task3, task);
    task = secretary_get_nth_task_scheduled_for(secretary, future_time, 1, false);
    CuAssertPtrEquals(test, task2, task);
    task = secretary_get_nth_task_scheduled_for(secretary, future_time, 2, false);
    CuAssertPtrEquals(test, task1, task);

    task = secretary_get_nth_task_scheduled_for_today(secretary, 0, false);
    CuAssertPtrEquals(test, task3, task);
    task = secretary_get_nth_task_scheduled_for_today(secretary, 1, false);
    CuAssertPtrEquals(test, task2, task);

    secretary_free(secretary);
}

void test_secretary_unschedule_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task");
    secretary_create_task(secretary, "Create thrid task");
    Project *project = secretary_create_project(secretary, "libsecretary");
    
    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 3, secretary_count_inbox_tasks(secretary, false));

    time_t now = time(NULL), future_time = now+60*60*48;

    secretary_schedule_task(secretary, task1, future_time);

    // One less in inbox
    CuAssertIntEquals(test, 2, secretary_count_inbox_tasks(secretary, false));
    secretary_schedule_task(secretary, task2, now);
    // The SCHEDULE state should be preserved!
    project_add_task(project, task2);

    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));

    CuAssertIntEquals(test, 2, secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 2, secretary_count_tasks_scheduled_for(secretary, future_time, false));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled_for_today(secretary, false));

    Task *task = secretary_get_nth_task_scheduled_for(secretary, future_time, 0, false);
    CuAssertPtrEquals(test, task2, task);
    task = secretary_get_nth_task_scheduled_for(secretary, future_time, 1, false);
    CuAssertPtrEquals(test, task1, task);

    task = secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task2, task);
    task = secretary_get_nth_task_scheduled(secretary, 1, false);
    CuAssertPtrEquals(test, task1, task);
    
    task = secretary_get_nth_task_scheduled_for_today(secretary, 0, false);
    CuAssertPtrEquals(test, task2, task);
    task = secretary_get_nth_task_scheduled(secretary, 1, false);
    CuAssertPtrEquals(test, task1, task);

    secretary_unschedule_task(secretary, task1);
    // One more in inbox, since it returns
    CuAssertIntEquals(test, 2, secretary_count_inbox_tasks(secretary, false));    
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled_for(secretary, now, false));   

    secretary_unschedule_task(secretary, task2);
    // No one more, since the task2 was in a project
    CuAssertIntEquals(test, 2, secretary_count_inbox_tasks(secretary, false)); 
    CuAssertIntEquals(test, 0, secretary_count_tasks_scheduled_for_today(secretary, false));   

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
    time_t t = timegm(&date);
    secretary_schedule_task(secretary, task1, timegm(&date));
    project_add_task(project, task2);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled_for(secretary, t, false));
    CuAssertIntEquals(test, 0, secretary_count_done_tasks(secretary, false));


    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_mark_as_done(task3);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_tasks_scheduled_for(secretary, t, false));
    CuAssertIntEquals(test, 3, secretary_count_done_tasks(secretary, false));

    // Should be found in their places
    Task *task = secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, task3, task);
    // Should be found as scheduled
    task = secretary_get_nth_task_scheduled_for(secretary, t, 0, false);
    CuAssertPtrEquals(test, task1, task);
    task = secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task1, task);
    // Should be found in project
    task = project_get_nth_task(project, 0, false);
    CuAssertPtrEquals(test, task2, task);

    // Getting the things done :)
    task = secretary_get_nth_done_task(secretary, 0, false);
    CuAssertPtrEquals(test, task1, task);
    task = secretary_get_nth_done_task(secretary, 1, false);
    CuAssertPtrEquals(test, task2, task);
    task = secretary_get_nth_done_task(secretary, 2, false);
    CuAssertPtrEquals(test, task3, task);
    task = secretary_get_nth_done_task(secretary, 3, false);
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
    time_t t = timegm(&date);
    secretary_schedule_task(secretary, task1, t);
    project_add_task(project, task2);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, 
            secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            secretary_count_tasks_scheduled_for(secretary, t, false));
    CuAssertIntEquals(test, 0, secretary_count_done_tasks(secretary, false));


    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_mark_as_done(task3);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, 
            secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            secretary_count_tasks_scheduled_for(secretary, t, false));
    CuAssertIntEquals(test, 3, secretary_count_done_tasks(secretary, false));

    // Should be found in inbox
    Task *task = secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, task3, task);
    // Should be found as scheduled
    task = secretary_get_nth_task_scheduled_for(secretary, t, 0, false);
    CuAssertPtrEquals(test, task1, task);
    task = secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task1, task);
    // Should be found in project
    task = project_get_nth_task(project, 0, false);
    CuAssertPtrEquals(test, task2, task);

    // UNDOING
    task_unmark_as_done(task1);
    task_unmark_as_done(task2);
    task_unmark_as_done(task3);

    // found as scheduled
    task = secretary_get_nth_task_scheduled_for(secretary, t, 0, false);
    CuAssertPtrEquals(test, task1, task);
    task = secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task1, task);
    // found in project
    task = project_get_nth_task(project, 0, false);
    CuAssertPtrEquals(test, task2, task);
    // Found in inbox
    task = secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, task3, task);

    CuAssertIntEquals(test, 0, 
            secretary_count_done_tasks(secretary, false));
    CuAssertPtrEquals(test, NULL, 
            secretary_get_nth_done_task(secretary, 0, false));
}

void test_secretary_archived_inbox(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task"),
         *task3 = secretary_create_task(secretary, "Create thrid task");

    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_archive(task2);

    CuAssertIntEquals(test, 2, secretary_count_inbox_tasks(secretary, false));
    Task *task = secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = secretary_get_nth_inbox_task(secretary, 1, false);
    CuAssertPtrEquals(test, task, task3);

    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, true));
    task = secretary_get_nth_inbox_task(secretary, 0, true);
    CuAssertPtrEquals(test, task, task2);
    
    secretary_free(secretary);
}

void test_secretary_archived_scheduled(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task"),
         *task3 = secretary_create_task(secretary, "Create thrid task");

    time_t date = time(NULL);

    secretary_schedule_task(secretary, task1, date);
    secretary_schedule_task(secretary, task2, date);
    secretary_schedule_task(secretary, task3, date);

    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_archive(task2);

    CuAssertIntEquals(test, 2, 
            secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 2, 
            secretary_count_tasks_scheduled_for(secretary, date, false));
    CuAssertIntEquals(test, 2,
            secretary_count_tasks_scheduled_for_today(secretary, false));

    Task *task = secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = secretary_get_nth_task_scheduled_for(secretary, date, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = secretary_get_nth_task_scheduled_for_today(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);

    task = secretary_get_nth_task_scheduled(secretary, 1, false);
    CuAssertPtrEquals(test, task, task3);
    task = secretary_get_nth_task_scheduled_for(secretary, date, 1, false);
    CuAssertPtrEquals(test, task, task3);
    task = secretary_get_nth_task_scheduled_for_today(secretary, 1, false);
    CuAssertPtrEquals(test, task, task3);

    CuAssertIntEquals(test, 1, 
            secretary_count_tasks_scheduled(secretary, true));
    CuAssertIntEquals(test, 1, 
            secretary_count_tasks_scheduled_for(secretary, date, true));
    CuAssertIntEquals(test, 1,
            secretary_count_tasks_scheduled_for_today(secretary, true));

    task = secretary_get_nth_task_scheduled(secretary, 0, true);
    CuAssertPtrEquals(test, task, task2);
    task = secretary_get_nth_task_scheduled_for(secretary, date, 0, true);
    CuAssertPtrEquals(test, task, task2);
    task = secretary_get_nth_task_scheduled_for_today(secretary, 0, true);
    CuAssertPtrEquals(test, task, task2);
    
    secretary_free(secretary);
}

void test_secretary_archived_done(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task"),
         *task3 = secretary_create_task(secretary, "Create thrid task");

    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_mark_as_done(task3);
    task_archive(task2);

    CuAssertIntEquals(test, 2, secretary_count_done_tasks(secretary, false));
    Task *task = secretary_get_nth_done_task(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = secretary_get_nth_done_task(secretary, 1, false);
    CuAssertPtrEquals(test, task, task3);

    CuAssertIntEquals(test, 1, secretary_count_done_tasks(secretary, true));
    task = secretary_get_nth_done_task(secretary, 0, true);
    CuAssertPtrEquals(test, task, task2);
    
    secretary_free(secretary);
}

void test_secretary_archive_inbox_tasks(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task"),
         *task3 = secretary_create_task(secretary, "Create thrid task");

    task_mark_as_done(task2);

    CuAssertIntEquals(test, 3, secretary_count_inbox_tasks(secretary, false));
    Task *task = secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = secretary_get_nth_inbox_task(secretary, 1, false);
    CuAssertPtrEquals(test, task, task2);
    task = secretary_get_nth_inbox_task(secretary, 2, false);
    CuAssertPtrEquals(test, task, task3);

    secretary_archive_inbox_tasks(secretary);

    CuAssertIntEquals(test, 2, secretary_count_inbox_tasks(secretary, false));
    task = secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = secretary_get_nth_inbox_task(secretary, 1, false);
    CuAssertPtrEquals(test, task, task3);

    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, true));
    task = secretary_get_nth_inbox_task(secretary, 0, true);
    CuAssertPtrEquals(test, task, task2);
    
    secretary_free(secretary);
}

void test_secretary_archive_lots_of_inbox_tasks(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *tasks[] = {
        secretary_create_task(secretary, "Create first task"),
        secretary_create_task(secretary, "Create snd task"),
        secretary_create_task(secretary, "Create thrid task"),
        secretary_create_task(secretary, "Create fourth task"),
        secretary_create_task(secretary, "Create fifth task"),
        secretary_create_task(secretary, "Create sixth task"),
        secretary_create_task(secretary, "Create seventh task"),
        secretary_create_task(secretary, "Create eighth task"),
        secretary_create_task(secretary, "Create nineth task"),
        secretary_create_task(secretary, "Create tenth task")
    };

    for (int i = 0; i < 10; i+= 2) {
        task_mark_as_done(tasks[i]);
    }

    secretary_archive_inbox_tasks(secretary);

    CuAssertIntEquals(test, 5, secretary_count_inbox_tasks(secretary, false));
    for (int i = 0; i < 5; i++) {
        Task *task = secretary_get_nth_inbox_task(secretary, i, false);
        CuAssertPtrEquals(test, task, tasks[i*2+1]);
        
    }

    CuAssertIntEquals(test, 5, secretary_count_inbox_tasks(secretary, true));
    for (int i = 0; i < 5; i++) {
        Task *task = secretary_get_nth_inbox_task(secretary, i, true);
        CuAssertPtrEquals(test, task, tasks[i*2]);
        
    }

    secretary_free(secretary);
}


void test_secretary_archive_scheduled_task(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task"),
         *task3 = secretary_create_task(secretary, "Create thrid task");

    time_t date1 = time(NULL);
    secretary_schedule_task(secretary, task1, date1);
    time_t date2 = date1 + 24*60*60*30;
    secretary_schedule_task(secretary, task2, date2);
    time_t date3 = date2 + 24*60*60*30;
    secretary_schedule_task(secretary, task3, date3);

    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_mark_as_done(task3);

    CuAssertIntEquals(test, 3, 
            secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            secretary_count_tasks_scheduled_for(secretary, date1, false));
    CuAssertIntEquals(test, 2, 
            secretary_count_tasks_scheduled_for(secretary, date2, false));
    CuAssertIntEquals(test, 3, 
            secretary_count_tasks_scheduled_for(secretary, date3, false));
    CuAssertIntEquals(test, 1,
            secretary_count_tasks_scheduled_for_today(secretary, false));

    Task *task = secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = secretary_get_nth_task_scheduled_for(secretary, date1, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = secretary_get_nth_task_scheduled_for_today(secretary, 0, false);
    CuAssertPtrEquals(test, task, task1);

    task = secretary_get_nth_task_scheduled(secretary, 1, false);
    CuAssertPtrEquals(test, task, task2);
    task = secretary_get_nth_task_scheduled_for(secretary, date2, 1, false);
    CuAssertPtrEquals(test, task, task2);

    task = secretary_get_nth_task_scheduled(secretary, 2, false);
    CuAssertPtrEquals(test, task, task3);
    task = secretary_get_nth_task_scheduled_for(secretary, date3, 2, false);
    CuAssertPtrEquals(test, task, task3);

    secretary_archive_tasks_scheduled_for_today(secretary);

    CuAssertIntEquals(test, 2, 
            secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 0, 
            secretary_count_tasks_scheduled_for(secretary, date1, false));
    CuAssertIntEquals(test, 0,
            secretary_count_tasks_scheduled_for_today(secretary, false));

    task = secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task, task2);
    task = secretary_get_nth_task_scheduled(secretary, 1, false);
    CuAssertPtrEquals(test, task, task3);
    task = secretary_get_nth_task_scheduled(secretary, 0, true);
    CuAssertPtrEquals(test, task, task1);
    task = secretary_get_nth_task_scheduled_for(secretary, date1, 0, true);
    CuAssertPtrEquals(test, task, task1);
    task = secretary_get_nth_task_scheduled_for_today(secretary, 0, true);
    CuAssertPtrEquals(test, task, task1);

    secretary_archive_tasks_scheduled_for(secretary, date2);

    CuAssertIntEquals(test, 1, 
            secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 0, 
            secretary_count_tasks_scheduled_for(secretary, date2, false));

    task = secretary_get_nth_task_scheduled(secretary, 0, false);
    CuAssertPtrEquals(test, task, task3);
    task = secretary_get_nth_task_scheduled_for(secretary, date2, 0, true);
    CuAssertPtrEquals(test, task, task1);
    task = secretary_get_nth_task_scheduled_for(secretary, date2, 1, true);
    CuAssertPtrEquals(test, task, task2);

    secretary_archive_scheduled_tasks(secretary);

    CuAssertIntEquals(test, 0, 
            secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 0, 
            secretary_count_tasks_scheduled_for(secretary, date3, false));

    task = secretary_get_nth_task_scheduled_for(secretary, date3, 0, true);
    CuAssertPtrEquals(test, task, task1);
    task = secretary_get_nth_task_scheduled_for(secretary, date3, 1, true);
    CuAssertPtrEquals(test, task, task2);
    task = secretary_get_nth_task_scheduled_for(secretary, date3, 2, true);
    CuAssertPtrEquals(test, task, task3);


    secretary_free(secretary);
}

void test_secretary_count_tasks_archived(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task");
         /**task3 = */secretary_create_task(secretary, "Create thrid task");

    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_archive(task2);

    CuAssertIntEquals(test, 2, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, true));
        
    secretary_free(secretary);
}

void test_secretary_count_all_tasks(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Create first task"),
         *task2 = secretary_create_task(secretary, "Create snd task");
         /**task3 = */secretary_create_task(secretary, "Create thrid task");

    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_archive(task2);

    CuAssertIntEquals(test, 2, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, true));

    CuAssertIntEquals(test, 3, secretary_count_all_tasks(secretary));

    secretary_free(secretary);
}


static void test_secretary_remove_from_project(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "task 1"),
        *task2 = secretary_create_task(secretary, "task 2"),
        *task3 = secretary_create_task(secretary, "task 3");

    Project *project = secretary_create_project(secretary, "project");
    secretary_move_task_to_project(secretary, project, task1);
    secretary_move_task_to_project(secretary, project, task3);

    CuAssertIntEquals(test, 2, project_count_tasks(project, false));
    CuAssertPtrEquals(test, task1, project_get_nth_task(project, 0, false));
    CuAssertPtrEquals(test, task3, project_get_nth_task(project, 1, false));

    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));
    CuAssertPtrEquals(test, task2, 
            secretary_get_nth_inbox_task(secretary, 0, false));

    secretary_remove_task_from_project(secretary, task1);

    CuAssertIntEquals(test, 1, project_count_tasks(project, false));
    CuAssertPtrEquals(test, task3, project_get_nth_task(project, 0, false));

    CuAssertIntEquals(test, 2, secretary_count_inbox_tasks(secretary, false));
    CuAssertPtrEquals(test, task1, 
            secretary_get_nth_inbox_task(secretary, 0, false));
    CuAssertPtrEquals(test, task2, 
            secretary_get_nth_inbox_task(secretary, 1, false));

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
    return suite;
}

