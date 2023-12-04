#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

%%{
    machine is_octet_string;

    alphtype unsigned char;

    OCTET = 0x00..0xff;

    OctetString = OCTET*;

    main := OctetString;
}%%

%%write data;

bool is_octet_string(const char *const in, const size_t len)
{
    const char *p = in;
    const char *const pe = in + len;
    int cs;

    %%{
        write init;
        write exec;
    }%%

    return cs != is_octet_string_error;
}
