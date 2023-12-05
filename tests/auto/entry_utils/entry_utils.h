#ifndef ENTRY_UTILS_H
#define ENTRY_UTILS_H

#include <cgreen/cgreen.h>

TestSuite*
entry_new_test_suite();

TestSuite*
entry_add_attribute_test_suite();

TestSuite*
entry_get_attribute_suite();

TestSuite*
entry_get_attributes_test_suite();

#endif//ENTRY_UTILS_H
