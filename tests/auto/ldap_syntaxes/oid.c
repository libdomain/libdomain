#include "ldap_syntax_tests.h"
#include <ldap_syntaxes.h>
#include <common.h>

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

typedef struct testcase_s
{
    char* name;
    char* value;
} testcase_t;

static const testcase_t VALID_VALUES[] =
{
    { "OID - Positive Test #1: Long OID", "1.3.6.1.4.1.1466.109.114.2" },
    { "OID - Positive Test #2: Syntax OID", "2.5.13.5" },
};
static const int NUMBER_OF_VALID_VALUES = number_of_elements(VALID_VALUES);

static const testcase_t INVALID_VALUES[] =
{
    { "OID - Negative Test #1: NULL value", NULL },
    { "OID - Negative Test #2: Empty string", "" },
    { "OID - Negative Test #3: Double dot", "1.1..1" },
    { "OID - Negative Test #4: Character", "1.1.a.1" },
};
static const int NUMBER_OF_INVALID_VALUES = number_of_elements(INVALID_VALUES);

Ensure(validate_oid_returns_true_on_valid_values) {
    for (int i = 0; i < NUMBER_OF_VALID_VALUES; ++i)
    {
        bool rc = validate_oid(VALID_VALUES[i].value);

        if (rc != true)
        {
            error("%s - Failed.\n", VALID_VALUES[i].name);
        }
        else
        {
            info("%s - Passed.\n", VALID_VALUES[i].name);
        }

        assert_that(rc, is_true);
    }
}

Ensure(validate_oid_returns_false_on_invalid_values) {
    for (int i = 0; i < NUMBER_OF_INVALID_VALUES; ++i)
    {
        bool rc = validate_oid(INVALID_VALUES[i].value);

        if (rc != false)
        {
            error("%s - Failed.\n", INVALID_VALUES[i].name);
        }
        else
        {
            info("%s - Passed.\n", INVALID_VALUES[i].name);
        }

        assert_that(rc, is_false);
    }
}

TestSuite* oid_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, validate_oid_returns_true_on_valid_values);
    add_test(suite, validate_oid_returns_false_on_invalid_values);
    return suite;
}
