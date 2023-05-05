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

#include <assert.h>
#include <sasl/sasl.h>

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

typedef struct option_value_t
{
    int option;
    const char* value;
} option_value_t;

const option_value_t option_strings[] =
{
    { LDAP_OPT_PROTOCOL_VERSION, "LDAP_OPT_PROTOCOL_VERSION" },
    { LDAP_OPT_SERVER_CONTROLS,  "LDAP_OPT_SERVER_CONTROLS"  },
    { LDAP_OPT_REFERRALS, "LDAP_OPT_REFERRALS" },
    { LDAP_OPT_CONNECT_ASYNC, "LDAP_OPT_CONNECT_ASYNC" },
    { LDAP_OPT_X_SASL_NOCANON, "LDAP_OPT_X_SASL_NOCANON" },
    { LDAP_OPT_X_SASL_SECPROPS, "LDAP_OPT_X_SASL_SECPROPS" },
    { LDAP_OPT_X_SASL_REALM, "LDAP_OPT_X_SASL_REALM" },
    { LDAP_OPT_X_SASL_AUTHCID, "LDAP_OPT_X_SASL_AUTHCID" },
    { LDAP_OPT_X_SASL_AUTHZID, "LDAP_OPT_X_SASL_AUTHZID" },
    { LDAP_OPT_RESULT_CODE, "LDAP_OPT_RESULT_CODE" },
    { LDAP_OPT_DIAGNOSTIC_MESSAGE, "LDAP_OPT_DIAGNOSTIC_MESSAGE" }
};
const int option_strings_size = number_of_elements(option_strings);

const char* ldap_option2string(int option)
{
    for (int i = 0; i < option_strings_size; ++i)
    {
        if (option_strings[i].option == option)
        {
            return option_strings[i].value;
        }
    }

    return "LDAP_OPT_NOT_FOUND";
}

#define set_ldap_option(ldap, option, value) \
    rc = ldap_set_option(ldap, option, value); \
    if (rc != LDAP_OPT_SUCCESS) \
    { \
        error("Unable to set ldap option %s - %s\n", ldap_option2string(option), ldap_err2string(rc)); \
        goto \
            error_exit; \
    } \

#define set_bool_option(ldap, option, value) \
    set_ldap_option(ldap, option, value ? LDAP_OPT_ON : LDAP_OPT_OFF); \

#define get_ldap_option(ldap, option, value) \
    rc = ldap_get_option(ldap, option, value); \
    if (rc != LDAP_OPT_SUCCESS) \
    { \
        error("Unable to get ldap option %s - %s\n", ldap_option2string(option), ldap_err2string(rc)); \
        goto \
            error_exit; \
    } \

/*!
 * \brief connection_configure
 * \param global_ctx
 * \param connection
 * \param config
 * \return
 */
enum OperationReturnCode connection_configure(struct ldap_global_context_t *global_ctx,
                                              struct ldap_connection_ctx_t *connection,
                                              struct ldap_connection_config_t *config)
{
    (void)(global_ctx);
    assert(connection);
    assert(config);

    int rc = ldap_initialize(&connection->ldap, config->server);

    if (rc != LDAP_SUCCESS)
    {
        error("Error initializing LDAP: %s\n", ldap_err2string(rc));
        goto
          error_exit;
    }

    connection->rmech = NULL;

    set_ldap_option(connection->ldap, LDAP_OPT_PROTOCOL_VERSION, &config->protocol_verion);

    set_bool_option(connection->ldap, LDAP_OPT_REFERRALS, config->chase_referrals);

    set_ldap_option(connection->ldap, LDAP_OPT_CONNECT_ASYNC, LDAP_OPT_ON);

    if (config->use_sasl)
    {
        set_bool_option(connection->ldap, LDAP_OPT_X_SASL_NOCANON, config->sasl_options->sasl_nocanon);
        set_ldap_option(connection->ldap, LDAP_OPT_X_SASL_SECPROPS, config->sasl_options->sasl_secprops);

        connection->ldap_defaults = talloc(global_ctx->talloc_ctx, struct ldap_sasl_defaults_t);

        get_ldap_option(connection->ldap, LDAP_OPT_X_SASL_REALM, &connection->ldap_defaults->realm);
        get_ldap_option(connection->ldap, LDAP_OPT_X_SASL_AUTHCID, &connection->ldap_defaults->authcid);
        get_ldap_option(connection->ldap, LDAP_OPT_X_SASL_AUTHZID, &connection->ldap_defaults->authzid);

        connection->ldap_defaults->flags = config->sasl_options->sasl_flags;
        connection->ldap_defaults->mechanism = config->sasl_options->mechanism;
    }

