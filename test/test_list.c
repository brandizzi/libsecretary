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
#include <secretary/test/list.h>
#include <secretary/list.h>
#include <secretary/task.h>
#include <secretary/task.h>
#include <stdlib.h>

static void test_list_create(CuTest *test) {
    SctList *list = sct_list_new();

    CuAssertTrue(test, list!=NULL);
    CuAssertIntEquals(test, sct_list_count_items(list), 0);

    sct_list_free(list);
}

static void test_list_get_invalid_item(CuTest *test) {
    SctList *list = sct_list_new();


    CuAssertTrue(test, list!=NULL);
    CuAssertIntEquals(test, sct_list_count_items(list), 0);

    CuAssertPtrEquals(test, NULL, sct_list_get_nth_item(list, 8000));

    sct_list_free(list);
}

static void test_list_add_items(CuTest *test) {
    SctList *list = sct_list_new();
    CuAssertIntEquals(test, sct_list_count_items(list), 0);

    int i = 4;
    int *ipointer = &i;
    char *name = "Adam";
    void *null = NULL;
    sct_list_add_item(list, ipointer);
    sct_list_add_item(list, name);
    sct_list_add_item(list, null);

    CuAssertIntEquals(test, sct_list_count_items(list), 3);
    CuAssertPtrEquals(test, sct_list_get_nth_item(list, 0), ipointer);
    CuAssertPtrEquals(test, sct_list_get_nth_item(list, 1), name);
    CuAssertPtrEquals(test, sct_list_get_nth_item(list, 2), null);

    sct_list_free(list);
}

static void test_list_new_with(CuTest *test) {
    int i = 4;
    int *ipointer = &i;
    char *name = "Adam";
    void *null = NULL;

    // Using counter mostly because we need a first parameter
    SctList *list = sct_list_new_with(3, ipointer, name, null);
    CuAssertIntEquals(test, sct_list_count_items(list), 3);
    
    CuAssertPtrEquals(test, sct_list_get_nth_item(list, 0), ipointer);
    CuAssertPtrEquals(test, sct_list_get_nth_item(list, 1), name);
    CuAssertPtrEquals(test, sct_list_get_nth_item(list, 2), null);

    sct_list_free(list);
}

static void test_list_add_a_lot(CuTest *test) {
    SctList *list = sct_list_new();
    CuAssertIntEquals(test, sct_list_count_items(list), 0);

    char *name = "Adam";
    for (int i = 0; i < 1024; i++) {
        sct_list_add_item(list, name);
    }

    CuAssertIntEquals(test, sct_list_count_items(list), 1024);
    for (int i = 0; i < 1024; i++) {
        CuAssertPtrEquals(test, sct_list_get_nth_item(list, i), name);
    }

    sct_list_free(list);

}

static void test_list_remove_item(CuTest *test) {
    SctList *list = sct_list_new();
    CuAssertIntEquals(test, sct_list_count_items(list), 0);

    char *item1 = "item 1", *item2 = "item 2", *item3 = "item 3", 
            *item4 = "item 4", *item5 = "item 5";
    sct_list_add_item(list, item1);
    sct_list_add_item(list, item2);
    sct_list_add_item(list, item3);
    sct_list_add_item(list, item4);
    sct_list_add_item(list, item5);

    CuAssertIntEquals(test, sct_list_count_items(list), 5);
    CuAssertPtrEquals(test, sct_list_get_nth_item(list, 0), item1);
    CuAssertPtrEquals(test, sct_list_get_nth_item(list, 1), item2);
    CuAssertPtrEquals(test, sct_list_get_nth_item(list, 2), item3);
    CuAssertPtrEquals(test, sct_list_get_nth_item(list, 3), item4);
    CuAssertPtrEquals(test, sct_list_get_nth_item(list, 4), item5);

    sct_list_remove_item(list, item2);
    sct_list_remove_item(list, item4);

    CuAssertIntEquals(test, sct_list_count_items(list), 3);
    CuAssertPtrEquals(test, sct_list_get_nth_item(list, 0), item1);
    CuAssertPtrEquals(test, sct_list_get_nth_item(list, 1), item3);
    CuAssertPtrEquals(test, sct_list_get_nth_item(list, 2), item5);

    sct_list_free(list);
}

