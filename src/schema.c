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

#include "domain.h"

#include "entry.h"

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

static LDAPObjectClass** get_objectclasses_from_entry_on_schema(TALLOC_CTX* ctx, ldap_schema_t* schema,
                                                                ld_entry_t* entry, char** objectclass_names)
{
    LDAPAttribute_t* objectclass_attribute = ld_entry_get_attribute(entry, "objectClass");
    return_null_if_null(objectclass_attribute, "get_objectclasses_from_entry_on_schema - attribute objectClass is NULL!\n");

    char** objectclass_attribute_values = objectclass_attribute->values;
    return_null_if_null(objectclass_attribute_values,
                        "get_objectclasses_from_entry_on_schema - attribute objectClass values is NULL!\n");

    int max_array_length = 0;
    while (objectclass_names[max_array_length] != NULL)
    {
        ++max_array_length;
    }

    LDAPObjectClass** result = talloc_array(ctx, LDAPObjectClass*, max_array_length);
    return_null_if_null(result, "get_objectclasses_from_entry_on_schema - talloc_array returned NULL!\n");

    int true_array_length = 0;

    for (int i = 0; objectclass_names[i] != NULL; ++i)
    {
        for (int j = 0; objectclass_attribute_values[j] != NULL; ++j)
        {
            if (!strcmp(objectclass_names[i], objectclass_attribute_values[j]))
            {
                result[i] = ldap_schema_get_objectclass_by_name(schema, objectclass_names[i]);
                return_null_if_null(result[true_array_length],
                                    "get_objectclasses_from_entry_on_schema - entry objectClass values does not match parameter!\n");
                ++true_array_length;

                break;
            }
        }
    }

    result[true_array_length] = NULL;

    return result;
}

static int g_hash_table_destructor(TALLOC_CTX *ctx)
{
    GHashTable *table = NULL;
    table = talloc_get_type_abort(ctx, GHashTable);

    g_hash_table_destroy(table);

    return 0;
}

static GHashTable*
get_attributes_dictionary_by_entry_names(TALLOC_CTX* ctx, ld_entry_t* entry)
{
    TALLOC_CTX* hash_ctx = talloc(ctx, void*);

    GHashTable* entry_attributes_by_name = g_hash_table_new(g_str_hash, g_str_equal);
    return_null_if_null(entry_attributes_by_name, "get_attributes_dictionary_by_entry_names - g_hash_table_new returned NULL!\n");

    talloc_steal(hash_ctx, entry_attributes_by_name);
    talloc_set_destructor(hash_ctx, g_hash_table_destructor);

    LDAPAttribute_t** attributes = ld_entry_get_attributes(entry);
    for (int i = 0; attributes[i] != NULL; ++i)
    {
        char* attribute_name = attributes[i]->name;
        return_null_if_null(attribute_name, "get_attributes_dictionary_by_entry_names - attribute name is NULL!\n");

        g_hash_table_insert(entry_attributes_by_name, attribute_name, attributes[i]);
    }

    return entry_attributes_by_name;
}

static bool
entry_contains_all_objectclass_must_attributes(ldap_schema_t* schema, GHashTable* entry_attributes_by_name, LDAPObjectClass* objectclass)
{
    char** at_oids = objectclass->oc_at_oids_must;

    if (at_oids == NULL)
    {
        return true;
    }

    for (int i = 0; at_oids[i] != NULL; ++i)
    {
        LDAPAttributeType* current_at = ldap_schema_get_attributetype_by_oid(schema, at_oids[i]);

        if (current_at == NULL || current_at->at_names == NULL)
        {
            ld_error("entry_contains_all_objectclass_must_attributes - missing required attribute for objectClass %s\n",
                     objectclass->oc_names[0]);

            return false;
        }

        char** at_names = current_at->at_names;

        bool contains_current_at = false;

        for (int j = 0; at_names[j] != NULL; ++j)
        {
            if (g_hash_table_contains(entry_attributes_by_name, at_names[j]))
            {
                contains_current_at = true;
                break;
            }
        }

        if (!contains_current_at)
        {
            return false;
        }
    }

    return true;
}

static bool
attribute_in_oid_list(ldap_schema_t* schema, LDAPAttributeType* target_attribute, char** list_of_oids)
{
    return_null_if_null(list_of_oids, "attribute_in_list_by_oid - list_of_oids parameter is NULL!\n");
    return_null_if_null(target_attribute, "attribute_in_list_by_oid - target_attribute parameter is NULL!\n");

    char* target_oid = target_attribute->at_oid;
    return_null_if_null(target_attribute->at_oid, "attribute_in_list_by_oid - target_attribute oid is NULL!\n");

    for (int i = 0; list_of_oids[i] != NULL; ++i)
    {
        if (!strcmp(target_oid, list_of_oids[i]))
        {
            return true;
        }
    }

    return false;
}

bool
ldap_schema_validate_entry(ldap_schema_t* schema, ld_entry_t* entry, char** objectclass_names)
{
    return_null_if_null(schema, "ldap_schema_validate_entry - schema parameter is NULL!\n");
    return_null_if_null(entry, "ldap_schema_validate_entry - entry parameter is NULL!\n");
    return_null_if_null(objectclass_names, "ldap_schema_validate_entry - objectclass_names parameter is NULL!\n");

    TALLOC_CTX* ctx = talloc_new(NULL);

    LDAPObjectClass** objectclasses = get_objectclasses_from_entry_on_schema(ctx, schema, entry, objectclass_names);
    return_null_if_null(entry, "ldap_schema_validate_entry - objectclasses is NULL!\n");

    GHashTable* entry_attributes_by_name = get_attributes_dictionary_by_entry_names(ctx, entry);
    return_null_if_null(entry, "ldap_schema_validate_entry - entry attributes is NULL!\n");

    for (int i = 0; objectclasses[i] != NULL; ++i)
    {
        if (!entry_contains_all_objectclass_must_attributes(schema, entry_attributes_by_name, objectclasses[i]))
        {
            talloc_free(ctx);

            return false;
        }

        GHashTableIter attributes_iter;
        gpointer attributes_key = NULL, attribute_value = NULL;

        g_hash_table_iter_init(&attributes_iter, entry_attributes_by_name);
        while (g_hash_table_iter_next(&attributes_iter, &attributes_key, &attribute_value))
        {
            LDAPAttribute_t* attribute = attribute_value;

            LDAPAttributeType* current_at = ldap_schema_get_attributetype_by_name(schema, attribute->name);
            if (!attribute_in_oid_list(schema, current_at, objectclasses[i]->oc_at_oids_must)
                    || !attribute_in_oid_list(schema, current_at, objectclasses[i]->oc_at_oids_may))
            {
                talloc_free(ctx);

                return false;
            }
        }
    }

    talloc_free(ctx);

    return true;
}
