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
#ifndef LIBDOMAIN_ENTRY_H
#define LIBDOMAIN_ENTRY_H

#include "connection.h"

void add_on_read(evutil_socket_t fd, short flags, void *arg);
void add_on_write(evutil_socket_t fd, short flags, void *arg);

void whoami(struct ldap_connection_ctx_t *connection);

void search(struct ldap_connection_ctx_t *connection,
            const char *base_dn,
            int scope,
            const char *filter,
            char **attrs,
            bool attrsonly);

enum OperationReturnCode search_on_read(int rc, LDAPMessage *message, struct ldap_connection_ctx_t *connection);
void search_on_write(evutil_socket_t fd, short flags, void *arg);

void modify_on_read(evutil_socket_t fd, short flags, void *arg);
void modify_on_write(evutil_socket_t fd, short flags, void *arg);

void delete_on_read(evutil_socket_t fd, short flags, void *arg);
void delete_on_write(evutil_socket_t fd, short flags, void *arg);

enum OperationReturnCode whoami_on_read(int rc, LDAPMessage *message, struct ldap_connection_ctx_t *connection);

#endif //LIBDOMAIN_ENTRY_H
