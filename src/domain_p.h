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

#ifndef LIB_DOMAIN_PRIVATE_H
#define LIB_DOMAIN_PRIVATE_H

#include <stdbool.h>
#include <talloc.h>

typedef struct config_s
{
    char *host;
    int protocol_version;

    char *base_dn;
    char *username;
    char *password;

    bool simple_bind;
    bool use_tls;
    bool use_sasl;
    bool use_anon;

    int timeout;

    char *cacertfile;
    char *certfile;
    char *keyfile;
} config_t;

typedef struct ldhandle
{
    TALLOC_CTX *talloc_ctx;
    struct ldap_global_context_t *global_ctx;
    struct ldap_connection_ctx_t *connection_ctx;
    struct ldap_connection_config_t *config_ctx;
    config_t *global_config;
} LDHandle;

#define check_handle(handle, function_name) \
    if (!handle) \
    { \
        error("Handle is null - %s", function_name); \
        return RETURN_CODE_FAILURE; \
    }

#define check_string(input, output, function_name) \
    if (!input || strlen(input) == 0) \
    { \
        error("No group name - %s", function_name); \
        return RETURN_CODE_FAILURE; \
    } \
    else \
    { \
        output = input; \
    }

#define check_and_assign_attribute(attrs, value, index, talloc_ctx) \
    if (value && strlen(value) > 0) \
    { \
        attrs[index]->values    = talloc_array(talloc_ctx, char *, 2); \
        attrs[index]->values[0] = talloc_strndup(talloc_ctx, value, strlen(value)); \
        attrs[index]->values[1] = NULL; \
    }

#define MAX_ATTRIBUTES 5

typedef struct attribute_value_pair_s
{
    char *name;
    char *value[MAX_ATTRIBUTES];
} attribute_value_pair_t;

#define number_of_elements(x) (sizeof(x) / sizeof((x)[0]))

enum OperationReturnCode ld_add_entry(
    LDHandle *handle, const char *name, const char *parent, const char *prefix, LDAPAttribute_t **entry_attrs);
enum OperationReturnCode ld_del_entry(LDHandle *handle, const char *name, const char *parent, const char *prefix);
enum OperationReturnCode ld_mod_entry(
    LDHandle *handle, const char *name, const char *parent, const char *prefix, LDAPAttribute_t **entry_attrs);
enum OperationReturnCode ld_rename_entry(
    LDHandle *handle, const char *old_name, const char *new_name, const char *parent, const char *prefix);

typedef struct LDAPAttribute_s LDAPAttribute_t;

LDAPAttribute_t **assign_default_attribute_values(TALLOC_CTX *talloc_ctx,
                                                  attribute_value_pair_t default_attrs[],
                                                  int size);

#endif //LIB_DOMAIN_PRIVATE_H
