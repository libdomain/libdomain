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

#ifndef LIB_DOMAIN_H
#define LIB_DOMAIN_H

#include <stdbool.h>
#include <verto.h>

/**
 * @brief LDHandle Structure that represents an libdomain session handle.
 */
typedef struct ldhandle LDHandle;

/**
 * @brief config_t
 */
typedef struct config_s config_t;

typedef struct LDAPAttribute_s
{
    char *name;
    char **values;
} LDAPAttribute_t;

typedef enum OperationReturnCode (*error_callback_fn)(int, void *, void *);

config_t *ld_create_config(char *host,
                           int port,
                           int protocol_version,
                           char *base_dn,
                           char *username,
                           char *password,
                           bool simple_bind,
                           bool use_tls,
                           bool use_sasl,
                           bool use_anon,
                           int timeout,
                           char *cacertfile,
                           char *certfile,
                           char *keyfile);

void ld_init(LDHandle **handle, const config_t *config);
void ld_install_default_handlers(LDHandle *handle);
void ld_install_handler(LDHandle *handle, verto_callback *callback, time_t interval);
void ld_install_error_handler(LDHandle *handle, error_callback_fn callback);
void ld_exec(LDHandle *handle);
void ld_exec_once(LDHandle *handle);
void ld_free(LDHandle *handle);

#endif //LIB_DOMAIN_H
