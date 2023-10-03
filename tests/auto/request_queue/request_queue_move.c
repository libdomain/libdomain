#include "request_queue_tests.h"

#include <request_queue.h>
#include <talloc.h>

Ensure(move_with_valid_parameters) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    request_queue *from_queue = request_queue_new(ctx, 10);
    request_queue *to_queue = request_queue_new(ctx, 10);

    struct Queue_Node_s node;

    // Push a node into the 'from' queue
    enum RequestQueueErrorCode push_result = request_queue_push(from_queue, &node);
    assert_that(push_result, is_equal_to(OPERATION_SUCCESS));

    // Perform the move operation
    enum RequestQueueErrorCode move_result = request_queue_move(from_queue, to_queue);

    // Check the result of the move operation
    assert_that(move_result, is_equal_to(OPERATION_SUCCESS));
    assert_that(request_queue_empty(from_queue), is_equal_to(true));
    assert_that(request_queue_empty(to_queue), is_equal_to(false));

    talloc_free(ctx);
}

Ensure(move_with_null_from_queue) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    request_queue *to_queue = request_queue_new(ctx, 10);

    // Attempt to move from a NULL 'from' queue should return OPERATION_ERROR_INVALID_PARAMETER
    enum RequestQueueErrorCode move_result = request_queue_move(NULL, to_queue);

    assert_that(move_result, is_equal_to(OPERATION_ERROR_INVALID_PARAMETER));

    talloc_free(ctx);
}

Ensure(move_with_null_to_queue) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    request_queue *from_queue = request_queue_new(ctx, 10);

    // Attempt to move to a NULL 'to' queue should return OPERATION_ERROR_INVALID_PARAMETER
    enum RequestQueueErrorCode move_result = request_queue_move(from_queue, NULL);

    assert_that(move_result, is_equal_to(OPERATION_ERROR_INVALID_PARAMETER));

    talloc_free(ctx);
}

Ensure(move_with_faulty_from_queue) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    request_queue *from_queue = request_queue_new(ctx, 10);
    request_queue *to_queue = request_queue_new(ctx, 10);

    // Attempt to move with a faulty 'to' queue should return OPERATION_ERROR_FAULT
    enum RequestQueueErrorCode move_result = request_queue_move(from_queue, to_queue);

    assert_that(move_result, is_equal_to(OPERATION_ERROR_FAULT));

    talloc_free(ctx);
}

Ensure(move_with_insufficient_capacity) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    request_queue *from_queue = request_queue_new(ctx, 10);
    request_queue *to_queue = request_queue_new(ctx, 1);

    struct Queue_Node_s node1;
    struct Queue_Node_s node2;

    // Push an element into the 'from' queue
    enum RequestQueueErrorCode push_result = OPERATION_ERROR_FAULT;
    push_result = request_queue_push(from_queue, &node1);
    assert_that(push_result, is_equal_to(OPERATION_SUCCESS));
    push_result = request_queue_push(from_queue, &node2);
    assert_that(push_result, is_equal_to(OPERATION_SUCCESS));
    assert_that(request_queue_empty(from_queue), is_equal_to(false));

    // Attempt to move with insufficient capacity in the 'to' queue should return OPERATION_ERROR_FULL
    enum RequestQueueErrorCode move_result = request_queue_move(from_queue, to_queue);

    assert_that(move_result, is_equal_to(OPERATION_ERROR_FULL));

    talloc_free(ctx);
}

TestSuite*
request_queue_move_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, move_with_valid_parameters);
    add_test(suite, move_with_null_from_queue);
    add_test(suite, move_with_null_to_queue);
    add_test(suite, move_with_faulty_from_queue);
    add_test(suite, move_with_insufficient_capacity);

    return suite;
}
