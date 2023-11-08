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

const int CONNECTION_UPDATE_INTERVAL = 1000;

typedef struct context_t
{
    struct ldap_global_context_t global_ctx;
    struct ldap_connection_ctx_t connection_ctx;
    struct ldap_connection_config_t config;
} context_t;

char *get_optional_environment_variable(TALLOC_CTX *talloc_ctx, const char *envvar)
{
    char *value = talloc_array(talloc_ctx, char, BUFFER_SIZE);

    if (!getenv(envvar))
    {
        return NULL;
    }

    int cx = snprintf(value, BUFFER_SIZE, "%s", getenv(envvar));

    if (cx < 0 || cx >= BUFFER_SIZE)
    {
        return NULL;
    }

    return value;
}

static struct context_t* create_context()
{
    struct context_t* ctx = malloc(sizeof(context_t));
    assert_that(ctx, is_non_null);

    ctx->global_ctx.global_ldap = NULL;
    ctx->global_ctx.talloc_ctx = talloc_new(NULL);
    assert_that(ctx->global_ctx.talloc_ctx, is_non_null);

    memset(&ctx->connection_ctx, 0, sizeof(ldap_connection_ctx_t));

    char *envvar = "LDAPS_SERVER";
    char *server = get_environment_variable(ctx->global_ctx.talloc_ctx, envvar);

    const char *ca_cert_var = "LDAP_CA_CERT";
    char *ca_cert = get_environment_variable(ctx->global_ctx.talloc_ctx, ca_cert_var);
    const char *cert_var = "LDAP_CERT";
    char *cert = get_optional_environment_variable(ctx->global_ctx.talloc_ctx, cert_var);
    const char *key_var = "LDAP_KEY";
    char *key = get_optional_environment_variable(ctx->global_ctx.talloc_ctx, key_var);

    ctx->config.server = server;
    ctx->config.port = 636;
    ctx->config.protocol_verion = LDAP_VERSION3;

    ctx->config.tls_ca_cert_file = ca_cert;
    ctx->config.tls_cert_file = cert;
    ctx->config.tls_key_file = key;

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

        search(connection, "dc=domain,dc=alt", LDAP_SCOPE_BASE, "(objectClass=*)", LDAP_DIRECTORY_ATTRS, 0);

        verto_add_timeout(ctx, VERTO_EV_FLAG_PERSIST, connection_on_search_message, CONNECTION_UPDATE_INTERVAL);
    }

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_ERROR)
    {
        verto_break(ctx);

        fail_test("Error encountered during bind\n");
    }
}

Ensure(Cgreen, tls_connection_test) {
    struct context_t* ctx = create_context();

    ctx->config.use_sasl = false;

    ctx->config.use_start_tls = true;

    ctx->config.sasl_options = talloc(ctx->global_ctx.talloc_ctx, struct ldap_sasl_options_t);
    ctx->config.sasl_options->mechanism = LDAP_SASL_SIMPLE;
    ctx->config.sasl_options->passwd = "";

    ctx->config.sasl_options->sasl_nocanon = true;
    ctx->config.sasl_options->sasl_secprops = "maxssf=56";
    ctx->config.sasl_options->sasl_flags = LDAP_SASL_QUIET;
    ctx->connection_ctx.ldap_params = talloc(ctx->global_ctx.talloc_ctx, struct ldap_sasl_params_t);
    ctx->connection_ctx.ldap_params->dn = NULL;
    ctx->connection_ctx.ldap_params->passwd = talloc(ctx->global_ctx.talloc_ctx, struct berval);
    ctx->connection_ctx.ldap_params->passwd->bv_len = 0;
    ctx->connection_ctx.ldap_params->passwd->bv_val = NULL;
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

    talloc_free(ctx->config.sasl_options);

    destroy_context(ctx);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, tls_connection_test);
    return run_test_suite(suite, create_text_reporter());
}
