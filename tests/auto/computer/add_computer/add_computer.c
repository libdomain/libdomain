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

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

static char* OPENLDAP_OBJECT_CLASSES[] = { "top", "device", NULL };
static char* OPENLDAP_CN[] = { "test_computer_add", NULL };
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

static char* AD_COMPUTER_OBJECT_CLASS[] = { "top", "person", "organizationalPerson", "user", "ipHost", "computer", NULL };
static char* AD_COMPUTER_CN[] = { "TestComputer", NULL };
static char* AD_COMPUTER_SAM_ACCOUNT_NAME[] = { "TestComputer", NULL };
static char* AD_COMPUTER_MS_SFU30_ALIASES[] = { "TestAlias1", "TestAlias2", NULL };
static char* AD_COMPUTER_MS_SFU30_NIS_DOMAIN[] = { "TestNisDomain", NULL };
static char* AD_COMPUTER_NIS_MAP_NAME[] = { "TestNisMapName", NULL };
static char* AD_COMPUTER_MS_SFU30_NAME[] = { "TestSFUName", NULL };
static char* AD_COMPUTER_MS_DS_HOST_SERVICE_ACCOUNT[] = { "TestHostServiceAccount", NULL };
static char* AD_COMPUTER_MANAGED_BY[] = { "cn=Administrator,cn=Users,dc=example,dc=com", NULL };
static char* AD_COMPUTER_LOCATION[] = { "Room101", NULL };
static char* AD_COMPUTER_DNS_HOST_NAME[] = { "testcomputer.example.com", NULL };
static char* AD_COMPUTER_DEFAULT_LOCAL_POLICY_OBJECT[] = { "TestLocalPolicyObject", NULL };
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
static char* AD_COMPUTER_NETBOOT_GUID[] = { "12345678-90ab-cdef-1234-567890abcdef", NULL };
static char* AD_COMPUTER_MS_DS_ADDITIONAL_SAM_ACCOUNT_NAME[] = { "TestAdditionalSamAccount", NULL };
static char* AD_COMPUTER_MS_DS_ADDITIONAL_DNS_HOST_NAME[] = { "TestAdditionalDNS", NULL };
static char* AD_COMPUTER_MACHINE_ROLE[] = { "Workstation", NULL };
static char* AD_COMPUTER_SITE_GUID[] = { "98765432-10ab-cdef-5678-90abcdefabcd", NULL };
static char* AD_COMPUTER_RID_SET_REFERENCES[] = { "CN=RID Set,CN=TestComputer,OU=Domain Controllers,DC=example,DC=com", NULL };
static char* AD_COMPUTER_POLICY_REPLICATION_FLAGS[] = { "1", NULL };
static char* AD_COMPUTER_PHYSICAL_LOCATION_OBJECT[] = { "CN=Building1,OU=Locations,DC=example,DC=com", NULL };
static char* AD_COMPUTER_VOLUME_COUNT[] = { "2", NULL };
static char* AD_COMPUTER_MS_TPM_TPM_INFORMATION_FOR_COMPUTER[] = { "TestTPMInfo", NULL };
static char* AD_COMPUTER_MS_DS_GENERATION_ID[] = { "1234567890", NULL };
static char* AD_COMPUTER_MS_IMAGING_THUMBPRINT_HASH[] = { "TestThumbprintHash", NULL };
static char* AD_COMPUTER_MS_IMAGING_HASH_ALGORITHM[] = { "SHA256", NULL };

static LDAPAttribute_t AD_COMPUTER_ATTRIBUTES[] =
{
   { .name = "objectClass", .values = AD_COMPUTER_OBJECT_CLASS },
   { .name = "cn", .values = AD_COMPUTER_CN },
   { .name = "sAMAccountName", .values = AD_COMPUTER_SAM_ACCOUNT_NAME },
   { .name = "msSFU30Aliases", .values = AD_COMPUTER_MS_SFU30_ALIASES },
   { .name = "msSFU30NisDomain", .values = AD_COMPUTER_MS_SFU30_NIS_DOMAIN },
   { .name = "nisMapName", .values = AD_COMPUTER_NIS_MAP_NAME },
   { .name = "msSFU30Name", .values = AD_COMPUTER_MS_SFU30_NAME },
   { .name = "msDS_HostServiceAccount", .values = AD_COMPUTER_MS_DS_HOST_SERVICE_ACCOUNT },
   { .name = "managedBy", .values = AD_COMPUTER_MANAGED_BY },
   { .name = "location", .values = AD_COMPUTER_LOCATION },
   { .name = "dNSHostName", .values = AD_COMPUTER_DNS_HOST_NAME },
   { .name = "defaultLocalPolicyObject", .values = AD_COMPUTER_DEFAULT_LOCAL_POLICY_OBJECT },
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
   { .name = "netbootGUID", .values = AD_COMPUTER_NETBOOT_GUID },
   { .name = "msDS_AdditionalSamAccountName", .values = AD_COMPUTER_MS_DS_ADDITIONAL_SAM_ACCOUNT_NAME },
   { .name = "msDS_AdditionalDnsHostName", .values = AD_COMPUTER_MS_DS_ADDITIONAL_DNS_HOST_NAME },
   { .name = "machineRole", .values = AD_COMPUTER_MACHINE_ROLE },
   { .name = "siteGUID", .values = AD_COMPUTER_SITE_GUID },
   { .name = "rIDSetReferences", .values = AD_COMPUTER_RID_SET_REFERENCES },
   { .name = "policyReplicationFlags", .values = AD_COMPUTER_POLICY_REPLICATION_FLAGS },
   { .name = "physicalLocationObject", .values = AD_COMPUTER_PHYSICAL_LOCATION_OBJECT },
   { .name = "volumeCount", .values = AD_COMPUTER_VOLUME_COUNT },
   { .name = "msTPM_TpmInformationForComputer", .values = AD_COMPUTER_MS_TPM_TPM_INFORMATION_FOR_COMPUTER },
   { .name = "msDS_GenerationId", .values = AD_COMPUTER_MS_DS_GENERATION_ID },
   { .name = "msImaging_ThumbprintHash", .values = AD_COMPUTER_MS_IMAGING_THUMBPRINT_HASH },
   { .name = "msImaging_HashAlgorithm", .values = AD_COMPUTER_MS_IMAGING_HASH_ALGORITHM }
};

static const int AD_COMPUTER_ATTRIBUTES_SIZE = number_of_elements(AD_COMPUTER_ATTRIBUTES);

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

        int rc = ld_add_computer(connection->handle,
                                 "test_computer_add",
                                 "description",
                                 "11-77-23-15",
                                 "dc=domain,dc=alt",
                                 "test org. inc",
                                 "room 12",
                                 "cn=john smith,ou=people,dc=domain,dc=alt",
                                 "r&d",
                                 "ou=equipment,dc=domain,dc=alt");
        if (rc != RETURN_CODE_SUCCESS)
        {
            verto_break(ctx);

            fail_test("Error encountered during bind\n");
        }
        else
        {
            ld_install_handler(connection->handle, connection_on_add_message, CONNECTION_UPDATE_INTERVAL);
        }
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

    fail_test("Computer addition was not successful\n");

    return RETURN_CODE_SUCCESS;
}

Ensure(Cgreen, computer_add_test)
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
    add_test_with_context(suite, Cgreen, computer_add_test);
    return run_test_suite(suite, create_text_reporter());
}
