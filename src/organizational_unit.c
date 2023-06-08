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

#include "organizational_unit.h"
#include "common.h"
#include "domain_p.h"
#include "entry.h"

static attribute_value_pair_t LDAP_OU_ATTRIBUTES[] =
{
    { "objectClass", { "top", "organizationalUnit", NULL, NULL, NULL } },
    { "ou", { NULL, NULL, NULL, NULL, NULL } },
    { "description", { NULL, NULL, NULL, NULL, NULL } },
};
static const int LDAP_OU_ATTRIBUTES_SIZE = number_of_elements(LDAP_OU_ATTRIBUTES);

enum OUAttributeIndex
{
    OBJECT_CLASS = 0,
    OU           = 1,
    DESCRIPTION  = 2,
};

/**
 * @brief ld_add_ou       Creates the OU.
 * @param handle          Pointer to libdomain session handle.
 * @param name            Name of the OU.
 * @param description     Description of the OU.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_add_ou(LDHandle *handle,
                                   const char *name,
                                   const char *description,
                                   const char *parent)
{
    const char *dn = handle ? handle->global_config->base_dn : NULL;
    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    LDAPAttribute_t **attrs  = assign_default_attribute_values(talloc_ctx,
                                                               LDAP_OU_ATTRIBUTES,
                                                               LDAP_OU_ATTRIBUTES_SIZE);

    check_and_assign_attribute(attrs, name, OU, talloc_ctx);
    check_and_assign_attribute(attrs, description, DESCRIPTION, talloc_ctx);

    if (parent && strlen(parent) > 0)
    {
        dn = parent;
    }

    rc = ld_add_entry(handle, name, dn, "ou", attrs);

    talloc_free(talloc_ctx);

    return rc;
}

/**
 * @brief ld_del_ou   Deletes the OU.
 * @param handle      Pointer to libdomain session handle.
 * @param name        Name of the OU.
 * @param parent      Parent container that holds the OU.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_del_ou(LDHandle *handle, const char *name, const char *parent)
{
    return ld_del_entry(handle, name, parent ? parent : handle ? handle->global_config->base_dn : NULL, "ou");
}

/**
 * @brief ld_mod_ou    Modifies the OU.
 * @param handle       Pointer to libdomain session handle.
 * @param name         Name of the OU.
 * @param parent       Parent container that holds the OU.
 * @param ou_attrs     List of the attributes to modify.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_mod_ou(LDHandle *handle, const char *name, const char *parent, LDAPAttribute_t **ou_attrs)
{
    return ld_mod_entry(handle, name, parent ? parent : handle ? handle->global_config->base_dn : NULL, "ou", ou_attrs);
}

/**
 * @brief ld_rename_ou Renames the OU.
 * @param handle       Pointer to libdomain session handle.
 * @param old_name     Old name of the OU.
 * @param new_name     New name of the OU.
 * @param parent       Parent container that holds the OU.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_rename_ou(LDHandle *handle, const char *old_name, const char *new_name, const char *parent)
{
    return ld_rename_entry(handle, old_name, new_name, parent ? parent : handle ? handle->global_config->base_dn : NULL, "ou");
}
