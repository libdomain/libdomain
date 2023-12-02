
#line 1 "ia5string.rl"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


#line 11 "ia5string.rl"



#line 13 "ia5string.c"
static const int is_ia5string_start = 1;
static const int is_ia5string_first_final = 1;
static const int is_ia5string_error = 0;

static const int is_ia5string_en_main = 1;


#line 14 "ia5string.rl"

bool is_ia5string(const char *const in, const size_t len)
{
    const char *p = in;
    const char *const pe = in + len;
    int cs;

    
#line 30 "ia5string.c"
	{
	cs = is_ia5string_start;
	}

#line 35 "ia5string.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
	if ( (*p) <= -1 )
		goto st0;
	goto st1;
st0:
cs = 0;
	goto _out;
	}
	_test_eof1: cs = 1; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 24 "ia5string.rl"


    return cs != is_ia5string_error;
}
