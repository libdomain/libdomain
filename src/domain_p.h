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

typedef struct ld_config_s
{
    char *host;                            //!< LDAP server to connect to.
    int protocol_version;                  //!< LDAP protocol version we require from server.

    char *base_dn;                         //!< Base dn to bind with.
    char *username;                        //!< Name of the user we connect as. Can be NULL.
    char *password;                        //!< Password for the said user. Can be NULL.

    bool simple_bind;                      //!< We can use two type of bind simple and interactive. Use true for simple and false otherwise.
    bool use_tls;                          //!< If we need to establish a TLS or SSL connection.
    bool use_sasl;                         //!< If we use SASL protocol.
    bool use_anon;                         //!< If we are going to perform "anonymous bind".

    int timeout;                           //!< Operation timeout. Once we reach specified limit current operation fails.

    char *cacertfile;                      //!< Defines the complete path to a CA certificate, which is utilized for validating the server's presented certificate.
    char *certfile;                        //!< Client certificate file path.
    char *keyfile;                         //!< Private key file associated with client certificate.
} ld_config_t;

typedef struct ldhandle
{
    TALLOC_CTX *talloc_ctx;                            //!< Talloc context we use during the allocation when working with the library.
    struct ldap_global_context_t *global_ctx;          //!< Global context of the library.
    struct ldap_connection_ctx_t *connection_ctx;      //!< Connection context.
    struct ldap_connection_config_t *config_ctx;       //!< Connection configuration.
    ld_config_t *global_config;                        //!< Global configuration of the library.
} LDHandle;

#define check_handle(handle, function_name) \
    if (!handle) \
    { \
        error("Handle is null - %s \n", function_name); \
        return RETURN_CODE_FAILURE; \
    }

#define check_string(input, output, function_name) \
    if (!input || strlen(input) == 0) \
    { \
        error("Empty string in function - %s \n", function_name); \
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

enum OperationReturnCode ld_add_entry(
    LDHandle *handle, const char *name, const char *parent, const char *prefix, LDAPAttribute_t **entry_attrs);
enum OperationReturnCode ld_del_entry(LDHandle *handle, const char *name, const char *parent, const char *prefix);
enum OperationReturnCode ld_mod_entry(
    LDHandle *handle, const char *name, const char *parent, const char *prefix, LDAPAttribute_t **entry_attrs);
enum OperationReturnCode ld_rename_entry(
    LDHandle *handle, const char *old_name, const char *new_name, const char *parent, const char *prefix);
enum OperationReturnCode ld_mod_entry_attrs(
        LDHandle *handle, const char *name, const char *parent, const char *prefix, LDAPAttribute_t **entry_attrs,
        int opcode);

typedef struct LDAPAttribute_s LDAPAttribute_t;

#endif //LIB_DOMAIN_PRIVATE_H
