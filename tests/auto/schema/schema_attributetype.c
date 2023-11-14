#include "schema_tests.h"

#include <stdbool.h>

#include <talloc.h>
#include <ldap.h>
#include <ldap_schema.h>

#include <schema.h>
#include <schema_p.h>

#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

Ensure(returns_true_on_successful_append_of_attribute) {
    TALLOC_CTX *ctx = talloc_new(NULL);

    struct ldap_schema_t *schema = ldap_schema_new(ctx);
    LDAPAttributeType* attribute = talloc_zero(ctx, LDAPAttributeType);

    bool result = ldap_schema_append_attributetype(schema, attribute);

    assert_that(result, is_equal_to(true));
    assert_that(ldap_schema_attribute_types(schema)[0], is_equal_to(attribute));

    talloc_free(ctx);
}

Ensure(returns_true_on_successful_append_to_full_capacity_to_attributetypes) {
    TALLOC_CTX *ctx = talloc_new(NULL);

    struct ldap_schema_t *schema = ldap_schema_new(ctx);
    schema->attribute_types_capacity = 1;
    LDAPAttributeType *attribute = talloc_zero(ctx, LDAPAttributeType);

    bool result = ldap_schema_append_attributetype(schema, attribute);

    assert_that(result, is_equal_to(true));
    assert_that(ldap_schema_attribute_types(schema)[0], is_equal_to(attribute));

    result = ldap_schema_append_attributetype(schema, attribute);

    assert_that(result, is_equal_to(true));
    assert_that(ldap_schema_attribute_types(schema)[1], is_equal_to(attribute));

    talloc_free(ctx);
}

Ensure(returns_false_on_null_schema_with_valid_attributetype) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    LDAPAttributeType *attribute = talloc_zero(ctx, LDAPAttributeType);

    bool result = ldap_schema_append_attributetype(NULL, attribute);

    assert_that(result, is_equal_to(false));  // Assuming that the function returns false on a null schema

    talloc_free(ctx);
}

Ensure(returns_false_on_null_attributetype_with_valid_schema) {
    TALLOC_CTX *ctx = talloc_new(NULL);

    struct ldap_schema_t *schema = ldap_schema_new(ctx);

    bool result = ldap_schema_append_attributetype(schema, NULL);

    assert_that(result, is_equal_to(false));  // Assuming that the function returns false on a null attribute type

    talloc_free(ctx);
}

TestSuite*
schema_attributetype_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, returns_true_on_successful_append_of_attribute);
    add_test(suite, returns_true_on_successful_append_to_full_capacity_to_attributetypes);
    add_test(suite, returns_false_on_null_schema_with_valid_attributetype);
    add_test(suite, returns_false_on_null_attributetype_with_valid_schema);
    return suite;
}
