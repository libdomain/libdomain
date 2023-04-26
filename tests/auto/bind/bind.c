#include <cgreen/cgreen.h>

#include <connection.h>
#include <talloc.h>

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

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

    ctx->config.server = "ldap://dc0.domain.alt:389";
    ctx->config.port = 389;
    ctx->config.protocol_verion = LDAP_VERSION3;

    ctx->config.use_sasl = false;
    ctx->config.use_start_tls = false;
    ctx->config.chase_referrals = false;

    return ctx;
}

static void destroy_context(struct context_t* ctx)
{
    connection_close(&ctx->connection_ctx);
    talloc_free(ctx->global_ctx.talloc_ctx);
    free(ctx);
}

Ensure(Cgreen, connection_sasl_bind_test) {
    struct context_t* ctx = create_context();

    ctx->config.use_sasl = true;

    const char* passwd = "";

    ctx->config.sasl_options = talloc(ctx->global_ctx.talloc_ctx, struct ldap_sasl_options_t);
    ctx->config.sasl_options->mechanism = "GSSAPI";
    ctx->config.sasl_options->passwd = passwd;

    ctx->config.sasl_options->sasl_nocanon = true;
    ctx->config.sasl_options->sasl_secprops = "maxssf=56";
    ctx->config.sasl_options->sasl_flags = LDAP_SASL_QUIET | LDAP_AUTH_NEGOTIATE;
    ctx->connection_ctx.ldap_params = talloc(ctx->global_ctx.talloc_ctx, struct ldap_sasl_params_t);
    ctx->connection_ctx.ldap_params->dn = "";
    ctx->connection_ctx.ldap_params->passwd = talloc(ctx->global_ctx.talloc_ctx, struct berval);
    ctx->connection_ctx.ldap_params->passwd->bv_len = 0;
    ctx->connection_ctx.ldap_params->passwd->bv_val = NULL;
    ctx->connection_ctx.ldap_params->clientctrls = talloc_array(ctx->global_ctx.talloc_ctx, LDAPControl*, 1);
    ctx->connection_ctx.ldap_params->clientctrls[0] = NULL;
    ctx->connection_ctx.ldap_params->serverctrls = talloc_array(ctx->global_ctx.talloc_ctx, LDAPControl*, 1);
    ctx->connection_ctx.ldap_params->serverctrls[0] = NULL;

    int rc = RETURN_CODE_FAILURE;

    int debug_level = 0x0001 + 0x0002 + 0x0004 + 0x0008;
    ldap_set_option(ctx->connection_ctx.ldap, LDAP_OPT_DEBUG_LEVEL, &debug_level);

    rc = connection_configure(&ctx->global_ctx, &ctx->connection_ctx, &ctx->config);
    assert_that(rc, is_equal_to(RETURN_CODE_SUCCESS));

    rc = connection_sasl_bind(&ctx->connection_ctx);
    assert_that(rc, is_equal_to(RETURN_CODE_SUCCESS));

    if(event_add(ctx->connection_ctx.read_event, NULL) < 0)
    {
        fprintf(stderr, "event_add() failed");
    }

    if(event_base_loop(ctx->connection_ctx.base, EVLOOP_ONCE) < 0)
    {
        fprintf(stderr, "event_base_dispatch() failed");
    }

    assert_that(ctx->connection_ctx.ldap_defaults, is_not_null);

    talloc_free(ctx->config.sasl_options);

    destroy_context(ctx);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, connection_sasl_bind_test);
    return run_test_suite(suite, create_text_reporter());
}
