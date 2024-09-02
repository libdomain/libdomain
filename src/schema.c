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

/*!
 * \brief ldap_schema_new Allocates ldap_schema_t and checks it for validity.
 * \param[in] ctx         TALLOC_CTX to use.
 * \return
 *        - NULL on error.
 *        - Pointer to ldap schema on success.
 */
ldap_schema_t*
ldap_schema_new(TALLOC_CTX *ctx)
{
    if (!ctx)
    {
        ld_error("NULL talloc context.\n");

        return NULL;
    }

    ldap_schema_t* result = talloc_zero(ctx, struct ldap_schema_t);

    if (!result)
    {
        ld_error("Unable to allocate ldap_schema_t.\n");

        return NULL;
    }

    result->attribute_types = talloc_zero_array(ctx, LDAPAttributeType*, 1024);

    if (!result->attribute_types)
    {
        ld_error("Unable allocate attribute types in schema: %d ", result);

        return NULL;
    }

    result->attribute_types_capacity = 1024;
    result->attribute_types_size = 0;

    result->object_classes = talloc_zero_array(ctx, LDAPObjectClass*, 1024);

    if (!result->object_classes)
    {
        ld_error("Unable allocate object classes in schema: %d ", result);

        return NULL;
    }

    result->object_classes_capacity = 1024;
    result->object_classes_size = 0;

    return result;
}

/*!
 * \brief ldap_schema_object_classes Returns a list of LDAPObjectClass structs.
 * \param[in] schema                 Schema to work with.
 * \return
 *        - NULL if schema is NULL.
 *        - List of object classes from schema.
 */
LDAPObjectClass**
ldap_schema_object_classes(const ldap_schema_t *schema)
{
    if (!schema)
    {
        ld_error("Schema is NULL.\n");

        return NULL;
    }

    return schema->object_classes;
}

/*!
 * \brief ldap_schema_attribute_types Returns a list of LDAPAttributeType structs.
 * \param[in] schema                  Schema to work with.
 * \return
 *        - NULL if schema is NULL.
 *        - List of attribute types from schema.
 */
LDAPAttributeType**
ldap_schema_attribute_types(const ldap_schema_t* schema)
{
    if (!schema)
    {
        ld_error("Schema is NULL.\n");

        return NULL;
    }

    return schema->attribute_types;
}

/*!
 * \brief ldap_schema_append_attributetype Appends attribute type to the list of schema's attribute types.
 * \param[in] schema                       Schema to work with.
 * \param[in] attributetype                Attribute type to add.
 * \return
 *        - false - on error.
 *        - true - on success.
 */
bool
ldap_schema_append_attributetype(struct ldap_schema_t *schema, LDAPAttributeType *attributetype)
{
    if (!schema || !attributetype)
    {
        if (!schema)
        {
            ld_error("Attempt to pass NULL schema parameter.\n");
        }

        if (!attributetype)
        {
            ld_error("Attempt to pass NULL attribute type parameter. \n");
        }

        return false;
    }

    if (schema->attribute_types_size >= schema->attribute_types_capacity - 1)
    {
        int required_capacity = schema->attribute_types_capacity * 2;
        TALLOC_CTX* ctx = talloc_parent(schema);
        LDAPAttributeType** attributes = talloc_realloc(ctx,
                                                        schema->attribute_types,
                                                        LDAPAttributeType*,
                                                        required_capacity);
        if (!attributes)
        {
            ld_error("Unable to increase capacity in schema %d, to value of %d. \n", schema, required_capacity);
            return false;
        }

        schema->attribute_types_capacity = required_capacity;
    }

    schema->attribute_types[schema->attribute_types_size] = attributetype;

    ++schema->attribute_types_size;

    schema->attribute_types[schema->attribute_types_size] = NULL;

    return true;
}

/*!
 * \brief ldap_schema_append_objectclass Appends object class to the list of schema's object classes.
 * \param[in] schema                     Current schema we working with.
 * \param[in] objectclass                Object class we want to add to schema.
 * \return
 *        - false - on error.
 *        - true - on success.
 */
bool
ldap_schema_append_objectclass(struct ldap_schema_t *schema, LDAPObjectClass *objectclass)
{
    if (!schema || !objectclass)
    {
        if (!schema)
        {
            ld_error("Attempt to pass NULL schema parameter.\n");
        }

        if (!objectclass)
        {
            ld_error("Attempt to pass NULL object class parameter. \n");
        }

        return false;
    }

    if (schema->object_classes_size >= schema->object_classes_capacity - 1)
    {
        int required_capacity = schema->object_classes_capacity * 2;
        TALLOC_CTX* ctx = talloc_parent(schema);
        LDAPObjectClass** classes = talloc_realloc(ctx,
                                                   schema->object_classes,
                                                   LDAPObjectClass*,
                                                   required_capacity);
        if (!classes)
        {
            ld_error("Unable to increase capacity in schema %d, to value of %d. \n", schema, required_capacity);
            return false;
        }

        schema->object_classes_capacity = required_capacity;
    }

    schema->object_classes[schema->object_classes_size] = objectclass;

    ++schema->object_classes_size;

    schema->object_classes[schema->object_classes_size] = NULL;

    return true;
}

/**
 * @brief ldap_schema_load  Loads the schema from the connection depending on the type of directory.
 * @param[in] connection    Connection to work with.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode
ldap_schema_load(struct ldap_connection_ctx_t* connection)
{
    switch (connection->directory_type)
    {
    case LDAP_TYPE_OPENLDAP:
        return schema_load_openldap(connection, connection->schema);

    case LDAP_TYPE_ACTIVE_DIRECTORY:
        // TODO: move call `schema_load_active_directory` function
        return RETURN_CODE_SUCCESS;

    case LDAP_TYPE_FREE_IPA:
        // TODO: move call `schema_load_free_ipa` function
        return RETURN_CODE_FAILURE;

    case LDAP_TYPE_UNKNOWN:
        // TODO
        return RETURN_CODE_SUCCESS;
    }

    return RETURN_CODE_SUCCESS;
}

bool
ldap_schema_ready(struct ldap_connection_ctx_t* connection)
{
    switch (connection->directory_type)
    {
    case LDAP_TYPE_OPENLDAP:
        return connection->schema->attribute_types_size > 0
                && connection->schema->object_classes_size > 0;
    default:
        return true;
    }
}

/**
 * @brief ldap_schema_read_entry   Reads attribute values of entry and writes to output parameter.
 * @param[in] entry                Entry to work with.
 * @param[in] callback             Callback for processing attribute values.
 * @param[in] user_data            An output parameter for returning data from callback.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode
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
enum OperationReturnCode
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
