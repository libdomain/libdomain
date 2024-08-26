#include <cgreen/cgreen.h>

#include <connection.h>
#include <connection_state_machine.h>
#include <entry.h>
#include <entry_p.h>
#include <directory.h>
#include <talloc.h>

#include <ldap.h>
#include <ldap_schema.h>

#include <test_common.h>

const int LDAP_DEBUG_ANY = -1;
const int BUFFER_SIZE = 80;

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

char* LDAP_DIRECTORY_ATTRS[] = { "attributetypes", NULL };

const int CONNECTION_UPDATE_INTERVAL = 1000;

static int current_directory_type = LDAP_TYPE_UNKNOWN;

static enum OperationReturnCode middle_search_callback(struct ldap_connection_ctx_t *connection, ld_entry_t** entries)
{
    if (entries != NULL && entries[0] != NULL)
    {
        ld_entry_t* current_entry = entries[0];
        printf("%s\n", current_entry->dn);
        LDAPAttribute_t** attributes = ld_entry_get_attributes(current_entry);
        
        if (attributes == NULL)
        {
            return RETURN_CODE_SUCCESS;
        }

        int index = 0;
        LDAPAttribute_t* current_attribute = attributes[index];
        while (current_attribute != NULL)
        {
            printf("Attribute: %s\n", current_attribute->name);

            if (current_attribute->values == NULL)
            {
                continue;
            }

            int value_index = 0;
            char* current_value = current_attribute->values[value_index];
            while (current_value != NULL)
            {
                // printf("Value: %s\n", current_value);
                int error_code = 0;
                char* error_message = NULL;
                LDAPAttributeType* attribute_type = ldap_str2attributetype(current_value, &error_code, &error_message, LDAP_SCHEMA_ALLOW_ALL);
                if (!attribute_type)
                {
                    printf("Error: %d %s", error_code, error_message);
                }
                else
                {
                    printf("Attribute_type: %s\n", attribute_type->at_names[0]);
                }
                current_value = current_attribute->values[++value_index];
            }
            current_attribute = attributes[++index];
        }
        
    }
    ld_info("Empty search callback has been called!\n");

    return RETURN_CODE_SUCCESS;
}

static void connection_on_search_message(verto_ctx *ctx, verto_ev *ev)
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

    csm_next_state(connection->state_machine);

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_RUN)
    {
        verto_del(ev);

        char* search_base = "cn=subschema";

        search(connection, search_base, LDAP_SCOPE_BASE,
               "(objectclass=subschema)", LDAP_DIRECTORY_ATTRS, 0, &middle_search_callback);

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
