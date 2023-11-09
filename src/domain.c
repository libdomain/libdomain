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
 * @brief ld_create_config     Fills fields of configuration structure.
 * @param[in] host             Hostname of LDAP server.
 * @param[in] port             Port number.
 * @param[in] protocol_version LDAP protocol version.
 * @param[in] base_dn          DN to use during bind.
 * @param[in] username         Username to use
 * @param[in] password         User password
 * @param[in] simple_bind      If we going to perform simple bind instead of interactive bind.
 * @param[in] use_tls          If we going to use TLS encryption.
 * @param[in] use_sasl         If we going to enable SASL.
 * @param[in] use_anon         If we going to enable anonymous logon.
 * @param[in] timeout          Timeout to read and write operations.
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

    if (port > 0)
    {
        result->host = talloc_asprintf(NULL, "%s:%d", host, port);
    }
    else
    {
        result->host = talloc_asprintf(NULL, "%s", host);
    }

    result->protocol_version = protocol_version;

    result->base_dn  = strndup(base_dn, strlen(base_dn));
    result->username = username ? strndup(username, strlen(username)) : NULL;
    result->password = password ? strndup(password, strlen(password)) : NULL;

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
 * @brief ld_init     Initializes the library allowing us to performing various operations.
 * @param[out] handle Pointer to libdomain session handle.
 * @param[in]  config Configuration of the connections.
 */
void ld_init(LDHandle** handle, const config_t* config)
{
    *handle = malloc(sizeof(LDHandle));

    if (!*handle)
    {
        error("Unable to allocate memory for ldhandle");
        return;
    }

    if (!config)
    {
        error("Config is invalid!");
        return;
    }

    (*handle)->talloc_ctx = talloc_new(NULL);

    (*handle)->global_config = talloc_memdup((*handle)->talloc_ctx, config, sizeof (config_t));

    (*handle)->global_ctx = talloc_zero((*handle)->talloc_ctx, ldap_global_context_t);
    (*handle)->connection_ctx = talloc_zero((*handle)->talloc_ctx, ldap_connection_ctx_t);
    (*handle)->config_ctx = talloc_zero((*handle)->talloc_ctx, ldap_connection_config_t);

    (*handle)->global_ctx->talloc_ctx = (*handle)->talloc_ctx;

    (*handle)->config_ctx->server = config->host;
    (*handle)->config_ctx->protocol_verion = config->protocol_version;

    (*handle)->config_ctx->use_sasl = config->use_sasl;
    (*handle)->config_ctx->use_start_tls = config->use_tls;
    (*handle)->config_ctx->chase_referrals = false;

    int debug_level = -1;
    ldap_set_option((*handle)->connection_ctx->ldap, LDAP_OPT_DEBUG_LEVEL, &debug_level);

    if (config->use_sasl)
    {
        (*handle)->config_ctx->sasl_options = talloc((*handle)->global_ctx->talloc_ctx, struct ldap_sasl_options_t);
        (*handle)->config_ctx->sasl_options->mechanism = config->simple_bind ? LDAP_SASL_SIMPLE : "GSSAPI";
        (*handle)->config_ctx->sasl_options->passwd = talloc_strdup((*handle)->global_ctx->talloc_ctx, config->password);

        (*handle)->config_ctx->sasl_options->sasl_nocanon = true;
        (*handle)->config_ctx->sasl_options->sasl_secprops = "maxssf=56";
        (*handle)->config_ctx->sasl_options->sasl_flags = LDAP_SASL_QUIET;

    }

    (*handle)->connection_ctx->ldap_params = talloc((*handle)->global_ctx->talloc_ctx, struct ldap_sasl_params_t);
    (*handle)->connection_ctx->ldap_params->dn = talloc_asprintf((*handle)->global_ctx->talloc_ctx,
                                                                 "cn=%s,%s",config->username, config->base_dn);
    (*handle)->connection_ctx->ldap_params->passwd = talloc((*handle)->global_ctx->talloc_ctx, struct berval);
    (*handle)->connection_ctx->ldap_params->passwd->bv_len = config->password ? strlen(config->password) : 0;
    (*handle)->connection_ctx->ldap_params->passwd->bv_val = config->password ? talloc_strdup((*handle)->global_ctx->talloc_ctx,
                                                                           config->password) : NULL;
    (*handle)->connection_ctx->ldap_params->clientctrls = NULL;
    (*handle)->connection_ctx->ldap_params->serverctrls = NULL;

    int rc = connection_configure((*handle)->global_ctx, (*handle)->connection_ctx, (*handle)->config_ctx);

    if (rc != RETURN_CODE_SUCCESS)
    {
        error("Unable to configure connection");
        return;
    }

    (*handle)->connection_ctx->handle = (*handle);
}

