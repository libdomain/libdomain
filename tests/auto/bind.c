#include <cgreen/cgreen.h>

#include <connection.h>
#include <talloc.h>

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

Ensure(Cgreen, connection_configure_test) {
    struct ldap_global_context_t global_ctx;
    global_ctx.global_ldap = NULL;
    global_ctx.talloc_ctx = talloc_new(NULL);

    struct ldap_connection_ctx_t connection_ctx;
    memset(&connection_ctx, 0, sizeof(ldap_connection_ctx_t));

    struct ldap_connection_config_t config;
    config.chase_referrals = false;

    connection_configure(&global_ctx, &connection_ctx, &config);

    assert_that(1 == 1);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, connection_configure_test);
    return run_test_suite(suite, create_text_reporter());
}
