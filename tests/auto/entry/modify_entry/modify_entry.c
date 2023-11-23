#include <cgreen/cgreen.h>

#include <directory.h>
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

typedef struct testcase_s
{
    char* name;
    char* entry_dn;
    int number_of_attributes;
    int desired_test_result;
    attribute_value_pair_t* attributes;
} testcase_t;

typedef struct current_testcases_s
{
    int number_of_testcases;
    testcase_t* testcases;
} current_testcases_t;

static attribute_value_pair_t LDAP_TEST_USER_ATTRIBUTES[] =
{
    { "objectClass", { "top", "account", "posixAccount", "shadowAccount" , NULL } },
    { "uid", { "adam2", NULL, NULL, NULL, NULL } },
    { "uidNumber", { "0", NULL, NULL, NULL, NULL } },
    { "gidNumber", { "0", NULL, NULL, NULL, NULL } },
    { "homeDirectory", { "/home/adam2", NULL, NULL, NULL, NULL } },
    { "loginShell", { "/bin/bash", NULL, NULL, NULL, NULL } },
    { "gecos", { "adam2", NULL, NULL, NULL, NULL } },
    { "userPassword", { "{crypt}x", NULL, NULL, NULL, NULL } },
    { "shadowLastChange", { "0", NULL, NULL, NULL, NULL } },
    { "shadowMax", { "0", NULL, NULL, NULL, NULL } },
    { "shadowWarning", { "0", NULL, NULL, NULL, NULL } }
};
static const int USER_ATTRIBUTES_SIZE = number_of_elements(LDAP_TEST_USER_ATTRIBUTES);

static testcase_t OPENLDAP_TESTCASES[] =
{
    {
        "Modification of existing entry testcase",
        "cn=adam,ou=users,dc=domain,dc=alt",
        USER_ATTRIBUTES_SIZE,
        RETURN_CODE_SUCCESS,
        LDAP_TEST_USER_ATTRIBUTES
    }
};

static const int NUMBER_OF_OPENLDAP_TESTCASES = number_of_elements(OPENLDAP_TESTCASES);

static attribute_value_pair_t LDAP_TEST_USER_ATTRIBUTES_AD[] =
{
    { "objectClass", { "top", "person", "organizationalPerson", "user", NULL } },
    { "objectCategory", { "CN=Person,CN=Schema,CN=Configuration,DC=domain,DC=alt", NULL, NULL, NULL, NULL } },
    { "codePage", { "0", NULL, NULL, NULL, NULL } },
    { "countryCode", { "0", NULL, NULL, NULL, NULL } },
    { "distinguishedName", { "CN=adam,CN=Users,DC=domain,DC=alt", NULL, NULL, NULL, NULL } },
    { "cn", { "adam", NULL, NULL, NULL, NULL } },
    { "sn", { "adam", NULL, NULL, NULL, NULL } },
    { "givenName", { "adam2", NULL, NULL, NULL, NULL } },
    { "displayName", { "adam2", NULL, NULL, NULL, NULL } },
    { "name", { "adam", NULL, NULL, NULL, NULL } },
    { "telephoneNumber", { "+7(999) 999-99-99", NULL, NULL, NULL, NULL } },
    { "instanceType", { "4", NULL, NULL, NULL, NULL } },
    { "userAccountControl", { "514", NULL, NULL, NULL, NULL } },
    { "accountExpires", { "0", NULL, NULL, NULL, NULL } },
    { "uidNumber", { "500", NULL, NULL, NULL, NULL } },
    { "gidNumber", { "500", NULL, NULL, NULL, NULL } },
    { "sAMAccountName", { "adam2", NULL, NULL, NULL, NULL } },
    { "userPrincipalName", { "adam2@domain.alt", NULL, NULL, NULL, NULL } },
    { "altSecurityIdentities", { "Kerberos:adam@DOMAIN.ALT", NULL, NULL, NULL, NULL } },
    { "mail", { "adam2@domain.alt", NULL, NULL, NULL, NULL } },
    { "homeDirectory", { "/home/adam2", NULL, NULL, NULL, NULL } },
    { "homeDrive", { "A:", NULL, NULL, NULL, NULL } },
    { "unixHomeDirectory", { "/home/adam2", NULL, NULL, NULL, NULL } },
    { "loginShell", { "/bin/bash", NULL, NULL, NULL, NULL } }
};
static const int USER_ATTRIBUTES_AD_SIZE = number_of_elements(LDAP_TEST_USER_ATTRIBUTES_AD);

