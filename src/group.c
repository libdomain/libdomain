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
    { "objectClass", "group" },
    { "cn", NULL },
    { "description", NULL },
    { "displayName", NULL },
    { "groupType", NULL },
    { "wWWHomePage", NULL },
    { "sAMAccountName", NULL },
};
static const int LDAP_GROUP_ATTRIBUTES_SIZE = number_of_elements(LDAP_GROUP_ATTRIBUTES);

enum GroupAttributeIndex
{
    OBJECT_CLASS = 0,
    CN           = 1,
    DESCRIPTION  = 2,
    DISPLAY_NAME = 3,
    GROUP_TYPE   = 4,
    HOME_PAGE    = 5,
    ACCOUNT_NAME = 6
};

static int value_from_group_scope(enum GroupScope group_scope)
{
    switch (group_scope) {
    case GROUP_SCOPE_GLOBAL:
        return 0x00000002;
    case GROUP_SCOPE_DOMAIN_LOCAL:
        return 0x00000004;
    case GROUP_SCOPE_UNIVERSAL:
        return 0x00000008;
    default:
        break;
    }

    return 0;
}

enum OperationReturnCode ld_add_group(LDHandle *handle,
                                      const char *name,
                                      const char *description,
                                      const char *display_name,
                                      enum GroupCategory group_category,
                                      enum GroupScope group_scope,
                                      const char *home_page,
                                      const char *parent,
                                      const char *sam_account_name)
{
    (void)(group_category);

    const char *dn = handle ? handle->global_config->base_dn : NULL;
    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    LDAPAttribute_t **group_attrs  = assign_default_attribute_values(talloc_ctx,
                                                                     LDAP_GROUP_ATTRIBUTES,
                                                                     LDAP_GROUP_ATTRIBUTES_SIZE);

    char* group_type = talloc_asprintf(talloc_ctx, "%d", value_from_group_scope(group_scope));

    check_and_assign_attribute(group_attrs, name, CN, talloc_ctx);
    check_and_assign_attribute(group_attrs, description, DESCRIPTION, talloc_ctx);
    check_and_assign_attribute(group_attrs, display_name, DISPLAY_NAME, talloc_ctx);
    check_and_assign_attribute(group_attrs, group_type, GROUP_TYPE, talloc_ctx);
    check_and_assign_attribute(group_attrs, home_page, HOME_PAGE, talloc_ctx);
    check_and_assign_attribute(group_attrs, sam_account_name, ACCOUNT_NAME, talloc_ctx);

    if (parent && strlen(parent) > 0)
    {
        dn = parent;
    }

    rc = ld_add_entry(handle, name, dn, group_attrs);

    talloc_free(talloc_ctx);

    return rc;
}

enum OperationReturnCode ld_del_group(LDHandle *handle, const char *name)
{   
    return ld_del_entry(handle, name, handle ? handle->global_config->base_dn : NULL);
}

enum OperationReturnCode ld_mod_group(LDHandle *handle,  const char *name, LDAPAttribute_t **group_attrs)
{
    return ld_mod_entry(handle, name, handle ? handle->global_config->base_dn : NULL, group_attrs);
}

enum OperationReturnCode ld_rename_group(LDHandle *handle, const char *old_name, const char *new_name)
{
    return ld_rename_entry(handle, old_name, new_name, handle ? handle->global_config->base_dn : NULL);
}

static enum OperationReturnCode group_member_modify(LDHandle *handle, const char *group_name, const char *user_name,
                                                    char mod_operation)
{
    const char *this_group_name = NULL;
    const char *this_user_name = NULL;
    const char *member = "member";

    check_handle(handle, "ld_group_add_user");

    check_string(group_name, this_group_name, "ld_group_add_user");

    check_string(user_name, this_user_name, "ld_group_add_user");

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    LDAPMod **attrs = talloc_array(talloc_ctx, LDAPMod*, 2);
    attrs[0]->mod_op = mod_operation;
    attrs[0]->mod_type = talloc_strndup(talloc_ctx, member, strlen(member));
    attrs[0]->mod_values = talloc_array(talloc_ctx, char*, 2);

    attrs[0]->mod_values[0] = talloc_strndup(talloc_ctx, this_user_name, strlen(this_user_name));
    attrs[0]->mod_values[1] = NULL;
    attrs[1] = NULL;

    int rc = modify(handle->connection_ctx, this_group_name, attrs);

    talloc_free(talloc_ctx);

    return rc;
}

enum OperationReturnCode ld_group_add_user(LDHandle *handle, const char *group_name, const char *user_name)
{
    return group_member_modify(handle, group_name, user_name, LDAP_MOD_ADD);
}

enum OperationReturnCode ld_group_remove_user(LDHandle *handle, const char *group_name, const char *user_name)
{
    return group_member_modify(handle, group_name, user_name, LDAP_MOD_DELETE);
}

