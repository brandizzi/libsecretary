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
#include <secretary/_internal/project.h>
#include <secretary/_internal/task.h>

/**
 * Assures that the list of scheduled tasks is sorted when a task has its date
 * changed.
 */
static void test_optimization_requisites_task_set_date_sort_list(CuTest *test) {
 SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "task 1"),
        *task2 = sct_secretary_create_task(secretary, "task 2"),
        *task3 = sct_secretary_create_task(secretary, "task 3"),
        *task4 = sct_secretary_create_task(secretary, "task 4"),
        *task5 = sct_secretary_create_task(secretary, "task 5");

    time_t now = time(NULL);
    struct tm date = *localtime(&now);

    SctList *list = sct_list_new();
    date.tm_mday = 2;
    sct_secretary_schedule_task(secretary, task1, timegm(&date));

    date.tm_mday = 3;
    sct_secretary_schedule_task(secretary, task2, timegm(&date));
    
    date.tm_mday = 4;
    sct_secretary_schedule_task(secretary, task3, timegm(&date));
    
    date.tm_mday = 5;
    sct_secretary_schedule_task(secretary, task4, timegm(&date));
    
    date.tm_mday = 7;
    sct_secretary_schedule_task(secretary, task5, timegm(&date));
    sct_list_add_item(list, task5);

    CuAssertPtrEquals(test, task1, sct_secretary_get_nth_task_scheduled(secretary, 0, false));
    CuAssertPtrEquals(test, task2, sct_secretary_get_nth_task_scheduled(secretary, 1, false));
    CuAssertPtrEquals(test, task3, sct_secretary_get_nth_task_scheduled(secretary, 2, false));
    CuAssertPtrEquals(test, task4, sct_secretary_get_nth_task_scheduled(secretary, 3, false));
    CuAssertPtrEquals(test, task5, sct_secretary_get_nth_task_scheduled(secretary, 4, false));

    date.tm_mday = 10;
    sct_secretary_schedule_task(secretary, task3, timegm(&date));

    CuAssertPtrEquals(test, task1, sct_secretary_get_nth_task_scheduled(secretary, 0, false));
    CuAssertPtrEquals(test, task2, sct_secretary_get_nth_task_scheduled(secretary, 1, false));
    CuAssertPtrEquals(test, task4, sct_secretary_get_nth_task_scheduled(secretary, 2, false));
    CuAssertPtrEquals(test, task5, sct_secretary_get_nth_task_scheduled(secretary, 3, false));
    CuAssertPtrEquals(test, task3, sct_secretary_get_nth_task_scheduled(secretary, 4, false));

    date.tm_mday = 1;
    sct_secretary_schedule_task(secretary, task5, timegm(&date));

    CuAssertPtrEquals(test, task5, sct_secretary_get_nth_task_scheduled(secretary, 0, false));
    CuAssertPtrEquals(test, task1, sct_secretary_get_nth_task_scheduled(secretary, 1, false));
    CuAssertPtrEquals(test, task2, sct_secretary_get_nth_task_scheduled(secretary, 2, false));
    CuAssertPtrEquals(test, task4, sct_secretary_get_nth_task_scheduled(secretary, 3, false));
    CuAssertPtrEquals(test, task3, sct_secretary_get_nth_task_scheduled(secretary, 4, false));

    date.tm_mday = 6;
    sct_secretary_schedule_task(secretary, task1, timegm(&date));

    CuAssertPtrEquals(test, task5, sct_secretary_get_nth_task_scheduled(secretary, 0, false));
    CuAssertPtrEquals(test, task2, sct_secretary_get_nth_task_scheduled(secretary, 1, false));
    CuAssertPtrEquals(test, task4, sct_secretary_get_nth_task_scheduled(secretary, 2, false));
    CuAssertPtrEquals(test, task1, sct_secretary_get_nth_task_scheduled(secretary, 3, false));
    CuAssertPtrEquals(test, task3, sct_secretary_get_nth_task_scheduled(secretary, 4, false));


    sct_list_free(list);
    sct_task_free(task1);
    sct_task_free(task2);
    sct_task_free(task3);
    sct_task_free(task4);
    sct_task_free(task5);
}

