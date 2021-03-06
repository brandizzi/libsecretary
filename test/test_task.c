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
#include <secretary/test/task.h>
#include <secretary/project.h>
#include <stdlib.h>

static void test_task_create(CuTest *test) {
    SctTask *task = sct_task_new("Do something" );
    CuAssertStrEquals(test, "Do something", sct_task_get_description(task));

    sct_task_free(task);
}

static void test_task_alter_description(CuTest *test) {
    SctTask *task = sct_task_new("Do something" );
    CuAssertStrEquals(test, "Do something", sct_task_get_description(task));
    sct_task_set_description(task, "Do something else");
    CuAssertStrEquals(test, "Do something else", sct_task_get_description(task));
    
    sct_task_free(task);
}

static void test_task_remove_from_project(CuTest *test) {
    SctTask *task = sct_task_new("Do something" );
    SctProject *project = sct_project_new("libsecretary");
    CuAssertPtrEquals(test, sct_task_get_project(task), NULL);

    sct_task_set_project(task, project);
    CuAssertPtrEquals(test, sct_task_get_project(task), project);
    sct_task_unset_project(task);
    CuAssertPtrEquals(test, sct_task_get_project(task), NULL);

    sct_task_free(task);
    sct_project_free(project);
}

static void test_task_is_in_project(CuTest *test) {
    SctTask *task = sct_task_new("Do something" );
    SctProject *project = sct_project_new("libsecretary");
    CuAssertTrue(test, !sct_task_is_in_project(task, project));

    sct_task_set_project(task, project);
    CuAssertTrue(test, sct_task_is_in_project(task, project));

    sct_task_free(task);
    sct_project_free(project);
}

static void test_task_switch_done_status(CuTest *test) {
    SctTask *task = sct_task_new("Do something" );
    CuAssertTrue(test, !sct_task_is_done(task));
    sct_task_mark_as_done(task);
    CuAssertTrue(test, sct_task_is_done(task));
    sct_task_unmark_as_done(task);
    CuAssertTrue(test, !sct_task_is_done(task));
    sct_task_switch_done_status(task);
    CuAssertTrue(test, sct_task_is_done(task));
    sct_task_switch_done_status(task);
    CuAssertTrue(test, !sct_task_is_done(task));

    sct_task_free(task);
}

static void test_task_is_in_inbox(CuTest *test) {
    SctTask *task = sct_task_new("Do something" );
    CuAssertTrue(test, (sct_task_is_in_inbox(task) && !sct_task_is_archived(task)));

    sct_task_mark_as_done(task);
    CuAssertTrue(test, (sct_task_is_in_inbox(task) && !sct_task_is_archived(task)));

    sct_task_schedule(task, time(NULL));
    CuAssertTrue(test, !(sct_task_is_in_inbox(task) && !sct_task_is_archived(task)));
    sct_task_unschedule(task);
    CuAssertTrue(test, (sct_task_is_in_inbox(task) && !sct_task_is_archived(task)));

    SctProject *project = sct_project_new("libsecretary");
    sct_project_add_task(project, task);
    CuAssertTrue(test, !(sct_task_is_in_inbox(task) && !sct_task_is_archived(task)));
    sct_project_remove_task(project, task);
    CuAssertTrue(test, (sct_task_is_in_inbox(task) && !sct_task_is_archived(task)));

    sct_task_archive(task);
    CuAssertTrue(test, !(sct_task_is_in_inbox(task) && !sct_task_is_archived(task)));

    sct_project_free(project);
    sct_task_free(task);
}

static void test_task_archived_is_in_inbox(CuTest *test) {
    SctTask *task = sct_task_new("Do something" );
    CuAssertTrue(test, (sct_task_is_in_inbox(task) && !sct_task_is_archived(task)));

    CuAssertTrue(test, !(sct_task_is_in_inbox(task) && sct_task_is_archived(task)));
    sct_task_mark_as_done(task);
    CuAssertTrue(test, !(sct_task_is_in_inbox(task) && sct_task_is_archived(task)));
    sct_task_archive(task);
    CuAssertTrue(test, (sct_task_is_in_inbox(task) && sct_task_is_archived(task)));

    sct_task_schedule(task, time(NULL));
    CuAssertTrue(test, !(sct_task_is_in_inbox(task) && sct_task_is_archived(task)));
    sct_task_unschedule(task);
    CuAssertTrue(test, (sct_task_is_in_inbox(task) && sct_task_is_archived(task)));

    SctProject *project = sct_project_new("libsecretary");
    sct_project_add_task(project, task);
    CuAssertTrue(test, !(sct_task_is_in_inbox(task) && sct_task_is_archived(task)));
    sct_project_remove_task(project, task);
    CuAssertTrue(test, (sct_task_is_in_inbox(task) && sct_task_is_archived(task)));

    sct_project_free(project);
    sct_task_free(task);
}

