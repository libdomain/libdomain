#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

%%{
    machine is_ia5string;

    IA5String = 0x00..0x7f*;

    main := IA5String;
}%%

%%write data;

bool is_ia5string(const char *const in, const size_t len)
{
    const char *p = in;
    const char *const pe = in + len;
    int cs;

    %%{
        write init;
        write exec;
    }%%

    return cs != is_ia5string_error;
}
