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

enum OperationReturnCode ld_add_group(LDHandle *handle, const char *name, const char *base_dn)
{
    const char* group_name = NULL;
    const char* group_dn = NULL;

    check_handle(handle, "ld_add_group");

    check_string(name, group_name, "ld_add_group");

    if (!base_dn || strlen(base_dn) == 0)
    {
        group_dn = "";
    }
    else
    {
        group_dn = base_dn;
    }

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    LDAPMod **attrs = talloc_array(talloc_ctx, LDAPMod*, 1);
    attrs[0] = NULL;

    int rc = add(handle->connection_ctx, group_name, attrs);

    talloc_free(talloc_ctx);

    return rc;
}

enum OperationReturnCode ld_del_group(LDHandle *handle, const char *name)
{
    const char* group_name = NULL;

    check_handle(handle, "ld_del_group");

    check_string(name, group_name, "ld_del_group");

    return delete(handle->connection_ctx, group_name);
}

enum OperationReturnCode ld_mod_group(LDHandle *handle,  const char *name, const char *comment)
{
    const char* group_comment = NULL;
    const char* group_name = NULL;

    check_handle(handle, "ld_mod_group");

    check_string(name, group_name, "ld_mod_group");

    if (!comment || strlen(comment) == 0)
    {
        group_comment = "";
    }
    else
    {
        group_comment = comment;
    }

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    LDAPMod **attrs = talloc_array(talloc_ctx, LDAPMod*, 1);
    attrs[0] = NULL;

    int rc = modify(handle->connection_ctx, name, attrs);

    talloc_free(talloc_ctx);

    return rc;
}

enum OperationReturnCode ld_rename_group(LDHandle *handle, const char *old_name, const char *new_name)
{
    const char* group_old_name = NULL;
    const char* group_new_name = NULL;

    check_handle(handle, "ld_rename_group");

    check_string(old_name, group_old_name, "ld_rename_group");

    check_string(new_name, group_new_name, "ld_rename_group");

    return ld_rename(handle->connection_ctx, old_name, new_name, NULL, false);
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
