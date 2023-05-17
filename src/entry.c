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
 * @brief add_on_read
 * @param fd
 * @param flags
 * @param arg
 */
void add_on_read(int fd, short flags, void *arg)
{
    (void)(fd);
    (void)(flags);
    (void)(arg);
}

/**
 * @brief add_on_write
 * @param fd
 * @param flags
 * @param arg
 */
void add_on_write(int fd, short flags, void *arg)
{
    (void)(fd);
    (void)(flags);
    (void)(arg);
}

/**
 * @brief search
 * @param connection
 * @param base_dn
 * @param scope
 * @param filter
 * @param attrs
 * @param attrsonly
 */
void search(struct ldap_connection_ctx_t* connection, const char *base_dn, int scope, const char *filter,
            char **attrs, bool attrsonly)
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
        error("Unable to create search request: \n");
    }
    connection->on_read_operation = search_on_read;
}

/**
 * @brief search_on_read
 * @param rc
 * @param message
 * @param connection
 * @return
 */
enum OperationReturnCode search_on_read(int rc, LDAPMessage *message, struct ldap_connection_ctx_t *connection)
{
    (void)(rc);
    (void)(message);
    (void)(connection);

    const char *attribute;
    struct berval **values;
    BerElement *ber_element;

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
 * @brief search_on_write
 * @param fd
 * @param flags
 * @param arg
 */
void search_on_write(int fd, short flags, void *arg)
{
    (void)(fd);
    (void)(flags);
    (void)(arg);
}

/**
 * @brief modify_on_read
 * @param fd
 * @param flags
 * @param arg
 */
void modify_on_read(int fd, short flags, void *arg)
{
    (void)(fd);
    (void)(flags);
    (void)(arg);
}

/**
 * @brief modify_on_write
 * @param fd
 * @param flags
 * @param arg
 */
void modify_on_write(int fd, short flags, void *arg)
{
    (void)(fd);
    (void)(flags);
    (void)(arg);
}

/**
 * @brief delete_on_read
 * @param fd
 * @param flags
 * @param arg
 */
void delete_on_read(int fd, short flags, void *arg)
{
    (void)(fd);
    (void)(flags);
    (void)(arg);
}

/**
 * @brief delete_on_write
 * @param fd
 * @param flags
 * @param arg
 */
void delete_on_write(int fd, short flags, void *arg)
{
    (void)(fd);
    (void)(flags);
    (void)(arg);
}

/**
 * @brief whoami
 * @param connection
 */
void whoami(struct ldap_connection_ctx_t *connection)
{
    int rc = ldap_whoami(connection->ldap, NULL, NULL, &connection->current_msgid);

    if (rc != LDAP_SUCCESS)
    {
        error("Unable to create whoami request: \n");
    }
    connection->on_read_operation = whoami_on_read;
}

/**
 * @brief whoami_on_read
 * @param rc
 * @param message
 * @param connection
 * @return
 */
enum OperationReturnCode whoami_on_read(int rc, LDAPMessage *message, struct ldap_connection_ctx_t *connection)
{
    (void)(rc);
    (void)(message);
    (void)(connection);

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
