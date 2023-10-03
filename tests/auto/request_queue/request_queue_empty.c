#include "request_queue_tests.h"

#include <request_queue.h>
#include <talloc.h>

Ensure(empty_with_valid_parameters) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    request_queue *queue = request_queue_new(ctx, 10);

    struct Queue_Node_s node;

    // Push a node into the queue
    enum RequestQueueErrorCode push_result = request_queue_push(queue, &node);
    assert_that(push_result, is_equal_to(OPERATION_SUCCESS));

    // Check if the queue is not empty
    assert_that(request_queue_empty(queue), is_equal_to(false));

    talloc_free(ctx);
}

Ensure(empty_with_empty_queue) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    request_queue *queue = request_queue_new(ctx, 10);

    // Check if an empty queue is reported as empty
    assert_that(request_queue_empty(queue), is_equal_to(true));

    talloc_free(ctx);
}

Ensure(empty_with_null_queue) {
    // Check if a NULL queue is reported as empty
    assert_that(request_queue_empty(NULL), is_equal_to(true));
}

TestSuite*
request_queue_empty_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, empty_with_valid_parameters);
    add_test(suite, empty_with_empty_queue);
    add_test(suite, empty_with_null_queue);

    return suite;
}
