#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include <connection.h>
#include <connection_state_machine.h>
#include <directory.h>
#include <entry.h>
#include <entry_p.h>
#include <schema.h>
#include <schema_p.h>
#include <talloc.h>

#include <ldap.h>
#include <ldap_schema.h>

#include <test_common.h>

const int LDAP_DEBUG_ANY = -1;
const int BUFFER_SIZE = 80;

const int CONNECTION_UPDATE_INTERVAL = 1000;

static int current_directory_type = LDAP_TYPE_UNKNOWN;

static ldap_schema_t* schema = NULL;
static TALLOC_CTX* talloc_ctx = NULL;

static void connection_on_search_message(verto_ctx *ctx, verto_ev *ev)
{
    (void)(ev);

    static int callcount = 0;

    if (++callcount > 10)
    {
        verto_break(ctx);

        int index = 0;
        LDAPObjectClass* current_class = ldap_schema_object_classes(schema)[index];
        while (current_class != NULL)
        {
            printf("Object class: %s\n", current_class->oc_names[0]);
            current_class = ldap_schema_object_classes(schema)[++index];
        }

        index = 0;
        LDAPAttributeType* current_attrribute = ldap_schema_attribute_types(schema)[index];
        while (current_attrribute)
        {
            printf("Attribute type: %s\n", current_attrribute->at_names[0]);
            current_attrribute = ldap_schema_attribute_types(schema)[++index];
        }

        talloc_free(talloc_ctx);
    }
}

static void connection_on_timeout(verto_ctx *ctx, verto_ev *ev)
{
    (void)(ctx);

    struct ldap_connection_ctx_t* connection = verto_get_private(ev);

    csm_next_state(connection->state_machine);


    if (connection->state_machine->state == LDAP_CONNECTION_STATE_RUN)
    {
        verto_del(ev);

        switch (current_directory_type)
        {
        case LDAP_TYPE_ACTIVE_DIRECTORY:
            break;
        default:
            verto_break(ctx);

            ld_warning("Disable openldap schema test for non AD directory servers!");

            pass_test();

            return;
        }

        talloc_ctx = talloc_new(NULL);

        schema = ldap_schema_new(talloc_ctx);

        int rc = schema_load_active_directory(connection, schema);

        if (rc == RETURN_CODE_FAILURE)
        {
            fail_test("Error schema_load_active_directory failed\n");
        }

        if (rc == RETURN_CODE_SUCCESS)
        {
            verto_add_timeout(ctx, VERTO_EV_FLAG_PERSIST, connection_on_search_message, CONNECTION_UPDATE_INTERVAL);
        }
    }

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_ERROR)
    {
        verto_break(ctx);

        fail_test("Error encountered during bind\n");
    }
}

Ensure(load_active_directory_schema_test) {
    start_test(connection_on_timeout, CONNECTION_UPDATE_INTERVAL, &current_directory_type, false);
}

TestSuite*
schema_load_active_directory_schema_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, load_active_directory_schema_test);
    return suite;
}
