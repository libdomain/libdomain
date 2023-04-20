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
#ifndef LIBDOMAIN_CONNECTION_H
#define LIBDOMAIN_CONNECTION_H

#include <event2/event.h>
#include <ldap.h>
#include <stdbool.h>

#include "common.h"

struct ldap_connection_config_t
{
    const char *server;
    int port;
    bool chase_referrals;
    bool start_tls;
    bool sasl;
} ldap_connection_config_t;

struct ldap_connection_ctx_t
{
    LDAP *ldap;

    struct ldap_connection_ctx_t *next;
    struct ldap_connection_ctx_t *prev;

    evutil_socket_t fd;

    struct event_base *base;

    struct event *read_event;
    struct event *write_event;

    int current_msgid;
} ldap_connection_ctx_t;

enum OperationReturnCode connection_configure(struct ldap_global_context_t *global_ctx,
                                              struct ldap_connection_ctx_t *connection,
                                              struct ldap_connection_config_t *config);
enum OperationReturnCode connection_start_tls(struct ldap_connection_ctx_t *connection);
enum OperationReturnCode connection_sasl_bind(struct ldap_connection_ctx_t *connection);
enum OperationReturnCode connection_ldap_bind(struct ldap_connection_ctx_t *connection);

#endif //LIBDOMAIN_CONNECTION_H
