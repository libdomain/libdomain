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

#ifndef LIB_DOMAIN_PARSERS_H
#define LIB_DOMAIN_PARSERS_H

#include <talloc.h>
#include <ldap_schemas.h>

LDAPObjectClass* parse_object_class_description(TALLOC_CTX* talloc_ctx, const char *const in, const size_t len);

LDAPAttributeType* parse_attribute_type_description(TALLOC_CTX *talloc_ctx, const char *const in, const size_t len);

#endif//LIB_DOMAIN_PARSERS_H