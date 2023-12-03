#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

%%{
    machine is_generalized_time;

    PLUS = "+";
    COMMA = ",";
    DOT = ".";
    MINUS = "-";

    century = 0x30..0x39{2};
    year = 0x30..0x39{2};
    month = ( "0" 0x31..0x39 ) | ( "1" 0x30..0x32 );
    day = ( "0" 0x31..0x39 ) | ( 0x31..0x32 0x30..0x39 ) | ( "3" 0x30..0x31 );
    hour = ( 0x30..0x31 0x30..0x39 ) | ( "2" 0x30..0x33 );
    minute = 0x30..0x35 0x30..0x39;
    second = ( 0x30..0x35 0x30..0x39 );
    leap_second = ( "6" "0" );
    fraction = ( DOT | COMMA ) 0x30..0x39+;
    g_differential = ( MINUS | PLUS ) hour minute?;
    g_time_zone = 0x5a | g_differential;
    GeneralizedTime = century year month day hour ( minute ( second | leap_second )? )? fraction? g_time_zone;

    main := GeneralizedTime;
}%%

%%write data;

bool is_generalized_time(const char *const in, const size_t len)
{
    const char *p = in;
    const char *const pe = in + len;
    int cs;

    %%{
        write init;
        write exec;
    }%%

    return cs != is_generalized_time_error;
}
