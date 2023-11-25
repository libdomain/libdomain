#include <cgreen/cgreen.h>

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

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

static char* AD_GROUP_OBJECTCLASS[] = { "top", "group", "posixGroup", NULL };
static char* AD_GROUP_CN[] = { "TestGroup", NULL };
static char* AD_GROUP_GOVERNSID[] = { "1.2.840.113556.1.5.8", NULL };
static char* AD_GROUP_OBJECTCLASSCATEGORY[] = { "1", NULL };
static char* AD_GROUP_RDNATTID[] = { "cn", NULL };
static char* AD_GROUP_SUBCLASSOF[] = { "top", NULL };
static char* AD_GROUP_AUXILIARYCLASS[] = { "posixGroup", NULL };
static char* AD_GROUP_GROUPTYPE[] = { "2147483648", NULL };
static char* AD_GROUP_MSSFU30NAME[] = { "TestGroupSFU", NULL };
static char* AD_GROUP_MSSFU30NISDOMAIN[] = { "exampleNisDomain", NULL };
static char* AD_GROUP_MSSFU30POSIXMEMBER[] = { "posixUser1", "posixUser2", NULL };
static char* AD_GROUP_MSDS_AZAPPLICATIONDATA[] = { "AppData1", "AppData2", NULL };
static char* AD_GROUP_MSDS_AZLASTIMPORTEDBIZRULEPATH[] = { "/path/to/biz/rule", NULL };
static char* AD_GROUP_MSDS_AZBIZRULELANGUAGE[] = { "PowerShell", NULL };
static char* AD_GROUP_MSDS_AZBIZRULE[] = { "<biz_rule_script>", NULL };
static char* AD_GROUP_MSDS_AZGENERICDATA[] = { "GenericDataValue", NULL };
static char* AD_GROUP_MSDS_AZOBJECTGUID[] = { "aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee", NULL };
static char* AD_GROUP_PRIMARYGROUPTOKEN[] = { "12345", NULL };
static char* AD_GROUP_OPERATORCOUNT[] = { "2", NULL };
static char* AD_GROUP_NTGROUPMEMBERS[] = { "CN=User1,CN=Users,DC=example,DC=com", NULL };
static char* AD_GROUP_NONSECURITYMEMBER[] = { "CN=User2,CN=Users,DC=example,DC=com", NULL };
static char* AD_GROUP_MSDS_NONMEMBERS[] = { "CN=User3,CN=Users,DC=example,DC=com", NULL };
static char* AD_GROUP_MSDS_AZLDAPQUERY[] = { "(objectClass=user)", NULL };
static char* AD_GROUP_MEMBER[] = { "CN=User4,CN=Users,DC=example,DC=com", NULL };
static char* AD_GROUP_MANAGEDBY[] = { "CN=Manager,CN=Users,DC=example,DC=com", NULL };
static char* AD_GROUP_GROUPMEMBERSHIPSAM[] = { "GroupSAM", NULL };
static char* AD_GROUP_GROUPATTRIBUTES[] = { "Attribute1", "Attribute2", NULL };
static char* AD_GROUP_MAIL[] = { "testgroup@example.com", NULL };
static char* AD_GROUP_DESKTOPPROFILE[] = { "\\\\server\\profiles\\TestGroup", NULL };
static char* AD_GROUP_CONTROLACCESSRIGHTS[] = { "Read", "Write", NULL };
static char* AD_GROUP_ADMINCOUNT[] = { "1", NULL };
static char* AD_GROUP_MSDS_PRIMARYCOMPUTER[] = { "CN=Computer1,CN=Computers,DC=example,DC=com", NULL };
static char* AD_GROUP_MSDS_PREFERREDDATALOCATION[] = { "C:\\Data\\Groups\\TestGroup", NULL };

static LDAPAttribute_t AD_GROUP_ATTRIBUTES[] =
{
    { .name = "objectClass", .values = AD_GROUP_OBJECTCLASS },
    { .name = "cn", .values = AD_GROUP_CN },
    { .name = "governsID", .values = AD_GROUP_GOVERNSID },
    { .name = "objectClassCategory", .values = AD_GROUP_OBJECTCLASSCATEGORY },
    { .name = "rdnAttID", .values = AD_GROUP_RDNATTID },
    { .name = "subClassOf", .values = AD_GROUP_SUBCLASSOF },
    { .name = "auxiliaryClass", .values = AD_GROUP_AUXILIARYCLASS },
    { .name = "groupType", .values = AD_GROUP_GROUPTYPE },
    { .name = "msSFU30Name", .values = AD_GROUP_MSSFU30NAME },
    { .name = "msSFU30NisDomain", .values = AD_GROUP_MSSFU30NISDOMAIN },
    { .name = "msSFU30PosixMember", .values = AD_GROUP_MSSFU30POSIXMEMBER },
    { .name = "msDS-AzApplicationData", .values = AD_GROUP_MSDS_AZAPPLICATIONDATA },
    { .name = "msDS-AzLastImportedBizRulePath", .values = AD_GROUP_MSDS_AZLASTIMPORTEDBIZRULEPATH },
    { .name = "msDS-AzBizRuleLanguage", .values = AD_GROUP_MSDS_AZBIZRULELANGUAGE },
    { .name = "msDS-AzBizRule", .values = AD_GROUP_MSDS_AZBIZRULE },
    { .name = "msDS-AzGenericData", .values = AD_GROUP_MSDS_AZGENERICDATA },
    { .name = "msDS-AzObjectGuid", .values = AD_GROUP_MSDS_AZOBJECTGUID },
    { .name = "primaryGroupToken", .values = AD_GROUP_PRIMARYGROUPTOKEN },
    { .name = "operatorCount", .values = AD_GROUP_OPERATORCOUNT },
    { .name = "ntGroupMembers", .values = AD_GROUP_NTGROUPMEMBERS },
    { .name = "nonSecurityMember", .values = AD_GROUP_NONSECURITYMEMBER },
    { .name = "msDS-NonMembers", .values = AD_GROUP_MSDS_NONMEMBERS },
    { .name = "msDS-AzLDAPQuery", .values = AD_GROUP_MSDS_AZLDAPQUERY },
    { .name = "member", .values = AD_GROUP_MEMBER },
    { .name = "managedBy", .values = AD_GROUP_MANAGEDBY },
    { .name = "groupMembershipSAM", .values = AD_GROUP_GROUPMEMBERSHIPSAM },
    { .name = "groupAttributes", .values = AD_GROUP_GROUPATTRIBUTES },
    { .name = "mail", .values = AD_GROUP_MAIL },
    { .name = "desktopProfile", .values = AD_GROUP_DESKTOPPROFILE },
    { .name = "controlAccessRights", .values = AD_GROUP_CONTROLACCESSRIGHTS },
    { .name = "adminCount", .values = AD_GROUP_ADMINCOUNT },
    { .name = "msDS-PrimaryComputer", .values = AD_GROUP_MSDS_PRIMARYCOMPUTER },
    { .name = "msDS-PreferredDataLocation", .values = AD_GROUP_MSDS_PREFERREDDATALOCATION }
};

static const int AD_GROUP_ATTRIBUTES_SIZE = number_of_elements(AD_GROUP_ATTRIBUTES);

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

        int rc = ld_add_group(connection->handle, "test_group", "description", 100, "dc=domain,dc=alt");
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

    fail_test("Group addition was not successful\n");

    return RETURN_CODE_SUCCESS;
}

Ensure(Cgreen, group_add_test)
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
    add_test_with_context(suite, Cgreen, group_add_test);
    return run_test_suite(suite, create_text_reporter());
}
