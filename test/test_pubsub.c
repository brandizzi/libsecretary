#include "secretary/test/pubsub.h"
#include "secretary/pubsub.h"

#include <stdlib.h>

static void nop_callback(const char *event_name, void *params) {}

static void test_pubsub_exists(CuTest *test) {
    SctPublisher *publisher = sct_publisher_new();
    sct_publisher_add_event(publisher, "nop", nop_callback, NULL);
    sct_publisher_trigger(publisher, "nop");
    sct_publisher_free(publisher);
}

CuSuite *test_pubsub_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_pubsub_exists);
    return suite;
}
