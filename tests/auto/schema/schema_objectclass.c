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
    objectclass->oc_names = talloc_array(ctx, char*, 2);
    objectclass->oc_names[0] = "test_objectclass_name";
    objectclass->oc_names[1] = NULL;
    objectclass->oc_oid = "test_objectclass_oid";

    bool result = ldap_schema_append_objectclass(schema, objectclass);

    assert_that(result, is_equal_to(true));
    assert_that(ldap_schema_object_classes(schema)[0], is_equal_to(objectclass));
    assert_that(ldap_schema_get_objectclass_by_oid(schema, "test_objectclass_oid"), is_equal_to(objectclass));
    assert_that(ldap_schema_get_objectclass_by_name(schema, "test_objectclass_name"), is_equal_to(objectclass));

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

Ensure(get_objectclass_with_valid_schema) {
    TALLOC_CTX *ctx = talloc_new(NULL);

    struct ldap_schema_t *schema = ldap_schema_new(ctx);

    LDAPObjectClass** objectclasses = talloc_array(ctx, LDAPObjectClass*, 10);

    for (int i = 0; i < 10; ++i)
    {
        objectclasses[i] = talloc_zero(ctx, LDAPObjectClass);
        objectclasses[i]->oc_names = talloc_array(ctx, char*, 4);

        char* first_name_buffer = malloc(30);
        sprintf(first_name_buffer, "test_objectclass_first_name_%d", i);
        objectclasses[i]->oc_names[0] = first_name_buffer;

        char* second_name_buffer = malloc(30);
        sprintf(second_name_buffer, "test_objectclass_second_name_%d", i);
        objectclasses[i]->oc_names[1] = second_name_buffer;

        char* third_name_buffer = malloc(30);
        sprintf(third_name_buffer, "test_objectclass_third_name_%d", i);
        objectclasses[i]->oc_names[2] = third_name_buffer;

        objectclasses[i]->oc_names[3] = NULL;

        char* oid_buffer = malloc(30);
        sprintf(oid_buffer, "test_objectclass_oid_%d", i);
        objectclasses[i]->oc_oid = oid_buffer;

        ldap_schema_append_objectclass(schema, objectclasses[i]);
    }

    for (int i = 0; i < 10; ++i)
    {
        LDAPObjectClass* result = ldap_schema_get_objectclass_by_oid(schema, objectclasses[i]->oc_oid);
        assert_that(result, is_equal_to(objectclasses[i]));

        for (int j = 0; j < 3; ++j)
        {
            result = ldap_schema_get_objectclass_by_name(schema, objectclasses[i]->oc_names[j]);
            assert_that(result, is_equal_to(objectclasses[i]));
        }
    }

    talloc_free(ctx);
}

Ensure(get_objectclass_with_valid_schema_and_invalid_name_or_oid)
{
    TALLOC_CTX *ctx = talloc_new(NULL);

    struct ldap_schema_t *schema = ldap_schema_new(ctx);

    LDAPObjectClass *objectclass = talloc_zero(ctx, LDAPObjectClass);

    objectclass->oc_names = talloc_array(ctx, char*, 2);
    objectclass->oc_names[0] = "test_objectclass_name";
    objectclass->oc_names[1] = NULL;
    objectclass->oc_oid = "test_objectclass_oid";

    ldap_schema_append_objectclass(NULL, objectclass);

    LDAPObjectClass* result = ldap_schema_get_objectclass_by_name(schema, "wrong_objectclass_name");

    assert_that(result, is_equal_to(NULL));

    result = ldap_schema_get_objectclass_by_oid(schema, "wrong_objectclass_oid");

    assert_that(result, is_equal_to(NULL));

    talloc_free(ctx);
}

Ensure(get_objectclass_with_invalid_schema)
{
    LDAPObjectClass* result = ldap_schema_get_objectclass_by_name(NULL, "test_objectclass_name");

    assert_that(result, is_equal_to(NULL));

    result = ldap_schema_get_objectclass_by_oid(NULL, "test_objectclass_oid");

    assert_that(result, is_equal_to(NULL));
}


TestSuite*
schema_objectclass_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, returns_true_on_successful_append_of_objectclass);
    add_test(suite, returns_false_on_null_schema_with_valid_objectclass);
    add_test(suite, returns_false_on_null_objectclass_with_valid_schema);
    add_test(suite, get_objectclass_with_valid_schema);
    add_test(suite, get_objectclass_with_valid_schema_and_invalid_name_or_oid);
    add_test(suite, get_objectclass_with_invalid_schema);

    return suite;
}
