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
#include "connection.h"

#include <stdbool.h>

#include <glib-2.0/glib.h>

#include <ldap.h>
#include <ldap_schema.h>

/*!
 * \brief The ldap_schema_t struct - Represents LDAP schema.
 */
struct ldap_schema_t
{
    GHashTable *object_classes_by_oid;               //!< Hash table of object classes by oc_oid key.
    GHashTable *object_classes_by_name;              //!< Hash table of object classes by oc_name key.
    GHashTable *attribute_types_by_oid;              //!< Hash table of attribute types by at_oid key.
    GHashTable *attribute_types_by_name;             //!< Hash table of attribute types by at_name key.
};

enum OperationReturnCode schema_load_openldap(struct ldap_connection_ctx_t* connection,
                                              struct ldap_schema_t* schema);

#endif//LIB_DOMAIN_SCHEMA_PRIVATE_H
