#include <cgreen/cgreen.h>

#include <connection.h>
#include <connection_state_machine.h>
#include <entry.h>
#include <directory.h>
#include <talloc.h>

#include <test_common.h>

const int LDAP_DEBUG_ANY = -1;
const int BUFFER_SIZE = 80;

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

char* LDAP_DIRECTORY_ATTRS[] = { "objectClass", NULL };

const int CONNECTION_UPDATE_INTERVAL = 1000;

static int current_directory_type = LDAP_TYPE_UNKNOWN;

static void connection_on_search_message(verto_ctx *ctx, verto_ev *ev)
{
    (void)(ev);

    static int callcount = 0;

    if (++callcount > 10)
    {
        verto_break(ctx);
    }
}

static enum OperationReturnCode middle_search_callback(struct ldap_connection_ctx_t *connection, ld_entry_t** entries)
{
    info("Empty search callback has been called!\n");

    return RETURN_CODE_SUCCESS;
}

static enum OperationReturnCode begin_search_callback(struct ldap_connection_ctx_t *connection, ld_entry_t** entries)
{
    info("Begin search callback has been called!\n");

    return RETURN_CODE_SUCCESS;
}

static enum OperationReturnCode end_search_callback(struct ldap_connection_ctx_t *connection, ld_entry_t** entries)
{
    info("End search callback has been called!\n");

    return RETURN_CODE_SUCCESS;
}

static void connection_on_timeout(verto_ctx *ctx, verto_ev *ev)
{
    (void)(ctx);

    struct ldap_connection_ctx_t* connection = verto_get_private(ev);

    csm_next_state(connection->state_machine);

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_RUN)
    {
        verto_del(ev);

        search(connection, "dc=domain,dc=alt", LDAP_SCOPE_SUBTREE,
               "(objectClass=*)", LDAP_DIRECTORY_ATTRS, 0, begin_search_callback);

        search(connection, "dc=domain,dc=alt", LDAP_SCOPE_SUBTREE,
               "(objectClass=*)", LDAP_DIRECTORY_ATTRS, 0, NULL);

        search(connection, "dc=domain,dc=alt", LDAP_SCOPE_SUBTREE,
               "(objectClass=*)", LDAP_DIRECTORY_ATTRS, 0, middle_search_callback);

        search(connection, "dc=domain,dc=alt", LDAP_SCOPE_SUBTREE,
               "(objectClass=*)", LDAP_DIRECTORY_ATTRS, 0, end_search_callback);

        verto_add_timeout(ctx, VERTO_EV_FLAG_PERSIST, connection_on_search_message, CONNECTION_UPDATE_INTERVAL);
    }

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_ERROR)
    {
        verto_break(ctx);

        fail_test("Error encountered during bind\n");
    }
}

Ensure(Cgreen, entry_search_test) {
    start_test(connection_on_timeout, CONNECTION_UPDATE_INTERVAL, &current_directory_type, false);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, entry_search_test);
    return run_test_suite(suite, create_text_reporter());
}
