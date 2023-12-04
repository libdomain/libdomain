#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

%%{
    machine is_directory_string;

    UTF1 = 0x00..0x7f;
    UTF0 = 0x80..0xbf;
    UTF2 = 0xc2..0xdf UTF0;
    UTF3 = ( 0xe0 0xa0..0xbf UTF0 ) | ( 0xe1..0xec UTF0{2} ) | ( 0xed 0x80..0x9f UTF0 ) | ( 0xee..0xef UTF0{2} );
    UTF4 = ( 0xf0 0x90..0xbf UTF0{2} ) | ( 0xf1..0xf3 UTF0{3} ) | ( 0xf4 0x80..0x8f UTF0{2} );
    UTFMB = UTF2 | UTF3 | UTF4;
    UTF8 = UTF1 | UTFMB;

    DirectoryString = UTF8+;

    main := DirectoryString;
}%%

%%write data;

bool is_directory_string(const char *const in, const size_t len)
{
    const char *p = in;
    const char *const pe = in + len;
    int cs;

    %%{
        write init;
        write exec;
    }%%

    return cs != is_directory_string_error;
}
