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

#include "group.h"
#include "common.h"
#include "entry.h"

#include <string.h>

// TODO:
// - Base dn in handle
// - Private header for domain.c
// - List of attributes for group.

void ld_add_group(LDHandle *handle, const char *name, const char *base_dn)
{
    const char* group_name = NULL;
    const char* group_dn = NULL;

    if (!handle)
    {
        error("Handle is null - ld_add_group");
        return;
    }

    if (!name || strlen(name) == 0)
    {
        error("No group name - ld_add_group");
        return;
    }
    else
    {
        group_name = name;
    }

    if (!base_dn || strlen(base_dn) == 0)
    {
        group_dn = "";
    }
    else
    {
        group_dn = base_dn;
    }

    add(handle->connection_ctx, group_name, NULL);
}
