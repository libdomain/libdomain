#include "common.h"

#include <cgreen/cgreen.h>

const int BUFFER_SIZE = 80;

/**
 * @brief get_environment_variable Wrapper around getenv retrievs environment variable and stores it in current
 * talloc context.
 * @param talloc_ctx [in] global talloc context to allocate variable context on
 * @param envvar [in] name of environment variable
 * @return
 *        - pointer to variable on success.
 *        - NULL on failure.
 */
char *get_environment_variable(TALLOC_CTX *talloc_ctx, const char *envvar)
{
    char *value = talloc_array(talloc_ctx, char, BUFFER_SIZE);

    if (!getenv(envvar))
    {
        fail_test("The environment variable %s was not found.\n", envvar);
        return NULL;
    }

    int cx = snprintf(value, BUFFER_SIZE, "%s", getenv(envvar));

    if (cx < 0 || cx >= BUFFER_SIZE)
    {
        fail_test("BUFFER_SIZE of %d was too small. Aborting\n", BUFFER_SIZE);
        return NULL;
    }

    return value;
}
