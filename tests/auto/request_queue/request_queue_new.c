#include "request_queue_tests.h"

#include <request_queue.h>
#include <talloc.h>

Ensure(queue_is_empty_when_created) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    const int CAPACITY = 10;
    request_queue *queue = request_queue_new(ctx, CAPACITY);

    // Check if the queue is empty when created
    assert_that(request_queue_empty(queue), is_equal_to(true));

    talloc_free(ctx);
}

Ensure(queue_capacity_matches_requested_capacity) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    const int CAPACITY = 10;
    request_queue *queue = request_queue_new(ctx, CAPACITY);

    // Check capacity of the queue.
    assert_that(queue->capacity, is_equal_to(CAPACITY));

    talloc_free(ctx);
}

TestSuite*
request_queue_new_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, queue_is_empty_when_created);
    add_test(suite, queue_capacity_matches_requested_capacity);
    return suite;
}