static void connection_update(verto_ctx *ctx, verto_ev *ev)
{
    (void)(ctx);

    struct ldap_connection_ctx_t* connection = verto_get_private(ev);

    // TODO: Implement error checking.
    csm_next_state(connection->state_machine);

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_RUN
     || connection->state_machine->state == LDAP_CONNECTION_STATE_ERROR)
    {
        verto_del(ev);
    }
}

/**
 * @brief ld_install_default_handlers Installs default handlers to control connection. This method must be
 * called before performing any operations.
 * @param[in] handle Pointer to libdomain session handle.
 */
void ld_install_default_handlers(LDHandle* handle)
{
    if (!handle)
    {
        error("Invalid handle was provided - ld_install_default_handlers\n");
        return;
    }

    verto_ev* ev = verto_add_timeout(handle->connection_ctx->base, VERTO_EV_FLAG_PERSIST, connection_update,
                                     CONNECTION_UPDATE_INTERVAL);
    verto_set_private(ev, handle->connection_ctx, NULL);
}

/**
 * @brief ld_install_handler If we need to install custom error callback this method allows us to do so.
 * @param[in] handle Pointer to libdomain session handle.
 */
void ld_install_handler(LDHandle* handle, verto_callback *callback, time_t interval)
{
    if (!handle)
    {
        error("Invalid handle was provided - ld_install_handler\n");
        return;
    }

    verto_ev* ev = verto_add_timeout(handle->connection_ctx->base, VERTO_EV_FLAG_PERSIST, callback, interval);
    verto_set_private(ev, handle->connection_ctx, NULL);
}

/**
 * @brief ld_exec Start main event cycle. You don't need to call this function if there is already existing
 * event loop e.g. inside of Qt application.
 * @param[in] handle Pointer to libdomain session handle.
 */
void ld_exec(LDHandle* handle)
{
    if (!handle)
    {
        error("Invalid handle was provided - ld_exec\n");
        return;
    }

    verto_run(handle->connection_ctx->base);
}

/**
 * @brief ld_exec_once Cycles through event list once. May block.
 * @param[in] handle Pointer to libdomain session handle.
 */
void ld_exec_once(LDHandle* handle)
{
    if (!handle)
    {
        error("Invalid handle was provided - ld_exec_once\n");
        return;
    }

    verto_run_once(handle->connection_ctx->base);
}

