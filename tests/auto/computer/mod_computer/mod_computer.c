#include <cgreen/cgreen.h>

#include <directory.h>
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

const int CONNECTION_UPDATE_INTERVAL = 1000;

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

static char* OPENLDAP_OBJECT_CLASSES[] = { "top", "device", NULL };
static char* OPENLDAP_CN[] = { "test_mod_computer", NULL };
static char* OPENLDAP_DESCRIPTION[] = { "description", NULL };
static char* OPENLDAP_SERIAL_NUMBER[] = { "11-77-23-15", NULL };
static char* OPENLDAP_SEE_ALSO[] = { "dc=domain,dc=alt", NULL };
static char* OPENLDAP_O[] = { "test org. inc", NULL };
static char* OPENLDAP_L[] = { "room 12", NULL };
static char* OPENLDAP_OWNER[] = { "cn=john smith,ou=people,dc=domain,dc=alt", NULL };
static char* OPENLDAP_OU[] = { "r&d", NULL };

static LDAPAttribute_t OPENLDAP_COMPUTER_ATTRIBUTES[] =
{
    { .name = "objectClass", .values = OPENLDAP_OBJECT_CLASSES },
    { "cn", OPENLDAP_CN },
    { "description", OPENLDAP_DESCRIPTION },
    { "serialnumber", OPENLDAP_SERIAL_NUMBER },
    { "seeAlso", OPENLDAP_SEE_ALSO },
    { "o", OPENLDAP_O },
    { "l", OPENLDAP_L },
    { "owner", OPENLDAP_OWNER },
    { "ou", OPENLDAP_OU },
};
static const int OPENLDAP_COMPUTER_ATTRIBUTES_SIZE = number_of_elements(OPENLDAP_COMPUTER_ATTRIBUTES);

static char* AD_COMPUTER_OBJECT_CLASS[] = { "top", "person", "organizationalPerson", "user", "computer", NULL };
static char* AD_COMPUTER_DESCRIPTION[] = { "Test computer modification success", NULL };
static char* AD_COMPUTER_SAM_ACCOUNT_NAME[] = { "test_mod_c", NULL };
static char* AD_COMPUTER_MS_SFU30_ALIASES[] = { "TestAlias1", "TestAlias2", NULL };
static char* AD_COMPUTER_MS_SFU30_NIS_DOMAIN[] = { "TestNisDomain", NULL };
static char* AD_COMPUTER_NIS_MAP_NAME[] = { "TestNisMapName", NULL };
static char* AD_COMPUTER_MS_SFU30_NAME[] = { "TestSFUName", NULL };
static char* AD_COMPUTER_MANAGED_BY[] = { "cn=Administrator,cn=Users,dc=domain,dc=alt", NULL };
static char* AD_COMPUTER_LOCATION[] = { "Room101", NULL };
static char* AD_COMPUTER_DNS_HOST_NAME[] = { "test_mod_computer.domain.alt", NULL };
static char* AD_COMPUTER_OPERATING_SYSTEM_VERSION[] = { "Windows 10", NULL };
static char* AD_COMPUTER_OPERATING_SYSTEM_SERVICE_PACK[] = { "SP1", NULL };
static char* AD_COMPUTER_OPERATING_SYSTEM_HOTFIX[] = { "KB123456", NULL };
static char* AD_COMPUTER_OPERATING_SYSTEM[] = { "Windows", NULL };
static char* AD_COMPUTER_NETWORK_ADDRESS[] = { "192.168.1.100", NULL };
static char* AD_COMPUTER_NETBOOT_DUID[] = { "01:02:03:04:05:06", NULL };
static char* AD_COMPUTER_NETBOOT_SIF_FILE[] = { "/path/to/netboot.sif", NULL };
static char* AD_COMPUTER_NETBOOT_MIRROR_DATA_FILE[] = { "/path/to/mirror.dat", NULL };
static char* AD_COMPUTER_NETBOOT_MACHINE_FILE_PATH[] = { "/path/to/machinefile.txt", NULL };
static char* AD_COMPUTER_NETBOOT_INITIALIZATION[] = { "TestInitialization", NULL };
static char* AD_COMPUTER_POLICY_REPLICATION_FLAGS[] = { "1", NULL };
static char* AD_COMPUTER_VOLUME_COUNT[] = { "2", NULL };

