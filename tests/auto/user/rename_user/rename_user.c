#include <cgreen/cgreen.h>

#include <directory.h>
#include <domain.h>
#include <user.h>
#include <talloc.h>

#include <connection_state_machine.h>

#include <test_common.h>

const int LDAP_DEBUG_ANY = -1;
const int BUFFER_SIZE = 80;

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

typedef struct testcase_s
{
    char* name;
    char* old_entry_cn;
    char* new_entry_cn;
    char* entry_parent;
    int desired_test_result;
} testcase_t;

typedef struct current_testcases_s
{
    int number_of_testcases;
    testcase_t* testcases;
} current_testcases_t;

static testcase_t OPENLDAP_TESTCASES[] =
{
    {
        "Rename existing user in OpenLDAP",
        "test_rename_user",
        "user2",
        "ou=users,dc=domain,dc=alt",
        RETURN_CODE_SUCCESS
    }
};

static const int NUMBER_OF_OPENLDAP_TESTCASES = number_of_elements(OPENLDAP_TESTCASES);

static testcase_t AD_TESTCASES[] =
{
    {
        "Rename existing user in AD",
        "test rename",
        "user2",
        "cn=users,dc=domain,dc=alt",
        RETURN_CODE_SUCCESS
    }
};

static const int NUMBER_OF_AD_TESTCASES = number_of_elements(AD_TESTCASES);

static int current_directory_type = LDAP_TYPE_UNKNOWN;

static current_testcases_t get_current_testcases(int directory_type)
{
    current_testcases_t result = { .testcases = NULL, .number_of_testcases = 0 };

    switch (directory_type)
    {
    case LDAP_TYPE_ACTIVE_DIRECTORY:
        result.testcases = AD_TESTCASES;
        result.number_of_testcases = NUMBER_OF_AD_TESTCASES;
        break;
    case LDAP_TYPE_OPENLDAP:
        result.testcases = OPENLDAP_TESTCASES;
        result.number_of_testcases = NUMBER_OF_OPENLDAP_TESTCASES;
    default:
        break;
    }

    return result;
}

const int CONNECTION_UPDATE_INTERVAL = 1000;

static void connection_on_add_message(verto_ctx *ctx, verto_ev *ev)
{
    (void)(ev);

    static int callcount = 0;

    if (++callcount > 10)
    {
        verto_break(ctx);
    }
}

static void connection_on_timeout(verto_ctx *ctx, verto_ev *ev)
{
    (void)(ctx);

    struct ldap_connection_ctx_t* connection = verto_get_private(ev);

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_RUN)
    {
        verto_del(ev);

        current_testcases_t current_testcases = get_current_testcases(current_directory_type);
        for (int test_index = 0; test_index < current_testcases.number_of_testcases; test_index++)
        {
            testcase_t testcase = current_testcases.testcases[test_index];

            enum OperationReturnCode rc = ld_rename_user(connection->handle,
                                                         testcase.old_entry_cn,
                                                         testcase.new_entry_cn,
                                                         testcase.entry_parent);
            assert_that(rc, is_equal_to(testcase.desired_test_result));

            test_status(testcase);
        }

        ld_install_handler(connection->handle, connection_on_add_message, CONNECTION_UPDATE_INTERVAL);
    }

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_ERROR)
    {
        verto_break(ctx);

        fail_test("Error encountered during bind\n");
    }
}

Ensure(Cgreen, group_add_test)
{
    start_test(connection_on_timeout, CONNECTION_UPDATE_INTERVAL, &current_directory_type, false);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, group_add_test);
    return run_test_suite(suite, create_text_reporter());
}
