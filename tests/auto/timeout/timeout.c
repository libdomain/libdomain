#include <cgreen/cgreen.h>

#include <common.h>
#include <domain.h>
#include <domain_p.h>
#include <connection.h>
#include <talloc.h>

#include <ldap.h>

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

    memset(ctx, 0, sizeof(context_t));

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

Ensure(Cgreen, connection_set_timeout_test) {
    struct context_t* ctx = create_context();

    ctx->config.use_sasl = true;

    ctx->config.sasl_options = talloc(ctx->global_ctx.talloc_ctx, struct ldap_sasl_options_t);
    ctx->config.sasl_options->mechanism = "SIMPLE";
    ctx->config.sasl_options->passwd = NULL;

    ctx->config.sasl_options->sasl_nocanon = true;
    ctx->config.sasl_options->sasl_secprops = "minssf=56";
    ctx->config.sasl_options->sasl_flags = LDAP_SASL_QUIET;

    ctx->config.network_timeout = 1000;

    enum OperationReturnCode rc = connection_configure(&ctx->global_ctx, &ctx->connection_ctx, &ctx->config);
    assert_that(rc, is_equal_to(RETURN_CODE_SUCCESS));

    struct timeval* network_timeout = NULL;

    ldap_get_option(ctx->connection_ctx.ldap, LDAP_OPT_NETWORK_TIMEOUT, &network_timeout);

    assert_that(network_timeout, is_non_null);

    if (network_timeout)
    {
        assert_that(ctx->config.network_timeout, is_equal_to(network_timeout->tv_usec));
        ld_info("LDAP_OPT_NETWORK_TIMEOUT - is %d\n", network_timeout->tv_usec);
        ldap_memfree(network_timeout);
    }
    else
    {
        fail_test("Unable to get network timeout!\n");
    }
    ld_info("connection_set_timeout_test - %s\n", rc == RETURN_CODE_SUCCESS ? "successful" : "failure");

    talloc_free(ctx->config.sasl_options);

    destroy_context(ctx);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, connection_set_timeout_test);
    return run_test_suite(suite, create_text_reporter());
}
