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

#include "domain.h"
#include "common.h"
#include "connection.h"
#include "connection_state_machine.h"

#include <talloc.h>

static const int CONNECTION_UPDATE_INTERVAL = 1000;

typedef struct ldhandle
{
    TALLOC_CTX* talloc_ctx;
    struct ldap_global_context_t* global_ctx;
    struct ldap_connection_ctx_t* connection_ctx;
    struct ldap_connection_config_t* config_ctx;
} LDHandle;

typedef struct config_s
{
    char* host;
    int protocol_version;

    char* base_dn;
    char* username;
    char* password;

    bool simple_bind;
    bool use_tls;
    bool use_sasl;
    bool use_anon;

    int  timeout;

    char* cacertfile;
    char* certfile;
    char* keyfile;
} config_t;

/**
 * @brief ld_create_config
 * @param[in] host             Hostname of LDAP server.
 * @param[in] port             Port number.
 * @param[in] protocol_version LDAP protocol version.
 * @param[in] base_dn          DN to use during bind.
 * @param[in] username
 * @param[in] password
 * @param[in] simple_bind
 * @param[in] use_tls
 * @param[in] use_sasl
 * @param[in] use_anon
 * @param[in] timeout
 * @param[in] cacertfile
 * @param[in] certfile
 * @param[in] keyfile
 * @return
 *        - NULL      on failure.
 *        - config_t* on success.
 */
config_t *ld_create_config(char *host,
                           int port,
                           int protocol_version,
                           char* base_dn,
                           char* username,
                           char* password,
                           bool simple_bind,
                           bool use_tls,
                           bool use_sasl,
                           bool use_anon,
                           int timeout,
                           char *cacertfile,
                           char *certfile,
                           char *keyfile)
{
    config_t *result = malloc(sizeof(config_t));

    if (!result)
    {
        error("Unable to allocate memory for config_t");
        return NULL;
    }

    result->host = talloc_asprintf(ctx, "%s://%s:%d", use_tls ? "ldaps" : "ldap", host, port);
    result->protocol_version = protocol_version;

    result->base_dn  = talloc_strdup(ctx, base_dn);
    result->username = talloc_strdup(ctx, username);
    result->password = talloc_strdup(ctx, password);

    result->simple_bind = simple_bind;
    result->use_tls     = use_tls;
    result->use_sasl    = use_sasl;
    result->use_anon    = use_anon;

    result->timeout = timeout;

    result->cacertfile = talloc_strdup(ctx, cacertfile);
    result->certfile   = talloc_strdup(ctx, certfile);
    result->keyfile    = talloc_strdup(ctx, keyfile);

    return result;
}

/**
 * @brief ld_init
 * @param[out] handle
 * @param[in]  config
 */
void ld_init(LDHandle* handle, const config_t* config)
{
    handle = malloc(sizeof(LDHandle));

    if (!handle)
    {
        error("Unable to allocate memory for ldhandle");
    }

    handle->talloc_ctx = talloc_new(NULL);
    handle->global_ctx = talloc_zero(handle->talloc_ctx, ldap_global_context_t);
    handle->connection_ctx = talloc_zero(handle->talloc_ctx, ldap_connection_ctx_t);
    handle->config_ctx = talloc_zero(handle->talloc_ctx, ldap_connection_config_t);

    handle->global_ctx->talloc_ctx = handle->talloc_ctx;

    handle->config_ctx->server = config->host;
    handle->config_ctx->protocol_verion = config->protocol_version;

    handle->config_ctx->use_sasl = config->use_sasl;
    handle->config_ctx->use_start_tls = config->use_tls;
    handle->config_ctx->chase_referrals = false;

    int rc = connection_configure(handle->global_ctx, handle->connection_ctx, handle->config_ctx);

    if (rc != RETURN_CODE_SUCCESS)
    {
        error("Unable to configure connection");
    }
}

static void connection_update(verto_ctx *ctx, verto_ev *ev)
{
    (void)(ctx);

    struct ldap_connection_ctx_t* connection = verto_get_private(ev);

    csm_next_state(connection->state_machine);

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_RUN
     || connection->state_machine->state == LDAP_CONNECTION_STATE_ERROR)
    {
        verto_del(ev);
    }
}

/**
 * @brief ld_install_handlers
 * @param[in] handle Pointer to libdomain session handle.
 */
void ld_install_handlers(LDHandle* handle)
{
    if (!handle)
    {
        error("Invalid handle was provided - ld_install_handlers");
        return;
    }

    verto_ev* ev = verto_add_timeout(handle->connection_ctx->base, VERTO_EV_FLAG_PERSIST, connection_update, CONNECTION_UPDATE_INTERVAL);
    verto_set_private(ev, handle->connection_ctx, NULL);
}

/**
 * @brief ld_exec
 * @param[in] handle Pointer to libdomain session handle.
 */
void ld_exec(LDHandle* handle)
{
    if (!handle)
    {
        error("Invalid handle was provided - ld_exec");
        return;
    }

    verto_run(handle->connection_ctx->base);
}

/**
 * @brief ld_exec_once
 * @param[in] handle Pointer to libdomain session handle.
 */
void ld_exec_once(LDHandle* handle)
{
    if (!handle)
    {
        error("Invalid handle was provided - ld_exec_once");
        return;
    }

    verto_run_once(handle->connection_ctx->base);
}

/**
 * @brief ld_free
 * @param[in] handle Pointer to libdomain session handle.
 */
void ld_free(LDHandle* handle)
{
    if (!handle)
    {
        error("Invalid handle was provided - ld_free");
        return;
    }

    connection_close(handle->connection_ctx);
    talloc_free(handle->talloc_ctx);
    free(handle);
}