/**
 * Ensures that the scheduled tasks are ordered by date in spite of the order
 * they are scheduled
 */
static void test_optimization_requisites_scheduled_ordered_by_date(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "task 1"),
        *task2 = sct_secretary_create_task(secretary, "task 2"),
        *task3 = sct_secretary_create_task(secretary, "task 3"),
        *task4 = sct_secretary_create_task(secretary, "task 4"),
        *task5 = sct_secretary_create_task(secretary, "task 5");

    time_t now = time(NULL);
    struct tm date = *localtime(&now);

    date.tm_mday = 4;
    sct_secretary_schedule_task(secretary, task4, timegm(&date));
    date.tm_mday = 1;
    sct_secretary_schedule_task(secretary, task1, timegm(&date));
    date.tm_mday = 3;
    sct_secretary_schedule_task(secretary, task3, timegm(&date));
    date.tm_mday = 5;
    sct_secretary_schedule_task(secretary, task5, timegm(&date));
    date.tm_mday = 2;
    sct_secretary_schedule_task(secretary, task2, timegm(&date));

    CuAssertIntEquals(test, 5, 
            sct_secretary_count_tasks_scheduled(secretary, false));

    CuAssertPtrEquals(test, task1, 
            sct_secretary_get_nth_task_scheduled(secretary, 0, false));
    CuAssertPtrEquals(test, task2, 
            sct_secretary_get_nth_task_scheduled(secretary, 1, false));
    CuAssertPtrEquals(test, task3, 
            sct_secretary_get_nth_task_scheduled(secretary, 2, false));
    CuAssertPtrEquals(test, task4, 
            sct_secretary_get_nth_task_scheduled(secretary, 3, false));
    CuAssertPtrEquals(test, task5, 
            sct_secretary_get_nth_task_scheduled(secretary, 4, false));

}

static void test_optimization_requisites_sort_tasks(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();

    SctTask *as6_task = sct_secretary_create_task(secretary, "as6_task"),
        *s6_task = sct_secretary_create_task(secretary, "s6_task"),
        *as3_task = sct_secretary_create_task(secretary, "as3_task"),
        *s3_task = sct_secretary_create_task(secretary, "s3_task"),
        *a_task = sct_secretary_create_task(secretary, "a_task"),
        *task = sct_secretary_create_task(secretary, "task");

    // Altering creation order
    s6_task->created_at += 1;
    as6_task->created_at += 2;
    s3_task->created_at += 3;
    as3_task->created_at += 4;
    task->created_at += 5;
    a_task->created_at += 6;
    
    // Scheduling by hand
    time_t scheduling_date = time(NULL);
    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    s3_task->scheduled = true; s3_task->scheduled_for = scheduling_date;
    as3_task->scheduled = true; as3_task->scheduled_for = scheduling_date;

    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    s6_task->scheduled = true; s6_task->scheduled_for = scheduling_date;
    as6_task->scheduled = true; as6_task->scheduled_for = scheduling_date;

    as6_task->done = as6_task->archived = true;
    as3_task->done = as3_task->archived = true;
    a_task->done = a_task->archived = true;

    CuAssertIntEquals(test, sct_secretary_count_all_tasks(secretary), 6);
    CuAssertPtrEquals(test, as6_task, sct_secretary_get_nth_task(secretary, 0));
    CuAssertPtrEquals(test, s6_task, sct_secretary_get_nth_task(secretary, 1));
    CuAssertPtrEquals(test, as3_task, sct_secretary_get_nth_task(secretary, 2));
    CuAssertPtrEquals(test, s3_task, sct_secretary_get_nth_task(secretary, 3));
    CuAssertPtrEquals(test, a_task, sct_secretary_get_nth_task(secretary, 4));
    CuAssertPtrEquals(test, task, sct_secretary_get_nth_task(secretary, 5));

    _secretary_sort_tasks(secretary);
    
    CuAssertIntEquals(test, sct_secretary_count_all_tasks(secretary), 6);
    CuAssertPtrEquals(test, s3_task, sct_secretary_get_nth_task(secretary, 0));
    CuAssertPtrEquals(test, s6_task, sct_secretary_get_nth_task(secretary, 1));
    CuAssertPtrEquals(test, task, sct_secretary_get_nth_task(secretary, 2));
    CuAssertPtrEquals(test, as3_task, sct_secretary_get_nth_task(secretary, 3));
    CuAssertPtrEquals(test, as6_task, sct_secretary_get_nth_task(secretary, 4));
    CuAssertPtrEquals(test, a_task, sct_secretary_get_nth_task(secretary, 5));


    sct_secretary_free(secretary);
}

