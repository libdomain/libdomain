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

#include "directory.h"
#include "entry.h"

char* LDAP_DIRECTORY_ATTRS[] = { "*", NULL };

/**
 * @brief directory_get_type Request LDAP type from service.
 * @param[in] connection connection to use
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode directory_get_type(struct ldap_connection_ctx_t *connection)
{
    int rc = ldap_search_ext(connection->ldap,
                    "",
                    LDAP_SCOPE_BASE,
                    "(objectClass=*)",
                    LDAP_DIRECTORY_ATTRS,
                    0,
                    NULL,
                    NULL,
                    NULL,
                    LDAP_NO_LIMIT,
                    &connection->current_msgid);
    if (rc != LDAP_SUCCESS)
    {
        error("Unable to create directory type request: %s\n", ldap_err2string(rc));
        return RETURN_CODE_FAILURE;
    }

    struct ldap_request_t* request = &connection->read_requests[connection->n_read_requests];
    request->msgid = connection->current_msgid;
    request->on_read_operation = directory_parse_result;
    ++connection->n_read_requests;
    request_queue_push(connection->callqueue, &request->node);

    return RETURN_CODE_SUCCESS;


    return RETURN_CODE_FAILURE;
}

/**
 * @brief directory_process_attribute
 * @param[in] attribute_name
 * @param[in] connection
 * @return
 *        - true if directory type is recognized.
 *        - false if we have not detected directory type yet.
 */
bool directory_process_attribute(const char* attribute_name, struct ldap_connection_ctx_t *connection)
{
    if (strcmp(attribute_name, "isGlobalCatalogReady") == 0)
    {
        connection->directory_type = LDAP_TYPE_ACTIVE_DIRECTORY;

        info("Directory type is Active Directory\n");

        return true;
    }

    if (strcmp(attribute_name, "objectClass") == 0)
    {
        connection->directory_type = LDAP_TYPE_OPENLDAP;

        info("Directory type is OpenLDAP\n");

        return true;
    }

    return false;
}

/**
 * @brief directory_parse_result Parses results returned by directory_get_type.
 * @param[in] rc                 Return code of ldap_result.
 * @param[in] message            Message received from ldap.
 * @param[in] connection         Connection to work with.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode directory_parse_result(int rc, LDAPMessage *message, struct ldap_connection_ctx_t *connection)
{
    const char *attribute   = NULL;
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
            attribute = ldap_first_attribute(connection->ldap, message, &ber_element);
            while (attribute != NULL)
            {
                if (directory_process_attribute(attribute, connection))
                {
                    break;
                }

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

    if (connection->on_error_operation)
    {
        connection->on_error_operation(rc, message, connection);
    }

    return RETURN_CODE_FAILURE;
}
