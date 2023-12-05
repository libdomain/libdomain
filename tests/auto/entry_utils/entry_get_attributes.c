#include "entry_utils.h"
#include <domain.h>
#include <entry.h>
#include <entry_p.h>
#include <talloc.h>

Ensure(ld_entry_get_attributes_returns_null_when_entry_is_null)
{
    ld_entry_t *entry = NULL;
    LDAPAttribute_t **attributes = ld_entry_get_attributes(entry);
    assert_that(attributes, is_null);
}

Ensure(ld_entry_get_attributes_returns_null_when_entry_has_no_attributes)
{
    TALLOC_CTX *ctx = talloc_new(NULL);

    ld_entry_t *entry = talloc_zero(ctx, ld_entry_t);
    entry->attributes = g_hash_table_new(g_str_hash, g_str_equal);
    LDAPAttribute_t **attributes = ld_entry_get_attributes(entry);
    assert_that(attributes[0], is_null);

    talloc_free(ctx);
}

Ensure(ld_entry_get_attributes_returns_attributes_when_entry_has_attributes)
{
    TALLOC_CTX *ctx = talloc_new(NULL);

    ld_entry_t *entry = talloc_zero(ctx, ld_entry_t);

    entry->attributes = g_hash_table_new(g_str_hash, g_str_equal);

    LDAPAttribute_t *attribute = talloc_zero(entry, LDAPAttribute_t);
    attribute->name = "test";
    attribute->values = talloc_array(attribute, char*, 3);
    attribute->values[0] = talloc_strdup(attribute, "value1");
    attribute->values[1] = talloc_strdup(attribute, "value2");
    attribute->values[2] = NULL;

    g_hash_table_insert(entry->attributes, attribute->name, attribute);

    LDAPAttribute_t **attributes = ld_entry_get_attributes(entry);

    assert_that(attributes, is_not_null);
    assert_string_equal(attributes[0]->name, "test");
    assert_string_equal(attributes[0]->values[0], "value1");
    assert_string_equal(attributes[0]->values[1], "value2");

    talloc_free(ctx);
}

TestSuite *entry_get_attributes_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, ld_entry_get_attributes_returns_null_when_entry_is_null);
    add_test(suite, ld_entry_get_attributes_returns_null_when_entry_has_no_attributes);
    add_test(suite, ld_entry_get_attributes_returns_attributes_when_entry_has_attributes);
    return suite;
}
