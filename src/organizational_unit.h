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

#ifndef LIB_DOMAIN_ORGANIZATIONAL_UNIT_H
#define LIB_DOMAIN_ORGANIZATIONAL_UNIT_H

#include "common.h"
#include "domain.h"

enum OperationReturnCode ld_add_ou(LDHandle *handle,
                                   const char *name,
                                   const char *city,
                                   const char *country,
                                   const char *description,
                                   const char *display_name,
                                   const char *parent,
                                   const char *postalCode,
                                   const char *state,
                                   const char *street_address);
enum OperationReturnCode ld_del_ou(LDHandle *handle, const char *name, const char *parent);
enum OperationReturnCode ld_mod_ou(LDHandle *handle, const char *name, const char *parent, LDAPAttribute_t **ou_attrs);
enum OperationReturnCode ld_rename_ou(LDHandle *handle, const char *old_name, const char *new_name, const char *parent);
#endif//LIB_DOMAIN_ORGANIZATIONAL_UNIT_H
