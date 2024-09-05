#include <cgreen/cgreen.h>

#include <talloc.h>
#include <schema.h>

#include "ldap_parsers_tests.h"

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

int main(int argc, char **argv)
{
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_suite(suite, attribute_type_test_suite());
    add_suite(suite, object_class_test_suite());
    return run_test_suite(suite, create_text_reporter());
}
