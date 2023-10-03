#ifndef REQUEST_QUEUE_TESTS_H
#define REQUEST_QUEUE_TESTS_H

#include <cgreen/cgreen.h>

struct request_queue
{
    struct Queue_Node_s* head;
    struct Queue_Node_s* tail;
    int size;
    int capacity;
};

TestSuite*
request_queue_new_test_suite();

TestSuite*
request_queue_push_test_suite();

TestSuite*
request_queue_pop_test_suite();

TestSuite*
request_queue_empty_test_suite();

TestSuite*
request_queue_move_test_suite();

#endif//REQUEST_QUEUE_TESTS_H
