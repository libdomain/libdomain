#include <cgreen/cgreen.h>

#include <domain.h>
#include <domain_p.h>
#include <schema_p.h>
#include <talloc.h>

#include <connection_state_machine.h>

#include <test_common.h>

const int LDAP_DEBUG_ANY = -1;
const int BUFFER_SIZE = 80;

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

        enum OperationReturnCode rc = schema_load_active_directory(connection, NULL, "dc=domain,dc=alt");
        assert_that(rc, is_equal_to(RETURN_CODE_SUCCESS));

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

    fail_test("User addition was not successful\n");

    return RETURN_CODE_SUCCESS;
}

Ensure(load_ad_schema_test)
{
    TALLOC_CTX* talloc_ctx = talloc_new(NULL);

    char *envvar = "LDAP_SERVER";
    char *server = get_environment_variable(talloc_ctx, envvar);

    config_t *config = ld_create_config(server, 0, LDAP_VERSION3, "dc=domain,dc=alt",
                                        "admin", "password", false, false, true, false, CONNECTION_UPDATE_INTERVAL,
                                        "", "", "");
    LDHandle *handle = NULL;
    ld_init(&handle, config);

    int debug_level = 0;
    ldap_set_option((*handle).connection_ctx->ldap, LDAP_OPT_DEBUG_LEVEL, &debug_level);

    ld_install_default_handlers(handle);
    ld_install_handler(handle, connection_on_timeout, CONNECTION_UPDATE_INTERVAL);
    ld_install_error_handler(handle, connection_on_error);

    ld_exec(handle);

    ld_free(handle);

    talloc_free(talloc_ctx);
}

TestSuite*
schema_ad_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, load_ad_schema_test);
    return suite;
}
