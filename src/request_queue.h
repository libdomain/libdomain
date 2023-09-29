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

#ifndef LIB_DOMAIN_REQUEST_QUEUE_H
#define LIB_DOMAIN_REQUEST_QUEUE_H

#include "common.h"

typedef struct request_queue request_queue;

struct Queue_Node_s
{
    struct Queue_Node_s* prev;
};

enum RequestQueueErrorCode
{
    OPERATION_SUCCESS                 = 0,
    OPERATION_ERROR_FULL              = 1,
    OPERATION_ERROR_INVALID_PARAMETER = 2,
    OPERATION_ERROR_FAULT             = 3,
};

request_queue*
request_queue_new(TALLOC_CTX* ctx, unsigned int capacity);

enum RequestQueueErrorCode
request_queue_push(request_queue* queue, struct Queue_Node_s *node);

struct Queue_Node_s*
request_queue_pop(request_queue* queue);

struct Queue_Node_s*
request_queue_peek(request_queue* queue);

#endif//LIB_DOMAIN_REQUEST_QUEUE_H
