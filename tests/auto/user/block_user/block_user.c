#include <cgreen/cgreen.h>

#include <domain.h>
#include <talloc.h>

#include <test_common.h>

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

xEnsure(Cgreen, block_user_test)
{
    fail_test("Block user test failed!");
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, block_user_test);
    return run_test_suite(suite, create_text_reporter());
}
