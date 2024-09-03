#include <ldap_schema.h>
#include <talloc.h>

%%{
    machine parse_object_class_description;

    action start
    {
        symbol_start = fpc;
    }

    action oid_end
    {
        if (symbol_start != p)
        {
            result->oc_oid = talloc_strndup(talloc_ctx, fpc, fpc - symbol_start);
        }
        symbol_start = p;
    }

    action name_end
    {
        if (symbol_start != p)
        {
            result->oc_names = talloc_array(talloc_ctx, char*, 1);
            result->oc_names[0] = talloc_strndup(talloc_ctx, fpc, fpc - symbol_start);
        }
        symbol_start = p;
    }

    DOT = ".";
    SQUOTE = "'";
    LDIGIT = 0x31..0x39;
    DIGIT = "0" | LDIGIT;
    SPACE = " ";
    ALPHA = 0x41..0x5a | 0x61..0x7a;
    HYPHEN = "-";
    LPAREN = "(";
    RPAREN = ")";
    DOLLAR = "$";
    WSP = SPACE*;
    leadkeychar = ALPHA;
    keychar = ALPHA | DIGIT | HYPHEN;
    keystring = leadkeychar keychar*;
    whsp = SPACE+;
    number = DIGIT | ( LDIGIT DIGIT+ );
    numericoid = number ( DOT number )+;
    syntaxoid = SQUOTE ( numericoid | "OctetString"i ) SQUOTE;
    descr = keystring;
    qdescr = whsp "'" %start descr %name_end "'" whsp;
    qdescrlist = ( qdescr qdescr* )?;
    qdescrs = qdescr | ( whsp "(" qdescrlist ")" whsp );
    oid = descr | numericoid;
    oidlist = oid ( WSP DOLLAR WSP oid )*;
    oids = oid | ( LPAREN WSP oidlist WSP RPAREN );
    ObjectClassDescription = "(" whsp %start numericoid %oid_end whsp
                                ( "NAME"i qdescrs )?
                                ( "SUP"i oids )?
                                ( ( "ABSTRACT"i | "STRUCTURAL"i | "AUXILIARY"i ) whsp )?
                                ( "MUST"i oids )?
                                ( "MAY"i oids )?
                              whsp ")";

    # instantiate machine rules
    main:= ObjectClassDescription;
}%%

%%write data;

LDAPObjectClass* parse_object_class_description(TALLOC_CTX* talloc_ctx, const char *const in, const size_t len)
{
    const char *p = in;
    const char *const pe = in + len;
    int cs;
    const char *symbol_start = p;

    LDAPObjectClass* result = talloc(talloc_ctx, LDAPObjectClass);

    %%{
        write init;
        write exec;
    }%%

    if (cs != parse_object_class_description_error)
    {
        return result;
    }
    else
    {
        return NULL;
    }
}
