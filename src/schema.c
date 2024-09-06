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

#include <talloc.h>

#include <ldap.h>
#include <ldap_schema.h>

#define return_null_if_null(parameter, error) \
    if (parameter == NULL) \
    { \
        ld_error(error); \
        return NULL; \
    }

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
    return_null_if_null(ctx, "NULL talloc context.\n")

    ldap_schema_t* result = talloc_zero(ctx, struct ldap_schema_t);
    return_null_if_null(result, "Unable to allocate ldap_schema_t.\n")

    result->attribute_types_by_oid = g_hash_table_new(g_str_hash, g_str_equal);
    result->attribute_types_by_name = g_hash_table_new(g_str_hash, g_str_equal);

    if (!result->attribute_types_by_oid || !result->attribute_types_by_name)
    {
        talloc_free(result);

        ld_error("ldap_schema_new - out of memory - unable to create attribute types in schema!\n");

        return NULL;
    }

    result->object_classes_by_oid = g_hash_table_new(g_str_hash, g_str_equal);
    result->object_classes_by_name = g_hash_table_new(g_str_hash, g_str_equal);

    if (!result->object_classes_by_oid || !result->object_classes_by_name)
    {
        talloc_free(result);

        ld_error("ldap_schema_new - out of memory - unable to create object classes in schema!\n");

        return NULL;
    }

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
    return_null_if_null(schema, "ldap_schema_object_classes - schema is NULL!\n");
    return_null_if_null(schema->object_classes_by_oid,
                             "ldap_schema_object_classes - object_classes_by_oid is NULL!\n");

    int result_size = g_hash_table_size(schema->object_classes_by_oid);

    LDAPObjectClass** result = talloc_array(schema, LDAPObjectClass*, result_size + 1);

    return_null_if_null(result,
                             "ldap_schema_object_classes - talloc_array for LDAPObjectClass returned NULL!\n");

    GHashTableIter iter;
    gpointer key = NULL, value = NULL;

    int index = 0;
    g_hash_table_iter_init(&iter, schema->object_classes_by_oid);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        result[index] = value;
        index++;
    }
    result[result_size] = NULL;

    return result;
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
    return_null_if_null(schema, "ldap_schema_attribute_types - schema is NULL!\n");
    return_null_if_null(schema->attribute_types_by_oid,
                             "ldap_schema_attribute_types - attribute_types_by_oid is NULL!\n");

    int result_size = g_hash_table_size(schema->attribute_types_by_oid);

    LDAPAttributeType** result = talloc_array(schema, LDAPAttributeType*, result_size + 1);

    return_null_if_null(result,
                             "ldap_schema_attribute_types - talloc_array for LDAPAttributeType returned NULL!\n");

    GHashTableIter iter;
    gpointer key = NULL, value = NULL;

    int index = 0;
    g_hash_table_iter_init(&iter, schema->attribute_types_by_oid);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        result[index] = value;
        index++;
    }
    result[result_size] = NULL;

    return result;
}

/*!
 * \brief ldap_schema_get_objectclass_by_oid    Returns a LDAPObjectClass struct corresponding to the specified OID.
 * \param[in] schema                            Schema to work with.
 * \return
 *        - NULL if schema is NULL.
 *        - Object class from schema.
 */
LDAPObjectClass *ldap_schema_get_objectclass_by_oid(const ldap_schema_t* schema, const char *oid)
{
    return_null_if_null(schema, "ldap_schema_get_objectclass_by_oid - schema is NULL!\n");
    return_null_if_null(schema->object_classes_by_oid,
                             "ldap_schema_get_objectclass_by_oid - object_classes_by_oid is NULL!\n");

    return (LDAPObjectClass *)g_hash_table_lookup(schema->object_classes_by_oid, oid);
}

