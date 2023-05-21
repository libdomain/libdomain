#include <cgreen/cgreen.h>

#include <connection.h>
#include <connection_state_machine.h>
#include <entry.h>
#include <talloc.h>

#include <test_common.h>

const int LDAP_DEBUG_ANY = -1;
const int BUFFER_SIZE = 80;

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

char* LDAP_DIRECTORY_ATTRS[] = { "objectClass", NULL };
#define VALUE_ATTRIBUTES_SIZE 5

typedef struct attribute_value_pair_s
{
    char* name;
    char* value[VALUE_ATTRIBUTES_SIZE];
} attribute_value_pair_t;

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

attribute_value_pair_t LDAP_TEST_USER_ATTRIBUTES[] =
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

typedef struct context_t
{
    struct ldap_global_context_t global_ctx;
    struct ldap_connection_ctx_t connection_ctx;
    struct ldap_connection_config_t config;
} context_t;

static struct context_t* create_context()
{
    struct context_t* ctx = malloc(sizeof(context_t));
    assert_that(ctx, is_non_null);

    ctx->global_ctx.global_ldap = NULL;
    ctx->global_ctx.talloc_ctx = talloc_new(NULL);
    assert_that(ctx->global_ctx.talloc_ctx, is_non_null);

    memset(&ctx->connection_ctx, 0, sizeof(ldap_connection_ctx_t));

    char *envvar = "LDAP_SERVER";
    char *server = get_environment_variable(ctx->global_ctx.talloc_ctx, envvar);

    ctx->config.server = server;
    ctx->config.port = 389;
    ctx->config.protocol_verion = LDAP_VERSION3;

    ctx->config.use_sasl = false;
    ctx->config.use_start_tls = false;
    ctx->config.chase_referrals = false;

    return ctx;
}

static void destroy_context(struct context_t* ctx)
{
    if (ctx->connection_ctx.ldap_defaults)
    {
        if (ctx->connection_ctx.ldap_defaults->authcid)
        {
            ldap_memfree(ctx->connection_ctx.ldap_defaults->authcid);
        }

        if (ctx->connection_ctx.ldap_defaults->authzid)
        {
            ldap_memfree(ctx->connection_ctx.ldap_defaults->authzid);
        }

        if (ctx->connection_ctx.ldap_defaults->realm)
        {
            ldap_memfree(ctx->connection_ctx.ldap_defaults->realm);
        }
    }

    connection_close(&ctx->connection_ctx);
    talloc_free(ctx->global_ctx.talloc_ctx);
    free(ctx);
}

static void connection_on_delete_message(verto_ctx *ctx, verto_ev *ev)
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

        delete(connection, "cn=adam,ou=users,dc=domain,dc=alt");

        verto_add_timeout(ctx, VERTO_EV_FLAG_PERSIST, connection_on_delete_message, CONNECTION_UPDATE_INTERVAL);
    }

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_ERROR)
    {
        verto_break(ctx);

        fail_test("Error encountered during bind\n");
    }
}

Ensure(Cgreen, entry_delete_test) {
    struct context_t* ctx = create_context();

    ctx->config.use_sasl = true;

    ctx->config.sasl_options = talloc(ctx->global_ctx.talloc_ctx, struct ldap_sasl_options_t);
    ctx->config.sasl_options->mechanism = LDAP_SASL_SIMPLE;
    ctx->config.sasl_options->passwd = "password";

    ctx->config.sasl_options->sasl_nocanon = true;
    ctx->config.sasl_options->sasl_secprops = "maxssf=56";
    ctx->config.sasl_options->sasl_flags = LDAP_SASL_QUIET;
    ctx->connection_ctx.ldap_params = talloc(ctx->global_ctx.talloc_ctx, struct ldap_sasl_params_t);
    ctx->connection_ctx.ldap_params->dn = "cn=admin,dc=domain,dc=alt";
    ctx->connection_ctx.ldap_params->passwd = talloc(ctx->global_ctx.talloc_ctx, struct berval);
    ctx->connection_ctx.ldap_params->passwd->bv_len = strlen(ctx->config.sasl_options->passwd);
    ctx->connection_ctx.ldap_params->passwd->bv_val = strdup(ctx->config.sasl_options->passwd);
    ctx->connection_ctx.ldap_params->clientctrls = NULL;
    ctx->connection_ctx.ldap_params->serverctrls = NULL;

    int rc = RETURN_CODE_FAILURE;

    int debug_level = LDAP_DEBUG_ANY;
    ldap_set_option(ctx->connection_ctx.ldap, LDAP_OPT_DEBUG_LEVEL, &debug_level);

    rc = connection_configure(&ctx->global_ctx, &ctx->connection_ctx, &ctx->config);
    assert_that(rc, is_equal_to(RETURN_CODE_SUCCESS));

    verto_ev* ev = verto_add_timeout(ctx->connection_ctx.base, VERTO_EV_FLAG_PERSIST, connection_on_timeout,
                                     CONNECTION_UPDATE_INTERVAL);
    verto_set_private(ev, &ctx->connection_ctx, NULL);

    verto_run(ctx->connection_ctx.base);

    assert_that(ctx->connection_ctx.ldap_defaults, is_not_null);

    talloc_free(ctx->config.sasl_options);

    destroy_context(ctx);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, entry_delete_test);
    return run_test_suite(suite, create_text_reporter());
}
