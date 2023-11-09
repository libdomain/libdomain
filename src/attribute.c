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

#include "attribute.h"
#include "common.h"
#include "domain_p.h"

#include <ldap.h>

static const char* create_attribute_parent(LDHandle *handle)
{
    return handle ? handle->global_config->base_dn : "";
}

enum OperationReturnCode ld_add_attributes(LDHandle *handle, const char *cn, struct LDAPAttribute_s **attrs)
{
    return ld_mod_entry_attrs(handle, cn, create_attribute_parent(handle), "", attrs, LDAP_MOD_ADD);
}

enum OperationReturnCode ld_del_attributes(LDHandle *handle, const char *cn, struct LDAPAttribute_s **attrs)
{
    return ld_mod_entry_attrs(handle, cn, create_attribute_parent(handle), "", attrs, LDAP_MOD_DELETE);
}
