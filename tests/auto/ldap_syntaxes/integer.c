#include "ldap_syntax_tests.h"
#include <ldap_syntaxes.h>
#include <common.h>

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

static const char* VALID_VALUES[] =
{
    "120394",
    "-101912",
    "0"
};
static const int NUMBER_OF_VALID_VALUES = number_of_elements(VALID_VALUES);

static const char* INVALID_VALUES[] =
{
    NULL,
    "abc",
    "0123acbv",
    "",
    "-213-12",
    "1231231-"
};
static const int NUMBER_OF_INVALID_VALUES = number_of_elements(INVALID_VALUES);

Ensure(validate_integer_returns_true_on_valid_values) {
    for (int i = 0; i < NUMBER_OF_VALID_VALUES; ++i)
    {
        bool rc = validate_integer(VALID_VALUES[i]);

        if (rc != true)
        {
            error("validate_integer_returns_true_on_valid_values - failed case %s.\n", VALID_VALUES[i]);
        }

        assert_that(rc, is_true);
    }
}

Ensure(validate_integer_returns_false_on_invalid_values) {
    for (int i = 0; i < NUMBER_OF_INVALID_VALUES; ++i)
    {
        bool rc = validate_integer(INVALID_VALUES[i]);

        if (rc != false)
        {
            error("validate_integer_returns_false_on_invalid_values - failed case %s.\n", INVALID_VALUES[i]);
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
