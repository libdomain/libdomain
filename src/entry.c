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
#include "entry_p.h"
#include "connection.h"
#include "domain.h"
#include "domain_p.h"

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
    int msgid = 0;
    int rc = ldap_add_ext(connection->ldap, dn, attrs, NULL, NULL, &msgid);
    if (rc != LDAP_SUCCESS)
    {
        ld_error("Unable to add entry: %s\n", ldap_err2string(rc));
        return RETURN_CODE_FAILURE;
    }

    struct ldap_request_t* request = &connection->read_requests[connection->n_read_requests];
    request->msgid = msgid;
    request->on_read_operation = add_on_read;
    ++connection->n_read_requests;
    request_queue_push(connection->callqueue, &request->node);

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

        ldap_parse_result(connection->ldap, message, &error_code, &dn, &diagnostic_message, NULL, NULL, false);
        ld_info("ldap_result: %s %s %d\n", diagnostic_message, ldap_err2string(error_code), error_code);
        ldap_memfree(diagnostic_message);
        ldap_memfree(dn);

        switch (error_code)
        {
        case LDAP_SUCCESS:
        case LDAP_ALREADY_EXISTS:
            return RETURN_CODE_SUCCESS;

        default:
            if (connection->on_error_operation)
            {
                connection->on_error_operation(rc, message, connection);
            }
            return RETURN_CODE_FAILURE;
        }
    }
        break;
    default:
    {
        ldap_get_option(connection->ldap, LDAP_OPT_RESULT_CODE, (void*)&error_code);
        ldap_get_option(connection->ldap, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&diagnostic_message);
        ld_error("ldap_result failed: %s\n", diagnostic_message);
        ldap_memfree(diagnostic_message);
    }
        break;
    }

    if (connection->on_error_operation)
    {
        connection->on_error_operation(rc, message, connection);
    }

    return RETURN_CODE_FAILURE;
}

