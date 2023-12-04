#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

%%{
    machine is_oid;

    ALPHA = 0x41..0x5a | 0x61..0x7a;
    LDIGIT = 0x31..0x39;
    DIGIT = "0" | LDIGIT;
    HYPHEN = "-";
    DOT = ".";

    leadkeychar = ALPHA;
    keychar = ALPHA | DIGIT | HYPHEN;
    keystring = leadkeychar keychar*;
    number = DIGIT | ( LDIGIT DIGIT+ );
    numericoid = number ( DOT number )+;
    descr = keystring;
    oid = descr | numericoid;

    main := oid;
}%%

%%write data;

bool is_oid(const char *const in, const size_t len)
{
    const char *p = in;
    const char *const pe = in + len;
    int cs;

    %%{
        write init;
        write exec;
    }%%

    return cs != is_oid_error;
}
