#include "entry_utils.h"
#include <domain.h>
#include <entry.h>
#include <entry_p.h>
#include <talloc.h>

Ensure(returns_null_when_entry_is_null)
{
    LDAPAttribute_t *attribute = ld_entry_get_attribute(NULL, "attribute");
    assert_that(attribute, is_null);
}

Ensure(returns_null_when_attribute_does_not_exist)
{
    TALLOC_CTX *ctx = talloc_new(NULL);
    const char* dn = "cn=test,dc=domain,dc=alt";

    ld_entry_t* entry = ld_entry_new(ctx, dn);

    LDAPAttribute_t *attribute = ld_entry_get_attribute(entry, "attribute");
    assert_that(attribute, is_null);

    talloc_free(ctx);
}

Ensure(returns_attribute_when_it_exists)
{
    TALLOC_CTX *ctx = talloc_new(NULL);
    const char* dn = "cn=test,dc=domain,dc=alt";

    ld_entry_t* entry = ld_entry_new(ctx, dn);

    LDAPAttribute_t *expected_attribute = talloc(ctx, LDAPAttribute_t);
    g_hash_table_insert(entry->attributes, "attribute", expected_attribute);

    LDAPAttribute_t *attribute = ld_entry_get_attribute(entry, "attribute");
    assert_that(attribute, is_equal_to(expected_attribute));

    talloc_free(ctx);
}

TestSuite*
entry_get_attribute_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, returns_null_when_entry_is_null);
    add_test(suite, returns_null_when_attribute_does_not_exist);
    add_test(suite, returns_attribute_when_it_exists);
    return suite;
}
