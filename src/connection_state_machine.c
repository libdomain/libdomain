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

#include "connection_state_machine.h"

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

typedef struct csm_state_value_t
{
    int state;
    const char* value;
} csm_state_value_t;

const csm_state_value_t state_strings[] =
{
    { LDAP_CONNECTION_STATE_INIT, "LDAP_CONNECTION_STATE_INIT" },
    { LDAP_CONNECTION_STATE_TRANSPORT_READY, "LDAP_CONNECTION_STATE_TRANSPORT_READY" },
    { LDAP_CONNECTION_STATE_BOUND, "LDAP_CONNECTION_STATE_BOUND" },
    { LDAP_CONNECTION_STATE_RUN, "LDAP_CONNECTION_STATE_RUN" },
    { LDAP_CONNECTION_STATE_ERROR, "LDAP_CONNECTION_STATE_ERROR" },
};
const int state_strings_size = number_of_elements(state_strings);

const char* csm_state2str(int state)
{
    for (int i = 0; i < state_strings_size; ++i)
    {
        if (state_strings[i].state == state)
        {
            return state_strings[i].value;
        }
    }

    return "STATE_NOT_FOUND";
}

enum OperationReturnCode csm_init(struct state_machine_ctx_t *ctx, struct ldap_connection_ctx_t *connection)
{
    ctx->ctx = connection;
    ctx->state = LDAP_CONNECTION_STATE_INIT;

    return RETURN_CODE_SUCCESS;
}

enum OperationReturnCode csm_next_state(struct state_machine_ctx_t *ctx)
{
    int rc = RETURN_CODE_FAILURE;
    switch (ctx->state)
    {
    case LDAP_CONNECTION_STATE_INIT:
        csm_set_state(ctx, LDAP_CONNECTION_STATE_TRANSPORT_READY);
        break;

    case LDAP_CONNECTION_STATE_TRANSPORT_READY:
        if (ctx->ctx->ldap_defaults)
        {
            rc = connection_sasl_bind(ctx->ctx);
        }
        else
        {
            // TODO: Implement simple bind or remove it completely.
        }
        csm_set_state(ctx, rc == RETURN_CODE_SUCCESS ? LDAP_CONNECTION_STATE_BOUND : LDAP_CONNECTION_STATE_ERROR);
        return rc;

    case LDAP_CONNECTION_STATE_BOUND:
        csm_set_state(ctx, LDAP_CONNECTION_STATE_RUN);
        // TODO: Send signal that we connected.
        break;

    case LDAP_CONNECTION_STATE_RUN:
        // TODO: Await signals to either close or transition to error state.
        break;

    case LDAP_CONNECTION_STATE_ERROR:

        break;
    default:
        error("Unknown state code: %d\n", ctx->state);
        return RETURN_CODE_FAILURE;
    }

    return RETURN_CODE_SUCCESS;
}

enum OperationReturnCode csm_set_state(struct state_machine_ctx_t *ctx, enum LdapConnectionState state)
{
    info("Connection [%h] - transition from state: %s to state: %s\n", csm_state2str(ctx->state), csm_state2str(state));

    ctx->state = state;

    return RETURN_CODE_SUCCESS;
}