static int intcomp(const void *i1, const void *i2) {
    return **(int**)i1 - **(int**)i2;
}

static void test_list_sort(CuTest *test) {
    SctList *list = sct_list_new();

    int i1 = 1, i2 = 2, i3 = 3, i4 = 4, i5 = 5;

    sct_list_add_item(list, &i1);
    sct_list_add_item(list, &i4);
    sct_list_add_item(list, &i3);
    sct_list_add_item(list, &i5);
    sct_list_add_item(list, &i2);

    CuAssertIntEquals(test, sct_list_count_items(list), 5);
    CuAssertPtrEquals(test, &i1, sct_list_get_nth_item(list, 0));
    CuAssertPtrEquals(test, &i4, sct_list_get_nth_item(list, 1));
    CuAssertPtrEquals(test, &i3, sct_list_get_nth_item(list, 2));
    CuAssertPtrEquals(test, &i5, sct_list_get_nth_item(list, 3));
    CuAssertPtrEquals(test, &i2, sct_list_get_nth_item(list, 4));


    sct_list_sort(list, intcomp);

    CuAssertIntEquals(test, sct_list_count_items(list), 5);
    CuAssertPtrEquals(test, &i1, sct_list_get_nth_item(list, 0));
    CuAssertPtrEquals(test, &i2, sct_list_get_nth_item(list, 1));
    CuAssertPtrEquals(test, &i3, sct_list_get_nth_item(list, 2));
    CuAssertPtrEquals(test, &i4, sct_list_get_nth_item(list, 3));
    CuAssertPtrEquals(test, &i5, sct_list_get_nth_item(list, 4));

    sct_list_free(list);
}

static int my_task_compare(const void *p1, const void *p2) {
    const SctTask *t1 = *(SctTask**)p1,
            *t2 = *(SctTask**)p2;
    return sct_task_compare(t1, t2);
}

static void test_list_sort_tasks(CuTest *test) {
    SctList *list = sct_list_new();

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

    sct_list_add_item(list, as6_task);
    sct_list_add_item(list, s6_task);
    sct_list_add_item(list, as3_task);
    sct_list_add_item(list, s3_task);
    sct_list_add_item(list, a_task);
    sct_list_add_item(list, task);

    CuAssertIntEquals(test, sct_list_count_items(list), 6);
    CuAssertPtrEquals(test, as6_task, sct_list_get_nth_item(list, 0));
    CuAssertPtrEquals(test, s6_task, sct_list_get_nth_item(list, 1));
    CuAssertPtrEquals(test, as3_task, sct_list_get_nth_item(list, 2));
    CuAssertPtrEquals(test, s3_task, sct_list_get_nth_item(list, 3));
    CuAssertPtrEquals(test, a_task, sct_list_get_nth_item(list, 4));
    CuAssertPtrEquals(test, task, sct_list_get_nth_item(list, 5));

    CuAssertIntEquals(test, sct_list_count_items(list), 6);
    sct_list_sort(list, my_task_compare);
    
    CuAssertIntEquals(test, sct_list_count_items(list), 6);
    CuAssertPtrEquals(test, s3_task, sct_list_get_nth_item(list, 0));
    CuAssertPtrEquals(test, s6_task, sct_list_get_nth_item(list, 1));
    CuAssertPtrEquals(test, task, sct_list_get_nth_item(list, 2));
    CuAssertPtrEquals(test, as3_task, sct_list_get_nth_item(list, 3));
    CuAssertPtrEquals(test, as6_task, sct_list_get_nth_item(list, 4));
    CuAssertPtrEquals(test, a_task, sct_list_get_nth_item(list, 5));


    sct_list_free(list);
}

static bool is_even(void *p, SctList *params) {
    int *pi = p;
    return *pi % 2 == 0;
}

