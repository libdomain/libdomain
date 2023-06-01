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

#ifndef LIB_DOMAIN_GROUP_H
#define LIB_DOMAIN_GROUP_H

#include "domain.h"

void ld_add_group(LDHandle* handle, const char* name, const char* base_dn);
void ld_del_group(LDHandle* handle, const char* name);
void ld_mod_group(LDHandle* handle, const char* comment);
void ld_rename_group(LDHandle* handle, const char* old_name, const char* new_name);

void ld_group_add_user(LDHandle* handle, const char* group_name, const char* user_name);
void ld_group_remove_user(LDHandle* handle, const char* group_name, const char* user_name);

#endif//LIB_DOMAIN_GROUP_H
