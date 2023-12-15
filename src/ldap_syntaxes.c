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

/*!
 * \brief validate_boolean Validates array if indeed boolean value.
 *
 * The function validate_boolean takes in a string and checks if it
 * matches the format of a boolean value as defined by RFC 4517.
 * The function returns True if the input string is either "TRUE" or "FALSE",
 * otherwise it returns False.
 * From RFC 4517 :
 *   Boolean = "TRUE" / "FALSE"
 *
 * \param[in] value            NULL terminated array of char to verify.
 * \return
 *        - false - on error.
 *        - true - on success.
 * \sa [RFC4517](https://www.rfc-editor.org/rfc/rfc4517.txt)
 */
bool validate_boolean(const char *value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_boolean(value, strlen(value));
}

/*!
 * \brief validate_integer Validates array if indeed integer value.
 *
 * The function validate_integer takes in a string and checks if it
 * matches the format of a integer value as defined by RFC 4517.
 * The function returns True if the input string is integer value,
 * otherwise it returns False.
 * From RFC 4517 :
 *   Integer = ( HYPHEN LDIGIT *DIGIT ) | number
 * From RFC 4512 :
 *   number  = DIGIT | ( LDIGIT 1*DIGIT )
 *   DIGIT   = %x30 | LDIGIT       ; "0"-"9"
 *   LDIGIT  = %x31-39             ; "1"-"9"
 *   HYPHEN  = %x2D                ; hyphen ("-")
 *
 * \param[in] value            NULL terminated array of char to verify.
 * \return
 *        - false - on error.
 *        - true - on success.
 * \sa [RFC4517](https://www.rfc-editor.org/rfc/rfc4517.txt)
 */
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

/*!
 * \brief validate_octet_string Validates array if indeed integer value.
 *
 * The function validate_octet_string takes in a string and checks if it
 * matches the format of a octet string value as defined by RFC 4517.
 * The function returns True if the input string is octet string value,
 * otherwise it returns False.
 * From RFC 4517 :
 *   OctetString = *OCTET
 *   OCTET       = %x00-FF ; Any octet (8-bit data unit)
 *
 * \return
 *        - false - on error.
 *        - true - on success.
 * \sa [RFC4517](https://www.rfc-editor.org/rfc/rfc4517.txt)
 */
bool validate_octet_string(const char *value)
{
    if (!value || strlen(value) == 0)
    {
        return false;
    }

    return is_octet_string(value, strlen(value));
}

/*!
 * \brief validate_oid Validates array if indeed integer value.
 *
 * The function validate_oid takes in a string and checks if it
 * matches the format of a OID value as defined by RFC 4517.
 * The function returns True if the input string is OID value,
 * otherwise it returns False.
 * From RFC 4517 :
 *   OctetString = *OCTET
 *   OCTET       = %x00-FF ; Any octet (8-bit data unit)
 *
 * \return
 *        - false - on error.
 *        - true - on success.
 * \sa [RFC4517](https://www.rfc-editor.org/rfc/rfc4517.txt)
 */
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

/*!
 * \brief validate_large_integer Validates array if indeed int64 value.
 *
 * The function validate_large_integer takes in a string and checks if it
 * matches the format of a large integer value as defined by RFC 4517.
 * The function returns True if the input string is large integer value,
 * otherwise it returns False.
 * From RFC 4517 :
 *   Integer = ( HYPHEN LDIGIT *DIGIT ) | number
 * From RFC 4512 :
 *   number  = DIGIT | ( LDIGIT 1*DIGIT )
 *   DIGIT   = %x30 | LDIGIT       ; "0"-"9"
 *   LDIGIT  = %x31-39             ; "1"-"9"
 *   HYPHEN  = %x2D                ; hyphen ("-")
 *
 * \param[in] value            NULL terminated array of char to verify.
 * \return
 *        - false - on error.
 *        - true - on success.
 * \sa [RFC4517](https://www.rfc-editor.org/rfc/rfc4517.txt)
 */
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