static bool is_odd(void *p, SctList *params) {
    int *pi = p;
    return *pi % 2;
}

static bool is_rest_equal(void *item, SctList *params) {
    int *pi = item,
        *pq = sct_list_get_nth_item(params, 0), // First parameter: quotient
        *pr = sct_list_get_nth_item(params, 1); // Scnd  parameter: rest
    
    return *pi % *pq == *pr;
}

static void test_list_get_nth_item_by_criteria(CuTest *test) {
    SctList *list = sct_list_new();

    int i1 = 1, i2 = 2, i3 = 3, i4 = 4, i5 = 5;

    sct_list_add_item(list, &i1);
    sct_list_add_item(list, &i2);
    sct_list_add_item(list, &i3);
    sct_list_add_item(list, &i4);
    sct_list_add_item(list, &i5);

    int *p = sct_list_get_nth_item_by_criteria(list, 0, is_even, NULL);
    CuAssertTrue(test, p != NULL);
    CuAssertIntEquals(test, *p, 2);
    p = sct_list_get_nth_item_by_criteria(list, 1, is_even, NULL);
    CuAssertTrue(test, p != NULL);
    CuAssertIntEquals(test, *p, 4);
    p = sct_list_get_nth_item_by_criteria(list, 2, is_even, NULL);
    CuAssertPtrEquals(test, p, NULL);

    p = sct_list_get_nth_item_by_criteria(list, 0, is_odd, NULL);
    CuAssertTrue(test, p != NULL);
    CuAssertIntEquals(test, *p, 1);
    p = sct_list_get_nth_item_by_criteria(list, 1, is_odd, NULL);
    CuAssertTrue(test, p != NULL);
    CuAssertIntEquals(test, *p, 3);
    p = sct_list_get_nth_item_by_criteria(list, 2, is_odd, NULL);
    CuAssertTrue(test, p != NULL);
    CuAssertIntEquals(test, *p, 5);
    p = sct_list_get_nth_item_by_criteria(list, 3, is_odd, NULL);
    CuAssertPtrEquals(test, p, NULL);

    sct_list_free(list);
}

static void test_list_get_nth_item_by_criteria_with_params(CuTest *test) {
    SctList *list = sct_list_new();

    int i1 = 1, i2 = 2, i3 = 3, i4 = 4, i5 = 5;

    sct_list_add_item(list, &i1);
    sct_list_add_item(list, &i2);
    sct_list_add_item(list, &i3);
    sct_list_add_item(list, &i4);
    sct_list_add_item(list, &i5);


    int quotient, rest;
    SctList *params = sct_list_new_with(2, &quotient, &rest);

    quotient = 3;
    rest = 2;

    int *p = sct_list_get_nth_item_by_criteria(list, 0, is_rest_equal, params);
    CuAssertTrue(test, p != NULL);
    CuAssertIntEquals(test, *p, 2);
    p = sct_list_get_nth_item_by_criteria(list, 1, is_rest_equal, params);
    CuAssertTrue(test, p != NULL);
    CuAssertIntEquals(test, *p, 5);
    p = sct_list_get_nth_item_by_criteria(list, 2, is_rest_equal, params);
    CuAssertPtrEquals(test, p, NULL);

    quotient = 4;
    rest = 1;

    p = sct_list_get_nth_item_by_criteria(list, 0, is_rest_equal, params);
    CuAssertTrue(test, p != NULL);
    CuAssertIntEquals(test, *p, 1);
    p = sct_list_get_nth_item_by_criteria(list, 1, is_rest_equal, params);
    CuAssertTrue(test, p != NULL);
    CuAssertIntEquals(test, *p, 5);
    p = sct_list_get_nth_item_by_criteria(list, 2, is_rest_equal, params);
    CuAssertPtrEquals(test, p, NULL);

    sct_list_free(list);
}

