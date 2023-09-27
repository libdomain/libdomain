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

#include <ldap.h>
#include <stdbool.h>
#include <verto.h>

#include "common.h"

typedef struct ldap_sasl_options_t
{
    char *mechanism;

    const char *passwd;

    bool sasl_nocanon;
    short sasl_flags;
    char *sasl_secprops;
} ldap_sasl_options_t;

typedef struct ldap_sasl_defaults_t
{
    short flags;
    char *mechanism;

    char *realm;
    char *authcid;
    char *authzid;

    char *passwd;
} ldap_sasl_defaults_t;

typedef struct ldap_sasl_params_t
{
    char *dn;
    struct berval *passwd;
    LDAPControl **serverctrls;
    LDAPControl **clientctrls;
} ldap_sasl_params_t;

typedef struct ldap_connection_config_t
{
    const char *server;
    int port;

    int protocol_verion;

    bool chase_referrals;
    bool use_start_tls;
    bool use_sasl;

    int bind_type;

    struct ldap_sasl_options_t *sasl_options;
} ldap_connection_config_t;

struct ldap_connection_ctx_t;

typedef enum OperationReturnCode (*operation_callback_fn)(int, LDAPMessage *, struct ldap_connection_ctx_t *);

typedef struct ldhandle LDHandle;

enum BindType
{
    BIND_TYPE_INTERACTIVE = 1,
    BIND_TYPE_SIMPLE      = 2,
};

typedef struct ldap_connection_ctx_t
{
    LDHandle *handle;

    LDAP *ldap;

    struct ldap_connection_ctx_t *next;
    struct ldap_connection_ctx_t *prev;

    int fd;

    struct verto_ctx *base;

    struct verto_ev *read_event;
    struct verto_ev *write_event;

    operation_callback_fn on_read_operation;
    operation_callback_fn on_write_operation;
    operation_callback_fn on_error_operation;

    int bind_type;
    int directory_type;

    int current_msgid;
    const char *rmech;

    struct state_machine_ctx_t *state_machine;

    struct ldap_sasl_defaults_t *ldap_defaults;
    struct ldap_sasl_params_t *ldap_params;
} ldap_connection_ctx_t;

enum OperationReturnCode connection_configure(struct ldap_global_context_t *global_ctx,
                                              struct ldap_connection_ctx_t *connection,
                                              struct ldap_connection_config_t *config);
enum OperationReturnCode connection_start_tls(struct ldap_connection_ctx_t *connection);
enum OperationReturnCode connection_sasl_bind(struct ldap_connection_ctx_t *connection);
enum OperationReturnCode connection_ldap_bind(struct ldap_connection_ctx_t *connection);
enum OperationReturnCode connection_close(struct ldap_connection_ctx_t *connection);

// Operation handlers.
void connection_on_read(verto_ctx *ctx, verto_ev *ev);
void connection_on_write(verto_ctx *ctx, verto_ev *ev);

enum OperationReturnCode connection_bind_on_read(int, LDAPMessage *, struct ldap_connection_ctx_t *connection);

#endif //LIBDOMAIN_CONNECTION_H
