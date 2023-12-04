#include <cgreen/cgreen.h>

#include <talloc.h>
#include <schema.h>

#include "ldap_syntax_tests.h"

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_suite(suite, boolean_test_suite());
    add_suite(suite, directory_string_test_suite());
    add_suite(suite, dn_test_suite());
    add_suite(suite, generalized_time_test_suite());
    add_suite(suite, ia5string_test_suite());
    add_suite(suite, integer_test_suite());
    add_suite(suite, large_integer_test_suite());
    add_suite(suite, numeric_string_test_suite());
    add_suite(suite, octet_string_test_suite());
    add_suite(suite, oid_test_suite());
    add_suite(suite, printable_string_test_suite());
    add_suite(suite, utc_time_test_suite());
    return run_test_suite(suite, create_text_reporter());
}
