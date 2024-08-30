#include <cgreen/cgreen.h>

#include <common.h>
#include <connection.h>
#include <connection_state_machine.h>
#include <directory.h>
#include <domain.h>
#include <domain_p.h>
#include <entry.h>
#include <talloc.h>

#include <test_common.h>

const int LDAP_DEBUG_ANY = -1;
const int BUFFER_SIZE = 80;

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

char* LDAP_DIRECTORY_ATTRS[] = { LDAP_ALL_USER_ATTRIBUTES, NULL };

const int CONNECTION_UPDATE_INTERVAL = 1000;

static int current_directory_type = LDAP_TYPE_UNKNOWN;

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

        directory_get_type(connection);

        verto_add_timeout(ctx, VERTO_EV_FLAG_PERSIST, connection_on_search_message, CONNECTION_UPDATE_INTERVAL);
    }

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_ERROR)
    {
        verto_break(ctx);

        fail_test("Error encountered during bind\n");
    }
}

Ensure(Cgreen, get_diretory_type_test) {
    struct context_t* ctx = create_context();

    char *directory_envvar = "DIRECTORY_TYPE";
    char *directory = get_environment_variable(ctx->global_ctx.talloc_ctx, directory_envvar);
    current_directory_type = get_current_directory_type(directory);

    ctx->config.use_sasl = true;

    ctx->config.sasl_options = talloc(ctx->global_ctx.talloc_ctx, struct ldap_sasl_options_t);
    ctx->config.sasl_options->mechanism = current_directory_type == LDAP_TYPE_ACTIVE_DIRECTORY ? "GSSAPI" : LDAP_SASL_SIMPLE;
    ctx->config.sasl_options->passwd = NULL;

    ctx->config.sasl_options->sasl_nocanon = true;
    ctx->config.sasl_options->sasl_secprops = "minssf=56";
    ctx->config.sasl_options->sasl_flags = LDAP_SASL_QUIET;
    ctx->connection_ctx.ldap_params = talloc(ctx->global_ctx.talloc_ctx, struct ldap_sasl_params_t);
    ctx->connection_ctx.ldap_params->dn = NULL;
    ctx->connection_ctx.ldap_params->passwd = talloc(ctx->global_ctx.talloc_ctx, struct berval);
    ctx->connection_ctx.ldap_params->passwd->bv_len = 0;
    ctx->connection_ctx.ldap_params->passwd->bv_val = NULL;
    ctx->connection_ctx.ldap_params->clientctrls = NULL;
    ctx->connection_ctx.ldap_params->serverctrls = NULL;

    ctx->connection_ctx.handle = talloc(ctx->global_ctx.talloc_ctx, struct ldhandle);
    ctx->connection_ctx.handle->talloc_ctx = ctx->global_ctx.talloc_ctx;

    int rc = RETURN_CODE_FAILURE;

    int debug_level = LDAP_DEBUG_ANY;
    ldap_set_option(ctx->connection_ctx.ldap, LDAP_OPT_DEBUG_LEVEL, &debug_level);

    rc = connection_configure(&ctx->global_ctx, &ctx->connection_ctx, &ctx->config);
    assert_that(rc, is_equal_to(RETURN_CODE_SUCCESS));

    verto_ev* ev = verto_add_timeout(ctx->connection_ctx.base, VERTO_EV_FLAG_PERSIST, connection_on_timeout,
                                     CONNECTION_UPDATE_INTERVAL);
    verto_set_private(ev, &ctx->connection_ctx, NULL);

    verto_run(ctx->connection_ctx.base);

    switch (current_directory_type)
    {
    case LDAP_TYPE_OPENLDAP:
        assert_that(ctx->connection_ctx.directory_type, is_equal_to(LDAP_TYPE_OPENLDAP));
        break;
    case LDAP_TYPE_ACTIVE_DIRECTORY:
        assert_that(ctx->connection_ctx.directory_type, is_equal_to(LDAP_TYPE_ACTIVE_DIRECTORY));
        break;
    default:
        fail_test("Unknown directory type, please check environment variables!\n");
        exit(EXIT_FAILURE);
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
    add_test_with_context(suite, Cgreen, get_diretory_type_test);
    return run_test_suite(suite, create_text_reporter());
}