    if (config->use_start_tls)
    {
        // TODO: Implement.
    }

    connection->base = event_base_new();
    if (!connection->base)
    {
        error("Unable to create event base!");
        goto
          error_exit;
    }

    return RETURN_CODE_SUCCESS;

    error_exit:
        return RETURN_CODE_FAILURE;
}

enum OperationReturnCode connection_start_tls(struct ldap_connection_ctx_t *connection)
{
    (void)(connection);

    return RETURN_CODE_FAILURE;
}

enum OperationReturnCode connection_install_handlers(struct ldap_connection_ctx_t *connection)
{
    int fd = 0;
    int rc = 0;
    get_ldap_option(connection->ldap, LDAP_OPT_DESC, &fd);

    if (fd < 0)
    {
        error("Failed to get valid descriptor");
        goto
            error_exit;
    }

    if (evutil_make_socket_nonblocking(fd) < 0)
    {
        error("Error - evutil_make_socket_nonblocking() failed\n");
        goto
            error_exit;
    }

    connection->read_event = event_new(connection->base, fd, EV_READ | EV_PERSIST, connection_on_read, connection);
    connection->write_event = event_new(connection->base, fd, EV_WRITE | EV_PERSIST, connection_on_write, connection);

    return RETURN_CODE_SUCCESS;

    error_exit:
        ldap_unbind_ext_s(connection->ldap, NULL, NULL);
        return RETURN_CODE_FAILURE;
}

enum OperationReturnCode connection_sasl_bind(struct ldap_connection_ctx_t *connection)
{
    assert(connection);

    int rc = ldap_sasl_bind(connection->ldap, connection->ldap_params->dn, connection->ldap_defaults->mechanism,
                            connection->ldap_params->passwd, connection->ldap_params->serverctrls,
                            connection->ldap_params->clientctrls, &connection->current_msgid);
    if (rc != LDAP_SUCCESS)
    {
        // TODO: Verify that we need to perform abandon operation here.
        error("Unable to perform ldap_sasl_bind - error: %s", ldap_err2string(rc));
        ldap_unbind_ext_s(connection->ldap, NULL, NULL);
        return RETURN_CODE_FAILURE;
    }

    if (connection_install_handlers(connection) != RETURN_CODE_SUCCESS)
    {
        error("Unable to install event handlers.");
        ldap_unbind_ext_s(connection->ldap, NULL, NULL);
        return RETURN_CODE_FAILURE;
    }
    connection->on_read_operation = connection_bind_on_read;

    return RETURN_CODE_SUCCESS;
}

int sasl_interact_gssapi(LDAP *ld, unsigned flags, void *indefaults, void *in) {
    (void)(flags);

    struct ldap_sasl_defaults_t *defaults = (struct ldap_sasl_defaults_t *) indefaults;
    sasl_interact_t *interact = (sasl_interact_t *) in;

    if (ld == NULL) {
        return LDAP_PARAM_ERROR;
    }

    while (interact->id != SASL_CB_LIST_END) {
        const char *dflt = interact->defresult;

        switch (interact->id) {
            case SASL_CB_GETREALM:
                if (defaults)
                    dflt = defaults->realm;
                break;
            case SASL_CB_AUTHNAME:
                if (defaults)
                    dflt = defaults->authcid;
                break;
            case SASL_CB_PASS:
                if (defaults)
                    dflt = defaults->passwd;
                break;
            case SASL_CB_USER:
                if (defaults)
                    dflt = defaults->authzid;
                break;
            case SASL_CB_NOECHOPROMPT:
                break;
            case SASL_CB_ECHOPROMPT:
                break;
        }

        if (dflt && !*dflt) {
            dflt = NULL;
        }

        /* input must be empty */
        interact->result = (dflt && *dflt) ? dflt : "";
        interact->len = strlen((const char *) interact->result);
        interact++;
    }

    return LDAP_SUCCESS;
}

enum OperationReturnCode connection_ldap_bind(struct ldap_connection_ctx_t *connection)
{
    assert(connection);

    LDAPMessage* bind_message = NULL;

    int rc = LDAP_OTHER;
    rc = ldap_sasl_interactive_bind(connection->ldap,
                                    NULL,
                                    connection->ldap_defaults->mechanism,
                                    NULL,
                                    NULL,
                                    connection->ldap_defaults->flags,
                                    sasl_interact_gssapi,
                                    connection->ldap_defaults,
                                    bind_message,
                                    &connection->rmech,
                                    &connection->current_msgid);
    ldap_msgfree(bind_message);

