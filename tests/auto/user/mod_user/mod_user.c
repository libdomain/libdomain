#include <cgreen/cgreen.h>

#include <domain.h>
#include <user.h>
#include <talloc.h>

#include <connection_state_machine.h>

#include <test_common.h>

const int LDAP_DEBUG_ANY = -1;
const int BUFFER_SIZE = 80;

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

const int CONNECTION_UPDATE_INTERVAL = 1000;

static int current_directory_type = LDAP_TYPE_UNKNOWN;

LDAPAttribute_t** attrs;

static LDAPAttribute_t** fill_user_attributes2(TALLOC_CTX* ctx)
{
    attrs = talloc_array(ctx, LDAPAttribute_t*, 2);
    attrs[0] = talloc(ctx, LDAPAttribute_t);
    attrs[0]->values = talloc_array(ctx, char*, 2);
    attrs[0]->name = talloc_strdup(ctx, "userPassword");
    attrs[0]->values[0] = talloc_strdup(ctx, "plainPass123");
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

        int rc = ld_mod_user(connection->handle, "test_mod_user", NULL,
                             fill_user_attributes2(talloc_ctx));
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

Ensure(Cgreen, user_mod_test)
{
    start_test(connection_on_timeout, CONNECTION_UPDATE_INTERVAL, &current_directory_type);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, user_mod_test);
    return run_test_suite(suite, create_text_reporter());
}