/**
 * @brief print_search_callback This callback prints values of entries after a search.
 * @param[in] connection        Connection to work with.
 * @param[in] entries           Entries to work with.
 * @param[in] user_data         An output parameter for returning data from callback.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
static enum OperationReturnCode print_search_callback(struct ldap_connection_ctx_t *connection, ld_entry_t** entries, void* user_data)
{
    int entry_index = 0;
    while (entries[entry_index] != NULL)
    {
        ld_entry_t* entry = entries[entry_index];

        fprintf(stderr, "Search result - entry dn: %s\n", ld_entry_get_dn(entry));

        int attribute_index = 0;
        LDAPAttribute_t** attributes = ld_entry_get_attributes(entry);
        while(attributes[attribute_index] != NULL)
        {
            LDAPAttribute_t* attribute = attributes[attribute_index];

            int value_index = 0;
            char** values = attribute->values;
            while (values[value_index] != NULL)
            {
                fprintf(stderr, "%s: %s\n", attribute->name, values[value_index]);
                value_index++;
            }

            attribute_index++;
        }

        talloc_free(entry);

        entry_index++;
    }

    return RETURN_CODE_SUCCESS;
}

/**
 * @brief search                Function wraps ldap search operation associating it with connection.
 * @param[in] connection        Connection to work with.
 * @param[in] base_dn           The dn of the entry at which to start the search.
 *                              If NULL, a zero length DN is sent to the server.
 * @param[in] scope             One of LDAP_SCOPE_BASE (0x00), LDAP_SCOPE_ONELEVEL (0x01),
 *                              or LDAP_SCOPE_SUBTREE (0x02), indicating the scope of the search.
 * @param[in] filter            A character string as described in [13], representing the
 *                              search filter.  The value NULL can be passed to indicate
 *                              that the filter "(objectclass=*)" which matches all entries
 *                              is to be used.  Note that if the caller of the API is using
 *                              LDAPv2, only a subset of the filter functionality described
 *                              in [13] can be successfully used.
 * @param[in] attrs             A NULL-terminated array of strings indicating which attributes
 *                              to return for each matching entry. Passing NULL for
 *                              this parameter causes all available user attributes to be
 *                              retrieved.  The special constant string LDAP_NO_ATTRS
 *                              ("1.1") MAY be used as the only string in the array to
 *                              indicate that no attribute types are to be returned by the
 *                              server.  The special constant string LDAP_ALL_USER_ATTRS
 *                              ("*") can be used in the attrs array along with the names
 *                              of some operational attributes to indicate that all user
 *                              attributes plus the listed operational attributes are to be
 *                              returned.
 * @param[in] attrsonly         A boolean value that MUST be zero if both attribute types
 *                              and values are to be returned, and non-zero if only types
 *                              are wanted.
 * @param[in] search_callback   A callback function on search operation.
 * @param[in] user_data         An output parameter for returning data after a search.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode search(struct ldap_connection_ctx_t *connection,
                                const char *base_dn,
                                int scope,
                                const char *filter,
                                char **attrs,
                                bool attrsonly,
                                search_callback_fn search_callback,
                                void* user_data)
{
    int msgid = 0;
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
                    &msgid);
    if (rc != LDAP_SUCCESS)
    {
        ld_error("Unable to create search request: %s\n", ldap_err2string(rc));
        return RETURN_CODE_FAILURE;
    }

    struct ldap_request_t* request = &connection->read_requests[connection->n_read_requests];
    request->msgid = msgid;
    request->on_read_operation = search_on_read;
    ++connection->n_read_requests;
    request_queue_push(connection->callqueue, &request->node);

    if (connection->n_search_requests + 1 >= MAX_REQUESTS)
    {
        ld_error("Maximum amount of search requests exceeded for connection %d.\n", connection);

        return RETURN_CODE_FAILURE;
    }

    struct ldap_search_request_t* search_request = &connection->search_requests[connection->n_search_requests];
    search_request->msgid = msgid;
    search_request->on_search_operation = search_callback ? search_callback : print_search_callback;
    search_request->user_data = user_data;
    ++connection->n_search_requests;

    return RETURN_CODE_SUCCESS;
}

/**
 * @brief remove_search_request Removes search request from connection by index.
 * @param[in] connection        Connection to remove request from.
 * @param[in] index             Index to remove.
 */