static void test_list_count_items_by_criteria(CuTest *test) {
    SctList *list = sct_list_new();

    int i1 = 1, i2 = 2, i3 = 3, i4 = 4, i5 = 5;

    sct_list_add_item(list, &i1);
    sct_list_add_item(list, &i2);
    sct_list_add_item(list, &i3);
    sct_list_add_item(list, &i4);
    sct_list_add_item(list, &i5);

    CuAssertIntEquals(test, 2, 
            sct_list_count_items_by_criteria(list, is_even, NULL));
    CuAssertIntEquals(test, 3, sct_list_count_items_by_criteria(list, is_odd, NULL));
            
    sct_list_free(list);
}


static void test_list_count_items_by_criteria_with_params(CuTest *test) {
    SctList *list = sct_list_new();

    int i1 = 1, i2 = 2, i3 = 3, i4 = 4, i5 = 5;

    sct_list_add_item(list, &i1);
    sct_list_add_item(list, &i2);
    sct_list_add_item(list, &i3);
    sct_list_add_item(list, &i4);
    sct_list_add_item(list, &i5);

    int quotient, rest;
    SctList *params = sct_list_new_with(2, &quotient, &rest);

    quotient = 3;
    rest = 2;

    CuAssertIntEquals(test, 2, 
            sct_list_count_items_by_criteria(list, is_rest_equal, params));

    quotient = 4;
    rest = 1;

    CuAssertIntEquals(test, 2, 
            sct_list_count_items_by_criteria(list, is_rest_equal, params));

    sct_list_free(list);
}

static void test_list_sublist(CuTest *test) {
    SctList *list = sct_list_new();

    int i1 = 1, i2 = 2, i3 = 3, i4 = 4, i5 = 5;

    sct_list_add_item(list, &i1);
    sct_list_add_item(list, &i2);
    sct_list_add_item(list, &i3);
    sct_list_add_item(list, &i4);
    sct_list_add_item(list, &i5);


    SctList *sublist = sct_sublist_new(list, 2, 2);

    CuAssertIntEquals(test, 2, sct_list_count_items(sublist));
    int *p = sct_list_get_nth_item(sublist, 0);
    CuAssertIntEquals(test, 3, *p);
    p = sct_list_get_nth_item(sublist, 1);
    CuAssertIntEquals(test, 4, *p);
    p = sct_list_get_nth_item(sublist, 3);
    CuAssertPtrEquals(test, NULL, p);

    sct_sublist_free(sublist);
    sct_list_free(list);
}

static void test_list_sublist_update_range(CuTest *test) {
    SctList *list = sct_list_new();

    int i1 = 1, i2 = 2, i3 = 3, i4 = 4, i5 = 5;

    sct_list_add_item(list, &i1);
    sct_list_add_item(list, &i2);
    sct_list_add_item(list, &i3);
    sct_list_add_item(list, &i4);
    sct_list_add_item(list, &i5);


    SctList *sublist = sct_sublist_new(list, 2, 2);

    CuAssertIntEquals(test, 2, sct_list_count_items(sublist));
    int *p = sct_list_get_nth_item(sublist, 0);
    CuAssertIntEquals(test, 3, *p);
    p = sct_list_get_nth_item(sublist, 1);
    CuAssertIntEquals(test, 4, *p);
    p = sct_list_get_nth_item(sublist, 3);
    CuAssertPtrEquals(test, NULL, p);

    sct_sublist_update_range(sublist, 3, 2);

    CuAssertIntEquals(test, 2, sct_list_count_items(sublist));
    p = sct_list_get_nth_item(sublist, 0);
    CuAssertIntEquals(test, 4, *p);
    p = sct_list_get_nth_item(sublist, 1);
    CuAssertIntEquals(test, 5, *p);
    p = sct_list_get_nth_item(sublist, 3);
    CuAssertPtrEquals(test, NULL, p);

    sct_sublist_free(sublist);
    sct_list_free(list);
}

