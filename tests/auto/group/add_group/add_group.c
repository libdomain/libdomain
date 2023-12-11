#include <cgreen/cgreen.h>

#include <directory.h>
#include <domain.h>
#include <group.h>
#include <talloc.h>

#include <connection_state_machine.h>

#include <test_common.h>

const int LDAP_DEBUG_ANY = -1;
const int BUFFER_SIZE = 80;

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

typedef struct testcase_s
{
    char* name;
    char* entry_parent;
    char* entry_cn;
    int number_of_attributes;
    int desired_test_result;
    LDAPAttribute_t* attributes;
} testcase_t;

typedef struct current_testcases_s
{
    int number_of_testcases;
    testcase_t* testcases;
} current_testcases_t;

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

static char* OPENLDAP_GROUP_OBJECTCLASS[] = { "top", "posixGroup", NULL };
static char* OPENLDAP_GROUP_CN[] = { "test_group", NULL };
static char* OPENLDAP_GROUP_DESCRIPTION[] = { "description", NULL };
static char* OPENLDAP_GROUP_GID_NUMBER[] = { "100", NULL };

static LDAPAttribute_t OPENLDAP_GROUP_ATTRIBUTES[] =
{
    { .name = "objectClass", .values = OPENLDAP_GROUP_OBJECTCLASS },
    { .name = "cn", .values = OPENLDAP_GROUP_CN },
    { .name = "description", .values = OPENLDAP_GROUP_DESCRIPTION },
    { .name = "gidNumber", .values = OPENLDAP_GROUP_GID_NUMBER }
};
static const int OPENLDAP_GROUP_ATTRIBUTES_SIZE = number_of_elements(OPENLDAP_GROUP_ATTRIBUTES);

static char* AD_GROUP_OBJECTCLASS[] = { "top", "group", NULL };
static char* AD_GROUP_CN[] = { "test_group", NULL };
static char* AD_GROUP_INSTANCE_TYPE[] = { "4", NULL };
static char* AD_GROUP_MSSFU30NAME[] = { "test_group", NULL };
static char* AD_GROUP_MSDS_AZAPPLICATIONDATA[] = { "AppData", NULL };
static char* AD_GROUP_MSDS_AZLASTIMPORTEDBIZRULEPATH[] = { "/path/to/biz/rule", NULL };
static char* AD_GROUP_MSDS_AZBIZRULELANGUAGE[] = { "PowerShell", NULL };
static char* AD_GROUP_MSDS_AZBIZRULE[] = { "<biz_rule_script>", NULL };
static char* AD_GROUP_MSDS_AZGENERICDATA[] = { "GenericDataValue", NULL };
static char* AD_GROUP_MANAGEDBY[] = { "CN=Administrator,CN=Users,dc=domain,dc=alt", NULL };
static char* AD_GROUP_MAIL[] = { "testgroup@example.com", NULL };
static char* AD_GROUP_DESKTOPPROFILE[] = { "\\\\server\\profiles\\TestGroup", NULL };
static char* AD_GROUP_ADMINCOUNT[] = { "1", NULL };
static char* AD_GROUP_SAM_ACCOUNT_NAME[] = { "test_group", NULL };

static LDAPAttribute_t AD_GROUP_ATTRIBUTES[] =
{
    { .name = "objectClass", .values = AD_GROUP_OBJECTCLASS },
    { .name = "cn", .values = AD_GROUP_CN },
    { .name = "msSFU30Name", .values = AD_GROUP_MSSFU30NAME },
    { .name = "instanceType", .values = AD_GROUP_INSTANCE_TYPE },
    { .name = "msDS-AzApplicationData", .values = AD_GROUP_MSDS_AZAPPLICATIONDATA },
    { .name = "msDS-AzLastImportedBizRulePath", .values = AD_GROUP_MSDS_AZLASTIMPORTEDBIZRULEPATH },
    { .name = "msDS-AzBizRuleLanguage", .values = AD_GROUP_MSDS_AZBIZRULELANGUAGE },
    { .name = "msDS-AzBizRule", .values = AD_GROUP_MSDS_AZBIZRULE },
    { .name = "msDS-AzGenericData", .values = AD_GROUP_MSDS_AZGENERICDATA },
    { .name = "managedBy", .values = AD_GROUP_MANAGEDBY },
    { .name = "mail", .values = AD_GROUP_MAIL },
    { .name = "desktopProfile", .values = AD_GROUP_DESKTOPPROFILE },
    { .name = "adminCount", .values = AD_GROUP_ADMINCOUNT },
    { .name = "sAMAccountName", .values = AD_GROUP_SAM_ACCOUNT_NAME }
};

static const int AD_GROUP_ATTRIBUTES_SIZE = number_of_elements(AD_GROUP_ATTRIBUTES);

static testcase_t OPENLDAP_TESTCASES[] =
{
    {
        "Addition of valid OpenLDAP group testcase",
        "dc=domain,dc=alt",
        "test_group",
        OPENLDAP_GROUP_ATTRIBUTES_SIZE,
        RETURN_CODE_SUCCESS,
        OPENLDAP_GROUP_ATTRIBUTES
    }
};

static const int NUMBER_OF_OPENLDAP_TESTCASES = number_of_elements(OPENLDAP_TESTCASES);

static testcase_t AD_TESTCASES[] =
{
    {
        "Addition of valid AD group testcase",
        "cn=users,dc=domain,dc=alt",
        "test_group",
        AD_GROUP_ATTRIBUTES_SIZE,
        RETURN_CODE_SUCCESS,
        AD_GROUP_ATTRIBUTES
    }
};

static const int NUMBER_OF_AD_TESTCASES = number_of_elements(AD_TESTCASES);

const int CONNECTION_UPDATE_INTERVAL = 1000;

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

            int rc = ld_add_group(connection->handle,
                                  testcase.entry_cn,
                                  fill_user_attributes(talloc_ctx, testcase.attributes, testcase.number_of_attributes),
                                  testcase.entry_parent);
            assert_that(rc,is_equal_to(testcase.desired_test_result));
            test_status(testcase);

            talloc_free(talloc_ctx);

            if (rc != RETURN_CODE_SUCCESS)
            {
                verto_break(ctx);

                fail_test("Error encountered ld_add_group bind\n");
                exit(EXIT_FAILURE);
            }
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