static void test_optimization_requisites_project_sort_tasks(CuTest *test) {
        SctSecretary *secretary = sct_secretary_new();

    SctTask *as6_task = sct_secretary_create_task(secretary, "as6_task"),
        *s6_task = sct_secretary_create_task(secretary, "s6_task"),
        *as3_task = sct_secretary_create_task(secretary, "as3_task"),
        *s3_task = sct_secretary_create_task(secretary, "s3_task"),
        *a_task = sct_secretary_create_task(secretary, "a_task"),
        *task = sct_secretary_create_task(secretary, "task"),
        *s6p_task = sct_secretary_create_task(secretary, "s6p_task"),
        *as6p_task = sct_secretary_create_task(secretary, "as6p_task"),
        *p_task = sct_secretary_create_task(secretary, "p_task"),
        *ap_task = sct_secretary_create_task(secretary, "ap_task");

    // Altering creation order
    s6_task->created_at += 1;
    as6_task->created_at += 2;
    s3_task->created_at += 3;
    as3_task->created_at += 4;
    task->created_at += 5;
    a_task->created_at += 6;
    s6p_task->created_at += 7;
    as6p_task->created_at += 8;
    p_task->created_at += 9;
    ap_task->created_at += 10;
    

    CuAssertIntEquals(test, sct_secretary_count_all_tasks(secretary), 10);
    CuAssertPtrEquals(test, as6_task, sct_secretary_get_nth_task(secretary, 0));
    CuAssertPtrEquals(test, s6_task, sct_secretary_get_nth_task(secretary, 1));
    CuAssertPtrEquals(test, as3_task, sct_secretary_get_nth_task(secretary, 2));
    CuAssertPtrEquals(test, s3_task, sct_secretary_get_nth_task(secretary, 3));
    CuAssertPtrEquals(test, a_task, sct_secretary_get_nth_task(secretary, 4));
    CuAssertPtrEquals(test, task, sct_secretary_get_nth_task(secretary, 5));
    CuAssertPtrEquals(test, s6p_task, sct_secretary_get_nth_task(secretary, 6));
    CuAssertPtrEquals(test, as6p_task, sct_secretary_get_nth_task(secretary, 7));
    CuAssertPtrEquals(test, p_task, sct_secretary_get_nth_task(secretary, 8));
    CuAssertPtrEquals(test, ap_task, sct_secretary_get_nth_task(secretary, 9));

    // Scheduling by hand
    time_t scheduling_date = time(NULL);
    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    sct_secretary_schedule_task(secretary, s3_task, scheduling_date);
    sct_secretary_schedule_task(secretary, as3_task, scheduling_date);

    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    s6_task->scheduled = true; s6_task->scheduled_for = scheduling_date;
    as6_task->scheduled = true; as6_task->scheduled_for = scheduling_date;
    sct_secretary_schedule_task(secretary, s6_task, scheduling_date);
    sct_secretary_schedule_task(secretary, as6_task, scheduling_date);
    sct_secretary_schedule_task(secretary, s6p_task, scheduling_date);
    sct_secretary_schedule_task(secretary, as6p_task, scheduling_date);

    // Projecting
    SctProject *project = sct_project_new("p");
    sct_secretary_move_task_to_project(secretary, project, s6p_task);
    sct_secretary_move_task_to_project(secretary, project, as6p_task);
    sct_secretary_move_task_to_project(secretary, project, p_task);
    sct_secretary_move_task_to_project(secretary, project, ap_task);

    // Archiving
    sct_task_mark_as_done(a_task); sct_secretary_archive_task(secretary, a_task);
    sct_task_mark_as_done(as3_task); sct_secretary_archive_task(secretary, as3_task);
    sct_task_mark_as_done(as6_task); sct_secretary_archive_task(secretary, as6_task);
    sct_task_mark_as_done(as6p_task); sct_secretary_archive_task(secretary, as6p_task);
    sct_task_mark_as_done(ap_task); sct_secretary_archive_task(secretary, ap_task);
    
    CuAssertIntEquals(test, 10, sct_secretary_count_all_tasks(secretary));
    CuAssertPtrEquals(test, s3_task, sct_secretary_get_nth_task(secretary, 0));
    CuAssertPtrEquals(test, s6_task, sct_secretary_get_nth_task(secretary, 1));
    // Because was created later. SctProject does not influence
    CuAssertPtrEquals(test, s6p_task, sct_secretary_get_nth_task(secretary, 2));
    CuAssertPtrEquals(test, p_task, sct_secretary_get_nth_task(secretary, 3));
    CuAssertPtrEquals(test, task, sct_secretary_get_nth_task(secretary, 4));
    CuAssertPtrEquals(test, as3_task, sct_secretary_get_nth_task(secretary, 5));
    CuAssertPtrEquals(test, as6_task, sct_secretary_get_nth_task(secretary, 6));
    CuAssertPtrEquals(test, as6p_task, sct_secretary_get_nth_task(secretary, 7));
    CuAssertPtrEquals(test, ap_task, sct_secretary_get_nth_task(secretary, 8));
    CuAssertPtrEquals(test, a_task, sct_secretary_get_nth_task(secretary, 9));


    sct_secretary_free(secretary);
}

