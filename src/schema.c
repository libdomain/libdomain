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

#include "entry.h"

#include <talloc.h>

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
        error("NULL talloc context.\n");

        return NULL;
    }

    ldap_schema_t* result = talloc_zero(ctx, struct ldap_schema_t);

    if (!result)
    {
        error("Unable to allocate ldap_schema_t.\n");

        return NULL;
    }

    result->attribute_types = talloc_array(ctx, LDAPAttributeType*, 1024);

    if (!result->attribute_types)
    {
        error("Unable allocate attribute types in schema: %d ", result);

        return NULL;
    }

    result->attribute_types_capacity = 1024;
    result->attribute_types_size = 0;

    result->object_classes = talloc_array(ctx, LDAPObjectClass*, 1024);

    if (!result->object_classes)
    {
        error("Unable allocate object classes in schema: %d ", result);

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
        error("Schema is NULL.\n");

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
        error("Schema is NULL.\n");

        return NULL;
    }

    return schema->attribute_types;
}

/*!
 * \brief ldap_schema_append_attributetype
 * \param schema
 * \param attributetype
 * \return
 */
bool
ldap_schema_append_attributetype(struct ldap_schema_t *schema, LDAPAttributeType *attributetype)
{
    if (!schema || !attributetype)
    {
        if (!schema)
        {
            error("Attempt to pass NULL schema parameter.\n");
        }

        if (!attributetype)
        {
            error("Attempt to pass NULL attribute type parameter. \n");
        }

        return false;
    }

    if (schema->attribute_types_size >= schema->attribute_types_capacity)
    {
        int required_capacity = schema->attribute_types_capacity * 2;
        TALLOC_CTX* ctx = talloc_parent(schema);
        LDAPAttributeType** attributes = talloc_realloc(ctx,
                                                        schema->attribute_types,
                                                        LDAPAttributeType*,
                                                        required_capacity);
        if (!attributes)
        {
            error("Unable to increase capacity in schema %d, to value of %d. \n", schema, required_capacity);
            return false;
        }

        schema->attribute_types_capacity = required_capacity;
    }

    schema->attribute_types[schema->attribute_types_size] = attributetype;

    ++schema->attribute_types_size;

    return true;
}

/*!
 * \brief ldap_schema_append_objectclass
 * \param schema
 * \param objectclass
 * \return
 */
bool
ldap_schema_append_objectclass(struct ldap_schema_t *schema, LDAPObjectClass *objectclass)
{
    if (!schema || !objectclass)
    {
        if (!schema)
        {
            error("Attempt to pass NULL schema parameter.\n");
        }

        if (!objectclass)
        {
            error("Attempt to pass NULL object class parameter. \n");
        }

        return false;
    }

    if (schema->object_classes_size >= schema->object_classes_capacity)
    {
        int required_capacity = schema->object_classes_capacity * 2;
        TALLOC_CTX* ctx = talloc_parent(schema);
        LDAPObjectClass** classes = talloc_realloc(ctx,
                                                   schema->object_classes,
                                                   LDAPObjectClass*,
                                                   required_capacity);
        if (!classes)
        {
            error("Unable to increase capacity in schema %d, to value of %d. \n", schema, required_capacity);
            return false;
        }

        schema->object_classes_capacity = required_capacity;
    }

    schema->object_classes[schema->object_classes_size] = objectclass;

    ++schema->object_classes_size;

    return true;
}

enum OperationReturnCode schema_load_active_directory(struct ldap_connection_ctx_t *connection,
                                                      struct ldap_schema_t *schema,
                                                      const char* basedn)
{
    TALLOC_CTX* talloc_ctx = talloc_new(NULL);
    int rc = RETURN_CODE_SUCCESS;

    if (!talloc_ctx)
    {
        error("schema_load_active_directory - unable to allocate memory.\n");

        return RETURN_CODE_FAILURE;
    }

    const char* schema_dn = talloc_asprintf(talloc_ctx, "cn=schema,cn=configuration,%s", basedn);

    rc = search(connection,
           schema_dn,
           LDAP_SCOPE_SUB,
           "(objectClass=attributeSchema)",
           NULL,
           false,
           NULL);

    if (rc != RETURN_CODE_SUCCESS)
    {
        error("schema_load_active_directory - unable to search attributes.\n");

        return RETURN_CODE_FAILURE;
    }

    rc = search(connection,
           schema_dn,
           LDAP_SCOPE_SUB,
           "(objectClass=classSchema)",
           NULL,
           false,
           NULL);

    if (rc != RETURN_CODE_SUCCESS)
    {
        error("schema_load_active_directory - unable to search object classes.\n");

        return RETURN_CODE_FAILURE;
    }

    talloc_free(talloc_ctx);

    return RETURN_CODE_SUCCESS;
}
