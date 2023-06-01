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
#include "domain_p.h"
#include "common.h"
#include "connection.h"
#include "connection_state_machine.h"
#include "entry.h"

#include <stdio.h>

#include <talloc.h>

static const int CONNECTION_UPDATE_INTERVAL = 1000;

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

    asprintf(result->host, "%s://%s:%d", use_tls ? "ldaps" : "ldap", host, port);
    result->protocol_version = protocol_version;

    result->base_dn  = strndup(base_dn, strlen(base_dn));
    result->username = strndup(username, strlen(username));
    result->password = strndup(password, strlen(password));

    result->simple_bind = simple_bind;
    result->use_tls     = use_tls;
    result->use_sasl    = use_sasl;
    result->use_anon    = use_anon;

    result->timeout = timeout;

    result->cacertfile = strndup(cacertfile, strlen(cacertfile));
    result->certfile   = strndup(certfile, strlen(certfile));
    result->keyfile    = strndup(keyfile, strlen(keyfile));

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
        return;
    }

    if (!config)
    {
        error("Config is invalid!");
        return;
    }

    handle->talloc_ctx = talloc_new(NULL);

    handle->global_config = talloc_memdup(handle->talloc_ctx, config, sizeof (config_t));

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

enum OperationReturnCode ld_add_entry(LDHandle *handle, const char *name, const char* parent, void **entry_attrs)
{
    const char* entry_name = NULL;
    const char* entry_parent = NULL;

    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    (void)(entry_attrs);

    check_handle(handle, "ld_add_entry");

    check_string(name, entry_name, "ld_add_entry");
    check_string(parent, entry_parent, "ld_add_entry");

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    const char* dn = talloc_asprintf(talloc_ctx,"cn=%s,%s,%s", entry_name, entry_parent, handle->global_config->base_dn);

    LDAPMod **attrs = talloc_array(talloc_ctx, LDAPMod*, 1);
    attrs[0] = NULL;

    rc = add(handle->connection_ctx, dn, attrs);

    talloc_free(talloc_ctx);

    return rc;
}

enum OperationReturnCode ld_del_entry(LDHandle *handle, const char *name, const char* parent)
{
    const char* entry_name = NULL;
    const char* entry_parent = NULL;

    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    check_handle(handle, "ld_del_entry");

    check_string(name, entry_name, "ld_del_entry");
    check_string(parent, entry_parent, "ld_del_entry");

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    const char* dn = talloc_asprintf(talloc_ctx,"cn=%s,%s,%s", entry_name, entry_parent, handle->global_config->base_dn);

    rc = delete(handle->connection_ctx, dn);

    talloc_free(talloc_ctx);

    return rc;
}

enum OperationReturnCode ld_mod_entry(LDHandle *handle, const char *name, const char* parent, void **entry_attrs)
{
    const char* entry_name = NULL;
    const char* entry_parent = NULL;

    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    (void)(entry_attrs);

    check_handle(handle, "ld_mod_entry");

    check_string(name, entry_name, "ld_mod_entry");
    check_string(parent, entry_parent, "ld_mod_entry");

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    LDAPMod **attrs = talloc_array(talloc_ctx, LDAPMod*, 1);
    attrs[0] = NULL;

    const char* dn = talloc_asprintf(talloc_ctx,"cn=%s,%s,%s", entry_name, entry_parent, handle->global_config->base_dn);

    rc = modify(handle->connection_ctx, dn, attrs);

    talloc_free(talloc_ctx);

    return rc;
}

enum OperationReturnCode ld_rename_entry(LDHandle *handle, const char *old_name, const char *new_name,
                                         const char* parent)
{
    const char* entry_old_name = NULL;
    const char* entry_new_name = NULL;
    const char* entry_parent = NULL;

    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    check_handle(handle, "ld_rename_entry");

    check_string(old_name, entry_old_name, "ld_rename_entry");
    check_string(new_name, entry_new_name, "ld_rename_entry");
    check_string(parent, entry_parent, "ld_rename_entry");

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    const char* old_dn = talloc_asprintf(talloc_ctx,"cn=%s,%s,%s", entry_old_name, entry_parent, handle->global_config->base_dn);
    const char* new_dn = talloc_asprintf(talloc_ctx,"cn=%s,%s,%s", entry_new_name, entry_parent, handle->global_config->base_dn);

    const char* parent_dn = talloc_asprintf(talloc_ctx,"%s,%s", entry_parent, handle->global_config->base_dn);

    rc = ld_rename(handle->connection_ctx, old_dn, new_dn, parent_dn, true);

    talloc_free(talloc_ctx);

    return rc;
}
