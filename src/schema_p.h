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

#ifndef LIB_DOMAIN_SCHEMA_PRIVATE_H
#define LIB_DOMAIN_SCHEMA_PRIVATE_H

#include "common.h"

#include <stdbool.h>

#include <ldap.h>
#include <ldap_schema.h>

struct ldap_schema_t
{
    LDAPObjectClass** object_classes;

    LDAPAttributeType** attribute_types;

    int object_classes_size;
    int object_classes_capacity;

    int attribute_types_size;
    int attribute_types_capacity;
};

#endif//LIB_DOMAIN_SCHEMA_PRIVATE_H
