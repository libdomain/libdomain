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
    { "Distinguished name - Positive Test #1: Basic", "UID=jsmith,DC=example,DC=net" },
    { "Distinguished name - Positive Test #2: Plus and dot", "OU=Sales+CN=J. Smith,DC=example,DC=net" },
    { "Distinguished name - Positive Test #3: Special characters", "CN=John Smith\\, III,DC=example,DC=net" },
    { "Distinguished name - Positive Test #4: Slash", "CN=Before\0dAfter,DC=example,DC=net" },
    { "Distinguished name - Positive Test #4: OID", "1.3.6.1.4.1.1466.0=#04024869,DC=example,DC=com" },
    { "Distinguished name - Positive Test #4: CN only", "CN=Lu\\C4\\8Di\\C4\\87" },
};
static const int NUMBER_OF_VALID_VALUES = number_of_elements(VALID_VALUES);

static const testcase_t INVALID_VALUES[] =
{
    { "Distinguished name - Negative Test #1: NULL value", NULL },
    { "Distinguished name - Negative Test #2: Empty string", "" }
};
static const int NUMBER_OF_INVALID_VALUES = number_of_elements(INVALID_VALUES);

Ensure(validate_dn_returns_true_on_valid_values) {
    for (int i = 0; i < NUMBER_OF_VALID_VALUES; ++i)
    {
        bool rc = validate_dn(VALID_VALUES[i].value);

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

Ensure(validate_dn_returns_false_on_invalid_values) {
    for (int i = 0; i < NUMBER_OF_INVALID_VALUES; ++i)
    {
        bool rc = validate_dn(INVALID_VALUES[i].value);

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

TestSuite* dn_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, validate_dn_returns_true_on_valid_values);
    add_test(suite, validate_dn_returns_false_on_invalid_values);
    return suite;
}
