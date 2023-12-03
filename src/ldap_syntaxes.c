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

#include "ldap_syntaxes.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "syntaxes/syntaxes.h"

bool validate_boolean(const char *value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_boolean(value, strlen(value));
}

bool validate_integer(const char *value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    char buffer[sizeof("-2147483648")] = {0};
    char* end = NULL;

    if (strlen(value) >= sizeof(buffer))
    {
        return false;
    }

    if (is_integer(value, strlen(value)))
    {
        memcpy(buffer, value, strlen(value));

        errno = 0;
        long ivalue = strtol(buffer, &end, 10);

        if (errno != 0)
        {
            return false;
        }

        if ((ivalue < -2147483648) || (ivalue > 2147483647))
        {
            return false;
        }

        return true;
    }

    return false;
}

bool validate_octet_string(const char *value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_octet_string(value, strlen(value));
}

bool validate_oid(const char *value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_oid(value, strlen(value));
}

bool validate_numeric_string(const char *value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_numeric_string(value, strlen(value));
}

bool validate_printable_string(const char *value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_printable_string(value, strlen(value));
}

bool validate_case_ignore_string(const char *value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_directory_string(value, strlen(value));
}

bool validate_ia5_string(const char *value)
{
    if (!value)
    {
        return false;
    }

    return is_ia5string(value, strlen(value));
}

bool validate_utc_time(const char *value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_utc_time(value, strlen(value));
}

bool validate_generalized_time(const char *value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_generalized_time(value, strlen(value));
}

bool validate_case_sensitive_string(const char *value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_directory_string(value, strlen(value));
}

bool validate_directory_string(const char* value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_directory_string(value, strlen(value));
}

bool validate_large_integer(const char* value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    char buffer[sizeof("-9223372036854775808")] = {0};
    char* end = NULL;

    if (strlen(value) >= sizeof(buffer))
    {
        return false;
    }

    if (is_integer(value, strlen(value)))
    {
        memcpy(buffer, value, strlen(value));

        errno = 0;
        strtoll(buffer, &end, 10);

        if (errno != 0)
        {
            return false;
        }

        return true;
    }

    return false;
}

bool validate_object_security_descriptor(const char* value)
{
    (void)(value);
    return false;
}

bool validate_dn(const char* value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_dn(value, strlen(value));
}

bool validate_dn_with_octet_string(const char* value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_dn(value, strlen(value));
}

bool validate_dn_with_string(const char* value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_dn(value, strlen(value));
}

bool validate_or_name(const char* value)
{
    (void)(value);
    return false;
}

bool validate_presentation_address(const char* value)
{
    (void)(value);
    return false;
}

bool validate_access_point(const char* value)
{
    (void)(value);
    return false;
}
