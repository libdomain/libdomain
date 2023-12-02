#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

%%{
    machine is_boolean;

    Boolean = "TRUE"i | "FALSE"i;

    main := Boolean;
}%%

%%write data;

bool is_boolean(const char *const in, const size_t len)
{
    const char *p = in;
    const char *const pe = in + len;
    int cs;

    %%{
        write init;
        write exec;
    }%%

    return cs != is_boolean_error;
}