    if (rc != LDAP_SUCCESS && rc != LDAP_SASL_BIND_IN_PROGRESS)
    {
        // TODO: Verify that we need to perform abandon operation here.
        error("Unable to perform ldap_sasl_interactive_bind - error: %s\n", ldap_err2string(rc));
        ldap_unbind_ext_s(connection->ldap, NULL, NULL);
        return RETURN_CODE_FAILURE;
    }

    if (connection_install_handlers(connection) != RETURN_CODE_SUCCESS)
    {
        error("Unable to install event handlers.\n");
        ldap_unbind_ext_s(connection->ldap, NULL, NULL);
        return RETURN_CODE_FAILURE;
    }
    connection->on_read_operation = connection_bind_on_read;

    return RETURN_CODE_SUCCESS;
}

void connection_on_read(int fd, short flags, void *arg)
{
    (void)(fd);
    (void)(flags);
    struct ldap_connection_ctx_t* connection = (struct ldap_connection_ctx_t*)arg;

    int rc = 0;
    LDAPMessage* result_message = NULL;
    struct timeval timeout = { 0, 10 };

    int error_code = 0;
    char *diagnostic_message = NULL;

    rc = ldap_result(connection->ldap, connection->current_msgid, LDAP_MSG_ALL, &timeout, &result_message);
    switch (rc)
    {
    case LDAP_RES_ANY:
        get_ldap_option(connection->ldap, LDAP_OPT_RESULT_CODE, (void*)&error_code);
        get_ldap_option(connection->ldap, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&diagnostic_message);
        error("Error - ldap_result failed - code: %d %s\n", error_code, diagnostic_message);
        ldap_memfree(diagnostic_message);
        ldap_memfree(result_message);
        break;
    case LDAP_RES_UNSOLICITED:
        error("Warning - Message timeout\n!");
        ldap_memfree(result_message);
        break;
    default:
        error_code = connection->on_read_operation ? connection->on_read_operation(rc, result_message, connection)
                                                   : RETURN_CODE_FAILURE;
        ldap_memfree(result_message);
        break;
    };

    error_exit:
        return;
}

void connection_on_write(int fd, short flags, void *arg)
{
    (void)(fd);
    (void)(flags);
    (void)(arg);
}

enum OperationReturnCode connection_close(struct ldap_connection_ctx_t *connection)
{
    assert(connection);
    talloc_free(connection->ldap_defaults);

    if (connection->read_event)
    {
        event_del(connection->read_event);
        event_free(connection->read_event);
    }

    if(connection->write_event) {
        event_del(connection->write_event);
        event_free(connection->write_event);
    }

    event_base_free(connection->base);
    ldap_unbind_ext(connection->ldap, NULL, NULL);
    return RETURN_CODE_FAILURE;
}

enum OperationReturnCode connection_bind_on_read(int rc, LDAPMessage * message, struct ldap_connection_ctx_t *connection)
{
    (void)(message);
    (void)(connection);

    int error_code = 0;
    char *diagnostic_message = NULL;

    switch (rc)
    {
    case LDAP_RES_BIND:
        info("Message - connection_bind_on_read - message success!\n");
        rc = ldap_sasl_interactive_bind(connection->ldap,
                                        NULL,
                                        connection->ldap_defaults->mechanism,
                                        NULL,
                                        NULL,
                                        connection->ldap_defaults->flags,
                                        sasl_interact_gssapi,
                                        connection->ldap_defaults,
                                        message,
                                        &connection->rmech,
                                        &connection->current_msgid);
        if (rc == LDAP_SASL_BIND_IN_PROGRESS)
        {
            info("Bind in progress!\n");
        }
        else if (rc == LDAP_SUCCESS)
        {
            info("Message - connection_bind_on_read - bind success!\n");
            connection->on_read_operation = NULL;
            return RETURN_CODE_SUCCESS;
        }
        else
        {
            get_ldap_option(connection->ldap, LDAP_OPT_RESULT_CODE, (void*)&error_code);
            get_ldap_option(connection->ldap, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&diagnostic_message);
            error("Error - ldap_result failed - op code: %d - code: %d %s\n", rc, error_code, diagnostic_message);
            ldap_memfree(diagnostic_message);
            return RETURN_CODE_FAILURE;
        }
    default:
        break;
    }

    return RETURN_CODE_SUCCESS;

    error_exit:
        return RETURN_CODE_FAILURE;
}