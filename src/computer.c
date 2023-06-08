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

#include "computer.h"

#include "common.h"
#include "domain_p.h"
#include "entry.h"

static attribute_value_pair_t LDAP_COMPUTER_ATTRIBUTES[] =
{
    { "objectClass", { "top", "computer", NULL, NULL, NULL } },
    { "cn", { NULL, NULL, NULL, NULL, NULL } },
    { "description", { NULL, NULL, NULL, NULL, NULL } },
    { "displayName", { NULL, NULL, NULL, NULL, NULL } },
    { "name", { NULL, NULL, NULL, NULL, NULL } },
};
static const int LDAP_COMPUTER_ATTRIBUTES_SIZE = number_of_elements(LDAP_COMPUTER_ATTRIBUTES);

enum ComputerAttributeIndex
{
    OBJECT_CLASS = 0,
    CN           = 1,
    DESCRIPTION  = 2,
    DISPLAY_NAME = 3,
    NAME         = 4,
};

/**
 * @brief ld_add_computer   Creates new computer
 * @param[in] handle        LibDomain handle
 * @param[in] name          name of the computer
 * @param[in] description   Description of the computer.
 * @param[in] display_name  Display name of the computer.
 * @param[in] parent        Parent container of the computer.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_add_computer(LDHandle *handle,
                                         const char *name,
                                         const char *description,
                                         const char *display_name,
                                         const char *parent)
{
    const char *dn = handle ? handle->global_config->base_dn : NULL;
    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    LDAPAttribute_t **attrs  = assign_default_attribute_values(talloc_ctx,
                                                               LDAP_COMPUTER_ATTRIBUTES,
                                                               LDAP_COMPUTER_ATTRIBUTES_SIZE);

    check_and_assign_attribute(attrs, name, CN, talloc_ctx);
    check_and_assign_attribute(attrs, name, NAME, talloc_ctx);
    check_and_assign_attribute(attrs, description, DESCRIPTION, talloc_ctx);
    check_and_assign_attribute(attrs, display_name, DISPLAY_NAME, talloc_ctx);

    if (parent && strlen(parent) > 0)
    {
        dn = parent;
    }

    rc = ld_add_entry(handle, name, dn, "cn", attrs);

    talloc_free(talloc_ctx);

    return rc;
}

/**
 * @brief ld_del_computer Deletes computer
 * @param[in] handle      Pointer to libdomain session handle.
 * @param[in] name        Name of the computer.
 * @param[in] parent      Parent container of the computer.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_del_computer(LDHandle *handle, const char *name, const char *parent)
{
    (void)(parent);
    return ld_del_entry(handle, name, handle ? handle->global_config->base_dn : NULL, "cn");
}

/**
 * @brief ld_mod_computer    Modifies computer
 * @param[in] handle         Pointer to libdomain session handle.
 * @param[in] name           Name of the computer.
 * @param[in] parent         Parent container of the computer.
 * @param[in] computer_attrs List of the attributes to modify.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_mod_computer(LDHandle *handle, const char *name, const char *parent, LDAPAttribute_t **computer_attrs)
{
    (void)(parent);
    return ld_mod_entry(handle, name, handle ? handle->global_config->base_dn : NULL, "cn", computer_attrs);
}

/**
 * @brief ld_rename_computer Renames computer
 * @param[in] handle         Pointer to libdomain session handle.
 * @param[in] old_name       Old name of the computer.
 * @param[in] new_name       New name of the computer.
 * @param[in] parent         Parent container of the computer.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_rename_computer(LDHandle *handle, const char *old_name, const char *new_name, const char *parent)
{
    return ld_rename_entry(handle, old_name, new_name, parent ? parent : handle ? handle->global_config->base_dn : NULL, "cn");
}