void connection_remove_search_request(struct ldap_connection_ctx_t *connection, int index)
{
    if (index == connection->n_read_requests - 1)
    {
        --connection->n_search_requests;
        memset(&connection->search_requests[index], 0, sizeof(struct ldap_search_request_t));
    }
    else
    {
        int request_index = index;
        while (request_index < connection->n_search_requests - 1)
        {
            connection->search_requests[request_index] = connection->search_requests[request_index + 1];
            ++request_index;
        }
        memset(&connection->search_requests[--connection->n_search_requests], 0, sizeof(struct ldap_search_request_t));
    }
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
    char *attribute   = NULL;
    struct berval **values  = NULL;
    BerElement *ber_element = NULL;
    int values_count = 0;

    int error_code = 0;
    char *diagnostic_message = NULL;

    switch (rc)
    {
    case LDAP_RES_SEARCH_ENTRY:
    case LDAP_RES_SEARCH_RESULT:
    {
        for (int i = 0; i < connection->n_search_requests; ++i)
        {
            if (connection->search_requests[i].msgid == ldap_msgid(message))
            {
                if (!connection->search_requests[i].on_search_operation)
                {
                    return RETURN_CODE_FAILURE;
                }

                const int INITIAL_ARRAY_SIZE = 256;

                ld_info("Handle %d\n", connection->handle);

                ld_entry_t** entries = talloc_array(connection->handle->talloc_ctx, ld_entry_t*, INITIAL_ARRAY_SIZE);

                if (!entries)
                {
                    ld_error("search_on_read - out of memory during allocation of entries!\n");

                    return RETURN_CODE_FAILURE;
                }

                int entry_index = 0;

                while (message)
                {
                    int entries_size = talloc_array_length(entries);

                    if (entry_index + 2 >= entries_size)
                    {
                        entries = talloc_realloc(connection->handle->talloc_ctx, entries, ld_entry_t*, entries_size * 2);

                        if (!entries)
                        {
                            ld_error("search_on_read - out of memory during allocation of entries!\n");

                            return RETURN_CODE_FAILURE;
                        }
                    }

                    char* dn = ldap_get_dn(connection->ldap, message);
                    ld_entry_t* ld_entry = ld_entry_new(connection->handle->talloc_ctx, dn);
                    ldap_memfree(dn);

                    if (!ld_entry)
                    {
                        ld_error("search_on_read - out of memory - unable to create new entry!\n");

                        return RETURN_CODE_FAILURE;
                    }

                    entries[entry_index] = ld_entry;

                    attribute = ldap_first_attribute(connection->ldap, message, &ber_element);
                    while (attribute != NULL)
                    {
                        LDAPAttribute_t* ld_attribute = talloc_zero(connection->handle->talloc_ctx, LDAPAttribute_t);
                        ld_attribute->name = talloc_strdup(connection->handle->talloc_ctx, attribute);

                        values = ldap_get_values_len(connection->ldap, message, attribute);
                        values_count = ldap_count_values_len(values);

                        ld_attribute->values = talloc_array(connection->handle->talloc_ctx, char*, values_count + 1);

                        for(int values_index = 0; values_index < values_count; values_index++)
                        {
                            ld_attribute->values[values_index] = talloc_strdup(connection->handle->talloc_ctx, values[values_index]->bv_val);
                        }
                        ld_attribute->values[values_count] = NULL;
                        ldap_value_free_len(values);

                        ld_entry_add_attribute(ld_entry, ld_attribute);

                        ldap_memfree(attribute);
                        attribute = ldap_next_attribute(connection->ldap, message, ber_element);
                    };
                    ber_free(ber_element, 0);

                    message = ldap_next_message(connection->ldap, message);

                    entry_index++;
                }

                entries[entry_index] = NULL;

                int rc = connection->search_requests[i].on_search_operation(connection, entries, connection->search_requests[i].user_data);

                connection_remove_search_request(connection, i);

                return rc;
            }
        }
    }
        break;
    case LDAP_RES_SEARCH_REFERENCE:
        ld_info("Received search referral but not following it!");
        return RETURN_CODE_SUCCESS;
    default:
    {
        ldap_get_option(connection->ldap, LDAP_OPT_RESULT_CODE, (void*)&error_code);
        ldap_get_option(connection->ldap, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&diagnostic_message);
        ld_error("ldap_result failed: %s\n", diagnostic_message);
        ldap_memfree(diagnostic_message);
    }
        break;
    }

    if (connection->on_error_operation)
    {
        connection->on_error_operation(rc, message, connection);
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
    int msgid = 0;
    int rc = ldap_modify_ext(connection->ldap,
                             dn,
                             attrs,
                             NULL,
                             NULL,
                             &msgid);
    if (rc != LDAP_SUCCESS)
    {
        ld_error("Unable to create modify request: %s\n", ldap_err2string(rc));
        return RETURN_CODE_FAILURE;
    }

    struct ldap_request_t* request = &connection->read_requests[connection->n_read_requests];
    request->msgid = msgid;
    request->on_read_operation = modify_on_read;
    ++connection->n_read_requests;
    request_queue_push(connection->callqueue, &request->node);

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

        ldap_parse_result(connection->ldap, message, &error_code, &dn, &diagnostic_message, NULL, NULL, false);
        ld_info("ldap_result: %s %s %d\n", diagnostic_message, ldap_err2string(error_code), error_code);
        ldap_memfree(diagnostic_message);
        ldap_memfree(dn);

        switch (error_code)
        {
        case LDAP_SUCCESS:
            return RETURN_CODE_SUCCESS;

        default:
            if (connection->on_error_operation)
            {
                connection->on_error_operation(rc, message, connection);
            }

            return RETURN_CODE_FAILURE;
        }
    }
        break;
    default:
    {
        ldap_get_option(connection->ldap, LDAP_OPT_RESULT_CODE, (void*)&error_code);
        ldap_get_option(connection->ldap, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&diagnostic_message);
        ld_error("ldap_result failed: %s\n", diagnostic_message);
        ldap_memfree(diagnostic_message);
    }
        break;
    }

    if (connection->on_error_operation)
    {
        connection->on_error_operation(rc, message, connection);
    }

    return RETURN_CODE_FAILURE;
}

