#ifndef SCHEMA_TESTS_H
#define SCHEMA_TESTS_H

#include <cgreen/cgreen.h>

TestSuite*
schema_new_test_suite();

TestSuite*
schema_attributetype_test_suite();

TestSuite*
schema_objectclass_test_suite();

TestSuite*
schema_active_directory_test_suite();

#endif//SCHEMA_TESTS_H
