#include <cgreen/cgreen.h>

#include <talloc.h>
#include <request_queue.h>

#include "request_queue_tests.h"

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

Ensure(Cgreen, request_queue_peek_test) {
    // TODO: Implement.
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_suite(suite, request_queue_new_test_suite());
    add_suite(suite, request_queue_push_test_suite());
    add_suite(suite, request_queue_pop_test_suite());
    add_suite(suite, request_queue_empty_test_suite());
    add_suite(suite, request_queue_move_test_suite());
    return run_test_suite(suite, create_text_reporter());
}
