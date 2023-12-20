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
    { "Integer - Positive Test #1: Minimum integer value", "-2147483648" },
    { "Integer - Positive Test #2: Maximum integer value", "2147483647" },
    { "Integer - Positive Test #3: Zero", "0" },
    { "Integer - Positive Test #4: Random integer value", "123456789" },
};
static const int NUMBER_OF_VALID_VALUES = number_of_elements(VALID_VALUES);

static const testcase_t INVALID_VALUES[] =
{
    { "Integer - Negative Test #1: NULL value", NULL },
    { "Integer - Negative Test #2: Empty string", "" },
    { "Integer - Negative Test #3: Unsigned integer max", "4294967295" },
    { "Integer - Negative Test #4: Below minimum integer value", "-2147483649" },
    { "Integer - Negative Test #4: Leading zero", "0214748364" },
};
static const int NUMBER_OF_INVALID_VALUES = number_of_elements(INVALID_VALUES);

Ensure(validate_integer_returns_true_on_valid_values) {
    for (int i = 0; i < NUMBER_OF_VALID_VALUES; ++i)
    {
        bool rc = validate_integer(VALID_VALUES[i].value);

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

Ensure(validate_integer_returns_false_on_invalid_values) {
    for (int i = 0; i < NUMBER_OF_INVALID_VALUES; ++i)
    {
        bool rc = validate_integer(INVALID_VALUES[i].value);

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

TestSuite* integer_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, validate_integer_returns_true_on_valid_values);
    add_test(suite, validate_integer_returns_false_on_invalid_values);
    return suite;
}
