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

#ifndef LIB_DOMAIN_SYNTAXES_H
#define LIB_DOMAIN_SYNTAXES_H

#include <stdbool.h>
#include <stdlib.h>

bool is_boolean(const char *const in, const size_t len);
bool is_directory_string(const char *const in, const size_t len);
bool is_dn(const char *const in, const size_t len);
bool is_generalized_time(const char *const in, const size_t len);
bool is_ia5string(const char *const in, const size_t len);
bool is_integer(const char *const in, const size_t len);
bool is_numeric_string(const char *const in, const size_t len);
bool is_octet_string(const char *const in, const size_t len);
bool is_oid(const char *const in, const size_t len);
bool is_printable_string(const char *const in, const size_t len);
bool is_utc_time(const char *const in, const size_t len);

#endif//LIB_DOMAIN_SYNTAXES_H
