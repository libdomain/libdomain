# libdomain

**libdomain** is a versatile C library designed to simplify interactions with various LDAP servers, including FreeIPA, Samba/AD, and OpenLDAP. It provides a convenient and consistent interface for managing LDAP-related tasks, making it easier for developers to work with LDAP servers in their applications.

## Features

- **Support for Multiple LDAP Servers:** Whether you are working with FreeIPA, Samba/AD, or OpenLDAP, libdomain abstracts the complexities,
allowing you to focus on your application's logic.

- **Ease of Use:** The library provides a user-friendly interface for common LDAP operations, reducing the learning curve and accelerating development.

## Installation

To use libdomain in your C project, follow these steps:

1. Clone the repository:

```bash
git clone https://github.com/libdomain/libdomain.git
```

2. Build the library:

```bash
cd libdomain && mkdir build && cd build && cmake .. && make -j `nproc`
```

3. Link libdomain with your project.

## Getting Started

### 1. Include libdomain in Your Code

```c
#include <ldap.h>
#include <talloc.h>

#include <libdomain/common.h>
#include <libdomain/domain.h>
#include <libdomain/domain_p.h>
#include <libdomain/directory.h>
#include <libdomain/entry.h>
#include <libdomain/connection_state_machine.h>
```

### 2. Initialize Connection

```c
// Replace these values with your LDAP server details
const char *ldap_server = "ldap://example.com";
const char *ldap_username = "your_username";
const char *ldap_password = "your_password";
const char *ldap_bind_dn = "dc=example,dc=com";

// Initialize the LDAP connection
const int update_interval = 1000;

ld_config_t *config = NULL;
config = ld_create_config(talloc_ctx, ldap_server, 0, LDAP_VERSION3, ldap_bind_dn,
                          ldap_username, ldap_password, false, false, true, false,
                          update_interval, "", "", "");

LDHandle *handle = NULL;
ld_init(&handle, config);

ld_install_default_handlers(handle);
ld_install_handler(handle, connection_on_update, update_interval);
```

### 3. Perform LDAP Operations

```c
// Install search handler
ld_install_handler(handle, connection_on_update, update_interval);

// Start event loop.
ld_exec(handle);
```

```c
// Example: Search for a user
static char* LDAP_DIRECTORY_ATTRS[] = { "objectClass", NULL };

static void connection_on_update(verto_ctx *ctx, verto_ev *ev)
{
    (void)(ctx);

    struct ldap_connection_ctx_t* connection = verto_get_private(ev);

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_RUN)
    {
        verto_del(ev);

        search(connection, "ou=users,dc=example,dc=com", LDAP_SCOPE_SUBTREE,
               "(uid=johndoe)", LDAP_DIRECTORY_ATTRS, 0, NULL);
    }

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_ERROR)
    {
        verto_break(ctx);

        error("Error encountered during bind!\n");
    }
}
```

### 4. Close Connection

```c
// Close the LDAP connection when done
ld_free(handle);

talloc_free(talloc_ctx);
```

## Documentation

For detailed information on libdomain's API and usage, refer to the [documentation](https://august-alt.github.io/libdomain/).

## Examples
- Full source of example above - [libdomain c sample](https://github.com/libdomain/libdomain-c-sample).

## Contribution

Contributions are welcome! If you encounter any issues or have suggestions for improvements, please [open an issue](https://github.com/libdomain/libdomain/issues).

## License

This project is licensed under the [GPLv2](LICENSE.md).