static void test_task_archive(CuTest *test) {
    SctTask *task = sct_task_new("Do something" );
    CuAssertTrue(test, !sct_task_is_archived(task));
    sct_task_archive(task); // does nothing, task is not done
    CuAssertTrue(test, !sct_task_is_archived(task));
    sct_task_mark_as_done(task);
    sct_task_archive(task); // Now it archives
    CuAssertTrue(test, sct_task_is_archived(task));

    // Now testing unarchival - which is done by undoing the task
    sct_task_unmark_as_done(task);
    CuAssertTrue(test, !sct_task_is_archived(task));
    sct_task_mark_as_done(task); // Should not return to archives
    CuAssertTrue(test, !sct_task_is_archived(task));
    
    sct_task_free(task);
}

static void test_task_created_at(CuTest *test) {
    SctTask *task = sct_task_new("Do something" );
    time_t now = time(NULL);
    CuAssertTrue(test, now-sct_task_get_creation_date(task) < 1); // Less than 1s

    sct_task_free(task);
}

void test_task_compare_orders_by_creation_time(CuTest *test) {
    SctTask *task1 = sct_task_new("task 1"),
        *task2 = sct_task_new("task 2"),
        *task3 = sct_task_new("task 3");

    // A little help...
    task2->created_at++;
    task3->created_at += 2;
    
    CuAssertTrue(test, sct_task_compare(task1, task2) < 0);
    CuAssertTrue(test, sct_task_compare(task2, task1) > 0);

    CuAssertTrue(test, sct_task_compare(task2, task3) < 0);
    CuAssertTrue(test, sct_task_compare(task3, task2) > 0);

    CuAssertTrue(test, sct_task_compare(task1, task3) < 0);
    CuAssertTrue(test, sct_task_compare(task3, task1) > 0);

    sct_task_free(task1);
    sct_task_free(task2);
    sct_task_free(task3);
}

void test_task_compare_orders_by_scheduld_time(CuTest *test) {
    SctTask *task1 = sct_task_new("task 1"),
        *scheduled_3_task2 = sct_task_new("task 2"),
        *scheduled_6_task3 = sct_task_new("task 3"),
        *task4 = sct_task_new("task 4"),
        *scheduled_6_task5 = sct_task_new("task 5");

    // A little help...
    scheduled_3_task2->created_at++;
    scheduled_6_task3->created_at += 2;
    task4->created_at += 3;
    scheduled_6_task5->created_at += 4;

    time_t scheduling_date = time(NULL);
    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    sct_task_schedule(scheduled_3_task2, scheduling_date);

    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    sct_task_schedule(scheduled_6_task5, scheduling_date);
    sct_task_schedule(scheduled_6_task3, scheduling_date);

    // Greater priority: scheduled_3_task2
    CuAssertTrue(test, sct_task_compare(scheduled_3_task2, task1) < 0);
    CuAssertTrue(test, sct_task_compare(task1, scheduled_3_task2) > 0);
    CuAssertTrue(test, sct_task_compare(scheduled_3_task2, scheduled_6_task3) < 0);
    CuAssertTrue(test, sct_task_compare(scheduled_6_task3, scheduled_3_task2) > 0);
    CuAssertTrue(test, sct_task_compare(scheduled_3_task2, task4) < 0);
    CuAssertTrue(test, sct_task_compare(task4, scheduled_3_task2) > 0);
    CuAssertTrue(test, sct_task_compare(scheduled_3_task2, scheduled_6_task5) < 0);
    CuAssertTrue(test, sct_task_compare(scheduled_6_task5, scheduled_3_task2) > 0);

    // scheduled priority: scheduled_6_task3 and scheduled_6_task5
    CuAssertTrue(test, sct_task_compare(scheduled_6_task3, task1) < 0);
    CuAssertTrue(test, sct_task_compare(task1, scheduled_6_task3) > 0);
    CuAssertTrue(test, sct_task_compare(scheduled_6_task3, task4) < 0);
    CuAssertTrue(test, sct_task_compare(task4, scheduled_6_task3) > 0);
    CuAssertTrue(test, sct_task_compare(scheduled_6_task5, task1) < 0);
    CuAssertTrue(test, sct_task_compare(task1, scheduled_6_task5) > 0);
    CuAssertTrue(test, sct_task_compare(scheduled_6_task5, task4) < 0);
    CuAssertTrue(test, sct_task_compare(task4, scheduled_6_task5) > 0);
    // Tie break: scheduled_6_task3 < scheduled_6_task5 because older
    CuAssertTrue(test, sct_task_compare(scheduled_6_task3, scheduled_6_task5) < 0);
    CuAssertTrue(test, sct_task_compare(scheduled_6_task5, scheduled_6_task3) > 0);

    // Lesser priority: task1 and task4, since not scheduled
    // Tie break: task1 < task4 because older
    CuAssertTrue(test, sct_task_compare(task1, task4) < 0);
    CuAssertTrue(test, sct_task_compare(task4, task1) > 0);
    
    sct_task_free(task1);
    sct_task_free(scheduled_3_task2);
    sct_task_free(scheduled_6_task3);
    sct_task_free(task4);
    sct_task_free(scheduled_6_task5);
}

