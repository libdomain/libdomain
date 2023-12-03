#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

%%{
    machine is_numeric_string;

    LDIGIT = 0x31..0x39;
    DIGIT = "0" | LDIGIT;
    SPACE = " ";

    NumericString = ( DIGIT | SPACE )+;

    main := NumericString;
}%%

%%write data;

bool is_numeric_string(const char *const in, const size_t len)
{
    const char *p = in;
    const char *const pe = in + len;
    int cs;

    %%{
        write init;
        write exec;
    }%%

    return cs != is_numeric_string_error;
}
