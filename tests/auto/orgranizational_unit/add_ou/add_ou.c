#include <cgreen/cgreen.h>

#include <directory.h>
#include <domain.h>
#include <organizational_unit.h>
#include <talloc.h>

#include <connection_state_machine.h>

#include <test_common.h>

const int LDAP_DEBUG_ANY = -1;
const int BUFFER_SIZE = 80;

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

static char* OPENLDAP_OU_OBJECTCLASS[] = { "top", "organizationalUnit", NULL };
static char* OPENLDAP_OU_CN[] = { "test_ou_creation", NULL };
static char* OPENLDAP_OU_DESCRIPTION[] = { "description", NULL };

static LDAPAttribute_t OPENLDAP_OU_ATTRIBUTES[] =
{
    { .name = "objectClass", .values = OPENLDAP_OU_OBJECTCLASS },
    { .name = "cn", .values = OPENLDAP_OU_CN },
    { .name = "description", .values = OPENLDAP_OU_DESCRIPTION }
};
static const int OPENLDAP_OU_ATTRIBUTES_SIZE = number_of_elements(OPENLDAP_OU_ATTRIBUTES);

static char* AD_OU_OBJECTCLASS[] = { "top", "organizationalUnit", NULL };
static char* AD_OU_OU[] = { "TestOU", NULL };
static char* AD_OU_X121ADDRESS[] = { "123456", NULL };
static char* AD_OU_USERPASSWORD[] = { "{SSHA}encrypted_password", NULL };
static char* AD_OU_UPNSUFFIXES[] = { "example.com", NULL };
static char* AD_OU_CO[] = { "US", NULL };
static char* AD_OU_TELEXNUMBER[] = { "12345", NULL };
static char* AD_OU_TELETEXTERMINALIDENTIFIER[] = { "TTI123", NULL };
static char* AD_OU_TELEPHONENUMBER[] = { "+1 123-456-7890", NULL };
static char* AD_OU_STREET[] = { "123 Main St", NULL };
static char* AD_OU_ST[] = { "State", NULL };
static char* AD_OU_SEEALSO[] = { "cn=John Doe,ou=Users,dx=domain,dc=alt", NULL };
static char* AD_OU_SEARCHGUID[] = { "(objectClass=user)", NULL };
static char* AD_OU_REGISTEREDADDRESS[] = { "456 Business St", NULL };
static char* AD_OU_PREFERREDELIVERYMETHOD[] = { "email", NULL };
static char* AD_OU_POSTALCODE[] = { "12345", NULL };
static char* AD_OU_POSTALADDRESS[] = { "P.O. Box 789", NULL };
static char* AD_OU_POSTOFFICEBOX[] = { "789", NULL };
static char* AD_OU_PHYSICALDELIVERYOFFICENAME[] = { "Office123", NULL };
static char* AD_OU_MSCOMUSERPARTITIONSETLINK[] = { "cn=PartitionLink,ou=Partitions,dx=domain,dc=alt", NULL };
static char* AD_OU_MANAGEDBY[] = { "cn=Manager,ou=Users,dx=domain,dc=alt", NULL };
static char* AD_OU_THUMBNAILLOGO[] = { "/path/to/logo.jpg", NULL };
static char* AD_OU_L[] = { "City", NULL };
static char* AD_OU_INTERNATIONALISDNNUMBER[] = { "+1 987-654-3210", NULL };
static char* AD_OU_GPOPTIONS[] = { "1", NULL };
static char* AD_OU_GPLINK[] = { "[LDAP://cn={GUID},cn=policies,cn=system,dx=domain,dc=alt;1]", NULL };
static char* AD_OU_FACSIMILETELEPHONENUMBER[] = { "+1 987-654-3211", NULL };
static char* AD_OU_DESTINATIONINDICATOR[] = { "Indicator123", NULL };
static char* AD_OU_DESKTOPPROFILE[] = { "cn=Profile,ou=Profiles,dx=domain,dc=alt", NULL };
static char* AD_OU_DEFAULTGROUP[] = { "cn=Users,ou=Groups,dx=domain,dc=alt", NULL };
static char* AD_OU_COUNTRYCODE[] = { "1", NULL };
static char* AD_OU_C[] = { "USA", NULL };
static char* AD_OU_BUSINESSCATEGORY[] = { "Technology", NULL };

