#include <cgreen/cgreen.h>

#include <talloc.h>
#include <schema.h>

#include "schema_tests.h"

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_suite(suite, schema_new_test_suite());
    add_suite(suite, schema_attributetype_test_suite());
    add_suite(suite, schema_objectclass_test_suite());
    return run_test_suite(suite, create_text_reporter());
}
