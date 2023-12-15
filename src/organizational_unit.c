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

#include "organizational_unit.h"
#include "common.h"
#include "domain_p.h"
#include "entry.h"

enum OUAttributeIndex
{
    OBJECT_CLASS = 0,
    OU           = 1,
    DESCRIPTION  = 2,
};

/**
 * @brief ld_add_ou       Creates the OU.
 * @param[in] handle          Pointer to libdomain session handle.
 * @param[in] name            Name of the OU.
 * @param[in] ou_attrs        Attributes of the OU.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_add_ou(LDHandle *handle,
                                   const char *name,
                                   LDAPAttribute_t** ou_attrs,
                                   const char *parent)
{
    const char *dn = handle ? handle->global_config->base_dn : NULL;
    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    if (parent && strlen(parent) > 0)
    {
        dn = parent;
    }

    rc = ld_add_entry(handle, name, dn, "ou", ou_attrs);

    return rc;
}

/**
 * @brief ld_del_ou   Deletes the OU.
 * @param[in] handle      Pointer to libdomain session handle.
 * @param[in] name        Name of the OU.
 * @param[in] parent      Parent container that holds the OU.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_del_ou(LDHandle *handle, const char *name, const char *parent)
{
    return ld_del_entry(handle, name, parent ? parent : handle ? handle->global_config->base_dn : NULL, "ou");
}

/**
 * @brief ld_mod_ou    Modifies the OU.
 * @param[in] handle       Pointer to libdomain session handle.
 * @param[in] name         Name of the OU.
 * @param[in] parent       Parent container that holds the OU.
 * @param[in] ou_attrs     List of the attributes to modify.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_mod_ou(LDHandle *handle, const char *name, const char *parent, LDAPAttribute_t **ou_attrs)
{
    return ld_mod_entry(handle, name, parent ? parent : handle ? handle->global_config->base_dn : NULL, "ou", ou_attrs);
}

/**
 * @brief ld_rename_ou Renames the OU.
 * @param[in] handle       Pointer to libdomain session handle.
 * @param[in] old_name     Old name of the OU.
 * @param[in] new_name     New name of the OU.
 * @param[in] parent       Parent container that holds the OU.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_rename_ou(LDHandle *handle, const char *old_name, const char *new_name, const char *parent)
{
    return ld_rename_entry(handle, old_name, new_name, parent ? parent : handle ? handle->global_config->base_dn : NULL, "ou");
}