static void test_optimization_requisites_secretary_has_sublists(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();

    CuAssertTrue(test, secretary->visible_scheduled_tasks != NULL);
    CuAssertIntEquals(test, 0, 
            sct_list_count_items(secretary->visible_scheduled_tasks));
    CuAssertTrue(test, secretary->visible_scheduled_for_today_tasks != NULL);
    CuAssertIntEquals(test, 0, 
            sct_list_count_items(secretary->visible_scheduled_for_today_tasks));
    CuAssertTrue(test, secretary->visible_inbox != NULL);
    CuAssertIntEquals(test, 0, sct_list_count_items(secretary->visible_inbox));

    sct_secretary_free(secretary);
}

static void test_optimization_requisites_secretary_has_archived_sublists(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();

    CuAssertTrue(test, secretary->archived_scheduled_tasks != NULL);
    CuAssertIntEquals(test, 0, 
            sct_list_count_items(secretary->archived_scheduled_tasks));
    CuAssertTrue(test, secretary->archived_scheduled_for_today_tasks != NULL);
    CuAssertIntEquals(test, 0, 
            sct_list_count_items(secretary->archived_scheduled_for_today_tasks));
    CuAssertTrue(test, secretary->archived_inbox != NULL);
    CuAssertIntEquals(test, 0, sct_list_count_items(secretary->archived_inbox));

    sct_secretary_free(secretary);
}

static void test_optimization_requisites_secretary_has_archived_and_visible_sublists(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();

    CuAssertTrue(test, secretary->archived_tasks != NULL);
    CuAssertIntEquals(test, 0, sct_list_count_items(secretary->archived_tasks));
    CuAssertTrue(test, secretary->visible_tasks != NULL);
    CuAssertIntEquals(test, 0, sct_list_count_items(secretary->visible_tasks));

    sct_secretary_free(secretary);
}

