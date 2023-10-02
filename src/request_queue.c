/***********************************************************************************************************************
**
** Copyright (C) 2023 BaseALT Ltd. <org@basealt.ru>
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
***********************************************************************************************************************/

#include "request_queue.h"

struct request_queue
{
    struct Queue_Node_s* head;
    struct Queue_Node_s* tail;
    int size;
    int capacity;
};

/*!
 * \brief request_queue_new Creates new request_queue.
 * \param[in] ctx           Memory context to operate upon.
 * \param[in] capacity      Maximum size of the queue.
 * \return
 *        - NULL on error.
 *        - Pointer to queue on success.
 */
request_queue *request_queue_new(TALLOC_CTX *ctx, unsigned int capacity)
{
    request_queue* result = talloc_zero(ctx, struct request_queue);
    if (!result)
    {
        error("Unable to allocate request_queue.\n");

        return NULL;
    }

    result->capacity = capacity;

    return result;
}

/*!
 * \brief request_queue_push Puts node into queue.
 * \param[in] queue          Current queue to push to.
 * \param[in] node           Node to push to queue.
 * \return
 *        - OPERATION_ERROR_INVALID_PARAMETER if @var{queue} or @var{node} is NULL.
 *        - OPERATION_ERROR_FULL if there is queue overflow.
 *        - OPERATION_ERROR_FAULT if there is corruption of tail pointer.
 *        - OPERATION_SUCCESS if push successful.
 */
enum RequestQueueErrorCode request_queue_push(request_queue *queue, struct Queue_Node_s *node)
{
    if (!queue || !node)
    {
        if (!queue)
        {
            error("Attempt to pass parameter node %d with NULL queue pointer\n", node);
        }

        if (!node)
        {
            error("Attempt to pass NULL node parameter to queue: %d\n", queue);
        }

        return OPERATION_ERROR_INVALID_PARAMETER;
    }

    if (queue->size >= queue->capacity)
    {
        error("Queue overflow %d", queue);

        return OPERATION_ERROR_FULL;
    }

    node->prev = NULL;

    if (queue->size == 0)
    {
        queue->head = node;
        queue->tail = node;
    }
    else
    {
        if (!queue->tail)
        {
            error("Queue does not contain valid tail pointer %d\n", queue);

            return OPERATION_ERROR_FAULT;
        }

        queue->tail->prev = node;
        queue->tail = node;
    }

    ++queue->size;

    return OPERATION_SUCCESS;
}

/*!
 * \brief request_queue_pop Get node from the top of queue and remove it.
 * \param queue[in]         Current queue to receive element from.
 * \return
 *        - NULL on underflow.
 *        - Pointer to element on success.
 */
struct Queue_Node_s *request_queue_pop(request_queue *queue)
{
    if (!queue)
    {
        error("Queue pointer is NULL\n");

        return NULL;
    }

    if (queue->size <= 0)
    {
        error("Unable to get element from empty queue %d\n", queue);

        return NULL;
    }

    if (!queue->head)
    {
        error("Invalid head pointer in queue %d\n", queue);

        return NULL;
    }

    struct Queue_Node_s* result = queue->head;

    queue->head = queue->head->prev;
    --queue->size;

    return result;
}

/*!
 * \brief request_queue_peek Get pointer to the head of queue.
 * \param queue[in]          Current queue to receive element from.
 * \return
 *        - NULL on underflow.
 *        - Pointer to element on success.
 */
struct Queue_Node_s *request_queue_peek(request_queue *queue)
{
    if (!queue)
    {
        error("Queue pointer is NULL\n");

        return NULL;
    }

    if (!queue->head)
    {
        error("Invalid head pointer in queue %d\n", queue);

        return NULL;
    }

    return queue->head;
}

/*!
 * \brief request_queue_empty Returns true if queue is empty.
 * \param queue[in]           Current queue to operate upon.
 * \return
 *        - true if empty.
 *        - false if there are elements in queue.
 */
bool request_queue_empty(request_queue *queue)
{
    if (!queue)
    {
        error("Queue pointer is NULL\n");

        return NULL;
    }

    return queue->size <= 0;
}

/*!
 * \brief request_queue_move Moves elements from @var{from} to @var{to}.
 * \param from[in]           Source queue.
 * \param to[in]             Target queue.
 * \return
 *        - OPERATION_ERROR_INVALID_PARAMETER - if @var{from} or/and @var{to} is NULL.
 *        - OPERATION_ERROR_FAULT - if @var{to} queue has NULL tail pointer. Or if @var{from} queue has invalid
 *                                  tail or head pointers.
 *        - OPERATION_ERROR_FULL - if there is overflow.
 *        - OPERATION_SUCCESS - if operation was successful.
 */
enum RequestQueueErrorCode request_queue_move(request_queue *from, request_queue *to)
{
    if (!from || !to)
    {
        if (!from)
        {
            error("From queue pointer is NULL\n");
        }

        if (!to)
        {
            error("To queue pointer is NULL\n");
        }

        return OPERATION_ERROR_INVALID_PARAMETER;
    }

    if (from->head && from->tail)
    {
        if (!to->tail)
        {
            error("Queue does not contain valid tail pointer %d\n", to);

            return OPERATION_ERROR_FAULT;
        }

        if ((to->size + from->size) > to->capacity)
        {
            error("Unable add reuests to queue %d due to insufficient capacity of receiving queue\n", to);

            return OPERATION_ERROR_FULL;
        }

        if (to->size > 0)
        {
            to->tail->prev = from->head;
            to->tail = from->tail;

            to->size += from->size;
        }
        else
        {
            to->head = from->head;
            to->tail = from->tail;
            to->size = from->size;
        }
    }
    else
    {
        error("From queue malformed: from %d, to %d\n", from, to);

        return OPERATION_ERROR_FAULT;
    }

    return OPERATION_SUCCESS;
}
