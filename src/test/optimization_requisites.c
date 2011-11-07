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
    
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_projects(secretary));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, false));

    CuAssertStrEquals(test, "Test task creation", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));
    CuAssertPtrEquals(test, secretary, task->secretary);
    
    secretary_free(secretary);
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
    task_schedule(task1, timegm(&date));

    date.tm_mday = 3;
    task_schedule(task2, timegm(&date));
    
    date.tm_mday = 4;
    task_schedule(task3, timegm(&date));
    
    date.tm_mday = 5;
    task_schedule(task4, timegm(&date));
    
    date.tm_mday = 7;
    task_schedule(task5, timegm(&date));
    list_add_item(list, task5);

    CuAssertPtrEquals(test, task1, secretary_get_nth_task_scheduled(secretary, 0, false));
    CuAssertPtrEquals(test, task2, secretary_get_nth_task_scheduled(secretary, 1, false));
    CuAssertPtrEquals(test, task3, secretary_get_nth_task_scheduled(secretary, 2, false));
    CuAssertPtrEquals(test, task4, secretary_get_nth_task_scheduled(secretary, 3, false));
    CuAssertPtrEquals(test, task5, secretary_get_nth_task_scheduled(secretary, 4, false));

    date.tm_mday = 10;
    task_schedule(task3, timegm(&date));

    CuAssertPtrEquals(test, task1, secretary_get_nth_task_scheduled(secretary, 0, false));
    CuAssertPtrEquals(test, task2, secretary_get_nth_task_scheduled(secretary, 1, false));
    CuAssertPtrEquals(test, task4, secretary_get_nth_task_scheduled(secretary, 2, false));
    CuAssertPtrEquals(test, task5, secretary_get_nth_task_scheduled(secretary, 3, false));
    CuAssertPtrEquals(test, task3, secretary_get_nth_task_scheduled(secretary, 4, false));

    date.tm_mday = 1;
    task_schedule(task5, timegm(&date));

    CuAssertPtrEquals(test, task5, secretary_get_nth_task_scheduled(secretary, 0, false));
    CuAssertPtrEquals(test, task1, secretary_get_nth_task_scheduled(secretary, 1, false));
    CuAssertPtrEquals(test, task2, secretary_get_nth_task_scheduled(secretary, 2, false));
    CuAssertPtrEquals(test, task4, secretary_get_nth_task_scheduled(secretary, 3, false));
    CuAssertPtrEquals(test, task3, secretary_get_nth_task_scheduled(secretary, 4, false));

    date.tm_mday = 6;
    task_schedule(task1, timegm(&date));

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
    SUITE_ADD_TEST(suite, test_optimization_requisites_task_set_date_sort_list);
    return suite;
}