static void test_optimization_requisites_sublists_work(CuTest *test) {
        SctSecretary *secretary = sct_secretary_new();

    SctTask *as6_task = sct_secretary_create_task(secretary, "as6_task"),
        *s6_task = sct_secretary_create_task(secretary, "s6_task"),
        *as3_task = sct_secretary_create_task(secretary, "as3_task"),
        *s3_task = sct_secretary_create_task(secretary, "s3_task"),
        *a_task = sct_secretary_create_task(secretary, "a_task"),
        *task = sct_secretary_create_task(secretary, "task"),
        *s6p_task = sct_secretary_create_task(secretary, "s6p_task"),
        *as6p_task = sct_secretary_create_task(secretary, "as6p_task"),
        *p_task = sct_secretary_create_task(secretary, "p_task"),
        *ap_task = sct_secretary_create_task(secretary, "ap_task");

    // Altering creation order
    s6_task->created_at += 1;
    as6_task->created_at += 2;
    s3_task->created_at += 3;
    as3_task->created_at += 4;
    task->created_at += 5;
    a_task->created_at += 6;
    s6p_task->created_at += 7;
    as6p_task->created_at += 8;
    p_task->created_at += 9;
    ap_task->created_at += 10;

    // Scheduling by hand
    time_t scheduling_date = time(NULL);
    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    sct_secretary_schedule_task(secretary, s3_task, scheduling_date);
    sct_secretary_schedule_task(secretary, as3_task, scheduling_date);

    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    s6_task->scheduled = true; s6_task->scheduled_for = scheduling_date;
    as6_task->scheduled = true; as6_task->scheduled_for = scheduling_date;
    sct_secretary_schedule_task(secretary, s6_task, scheduling_date);
    sct_secretary_schedule_task(secretary, as6_task, scheduling_date);
    sct_secretary_schedule_task(secretary, s6p_task, scheduling_date);
    sct_secretary_schedule_task(secretary, as6p_task, scheduling_date);

    // Projecting
    SctProject *project = sct_project_new("p");
    sct_secretary_move_task_to_project(secretary, project, s6p_task);
    sct_secretary_move_task_to_project(secretary, project, as6p_task);
    sct_secretary_move_task_to_project(secretary, project, p_task);
    sct_secretary_move_task_to_project(secretary, project, ap_task);

    // Archiving
    sct_task_mark_as_done(a_task); sct_secretary_archive_task(secretary, a_task);
    sct_task_mark_as_done(as3_task); sct_secretary_archive_task(secretary, as3_task);
    sct_task_mark_as_done(as6_task); sct_secretary_archive_task(secretary, as6_task);
    sct_task_mark_as_done(as6p_task); sct_secretary_archive_task(secretary, as6p_task);
    sct_task_mark_as_done(ap_task); sct_secretary_archive_task(secretary, ap_task);

    CuAssertIntEquals(test, 1, sct_list_count_items(secretary->visible_inbox));
    CuAssertPtrEquals(test, task, 
            sct_list_get_nth_item(secretary->visible_inbox, 0));
    CuAssertIntEquals(test, 3, 
            sct_list_count_items(secretary->visible_scheduled_tasks));
    CuAssertPtrEquals(test, s3_task, 
            sct_list_get_nth_item(secretary->visible_scheduled_tasks, 0));
    CuAssertPtrEquals(test, s6_task, 
            sct_list_get_nth_item(secretary->visible_scheduled_tasks, 1));
    CuAssertPtrEquals(test, s6p_task, 
            sct_list_get_nth_item(secretary->visible_scheduled_tasks, 2));

    sct_task_mark_as_done(task); sct_secretary_archive_task(secretary, task);
    CuAssertIntEquals(test, 0, sct_list_count_items(secretary->visible_inbox));

    sct_secretary_free(secretary);
}

