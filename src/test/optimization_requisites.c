
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
#include <secretary/test/optimization_requisites.h>
#include <secretary/_internal/secretary.h>
#include <secretary/_internal/task.h>

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
 * Ensures that a secretary creates tasks putting them in inbox perspective and
 * remove them from it when it is the case (the task is scheduled and/or
 * added to a project.
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
 * Ensures that a secretary moves a task to scheduled perspective when it is
 * scheduled.
 */
static void test_optimization_requisites_scheduled_perspective(CuTest *test) {
    Secretary *secretary = secretary_new();

    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.archived_tasks));

    Task *task = secretary_create_task(secretary, "task 1");
    // All in inbox
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrEquals(test, task, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 0));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.archived_tasks));
    // Scheduled task not in inbox anymore
    time_t now = time(NULL);
    task_schedule(task, *localtime(&now));
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));
    CuAssertPtrEquals(test, task, 
            list_get_nth_item(secretary->scheduled_perspective.visible_tasks, 0));
    // Project-related task nt in inbox anymore
    Project *project = secretary_create_project(secretary, "project");
    project_add_task(project, task);
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));
    CuAssertPtrEquals(test, task, 
            list_get_nth_item(secretary->scheduled_perspective.visible_tasks, 0));
    // Removed from project - returns to inbox
    project_remove_task(project, task);
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));
    CuAssertPtrEquals(test, task, 
            list_get_nth_item(secretary->scheduled_perspective.visible_tasks, 0));

    task_unschedule(task);
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));

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

/**
 * Ensures that a secretary manages correctly the archival of a task in inbox.
 */
static void test_optimization_requisites_inbox_archived(CuTest *test) {
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

    // Do some task
    task_mark_as_done(task1);
    task_mark_as_done(task2);
    // Nothing changes
    CuAssertIntEquals(test, 2, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrEquals(test, task1, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 0));
    CuAssertPtrEquals(test, task2, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 1));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.archived_tasks));

    // Archive this tasks
    task_archive(task1);
    // One less task in visible tasks
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrEquals(test, task2, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 0));
    // One task in archived tasks
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->inbox_perspective.archived_tasks));
    CuAssertPtrEquals(test, task1, 
            list_get_nth_item(secretary->inbox_perspective.archived_tasks, 0));

    // Undo task
    task_unmark_as_done(task1);
    // Returns to initial configuration... except that tasks change order.
    CuAssertIntEquals(test, 2, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrEquals(test, task2, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 0));
    CuAssertPtrEquals(test, task1, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 1));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.archived_tasks));
    
    secretary_free(secretary);
}

/**
 * Ensures that a secretary creates tasks putting them in inbox perspective and
 * remove them from it when it is the case (the task is scheduled and/or
 * added to a project.
 */
static void test_optimization_requisites_switch_list(CuTest *test) {
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
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.archived_tasks));
    // Scheduled task not in inbox anymore
    time_t now = time(NULL);
    task_schedule(task1, *localtime(&now));
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrEquals(test, task2, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 0));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.archived_tasks));
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));
    CuAssertPtrEquals(test, task1, 
            list_get_nth_item(secretary->scheduled_perspective.visible_tasks, 0));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.archived_tasks));

    // Archive tasks
    task1->done = task1->archived = true;
    task2->done = task2->archived = true;
    // Now, switch
    _secretary_switch_list_in_inbox_perspective(secretary, task2);
    _secretary_switch_list_in_scheduled_perspective(secretary, task1);

    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->inbox_perspective.archived_tasks));
    CuAssertPtrEquals(test, task2, 
            list_get_nth_item(secretary->inbox_perspective.archived_tasks, 0));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->scheduled_perspective.archived_tasks));
    CuAssertPtrEquals(test, task1, 
            list_get_nth_item(secretary->scheduled_perspective.archived_tasks, 0));

    // Now returns to previous state...
    task1->archived = false;
    task2->archived = false;
    // ...and switch again
    _secretary_switch_list_in_inbox_perspective(secretary, task2);
    _secretary_switch_list_in_scheduled_perspective(secretary, task1);
    // Should return to previous situation
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->inbox_perspective.visible_tasks));
    CuAssertPtrEquals(test, task2, 
            list_get_nth_item(secretary->inbox_perspective.visible_tasks, 0));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->inbox_perspective.archived_tasks));
    CuAssertIntEquals(test, 1, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));
    CuAssertPtrEquals(test, task1, 
            list_get_nth_item(secretary->scheduled_perspective.visible_tasks, 0));
    CuAssertIntEquals(test, 0, 
            list_count_items(secretary->scheduled_perspective.archived_tasks));

    secretary_free(secretary);
}

/**
 * Tests functions for getting lists from perspectives
 */
