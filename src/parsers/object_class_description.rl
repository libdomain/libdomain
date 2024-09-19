#include <ldap_schema.h>
#include <talloc.h>

%%{
    machine parse_object_class_description;

    action start
    {
        symbol_start = fpc;
    }

    action numericoid_end
    {
        if (symbol_start != p)
        {
            result->oc_oid = talloc_strndup(talloc_ctx, symbol_start, fpc - symbol_start);
        }
        symbol_start = p;
    }

    action name_end
    {
        if (symbol_start != p)
        {
            result->oc_names = talloc_realloc(talloc_ctx, result->oc_names, char*, name_index + 2);
            result->oc_names[name_index] = talloc_strndup(talloc_ctx, symbol_start, fpc - symbol_start);
            name_index++;
            result->oc_names[name_index] = NULL;
        }
        symbol_start = p;
    }

    action oid_end
    {
        if (symbol_start != p)
        {
            oids = talloc_realloc(talloc_ctx, oids, char*, oid_index + 2);
            oids[oid_index] = talloc_strndup(talloc_ctx, symbol_start, fpc - symbol_start);
            oid_index++;
            oids[oid_index] = NULL;
        }
        symbol_start = p;
    }

    action write_sups
    {
        if (oids)
        {
            result->oc_sup_oids = oids;
            oid_index = 0;
            oids = NULL;
        }
    }

    action write_must
    {
        if (oids)
        {
            result->oc_at_oids_must = oids;
            oid_index = 0;
            oids = NULL;
        }
    }

    action write_may
    {
        if (oids)
        {
            result->oc_at_oids_may = oids;
            oid_index = 0;
            oids = NULL;
        }
    }

    action set_structural
    {
        result->oc_kind = 1;
    }

    action set_auxiliary
    {
        result->oc_kind = 2;
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
    whsp = SPACE*;
    number = DIGIT | ( LDIGIT DIGIT+ );
    numericoid = number ( DOT number )+;
    syntaxoid = SQUOTE ( numericoid | "OctetString"i ) SQUOTE;
    descr = keystring;
    qdescr = whsp "'" %start descr %name_end "'" whsp;
    qdescrlist = ( qdescr qdescr* )?;
    qdescrs = qdescr | ( whsp "(" qdescrlist ")" whsp );
    oid = (descr | numericoid) >start %oid_end;
    oidlist = oid ( WSP DOLLAR WSP oid )*;
    oids = oid | ( LPAREN WSP oidlist WSP RPAREN );
    ObjectClassDescription = "(" whsp %start numericoid %numericoid_end whsp
                                ( "NAME"i qdescrs )?
                                ( "SUP"i whsp oids %write_sups whsp )?
                                (
                                  ( "ABSTRACT"i
                                    | "STRUCTURAL"i %set_structural
                                    | "AUXILIARY"i %set_auxiliary
                                  ) whsp
                                )?
                                ( "MUST"i whsp oids whsp )?
                                ( "MAY"i whsp oids )?
                              whsp ")";

    # instantiate machine rules
    main:= ("objectClasses"i ":" whsp)? ObjectClassDescription;
}%%

%%write data;

LDAPObjectClass* parse_object_class_description(TALLOC_CTX* talloc_ctx, const char *const in, const size_t len)
{
    if (!talloc_ctx || !in)
    {
        return NULL;
    }

    const char *p = in;
    const char *const pe = in + len;
    const char *symbol_start = p;
    int cs = 0;

    int name_index = 0;
    int oid_index  = 0;

    char **oids = NULL;

    LDAPObjectClass* result = talloc_zero(talloc_ctx, LDAPObjectClass);

    if (!result)
    {
        return NULL;
    }

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
