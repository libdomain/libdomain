#include "user.h"

#include "group.h"
#include "common.h"
#include "domain_p.h"
#include "entry.h"

#include <string.h>

static attribute_value_pair_t LDAP_USER_ATTRIBUTES[] =
{
    { "objectClass", "top:account:posixAccount:shadowAccount" },
    { "cn", NULL },
    { "uid", NULL },
    { "uidNumber", NULL },
    { "gidNumber", NULL },
    { "homeDirectory", NULL },
    { "loginShell", NULL },
    { "gecos", NULL },
    { "userPassword", NULL },
    { "shadowLastChange", NULL },
    { "shadowMax", NULL },
    { "shadowWarning", NULL }
};
static const int USER_ATTRIBUTES_SIZE = number_of_elements(LDAP_USER_ATTRIBUTES);

enum UserAttributeIndex
{
    CN             = 1,
    UID            = 2,
    UID_NUMBER     = 3,
    GID_NUMBER     = 4,
    HOME_DIRECTORY = 5,
    LOGIN_SHELL    = 6,
    GECOS          = 7,
    USER_PASSWORD  = 8,
};

const char* create_user_parent(TALLOC_CTX *talloc_ctx, LDHandle *handle)
{
    return talloc_asprintf(talloc_ctx, "%s,%s", "ou=users", handle ? handle->global_config->base_dn : "");
}

/**
 * @brief ld_add_user     Creates user.
 * @param handle          Pointer to libdomain session handle.
 * @param name            Name of the user.
 * @param uid             User id number.
 * @param gid             Group id number.
 * @param home_directory  Home directory associated with the user.
 * @param login_shell     Login shell to launch on user login.
 * @param password        Password of the user.
 * @param parent          Container to create user into.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_add_user(LDHandle *handle, const char *name, int uid, int gid, const char* home_directory,
                                     const char* login_shell, const char* password, const char* parent)
{
    const char* dn = NULL;
    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    LDAPAttribute_t **group_attrs  = assign_default_attribute_values(talloc_ctx,
                                                                     LDAP_USER_ATTRIBUTES,
                                                                     USER_ATTRIBUTES_SIZE);

    const char* uid_number = talloc_asprintf(talloc_ctx, "%d", uid);
    const char* gid_number = talloc_asprintf(talloc_ctx, "%d", gid);

    check_and_assign_attribute(group_attrs, name, CN, talloc_ctx);
    check_and_assign_attribute(group_attrs, name, UID, talloc_ctx);
    check_and_assign_attribute(group_attrs, name, GECOS, talloc_ctx);
    check_and_assign_attribute(group_attrs, uid_number, UID_NUMBER, talloc_ctx);
    check_and_assign_attribute(group_attrs, gid_number, GID_NUMBER, talloc_ctx);
    check_and_assign_attribute(group_attrs, home_directory, HOME_DIRECTORY, talloc_ctx);
    check_and_assign_attribute(group_attrs, login_shell, LOGIN_SHELL, talloc_ctx);
    check_and_assign_attribute(group_attrs, password, USER_PASSWORD, talloc_ctx);

    if (parent && strlen(parent) > 0)
    {
        dn = parent;
    }
    else
    {
        dn = create_user_parent(talloc_ctx, handle);
    }

    rc = ld_add_entry(handle, name, dn, group_attrs);

    talloc_free(talloc_ctx);

    return rc;
}

/**
 * @brief ld_del_user Deletes user.
 * @param handle      Pointer to libdomain session handle.
 * @param name        Name of the user.
 * @param parent      Container that holds the user.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_del_user(LDHandle *handle, const char *name, const char* parent)
{
    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    int rc = ld_del_entry(handle, name, parent ? parent : create_user_parent(talloc_ctx, handle), "cn");

    talloc_free(talloc_ctx);

    return rc;
}

/**
 * @brief ld_mod_user Modifies the user.
 * @param handle      Pointer to libdomain session handle.
 * @param name        Name of the user.
 * @param parent      Container that holds the user.
 * @param user_attrs  List of user attributes.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_mod_user(LDHandle *handle, const char *name, const char *parent, LDAPAttribute_t **user_attrs)
{
    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    int rc = ld_mod_entry(handle, name, parent ? parent : create_user_parent(talloc_ctx, handle), user_attrs);

    talloc_free(talloc_ctx);

    return rc;
}

/**
 * @brief ld_rename_user Renames the user.
 * @param handle         Pointer to libdomain session handle.
 * @param old_name       Old name of the user.
 * @param new_name       New name of the user.
 * @param parent         Container that holds the user.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_rename_user(LDHandle *handle, const char *old_name, const char *new_name, const char *parent)
{
    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    int rc = ld_rename_entry(handle, old_name, new_name, parent ? parent : create_user_parent(talloc_ctx, handle), "cn");

    talloc_free(talloc_ctx);

    return rc;
}
