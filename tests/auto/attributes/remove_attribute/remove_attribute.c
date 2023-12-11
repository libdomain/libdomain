#include <cgreen/cgreen.h>

#include <directory.h>
#include <domain.h>
#include <attribute.h>
#include <talloc.h>

#include <connection_state_machine.h>

#include <test_common.h>

const int LDAP_DEBUG_ANY = -1;
const int BUFFER_SIZE = 80;

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

const int CONNECTION_UPDATE_INTERVAL = 1000;

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

typedef struct testcase_s
{
    char* name;
    char* entry_dn;
    char* entry_attr;
    char* entry_value;
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
        "Remove attribute OpenLDAP",
        "cn=test_mod_computer,ou=equipment",
        "seeAlso",
        "dc=domain,dc=alt",
        RETURN_CODE_SUCCESS,
    }
};

static const int NUMBER_OF_OPENLDAP_TESTCASES = number_of_elements(OPENLDAP_TESTCASES);

static testcase_t AD_TESTCASES[] =
{
    {
        "Remove attribute AD",
        "cn=test_mod_group,cn=users",
        "adminCount",
        "1",
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


LDAPAttribute_t** attrs;

static LDAPAttribute_t** fill_attributes_to_remove(TALLOC_CTX* ctx, char* name, char* value)
{
    attrs = talloc_array(ctx, LDAPAttribute_t*, 2);
    attrs[0] = talloc(ctx, LDAPAttribute_t);
    attrs[0]->values = talloc_array(ctx, char*, 2);
    attrs[0]->name = talloc_strdup(ctx, name);
    attrs[0]->values[0] = talloc_strdup(ctx, value);
    attrs[0]->values[1] = NULL;
    attrs[1] = NULL;

    return attrs;
}

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

            TALLOC_CTX* talloc_ctx = talloc_new(NULL);

            enum OperationReturnCode rc = ld_del_attributes(connection->handle,
                                                            testcase.entry_dn,
                                                            fill_attributes_to_remove(talloc_ctx,
                                                                                      testcase.entry_attr,
                                                                                      testcase.entry_value));
            assert_that(rc, is_equal_to(testcase.desired_test_result));
            test_status(testcase);

            talloc_free(talloc_ctx);
        }

        ld_install_handler(connection->handle, connection_on_add_message, CONNECTION_UPDATE_INTERVAL);
    }

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_ERROR)
    {
        verto_break(ctx);

        fail_test("Error encountered during bind\n");
    }
}

Ensure(Cgreen, attribute_remove_test)
{
    start_test(connection_on_timeout, CONNECTION_UPDATE_INTERVAL, &current_directory_type, false);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, attribute_remove_test);
    return run_test_suite(suite, create_text_reporter());
}
