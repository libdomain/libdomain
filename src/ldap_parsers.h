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

#ifndef LIB_DOMAIN_LDAP_PARSERS_H
#define LIB_DOMAIN_LDAP_PARSERS_H

#include <talloc.h>
#include <ldap_schema.h>

LDAPAttributeType* parse_attribute_type(TALLOC_CTX *talloc_ctx, const char *value);

LDAPObjectClass* parse_object_class(TALLOC_CTX* talloc_ctx, const char *value);

#endif//LIB_DOMAIN_LDAP_PARSERS_H
