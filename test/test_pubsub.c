#include "secretary/test/pubsub.h"
#include "secretary/pubsub.h"

#include <stdlib.h>

static void nop_callback(const char *event_name, SctList *params) {}

static void test_pubsub_exists(CuTest *test) {
    SctPublisher *publisher = sct_publisher_new();
    sct_publisher_add_event(publisher, "nop", nop_callback, NULL);
    sct_publisher_trigger(publisher, "nop");
    sct_publisher_free(publisher);
}

static int var_to_set = 0;
static void setter_callback(const char *event_name, SctList *params) {
    int i = *(int*) sct_list_get_nth_item(params, 0);
    var_to_set = i;
}

static void test_pubsub_execute(CuTest *test) {
    SctPublisher *publisher = sct_publisher_new();

    var_to_set = 0;
    int i = 4;
    SctList *params = sct_list_new_with(1, &i);
    sct_publisher_add_event(publisher, "set-var", setter_callback, params);
    CuAssertIntEquals(test, 0, var_to_set);

    sct_publisher_trigger(publisher, "set-var");
    CuAssertIntEquals(test, i, var_to_set);
        
    sct_publisher_free(publisher);
}

CuSuite *test_pubsub_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_pubsub_exists);
        SUITE_ADD_TEST(suite, test_pubsub_execute);
    return suite;
}
