#include "ldap_syntax_tests.h"
#include <ldap_syntaxes.h>

#define number_of_elements(x)  (sizeof(x) / sizeof((x)[0]))

static const char* VALID_VALUES[] =
{
    "",
    "hello world"
};
static const int NUMBER_OF_VALID_VALUES = number_of_elements(VALID_VALUES);

static const char* INVALID_VALUES[] =
{
    NULL,
};
static const int NUMBER_OF_INVALID_VALUES = number_of_elements(INVALID_VALUES);

Ensure(validate_ia5_string_returns_true_on_valid_values) {
    for (int i = 0; i < NUMBER_OF_VALID_VALUES; ++i)
    {
        bool rc = validate_ia5_string(VALID_VALUES[i]);

        assert_that(rc, is_true);
    }
}

Ensure(validate_ia5_string_returns_false_on_invalid_values) {
    for (int i = 0; i < NUMBER_OF_INVALID_VALUES; ++i)
    {
        bool rc = validate_ia5_string(INVALID_VALUES[i]);

        assert_that(rc, is_false);
    }
}

TestSuite* ia5string_test_suite()
{
    TestSuite *suite = create_test_suite();
    add_test(suite, validate_ia5_string_returns_true_on_valid_values);
    add_test(suite, validate_ia5_string_returns_false_on_invalid_values);
    return suite;
}
