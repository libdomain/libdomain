#include <cgreen/cgreen.h>

#include <domain.h>
#include <computer.h>
#include <talloc.h>

#include <connection_state_machine.h>

#include <test_common.h>

const int LDAP_DEBUG_ANY = -1;
const int BUFFER_SIZE = 80;

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

xEnsure(Cgreen, computer_delete_test)
{
    fail_test("Computer deletion was not successfull\n");
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, computer_delete_test);
    return run_test_suite(suite, create_text_reporter());
}
