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

/**
 * @brief create_user_parent  Get user parent string allocated with talloc.
 * @param[in] talloc_ctx      Pointer to talloc context.
 * @param[in] LDHandle        libdomain session handle.
 * @return
 *        - User Parent if exist.
 *        - "" if user parent doesn't exist.
 *        - NULL on out of memory
 */
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

/**
 * @brief create_lockout_time_attributes_openldap  Create lockout time attributes for OpenLDAP
 * @param[in] ctx                                  Pointer to talloc context.
 * @param[in] value                                lockout time.
 * @return
 *        - Pointer to array with already created lockout time attribute.
 *        - NULL on out of memory
 */
static LDAPAttribute_t** create_lockout_time_attributes_openldap(TALLOC_CTX* ctx, const char* value)
{
    LDAPAttribute_t** attrs = NULL;

    ld_talloc_array(attrs, error_exit, ctx, LDAPAttribute_t*, 2);
    ld_talloc(attrs[0], error_exit, attrs, LDAPAttribute_t);
    ld_talloc_strdup(attrs[0]->name, error_exit, attrs, "pwdAccountLockedTime");
    ld_talloc_zero_array(attrs[0]->values, error_exit, attrs, char*, 2);

    if (value)
    {
        // ctx - because of last allocation.
        // see steal lower
        ld_talloc_strdup(attrs[0]->values[0], error_exit, ctx, value);
    }
    // or NULL
    // `attrs[0]->values[0]` and `attrs[0]->values[1]` already zeroed

    attrs[1] = NULL;

    talloc_steal(ctx, attrs[0]);
    talloc_steal(ctx, attrs[0]->name);
    talloc_steal(ctx, attrs[0]->values);

    return attrs;

    error_exit:
        if (attrs)
        {
            talloc_free(attrs);
        }

        return NULL;
}

/**
 * @brief create_lockout_time_attributes_openldap  Create lockout time attributes for Active Directory
 * @param[in] ctx                                  Pointer to talloc context.
 * @param[in] value                                lockout time.
 * @return
 *        - Pointer to array with already created lockout time attribute.
 *        - NULL on out of memory
 */
