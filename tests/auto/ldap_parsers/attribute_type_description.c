#include "ldap_parsers_tests.h"
#include <ldap_parsers.h>
#include <ldap_schema.h>
#include <common.h>

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

static const char* VALID_VALUES[] =
{
    "attributeTypes: ( 1.2.840.113556.1.4.965 NAME 'mSMQSiteName' SYNTAX '1.2.840.113556.1.4.905' SINGLE-VALUE )",
    "attributeTypes: ( 1.2.840.113556.1.2.26 NAME 'rDNAttID' SYNTAX '1.3.6.1.4.1.1466.115.121.1.38' SINGLE-VALUE NO-USER-MODIFICATION )",
    "attributeTypes: ( 1.2.840.113556.1.4.2195 NAME 'msDS-AppliesToResourceTypes' SYNTAX '1.3.6.1.4.1.1466.115.121.1.15' )",
    "attributeTypes: ( 1.2.840.113556.1.4.760 NAME 'aCSAggregateTokenRatePerUser' SYNTAX '1.2.840.113556.1.4.906' SINGLE-VALUE )",
    "attributeTypes: ( 1.2.840.113556.1.4.367 NAME 'rpcNsCodeset' SYNTAX '1.3.6.1.4.1.1466.115.121.1.15' )",
    "attributeTypes: ( 1.2.840.113556.1.4.705 NAME ( 'dhcpSubnets' 'subnets' ) SYNTAX '1.3.6.1.4.1.1466.115.121.1.44' )",
    "attributeTypes: ( 1.2.840.113556.1.4.274 NAME 'printSpooling' SYNTAX '1.3.6.1.4.1.1466.115.121.1.15' SINGLE-VALUE )",
    "attributeTypes: ( 1.2.840.113556.1.4.1782 NAME 'msDS-KeyVersionNumber' SYNTAX '1.3.6.1.4.1.1466.115.121.1.27' SINGLE-VALUE NO-USER-MODIFICATION )",
};
static const int NUMBER_OF_VALID_VALUES = number_of_elements(VALID_VALUES);

static const char* INVALID_VALUES[] =
{
    NULL,
    "1231231",
    "abcd"
};
static const int NUMBER_OF_INVALID_VALUES = number_of_elements(INVALID_VALUES);

Ensure(parse_attribute_type_description_returns_attribute_type_on_valid_values)
{
    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    if (!talloc_ctx)
    {
        fail_test("parse_attribute_type_description_returns_attribute_type_on_valid_values - Unable to create talloc_ctx!\n");
    }

    for (int i = 0; i < NUMBER_OF_VALID_VALUES; ++i)
    {
        LDAPAttributeType *attribute_type = parse_attribute_type(talloc_ctx, VALID_VALUES[i]);

        if (!attribute_type)
        {
            ld_error("parse_attribute_type_description_returns_attribute_type_on_valid_values - failed case %s.\n", VALID_VALUES[i]);
        }

        assert_that(attribute_type, is_non_null);
    }

    talloc_free(talloc_ctx);
}

Ensure(parse_attribute_type_description_returns_null_on_invalid_values)
{
    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    if (!talloc_ctx)
    {
        fail_test("parse_attribute_type_description_returns_null_on_invalid_values - Unable to create talloc_ctx!\n");
    }

    for (int i = 0; i < NUMBER_OF_INVALID_VALUES; ++i)
    {
        LDAPAttributeType *attribute_type = parse_attribute_type(talloc_ctx, INVALID_VALUES[i]);

        if (attribute_type != NULL)
        {
            ld_error("parse_attribute_type_description_returns_null_on_invalid_values - failed case %s.\n", INVALID_VALUES[i]);
        }

        assert_that(attribute_type, is_null);
    }

    talloc_free(talloc_ctx);
}

TestSuite* attribute_type_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, parse_attribute_type_description_returns_attribute_type_on_valid_values);
    add_test(suite, parse_attribute_type_description_returns_null_on_invalid_values);
    return suite;
}
