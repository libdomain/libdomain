#include <cgreen/cgreen.h>

#include <connection.h>
#include <connection_state_machine.h>
#include <talloc.h>

const int LDAP_DEBUG_ANY = -1;

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

Ensure(Cgreen, connection_state_machine_init) {
    struct context_t* ctx = create_context();

    int rc = RETURN_CODE_FAILURE;
    int debug_level = LDAP_DEBUG_ANY;
    ldap_set_option(ctx->connection_ctx.ldap, LDAP_OPT_DEBUG_LEVEL, &debug_level);

    rc = connection_configure(&ctx->global_ctx, &ctx->connection_ctx, &ctx->config);
    assert_that(rc, is_equal_to(RETURN_CODE_SUCCESS));

    struct state_machine_ctx_t* csm = talloc(ctx->global_ctx.talloc_ctx, struct state_machine_ctx_t);

    rc = csm_init(csm, &ctx->connection_ctx);
    assert_that(rc, is_equal_to(RETURN_CODE_SUCCESS));

    assert_that(csm->ctx, is_equal_to(&ctx->connection_ctx));

    assert_that(csm->state, is_equal_to(LDAP_CONNECTION_STATE_INIT));

    destroy_context(ctx);
}

Ensure(Cgreen, connection_state_machine_next_state) {
    struct context_t* ctx = create_context();

    int rc = RETURN_CODE_FAILURE;
    int debug_level = LDAP_DEBUG_ANY;
    ldap_set_option(ctx->connection_ctx.ldap, LDAP_OPT_DEBUG_LEVEL, &debug_level);

    ctx->config.use_sasl = true;

    ctx->config.sasl_options = talloc(ctx->global_ctx.talloc_ctx, struct ldap_sasl_options_t);
    ctx->config.sasl_options->mechanism = "GSSAPI";
    ctx->config.sasl_options->passwd = NULL;

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

    rc = connection_configure(&ctx->global_ctx, &ctx->connection_ctx, &ctx->config);
    assert_that(rc, is_equal_to(RETURN_CODE_SUCCESS));

    struct state_machine_ctx_t* csm = talloc(ctx->global_ctx.talloc_ctx, struct state_machine_ctx_t);

    rc = csm_init(csm, &ctx->connection_ctx);
    assert_that(rc, is_equal_to(RETURN_CODE_SUCCESS));

    rc = csm_next_state(csm);
    assert_that(csm->state, is_equal_to(LDAP_CONNECTION_STATE_TRANSPORT_READY));
    assert_that(rc, is_equal_to(RETURN_CODE_SUCCESS));

    rc = csm_next_state(csm);
    assert_that(csm->state, is_equal_to(LDAP_CONNECTION_STATE_BIND_IN_PROGRESS));
    assert_that(rc, is_equal_to(RETURN_CODE_OPERATION_IN_PROGRESS));

    rc = csm_set_state(csm, LDAP_CONNECTION_STATE_BOUND);
    assert_that(rc, is_equal_to(RETURN_CODE_SUCCESS));

    rc = csm_next_state(csm);
    assert_that(csm->state, is_equal_to(LDAP_CONNECTION_STATE_RUN));
    assert_that(rc, is_equal_to(RETURN_CODE_SUCCESS));

    destroy_context(ctx);
}


Ensure(Cgreen, connection_state_machine_set_state) {
    void* talloc_ctx = talloc_new(NULL);

    struct state_machine_ctx_t* csm = talloc(talloc_ctx, struct state_machine_ctx_t);

    int rc = csm_set_state(csm, LDAP_CONNECTION_STATE_INIT);
    assert_that(csm->state, is_equal_to(LDAP_CONNECTION_STATE_INIT));
    assert_that(rc, is_equal_to(RETURN_CODE_SUCCESS));

    talloc_free(talloc_ctx);
}


int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, connection_state_machine_init);
    add_test_with_context(suite, Cgreen, connection_state_machine_next_state);
    add_test_with_context(suite, Cgreen, connection_state_machine_set_state);
    return run_test_suite(suite, create_text_reporter());
}