static void test_list_get_nth_item_index_by_criteria(CuTest *test) {
    SctList *list = sct_list_new();

    int i1 = 1, i2 = 2, i3 = 3, i4 = 4, i5 = 5;

    sct_list_add_item(list, &i1);
    sct_list_add_item(list, &i2);
    sct_list_add_item(list, &i3);
    sct_list_add_item(list, &i4);
    sct_list_add_item(list, &i5);


    int quotient, rest;
    SctList *params = sct_list_new_with(2, &quotient, &rest);

    quotient = 3;
    rest = 2;

    int i = sct_list_get_nth_item_index_by_criteria(
            list, 0, is_rest_equal, params, 0);
    CuAssertTrue(test, i != SCT_LIST_ITEM_NOT_FOUND);
    CuAssertIntEquals(test, i, 1);
    i = sct_list_get_nth_item_index_by_criteria(list, 1, is_rest_equal, params, 0);
    CuAssertTrue(test, i != SCT_LIST_ITEM_NOT_FOUND);
    CuAssertIntEquals(test, i, 4);
    i = sct_list_get_nth_item_index_by_criteria(list, 2, is_rest_equal, params, 0);
    CuAssertIntEquals(test, SCT_LIST_ITEM_NOT_FOUND, i);

    quotient = 4;
    rest = 1;

    i = sct_list_get_nth_item_index_by_criteria(list, 0, is_rest_equal, params, 0);
    CuAssertTrue(test, i != SCT_LIST_ITEM_NOT_FOUND);
    CuAssertIntEquals(test, i, 0);
    i = sct_list_get_nth_item_index_by_criteria(list, 1, is_rest_equal, params, 0);
    CuAssertTrue(test, i != SCT_LIST_ITEM_NOT_FOUND);
    CuAssertIntEquals(test, i, 4);
    i = sct_list_get_nth_item_index_by_criteria(list, 2, is_rest_equal, params, 0);
    CuAssertIntEquals(test, SCT_LIST_ITEM_NOT_FOUND, i);


    sct_list_free(list);
}

static void test_list_get_nth_item_index_by_criteria_start(CuTest *test) {
    SctList *list = sct_list_new();

    int i1 = 1, i2 = 2, i3 = 3, i4 = 4, i5 = 5;

    sct_list_add_item(list, &i1);
    sct_list_add_item(list, &i2);
    sct_list_add_item(list, &i3);
    sct_list_add_item(list, &i4);
    sct_list_add_item(list, &i5);


    int quotient, rest;
    SctList *params = sct_list_new_with(2, &quotient, &rest);

    quotient = 3;
    rest = 2;

    int i = 0;
    i = sct_list_get_nth_item_index_by_criteria(list, 0, is_rest_equal, params, i);
    CuAssertTrue(test, i != SCT_LIST_ITEM_NOT_FOUND);
    CuAssertIntEquals(test, i, 1);
    i = sct_list_get_nth_item_index_by_criteria(list, 0, is_rest_equal, params, i+1);
    CuAssertTrue(test, i != SCT_LIST_ITEM_NOT_FOUND);
    CuAssertIntEquals(test, i, 4);
    i = sct_list_get_nth_item_index_by_criteria(list, 0, is_rest_equal, params, i+1);
    CuAssertIntEquals(test, SCT_LIST_ITEM_NOT_FOUND, i);


    quotient = 4;
    rest = 1;

    i = 0;
    i = sct_list_get_nth_item_index_by_criteria(list, 0, is_rest_equal, params, i);
    CuAssertTrue(test, i != SCT_LIST_ITEM_NOT_FOUND);
    CuAssertIntEquals(test, i, 0);
    i = sct_list_get_nth_item_index_by_criteria(list, 0, is_rest_equal, params, i+1);
    CuAssertTrue(test, i != SCT_LIST_ITEM_NOT_FOUND);
    CuAssertIntEquals(test, i, 4);
    i = sct_list_get_nth_item_index_by_criteria(list, 0, is_rest_equal, params, i+1);
    CuAssertIntEquals(test, SCT_LIST_ITEM_NOT_FOUND, i);


    sct_list_free(list);
}

