#include "request_queue_tests.h"

#include <request_queue.h>
#include <talloc.h>

Ensure(push_with_valid_parameters) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    request_queue *queue = request_queue_new(ctx, 10);

    struct Queue_Node_s node;

    // Push with valid parameters should return OPERATION_SUCCESS
    enum RequestQueueErrorCode push_result = request_queue_push(queue, &node);

    assert_that(push_result, is_equal_to(OPERATION_SUCCESS));
    assert_that(request_queue_empty(queue), is_equal_to(false));

    talloc_free(ctx);
}

Ensure(push_with_null_queue) {
    struct Queue_Node_s node;

    // Push with a NULL queue should return OPERATION_ERROR_INVALID_PARAMETER
    enum RequestQueueErrorCode push_result = request_queue_push(NULL, &node);

    assert_that(push_result, is_equal_to(OPERATION_ERROR_INVALID_PARAMETER));
}

Ensure(push_with_null_node) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    request_queue *queue = request_queue_new(ctx, 10);

    // Push with a NULL node should return OPERATION_ERROR_INVALID_PARAMETER
    enum RequestQueueErrorCode push_result = request_queue_push(queue, NULL);

    assert_that(push_result, is_equal_to(OPERATION_ERROR_INVALID_PARAMETER));

    talloc_free(ctx);
}

Ensure(push_with_queue_overflow) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    request_queue *queue = request_queue_new(ctx, 1);

    struct Queue_Node_s node1;
    struct Queue_Node_s node2;

    // Push one element to the queue
    enum RequestQueueErrorCode push_result1 = request_queue_push(queue, &node1);
    assert_that(push_result1, is_equal_to(OPERATION_SUCCESS));
    assert_that(request_queue_empty(queue), is_equal_to(false));

    // Pushing a second element when the queue is full should return OPERATION_ERROR_FULL
    enum RequestQueueErrorCode push_result2 = request_queue_push(queue, &node2);
    assert_that(push_result2, is_equal_to(OPERATION_ERROR_FULL));

    talloc_free(ctx);
}

Ensure(push_with_faulty_tail) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    request_queue *queue = request_queue_new(ctx, 10);

    struct Queue_Node_s node;

    // Due to talloc_zero queue created with null tail pointer.
    enum RequestQueueErrorCode push_result = request_queue_push(queue, &node);
    assert_that(push_result, is_equal_to(OPERATION_SUCCESS));
    // Now we need to damage tail pointer.
    queue->tail = NULL;

    // Pushing an element with a faulty tail pointer should return OPERATION_ERROR_FAULT
    push_result = request_queue_push(queue, &node);
    assert_that(push_result, is_equal_to(OPERATION_ERROR_FAULT));

    talloc_free(ctx);
}

TestSuite* request_queue_push_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, push_with_valid_parameters);
    add_test(suite, push_with_null_queue);
    add_test(suite, push_with_null_node);
    add_test(suite, push_with_queue_overflow);
    add_test(suite, push_with_faulty_tail);

    return suite;
}
