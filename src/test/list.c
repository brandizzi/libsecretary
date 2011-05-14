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

CuSuite *test_list_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_list_create);
    SUITE_ADD_TEST(suite, test_list_get_invalid_item);
    SUITE_ADD_TEST(suite, test_list_add_items);
    SUITE_ADD_TEST(suite, test_list_add_a_lot);
    SUITE_ADD_TEST(suite, test_list_remove_item);
    return suite;
}
