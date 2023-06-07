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

#include "directory.h"

/**
 * @brief directory_get_type Request LDAP type from service.
 * @param[in] connection connection to use
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode directory_get_type(struct ldap_connection_ctx_t *connection)
{
    (void)(connection);

    return RETURN_CODE_FAILURE;
}

/**
 * @brief directory_parse_result Parse results of directory type request and intialize the connection directory type.
 * @param[in] connection connection to use
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode directory_parse_result(struct ldap_connection_ctx_t *connection)
{
    (void)(connection);

    return RETURN_CODE_FAILURE;
}
