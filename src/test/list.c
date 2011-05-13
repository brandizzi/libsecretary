#include <secretary/test/list.h>
#include <secretary/list.h>
#include <stdlib.h>

static void test_list_create(CuTest *test) {
    List *list = list_new();

    CuAssertTrue(test, list!=NULL);
    CuAssertIntEquals(test, list_count_items(list), 0);

    list_free(list);
}

static void test_list_add_items(CuTest *test) {
//malloc(sizeof(400));
    List *list = list_new();
    CuAssertIntEquals(test, list_count_items(list), 0);

    int i = 4;
    int *ipointer = &i;
    char *name = "Adam";
    void *null = "null";
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

CuSuite *test_list_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_list_create);
    SUITE_ADD_TEST(suite, test_list_add_items);
    SUITE_ADD_TEST(suite, test_list_add_a_lot);
    return suite;
}
