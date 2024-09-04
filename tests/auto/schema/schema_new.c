#include "schema_tests.h"

#include <talloc.h>

#include <schema.h>
#include <schema_p.h>

#include <cgreen/cgreen.h>

Ensure(returns_schema_on_successful_allocation) {
    TALLOC_CTX *ctx = talloc_new(NULL);

    struct ldap_schema_t *schema = ldap_schema_new(ctx);

    assert_that(schema, is_not_equal_to(NULL));
    assert_that(schema->attribute_types_by_oid, is_not_equal_to(NULL));
    assert_that(schema->attribute_types_by_name, is_not_equal_to(NULL));
    assert_that(schema->object_classes_by_oid, is_not_equal_to(NULL));
    assert_that(schema->object_classes_by_name, is_not_equal_to(NULL));

    talloc_free(ctx);
}

Ensure(returns_null_on_null_talloc_context) {
    struct ldap_schema_t *schema = ldap_schema_new(NULL);

    assert_that(schema, is_equal_to(NULL));
}

TestSuite*
schema_new_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, returns_schema_on_successful_allocation);
    add_test(suite, returns_null_on_null_talloc_context);
    return suite;
}
