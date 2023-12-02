#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

%%{
    machine is_integer;

	LDIGIT = 0x31..0x39;
	DIGIT = "0" | LDIGIT;
	HYPHEN = "-";
	number = DIGIT | ( LDIGIT DIGIT+ );

    Integer = ( HYPHEN LDIGIT DIGIT* ) | number;

    main := Integer;
}%%

%%write data;

bool is_integer(const char *const in, const size_t len)
{
    const char *p = in;
    const char *const pe = in + len;
    int cs;

    %%{
        write init;
        write exec;
    }%%

    return cs != is_integer_error;
}
