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
    LDAP_CONNECTION_STATE_INIT            = 1,
    LDAP_CONNECTION_STATE_TRANSPORT_READY = 2,
    LDAP_CONNECTION_STATE_BOUND           = 3,
    LDAP_CONNECTION_STATE_RUN             = 4,
    LDAP_CONNECTION_STATE_ERROR           = 5,
};

struct state_machine_ctx_t
{
    enum LdapConnectionState state;
    struct ldap_connection_ctx_t *ctx;
} state_machine_ctx_t;

enum OperationReturnCode csm_init(struct state_machine_ctx_t *ctx, struct ldap_connection_ctx_t *connection);
enum OperationReturnCode csm_next_state(struct state_machine_ctx_t *ctx);
enum OperationReturnCode csm_set_state(struct state_machine_ctx_t *ctx, enum LdapConnectionState state);

#endif //LIBDOMAIN_CSM_H
