#include "entry_utils.h"
#include <domain.h>
#include <entry.h>
#include <entry_p.h>
#include <talloc.h>

Ensure(returns_failure_when_entry_is_null)
{
    TALLOC_CTX *ctx = talloc_new(NULL);
    LDAPAttribute_t *attr = talloc(ctx, LDAPAttribute_t);

    attr->name = "attr";
    attr->values = NULL;

    ld_entry_t *entry = NULL;

    assert_that(ld_entry_add_attribute(entry, attr), is_equal_to(RETURN_CODE_FAILURE));

    talloc_free(ctx);
}

Ensure(returns_failure_when_attr_is_null)
{
    TALLOC_CTX *ctx = talloc_new(NULL);
    const char* dn = "cn=test,dc=domain,dc=alt";

    ld_entry_t* entry = ld_entry_new(ctx, dn);

    LDAPAttribute_t *attr = NULL;

    assert_that(ld_entry_add_attribute(entry, attr), is_equal_to(RETURN_CODE_FAILURE));

    talloc_free(ctx);
}

Ensure(returns_failure_when_attr_name_is_null)
{
    TALLOC_CTX *ctx = talloc_new(NULL);
    const char* dn = "cn=test,dc=domain,dc=alt";

    ld_entry_t* entry = ld_entry_new(ctx, dn);

    LDAPAttribute_t *attr = talloc(ctx, LDAPAttribute_t);
    attr->name = NULL;

    assert_that(ld_entry_add_attribute(entry, attr), is_equal_to(RETURN_CODE_FAILURE));

    talloc_free(ctx);
}

Ensure(returns_success_when_attr_is_added)
{
    TALLOC_CTX *ctx = talloc_new(NULL);
    const char* dn = "cn=test,dc=domain,dc=alt";
    ld_entry_t* entry = ld_entry_new(ctx, dn);

    LDAPAttribute_t *attr = talloc(ctx, LDAPAttribute_t);
    attr->name = "attr";

    assert_that(ld_entry_add_attribute(entry, attr), is_equal_to(RETURN_CODE_SUCCESS));

    talloc_free(ctx);
}

TestSuite*
entry_add_attribute_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, returns_failure_when_entry_is_null);
    add_test(suite, returns_failure_when_attr_name_is_null);
    add_test(suite, returns_failure_when_attr_name_is_null);
    add_test(suite, returns_success_when_attr_is_added);
    return suite;
}
