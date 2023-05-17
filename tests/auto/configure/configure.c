#include <cgreen/cgreen.h>

#include <connection.h>
#include <talloc.h>

#include <test_common.h>

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
    connection_close(&ctx->connection_ctx);
    talloc_free(ctx->global_ctx.talloc_ctx);
    free(ctx);
}

Ensure(Cgreen, connection_configure_test) {
    struct context_t* ctx = create_context();

    connection_configure(&ctx->global_ctx, &ctx->connection_ctx, &ctx->config);

    int protocol_version = 0;
    ldap_get_option(ctx->connection_ctx.ldap, LDAP_OPT_PROTOCOL_VERSION, &protocol_version);
    assert_that(protocol_version, is_equal_to(ctx->config.protocol_verion));

    char* referrals = NULL;
    ldap_get_option(ctx->connection_ctx.ldap, LDAP_OPT_REFERRALS, &referrals);
    assert_that(referrals, is_equal_to(LDAP_OPT_OFF));

    assert_that(ctx->connection_ctx.base, is_not_equal_to(NULL));

    destroy_context(ctx);
}

Ensure(Cgreen, connection_configure_with_sasl_test) {
    struct context_t* ctx = create_context();

    ctx->config.use_sasl = true;

    ctx->config.sasl_options = talloc(ctx->global_ctx.talloc_ctx, struct ldap_sasl_options_t);
    ctx->config.sasl_options->mechanism = "SIMPLE";
    ctx->config.sasl_options->passwd = NULL;

    ctx->config.sasl_options->sasl_nocanon = true;
    ctx->config.sasl_options->sasl_secprops = "maxssf=56";
    ctx->config.sasl_options->sasl_flags = LDAP_SASL_QUIET;

    connection_configure(&ctx->global_ctx, &ctx->connection_ctx, &ctx->config);

    assert_that(ctx->connection_ctx.ldap_defaults, is_not_null);

    talloc_free(ctx->config.sasl_options);

    destroy_context(ctx);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, connection_configure_test);
    add_test_with_context(suite, Cgreen, connection_configure_with_sasl_test);
    return run_test_suite(suite, create_text_reporter());
}
