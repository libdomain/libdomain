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
#include "domain.h"
#include "entry.h"

#include <talloc.h>

#include <ldap.h>
#include <ldap_schema.h>

char* LDAP_ATTRIBUTE_TYPES[] = { "attributetypes", NULL };
char* LDAP_OBJECT_CLASSES[] = { "objectclasses", NULL };

typedef enum OperationReturnCode (*op_fn)(char *attribute_value, void* user_data);

/**
 * @brief ldap_schema_read_entry   Reads attribute values of entry and writes to output parameter.
 * @param[in] entry                Entry to work with.
 * @param[in] callback             Callback for processing attribute values.
 * @param[in] user_data            An output parameter for returning data from callback.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
static enum OperationReturnCode
ldap_schema_read_entry(ld_entry_t* entry, op_fn callback, void* user_data)
{
    LDAPAttribute_t** attributes = ld_entry_get_attributes(entry); // TODO: Check memory leak

    if (attributes == NULL)
    {
        return RETURN_CODE_SUCCESS;
    }

    int index = 0;
    LDAPAttribute_t* current_attribute = attributes[index];
    while (current_attribute != NULL)
    {
        if (current_attribute->values == NULL)
        {
            continue;
        }

        int value_index = 0;
        char* current_value = current_attribute->values[value_index];
        while (current_value != NULL)
        {
            if (callback(current_value, user_data) == RETURN_CODE_FAILURE)
            {
                return RETURN_CODE_FAILURE;
            }

            current_value = current_attribute->values[++value_index];
        }
        current_attribute = attributes[++index];
    }

    return RETURN_CODE_SUCCESS;
}

/**
 * @brief ldap_schema_callback_common   This callback processes LDAP attributes from entries with a callback parameter.
 * @param[in] connection                Connection to work with.
 * @param[in] entries                   Entries to work with.
 * @param[in] callback                  Callback for processing attribute values.
 * @param[in] user_data                 An output parameter for returning data from callback.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
static enum OperationReturnCode
ldap_schema_callback_common(struct ldap_connection_ctx_t *connection, ld_entry_t** entries, op_fn callback, void* user_data)
{
    if (entries != NULL && entries[0] != NULL)
    {
        int index = 0;
        ld_entry_t* current_entry = entries[index];

        while (current_entry != NULL)
        {
            if (ldap_schema_read_entry(current_entry, callback, user_data) == RETURN_CODE_FAILURE)
            {
                return RETURN_CODE_FAILURE;
            }

            current_entry = entries[++index];
        }
    }

    return RETURN_CODE_SUCCESS;
}

/**
 * @brief attribute_type_destructor Destructor of the attribute type description.
 * @param[in] reference             Pointer to the pointer to attribute type description.
 * @return
 *        0 - on success.
 */
int attribute_type_destructor(LDAPAttributeType **reference)
{
    if (reference)
    {
        ldap_attributetype_free(*reference);
    }

    return 0;
}

/**
 * @brief attribute_type_callback   This callback appends LDAP attribute type to schema.
 * @param[in] attribute_value       Attribute value to work with.
 * @param[in] user_data             An output parameter for returning data (schema in this case) from callback.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
static enum OperationReturnCode attribute_type_callback(char *attribute_value, void* user_data)
{
    ldap_schema_t* schema = talloc_get_type_abort(user_data, struct ldap_schema_t);

    LDAPAttributeType **reference = talloc_zero(schema, LDAPAttributeType*);
    talloc_set_destructor(reference, attribute_type_destructor);

    int error_code = 0;
    const char* error_message = NULL;
    LDAPAttributeType* attribute_type = ldap_str2attributetype(attribute_value, &error_code, &error_message, LDAP_SCHEMA_ALLOW_ALL);
    if (!attribute_type || error_code != 0 || error_message != 0)
    {
        talloc_free(reference);

        ld_error("Unable to parse attribute type %d %s\n", error_code, error_message);
        return RETURN_CODE_FAILURE;
    }
    else
    {
        reference = &attribute_type;

        if (!ldap_schema_append_attributetype(schema, attribute_type))
        {
            ld_error("Unable to add attribute type to the schema!\n");
            return RETURN_CODE_FAILURE;
        }
    }

    return RETURN_CODE_SUCCESS;
}

/**
 * @brief object_class_destructor Destructor of the object class description.
 * @param[in] reference           Pointer to pointer to object class description.
 * @return
 *        0 - on success.
 */
