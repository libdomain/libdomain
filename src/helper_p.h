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

#ifndef LIB_DOMAIN_HELPER_PRIVATE_H
#define LIB_DOMAIN_HELPER_PRIVATE_H

#include <talloc.h>
#include "common.h"

#define LD_ALLOC_HELPER(var, func, error, error_handler, ...) \
    var = func(__VA_ARGS__); \
    if (var == NULL) \
    { \
        ld_error(error); \
        goto error_handler; \
    }
#define LD_FREE_HELPER(func, error, error_handler, ctx, ...) \
    if (func(ctx) != 0) \
    { \
        ctx = NULL; \
        ld_error(error); \
        goto error_handler; \
    } \
    ctx = NULL; 

#define ld_talloc_e(var, error_handler, error_message, ctx, type) \
    LD_ALLOC_HELPER(var, talloc, error_message, error_handler, ctx, type)

#define ld_talloc(var, error_handler, ctx, type) \
    ld_talloc_e(var, error_handler, "Error - failed to allocate memory", ctx, type)

#define ld_talloc_array_e(var, error_handler, error_message, ctx, type, count) \
    LD_ALLOC_HELPER(var, talloc_array, error_message, error_handler, ctx, type, count)

#define ld_talloc_array(var, error_handler, ctx, type, count) \
    ld_talloc_array_e(var, error_handler, "Error - failed to allocate memory", ctx, type, count)

#define ld_talloc_size_e(var, error_handler, error_message, ctx, size) \
    LD_ALLOC_HELPER(var, talloc_size, error_message, error_handler, ctx, size)

#define ld_talloc_size(var, error_handler, ctx, size) \
    ld_talloc_size_e(var, error_handler, "Error - failed to allocate memory", ctx, size)

#define ld_talloc_zero_e(var, error_handler, error_message, ctx, type) \
    LD_ALLOC_HELPER(var, talloc_zero, error_message, error_handler, ctx, type)

#define ld_talloc_zero(var, error_handler, ctx, type) \
    ld_talloc_zero_e(var, error_handler, "Error - failed to allocate memory", ctx, type)

#define ld_talloc_zero_array_e(var, error_handler, error_message, ctx, type, count) \
    LD_ALLOC_HELPER(var, talloc_zero_array, error_message, error_handler, ctx, type, count)

#define ld_talloc_zero_array(var, error_handler, ctx, type, count) \
    ld_talloc_zero_array_e(var, error_handler, "Error - failed to allocate memory", ctx, type, count)

#define ld_talloc_zero_size_e(var, error_handler, error_message, ctx, size) \
    LD_ALLOC_HELPER(var, talloc_zero_size, error_message, error_handler, ctx, size)

#define ld_talloc_zero_size(var, error_handler, ctx, size) \
    ld_talloc_zero_size_e(var, error_handler, "Error - failed to allocate memory", ctx, size)

#define ld_talloc_asprintf_e(var, error_handler, error_message, ctx, fmt, ...) \
    LD_ALLOC_HELPER(var, talloc_asprintf, error_message, error_handler, ctx, fmt, __VA_ARGS__)

#define ld_talloc_asprintf(var, error_handler, ctx, fmt, ...) \
    ld_talloc_asprintf_e(var, error_handler, "Error - failed to allocate memory", ctx, fmt, __VA_ARGS__)

#define ld_talloc_asprintf_addbuf_e(var, error_handler, error_message, ps, fmt, ...) \
    LD_ALLOC_HELPER(var, talloc_asprintf_addbuf, error_message, error_handler, ps, fmt, __VA_ARGS__)

#define ld_talloc_asprintf_addbuf(var, error_handler, ps, fmt, ...) \
    ld_talloc_asprintf_addbuf_e(var, error_handler, "Error - failed to allocate memory", ps, fmt, __VA_ARGS__)

#define ld_talloc_asprintf_append_e(var, error_handler, error_message, str, fmt, ...) \
    LD_ALLOC_HELPER(var, talloc_asprintf_append, error_message, error_handler, str, fmt, __VA_ARGS__)