/**
 * @brief ld_delete Function wraps ldap_delete_ext.
 * @param[in] connection Connection to work with.
 * @param[in] dn         The name of the entry to delete.  If NULL, a zero length DN is sent to the server.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_delete(struct ldap_connection_ctx_t* connection, const char *dn)
{
    int msgid = 0;
    int rc = ldap_delete_ext(connection->ldap,
                             dn,
                             NULL,
                             NULL,
                             &msgid);
    if (rc != LDAP_SUCCESS)
    {
        ld_error("Unable to create modify request: %s\n", ldap_err2string(rc));
        return RETURN_CODE_FAILURE;
    }

    struct ldap_request_t* request = &connection->read_requests[connection->n_read_requests];
    request->msgid = msgid;
    request->on_read_operation = delete_on_read;
    ++connection->n_read_requests;
    request_queue_push(connection->callqueue, &request->node);

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

        ldap_parse_result(connection->ldap, message, &error_code, &dn, &diagnostic_message, NULL, NULL, false);
        ld_info("ldap_result: %s %s %d\n", diagnostic_message, ldap_err2string(error_code), error_code);
        ldap_memfree(diagnostic_message);
        ldap_memfree(dn);

        switch (error_code)
        {
        case LDAP_SUCCESS:
            return RETURN_CODE_SUCCESS;

        default:
            if (connection->on_error_operation)
            {
                connection->on_error_operation(rc, message, connection);
            }
            return RETURN_CODE_FAILURE;
        }
    }
        break;
    default:
    {
        ldap_get_option(connection->ldap, LDAP_OPT_RESULT_CODE, (void*)&error_code);
        ldap_get_option(connection->ldap, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&diagnostic_message);
        ld_error("ldap_result failed: %s\n", diagnostic_message);
        ldap_memfree(diagnostic_message);
    }
        break;
    }

    if (connection->on_error_operation)
    {
        connection->on_error_operation(rc, message, connection);
    }

    return RETURN_CODE_FAILURE;
}

/**
 * @brief whoami This operation only supported on OpenLDAP and determines who current user is.
 * @param[in] connection Connection to work with.
 */
enum OperationReturnCode whoami(struct ldap_connection_ctx_t *connection)
{
    int msgid = 0;
    int rc = ldap_whoami(connection->ldap, NULL, NULL, &msgid);

    if (rc != LDAP_SUCCESS)
    {
        ld_error("Unable to create whoami request: %s\n", ldap_err2string(rc));
        return RETURN_CODE_FAILURE;
    }

    struct ldap_request_t* request = &connection->read_requests[connection->n_read_requests];
    request->msgid = msgid;
    request->on_read_operation = whoami_on_read;
    ++connection->n_read_requests;
    request_queue_push(connection->callqueue, &request->node);

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
        ld_error("ldap_result failed: %s\n", diagnostic_message);
        ldap_memfree(diagnostic_message);
    }
        break;
    }

    return RETURN_CODE_SUCCESS;
}

