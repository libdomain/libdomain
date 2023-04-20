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

#include "connection.h"

enum OperationReturnCode connection_configure(struct ldap_global_context_t *global_ctx,
                                              struct ldap_connection_ctx_t *connection,
                                              struct ldap_connection_config_t *config)
{
    (void)(global_ctx);
    (void)(connection);
    (void)(config);

    return RETURN_CODE_FAILURE;
}

enum OperationReturnCode connection_start_tls(struct ldap_connection_ctx_t *connection)
{
    (void)(connection);

    return RETURN_CODE_FAILURE;
}

enum OperationReturnCode connection_sasl_bind(struct ldap_connection_ctx_t *connection)
{
    (void)(connection);

    return RETURN_CODE_FAILURE;
}

enum OperationReturnCode connection_ldap_bind(struct ldap_connection_ctx_t *connection)
{
    (void)(connection);

    return RETURN_CODE_FAILURE;
}
