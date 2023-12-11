#include <cgreen/cgreen.h>

#include <directory.h>
#include <connection_state_machine.h>
#include <domain.h>
#include <talloc.h>

#include <test_common.h>

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

static const int CONNECTION_UPDATE_INTERVAL = 1000;

static int current_directory_type = LDAP_TYPE_UNKNOWN;

static void connection_on_message(verto_ctx *ctx, verto_ev *ev)
{
    (void)(ev);

    static int callcount = 0;

    struct ldap_connection_ctx_t* connection = verto_get_private(ev);

    csm_next_state(connection->state_machine);

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_RUN)
    {
        info("Reconnect attempt test success!\n");

        verto_break(ctx);
    }

    if (++callcount > 10)
    {
        verto_break(ctx);
    }
}

static void connection_on_timeout(verto_ctx *ctx, verto_ev *ev)
{
    (void)(ctx);

    struct ldap_connection_ctx_t* connection = verto_get_private(ev);

    csm_next_state(connection->state_machine);

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_ERROR)
    {
        verto_break(ctx);

        fail_test("Error during test!\n");
    }

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_RUN)
    {
        verto_del(ev);

        connection->state_machine->state = LDAP_CONNECTION_STATE_ERROR;

        ldap_unbind_ext_s(connection->ldap, NULL, NULL);

        verto_ev* ev2 = verto_add_timeout(ctx, VERTO_EV_FLAG_PERSIST, connection_on_message, CONNECTION_UPDATE_INTERVAL);
        verto_set_private(ev2, connection, NULL);
    }
}

Ensure(Cgreen, reconnect_test)
{
    start_test(connection_on_timeout, CONNECTION_UPDATE_INTERVAL, &current_directory_type, false);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, reconnect_test);
    return run_test_suite(suite, create_text_reporter());
}