/**
 * @brief ld_free Free library handle and resources associated with it. After freeing the handle you can no longer
 * perform any operations.
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

static LDAPMod ** fill_attributes(LDAPAttribute_t **entry_attrs, TALLOC_CTX *talloc_ctx, int mod_op)
{
    int attr_count = 0;
    int attr_index = 0;

    while (entry_attrs[attr_count] != NULL)
    {
        attr_count++;
    }

    LDAPMod **attrs = talloc_array(talloc_ctx, LDAPMod*, attr_count + 1);

    while (entry_attrs[attr_index] != NULL)
    {
        attrs[attr_index] = talloc(talloc_ctx, LDAPMod);
        attrs[attr_index]->mod_op = mod_op;
        attrs[attr_index]->mod_type = talloc_strdup(talloc_ctx, entry_attrs[attr_index]->name);

        int val_count = 0;
        while (entry_attrs[attr_index]->values[val_count] != NULL)
        {
            val_count++;
        }

        attrs[attr_index]->mod_values = talloc_array(talloc_ctx, char*, val_count + 1);

        int val_index = 0;
        while (entry_attrs[attr_index]->values[val_index] != NULL)
        {
            attrs[attr_index]->mod_values[val_index] = talloc_strdup(talloc_ctx, entry_attrs[attr_index]->values[val_index]);
            val_index++;
        }
        attrs[attr_index]->mod_values[val_count] = NULL;

        attr_index++;
    }

    attrs[attr_count] = NULL;

    return attrs;
}

/**
 * @brief ld_add_entry    Creates the entry.
 * @param[in] handle      Pointer to libdomain session handle.
 * @param[in] name        Name of the entry.
 * @param[in] parent      Parent container that holds the entry.
 * @param[in] entry_attrs List of the attributes to create entry with.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_add_entry(LDHandle *handle, const char *name, const char* parent, const char* prefix,
                                      LDAPAttribute_t **entry_attrs)
{
    const char* entry_name = NULL;
    const char* entry_parent = NULL;

    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    (void)(entry_attrs);

    check_handle(handle, "ld_add_entry");

    check_string(name, entry_name, "ld_add_entry");
    check_string(parent, entry_parent, "ld_add_entry");

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    const char* dn = talloc_asprintf(talloc_ctx,"%s=%s,%s", prefix, entry_name, entry_parent);

    LDAPMod **attrs = fill_attributes(entry_attrs, talloc_ctx, LDAP_MOD_ADD);

    rc = add(handle->connection_ctx, dn, attrs);

    talloc_free(talloc_ctx);

    return rc;
}

/**
 * @brief ld_del_entry Deletes entry.
 * @param[in] handle   Pointer to libdomain session handle.
 * @param[in] name     Name of the entry.
 * @param[in] parent   Parent container that holds the entry.
 * @param[in] prefix   Prefix of the entry.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_del_entry(LDHandle *handle, const char *name, const char* parent, const char* prefix)
{
    const char* entry_name = NULL;
    const char* entry_parent = NULL;

    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    check_handle(handle, "ld_del_entry");

    check_string(name, entry_name, "ld_del_entry");
    check_string(parent, entry_parent, "ld_del_entry");

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    const char* dn = talloc_asprintf(talloc_ctx,"%s=%s,%s", prefix, entry_name, entry_parent);

    rc = delete(handle->connection_ctx, dn);

    talloc_free(talloc_ctx);

    return rc;
}

/**
 * @brief ld_mod_entry    Modifies the entry.
 * @param[in] handle      Pointer to libdomain session handle.
 * @param[in] name        Name of the entry.
 * @param[in] parent      Parent container that holds the entry.
 * @param[in] entry_attrs List of the attributes to modify.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_mod_entry(LDHandle *handle, const char *name, const char* parent, const char* prefix,
                                      LDAPAttribute_t **entry_attrs)
{
    const char* entry_name = NULL;
    const char* entry_parent = NULL;

    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    (void)(entry_attrs);

    check_handle(handle, "ld_mod_entry");

    check_string(name, entry_name, "ld_mod_entry");
    check_string(parent, entry_parent, "ld_mod_entry");

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    LDAPMod **attrs = fill_attributes(entry_attrs, talloc_ctx, LDAP_MOD_REPLACE);

    const char* dn = talloc_asprintf(talloc_ctx,"%s=%s,%s", prefix, entry_name, entry_parent);

    rc = modify(handle->connection_ctx, dn, attrs);

    talloc_free(talloc_ctx);

    return rc;
}

/**
 * @brief ld_rename_entry Renames the entry.
 * @param[in] handle      Pointer to libdomain session handle.
 * @param[in] old_name    Old name of the entry.
 * @param[in] new_name    New name of the entry.
 * @param[in] parent      Parent container that holds the entry.
 * @param[in] prefix      Prefix for entry type.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_rename_entry(LDHandle *handle, const char *old_name, const char *new_name,
                                         const char* parent, const char* prefix)
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

    const char* old_dn = talloc_asprintf(talloc_ctx,"%s=%s,%s", prefix, entry_old_name, entry_parent);
    const char* new_dn = talloc_asprintf(talloc_ctx,"%s=%s", prefix, entry_new_name);

    rc = ld_rename(handle->connection_ctx, old_dn, new_dn, entry_parent, true);

    talloc_free(talloc_ctx);

    return rc;
}

LDAPAttribute_t **assign_default_attribute_values(TALLOC_CTX *talloc_ctx,
                                                  attribute_value_pair_t default_attrs[],
                                                  int size)
{
    LDAPAttribute_t **entry_attrs  = talloc_array(talloc_ctx, LDAPAttribute_t*, size + 1);

    for (int i = 0; i < size; ++i)
    {
        entry_attrs[i] = talloc(talloc_ctx, LDAPAttribute_t);
        entry_attrs[i]->name = default_attrs[i].name;

        if (default_attrs[i].value[0] != NULL)
        {
            int attribute_count = 0;
            while (default_attrs[i].value[attribute_count] != NULL) {
                ++attribute_count;
            }

            entry_attrs[i]->values = talloc_array(talloc_ctx, char*, attribute_count + 1);

            for (int j = 0; j < attribute_count; ++j)
            {
                entry_attrs[i]->values[j] = talloc_strdup(talloc_ctx, default_attrs[i].value[j]);
            }
            entry_attrs[i]->values[attribute_count] = NULL;
        }
        else
        {
            entry_attrs[i]->values = talloc_array(talloc_ctx, char*, 2);
            entry_attrs[i]->values[0] = talloc_strdup(talloc_ctx, "");
            entry_attrs[i]->values[1] = NULL;
        }
    }
    entry_attrs[size] = NULL;

    return entry_attrs;
}

/**
 * @brief ld_install_error_handler Allows us to install custom error handle for our application.
 * @param[in] handle   Pointer to libdomain session handle.
 * @param[in] callback Callback to call.
 */
