#include "ldap_syntax_tests.h"
#include <ldap_syntaxes.h>
#include <common.h>

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

static const char* VALID_VALUES[] =
{
    "TRUE",
    "FALSE",
    "True",
    "False",
    "true",
    "false",
    "TrUe",
    "FaLsE"
};
static const int NUMBER_OF_VALID_VALUES = number_of_elements(VALID_VALUES);

static const char* INVALID_VALUES[] =
{
    NULL,
    "1231231",
    "abcd"
};
static const int NUMBER_OF_INVALID_VALUES = number_of_elements(INVALID_VALUES);

Ensure(validate_boolean_returns_true_on_valid_values) {
    for (int i = 0; i < NUMBER_OF_VALID_VALUES; ++i)
    {
        bool rc = validate_boolean(VALID_VALUES[i]);

        if (rc != true)
        {
            error("validate_boolean_returns_true_on_valid_values - failed case %s.\n", VALID_VALUES[i]);
        }

        assert_that(rc, is_true);
    }
}

Ensure(validate_boolean_returns_false_on_invalid_values) {
    for (int i = 0; i < NUMBER_OF_INVALID_VALUES; ++i)
    {
        bool rc = validate_boolean(INVALID_VALUES[i]);

        if (rc != false)
        {
            error("validate_boolean_returns_false_on_invalid_values - failed case %s.\n", INVALID_VALUES[i]);
        }

        assert_that(rc, is_false);
    }
}

TestSuite* boolean_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, validate_boolean_returns_true_on_valid_values);
    add_test(suite, validate_boolean_returns_false_on_invalid_values);
    return suite;
}