#define ld_talloc_asprintf_append(var, error_handler, str, fmt, ...) \
    ld_talloc_asprintf_append_e(var, error_handler, "Error - failed to allocate memory", str, fmt, __VA_ARGS__)

#define ld_talloc_asprintf_append_buffer_e(var, error_handler, error_message, str, fmt, ...) \
    LD_ALLOC_HELPER(var, talloc_asprintf_append_buffer, error_message, error_handler, str, fmt, __VA_ARGS__)

#define ld_talloc_asprintf_append_buffer(var, error_handler, str, fmt, ...) \
    ld_talloc_asprintf_append_buffer_e(var, error_handler, "Error - failed to allocate memory", str, fmt, __VA_ARGS__)

#define ld_talloc_vasprintf_e(var, error_handler, error_message, ctx, fmt, valist) \
    LD_ALLOC_HELPER(var, talloc_vasprintf, error_message, error_handler, ctx, fmt, valist)

#define ld_talloc_vasprintf(var, error_handler, ctx, fmt, valist) \
    ld_talloc_vasprintf_e(var, error_handler, "Error - failed to allocate memory", ctx, fmt, valist)

#define ld_talloc_vasprintf_append_e(var, error_handler, error_message, str, fmt, valist) \
    LD_ALLOC_HELPER(var, talloc_vasprintf_append, error_message, str, fmt, valist)

#define ld_talloc_vasprintf_append(var, error_handler, str, fmt, valist) \
    ld_talloc_vasprintf_append_e(var, error_handler, "Error - failed to allocate memory", str, fmt, valist)

#define ld_talloc_vasprintf_append_buffer_e(var, error_handler, error_message, fmt, valist) \
    LD_ALLOC_HELPER(var, talloc_vasprintf_append_buffer, error_message, error_handler, str, fmt, valist)

#define ld_talloc_vasprintf_append_buffer(var, error_handler, fmt, valist) \
    ld_talloc_vasprintf_append_buffer_e(var, error_handler, "Error - failed to allocate memory", fmt, valist)

#define ld_talloc_new_e(var, error_handler, error_message, ctx) \
    LD_ALLOC_HELPER(var, talloc_new, error_message, error_handler, ctx)

#define ld_talloc_new(var, error_handler, ctx) \
    ld_talloc_new_e(var, error_handler, "Error - failed to allocate memory", ctx)

#define ld_talloc_memdup_e(var, error_handler, error_message, t, p, size) \
    LD_ALLOC_HELPER(var, talloc_memdup, error_message, error_handler, t, p, size)

#define ld_talloc_memdup(var, error_handler, t, p, size) \
    ld_talloc_memdup_e(var, error_handler, "Error - failed to allocate memory", t, p, size)

#define ld_talloc_strdup_e(var, error_handler, error_message, ctx, str) \
    LD_ALLOC_HELPER(var, talloc_strdup, error_message, error_handler, ctx, str)

#define ld_talloc_strdup(var, error_handler, ctx, str) \
    ld_talloc_strdup_e(var, error_handler, "Error - failed to allocate memory", ctx, str)

#define ld_talloc_strndup_e(var, error_handler, error_message, ctx, str, n) \
    LD_ALLOC_HELPER(var, talloc_strndup, error_message, error_handler, ctx, str, n)

#define ld_talloc_strndup(var, error_handler, ctx, str, n) \
    ld_talloc_strndup_e(var, error_handler, "Error - failed to allocate memory", ctx, str, n)

#define ld_talloc_realloc_e(var, error_handler, error_message, ctx, type, count) \
    LD_ALLOC_HELPER(var, talloc_realloc, error_message, error_handler, ctx, var, type, count)

#define ld_talloc_realloc(var, error_handler, ctx, type, count) \
    ld_talloc_realloc_e(var, error_handler, "Error - failed to allocate memory", ctx, type, count)

#define ld_talloc_free_e(ctx, error_handler, error_message) \
    LD_FREE_HELPER(talloc_free, error_message, error_handler, ctx)

#define ld_talloc_free(ctx, error_handler) \
ld_talloc_free_e(ctx, error_handler, "Error - failed to free memory")





#endif // LIB_DOMAIN_HELPER_PRIVATE_H