static void test_optimization_requisites_sublists_work_for_archived(CuTest *test) {
        SctSecretary *secretary = sct_secretary_new();

    SctTask *as6_task = sct_secretary_create_task(secretary, "as6_task"),
        *s6_task = sct_secretary_create_task(secretary, "s6_task"),
        *as3_task = sct_secretary_create_task(secretary, "as3_task"),
        *s3_task = sct_secretary_create_task(secretary, "s3_task"),
        *a_task = sct_secretary_create_task(secretary, "a_task"),
        *task = sct_secretary_create_task(secretary, "task"),
        *s6p_task = sct_secretary_create_task(secretary, "s6p_task"),
        *as6p_task = sct_secretary_create_task(secretary, "as6p_task"),
        *p_task = sct_secretary_create_task(secretary, "p_task"),
        *ap_task = sct_secretary_create_task(secretary, "ap_task");

    // Altering creation order
    s6_task->created_at += 1;
    as6_task->created_at += 2;
    s3_task->created_at += 3;
    as3_task->created_at += 4;
    task->created_at += 5;
    a_task->created_at += 6;
    s6p_task->created_at += 7;
    as6p_task->created_at += 8;
    p_task->created_at += 9;
    ap_task->created_at += 10;

    // Scheduling by hand
    time_t scheduling_date = time(NULL);
    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    sct_secretary_schedule_task(secretary, s3_task, scheduling_date);
    sct_secretary_schedule_task(secretary, as3_task, scheduling_date);

    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    s6_task->scheduled = true; s6_task->scheduled_for = scheduling_date;
    as6_task->scheduled = true; as6_task->scheduled_for = scheduling_date;
    sct_secretary_schedule_task(secretary, s6_task, scheduling_date);
    sct_secretary_schedule_task(secretary, as6_task, scheduling_date);
    sct_secretary_schedule_task(secretary, s6p_task, scheduling_date);
    sct_secretary_schedule_task(secretary, as6p_task, scheduling_date);

    // Projecting
    SctProject *project = sct_project_new("p");
    sct_secretary_move_task_to_project(secretary, project, s6p_task);
    sct_secretary_move_task_to_project(secretary, project, as6p_task);
    sct_secretary_move_task_to_project(secretary, project, p_task);
    sct_secretary_move_task_to_project(secretary, project, ap_task);

    // Archiving
    sct_task_mark_as_done(a_task); sct_secretary_archive_task(secretary, a_task);
    sct_task_mark_as_done(as3_task); sct_secretary_archive_task(secretary, as3_task);
    sct_task_mark_as_done(as6_task); sct_secretary_archive_task(secretary, as6_task);
    sct_task_mark_as_done(as6p_task); sct_secretary_archive_task(secretary, as6p_task);
    sct_task_mark_as_done(ap_task); sct_secretary_archive_task(secretary, ap_task);

    CuAssertIntEquals(test, 1, sct_list_count_items(secretary->archived_inbox));
    CuAssertPtrEquals(test, a_task, 
            sct_list_get_nth_item(secretary->archived_inbox, 0));
    CuAssertIntEquals(test, 3, 
            sct_list_count_items(secretary->archived_scheduled_tasks));
    CuAssertPtrEquals(test, as3_task, 
            sct_list_get_nth_item(secretary->archived_scheduled_tasks, 0));
    CuAssertPtrEquals(test, as6_task, 
            sct_list_get_nth_item(secretary->archived_scheduled_tasks, 1));
    CuAssertPtrEquals(test, as6p_task, 
            sct_list_get_nth_item(secretary->archived_scheduled_tasks, 2));

    sct_task_mark_as_done(task); sct_secretary_archive_task(secretary, task);
    CuAssertIntEquals(test, 0, sct_list_count_items(secretary->visible_inbox));

    sct_secretary_free(secretary);
}

