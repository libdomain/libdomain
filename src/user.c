#include "user.h"

#include "group.h"
#include "common.h"
#include "domain_p.h"
#include "entry.h"

#include <string.h>

static attribute_value_pair_t LDAP_USER_ATTRIBUTES =
{
    { "objectClass", "top:account:posixAccount:shadowAccount" },
    { "cn", "" },
    { "uid", "" },
    { "uidNumber", "0" },
    { "gidNumber", "0" },
    { "homeDirectory", "/home/adam" },
    { "loginShell", "/bin/bash" },
    { "gecos", "adam" },
    { "userPassword", "{crypt}x" },
    { "shadowLastChange", "0" },
    { "shadowMax", "0" },
    { "shadowWarning",  "0" }
};
static const int USER_ATTRIBUTES_SIZE = number_of_elements(LDAP_USER_ATTRIBUTES);

enum OperationReturnCode ld_add_user(LDHandle *handle, const char *name, void **user_attrs)
{
    return ld_add_entry(handle, name, "ou=users", user_attrs);
}

enum OperationReturnCode ld_del_user(LDHandle *handle, const char *name)
{
    return ld_del_entry(handle, name, "ou=users");
}

enum OperationReturnCode ld_mod_user(LDHandle *handle, const char *name, void **user_attrs)
{
    return ld_mod_entry(handle, name, "ou=users", user_attrs);
}

enum OperationReturnCode ld_rename_user(LDHandle *handle, const char *old_name, const char *new_name)
{
    return ld_rename_entry(handle, old_name, new_name, "ou=users");
}
