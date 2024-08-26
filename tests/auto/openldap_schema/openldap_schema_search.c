#include <cgreen/cgreen.h>

#include <connection.h>
#include <connection_state_machine.h>
#include <entry.h>
#include <entry_p.h>
#include <directory.h>
#include <talloc.h>
#include <schema.h>
#include <schema_p.h>

#include <ldap.h>
#include <ldap_schema.h>

#include <test_common.h>

const int LDAP_DEBUG_ANY = -1;
const int BUFFER_SIZE = 80;

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

char* LDAP_DIRECTORY_ATTRS[] = { "attributetypes", NULL };
char* LDAP_DIRECTORY_OBJECTCLASSES[] = { "objectclasses", NULL };
 
const int CONNECTION_UPDATE_INTERVAL = 1000;

static int current_directory_type = LDAP_TYPE_UNKNOWN;

static ldap_schema_t* schema = NULL;
static TALLOC_CTX* talloc_ctx = NULL;

static enum OperationReturnCode ldapobjectclass_search_callback(struct ldap_connection_ctx_t *connection, ld_entry_t** entries, void* user_data)
{
    if (entries != NULL && entries[0] != NULL)
    {
        ld_entry_t* current_entry = entries[0];
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
                int error_code = 0;
                const char* error_message = NULL;
                LDAPObjectClass* object_class = ldap_str2objectclass(current_value, &error_code, &error_message, LDAP_SCHEMA_ALLOW_ALL);

                if (!object_class)
                {
                    printf("Error: %d %s\n", error_code, error_message);
                }
                else
                {
                    if (!ldap_schema_append_objectclass(schema, object_class))
                    {
                        printf("Error: unable to add class to the schema!\n");
                    }                    // printf("Attribute_type: %s\n", attribute_type->at_names[0]);
                }
                current_value = current_attribute->values[++value_index];
            }
            current_attribute = attributes[++index];
        }
        
    }
    ld_info("Empty search callback has been called!\n");

    return RETURN_CODE_SUCCESS;
}

static enum OperationReturnCode ldapattributetype_search_callback(struct ldap_connection_ctx_t *connection, ld_entry_t** entries, void* user_data)
{
    if (entries != NULL && entries[0] != NULL)
    {
        ld_entry_t* current_entry = entries[0];
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
                int error_code = 0;
                const char* error_message = NULL;
                LDAPAttributeType* attribute_type = ldap_str2attributetype(current_value, &error_code, &error_message, LDAP_SCHEMA_ALLOW_ALL);
                if (!attribute_type)
                {
                    printf("Error: %d %s\n", error_code, error_message);
                }
                else
                {
                    if (!ldap_schema_append_attributetype(schema, attribute_type))
                    {
                        printf("Error: unable to add class to the schema!\n");
                    }
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

        char* search_base = "cn=subschema";

        talloc_ctx = talloc_new(NULL);

        schema = ldap_schema_new(talloc_ctx);

        search(connection, search_base, LDAP_SCOPE_BASE,
               "(objectclass=subschema)", LDAP_DIRECTORY_ATTRS, 0, &ldapattributetype_search_callback, NULL);
        search(connection, search_base, LDAP_SCOPE_BASE,
               "(objectclass=subschema)", LDAP_DIRECTORY_OBJECTCLASSES, 0, &ldapobjectclass_search_callback, NULL);

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
