#include "schema_tests.h"

#include <stdbool.h>

#include <talloc.h>
#include <ldap.h>
#include <ldap_schema.h>

#include <schema.h>
#include <schema_p.h>

#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

Ensure(returns_true_on_successful_append_of_objectclass) {
    TALLOC_CTX *ctx = talloc_new(NULL);

    struct ldap_schema_t *schema = ldap_schema_new(ctx);
    LDAPObjectClass *objectclass = talloc_zero(ctx, LDAPObjectClass);

    bool result = ldap_schema_append_objectclass(schema, objectclass);

    assert_that(result, is_equal_to(true));
    assert_that(ldap_schema_object_classes(schema)[0], is_equal_to(objectclass));

    talloc_free(ctx);
}

Ensure(returns_true_on_successful_append_to_full_capacity_to_objectclasses) {
    TALLOC_CTX *ctx = talloc_new(NULL);

    struct ldap_schema_t *schema = ldap_schema_new(ctx);

    LDAPObjectClass *objectclass = talloc_zero(ctx, LDAPObjectClass);

    bool result = ldap_schema_append_objectclass(schema, objectclass);

    assert_that(result, is_equal_to(true));
    assert_that(ldap_schema_object_classes(schema)[0], is_equal_to(objectclass));

    result = ldap_schema_append_objectclass(schema, objectclass);

    assert_that(result, is_equal_to(true));
    assert_that(ldap_schema_object_classes(schema)[1], is_equal_to(objectclass));

    talloc_free(ctx);
}

Ensure(returns_false_on_null_schema_with_valid_objectclass) {
    TALLOC_CTX *ctx = talloc_new(NULL);
    LDAPObjectClass *objectclass = talloc_zero(ctx, LDAPObjectClass);

    bool result = ldap_schema_append_objectclass(NULL, objectclass);

    assert_that(result, is_equal_to(false));  // Assuming that the function returns true on a null schema

    talloc_free(ctx);
}

Ensure(returns_false_on_null_objectclass_with_valid_schema) {
    TALLOC_CTX *ctx = talloc_new(NULL);

    struct ldap_schema_t *schema = ldap_schema_new(ctx);

    bool result = ldap_schema_append_objectclass(schema, NULL);

    assert_that(result, is_equal_to(false));  // Assuming that the function returns true on a null object class

    talloc_free(ctx);
}

TestSuite*
schema_objectclass_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, returns_true_on_successful_append_of_objectclass);
    add_test(suite, returns_true_on_successful_append_to_full_capacity_to_objectclasses);
    add_test(suite, returns_false_on_null_schema_with_valid_objectclass);
    add_test(suite, returns_false_on_null_objectclass_with_valid_schema);
    return suite;
}