void test_task_compare_archived(CuTest *test) {
    SctTask *as6_task = sct_task_new("as6_task"),
        *s6_task = sct_task_new("s6_task"),
        *as3_task = sct_task_new("as3_task"),
        *s3_task = sct_task_new("s3_task"),
        *a_task = sct_task_new("a_task"),
        *task = sct_task_new("task");

    // Setting it up
    s6_task->created_at += 1;
    as6_task->created_at += 2;
    s3_task->created_at += 3;
    as3_task->created_at += 4;
    task->created_at += 5;
    a_task->created_at += 6;

    time_t scheduling_date = time(NULL);
    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    sct_task_schedule(s3_task, scheduling_date);
    sct_task_schedule(as3_task, scheduling_date);

    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    sct_task_schedule(s6_task, scheduling_date);
    sct_task_schedule(as6_task, scheduling_date);

    sct_task_mark_as_done(as6_task); 
    sct_task_archive(as6_task);
    sct_task_mark_as_done(as3_task); 
    sct_task_archive(as3_task);
    sct_task_mark_as_done(a_task); 
    sct_task_archive(a_task);

    // Scheduled archived for 6 days smaller than all non archived...
    CuAssertTrue(test, sct_task_compare(as6_task, s6_task) > 0);
    CuAssertTrue(test, sct_task_compare(s6_task, as6_task) < 0);
    CuAssertTrue(test, sct_task_compare(as6_task, s3_task) > 0);
    CuAssertTrue(test, sct_task_compare(s3_task, as6_task) < 0);
    CuAssertTrue(test, sct_task_compare(as6_task, task) > 0);
    CuAssertTrue(test, sct_task_compare(task, as6_task) < 0);
    // Smaller than archived for 3 days
    CuAssertTrue(test, sct_task_compare(as6_task, as3_task) > 0);
    CuAssertTrue(test, sct_task_compare(as3_task, as6_task) < 0);
    // But greater than archived non scheduled
    CuAssertTrue(test, sct_task_compare(as6_task, a_task) < 0);
    CuAssertTrue(test, sct_task_compare(a_task, as6_task) > 0);

    // Scheduled archived for 3 days smaller than all non archived...
    CuAssertTrue(test, sct_task_compare(as3_task, s6_task) > 0);
    CuAssertTrue(test, sct_task_compare(s6_task, as3_task) < 0);
    CuAssertTrue(test, sct_task_compare(as3_task, s3_task) > 0);
    CuAssertTrue(test, sct_task_compare(s3_task, as3_task) < 0);
    CuAssertTrue(test, sct_task_compare(as3_task, task) > 0);
    CuAssertTrue(test, sct_task_compare(task, as3_task) < 0);
    // But greater than archived non scheduled
    CuAssertTrue(test, sct_task_compare(as3_task, a_task) < 0);
    CuAssertTrue(test, sct_task_compare(a_task, as3_task) > 0);

    // Scheduled no archived  smaller than all non archived...
    CuAssertTrue(test, sct_task_compare(a_task, s6_task) > 0);
    CuAssertTrue(test, sct_task_compare(s6_task, a_task) < 0);
    CuAssertTrue(test, sct_task_compare(a_task, s3_task) > 0);
    CuAssertTrue(test, sct_task_compare(s3_task, a_task) < 0);
    CuAssertTrue(test, sct_task_compare(a_task, task) > 0);
    CuAssertTrue(test, sct_task_compare(task, a_task) < 0);

    sct_task_free(as6_task);
    sct_task_free(s6_task);
    sct_task_free(as3_task);
    sct_task_free(s3_task);
    sct_task_free(a_task);
    sct_task_free(task);

}

void test_task_compare_orders_by_number(CuTest *test) {
    SctTask *task3 = sct_task_new("task 1"),
        *task2 = sct_task_new("task 2"),
        *task1 = sct_task_new("task 3");

    // A little help...
    task1->number = 1;
    task2->number = 2;
    task3->number = 3;
    
    CuAssertTrue(test, sct_task_compare(task1, task2) < 0);
    CuAssertTrue(test, sct_task_compare(task2, task1) > 0);

    CuAssertTrue(test, sct_task_compare(task2, task3) < 0);
    CuAssertTrue(test, sct_task_compare(task3, task2) > 0);

    CuAssertTrue(test, sct_task_compare(task1, task3) < 0);
    CuAssertTrue(test, sct_task_compare(task3, task1) > 0);

    sct_task_free(task1);
    sct_task_free(task2);
    sct_task_free(task3);
}

