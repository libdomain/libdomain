#include "user.h"

#include "group.h"
#include "common.h"
#include "directory.h"
#include "domain_p.h"
#include "entry.h"

#include <string.h>
#include <time.h>

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

static const char* create_user_parent(TALLOC_CTX *talloc_ctx, LDHandle *handle)
{
    char* ou_users = NULL;
    switch (handle->connection_ctx->directory_type)
    {
    case LDAP_TYPE_ACTIVE_DIRECTORY:
        ou_users = "cn=users";
        break;
    default:
        ou_users = "ou=users";
        break;
    }

    return talloc_asprintf(talloc_ctx, "%s,%s", ou_users, handle ? handle->global_config->base_dn : "");
}

static LDAPAttribute_t** create_lockout_time_attributes_openldap(TALLOC_CTX* ctx, const char* value)
{
    LDAPAttribute_t** attrs;

    attrs = talloc_array(ctx, LDAPAttribute_t*, 2);
    attrs[0] = talloc(ctx, LDAPAttribute_t);
    attrs[0]->values = talloc_array(ctx, char*, 2);
    attrs[0]->name = talloc_strdup(ctx, "pwdAccountLockedTime");
    attrs[0]->values[0] = value ? talloc_strdup(ctx, value) : NULL;
    attrs[0]->values[1] = NULL;
    attrs[1] = NULL;

    return attrs;
}

static LDAPAttribute_t** create_lockout_time_attributes_ad(TALLOC_CTX* ctx, const char* value)
{
    LDAPAttribute_t** attrs;

    attrs = talloc_array(ctx, LDAPAttribute_t*, 2);
    attrs[0] = talloc(ctx, LDAPAttribute_t);
    attrs[0]->values = talloc_array(ctx, char*, 2);
    attrs[0]->name = talloc_strdup(ctx, "userAccountControl");
    attrs[0]->values[0] = value ? talloc_strdup(ctx, value) : NULL;
    attrs[0]->values[1] = NULL;
    attrs[1] = NULL;

    return attrs;
}


/**
 * @brief ld_add_user     Creates user.
 * @param[in] handle          Pointer to libdomain session handle.
 * @param[in] name            Name of the user.
 * @param[in] user_attrs      Attributes of a user.
 * @return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_add_user(LDHandle *handle, const char *name, LDAPAttribute_t **user_attrs, const char* parent)
{
    const char* dn = NULL;
    enum OperationReturnCode rc = RETURN_CODE_FAILURE;

    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    if (parent && strlen(parent) > 0)
    {
        dn = parent;
    }
    else
    {
        dn = create_user_parent(talloc_ctx, handle);
    }

    rc = ld_add_entry(handle, name, dn, "cn", user_attrs);

    talloc_free(talloc_ctx);

    return rc;
}

/**
 * @brief ld_del_user Deletes user.
 * @param[in] handle      Pointer to libdomain session handle.
 * @param[in] name        Name of the user.
 * @param[in] parent      Container that holds the user.
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

    int rc = ld_mod_entry(handle, name, parent ? parent : create_user_parent(talloc_ctx, handle), "cn", user_attrs);

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

/*!
 * \brief ld_unblock_user Blocks user based on the type of directory service which we are connected to.
 * \param[in] handle Pointer to libdomain session handle.
 * \param[in] name   Name of the user to block.
 * \param[in] parent Parent dn of the user. Can be NULL than default parent will be selected.
 * \return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_block_user(LDHandle *handle, const char *name, const char *parent)
{
    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    LDAPAttribute_t** attrs;
    switch (handle->connection_ctx->directory_type) {
    case LDAP_TYPE_OPENLDAP:
        attrs = create_lockout_time_attributes_openldap(talloc_ctx, "000001010000Z");
        break;
    case LDAP_TYPE_ACTIVE_DIRECTORY:
        attrs = create_lockout_time_attributes_ad(talloc_ctx, "514");
        break;
    case LDAP_TYPE_FREE_IPA:
         ld_info("Unblocking users for free ipa is not implemented!\n");
    default:
        return RETURN_CODE_FAILURE;
        break;
    }

    int rc = ld_mod_entry_attrs(handle, name, parent ? parent : create_user_parent(talloc_ctx, handle), "cn", attrs, LDAP_MOD_REPLACE);

    talloc_free(talloc_ctx);

    return rc;
}

/*!
 * \brief ld_unblock_user Unblocks user based on the type of directory service which we are connected to.
 * \param[in] handle Pointer to libdomain session handle.
 * \param[in] name   Name of the user to unblock.
 * \param[in] parent Parent dn of the user. Can be NULL than default parent will be selected.
 * \return
 *        - RETURN_CODE_SUCCESS on success.
 *        - RETURN_CODE_FAILURE on failure.
 */
enum OperationReturnCode ld_unblock_user(LDHandle *handle, const char *name, const char *parent)
{
    TALLOC_CTX *talloc_ctx = talloc_new(NULL);

    int mod_op = LDAP_MOD_REPLACE;
    LDAPAttribute_t** attrs = NULL;
    switch (handle->connection_ctx->directory_type) {
    case LDAP_TYPE_OPENLDAP:
        mod_op = LDAP_MOD_DELETE;
        attrs = create_lockout_time_attributes_openldap(talloc_ctx, NULL);
        break;
    case LDAP_TYPE_ACTIVE_DIRECTORY:
        attrs = create_lockout_time_attributes_ad(talloc_ctx, "512");
        break;
    case LDAP_TYPE_FREE_IPA:
         ld_info("Unblocking users for free ipa is not implemented!\n");
    default:
        return RETURN_CODE_FAILURE;
        break;
    }

    int rc = ld_mod_entry_attrs(handle, name, parent ? parent : create_user_parent(talloc_ctx, handle), "cn", attrs, mod_op);

    talloc_free(talloc_ctx);

    return rc;
}
