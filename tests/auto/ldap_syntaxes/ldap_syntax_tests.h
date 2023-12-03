#ifndef LDAP_SYNTAXES_TESTS_H
#define LDAP_SYNTAXES_TESTS_H

#include <cgreen/cgreen.h>

TestSuite*
boolean_test_suite();

TestSuite*
directory_string_test_suite();

TestSuite*
dn_test_suite();

TestSuite*
generalized_time_test_suite();

TestSuite*
ia5string_test_suite();

TestSuite*
integer_test_suite();

TestSuite*
large_integer_test_suite();

TestSuite*
numeric_string_test_suite();

TestSuite*
octet_string_test_suite();

TestSuite*
oid_test_suite();

TestSuite*
printable_string_test_suite();

TestSuite*
utc_time_test_suite();

#endif//LDAP_SYNTAXES_TESTS_H