void ld_install_error_handler(LDHandle *handle, error_callback_fn callback)
{
    if (!handle)
    {
        error("Invalid handle - ld_install_error_handler\n");
        return;
    }

    if (!callback)
    {
        error("Invalid callback - ld_install_error_handler\n");
        return;
    }

    handle->connection_ctx->on_error_operation = (operation_callback_fn)callback;
}

/**
 * @brief ld_mod_entry_attrs Modifies list of attributes using supplied operation.
 * @param[in] handle         Pointer to libdomain session handle.
 * @param[in] name           Name of the entry.
 * @param[in] parent         Parent container that holds the entry.
 * @param[in] prefix         Prefix of the entry.
 * @param[in] entry_attrs    List of the attributes to modify.
 * @param[in] opcode         Code of operation e.g. LDAP_MOD_REPLACE.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_mod_entry_attrs(LDHandle *handle, const char *name, const char *parent, const char *prefix,
                                            LDAPAttribute_t **entry_attrs, int opcode)
{
    const char* entry_name = NULL;
    const char* entry_parent = NULL;

    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    (void)(entry_attrs);

    check_handle(handle, "ld_mod_entry_attrs");

    check_string(name, entry_name, "ld_mod_entry_attrs");
    check_string(parent, entry_parent, "ld_mod_entry_attrs");

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    LDAPMod **attrs = fill_attributes(entry_attrs, talloc_ctx, opcode);

    const char* dn;
    if (strlen(prefix) > 0)
    {
        dn = talloc_asprintf(talloc_ctx,"%s=%s,%s", prefix, entry_name, entry_parent);
    }
    else
    {
        dn = talloc_asprintf(talloc_ctx,"%s,%s", entry_name, entry_parent);
    }

    rc = modify(handle->connection_ctx, dn, attrs);

    talloc_free(talloc_ctx);

    return rc;
}