static void test_optimization_requisites_secretary_get_list_from_perspective(CuTest *test) {
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

    CuAssertPtrEquals(test, secretary->inbox_perspective.visible_tasks,
        _secretary_get_list_from_perspective(secretary->inbox_perspective, false));
    CuAssertPtrEquals(test, secretary->inbox_perspective.archived_tasks,
        _secretary_get_list_from_perspective(secretary->inbox_perspective, true));

    CuAssertPtrEquals(test, secretary->scheduled_perspective.visible_tasks,
        _secretary_get_list_from_perspective(secretary->scheduled_perspective, false));
    CuAssertPtrEquals(test, secretary->scheduled_perspective.archived_tasks,
        _secretary_get_list_from_perspective(secretary->scheduled_perspective, true));


    secretary_free(secretary);
}

static void test_optimization_requisites_task_compare_by_date(CuTest *test) {
    Task *task1 = task_new(0, "task 1"),
        *task2 = task_new(0, "task 2"),
        *task3 = task_new(0, "task 3");

    time_t now = time(NULL);
    struct tm date = *localtime(&now);

    task_schedule(task1, date);
    task_schedule(task2, date);
    date.tm_mday += 5;
    // Greater date
    task_schedule(task3, date);

    CuAssertIntEquals(test, 0, _task_compare_by_date(&task1, &task2));
    CuAssertTrue(test, _task_compare_by_date(&task1, &task3) < 0);
    CuAssertTrue(test, _task_compare_by_date(&task3, &task1) > 0);
}

/**
 * Ensures that the scheduled tasks are ordered by date in spite of the order
 * they are scheduled
 */
static void test_optimization_requisites_scheduled_ordered_by_date(CuTest *test) {
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "task 1"),
        *task2 = secretary_create_task(secretary, "task 2"),
        *task3 = secretary_create_task(secretary, "task 3"),
        *task4 = secretary_create_task(secretary, "task 4"),
        *task5 = secretary_create_task(secretary, "task 5");

   time_t now = time(NULL);
    struct tm date = *localtime(&now);

    date.tm_mday = 4;
    task_schedule(task4, date);
    date.tm_mday = 1;
    task_schedule(task1, date);
    date.tm_mday = 3;
    task_schedule(task3, date);
    date.tm_mday = 5;
    task_schedule(task5, date);
    date.tm_mday = 2;
    task_schedule(task2, date);

    CuAssertIntEquals(test, 5, 
            list_count_items(secretary->scheduled_perspective.visible_tasks));

    CuAssertPtrEquals(test, task1, 
            list_get_nth_item(secretary->scheduled_perspective.visible_tasks, 0));
    CuAssertPtrEquals(test, task2, 
            list_get_nth_item(secretary->scheduled_perspective.visible_tasks, 1));
    CuAssertPtrEquals(test, task3, 
            list_get_nth_item(secretary->scheduled_perspective.visible_tasks, 2));
    CuAssertPtrEquals(test, task4, 
            list_get_nth_item(secretary->scheduled_perspective.visible_tasks, 3));
    CuAssertPtrEquals(test, task5, 
            list_get_nth_item(secretary->scheduled_perspective.visible_tasks, 4));

}

/**
 * Tests list_sort() using _task_compare_by_date.
 */
static void test_optimization_requisites_list_sort_task_by_date(CuTest *test) {
    Task *task1 = task_new(0, "task 1"),
         *task2 = task_new(0, "task 2"),
         *task3 = task_new(0, "task 3"),
         *task4 = task_new(0, "task 4"),
         *task5 = task_new(0, "task 5");

    time_t now = time(NULL);
    struct tm date = *localtime(&now);

    List *list = list_new();
    date.tm_mday = 4;
    task_schedule(task4, date);
    list_add_item(list, task4);

    date.tm_mday = 1;
    task_schedule(task1, date);
    list_add_item(list, task1);
    
    date.tm_mday = 3;
    task_schedule(task3, date);
    list_add_item(list, task3);
    
    date.tm_mday = 5;
    task_schedule(task5, date);
    list_add_item(list, task5);
    
    date.tm_mday = 2;
    task_schedule(task2, date);
    list_add_item(list, task2);

    CuAssertPtrEquals(test, task4, list_get_nth_item(list, 0));
    CuAssertPtrEquals(test, task1, list_get_nth_item(list, 1));
    CuAssertPtrEquals(test, task3, list_get_nth_item(list, 2));
    CuAssertPtrEquals(test, task5, list_get_nth_item(list, 3));
    CuAssertPtrEquals(test, task2, list_get_nth_item(list, 4));

    list_sort(list, _task_compare_by_date);

    CuAssertPtrEquals(test, task1, list_get_nth_item(list, 0));
    CuAssertPtrEquals(test, task2, list_get_nth_item(list, 1));
    CuAssertPtrEquals(test, task3, list_get_nth_item(list, 2));
    CuAssertPtrEquals(test, task4, list_get_nth_item(list, 3));
    CuAssertPtrEquals(test, task5, list_get_nth_item(list, 4));

    list_free(list);
    task_free(task1);
    task_free(task2);
    task_free(task3);
    task_free(task4);
    task_free(task5);
}

