#include <cgreen/cgreen.h>

#include "entry_utils.h"

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_suite(suite, entry_new_test_suite());
    add_suite(suite, entry_add_attribute_test_suite());
    add_suite(suite, entry_get_attribute_suite());
    add_suite(suite, entry_get_attributes_test_suite());
    return run_test_suite(suite, create_text_reporter());
}
