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
