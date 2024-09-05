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
    attribute->at_names = talloc_array(ctx, char*, 2);
    attribute->at_names[0] = "test_attribute_name";
    attribute->at_names[1] = NULL;
    attribute->at_oid = "test_attribute_oid";

    bool result = ldap_schema_append_attributetype(schema, attribute);

    assert_that(result, is_equal_to(true));
    assert_that(ldap_schema_attribute_types(schema)[0], is_equal_to(attribute));
    assert_that(ldap_schema_get_attributetype_by_oid(schema, "test_attribute_oid"), is_equal_to(attribute));
    assert_that(ldap_schema_get_attributetype_by_name(schema, "test_attribute_name"), is_equal_to(attribute));

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

Ensure(get_attributetype_with_valid_schema) {
    TALLOC_CTX *ctx = talloc_new(NULL);

    struct ldap_schema_t *schema = ldap_schema_new(ctx);

    LDAPAttributeType** attributes = talloc_array(ctx, LDAPAttributeType*, 10);

    for (int i = 0; i < 10; ++i)
    {
        attributes[i] = talloc_zero(ctx, LDAPAttributeType);
        attributes[i]->at_names = talloc_array(ctx, char*, 4);

        char* first_name_buffer = malloc(30);
        sprintf(first_name_buffer, "test_attribute_first_name_%d", i);
        attributes[i]->at_names[0] = first_name_buffer;

        char* second_name_buffer = malloc(30);
        sprintf(second_name_buffer, "test_attribute_second_name_%d", i);
        attributes[i]->at_names[1] = second_name_buffer;

        char* third_name_buffer = malloc(30);
        sprintf(third_name_buffer, "test_attribute_third_name_%d", i);
        attributes[i]->at_names[2] = third_name_buffer;

        attributes[i]->at_names[3] = NULL;

        char* oid_buffer = malloc(30);
        sprintf(oid_buffer, "test_attribute_oid_%d", i);
        attributes[i]->at_oid = oid_buffer;

        ldap_schema_append_attributetype(schema, attributes[i]);
    }

    for (int i = 0; i < 10; ++i)
    {
        LDAPAttributeType* result = ldap_schema_get_attributetype_by_oid(schema, attributes[i]->at_oid);
        assert_that(result, is_equal_to(attributes[i]));

        for (int j = 0; j < 3; ++j)
        {
            result = ldap_schema_get_attributetype_by_name(schema, attributes[i]->at_names[j]);
            assert_that(result, is_equal_to(attributes[i]));
        }
    }

    talloc_free(ctx);
}

Ensure(get_attributetype_with_valid_schema_and_invalid_name_or_oid)
{
    TALLOC_CTX *ctx = talloc_new(NULL);

    struct ldap_schema_t *schema = ldap_schema_new(ctx);

    LDAPAttributeType *attribute = talloc_zero(ctx, LDAPAttributeType);

    attribute->at_names = talloc_array(ctx, char*, 2);
    attribute->at_names[0] = "test_attribute_name";
    attribute->at_names[1] = NULL;
    attribute->at_oid = "test_attribute_oid";

    ldap_schema_append_attributetype(NULL, attribute);

    LDAPAttributeType* result = ldap_schema_get_attributetype_by_name(schema, "wrong_attribute_name");

    assert_that(result, is_equal_to(NULL));

    result = ldap_schema_get_attributetype_by_oid(schema, "wrong_attribute_oid");

    assert_that(result, is_equal_to(NULL));

    talloc_free(ctx);
}

Ensure(get_attributetype_with_invalid_schema)
{
    LDAPAttributeType* result = ldap_schema_get_attributetype_by_name(NULL, "test_attribute_name");

    assert_that(result, is_equal_to(NULL));

    result = ldap_schema_get_attributetype_by_oid(NULL, "test_attribute_oid");

    assert_that(result, is_equal_to(NULL));
}

TestSuite*
schema_attributetype_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, returns_true_on_successful_append_of_attribute);
    add_test(suite, returns_false_on_null_schema_with_valid_attributetype);
    add_test(suite, returns_false_on_null_attributetype_with_valid_schema);
    add_test(suite, get_attributetype_with_valid_schema);
    add_test(suite, get_attributetype_with_valid_schema_and_invalid_name_or_oid);
    add_test(suite, get_attributetype_with_invalid_schema);
    return suite;
}
