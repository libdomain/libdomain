#include "request_queue_tests.h"

#include <request_queue.h>
#include <talloc.h>

Ensure(pop_with_valid_parameters) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    request_queue *queue = request_queue_new(ctx, 10);

    struct Queue_Node_s node;

    // Push a node into the queue
    enum RequestQueueErrorCode push_result = request_queue_push(queue, &node);
    assert_that(push_result, is_equal_to(OPERATION_SUCCESS));

    // Pop an element from the queue should return the node
    struct Queue_Node_s *popped_node = request_queue_pop(queue);

    assert_that(popped_node, is_equal_to(&node));
    assert_that(request_queue_empty(queue), is_equal_to(true));

    talloc_free(ctx);
}

Ensure(pop_from_empty_queue) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    request_queue *queue = request_queue_new(ctx, 10);

    // Pop from an empty queue should return NULL
    struct Queue_Node_s *popped_node = request_queue_pop(queue);

    assert_that(popped_node, is_equal_to(NULL));

    talloc_free(ctx);
}

Ensure(pop_with_null_queue) {
    // Pop with a NULL queue should return NULL
    struct Queue_Node_s *popped_node = request_queue_pop(NULL);

    assert_that(popped_node, is_equal_to(NULL));
}

Ensure(pop_with_faulty_head) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    request_queue *queue = request_queue_new(ctx, 10);

    struct Queue_Node_s node;

    // Push a node into the queue
    enum RequestQueueErrorCode push_result = request_queue_push(queue, &node);
    assert_that(push_result, is_equal_to(OPERATION_SUCCESS));

    // Simulate a faulty head pointer
    queue->head = NULL;

    // Pop with a faulty head pointer should return NULL
    struct Queue_Node_s *popped_node = request_queue_pop(queue);

    assert_that(popped_node, is_equal_to(NULL));

    talloc_free(ctx);
}

TestSuite*
request_queue_pop_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, pop_with_valid_parameters);
    add_test(suite, pop_from_empty_queue);
    add_test(suite, pop_with_null_queue);
    add_test(suite, pop_with_faulty_head);

    return suite;
}