/*!
 * \brief ldap_schema_get_objectclass_by_name   Returns a LDAPObjectClass struct corresponding to the specified name.
 * \param[in] schema                            Schema to work with.
 * \return
 *        - NULL if schema is NULL.
 *        - Object class from schema.
 */
LDAPObjectClass *ldap_schema_get_objectclass_by_name(const ldap_schema_t* schema, const char *name)
{
    return_null_if_null(schema, "ldap_schema_get_objectclass_by_name - schema is NULL!\n");
    return_null_if_null(schema->object_classes_by_name,
                             "ldap_schema_get_objectclass_by_name - object_classes_by_name is NULL!\n");

    return (LDAPObjectClass *)g_hash_table_lookup(schema->object_classes_by_name, name);
}

/*!
* \brief ldap_schema_get_attributetype_by_oid  Returns a LDAPAttributeType struct corresponding to the specified OID.
* \param[in] schema                            Schema to work with.
* \return
*        - NULL if schema is NULL.
*        - Attribute type from schema.
*/
LDAPAttributeType *ldap_schema_get_attributetype_by_oid(const ldap_schema_t* schema, const char *oid)
{
    return_null_if_null(schema, "ldap_schema_get_attributetype_by_oid - schema is NULL!\n");
    return_null_if_null(schema->attribute_types_by_oid,
                             "ldap_schema_get_attributetype_by_oid - attribute_types_by_oid is NULL!\n");

    return (LDAPAttributeType *)g_hash_table_lookup(schema->attribute_types_by_oid, oid);
}

/*!
* \brief ldap_schema_get_attributetype_by_name  Returns a LDAPAttributeType struct corresponding to the specified name.
* \param[in] schema                             Schema to work with.
* \return
*        - NULL if schema is NULL.
*        - Attribute type from schema.
*/
LDAPAttributeType *ldap_schema_get_attributetype_by_name(const ldap_schema_t* schema, const char *name)
{
    return_null_if_null(schema, "ldap_schema_get_attributetype_by_name - schema is NULL!\n");
    return_null_if_null(schema->attribute_types_by_name,
                             "ldap_schema_get_attributetype_by_name - attribute_types_by_name is NULL!\n");

    return (LDAPAttributeType *)g_hash_table_lookup(schema->attribute_types_by_name, name);
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
    return_null_if_null(schema, "Attempt to pass NULL schema parameter.\n");
    return_null_if_null(attributetype, "Attempt to pass NULL attribute type parameter.\n");

    char** attributetype_names = attributetype->at_names;
    return_null_if_null(attributetype_names, "Attribute type names list is empty!\n");

    return_null_if_null(attributetype->at_oid, "ldap_schema_append_attributetype - oid of attribute type parameter is NULL!\n");

    bool result = g_hash_table_insert(schema->attribute_types_by_oid, attributetype->at_oid, attributetype);

    for (int i = 0; attributetype_names[i] != NULL; ++i)
    {
        result = g_hash_table_insert(schema->attribute_types_by_name, attributetype_names[i], attributetype);
    }

    return result;
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
    return_null_if_null(schema, "Attempt to pass NULL schema parameter.\n");
    return_null_if_null(objectclass, "Attempt to pass NULL object class parameter.\n");

    char** objectclass_names = objectclass->oc_names;
    return_null_if_null(objectclass_names, "Object class names list is empty!\n");

    return_null_if_null(objectclass->oc_oid, "ldap_schema_append_objectclass - oid of object class parameter is NULL!\n");

    bool result = g_hash_table_insert(schema->object_classes_by_oid, objectclass->oc_oid, objectclass);

    for (int i = 0; objectclass_names[i] != NULL; ++i)
    {
        result = g_hash_table_insert(schema->object_classes_by_name, objectclass_names[i], objectclass);
    }

    return result;
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
        return g_hash_table_size(connection->schema->object_classes_by_oid) > 0
                && g_hash_table_size(connection->schema->attribute_types_by_oid) > 0;
    default:
        return true;
    }
}
