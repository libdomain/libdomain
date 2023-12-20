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
    RETURN_CODE_SUCCESS               = 1,          //!< Operation was successful.
    RETURN_CODE_FAILURE               = 2,          //!< Operation ended up in error state.
    RETURN_CODE_MISSING_ATTRIBUTE     = 3,          //!< Required attribute for LDAP entry was not provided.
    RETURN_CODE_OPERATION_IN_PROGRESS = 4,          //!< Current operation is in progress.
    RETURN_CODE_REPEAT_LAST_OPERATION = 5,          //!< Last function call must be repeated.
};

/*!
 * @brief ldap_global_context_t - One of the contexts associated with LDHandle.
 * @see LDHandle
 */
typedef struct ldap_global_context_t
{
    LDAP *global_ldap;                              //!< Global ldap context for sharing between connections.
    TALLOC_CTX *talloc_ctx;                         //!< Pointer to valid TALLOC_CTX. We use this internally
                                                    //!< when we working with ldap entries.
} ldap_global_context_t;

void ld_error(const char *format, ...);
void ld_warning(const char *format, ...);
void ld_info(const char *format, ...);

#endif //LIBDOMAIN_COMMON_H
