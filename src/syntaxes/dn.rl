#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

%%{
    machine is_dn;

    ALPHA = 0x41..0x5a | 0x61..0x7a;
    LDIGIT = 0x31..0x39;
    DIGIT = "0" | LDIGIT;
    HEX = DIGIT | 0x41..0x46 | 0x61..0x66;
    SQUOTE = "'";
    LPAREN = "(";
    RPAREN = ")";
    PLUS = "+";
    COMMA = ",";
    HYPHEN = "-";
    DOT = ".";
    EQUALS = "=";
    SLASH = "/";
    COLON = ":";
    QUESTION = "?";
    SPACE = " ";
    ESC = "\\";
    SHARP = "#";
    DQUOTE = "\"";
    SEMI = ";";
    LANGLE = "<";
    RANGLE = ">";

    UTF1 = 0x00..0x7f;
    UTF0 = 0x80..0xbf;
    UTF2 = 0xc2..0xdf UTF0;
    UTF3 = ( 0xe0 0xa0..0xbf UTF0 ) | ( 0xe1..0xec UTF0{2} ) | ( 0xed 0x80..0x9f UTF0 ) | ( 0xee..0xef UTF0{2} );
    UTF4 = ( 0xf0 0x90..0xbf UTF0{2} ) | ( 0xf1..0xf3 UTF0{3} ) | ( 0xf4 0x80..0x8f UTF0{2} );
    UTFMB = UTF2 | UTF3 | UTF4;
    UTF8 = UTF1 | UTFMB;

    LUTF1 = 0x01..0x1f | "!" | 0x24..0x2a | 0x2d..0x3a | "=" | 0x3f..0x5b | 0x5d..0x7f;
    leadchar = LUTF1 | UTFMB;
    escaped = DQUOTE | PLUS | COMMA | SEMI | LANGLE | RANGLE;
    special = escaped | SPACE | SHARP | EQUALS;
    hexpair = HEX HEX;
    pair = ESC ( ESC | special | hexpair );
    SUTF1 = 0x01..0x21 | 0x23..0x2a | 0x2d..0x3a | "=" | 0x3f..0x5b | 0x5d..0x7f;
    stringchar = SUTF1 | UTFMB;
    TUTF1 = 0x01..0x1f | "!" | 0x23..0x2a | 0x2d..0x3a | "=" | 0x3f..0x5b | 0x5d..0x7f;
    trailchar = TUTF1 | UTFMB;
    string = ( ( leadchar | pair ) ( ( stringchar | pair )* ( trailchar | pair ) )? )?;
    hexstring = SHARP hexpair+;

    leadkeychar = ALPHA;
    keychar = ALPHA | DIGIT | HYPHEN;
    keystring = leadkeychar keychar*;
    number = DIGIT | ( LDIGIT DIGIT+ );
    numericoid = number ( DOT number )+;
    descr = keystring;
    oid = descr | numericoid;

    attributeType = oid;
    attributeValue = string | hexstring;
    attributeTypeAndValue = attributeType EQUALS attributeValue;
    relativeDistinguishedName = attributeTypeAndValue ( PLUS attributeTypeAndValue )*;
    distinguishedName = ( relativeDistinguishedName ( COMMA relativeDistinguishedName )* )?;

    main := distinguishedName;
}%%

%%write data;

bool is_dn(const char *const in, const size_t len)
{
    const char *p = in;
    const char *const pe = in + len;
    int cs;

    %%{
        write init;
        write exec;
    }%%

    return cs != is_dn_error;
}
