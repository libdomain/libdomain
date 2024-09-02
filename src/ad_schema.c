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

#include "schema.h"
#include "schema_p.h"

#include "common.h"

#include "directory.h"
#include "entry.h"

#include <talloc.h>

#include <ldap.h>
#include <ldap_schema.h>


/**
 * @brief active_directory_attribute_type_callback     This callback appends Active Directory attribute type to schema.
 * @param[in] attribute_value                          Attribute value to work with.
 * @param[in] user_data                                An output parameter for returning data (schema in this case) from callback.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
static enum OperationReturnCode active_directory_attribute_type_callback(char *attribute_value, void* user_data)
{
    // TODO: implement
    return RETURN_CODE_FAILURE;
}

/**
 * @brief active_directory_object_class_callback     This callback appends Active Directory object class to schema.
 * @param[in] attribute_value                        Attribute value to work with.
 * @param[in] user_data                              An output parameter for returning data (schema in this case) from callback.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
static enum OperationReturnCode active_directory_object_class_callback(char *attribute_value, void* user_data)
{
    // TODO: implement
    return RETURN_CODE_FAILURE;
}

/**
 * @brief active_directory_schema_attribute_types_search_callback     This callback wraps ldap_schema_callback_common for attribute type appending to schema.
 * @param[in] connection                                              Connection to work with.
 * @param[in] entries                                                 Entries to work with.
 * @param[in] user_data                                               An output parameter for returning data (schema in this case) from callback.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
static enum OperationReturnCode
active_directory_schema_attribute_types_search_callback(struct ldap_connection_ctx_t *connection, ld_entry_t** entries, void* user_data)
{
    return ldap_schema_callback_common(connection, entries, &active_directory_attribute_type_callback, user_data);
}

/**
 * @brief active_directory_schema_object_classes_search_callback     This callback wraps ldap_schema_callback_common for object class appending to schema.
 * @param[in] connection                                             Connection to work with.
 * @param[in] entries                                                Entries to work with.
 * @param[in] user_data                                              An output parameter for returning data (schema in this case) from callback.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
static enum OperationReturnCode
active_directory_schema_object_classes_search_callback(struct ldap_connection_ctx_t *connection, ld_entry_t** entries, void* user_data)
{
    return ldap_schema_callback_common(connection, entries, &active_directory_object_class_callback, user_data);
}

/**
 * @brief schema_load_active_directory  Loads the schema of Active Directory directory type from the connection.
 * @param[in] connection                Connection to work with.
 * @param[in] schema                    Schema for loading data from connection.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode schema_load_active_directory(struct ldap_connection_ctx_t *connection,
                                                      struct ldap_schema_t *schema,
                                                      const char* basedn)
{
    TALLOC_CTX* talloc_ctx = talloc_new(NULL);
    int rc = RETURN_CODE_SUCCESS;

    if (!talloc_ctx)
    {
        ld_error("schema_load_active_directory - unable to allocate memory.\n");

        return RETURN_CODE_FAILURE;
    }

    const char* schema_dn = talloc_asprintf(talloc_ctx, "cn=schema,cn=configuration,%s", basedn);

    rc = search(connection,
           schema_dn,
           LDAP_SCOPE_SUB,
           "(objectClass=attributeSchema)",
           NULL,
           false,
           &active_directory_schema_attribute_types_search_callback,
           schema);

    if (rc != RETURN_CODE_SUCCESS)
    {
        ld_error("schema_load_active_directory - unable to search attributes.\n");

        return RETURN_CODE_FAILURE;
    }

    rc = search(connection,
           schema_dn,
           LDAP_SCOPE_SUB,
           "(objectClass=classSchema)",
           NULL,
           false,
           &active_directory_schema_object_classes_search_callback,
           schema);

    if (rc != RETURN_CODE_SUCCESS)
    {
        ld_error("schema_load_active_directory - unable to search object classes.\n");

        return RETURN_CODE_FAILURE;
    }

    talloc_free(talloc_ctx);

    return RETURN_CODE_SUCCESS;
}