static void test_list_extend(CuTest *test) {
    SctList *list1 = sct_list_new();
    int i0 = 0, i1 = 1, i2 = 2, i3 = 3, i4 = 4;

    sct_list_add_item(list1, &i0);
    sct_list_add_item(list1, &i1);
    sct_list_add_item(list1, &i2);
    sct_list_add_item(list1, &i3);
    sct_list_add_item(list1, &i4);

    CuAssertIntEquals(test, sct_list_count_items(list1), 5);
    CuAssertPtrEquals(test, &i0, sct_list_get_nth_item(list1, 0));
    CuAssertPtrEquals(test, &i1, sct_list_get_nth_item(list1, 1));
    CuAssertPtrEquals(test, &i2, sct_list_get_nth_item(list1, 2));
    CuAssertPtrEquals(test, &i3, sct_list_get_nth_item(list1, 3));
    CuAssertPtrEquals(test, &i4, sct_list_get_nth_item(list1, 4));

    SctList *list2 = sct_list_new();
    int i5 = 0, i6 = 1, i7 = 2, i8 = 3, i9 = 4;

    sct_list_add_item(list2, &i5);
    sct_list_add_item(list2, &i6);
    sct_list_add_item(list2, &i7);
    sct_list_add_item(list2, &i8);
    sct_list_add_item(list2, &i9);

    CuAssertPtrEquals(test, &i5, sct_list_get_nth_item(list2, 0));
    CuAssertPtrEquals(test, &i6, sct_list_get_nth_item(list2, 1));
    CuAssertPtrEquals(test, &i7, sct_list_get_nth_item(list2, 2));
    CuAssertPtrEquals(test, &i8, sct_list_get_nth_item(list2, 3));
    CuAssertPtrEquals(test, &i9, sct_list_get_nth_item(list2, 4));

    sct_list_extend(list1, list2);

    CuAssertIntEquals(test, sct_list_count_items(list1), 10);
    CuAssertPtrEquals(test, &i0, sct_list_get_nth_item(list1, 0));
    CuAssertPtrEquals(test, &i1, sct_list_get_nth_item(list1, 1));
    CuAssertPtrEquals(test, &i2, sct_list_get_nth_item(list1, 2));
    CuAssertPtrEquals(test, &i3, sct_list_get_nth_item(list1, 3));
    CuAssertPtrEquals(test, &i4, sct_list_get_nth_item(list1, 4));
    CuAssertPtrEquals(test, &i5, sct_list_get_nth_item(list1, 5));
    CuAssertPtrEquals(test, &i6, sct_list_get_nth_item(list1, 6));
    CuAssertPtrEquals(test, &i7, sct_list_get_nth_item(list1, 7));
    CuAssertPtrEquals(test, &i8, sct_list_get_nth_item(list1, 8));
    CuAssertPtrEquals(test, &i9, sct_list_get_nth_item(list1, 9));

    sct_list_free(list1);
    sct_list_free(list2);
}


CuSuite *test_list_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_list_create);
    SUITE_ADD_TEST(suite, test_list_new_with);
    SUITE_ADD_TEST(suite, test_list_get_invalid_item);
    SUITE_ADD_TEST(suite, test_list_add_items);
    SUITE_ADD_TEST(suite, test_list_add_a_lot);
    SUITE_ADD_TEST(suite, test_list_remove_item);
    SUITE_ADD_TEST(suite, test_list_sort);
    SUITE_ADD_TEST(suite, test_list_sort_tasks);
    SUITE_ADD_TEST(suite, test_list_get_nth_item_by_criteria);
    SUITE_ADD_TEST(suite, test_list_get_nth_item_by_criteria_with_params);
    SUITE_ADD_TEST(suite, test_list_count_items_by_criteria);
    SUITE_ADD_TEST(suite, test_list_count_items_by_criteria_with_params);
    SUITE_ADD_TEST(suite, test_list_sublist);
    SUITE_ADD_TEST(suite, test_list_sublist_update_range);
    SUITE_ADD_TEST(suite, test_list_get_nth_item_index_by_criteria);
    SUITE_ADD_TEST(suite, test_list_get_nth_item_index_by_criteria_start);
    SUITE_ADD_TEST(suite, test_list_extend);
    return suite;
}