/**
 * @brief ld_rename Wraps ldap_rename function.
 * @param connection[in]      Connection to work with.
 * @param olddn[in]           Old dn of the entry.
 * @param newdn[in]           New dn of the entry.
 * @param new_parent[in]      New parent of the entry.
 * @param delete_original[in] If we going to delete original entry or not
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_rename(struct ldap_connection_ctx_t *connection, const char *olddn,
                                   const char *newdn, const char* new_parent, bool delete_original)
{
    int msgid = 0;
    int rc = ldap_rename(connection->ldap,
                         olddn,
                         newdn,
                         new_parent,
                         delete_original,
                         NULL,
                         NULL,
                         &msgid);

    if (rc != LDAP_SUCCESS)
    {
        ld_error("Unable to create whoami request: %s\n", ldap_err2string(rc));
        return RETURN_CODE_FAILURE;
    }

    struct ldap_request_t* request = &connection->read_requests[connection->n_read_requests];
    request->msgid = msgid;
    request->on_read_operation = rename_on_read;
    ++connection->n_read_requests;
    request_queue_push(connection->callqueue, &request->node);

    return RETURN_CODE_SUCCESS;
}

/**
 * @brief rename_on_read This callback determines result of rename operation.
 * @param[in] rc         Return code of ldap_result.
 * @param[in] message    Message received from ldap.
 * @param[in] connection Connection to work with.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode rename_on_read(int rc, LDAPMessage *message, ldap_connection_ctx_t *connection)
{
    int error_code = LDAP_SUCCESS;
    char *diagnostic_message = NULL;

    switch (rc)
    {
    case LDAP_RES_RENAME:
    {
        char *dn = NULL;

        ldap_parse_result(connection->ldap, message, &error_code, &dn, &diagnostic_message, NULL, NULL, false);
        ld_info("ldap_result: %s %s %d\n", diagnostic_message, ldap_err2string(error_code), error_code);
        ldap_memfree(diagnostic_message);
        ldap_memfree(dn);

        switch (error_code)
        {
        case LDAP_SUCCESS:
            return RETURN_CODE_SUCCESS;

        default:
            if (connection->on_error_operation)
            {
                connection->on_error_operation(rc, message, connection);
            }
            return RETURN_CODE_FAILURE;
        }
    }
        break;
    default:
    {
        ldap_get_option(connection->ldap, LDAP_OPT_RESULT_CODE, (void*)&error_code);
        ldap_get_option(connection->ldap, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&diagnostic_message);
        ld_error("ldap_result failed: %s\n", diagnostic_message);
        ldap_memfree(diagnostic_message);
    }
        break;
    }

    if (connection->on_error_operation)
    {
        connection->on_error_operation(rc, message, connection);
    }

    return RETURN_CODE_FAILURE;
}

static int ld_entry_destructor(TALLOC_CTX *ctx)
{
    ld_entry_t *entry = NULL;
    entry = talloc_get_type_abort(ctx, ld_entry_t);

    g_hash_table_destroy(entry->attributes);

    return 0;
}

/**
 * @brief ld_entry_new Creates new ld_entry_t;
 * @param[in] ctx      Talloc ctx to use.
 * @return
 *        - Valid pointer to ld_entry_t.
 *        - NULL on error.
 */
ld_entry_t* ld_entry_new(TALLOC_CTX *ctx, const char* dn)
{
    if (!ctx)
    {
        ld_error("ld_entry_new - invalid talloc_ctx!\n");

        return NULL;
    }

    if (!dn)
    {
        ld_error("ld_entry_new - invalid dn!\n");

        return NULL;
    }

    ld_entry_t* result = talloc_zero(ctx, ld_entry_t);

    if (!result)
    {
        ld_error("ld_entry_new - out of memory - unable to create entry!\n");

        return NULL;
    }

    result->dn = talloc_strdup(result, dn);

    if (!result->dn)
    {
        talloc_free(result);

        ld_error("ld_entry_new - out of memory - unable to copy dn!\n");

        return NULL;
    }

    result->attributes = g_hash_table_new(g_str_hash, g_str_equal);

    if (!result->attributes)
    {
        talloc_free(result);

        ld_error("ld_entry_new - out of memory - unable to create attributes!\n");

        return NULL;
    }

    talloc_set_destructor((void*)result, ld_entry_destructor);

    return result;
}

