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
#define LD_FREE_HELPER(func, error, error_handler, var, ...) \
    if (func(var, __VA_ARGS__) != 0) \
    { \
        var = NULL; \
        ld_error(error); \
        goto error_handler; \
    } \
    var = NULL; 

#define ld_alloc(var, error_handler, ctx, type) \
    LD_ALLOC_HELPER(var, talloc, "Error - failed to allocate memory", error_handler, ctx, type)

#define ld_alloc_array(var, error_handler, ctx, type, count) \
    LD_ALLOC_HELPER(var, talloc_array, "Error - failed to allocate memory", error_handler, ctx, type, count)

#define ld_alloc_size(var, error_handler, ctx, size) \
    LD_ALLOC_HELPER(var, talloc_size, "Error - failed to allocate memory", error_handler, ctx, size)

#define ld_alloc_zero(var, error_handler, ctx, type) \
    LD_ALLOC_HELPER(var, talloc_zero, "Error - failed to allocate memory", error_handler, ctx, type)

#define ld_alloc_zero_array(var, error_handler, ctx, type, count) \
    LD_ALLOC_HELPER(var, talloc_zero_array, "Error - failed to allocate memory", error_handler, ctx, type, count)

#define ld_alloc_zero_size(var, error_handler, ctx, size) \
    LD_ALLOC_HELPER(var, talloc_zero_size, "Error - failed to allocate memory", error_handler, ctx, size)

#define ld_alloc_asprintf(var, error_handler, ctx, fmt, ...) \
    LD_ALLOC_HELPER(var, talloc_asprintf, "Error - failed to allocate memory", error_handler, ctx, fmt, __VA_ARGS__)

#define ld_alloc_asprintf_addbuf(var, error_handler, ps, fmt, ...) \
    LD_ALLOC_HELPER(var, talloc_asprintf_addbuf, "Error - failed to allocate memory", error_handler, ps, fmt, __VA_ARGS__)

#define ld_alloc_asprintf_append(var, error_handler, str, fmt, ...) \
    LD_ALLOC_HELPER(var, talloc_asprintf_append, "Error - failed to allocate memory", error_handler, str, fmt, __VA_ARGS__)

#define ld_alloc_asprintf_append_buffer(var, error_handler, str, fmt, ...) \
    LD_ALLOC_HELPER(var, talloc_asprintf_append_buffer, "Error - failed to allocate memory", error_handler, str, fmt, __VA_ARGS__)

#define ld_alloc_vasprintf(var, error_handler, ctx, fmt, valist) \
    LD_ALLOC_HELPER(var, talloc_vasprintf, "Error - failed to allocate memory", error_handler, ctx, fmt, valist)

#define ld_alloc_vasprintf_append(var, error_handler, str, fmt, valist) \
    LD_ALLOC_HELPER(var, talloc_vasprintf_append, "Error - failed to allocate memory", str, fmt, valist)

#define ld_alloc_vasprintf_append_buffer(var, error_handler, fmt, valist) \
    LD_ALLOC_HELPER(var, talloc_vasprintf_append_buffer, "Error - failed to allocate memory", error_handler, str, fmt, valist)

#define ld_alloc_new(var, error_handler, ctx) \
    LD_ALLOC_HELPER(var, talloc_new, "Error - failed to allocate memory", error_handler, ctx)

#define ld_alloc_memdup(var, error_handler, t, p, size) \
    LD_ALLOC_HELPER(var, talloc_memdup, "Error - failed to allocate memory", error_handler, t, p, size)

#define ld_alloc_strdup(var, error_handler, ctx, str) \
    LD_ALLOC_HELPER(var, talloc_strdup, "Error - failed to allocate memory", error_handler, ctx, str)

#define ld_alloc_strndup(var, error_handler, ctx, str, n) \
    LD_ALLOC_HELPER(var, talloc_strndup, "Error - failed to allocate memory", error_handler, ctx, str, n)


#define ld_free(ctx, error_handler) \
    LD_FREE_HELPER(talloc_free, "Error - failed to free memory", error_handler, ctx)





#endif // LIB_DOMAIN_HELPER_PRIVATE_H
