#include <cgreen/cgreen.h>

#include <domain.h>
#include <entry.h>
#include <talloc.h>

#include <connection_state_machine.h>

#include <test_common.h>

const int LDAP_DEBUG_ANY = -1;
const int BUFFER_SIZE = 80;

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

#define VALUE_ATTRIBUTES_SIZE 5

typedef struct attribute_value_pair_s
{
    char* name;
    char* value[VALUE_ATTRIBUTES_SIZE];
} attribute_value_pair_t;

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

static attribute_value_pair_t LDAP_TEST_USER_ATTRIBUTES[] =
{
    { "objectClass", { "top", "account", "posixAccount", "shadowAccount" , NULL } },
    { "cn", { "adam", NULL, NULL, NULL, NULL } },
    { "uid", { "adam", NULL, NULL, NULL, NULL } },
    { "uidNumber", { "0", NULL, NULL, NULL, NULL } },
    { "gidNumber", { "0", NULL, NULL, NULL, NULL } },
    { "homeDirectory", { "/home/adam", NULL, NULL, NULL, NULL } },
    { "loginShell", { "/bin/bash", NULL, NULL, NULL, NULL } },
    { "gecos", { "adam", NULL, NULL, NULL, NULL } },
    { "userPassword", { "{crypt}x", NULL, NULL, NULL, NULL } },
    { "shadowLastChange", { "0", NULL, NULL, NULL, NULL } },
    { "shadowMax", { "0", NULL, NULL, NULL, NULL } },
    { "shadowWarning", { "0", NULL, NULL, NULL, NULL } }
};
const int USER_ATTRIBUTES_SIZE = number_of_elements(LDAP_TEST_USER_ATTRIBUTES);

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

        TALLOC_CTX *talloc_ctx = talloc_new(NULL);

        LDAPMod **attrs = talloc_array(talloc_ctx, LDAPMod*, USER_ATTRIBUTES_SIZE + 1);

        int i = 0;
        for (; i < USER_ATTRIBUTES_SIZE; ++i)
        {
            char* name = LDAP_TEST_USER_ATTRIBUTES[i].name;
            char** value = LDAP_TEST_USER_ATTRIBUTES[i].value;

            attrs[i] = talloc(talloc_ctx, LDAPMod);

            attrs[i]->mod_op = LDAP_MOD_ADD;
            attrs[i]->mod_type = talloc_strndup(talloc_ctx, name, strlen(name));
            attrs[i]->mod_values = talloc_array(talloc_ctx, char*, VALUE_ATTRIBUTES_SIZE);

            for (int index = 0; index < VALUE_ATTRIBUTES_SIZE; ++index)
            {
                if (!value[index])
                {
                    attrs[i]->mod_values[index] = NULL;
                    break;
                }
                attrs[i]->mod_values[index] = talloc_strndup(talloc_ctx, value[index], strlen(value[index]));
            }
        }
        attrs[USER_ATTRIBUTES_SIZE] = NULL;

        enum OperationReturnCode rc = add(connection, "cn=adam,ou=users,dc=domain,dc=alt", attrs);

        talloc_free(talloc_ctx);

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

    fail_test("Entry addition was not successful\n");

    return RETURN_CODE_SUCCESS;
}

Ensure(Cgreen, entry_add_test)
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
    add_test_with_context(suite, Cgreen, entry_add_test);
    return run_test_suite(suite, create_text_reporter());
}
