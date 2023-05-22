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

#include "entry.h"
#include "connection.h"

/**
 * @brief add This function wraps ldap_add_ext function associating it with connection.
 * @param[in] connection Connection to work with.
 * @param[in] dn         The name of the entry to add. If NULL, a zero length DN is sent to the server.
 * @param[in] attrs      The entry's attributes, specified using the LDAPMod structure
 *                       defined for ldap_modify(). The mod_type and mod_vals
 *                       fields MUST be filled in.  The mod_op field is ignored
 *                       unless ORed with the constant LDAP_MOD_BVALUES, used to
 *                       select the mod_bvalues case of the mod_vals union.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode add(struct ldap_connection_ctx_t* connection, const char *dn, LDAPMod **attrs)
{
    int rc = ldap_add_ext(connection->ldap, dn, attrs, NULL, NULL, &connection->current_msgid);
    if (rc != LDAP_SUCCESS)
    {
        error("Unable to add entry: %s\n", ldap_err2string(rc));
        return RETURN_CODE_FAILURE;
    }
    connection->on_read_operation = add_on_read;

    return RETURN_CODE_SUCCESS;
}

/**
 * @brief add_on_write   This callback called on complition of ldap add operation.
 * @param[in] rc         Return code of ldap_result.
 * @param[in] message    Message received from ldap.
 * @param[in] connection Connection to work with.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode add_on_read(int rc, LDAPMessage *message, struct ldap_connection_ctx_t *connection)
{
    int error_code = LDAP_SUCCESS;
    char *diagnostic_message = NULL;

    switch (rc)
    {
    case LDAP_RES_ADD:
    {
        char *dn = NULL;

        connection->on_read_operation = NULL;

        ldap_parse_result(connection->ldap, message, &error_code, &dn, &diagnostic_message, NULL, NULL, false);
        info("ldap_result: %s %s %d\n", diagnostic_message, ldap_err2string(error_code), error_code);
        ldap_memfree(diagnostic_message);
        ldap_memfree(dn);

        switch (error_code)
        {
        case LDAP_SUCCESS:
        case LDAP_ALREADY_EXISTS:
            return RETURN_CODE_SUCCESS;

        default:
            return RETURN_CODE_FAILURE;
        }
    }
        break;
    default:
    {
        ldap_get_option(connection->ldap, LDAP_OPT_RESULT_CODE, (void*)&error_code);
        ldap_get_option(connection->ldap, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&diagnostic_message);
        error("ldap_result failed: %s\n", diagnostic_message);
        ldap_memfree(diagnostic_message);
    }
        break;
    }

    return RETURN_CODE_FAILURE;
}

/**
 * @brief search         Function wraps ldap search operation associating it with connection.
 * @param[in] connection Connection to work with.
 * @param[in] base_dn    The dn of the entry at which to start the search.
 *                       If NULL, a zero length DN is sent to the server.
 * @param[in] scope      One of LDAP_SCOPE_BASE (0x00), LDAP_SCOPE_ONELEVEL (0x01),
 *                       or LDAP_SCOPE_SUBTREE (0x02), indicating the scope of the search.
 * @param[in] filter     A character string as described in [13], representing the
 *                       search filter.  The value NULL can be passed to indicate
 *                       that the filter "(objectclass=*)" which matches all entries
 *                       is to be used.  Note that if the caller of the API is using
 *                       LDAPv2, only a subset of the filter functionality described
 *                       in [13] can be successfully used.
 * @param[in] attrs      A NULL-terminated array of strings indicating which attributes
 *                       to return for each matching entry. Passing NULL for
 *                       this parameter causes all available user attributes to be
 *                       retrieved.  The special constant string LDAP_NO_ATTRS
 *                       ("1.1") MAY be used as the only string in the array to
 *                       indicate that no attribute types are to be returned by the
 *                       server.  The special constant string LDAP_ALL_USER_ATTRS
 *                       ("*") can be used in the attrs array along with the names
 *                       of some operational attributes to indicate that all user
 *                       attributes plus the listed operational attributes are to be
 *                       returned.
 * @param[in] attrsonly  A boolean value that MUST be zero if both attribute types
 *                       and values are to be returned, and non-zero if only types
 *                       are wanted.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode  search(struct ldap_connection_ctx_t* connection, const char *base_dn, int scope,
                                 const char *filter, char **attrs, bool attrsonly)
{
    int rc = ldap_search_ext(connection->ldap,
                    base_dn,
                    scope,
                    filter,
                    attrs,
                    attrsonly,
                    NULL,
                    NULL,
                    NULL,
                    LDAP_NO_LIMIT,
                    &connection->current_msgid);
    if (rc != LDAP_SUCCESS)
    {
        error("Unable to create search request: %s\n", ldap_err2string(rc));
        return RETURN_CODE_FAILURE;
    }
    connection->on_read_operation = search_on_read;

    return RETURN_CODE_SUCCESS;
}

/**
 * @brief search_on_read This callback called upon complition of ldap search operation.
 * @param[in] rc         Return code of ldap_result.
 * @param[in] message    Message received from ldap.
 * @param[in] connection Connection to work with.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode search_on_read(int rc, LDAPMessage *message, struct ldap_connection_ctx_t *connection)
{
    const char *attribute   = NULL;
    struct berval **values  = NULL;
    BerElement *ber_element = NULL;

    int error_code = 0;
    char *diagnostic_message = NULL;

    switch (rc)
    {
    case LDAP_RES_SEARCH_ENTRY:
    case LDAP_RES_SEARCH_RESULT:
    {
        while (message)
        {
            char* dn = ldap_get_dn(connection->ldap, message);
            fprintf(stderr, "Search result - entry dn: %s\n", dn);
            ldap_memfree(dn);

            attribute = ldap_first_attribute(connection->ldap, message, &ber_element);
            while (attribute != NULL)
            {
                values = ldap_get_values_len(connection->ldap, message, attribute);
                for(int index = 0; index < ldap_count_values_len(values); index++)
                {
                    printf("%s: %s\n", attribute, values[index]->bv_val);
                }
                ldap_value_free_len(values);
                attribute = ldap_next_attribute(connection->ldap, message, ber_element);
            };
            ber_free(ber_element, 0);

            message = ldap_next_message(connection->ldap, message);
        }

        return RETURN_CODE_SUCCESS;
    }
        break;
    case LDAP_RES_SEARCH_REFERENCE:
        info("Received search referral but not following it!");
        return RETURN_CODE_SUCCESS;
    default:
    {
        ldap_get_option(connection->ldap, LDAP_OPT_RESULT_CODE, (void*)&error_code);
        ldap_get_option(connection->ldap, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&diagnostic_message);
        error("ldap_result failed: %s\n", diagnostic_message);
        ldap_memfree(diagnostic_message);
    }
        break;
    }

    return RETURN_CODE_FAILURE;
}

/**
 * @brief modify This function wraps ldap_modify_ext.
 * @param[in] connection Connection to work with.
 * @param[in] dn         The name of the entry to modify. If NULL, a zero length DN is sent to the server.

 * @param[in] attrs      A NULL-terminated array of modifications to make to the entry.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode modify(struct ldap_connection_ctx_t* connection, const char *dn, LDAPMod **attrs)
{
    int rc = ldap_modify_ext(connection->ldap,
                             dn,
                             attrs,
                             NULL,
                             NULL,
                             &connection->current_msgid);
    if (rc != LDAP_SUCCESS)
    {
        error("Unable to create modify request: %s\n", ldap_err2string(rc));
        return RETURN_CODE_FAILURE;
    }
    connection->on_read_operation = modify_on_read;

    return RETURN_CODE_SUCCESS;
}

/**
 * @brief modify_on_read This callback called upon complition of ldap modify operation.
 * @param rc             Return code of ldap_result.
 * @param message        Message received from ldap.
 * @param connection     Connection to work with.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode modify_on_read(int rc, LDAPMessage *message, struct ldap_connection_ctx_t *connection)
{
    int error_code = LDAP_SUCCESS;
    char *diagnostic_message = NULL;

    switch (rc)
    {
    case LDAP_RES_MODIFY:
    {
        char *dn = NULL;

        connection->on_read_operation = NULL;

        ldap_parse_result(connection->ldap, message, &error_code, &dn, &diagnostic_message, NULL, NULL, false);
        info("ldap_result: %s %s %d\n", diagnostic_message, ldap_err2string(error_code), error_code);
        ldap_memfree(diagnostic_message);
        ldap_memfree(dn);

        switch (error_code)
        {
        case LDAP_SUCCESS:
            return RETURN_CODE_SUCCESS;

        default:
            return RETURN_CODE_FAILURE;
        }
    }
        break;
    default:
    {
        ldap_get_option(connection->ldap, LDAP_OPT_RESULT_CODE, (void*)&error_code);
        ldap_get_option(connection->ldap, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&diagnostic_message);
        error("ldap_result failed: %s\n", diagnostic_message);
        ldap_memfree(diagnostic_message);
    }
        break;
    }

    return RETURN_CODE_FAILURE;
}

/**
 * @brief delete Function wraps ldap_delete_ext.
 * @param[in] connection Connection to work with.
 * @param[in] dn         The name of the entry to delete.  If NULL, a zero length DN is sent to the server.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode delete(struct ldap_connection_ctx_t* connection, const char *dn)
{
    int rc = ldap_delete_ext(connection->ldap,
                             dn,
                             NULL,
                             NULL,
                             &connection->current_msgid);
    if (rc != LDAP_SUCCESS)
    {
        error("Unable to create modify request: %s\n", ldap_err2string(rc));
        return RETURN_CODE_FAILURE;
    }
    connection->on_read_operation = delete_on_read;

    return RETURN_CODE_SUCCESS;
}

/**
 * @brief delete_on_read This callback determines result of delete operation.
 * @param[in] rc         Return code of ldap_result.
 * @param[in] message    Message received from ldap.
 * @param[in] connection Connection to work with.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode delete_on_read(int rc, LDAPMessage *message, struct ldap_connection_ctx_t *connection)
{
    int error_code = LDAP_SUCCESS;
    char *diagnostic_message = NULL;

    switch (rc)
    {
    case LDAP_RES_DELETE:
    {
        char *dn = NULL;

        connection->on_read_operation = NULL;

        ldap_parse_result(connection->ldap, message, &error_code, &dn, &diagnostic_message, NULL, NULL, false);
        info("ldap_result: %s %s %d\n", diagnostic_message, ldap_err2string(error_code), error_code);
        ldap_memfree(diagnostic_message);
        ldap_memfree(dn);

        switch (error_code)
        {
        case LDAP_SUCCESS:
            return RETURN_CODE_SUCCESS;

        default:
            return RETURN_CODE_FAILURE;
        }
    }
        break;
    default:
    {
        ldap_get_option(connection->ldap, LDAP_OPT_RESULT_CODE, (void*)&error_code);
        ldap_get_option(connection->ldap, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&diagnostic_message);
        error("ldap_result failed: %s\n", diagnostic_message);
        ldap_memfree(diagnostic_message);
    }
        break;
    }

    return RETURN_CODE_FAILURE;
}

/**
 * @brief whoami This operation only supported on OpenLDAP and determines who current user is.
 * @param[in] connection Connection to work with.
 */
