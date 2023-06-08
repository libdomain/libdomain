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

const int CONNECTION_UPDATE_INTERVAL = 1000;

LDAPAttribute_t** attrs;

static LDAPAttribute_t** fill_ou_attributes(TALLOC_CTX* ctx)
{
    attrs = talloc_array(ctx, LDAPAttribute_t*, 2);
    attrs[0] = talloc(ctx, LDAPAttribute_t);
    attrs[0]->values = talloc_array(ctx, char*, 2);
    attrs[0]->name = talloc_strdup(ctx, "description");
    attrs[0]->values[0] = talloc_strdup(ctx, "Description_modification");
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

        TALLOC_CTX* talloc_ctx = talloc_new(NULL);

        int rc = ld_mod_ou(connection->handle, "test_mod_ou", "dc=domain,dc=alt", fill_ou_attributes(talloc_ctx));
        assert_that(rc,is_equal_to(RETURN_CODE_SUCCESS));

        talloc_free(talloc_ctx);

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

    fail_test("OU modification was not successful\n");

    return RETURN_CODE_SUCCESS;
}

Ensure(Cgreen, ou_mod_test)
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
    add_test_with_context(suite, Cgreen, ou_mod_test);
    return run_test_suite(suite, create_text_reporter());
}