void test_task_compare_orders_by_description(CuTest *test) {
    SctTask *task3 = sct_task_new("ZYX"),
        *task2 = sct_task_new("JKL"),
        *task1 = sct_task_new("ABC");

    CuAssertTrue(test, sct_task_compare(task1, task2) < 0);
    CuAssertTrue(test, sct_task_compare(task2, task1) > 0);

    CuAssertTrue(test, sct_task_compare(task2, task3) < 0);
    CuAssertTrue(test, sct_task_compare(task3, task2) > 0);

    CuAssertTrue(test, sct_task_compare(task1, task3) < 0);
    CuAssertTrue(test, sct_task_compare(task3, task1) > 0);

    sct_task_free(task1);
    sct_task_free(task2);
    sct_task_free(task3);
}

void test_task_compare_orders_by_project(CuTest *test) {
    SctTask *task1 = sct_task_new("task 1"),
        *task2 = sct_task_new("task 2"),
        *task3 = sct_task_new("task 3");

    // A little help...
    task1->number = 1;
    task2->number = 2;
    task3->number = 3;
    // Projecting
    SctProject *project = sct_project_new("project");
    sct_project_add_task(project, task1);
    sct_project_add_task(project, task3);

    // If scheduled, project does not influence
    time_t scheduling_date = time(NULL);
    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    sct_task_schedule(task1, scheduling_date);
    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    sct_task_schedule(task2, scheduling_date);
    scheduling_date += SCT_UTIL_SECONDS_IN_DAY*3;
    sct_task_schedule(task3, scheduling_date);

    CuAssertTrue(test, sct_task_compare(task1, task2) < 0);
    CuAssertTrue(test, sct_task_compare(task2, task1) > 0);

    CuAssertTrue(test, sct_task_compare(task2, task3) < 0);
    CuAssertTrue(test, sct_task_compare(task3, task2) > 0);

    CuAssertTrue(test, sct_task_compare(task1, task3) < 0);
    CuAssertTrue(test, sct_task_compare(task3, task1) > 0);

    // If scheduled, project should come first:
    sct_task_unschedule(task1);
    sct_task_unschedule(task2);
    sct_task_unschedule(task3);

    CuAssertTrue(test, sct_task_compare(task1, task2) < 0);
    CuAssertTrue(test, sct_task_compare(task2, task1) > 0);

    CuAssertTrue(test, sct_task_compare(task2, task3) > 0);
    CuAssertTrue(test, sct_task_compare(task3, task2) < 0);

    CuAssertTrue(test, sct_task_compare(task1, task3) < 0);
    CuAssertTrue(test, sct_task_compare(task3, task1) > 0);

    sct_task_free(task1);
    sct_task_free(task2);
    sct_task_free(task3);
}

static void test_task_unscheduled_is_not_scheduled_for_day(CuTest *test) {
    SctTask *task = sct_task_new("Do something" );
    task->scheduled_for = time(NULL); /* This value is invalid */
    CuAssertTrue(test, !sct_task_is_scheduled_for(task, time(NULL)));
    sct_task_free(task);
}

CuSuite *test_task_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_task_create);
    SUITE_ADD_TEST(suite, test_task_alter_description);
    SUITE_ADD_TEST(suite, test_task_remove_from_project);
    SUITE_ADD_TEST(suite, test_task_is_in_project);
    SUITE_ADD_TEST(suite, test_task_switch_done_status);
    SUITE_ADD_TEST(suite, test_task_archive);
    SUITE_ADD_TEST(suite, test_task_is_in_inbox);
    SUITE_ADD_TEST(suite, test_task_archived_is_in_inbox);
    SUITE_ADD_TEST(suite, test_task_created_at);
    SUITE_ADD_TEST(suite, test_task_compare_orders_by_creation_time);
    SUITE_ADD_TEST(suite, test_task_compare_orders_by_scheduld_time);
    SUITE_ADD_TEST(suite, test_task_compare_archived);
    SUITE_ADD_TEST(suite, test_task_compare_orders_by_number);
    SUITE_ADD_TEST(suite, test_task_compare_orders_by_description);
    SUITE_ADD_TEST(suite, test_task_compare_orders_by_project);
    SUITE_ADD_TEST(suite, test_task_unscheduled_is_not_scheduled_for_day);
    return suite;
}
