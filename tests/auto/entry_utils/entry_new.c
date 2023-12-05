#include "entry_utils.h"
#include <entry.h>
#include <entry_p.h>
#include <talloc.h>


Ensure(creates_new_ld_entry_when_valid_input)
{
    TALLOC_CTX *ctx = talloc_new(NULL);
    const char* dn = "cn=test,dc=domain,dc=alt";

    ld_entry_t* result = ld_entry_new(ctx, dn);

    assert_that(result, is_not_null);
    assert_that(result->dn, is_equal_to_string(dn));
    assert_that(result->attributes, is_not_null);

    talloc_free(ctx);
}

Ensure(returns_null_when_invalid_talloc_ctx)
{
    TALLOC_CTX *ctx = NULL;
    const char* dn = "cn=test,dc=domain,dc=alt";

    ld_entry_t* result = ld_entry_new(ctx, dn);

    assert_that(result, is_null);
}

Ensure(returns_null_when_invalid_dn)
{
    TALLOC_CTX *ctx = talloc_new(NULL);
    const char* dn = NULL;

    ld_entry_t* result = ld_entry_new(ctx, dn);

    assert_that(result, is_null);

    talloc_free(ctx);
}

TestSuite*
entry_new_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, creates_new_ld_entry_when_valid_input);
    add_test(suite, returns_null_when_invalid_talloc_ctx);
    add_test(suite, returns_null_when_invalid_dn);
    return suite;
}
