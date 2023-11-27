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
#ifndef LIBDOMAIN_DIRECTORY_H
#define LIBDOMAIN_DIRECTORY_H

#include "common.h"
#include "connection.h"

enum LdapDirectoryType
{
    LDAP_TYPE_UNINITIALIZED    = -1,
    LDAP_TYPE_UNKNOWN          =  0,
    LDAP_TYPE_ACTIVE_DIRECTORY =  1,
    LDAP_TYPE_OPENLDAP         =  2,
    LDAP_TYPE_FREE_IPA         =  3
};

enum OperationReturnCode directory_get_type(struct ldap_connection_ctx_t *connection);
enum OperationReturnCode directory_parse_result(int rc, LDAPMessage *message, struct ldap_connection_ctx_t *connection);

#endif //LIBDOMAIN_DIRECTORY_H
