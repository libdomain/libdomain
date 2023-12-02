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
    add_suite(suite, ia5string_test_suite());
    return run_test_suite(suite, create_text_reporter());
}