/**
 * Assures that the list of scheduled tasks is sorted when a task has its date
 * changed.
 */
static void test_optimization_requisites_task_set_date_sort_list(CuTest *test) {
 Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "task 1"),
        *task2 = secretary_create_task(secretary, "task 2"),
        *task3 = secretary_create_task(secretary, "task 3"),
        *task4 = secretary_create_task(secretary, "task 4"),
        *task5 = secretary_create_task(secretary, "task 5");

    time_t now = time(NULL);
    struct tm date = *localtime(&now);

    List *list = list_new();
    date.tm_mday = 2;
    task_schedule(task1, date);

    date.tm_mday = 3;
    task_schedule(task2, date);
    
    date.tm_mday = 4;
    task_schedule(task3, date);
    
    date.tm_mday = 5;
    task_schedule(task4, date);
    
    date.tm_mday = 7;
    task_schedule(task5, date);
    list_add_item(list, task5);

    CuAssertPtrEquals(test, task1, secretary_get_nth_task_scheduled(secretary, 0, false));
    CuAssertPtrEquals(test, task2, secretary_get_nth_task_scheduled(secretary, 1, false));
    CuAssertPtrEquals(test, task3, secretary_get_nth_task_scheduled(secretary, 2, false));
    CuAssertPtrEquals(test, task4, secretary_get_nth_task_scheduled(secretary, 3, false));
    CuAssertPtrEquals(test, task5, secretary_get_nth_task_scheduled(secretary, 4, false));

    date.tm_mday = 10;
    task_schedule(task3, date);

    CuAssertPtrEquals(test, task1, secretary_get_nth_task_scheduled(secretary, 0, false));
    CuAssertPtrEquals(test, task2, secretary_get_nth_task_scheduled(secretary, 1, false));
    CuAssertPtrEquals(test, task4, secretary_get_nth_task_scheduled(secretary, 2, false));
    CuAssertPtrEquals(test, task5, secretary_get_nth_task_scheduled(secretary, 3, false));
    CuAssertPtrEquals(test, task3, secretary_get_nth_task_scheduled(secretary, 4, false));

    date.tm_mday = 1;
    task_schedule(task5, date);

    CuAssertPtrEquals(test, task5, secretary_get_nth_task_scheduled(secretary, 0, false));
    CuAssertPtrEquals(test, task1, secretary_get_nth_task_scheduled(secretary, 1, false));
    CuAssertPtrEquals(test, task2, secretary_get_nth_task_scheduled(secretary, 2, false));
    CuAssertPtrEquals(test, task4, secretary_get_nth_task_scheduled(secretary, 3, false));
    CuAssertPtrEquals(test, task3, secretary_get_nth_task_scheduled(secretary, 4, false));

    date.tm_mday = 6;
    task_schedule(task1, date);

    CuAssertPtrEquals(test, task5, secretary_get_nth_task_scheduled(secretary, 0, false));
    CuAssertPtrEquals(test, task2, secretary_get_nth_task_scheduled(secretary, 1, false));
    CuAssertPtrEquals(test, task4, secretary_get_nth_task_scheduled(secretary, 2, false));
    CuAssertPtrEquals(test, task1, secretary_get_nth_task_scheduled(secretary, 3, false));
    CuAssertPtrEquals(test, task3, secretary_get_nth_task_scheduled(secretary, 4, false));


    list_free(list);
    task_free(task1);
    task_free(task2);
    task_free(task3);
    task_free(task4);
    task_free(task5);
}

CuSuite *test_optimization_requisites_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_optimization_requisites_task_points_secretary);
    SUITE_ADD_TEST(suite, test_optimization_requisites_secretary_perspectives);
    SUITE_ADD_TEST(suite, test_optimization_requisites_inbox_perspective);
    SUITE_ADD_TEST(suite, test_optimization_requisites_scheduled_perspective);
    SUITE_ADD_TEST(suite, test_optimization_requisites_register_in_inbox);
    SUITE_ADD_TEST(suite, test_optimization_requisites_register_archived_in_inbox);
    SUITE_ADD_TEST(suite, test_optimization_requisites_do_not_go_to_inbox);
    SUITE_ADD_TEST(suite, test_optimization_requisites_register_in_scheduled);
    SUITE_ADD_TEST(suite, 
            test_optimization_requisites_register_archived_in_scheduled);
    SUITE_ADD_TEST(suite, test_optimization_requisites_inbox_archived);
    SUITE_ADD_TEST(suite, test_optimization_requisites_switch_list);
    SUITE_ADD_TEST(suite, 
            test_optimization_requisites_secretary_get_list_from_perspective);
    SUITE_ADD_TEST(suite, test_optimization_requisites_task_compare_by_date);
    SUITE_ADD_TEST(suite, 
        test_optimization_requisites_scheduled_ordered_by_date);
    SUITE_ADD_TEST(suite, test_optimization_requisites_list_sort_task_by_date);
    SUITE_ADD_TEST(suite, test_optimization_requisites_task_set_date_sort_list);
    return suite;
}
