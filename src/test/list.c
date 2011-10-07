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
#include <stdlib.h>

static void test_list_create(CuTest *test) {
    List *list = list_new();

    CuAssertTrue(test, list!=NULL);
    CuAssertIntEquals(test, list_count_items(list), 0);

    list_free(list);
}

static void test_list_get_invalid_item(CuTest *test) {
    List *list = list_new();


    CuAssertTrue(test, list!=NULL);
    CuAssertIntEquals(test, list_count_items(list), 0);

    CuAssertPtrEquals(test, NULL, list_get_nth_item(list, 8000));

    list_free(list);
}

static void test_list_add_items(CuTest *test) {
    List *list = list_new();
    CuAssertIntEquals(test, list_count_items(list), 0);

    int i = 4;
    int *ipointer = &i;
    char *name = "Adam";
    void *null = NULL;
    list_add_item(list, ipointer);
    list_add_item(list, name);
    list_add_item(list, null);

    CuAssertIntEquals(test, list_count_items(list), 3);
    CuAssertPtrEquals(test, list_get_nth_item(list, 0), ipointer);
    CuAssertPtrEquals(test, list_get_nth_item(list, 1), name);
    CuAssertPtrEquals(test, list_get_nth_item(list, 2), null);

    list_free(list);
}

static void test_list_add_a_lot(CuTest *test) {
    List *list = list_new();
    CuAssertIntEquals(test, list_count_items(list), 0);

    char *name = "Adam";
    for (int i = 0; i < 1024; i++) {
        list_add_item(list, name);
    }

    CuAssertIntEquals(test, list_count_items(list), 1024);
    for (int i = 0; i < 1024; i++) {
        CuAssertPtrEquals(test, list_get_nth_item(list, i), name);
    }

    list_free(list);

}

static void test_list_remove_item(CuTest *test) {
    List *list = list_new();
    CuAssertIntEquals(test, list_count_items(list), 0);

    char *item1 = "item 1", *item2 = "item 2", *item3 = "item 3", 
            *item4 = "item 4", *item5 = "item 5";
    list_add_item(list, item1);
    list_add_item(list, item2);
    list_add_item(list, item3);
    list_add_item(list, item4);
    list_add_item(list, item5);

    CuAssertIntEquals(test, list_count_items(list), 5);
    CuAssertPtrEquals(test, list_get_nth_item(list, 0), item1);
    CuAssertPtrEquals(test, list_get_nth_item(list, 1), item2);
    CuAssertPtrEquals(test, list_get_nth_item(list, 2), item3);
    CuAssertPtrEquals(test, list_get_nth_item(list, 3), item4);
    CuAssertPtrEquals(test, list_get_nth_item(list, 4), item5);

    list_remove_item(list, item2);
    list_remove_item(list, item4);

    CuAssertIntEquals(test, list_count_items(list), 3);
    CuAssertPtrEquals(test, list_get_nth_item(list, 0), item1);
    CuAssertPtrEquals(test, list_get_nth_item(list, 1), item3);
    CuAssertPtrEquals(test, list_get_nth_item(list, 2), item5);

    list_free(list);
}

static int intcomp(const void *i1, const void *i2) {
    return *(int*)i2 - *(int*)i1;
}

static void test_list_sort(CuTest *test) {
    List *list = list_new();

    int i1 = 1, i2 = 2, i3 = 3, i4 = 4, i5 = 5;

    list_add_item(list, &i1);
    list_add_item(list, &i4);
    list_add_item(list, &i3);
    list_add_item(list, &i5);
    list_add_item(list, &i2);

    CuAssertIntEquals(test, list_count_items(list), 5);
    CuAssertPtrEquals(test, &i1, list_get_nth_item(list, 0));
    CuAssertPtrEquals(test, &i4, list_get_nth_item(list, 1));
    CuAssertPtrEquals(test, &i3, list_get_nth_item(list, 2));
    CuAssertPtrEquals(test, &i5, list_get_nth_item(list, 3));
    CuAssertPtrEquals(test, &i2, list_get_nth_item(list, 4));


    list_sort(list, intcomp);

    CuAssertIntEquals(test, list_count_items(list), 5);
    CuAssertPtrEquals(test, &i1, list_get_nth_item(list, 0));
    CuAssertPtrEquals(test, &i2, list_get_nth_item(list, 1));
    CuAssertPtrEquals(test, &i3, list_get_nth_item(list, 2));
    CuAssertPtrEquals(test, &i4, list_get_nth_item(list, 3));
    CuAssertPtrEquals(test, &i5, list_get_nth_item(list, 4));

    list_free(list);
}

CuSuite *test_list_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_list_create);
    SUITE_ADD_TEST(suite, test_list_get_invalid_item);
    SUITE_ADD_TEST(suite, test_list_add_items);
    SUITE_ADD_TEST(suite, test_list_add_a_lot);
    SUITE_ADD_TEST(suite, test_list_remove_item);
    SUITE_ADD_TEST(suite, test_list_sort);
    return suite;
}