/**
 * @brief ld_entry_add_attribute Adds attribute to entry.
 * @param[in] entry              Entry to use.
 * @param[in] attr               Attribute to add.
 * @return
 *        - RETURN_CODE_FAILURE - on error.
 *        - RETURN_CODE_SUCCESS - when attribute was added successduly.
 */
enum OperationReturnCode ld_entry_add_attribute(ld_entry_t* entry, const LDAPAttribute_t *attr)
{
    if (!entry || !entry->attributes)
    {
        ld_error("ld_entry_add_attribute - entry is NULL!\n");

        return RETURN_CODE_FAILURE;
    }

    if (!attr)
    {
        ld_error("ld_entry_add_attribute - attribute is NULL!\n");

        return RETURN_CODE_FAILURE;
    }

    if (!attr->name)
    {
        ld_error("ld_entry_add_attribute - invalid attribute name!\n");

        return RETURN_CODE_FAILURE;
    }

    return g_hash_table_insert(entry->attributes, attr->name, (LDAPAttribute_t *)attr)
            ? RETURN_CODE_SUCCESS
            : RETURN_CODE_FAILURE;
}

/**
 * @brief ld_entry_get_attribute Gets attribute from entry.
 * @param[in] entry              Entry to use.
 * @param[in] name_or_oid        Name of attribute.
 * @return
 *        - NULL - if attribute not found.
 *        - Pointer to LDAPAttribute_t if attribute was found.
 */
LDAPAttribute_t *ld_entry_get_attribute(ld_entry_t* entry, const char *name_or_oid)
{
    if (!entry || !entry->attributes)
    {
        ld_error("ld_entry_get_attribute - entry is NULL!\n");

        return NULL;
    }

    return (LDAPAttribute_t *)g_hash_table_lookup(entry->attributes, name_or_oid);
}

/**
 * @brief ld_entry_get_dn Get entry's dn;
 * @param[in] entry       Entry to use.
 * @return
 *        - NULL - on error.
 *        - DN on success.
 */
const char *ld_entry_get_dn(ld_entry_t *entry)
{
    if (!entry || !entry->dn)
    {
        ld_error("ld_entry_add_attribute - entry is NULL!\n");

        return NULL;
    }

    return talloc_strdup(entry, entry->dn);
}

static void fill_attribute(gpointer key, gpointer value, gpointer userdata)
{
    if (!value)
    {
        return;
    }

    LDAPAttribute_t *attribute = userdata;

    attribute->name = key;

    char** values = ((LDAPAttribute_t*)value)->values;

    if (!values)
    {
        attribute->values = NULL;
        return;
    }

    int index = 0;
    while(values[index] != NULL)
    {
        index++;
    }

    attribute->values = talloc_array(userdata, char*, index + 1);

    int value_index = 0;
    while(values[value_index] != NULL)
    {
        attribute->values[value_index] = talloc_strdup(userdata, values[value_index]);
        value_index++;
    }

    attribute->values[index] = NULL;
}

/**
 * @brief ld_entry_get_attributes Get all attributes.
 * @param[in] entry               Entry to get attributes from.
 * @return
 *        - NULL terminated array of attributes on success.
 *        - NULL on error.
 * @see talloc_free();
 * It is required to call talloc_free() upon completing work with
 * attributes.
 */
LDAPAttribute_t **ld_entry_get_attributes(ld_entry_t *entry)
{
    if (!entry || !entry->attributes)
    {
        ld_error("ld_entry_add_attribute - entry is NULL!\n");

        return NULL;
    }

    int attributes_size = g_hash_table_size(entry->attributes);

    LDAPAttribute_t ** result = talloc_array(entry, LDAPAttribute_t*, attributes_size + 1);

    GHashTableIter iter;
    gpointer key = NULL, value = NULL;

    int index = 0;
    g_hash_table_iter_init(&iter, entry->attributes);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        result[index] = talloc_zero(result, LDAPAttribute_t);
        fill_attribute(key, value, result[index]);
        index++;
    }
    result[attributes_size] = NULL;

    return result;
}
