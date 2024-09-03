#include <ldap_schema.h>
#include <talloc.h>

%%{
    machine parse_attribute_type_description;

    action start
    {
        symbol_start = fpc;
    }

    action oid_end
    {
        if (symbol_start != p)
        {
            result->at_oid = talloc_strndup(talloc_ctx, fpc, fpc - symbol_start);
        }
        symbol_start = p;
    }

    action name_end
    {
        if (symbol_start != p)
        {
            result->at_names = talloc_array(talloc_ctx, char*, 1);
            result->at_names[0] = talloc_strndup(talloc_ctx, fpc, fpc - symbol_start);
        }
        symbol_start = p;
    }

    action syntax_end
    {
        if (symbol_start != p)
        {
            result->at_syntax_oid = talloc_strndup(talloc_ctx, fpc, fpc - symbol_start);
        }
        symbol_start = p;
    }

    action single_value
    {
        result->at_single_value = 1;
    }

    action no_user_modification
    {
        result->at_no_user_mod = 1;
    }

    DOT = ".";
    SQUOTE = "'";
    LDIGIT = 0x31..0x39;
    DIGIT = "0" | LDIGIT;
    SPACE = " ";
    ALPHA = 0x41..0x5a | 0x61..0x7a;
    HYPHEN = "-";
    leadkeychar = ALPHA;
    keychar = ALPHA | DIGIT | HYPHEN;
    keystring = leadkeychar keychar*;
    whsp = SPACE+;
    number = DIGIT | ( LDIGIT DIGIT+ );
    numericoid = number ( DOT number )+;
    syntaxoid = SQUOTE %start ( numericoid | "OctetString"i ) %syntax_end SQUOTE;
    descr = keystring;
    qdescr = whsp "'" %start descr %name_end "'" whsp;
    qdescrlist = ( qdescr qdescr* )?;
    qdescrs = qdescr | ( whsp "(" qdescrlist ")" whsp );
    AttributeTypeDescription = "(" whsp %start numericoid %oid_end whsp
                                   ( "NAME"i qdescrs )?
                                   ( "SYNTAX"i whsp syntaxoid whsp )?
                                   ( "SINGLE-VALUE"i whsp %single_value )?
                                   ( "NO-USER-MODIFICATION"i whsp %no_user_modification )?
                                whsp ")";

    # instantiate machine rules
    main:= AttributeTypeDescription;

}%%

%%write data;

LDAPAttributeType* parse_attribute_type_description(TALLOC_CTX *talloc_ctx, const char *const in, const size_t len)
{
    const char *p = in;
    const char *const pe = in + len;
    int cs;
    const char *symbol_start = p;

    LDAPAttributeType* result = talloc(talloc_ctx, LDAPAttributeType);

    %%{
        write init;
        write exec;
    }%%

    if (cs != parse_attribute_type_description_error)
    {
        return result;
    }
    else
    {
        return NULL;
    }
}