static LDAPAttribute_t** create_lockout_time_attributes_ad(TALLOC_CTX* ctx, const char* value)
{
    LDAPAttribute_t** attrs = NULL;

    ld_talloc_array(attrs, error_exit, ctx, LDAPAttribute_t*, 2);
    ld_talloc(attrs[0], error_exit, attrs, LDAPAttribute_t);
    ld_talloc_strdup(attrs[0]->name, error_exit, attrs, "userAccountControl");
    ld_talloc_zero_array(attrs[0]->values, error_exit, attrs, char*, 2);

    if (value)
    {
        // ctx - because of last allocation.
        // see steal lower
        ld_talloc_strdup(attrs[0]->values[0], error_exit, ctx, value);
    }
    // or NULL
    // `attrs[0]->values[0]` and `attrs[0]->values[1]` already zeroed

    talloc_steal(ctx, attrs[0]);
    talloc_steal(ctx, attrs[0]->name);
    talloc_steal(ctx, attrs[0]->values);

    return attrs;

    error_exit:
        if (attrs)
        {
            talloc_free(attrs);
        }
        
        return NULL;
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
    TALLOC_CTX *talloc_ctx = NULL;

    ld_talloc_new(talloc_ctx, error_exit, NULL);

    if (parent && strlen(parent) > 0)
    {
        dn = parent;
    }
    else
    {
        LD_ALLOC_HELPER(dn, create_user_parent, "Unable to create user parent - out of memory", error_exit, talloc_ctx, handle);
    }

    rc = ld_add_entry(handle, name, dn, "cn", user_attrs);


    // rc = RETURN_CODE_FAILURE on error exit. In any other case - result of ld_add_entry.
    error_exit:
        if (talloc_ctx)
        {
            talloc_free(talloc_ctx);
        }

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
    const char* dn = NULL;
    enum OperationReturnCode rc = RETURN_CODE_FAILURE;
    TALLOC_CTX *talloc_ctx = NULL;

    ld_talloc_new(talloc_ctx, error_exit, NULL);

    if (parent)
    {
        dn = parent;
    }
    else 
    {
        LD_ALLOC_HELPER(dn, create_user_parent, "Unable to create user parent - out of memory", error_exit, talloc_ctx, handle);
    }

    rc = ld_del_entry(handle, name, dn, "cn");


    // rc = RETURN_CODE_FAILURE on error exit. In any other case - result of ld_del_entry.
    error_exit:
        if (dn)
        {
            talloc_free(talloc_ctx);
        }

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
    const char* dn = NULL;
    enum OperationReturnCode rc = RETURN_CODE_FAILURE;
    TALLOC_CTX *talloc_ctx = NULL;
    
    ld_talloc_new(talloc_ctx, error_exit, NULL);

    if (parent)
    {
        dn = parent;
    }
    else 
    {
        LD_ALLOC_HELPER(dn, create_user_parent, "Unable to create user parent - out of memory", error_exit, talloc_ctx, handle);
    }

    rc = ld_mod_entry(handle, name, dn, "cn", user_attrs);

    error_exit: 
        if (talloc_ctx) 
        {
            talloc_free(talloc_ctx);
        }

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
    const char* dn;
    enum OperationReturnCode rc = RETURN_CODE_FAILURE;
    TALLOC_CTX *talloc_ctx = NULL;

    ld_talloc_new(talloc_ctx, error_exit, NULL);

    if (parent)
    {
        dn = parent;
    }
    else 
    {
        LD_ALLOC_HELPER(dn, create_user_parent, "Unable to create user parent - out of memory", error_exit, talloc_ctx, handle);
    }

    rc = ld_rename_entry(handle, old_name, new_name, dn, "cn");

    error_exit: 
        if (talloc_ctx) 
        {
            talloc_free(talloc_ctx);
        }

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
    const char* dn = NULL;
    enum OperationReturnCode rc = RETURN_CODE_FAILURE;
    TALLOC_CTX *talloc_ctx = NULL;
    LDAPAttribute_t** attrs = NULL;

    ld_talloc_new(talloc_ctx, error_exit, NULL);

    switch (handle->connection_ctx->directory_type) {
    case LDAP_TYPE_OPENLDAP:
        LD_ALLOC_HELPER(
            attrs, 
            create_lockout_time_attributes_openldap, 
            "Unable to create lockout time OpenLDAP attribute - out of memory", error_exit, 
            talloc_ctx, 
            "000001010000Z");
        break;
    case LDAP_TYPE_ACTIVE_DIRECTORY:
        LD_ALLOC_HELPER(
            attrs, 
            create_lockout_time_attributes_ad, 
            "Unable to create lockout time Active Directory attribute - out of memory", error_exit, 
            talloc_ctx, 
            "514");
        break;
    case LDAP_TYPE_FREE_IPA:
         ld_info("Unblocking users for free ipa is not implemented!\n");
    default:
        goto error_exit;
    }

    if (parent)
    {
        dn = parent;
    }
    else 
    {
        LD_ALLOC_HELPER(dn, create_user_parent, "Unable to create user parent - out of memory", error_exit, talloc_ctx, handle);
    }

    rc = ld_mod_entry_attrs(handle, name, dn, "cn", attrs, LDAP_MOD_REPLACE);

    error_exit:
        if (talloc_ctx)
        {
            talloc_free(talloc_ctx);
        }

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
    enum OperationReturnCode rc = RETURN_CODE_FAILURE;
    int mod_op = LDAP_MOD_REPLACE;
    const char* dn = NULL;
    TALLOC_CTX *talloc_ctx = NULL;
    LDAPAttribute_t** attrs = NULL;

    ld_talloc_new(talloc_ctx, error_exit, NULL);

    switch (handle->connection_ctx->directory_type) {
    case LDAP_TYPE_OPENLDAP:
        mod_op = LDAP_MOD_DELETE;
        LD_ALLOC_HELPER(
            attrs, 
            create_lockout_time_attributes_openldap, 
            "Unable to create lockout time OpenLDAP attribute - out of memory", error_exit, 
            talloc_ctx, 
            NULL);
        break;
    case LDAP_TYPE_ACTIVE_DIRECTORY:
        LD_ALLOC_HELPER(
            attrs, 
            create_lockout_time_attributes_ad, 
            "Unable to create lockout time OpenLDAP attribute - out of memory", error_exit, 
            talloc_ctx, 
            "512");
        break;
    case LDAP_TYPE_FREE_IPA:
         ld_info("Unblocking users for free ipa is not implemented!\n");
    default:
        goto error_exit;
    }

    if (parent)
    {
        dn = parent;
    }
    else 
    {
        LD_ALLOC_HELPER(dn, create_user_parent, "Unable to create user parent - out of memory", error_exit, talloc_ctx, handle);
    }

    rc = ld_mod_entry_attrs(handle, name, dn, "cn", attrs, mod_op);

    error_exit:
        if (talloc_ctx)
        {
            talloc_free(talloc_ctx);
        }

        return rc;
}