static void test_optimization_requisites_sublists_work_for_archived_and_visible(CuTest *test) {
        SctSecretary *secretary = sct_secretary_new();

    SctTask *as6_task = sct_secretary_create_task(secretary, "as6_task"),
        *s6_task = sct_secretary_create_task(secretary, "s6_task"),
        *as3_task = sct_secretary_create_task(secretary, "as3_task"),
        *s3_task = sct_secretary_create_task(secretary, "s3_task"),
        *a_task = sct_secretary_create_task(secretary, "a_task"),
        *task = sct_secretary_create_task(secretary, "task"),
        *s6p_task = sct_secretary_create_task(secretary, "s6p_task"),
        *as6p_task = sct_secretary_create_task(secretary, "as6p_task"),
        *p_task = sct_secretary_create_task(secretary, "p_task"),
        *ap_task = sct_secretary_create_task(secretary, "ap_task");

    // Altering creation order
    s6_task->created_at += 1;
    as6_task->created_at += 2;
    s3_task->created_at += 3;
    as3_task->created_at += 4;
    task->created_at += 5;
    a_task->created_at += 6;
    s6p_task->created_at += 7;
    as6p_task->created_at += 8;
    p_task->created_at += 9;
    ap_task->created_at += 10;

    // Scheduling by hand
    time_t scheduling_date = time(NULL);
    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    sct_secretary_schedule_task(secretary, s3_task, scheduling_date);
    sct_secretary_schedule_task(secretary, as3_task, scheduling_date);

    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    s6_task->scheduled = true; s6_task->scheduled_for = scheduling_date;
    as6_task->scheduled = true; as6_task->scheduled_for = scheduling_date;
    sct_secretary_schedule_task(secretary, s6_task, scheduling_date);
    sct_secretary_schedule_task(secretary, as6_task, scheduling_date);
    sct_secretary_schedule_task(secretary, s6p_task, scheduling_date);
    sct_secretary_schedule_task(secretary, as6p_task, scheduling_date);

    // Projecting
    SctProject *project = sct_project_new("p");
    sct_secretary_move_task_to_project(secretary, project, s6p_task);
    sct_secretary_move_task_to_project(secretary, project, as6p_task);
    sct_secretary_move_task_to_project(secretary, project, p_task);
    sct_secretary_move_task_to_project(secretary, project, ap_task);

    // Archiving
    sct_task_mark_as_done(a_task); sct_secretary_archive_task(secretary, a_task);
    sct_task_mark_as_done(as3_task); sct_secretary_archive_task(secretary, as3_task);
    sct_task_mark_as_done(as6_task); sct_secretary_archive_task(secretary, as6_task);
    sct_task_mark_as_done(as6p_task); sct_secretary_archive_task(secretary, as6p_task);
    sct_task_mark_as_done(ap_task); sct_secretary_archive_task(secretary, ap_task);

    CuAssertIntEquals(test, 5, sct_list_count_items(secretary->visible_tasks));
    CuAssertPtrEquals(test, s3_task, 
            sct_list_get_nth_item(secretary->visible_tasks, 0));
    CuAssertPtrEquals(test, s6_task, sct_list_get_nth_item(secretary->visible_tasks,  1));
    // Because was created later. SctProject does not influence
    CuAssertPtrEquals(test, s6p_task, sct_list_get_nth_item(secretary->visible_tasks,  2));
    CuAssertPtrEquals(test, p_task, sct_list_get_nth_item(secretary->visible_tasks,  3));
    CuAssertPtrEquals(test, task, sct_list_get_nth_item(secretary->visible_tasks,  4));

    CuAssertIntEquals(test, 5, sct_list_count_items(secretary->archived_tasks));
    CuAssertPtrEquals(test, as3_task, sct_list_get_nth_item(secretary->archived_tasks,  0));
    CuAssertPtrEquals(test, as6_task, sct_list_get_nth_item(secretary->archived_tasks,  1));
    CuAssertPtrEquals(test, as6p_task, sct_list_get_nth_item(secretary->archived_tasks,  2));
    CuAssertPtrEquals(test, ap_task, sct_list_get_nth_item(secretary->archived_tasks,  3));
    CuAssertPtrEquals(test, a_task, sct_list_get_nth_item(secretary->archived_tasks,  4));

    sct_secretary_free(secretary);
}

