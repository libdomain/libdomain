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
#ifndef LIBDOMAIN_CSM_H
#define LIBDOMAIN_CSM_H

#include "connection.h"

enum LdapConnectionState
{
    LDAP_CONNECTION_STATE_INIT             = 1, //!< Initial state of the LDAP connection.
    LDAP_CONNECTION_STATE_TLS_NEGOTIATION  = 2, //!< The connection is in the process of negotiating TLS encryption.
    LDAP_CONNECTION_STATE_TRANSPORT_READY  = 3, //!< The underlying transport layer is ready for communication.
    LDAP_CONNECTION_STATE_BIND_IN_PROGRESS = 4, //!< The connection is in the process of binding.
    LDAP_CONNECTION_STATE_BOUND            = 5, //!< The connection has successfully performed LDAP bound.
    LDAP_CONNECTION_STATE_DETECT_DIRECTORY = 6, //!< The connection is in the process of detecting type of the LDAP directory service.
    LDAP_CONNECTION_STATE_RUN              = 7, //!< The LDAP connection is active and ready for operations.
    LDAP_CONNECTION_STATE_ERROR            = 8, //!< The LDAP connection is in an error state.
    LDAP_CONNECTION_STATE_LOAD_SCHEMA      = 9, //!< The LDAP connection is in the process of loading schema.
};

/*!
 * @brief state_machine_ctx_t - Struct represents connection state machine.
 */
typedef struct state_machine_ctx_t
{
    enum LdapConnectionState state;             //!< State of the connection.
    struct ldap_connection_ctx_t *ctx;          //!< Connection context.
} state_machine_ctx_t;

enum OperationReturnCode csm_init(struct state_machine_ctx_t *ctx, struct ldap_connection_ctx_t *connection);
enum OperationReturnCode csm_next_state(struct state_machine_ctx_t *ctx);
enum OperationReturnCode csm_set_state(struct state_machine_ctx_t *ctx, enum LdapConnectionState state);
bool csm_is_in_state(struct state_machine_ctx_t *ctx, enum LdapConnectionState state);

#endif //LIBDOMAIN_CSM_H
