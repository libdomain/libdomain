#include <cgreen/cgreen.h>

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

        ld_add_ou(connection->handle, "test_ou_creation", "description", "ou=users,dc=domain,dc=alt");

        ld_install_handler(connection->handle, connection_on_add_message, CONNECTION_UPDATE_INTERVAL);
    }

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_ERROR)
    {
        verto_break(ctx);

        fail_test("Error encountered during bind\n");
    }
}

static enum OperationReturnCode connection_on_error(int rc, void* unused_a, void* connection)
{
    (void)(unused_a);

    assert_that(rc, is_not_equal_to(LDAP_SUCCESS));

    verto_break(((ldap_connection_ctx_t*)connection)->base);

    fail_test("OU addition was not successful\n");

    return RETURN_CODE_SUCCESS;
}

Ensure(Cgreen, ou_add_test)
{
    TALLOC_CTX* talloc_ctx = talloc_new(NULL);

    char *envvar = "LDAP_SERVER";
    char *server = get_environment_variable(talloc_ctx, envvar);

    config_t *config = ld_create_config(server, 0, LDAP_VERSION3, "dc=domain,dc=alt",
                                        "admin", "password", true, false, true, false, CONNECTION_UPDATE_INTERVAL,
                                        "", "", "");
    LDHandle *handle = NULL;
    ld_init(&handle, config);

    ld_install_default_handlers(handle);
    ld_install_handler(handle, connection_on_timeout, CONNECTION_UPDATE_INTERVAL);
    ld_install_error_handler(handle, connection_on_error);

    ld_exec(handle);

    ld_free(handle);

    talloc_free(talloc_ctx);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, ou_add_test);
    return run_test_suite(suite, create_text_reporter());
}
