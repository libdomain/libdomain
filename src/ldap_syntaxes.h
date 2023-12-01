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

#ifndef LIB_DOMAIN_LDAP_SYNTAXES_H
#define LIB_DOMAIN_LDAP_SYNTAXES_H

#include <stdbool.h>

bool validate_boolean(const char* value);
bool validate_integer(const char* value);
bool validate_octet_string(const char* value);
bool validate_oid(const char* value);
bool validate_numeric_string(const char* value);
bool validate_printable_string(const char* value);
bool validate_case_ignore_string(const char* value);
bool validate_ia5_string(const char* value);
bool validate_utc_time(const char* value);
bool validate_generalized_time(const char* value);
bool validate_case_sensitive_string(const char* value);
bool validate_directory_string(const char* value);
bool validate_large_integer(const char* value);
bool validate_object_security_descriptor(const char* value);
bool validate_dn(const char* value);
bool validate_dn_with_octet_string(const char* value);
bool validate_or_name(const char* value);
bool validate_presentation_address(const char* value);
bool validate_access_point(const char* value);
bool validate_dn_with_string(const char* value);

#endif//LIB_DOMAIN_LDAP_SYNTAXES_H
