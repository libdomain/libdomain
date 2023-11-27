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

#include "group.h"
#include "common.h"
#include "domain_p.h"
#include "entry.h"

#include <string.h>

static attribute_value_pair_t LDAP_GROUP_ATTRIBUTES[] =
{
    { "objectClass", { "top", "posixGroup", NULL, NULL, NULL } },
    { "cn", { NULL, NULL, NULL, NULL, NULL } },
    { "description", { NULL, NULL, NULL, NULL, NULL } },
    { "gidNumber", { NULL, NULL, NULL, NULL, NULL } }
};
static const int LDAP_GROUP_ATTRIBUTES_SIZE = number_of_elements(LDAP_GROUP_ATTRIBUTES);

enum GroupAttributeIndex
{
    OBJECT_CLASS = 0,
    CN           = 1,
    DESCRIPTION  = 2,
    GID_NUMBER   = 3,
};

/**
 * @brief ld_add_group     Creates the group.
 * @param[in] handle           Pointer to libdomain session handle.
 * @param[in] name             Name of a group.
 * @param[in] attributes       List of group attributes depends on directory type.
 * @param[in] parent           Parent container that holds the group.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_add_group(LDHandle *handle,
                                      const char* name,
                                      LDAPAttribute_t** attributes,
                                      const char* parent)
{
    const char *dn = handle ? handle->global_config->base_dn : NULL;
    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    if (parent && strlen(parent) > 0)
    {
        dn = parent;
    }

    rc = ld_add_entry(handle, name, dn, "cn", attributes);

    return rc;
}

/**
 * @brief ld_del_group     Deletes the group.
 * @param[in] handle       Pointer to libdomain session handle.
 * @param[in] name         Name of the group.
 * @param[in] parent       Container that holds the group.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_del_group(LDHandle *handle, const char *name, const char* parent)
{
    return ld_del_entry(handle, name, parent ? parent : handle ? handle->global_config->base_dn : NULL, "cn");
}

/**
 * @brief ld_mod_group     Modifies the group.
 * @param[in] handle       Pointer to libdomain session handle.
 * @param[in] name         Name of the group.
 * @param[in] parent       Container that holds the group.
 * @param[in] group_attrs  List of attributes to modify.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_mod_group(LDHandle *handle,  const char *name, const char *parent,
                                      LDAPAttribute_t **group_attrs)
{
    return ld_mod_entry(handle, name, parent ? parent : handle ? handle->global_config->base_dn : NULL, "cn", group_attrs);
}

/**
 * @brief ld_rename_group     Renames group.
 * @param[in] handle          Pointer to libdomain session handle.
 * @param[in] old_name        Old name of the group.
 * @param[in] new_name        New name of the group.
 * @param[in] parent          Container that holds the group.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_rename_group(LDHandle *handle, const char *old_name, const char *new_name, const char *parent)
{
    return ld_rename_entry(handle, old_name, new_name, parent ? parent : handle ? handle->global_config->base_dn : NULL, "cn");
}

static enum OperationReturnCode group_member_modify(LDHandle *handle, const char *group_dn, const char *user_dn,
                                                    char mod_operation)
{
    const char *this_group_dn = NULL;
    const char *this_user_dn = NULL;
    const char *member = "member";

    check_handle(handle, "group_member_modify");

    check_string(group_dn, this_group_dn, "group_member_modify");

    check_string(user_dn, this_user_dn, "group_member_modify");

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    LDAPMod **attrs = talloc_array(talloc_ctx, LDAPMod*, 2);
    attrs[0] = talloc_zero(talloc_ctx, LDAPMod);
    attrs[0]->mod_op = mod_operation;
    attrs[0]->mod_type = talloc_strndup(talloc_ctx, member, strlen(member));
    attrs[0]->mod_values = talloc_array(talloc_ctx, char*, 2);

    attrs[0]->mod_values[0] = talloc_strndup(talloc_ctx, this_user_dn, strlen(this_user_dn));
    attrs[0]->mod_values[1] = NULL;
    attrs[1] = NULL;

    int rc = modify(handle->connection_ctx, this_group_dn, attrs);

    talloc_free(talloc_ctx);

    return rc;
}

/**
 * @brief ld_group_add_user Adds user to the group.
 * @param[in] handle            Pointer to libdomain session handle.
 * @param[in] group_name        Name of the group to add user into.
 * @param[in] user_name         Name of the user to add.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_group_add_user(LDHandle *handle, const char *group_name, const char *user_name)
{
    return group_member_modify(handle, group_name, user_name, LDAP_MOD_ADD);
}

/**
 * @brief ld_group_remove_user Removes user from the group.
 * @param[in] handle               Pointer to libdomain session handle.
 * @param[in] group_name           Name of the group to remove user from.
 * @param[in] user_name            Name of the user.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_group_remove_user(LDHandle *handle, const char *group_name, const char *user_name)
{
    return group_member_modify(handle, group_name, user_name, LDAP_MOD_DELETE);
}

