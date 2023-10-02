#include <cgreen/cgreen.h>

#include <talloc.h>

#include <request_queue.h>

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

Ensure(Cgreen, request_queue_empty_test) {
    // TODO: Implement.
}

Ensure(Cgreen, request_queue_move_test) {
    // TODO: Implement.
}

Ensure(Cgreen, request_queue_peek_test) {
    // TODO: Implement.
}

Ensure(Cgreen, request_queue_pop_test) {
    // TODO: Implement.
}

Ensure(Cgreen, request_queue_push_test) {
    // TODO: Implement.
}

Ensure(Cgreen, request_queue_new_test) {
    TALLOC_CTX* talloc_ctx = talloc_new(NULL);

    struct request_queue* queue = request_queue_new(talloc_ctx, 1024);
    assert_not_equal(queue, NULL);

    talloc_free(talloc_ctx);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, request_queue_new_test);
    return run_test_suite(suite, create_text_reporter());
}