enum OperationReturnCode whoami(struct ldap_connection_ctx_t *connection)
{
    int rc = ldap_whoami(connection->ldap, NULL, NULL, &connection->current_msgid);

    if (rc != LDAP_SUCCESS)
    {
        error("Unable to create whoami request: %s\n", ldap_err2string(rc));
        return RETURN_CODE_FAILURE;
    }
    connection->on_read_operation = whoami_on_read;

    return RETURN_CODE_SUCCESS;
}

/**
 * @brief whoami_on_read This callback determines result of whoami operation.
 * @param[in] rc         Return code of ldap_result.
 * @param[in] message    Message received from ldap.
 * @param[in] connection Connection to work with.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode whoami_on_read(int rc, LDAPMessage *message, struct ldap_connection_ctx_t *connection)
{
    int error_code = 0;
    char *diagnostic_message = NULL;

    switch (rc)
    {
    case LDAP_RES_EXTENDED:
    {
        struct berval* authrizid = NULL;
        ldap_parse_whoami(connection->ldap, message, &authrizid);
    }
        break;
    default:
    {
        ldap_get_option(connection->ldap, LDAP_OPT_RESULT_CODE, (void*)&error_code);
        ldap_get_option(connection->ldap, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&diagnostic_message);
        error("ldap_result failed: %s\n", diagnostic_message);
        ldap_memfree(diagnostic_message);
    }
        break;
    }

    return RETURN_CODE_SUCCESS;
}
