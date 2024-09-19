#include <cgreen/cgreen.h>

#include <domain.h>
#include <domain_p.h>
#include <talloc.h>

#include <ldap.h>

#include <test_common.h>

Describe(Cgreen);
BeforeEach(Cgreen) {}
AfterEach(Cgreen) {}

Ensure(Cgreen, load_config_from_file) {
    TALLOC_CTX* talloc_ctx = talloc_new(NULL);

    char *valid_file_envvar = "VALID_CONFIG_FILE";
    char *valid_file = get_environment_variable(talloc_ctx, valid_file_envvar);
    assert_that(valid_file_envvar, is_non_null);

    ld_config_t* config = ld_load_config(talloc_ctx, valid_file);

    assert_that(config, is_non_null);

    if (config)
    {
        assert_that(config->host, is_equal_to_string("ldap://dc0.domain.alt"));

        assert_that(config->base_dn, is_equal_to_string("dc=domain,dc=alt"));

        assert_that(config->username, is_equal_to_string("admin"));
        assert_that(config->password, is_equal_to_string("password"));

        assert_that(config->timeout, is_equal_to(1000));

        assert_that(config->cacertfile, is_equal_to_string("CA.cert"));

        assert_that(config->certfile, is_equal_to_string("dc0.domain.alt.cert"));
        assert_that(config->keyfile, is_equal_to_string("dc0.domain.alt.key"));

        assert_that(config->protocol_version, is_equal_to(LDAP_VERSION3));
        assert_that(config->simple_bind, is_false);

        assert_that(config->use_tls, is_true);
        assert_that(config->use_sasl, is_true);
        assert_that(config->use_anon, is_false);
    }

    talloc_free(talloc_ctx);
}

int main(int argc, char **argv) {
    (void)(argc);
    (void)(argv);
    (void)(contextForCgreen);
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Cgreen, load_config_from_file);
    return run_test_suite(suite, create_text_reporter());
}
