#include "common.h"

#include <domain.h>
#include <directory.h>

#include <cgreen/cgreen.h>

static const int BUFFER_SIZE = 80;

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

/**
 * @brief get_current_directory_type Gets current directory type.
 * @param[in] directory_type String with directory name.
 * @return
 *        - Various directory types based on provided string.
 */
int get_current_directory_type(const char* directory_type)
{
    if (!directory_type || strlen(directory_type) == 0)
    {
        return LDAP_TYPE_UNKNOWN;
    }

    if (strcasecmp(directory_type, "AD") == 0)
    {
        return LDAP_TYPE_ACTIVE_DIRECTORY;
    }

    if (strcasecmp(directory_type, "OpenLDAP") == 0)
    {
        return LDAP_TYPE_OPENLDAP;
    }

    if (strcasecmp(directory_type, "FreeIPA") == 0)
    {
        return LDAP_TYPE_FREE_IPA;
    }

    return LDAP_TYPE_UNKNOWN;
}

static enum OperationReturnCode connection_on_error(int rc, void* unused_a, void* connection)
{
    (void)(unused_a);

    assert_that(rc, is_not_equal_to(LDAP_SUCCESS));

    verto_break(((ldap_connection_ctx_t*)connection)->base);

    fail_test("Unable to perform test!\n");

    return RETURN_CODE_SUCCESS;
}

void start_test(verto_callback *update_callback, const int update_interval, int* current_directory_type)
{
    TALLOC_CTX* talloc_ctx = talloc_new(NULL);

    char *server_envvar = "LDAP_SERVER";
    char *server = get_environment_variable(talloc_ctx, server_envvar);

    char *directory_envvar = "DIRECTORY_TYPE";
    char *directory = get_environment_variable(talloc_ctx, directory_envvar);
    (*current_directory_type) = get_current_directory_type(directory);

    ld_config_t *config = NULL;
    switch ((*current_directory_type))
    {
    case LDAP_TYPE_OPENLDAP:
        config = ld_create_config(talloc_ctx, server, 0, LDAP_VERSION3, "dc=domain,dc=alt",
                                            "admin", "password", true, false, true, false, update_interval,
                                            "", "", "");
        break;
    case LDAP_TYPE_ACTIVE_DIRECTORY:
        config = ld_create_config(talloc_ctx, server, 0, LDAP_VERSION3, "dc=domain,dc=alt",
                                            "admin", "password145Qw!", false, false, true, false, update_interval,
                                            "", "", "");
        break;
    default:
        fail_test("Unknown directory type, please check environment variables!\n");
        exit(EXIT_FAILURE);
    }

    LDHandle *handle = NULL;
    ld_init(&handle, config);

    ld_install_default_handlers(handle);
    ld_install_handler(handle, update_callback, update_interval);
    ld_install_error_handler(handle, connection_on_error);

    ld_exec(handle);

    ld_free(handle);

    talloc_free(talloc_ctx);
}

LDAPAttribute_t** fill_user_attributes(TALLOC_CTX* ctx, LDAPAttribute_t* attrs, int size)
{
    LDAPAttribute_t** copy_attrs = talloc_array(ctx, LDAPAttribute_t*, size + 1);
    for (int i = 0; i < size; ++i)
    {
        copy_attrs[i] = talloc(ctx, LDAPAttribute_t);
        copy_attrs[i]->name = talloc_strdup(ctx, attrs[i].name);

        int value_count = 0;

        while (attrs[i].values[value_count] != NULL)
        {
            value_count++;
        }

        copy_attrs[i]->values = talloc_array(ctx, char*, value_count + 1);

        for (int j = 0; j < value_count; ++j)
        {
            copy_attrs[i]->values[j] = talloc_strdup(ctx, attrs[i].values[j]);
        }

        copy_attrs[i]->values[value_count] = NULL;
    }

    copy_attrs[size] = NULL;

    return copy_attrs;
}