static LDAPAttribute_t AD_COMPUTER_ATTRIBUTES[] =
{
   { .name = "objectClass", .values = AD_COMPUTER_OBJECT_CLASS },
   { .name = "description", .values = AD_COMPUTER_DESCRIPTION },
   { .name = "sAMAccountName", .values = AD_COMPUTER_SAM_ACCOUNT_NAME },
   { .name = "msSFU30Aliases", .values = AD_COMPUTER_MS_SFU30_ALIASES },
   { .name = "msSFU30NisDomain", .values = AD_COMPUTER_MS_SFU30_NIS_DOMAIN },
   { .name = "nisMapName", .values = AD_COMPUTER_NIS_MAP_NAME },
   { .name = "msSFU30Name", .values = AD_COMPUTER_MS_SFU30_NAME },
   { .name = "managedBy", .values = AD_COMPUTER_MANAGED_BY },
   { .name = "location", .values = AD_COMPUTER_LOCATION },
   { .name = "dNSHostName", .values = AD_COMPUTER_DNS_HOST_NAME },
   { .name = "operatingSystemVersion", .values = AD_COMPUTER_OPERATING_SYSTEM_VERSION },
   { .name = "operatingSystemServicePack", .values = AD_COMPUTER_OPERATING_SYSTEM_SERVICE_PACK },
   { .name = "operatingSystemHotfix", .values = AD_COMPUTER_OPERATING_SYSTEM_HOTFIX },
   { .name = "operatingSystem", .values = AD_COMPUTER_OPERATING_SYSTEM },
   { .name = "networkAddress", .values = AD_COMPUTER_NETWORK_ADDRESS },
   { .name = "netbootDUID", .values = AD_COMPUTER_NETBOOT_DUID },
   { .name = "netbootSIFFile", .values = AD_COMPUTER_NETBOOT_SIF_FILE },
   { .name = "netbootMirrorDataFile", .values = AD_COMPUTER_NETBOOT_MIRROR_DATA_FILE },
   { .name = "netbootMachineFilePath", .values = AD_COMPUTER_NETBOOT_MACHINE_FILE_PATH },
   { .name = "netbootInitialization", .values = AD_COMPUTER_NETBOOT_INITIALIZATION },
   { .name = "policyReplicationFlags", .values = AD_COMPUTER_POLICY_REPLICATION_FLAGS },
   { .name = "volumeCount", .values = AD_COMPUTER_VOLUME_COUNT }
};
static const int AD_COMPUTER_ATTRIBUTES_SIZE = number_of_elements(AD_COMPUTER_ATTRIBUTES);

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

typedef struct testcase_s
{
    char* name;
    char* entry_cn;
    char* entry_parent;
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
        "Test existing computer modification OpenLDAP",
        "test_mod_computer",
        "ou=equipment,dc=domain,dc=alt",
        OPENLDAP_COMPUTER_ATTRIBUTES_SIZE,
        RETURN_CODE_SUCCESS,
        OPENLDAP_COMPUTER_ATTRIBUTES
    }
};

static const int NUMBER_OF_OPENLDAP_TESTCASES = number_of_elements(OPENLDAP_TESTCASES);

static testcase_t AD_TESTCASES[] =
{
    {
        "Test existing computer modification AD",
        "test_mod_c",
        "cn=computers,dc=domain,dc=alt",
        AD_COMPUTER_ATTRIBUTES_SIZE,
        RETURN_CODE_SUCCESS,
        AD_COMPUTER_ATTRIBUTES
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

            enum OperationReturnCode rc = ld_mod_computer(connection->handle,
                                                          testcase.entry_cn,
                                                          testcase.entry_parent,
                                                          fill_user_attributes(talloc_ctx, testcase.attributes, testcase.number_of_attributes));
            assert_that(rc,is_equal_to(testcase.desired_test_result));
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

Ensure(Cgreen, computer_mod_test)
{
    start_test(connection_on_timeout, CONNECTION_UPDATE_INTERVAL, &current_directory_type);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, computer_mod_test);
    return run_test_suite(suite, create_text_reporter());
}