int object_class_destructor(LDAPObjectClass **reference)
{
    if (reference)
    {
        ldap_objectclass_free(*reference);
    }

    return 0;
}

/**
 * @brief object_class_callback     This callback appends LDAP object class to schema.
 * @param[in] attribute_value       Attribute value to work with.
 * @param[in] user_data             An output parameter for returning data (schema in this case) from callback.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
static enum OperationReturnCode object_class_callback(char *attribute_value, void* user_data)
{
    ldap_schema_t* schema = talloc_get_type_abort(user_data, struct ldap_schema_t);

    LDAPObjectClass **reference = talloc_zero(schema, LDAPObjectClass*);
    talloc_set_destructor(reference, object_class_destructor);

    int error_code = 0;
    const char* error_message = NULL;
    LDAPObjectClass* object_class = ldap_str2objectclass(attribute_value, &error_code, &error_message, LDAP_SCHEMA_ALLOW_ALL);

    if (!object_class || error_code != 0 || error_message != 0)
    {
        talloc_free(reference);

        ld_error("Unable to parse object class: %d %s\n", error_code, error_message);
        return RETURN_CODE_FAILURE;
    }
    else
    {
        reference = &object_class;

        if (!ldap_schema_append_objectclass(schema, object_class))
        {
            ld_error("Unable to add class to the schema!\n");
            return RETURN_CODE_FAILURE;
        }
    }

    return RETURN_CODE_SUCCESS;
}

/**
 * @brief ldap_schema_attribute_types_search_callback   This callback wraps ldap_schema_callback_common for attribute type appending to schema.
 * @param[in] connection                                Connection to work with.
 * @param[in] entries                                   Entries to work with.
 * @param[in] user_data                                 An output parameter for returning data (schema in this case) from callback.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
static enum OperationReturnCode
ldap_schema_attribute_types_search_callback(struct ldap_connection_ctx_t *connection, ld_entry_t** entries, void* user_data)
{
    return ldap_schema_callback_common(connection, entries, &attribute_type_callback, user_data);
}

/**
 * @brief ldap_schema_object_classes_search_callback    This callback wraps ldap_schema_callback_common for object class appending to schema.
 * @param[in] connection                                Connection to work with.
 * @param[in] entries                                   Entries to work with.
 * @param[in] user_data                                 An output parameter for returning data (schema in this case) from callback.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
static enum OperationReturnCode
ldap_schema_object_classes_search_callback(struct ldap_connection_ctx_t *connection, ld_entry_t** entries, void* user_data)
{
    return ldap_schema_callback_common(connection, entries, &object_class_callback, user_data);
}

/**
 * @brief schema_load_openldap  Loads the schema of OpenLDAP directory type from the connection.
 * @param[in] connection        Connection to work with.
 * @param[in] schema            Schema for loading data from connection.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode
schema_load_openldap(struct ldap_connection_ctx_t* connection, struct ldap_schema_t* schema)
{
    int rc = RETURN_CODE_SUCCESS;
    const char* search_base = "cn=subschema";

    rc = search(connection,
                search_base,
                LDAP_SCOPE_BASE,
                "(objectclass=subschema)",
                LDAP_ATTRIBUTE_TYPES,
                false,
                &ldap_schema_attribute_types_search_callback,
                schema);

    if (rc != RETURN_CODE_SUCCESS)
    {
        ld_error("schema_load_openldap - unable to search attribute types.\n");

        return RETURN_CODE_FAILURE;
    }

    rc = search(connection,
                search_base,
                LDAP_SCOPE_BASE,
                "(objectclass=subschema)",
                LDAP_OBJECT_CLASSES,
                false,
                &ldap_schema_object_classes_search_callback,
                schema);

    if (rc != RETURN_CODE_SUCCESS)
    {
        ld_error("schema_load_openldap - unable to search object classes.\n");

        return RETURN_CODE_FAILURE;
    }

    return RETURN_CODE_SUCCESS;
}
