#include <cgreen/cgreen.h>

#include <domain.h>
#include <talloc.h>

#include <test_common.h>

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

xEnsure(Cgreen, remove_attributes_test)
{
    fail_test("Remove attributes test failed!");
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, remove_attributes_test);
    return run_test_suite(suite, create_text_reporter());
}