static LDAPAttribute_t AD_OU_ATTRIBUTES[] =
{
   { .name = "objectClass", .values = AD_OU_OBJECTCLASS },
   { .name = "ou", .values = AD_OU_OU },
   { .name = "x121Address", .values = AD_OU_X121ADDRESS },
   { .name = "userPassword", .values = AD_OU_USERPASSWORD },
   { .name = "uPNSuffixes", .values = AD_OU_UPNSUFFIXES },
   { .name = "co", .values = AD_OU_CO },
   { .name = "telexNumber", .values = AD_OU_TELEXNUMBER },
   { .name = "teletexTerminalIdentifier", .values = AD_OU_TELETEXTERMINALIDENTIFIER },
   { .name = "telephoneNumber", .values = AD_OU_TELEPHONENUMBER },
   { .name = "street", .values = AD_OU_STREET },
   { .name = "st", .values = AD_OU_ST },
   { .name = "seeAlso", .values = AD_OU_SEEALSO },
   { .name = "searchGuide", .values = AD_OU_SEARCHGUID },
   { .name = "registeredAddress", .values = AD_OU_REGISTEREDADDRESS },
   { .name = "preferredDeliveryMethod", .values = AD_OU_PREFERREDELIVERYMETHOD },
   { .name = "postalCode", .values = AD_OU_POSTALCODE },
   { .name = "postalAddress", .values = AD_OU_POSTALADDRESS },
   { .name = "postOfficeBox", .values = AD_OU_POSTOFFICEBOX },
   { .name = "physicalDeliveryOfficeName", .values = AD_OU_PHYSICALDELIVERYOFFICENAME },
   { .name = "msComUserPartitionSetLink", .values = AD_OU_MSCOMUSERPARTITIONSETLINK },
   { .name = "managedBy", .values = AD_OU_MANAGEDBY },
   { .name = "thumbnailLogo", .values = AD_OU_THUMBNAILLOGO },
   { .name = "l", .values = AD_OU_L },
   { .name = "internationalISDNumber", .values = AD_OU_INTERNATIONALISDNNUMBER },
   { .name = "gPOptions", .values = AD_OU_GPOPTIONS },
   { .name = "gPLink", .values = AD_OU_GPLINK },
   { .name = "facsimileTelephoneNumber", .values = AD_OU_FACSIMILETELEPHONENUMBER },
   { .name = "destinationIndicator", .values = AD_OU_DESTINATIONINDICATOR },
   { .name = "desktopProfile", .values = AD_OU_DESKTOPPROFILE },
   { .name = "defaultGroup", .values = AD_OU_DEFAULTGROUP },
   { .name = "countryCode", .values = AD_OU_COUNTRYCODE },
   { .name = "c", .values = AD_OU_C },
   { .name = "businessCategory", .values = AD_OU_BUSINESSCATEGORY }
};

const static int AD_OU_ATTRIBUTES_SIZE = number_of_elements(AD_OU_ATTRIBUTES);

typedef struct testcase_s
{
    char* name;
    char* entry_cn;
    char* parent_dn;
    int number_of_attributes;
    int desired_test_result;
    LDAPAttribute_t* attributes;
} testcase_t;

typedef struct current_testcases_s
{
    int number_of_testcases;
    testcase_t* testcases;
} current_testcases_t;

static testcase_t OPENLDAP_TESTCASES[] =
{
    {
        "Addition of OpenLDAP OU testcase",
        "test_ou_addition",
        "dc=domain,dc=alt",
        OPENLDAP_OU_ATTRIBUTES_SIZE,
        RETURN_CODE_SUCCESS,
        OPENLDAP_OU_ATTRIBUTES
    }
};

static const int NUMBER_OF_OPENLDAP_TESTCASES = number_of_elements(OPENLDAP_TESTCASES);

static testcase_t AD_TESTCASES[] =
{
    {
        "Addition of AD OU testcase",
        "TestOU",
        "dc=domain,dc=alt",
        AD_OU_ATTRIBUTES_SIZE,
        RETURN_CODE_SUCCESS,
        AD_OU_ATTRIBUTES
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

            enum OperationReturnCode rc = ld_add_ou(connection->handle, testcase.entry_cn, &testcase.attributes, testcase.parent_dn);
            assert_that(rc,is_equal_to(testcase.desired_test_result));
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

Ensure(Cgreen, ou_add_test)
{
    start_test(connection_on_timeout, CONNECTION_UPDATE_INTERVAL, &current_directory_type);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, ou_add_test);
    return run_test_suite(suite, create_text_reporter());
}