static testcase_t AD_TESTCASES[] =
{
    {
        "Modification of existing entry testcase",
        "cn=adam,cn=users,dc=domain,dc=alt",
        USER_ATTRIBUTES_AD_SIZE,
        RETURN_CODE_SUCCESS,
        LDAP_TEST_USER_ATTRIBUTES_AD
    }
};

static const int NUMBER_OF_AD_TESTCASES = number_of_elements(AD_TESTCASES);

static const int CONNECTION_UPDATE_INTERVAL = 1000;

static int current_directory_type = LDAP_TYPE_UNKNOWN;

static current_testcases_t get_current_testcases(int directory_type)
{
    current_testcases_t result = { .testcases = NULL, .number_of_testcases = 0 };

    switch (directory_type)
    {
    case LDAP_TYPE_ACTIVE_DIRECTORY:
        result.testcases = AD_TESTCASES;
        result.number_of_testcases = NUMBER_OF_AD_TESTCASES;
        break;
    case LDAP_TYPE_OPENLDAP:
        result.testcases = OPENLDAP_TESTCASES;
        result.number_of_testcases = NUMBER_OF_OPENLDAP_TESTCASES;
    default:
        break;
    }

    return result;
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

        current_testcases_t current_testcases = get_current_testcases(current_directory_type);
        for (int test_index = 0; test_index < current_testcases.number_of_testcases; test_index++)
        {
            testcase_t testcase = current_testcases.testcases[test_index];

            TALLOC_CTX *talloc_ctx = talloc_new(NULL);

            LDAPMod **attrs = talloc_array(talloc_ctx, LDAPMod*, testcase.number_of_attributes + 1);

            int i = 0;
            for (; i < testcase.number_of_attributes; ++i)
            {
                char* name = testcase.attributes[i].name;
                char** value = testcase.attributes[i].value;

                attrs[i] = talloc(talloc_ctx, LDAPMod);

                attrs[i]->mod_op = LDAP_MOD_REPLACE;
                attrs[i]->mod_type = talloc_strndup(talloc_ctx, name, strlen(name));
                attrs[i]->mod_values = talloc_array(talloc_ctx, char*, VALUE_ATTRIBUTES_SIZE);

                info("Attribute to modify: %s with index %d \n", attrs[i]->mod_type, i);

                for (int index = 0; index < VALUE_ATTRIBUTES_SIZE; ++index)
                {
                    if (!value[index])
                    {
                        attrs[i]->mod_values[index] = NULL;

                        info("Attribute value[%d]: %s \n", index, attrs[i]->mod_values[index]);

                        break;
                    }
                    attrs[i]->mod_values[index] = talloc_strndup(talloc_ctx, value[index], strlen(value[index]));

                    info("Attribute value[%d]: %s \n", index, attrs[i]->mod_values[index]);
                }
            }
            attrs[testcase.number_of_attributes] = NULL;

            enum OperationReturnCode rc = modify(connection, testcase.entry_dn, attrs);

            talloc_free(talloc_ctx);

            assert_that(rc, is_equal_to(testcase.desired_test_result));
            test_status(testcase);
        }

        ld_install_handler(connection->handle, connection_on_add_message, CONNECTION_UPDATE_INTERVAL);
    }

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_ERROR)
    {
        verto_break(ctx);

        fail_test("Error encountered during bind\n");
    }
}

Ensure(Cgreen, entry_modify_test)
{
    start_test(connection_on_timeout, CONNECTION_UPDATE_INTERVAL, &current_directory_type);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, entry_modify_test);
    return run_test_suite(suite, create_text_reporter());
}
