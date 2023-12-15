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
#ifndef LIBDOMAIN_ENTRY_PRIVATE_H
#define LIBDOMAIN_ENTRY_PRIVATE_H

#include <glib-2.0/glib.h>

/*!
 * @brief ld_entry_t - Structure holds LDAP entry information.
 */
typedef struct ld_entry_s
{
    char* dn;                            //!< Distinguished name of the LDAP entry.
    GHashTable *attributes;              //!< Hash table with entry's attributes.
} ld_entry_t;

void connection_remove_search_request(struct ldap_connection_ctx_t *connection, int index);

#endif //LIBDOMAIN_ENTRY_PRIVATE_H
