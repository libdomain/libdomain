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
#ifndef LIBDOMAIN_COMMON_H
#define LIBDOMAIN_COMMON_H

#include <ldap.h>
#include <talloc.h>

enum OperationReturnCode
{
    RETURN_CODE_SUCCESS               = 1,
    RETURN_CODE_FAILURE               = 2,
    RETURN_CODE_MISSING_ATTRIBUTE     = 3,
    RETURN_CODE_OPERATION_IN_PROGRESS = 4,
    RETURN_CODE_REPEAT_LAST_OPERATION = 5,
};

typedef struct ldap_global_context_t
{
    LDAP *global_ldap;
    TALLOC_CTX *talloc_ctx;
} ldap_global_context_t;

void error(const char *format, ...);
void warning(const char *format, ...);
void info(const char *format, ...);

#endif //LIBDOMAIN_COMMON_H