// PROJ
static void test_optimization_requisites_sort_tasks_in_projects(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();

    SctTask *as6_task = sct_secretary_create_task(secretary, "as6_task"),
        *s6_task = sct_secretary_create_task(secretary, "s6_task"),
        *as3_task = sct_secretary_create_task(secretary, "as3_task"),
        *s3_task = sct_secretary_create_task(secretary, "s3_task"),
        *a_task = sct_secretary_create_task(secretary, "a_task"),
        *task = sct_secretary_create_task(secretary, "task");

    SctProject *project = sct_project_new("project");
    sct_project_add_task(project, as6_task);
    sct_project_add_task(project, s6_task);
    sct_project_add_task(project, as3_task);
    sct_project_add_task(project, s3_task);
    sct_project_add_task(project, a_task);
    sct_project_add_task(project, task);

    // Altering creation order
    s6_task->created_at += 1;
    as6_task->created_at += 2;
    s3_task->created_at += 3;
    as3_task->created_at += 4;
    task->created_at += 5;
    a_task->created_at += 6;
    
    // Scheduling by hand
    time_t scheduling_date = time(NULL);
    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    s3_task->scheduled = true; s3_task->scheduled_for = scheduling_date;
    as3_task->scheduled = true; as3_task->scheduled_for = scheduling_date;

    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    s6_task->scheduled = true; s6_task->scheduled_for = scheduling_date;
    as6_task->scheduled = true; as6_task->scheduled_for = scheduling_date;

    as6_task->done = as6_task->archived = true;
    as3_task->done = as3_task->archived = true;
    a_task->done = a_task->archived = true;


    CuAssertIntEquals(test, 3, sct_project_count_tasks(project, false));
    CuAssertIntEquals(test, 3, sct_project_count_tasks(project, true));
    CuAssertPtrEquals(test, as6_task, sct_list_get_nth_item(project->tasks, 0));
    CuAssertPtrEquals(test, s6_task, sct_list_get_nth_item(project->tasks, 1));
    CuAssertPtrEquals(test, as3_task, sct_list_get_nth_item(project->tasks, 2));
    CuAssertPtrEquals(test, s3_task, sct_list_get_nth_item(project->tasks, 3));
    CuAssertPtrEquals(test, a_task, sct_list_get_nth_item(project->tasks, 4));
    CuAssertPtrEquals(test, task, sct_list_get_nth_item(project->tasks, 5));

    _project_sort_tasks(project);
    
    CuAssertIntEquals(test, 3, sct_project_count_tasks(project, false));
    CuAssertIntEquals(test, 3, sct_project_count_tasks(project, true));
    CuAssertPtrEquals(test, s3_task, sct_list_get_nth_item(project->tasks, 0));
    CuAssertPtrEquals(test, s6_task, sct_list_get_nth_item(project->tasks, 1));
    CuAssertPtrEquals(test, task, sct_list_get_nth_item(project->tasks, 2));
    CuAssertPtrEquals(test, as3_task, sct_list_get_nth_item(project->tasks, 3));
    CuAssertPtrEquals(test, as6_task, sct_list_get_nth_item(project->tasks, 4));
    CuAssertPtrEquals(test, a_task, sct_list_get_nth_item(project->tasks, 5));


    sct_secretary_free(secretary);
}

CuSuite *test_optimization_requisites_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_optimization_requisites_task_set_date_sort_list);
    SUITE_ADD_TEST(suite, test_optimization_requisites_sort_tasks);
    SUITE_ADD_TEST(suite, test_optimization_requisites_project_sort_tasks);
    SUITE_ADD_TEST(suite, test_optimization_requisites_scheduled_ordered_by_date);
    SUITE_ADD_TEST(suite, test_optimization_requisites_secretary_has_sublists);
    SUITE_ADD_TEST(suite, test_optimization_requisites_sublists_work);
    SUITE_ADD_TEST(suite, 
            test_optimization_requisites_secretary_has_archived_sublists);
    SUITE_ADD_TEST(suite, 
            test_optimization_requisites_sublists_work_for_archived);
    SUITE_ADD_TEST(suite, 
            test_optimization_requisites_secretary_has_archived_and_visible_sublists);
    SUITE_ADD_TEST(suite, 
            test_optimization_requisites_sublists_work_for_archived_and_visible);
    SUITE_ADD_TEST(suite, test_optimization_requisites_sort_tasks_in_projects);

    return suite;
}


