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
    { "Large integer - Positive Test #1: Minimum large integer value", "-9223372036854775808" },
    { "Large integer - Positive Test #2: Maximum large integer value", "9223372036854775807" },
    { "Large integer - Positive Test #3: Zero", "0" },
    { "Large integer - Positive Test #4: Random large integer value", "123456789" },
};
static const int NUMBER_OF_VALID_VALUES = number_of_elements(VALID_VALUES);

static const testcase_t INVALID_VALUES[] =
{
    { "Large integer - Negative Test #1: NULL value", NULL },
    { "Large integer - Negative Test #2: Empty string", "" },
    { "Large integer - Negative Test #3: Above maximum large integer value", "9223372036854775809" },
    { "Large integer - Negative Test #4: Below minimum large integer value", "-9223372036854775809" },
    { "Large integer - Negative Test #4: Leading zero", "0214748364" },
};
static const int NUMBER_OF_INVALID_VALUES = number_of_elements(INVALID_VALUES);

Ensure(validate_large_integer_returns_true_on_valid_values) {
    for (int i = 0; i < NUMBER_OF_VALID_VALUES; ++i)
    {
        bool rc = validate_large_integer(VALID_VALUES[i].value);

        if (rc != true)
        {
            ld_error("%s - Failed.\n", VALID_VALUES[i].name);
        }
        else
        {
            ld_info("%s - Passed.\n", VALID_VALUES[i].name);
        }

        assert_that(rc, is_true);
    }
}

Ensure(validate_large_integer_returns_false_on_invalid_values) {
    for (int i = 0; i < NUMBER_OF_INVALID_VALUES; ++i)
    {
        bool rc = validate_large_integer(INVALID_VALUES[i].value);

        if (rc != false)
        {
            ld_error("%s - Failed.\n", INVALID_VALUES[i].name);
        }
        else
        {
            ld_info("%s - Passed.\n", INVALID_VALUES[i].name);
        }

        assert_that(rc, is_false);
    }
}

TestSuite* large_integer_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, validate_large_integer_returns_true_on_valid_values);
    add_test(suite, validate_large_integer_returns_false_on_invalid_values);
    return suite;
}
