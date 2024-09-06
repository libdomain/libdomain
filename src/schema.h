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

#ifndef LIB_DOMAIN_SCHEMA_H
#define LIB_DOMAIN_SCHEMA_H

#include "common.h"
#include "connection.h"

#include <stdbool.h>

#include <ldap.h>
#include <ldap_schema.h>

typedef struct ldap_schema_t ldap_schema_t;

ldap_schema_t*
ldap_schema_new(TALLOC_CTX* ctx);


LDAPObjectClass**
ldap_schema_object_classes(const ldap_schema_t* schema);
bool ldap_schema_append_attributetype(ldap_schema_t* schema, LDAPAttributeType *attributetype);

LDAPAttributeType**
ldap_schema_attribute_types(const ldap_schema_t *schema);
bool ldap_schema_append_objectclass(ldap_schema_t* schema, LDAPObjectClass *objectclass);

LDAPAttributeType*
ldap_schema_get_attributetype_by_name(const ldap_schema_t* schema, const char *name);

LDAPAttributeType*
ldap_schema_get_attributetype_by_oid(const ldap_schema_t* schema, const char *oid);

LDAPObjectClass*
ldap_schema_get_objectclass_by_name(const ldap_schema_t* schema, const char *name);

LDAPObjectClass*
ldap_schema_get_objectclass_by_oid(const ldap_schema_t* schema, const char *oid);

enum OperationReturnCode
ldap_schema_load(struct ldap_connection_ctx_t* connection);

bool
ldap_schema_ready(struct ldap_connection_ctx_t* connection);

#endif//LIB_DOMAIN_SCHEMA_H
