/* JSON_parser.c */

/* 2005-12-30 */

/*
Copyright (c) 2005 JSON.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

The Software shall be used for Good, not Evil.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include "JSON_parser.h"

/* Windows defines IN for documentation */
#undef IN

#define true  1
#define false 0
#define __   -1     /* the universal error code */

/*
    Characters are mapped into these 31 character classes. This allows for
    a significant reduction in the size of the state transition table.
*/

enum classes {
    C_SPACE,  /* space */
    C_WHITE,  /* other whitespace */
    C_LCURB,  /* {  */
    C_RCURB,  /* } */
    C_LSQRB,  /* [ */
    C_RSQRB,  /* ] */
    C_COLON,  /* : */
    C_COMMA,  /* , */
    C_QUOTE,  /* " */
    C_BACKS,  /* \ */
    C_SLASH,  /* / */
    C_PLUS,   /* + */
    C_MINUS,  /* - */
    C_POINT,  /* . */
    C_ZERO ,  /* 0 */
    C_DIGIT,  /* 123456789 */
    C_LOW_A,  /* a */
    C_LOW_B,  /* b */
    C_LOW_C,  /* c */
    C_LOW_D,  /* d */
    C_LOW_E,  /* e */
    C_LOW_F,  /* f */
    C_LOW_L,  /* l */
    C_LOW_N,  /* n */
    C_LOW_R,  /* r */
    C_LOW_S,  /* s */
    C_LOW_T,  /* t */
    C_LOW_U,  /* u */
    C_ABCDF,  /* ABCDF */
    C_E,      /* E */
    C_ETC,    /* everything else */
    NR_CLASSES
};

static const int ascii_class[128] = {
/*
    This array maps the 128 ASCII characters into character classes.
    The remaining Unicode characters should be mapped to C_ETC.
    Non-whitespace control characters are errors.
*/
    __,      __,      __,      __,      __,      __,      __,      __,
    __,      C_WHITE, C_WHITE, __,      __,      C_WHITE, __,      __,
    __,      __,      __,      __,      __,      __,      __,      __,
    __,      __,      __,      __,      __,      __,      __,      __,

    C_SPACE, C_ETC,   C_QUOTE, C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,
    C_ETC,   C_ETC,   C_ETC,   C_PLUS,  C_COMMA, C_MINUS, C_POINT, C_SLASH,
    C_ZERO,  C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT,
    C_DIGIT, C_DIGIT, C_COLON, C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,

    C_ETC,   C_ABCDF, C_ABCDF, C_ABCDF, C_ABCDF, C_E,     C_ABCDF, C_ETC,
    C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,
    C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,
    C_ETC,   C_ETC,   C_ETC,   C_LSQRB, C_BACKS, C_RSQRB, C_ETC,   C_ETC,

    C_ETC,   C_LOW_A, C_LOW_B, C_LOW_C, C_LOW_D, C_LOW_E, C_LOW_F, C_ETC,
    C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_LOW_L, C_ETC,   C_LOW_N, C_ETC,
    C_ETC,   C_ETC,   C_LOW_R, C_LOW_S, C_LOW_T, C_LOW_U, C_ETC,   C_ETC,
    C_ETC,   C_ETC,   C_ETC,   C_LCURB, C_ETC,   C_RCURB, C_ETC,   C_ETC
};


/*
    The state codes.
*/
enum states {
    GO,  /* start    */
    OK,  /* ok       */
    OB,  /* object   */
    KE,  /* key      */
    CO,  /* colon    */
    VA,  /* value    */
    AR,  /* array    */
    ST,  /* string   */
    ES,  /* escape   */
    U1,  /* u1       */
    U2,  /* u2       */
    U3,  /* u3       */
    U4,  /* u4       */
    MI,  /* minus    */
    ZE,  /* zero     */
    IN,  /* integer  */
    FR,  /* fraction */
    E1,  /* e        */
    E2,  /* ex       */
    E3,  /* exp      */
    T1,  /* tr       */
    T2,  /* tru      */
    T3,  /* true     */
    F1,  /* fa       */
    F2,  /* fal      */
    F3,  /* fals     */
    F4,  /* false    */
    N1,  /* nu       */
    N2,  /* nul      */
    N3,  /* null     */
    NR_STATES
};


static const int state_transition_table[NR_STATES][NR_CLASSES] = {
/*
    The state transition table takes the current state and the current symbol,
    and returns either a new state or an action. An action is represented as a
    negative number. A JSON text is accepted if at the end of the text the
    state is OK and if the mode is MODE_DONE.

                 white                                      1-9                                   ABCDF  etc
             space |  {  }  [  ]  :  ,  "  \  /  +  -  .  0  |  a  b  c  d  e  f  l  n  r  s  t  u  |  E  |*/
/*start  GO*/ {GO,GO,-6,__,-5,__,__,__,ST,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*ok     OK*/ {OK,OK,__,-8,__,-7,__,-3,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*object OB*/ {OB,OB,__,-9,__,__,__,__,ST,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*key    KE*/ {KE,KE,__,__,__,__,__,__,ST,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*colon  CO*/ {CO,CO,__,__,__,__,-2,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*value  VA*/ {VA,VA,-6,__,-5,__,__,__,ST,__,__,__,MI,__,ZE,IN,__,__,__,__,__,F1,__,N1,__,__,T1,__,__,__,__},
/*array  AR*/ {AR,AR,-6,__,-5,-7,__,__,ST,__,__,__,MI,__,ZE,IN,__,__,__,__,__,F1,__,N1,__,__,T1,__,__,__,__},
/*string ST*/ {ST,__,ST,ST,ST,ST,ST,ST,-4,ES,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST},
/*escape ES*/ {__,__,__,__,__,__,__,__,ST,ST,ST,__,__,__,__,__,__,ST,__,__,__,ST,__,ST,ST,__,ST,U1,__,__,__},
/*u1     U1*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,U2,U2,U2,U2,U2,U2,U2,U2,__,__,__,__,__,__,U2,U2,__},
/*u2     U2*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,U3,U3,U3,U3,U3,U3,U3,U3,__,__,__,__,__,__,U3,U3,__},
/*u3     U3*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,U4,U4,U4,U4,U4,U4,U4,U4,__,__,__,__,__,__,U4,U4,__},
/*u4     U4*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,ST,ST,ST,ST,ST,ST,ST,ST,__,__,__,__,__,__,ST,ST,__},
/*minus  MI*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,ZE,IN,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*zero   ZE*/ {OK,OK,__,-8,__,-7,__,-3,__,__,__,__,__,FR,__,__,__,__,__,__,E1,__,__,__,__,__,__,__,__,E1,__},
/*int    IN*/ {OK,OK,__,-8,__,-7,__,-3,__,__,__,__,__,FR,IN,IN,__,__,__,__,E1,__,__,__,__,__,__,__,__,E1,__},
/*frac   FR*/ {OK,OK,__,-8,__,-7,__,-3,__,__,__,__,__,__,FR,FR,__,__,__,__,E1,__,__,__,__,__,__,__,__,E1,__},
/*e      E1*/ {__,__,__,__,__,__,__,__,__,__,__,E2,E2,__,E3,E3,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*ex     E2*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,E3,E3,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*exp    E3*/ {OK,OK,__,-8,__,-7,__,-3,__,__,__,__,__,__,E3,E3,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*tr     T1*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,T2,__,__,__,__,__,__},
/*tru    T2*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,T3,__,__,__},
/*true   T3*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__,__,__,__,__},
/*fa     F1*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,F2,__,__,__,__,__,__,__,__,__,__,__,__,__,__},
/*fal    F2*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,F3,__,__,__,__,__,__,__,__},
/*fals   F3*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,F4,__,__,__,__,__},
/*false  F4*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__,__,__,__,__},
/*nu     N1*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,N2,__,__,__},
/*nul    N2*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,N3,__,__,__,__,__,__,__,__},
/*null   N3*/ {__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__,__,__},
};


/*
    These modes can be pushed on the stack.
*/
enum modes {
    MODE_ARRAY, 
    MODE_DONE,  
    MODE_KEY,   
    MODE_OBJECT,
};


/*
    Push a mode onto the stack. Return false if there is overflow.
*/
static int
push(JSON_parser jp, int mode)
{
    jp->top += 1;
    if (jp->top >= jp->depth) {
		jp->error_code = PHP_JSON_ERROR_DEPTH;
        return false;
    }
    jp->stack[jp->top] = mode;
    return true;
}


/*
    Pop the stack, assuring that the current mode matches the expectation.
    Return false if there is underflow or if the modes mismatch.
*/
static int
pop(JSON_parser jp, int mode)
{
    if (jp->top < 0 || jp->stack[jp->top] != mode) {
		jp->error_code = PHP_JSON_ERROR_STATE_MISMATCH;
        return false;
    }
    jp->top -= 1;
    return true;
}

/*
    new_JSON_checker starts the checking process by constructing a JSON_checker
    object. It takes a depth parameter that restricts the level of maximum
    nesting.

    To continue the process, call JSON_checker_char for each character in the
    JSON text, and then call JSON_checker_done to obtain the final result.
    These functions are fully reentrant.

    The JSON_checker object will be deleted by JSON_checker_done.
    JSON_checker_char will delete the JSON_checker object if it sees an error.
*/
JSON_parser
new_JSON_parser(int depth)
{
    JSON_parser jp = (JSON_parser)emalloc(sizeof(struct JSON_parser_struct));
    jp->state = GO;
    jp->depth = depth;
    jp->top = -1;
	jp->error_code = PHP_JSON_ERROR_NONE;
    jp->stack = (int*)ecalloc(depth, sizeof(int));
    if (depth > JSON_PARSER_DEFAULT_DEPTH) {
        jp->the_zstack = (zval **)safe_emalloc(depth, sizeof(zval), 0);
    } else {
        jp->the_zstack = &jp->the_static_zstack[0];
    }
    push(jp, MODE_DONE);
    return jp;
}

/*
    Delete the JSON_parser object.
*/
int
free_JSON_parser(JSON_parser jp)
{
    efree((void*)jp->stack);
    if (jp->the_zstack != &jp->the_static_zstack[0]) {
        efree(jp->the_zstack);
    }
    efree((void*)jp);
    return false;
}

static int dehexchar(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'F')
    {
        return c - ('A' - 10);
    }
    else if (c >= 'a' && c <= 'f')
    {
        return c - ('a' - 10);
    }
    else
    {
        return -1;
    }
}


static void json_create_zval(zval **z, smart_str *buf, int type, int options)
{
    ALLOC_INIT_ZVAL(*z);

    if (type == IS_LONG)
    {
		zend_bool bigint = 0;

		if (buf->c[0] == '-') {
			buf->len--;
		}

		if (buf->len >= MAX_LENGTH_OF_LONG - 1) {
			if (buf->len == MAX_LENGTH_OF_LONG - 1) {
				int cmp = strcmp(buf->c + (buf->c[0] == '-'), long_min_digits);

				if (!(cmp < 0 || (cmp == 0 && buf->c[0] == '-'))) {
					bigint = 1;
				}
			} else {
				bigint = 1;
			}
		}

		if (bigint) {
			/* value too large to represent as a long */
			if (options & PHP_JSON_BIGINT_AS_STRING) {
				if (buf->c[0] == '-') {
					/* Restore last char consumed above */
					buf->len++;
				}
				goto use_string;
			} else {
				goto use_double;
			}
		}

		ZVAL_LONG(*z, strtol(buf->c, NULL, 10));
    }
    else if (type == IS_DOUBLE)
    {
use_double:
        ZVAL_DOUBLE(*z, zend_strtod(buf->c, NULL));
    }
    else if (type == IS_STRING)
    {
use_string:
        ZVAL_STRINGL(*z, buf->c, buf->len, 1);
    }
    else if (type == IS_BOOL)
    {
        ZVAL_BOOL(*z, (*(buf->c) == 't'));
    }
    else /* type == IS_NULL) || type unknown */
    {
        ZVAL_NULL(*z);
    }
}


static void utf16_to_utf8(smart_str *buf, unsigned short utf16)
{
    if (utf16 < 0x80)
    {
        smart_str_appendc(buf, (unsigned char) utf16);
    }
    else if (utf16 < 0x800)
    {
        smart_str_appendc(buf, 0xc0 | (utf16 >> 6));
        smart_str_appendc(buf, 0x80 | (utf16 & 0x3f));
    }
    else if ((utf16 & 0xfc00) == 0xdc00
                && buf->len >= 3
                && ((unsigned char) buf->c[buf->len - 3]) == 0xed
                && ((unsigned char) buf->c[buf->len - 2] & 0xf0) == 0xa0
                && ((unsigned char) buf->c[buf->len - 1] & 0xc0) == 0x80)
    {
        /* found surrogate pair */
        unsigned long utf32;

        utf32 = (((buf->c[buf->len - 2] & 0xf) << 16)
                    | ((buf->c[buf->len - 1] & 0x3f) << 10)
                    | (utf16 & 0x3ff)) + 0x10000;
        buf->len -= 3;

        smart_str_appendc(buf, (unsigned char) (0xf0 | (utf32 >> 18)));
        smart_str_appendc(buf, 0x80 | ((utf32 >> 12) & 0x3f));
        smart_str_appendc(buf, 0x80 | ((utf32 >> 6) & 0x3f));
        smart_str_appendc(buf, 0x80 | (utf32 & 0x3f));
    }
    else
    {
        smart_str_appendc(buf, 0xe0 | (utf16 >> 12));
        smart_str_appendc(buf, 0x80 | ((utf16 >> 6) & 0x3f));
        smart_str_appendc(buf, 0x80 | (utf16 & 0x3f));
    }
}

static void attach_zval(JSON_parser jp, int up, int cur, smart_str *key, int assoc TSRMLS_DC)
{
    zval *root = jp->the_zstack[up];
    zval *child =  jp->the_zstack[cur];
    int up_mode = jp->stack[up];

    if (up_mode == MODE_ARRAY)
    {
        add_next_index_zval(root, child);
    }
    else if (up_mode == MODE_OBJECT)
    {
        if (!assoc)
        {
            add_property_zval_ex(root, (key->len ? key->c : "_empty_"), (key->len ? (key->len + 1) : sizeof("_empty_")), child TSRMLS_CC);
            Z_DELREF_P(child);
        }
        else
        {
            add_assoc_zval_ex(root, (key->len ? key->c : ""), (key->len ? (key->len + 1) : sizeof("")), child);
        }
        key->len = 0;
    }
}


#define FREE_BUFFERS() smart_str_free(&buf); smart_str_free(&key);
#define SWAP_BUFFERS(from, to) do { \
        char *t1 = from.c; \
        int t2 = from.a; \
        from.c = to.c; \
        from.a = to.a; \
        to.c = t1; \
        to.a = t2; \
        to.len = from.len; \
        from.len = 0; \
        } while(0);
#define JSON_RESET_TYPE() type = -1;

/*
    The JSON_parser takes a UTF-16 encoded string and determines if it is a
    syntactically correct JSON text. Along the way, it creates a PHP variable.

    It is implemented as a Pushdown Automaton; that means it is a finite state
    machine with a stack.
*/
int
parse_JSON_ex(JSON_parser jp, zval *z, unsigned short utf16_json[], int length, int options TSRMLS_DC)
{
    int next_char;  /* the next character */
    int next_class;  /* the next character class */
    int next_state;  /* the next state */
    int the_index;
    int assoc = options & PHP_JSON_OBJECT_AS_ARRAY;

    smart_str buf = {0};
    smart_str key = {0};

    unsigned short utf16 = 0;
    int type;

	JSON_RESET_TYPE();

    for (the_index = 0; the_index < length; the_index += 1) {
        next_char = utf16_json[the_index];
		if (next_char >= 128) {
			next_class = C_ETC;
		} else {
			next_class = ascii_class[next_char];
			if (next_class <= __) {
				jp->error_code = PHP_JSON_ERROR_CTRL_CHAR;
				FREE_BUFFERS();
				return false;
			}
		}
/*
    Get the next state from the transition table.
*/
        next_state = state_transition_table[jp->state][next_class];
        if (next_state >= 0) {
/*
    Change the state and iterate
*/
			if (type == IS_STRING) {
	            if (next_state == ST && jp->state != U4) {
	                if (jp->state != ES) {
	                    utf16_to_utf8(&buf, next_char);
	                } else {
	                    switch (next_char) {
	                        case 'b':
	                            smart_str_appendc(&buf, '\b');
	                            break;
	                        case 't':
	                            smart_str_appendc(&buf, '\t');
	                            break;
	                        case 'n':
	                            smart_str_appendc(&buf, '\n');
	                            break;
	                        case 'f':
	                            smart_str_appendc(&buf, '\f');
	                            break;
	                        case 'r':
	                            smart_str_appendc(&buf, '\r');
	                            break;
	                        default:
	                            utf16_to_utf8(&buf, next_char);
	                            break;
	                    }
	                }
	            } else if (next_state == U2) {
	                utf16 = dehexchar(next_char) << 12;
	            } else if (next_state == U3) {
	                utf16 += dehexchar(next_char) << 8;
	            } else if (next_state == U4) {
	                utf16 += dehexchar(next_char) << 4;
	            } else if (next_state == ST && jp->state == U4) {
	                utf16 += dehexchar(next_char);
	                utf16_to_utf8(&buf, utf16);
	            }
	        } else if (type < IS_LONG && (next_class == C_DIGIT || next_class == C_ZERO)) {
	            type = IS_LONG;
	            smart_str_appendc(&buf, next_char);
	        } else if (type == IS_LONG && next_state == E1) {
	            type = IS_DOUBLE;
	            smart_str_appendc(&buf, next_char);
	        } else if (type < IS_DOUBLE && next_class == C_POINT) {
	            type = IS_DOUBLE;
	            smart_str_appendc(&buf, next_char);
	        } else if (type < IS_STRING && next_class == C_QUOTE) {
	            type = IS_STRING;
	        } else if (type < IS_BOOL && ((jp->state == T3 && next_state == OK) || (jp->state == F4 && next_state == OK))) {
	            type = IS_BOOL;
	        } else if (type < IS_NULL && jp->state == N3 && next_state == OK) {
	            type = IS_NULL;
	        } else if (type != IS_STRING && next_class > C_WHITE) {
	            utf16_to_utf8(&buf, next_char);
	        }
        	jp->state = next_state;
		} else {
/*
    Perform one of the predefined actions.
*/
            switch (next_state) {
/* empty } */
            case -9:
                if (!pop(jp, MODE_KEY)) {
                    FREE_BUFFERS();
                    return false;
                }
                jp->state = OK;
                break;
/* } */
            case -8:
                if (type != -1 && jp->stack[jp->top] == MODE_OBJECT)
                {
                    zval *mval;
                    smart_str_0(&buf);

                    json_create_zval(&mval, &buf, type, options);

                    if (!assoc) {
                        add_property_zval_ex(jp->the_zstack[jp->top], (key.len ? key.c : "_empty_"), (key.len ? (key.len + 1) : sizeof("_empty_")), mval TSRMLS_CC);
                        Z_DELREF_P(mval);
                    } else {
                        add_assoc_zval_ex(jp->the_zstack[jp->top], (key.len ? key.c : ""), (key.len ? (key.len + 1) : sizeof("")), mval);
                    }
                    key.len = 0;
                    buf.len = 0;
                    JSON_RESET_TYPE();
                }


                if (!pop(jp, MODE_OBJECT)) {
                    FREE_BUFFERS();
                    return false;
                }
                jp->state = OK;
                break;
/* ] */
            case -7:
            {
                if (type != -1 && jp->stack[jp->top] == MODE_ARRAY)
                {
                    zval *mval;
                    smart_str_0(&buf);

                    json_create_zval(&mval, &buf, type, options);
                    add_next_index_zval(jp->the_zstack[jp->top], mval);
                    buf.len = 0;
                    JSON_RESET_TYPE();
                }

                if (!pop(jp, MODE_ARRAY)) {
                    FREE_BUFFERS();
                    return false;
                }
                jp->state = OK;
            }
			break;
/* { */
            case -6:
                if (!push(jp, MODE_KEY)) {
                    FREE_BUFFERS();
                    return false;
                }

                jp->state = OB;
                if (jp->top > 0) {
                    zval *obj;

                    if (jp->top == 1) {
                        obj = z;
                  	} else {
                        ALLOC_INIT_ZVAL(obj);
                    }

                    if (!assoc) {
                        object_init(obj);
                    } else {
                        array_init(obj);
                    }

                    jp->the_zstack[jp->top] = obj;

                    if (jp->top > 1) {
                        attach_zval(jp, jp->top - 1, jp->top, &key, assoc TSRMLS_CC);
                    }

                    JSON_RESET_TYPE();
                }

                break;
/* [ */
            case -5:
                if (!push(jp, MODE_ARRAY)) {
                    FREE_BUFFERS();
                    return false;
                }
                jp->state = AR;

                if (jp->top > 0) {
                    zval *arr;

                    if (jp->top == 1) {
                        arr = z;
                    } else {
                        ALLOC_INIT_ZVAL(arr);
                    }

                    array_init(arr);
                    jp->the_zstack[jp->top] = arr;

                    if (jp->top > 1) {
                        attach_zval(jp, jp->top - 1, jp->top, &key, assoc TSRMLS_CC);
                    }

                    JSON_RESET_TYPE();
                }

                break;

/* " */
            case -4:
                switch (jp->stack[jp->top]) {
                case MODE_KEY:
                    jp->state = CO;
                    smart_str_0(&buf);
                    SWAP_BUFFERS(buf, key);
                    JSON_RESET_TYPE();
                    break;
                case MODE_ARRAY:
                case MODE_OBJECT:
                    jp->state = OK;
                    break;
				case MODE_DONE:
					if (type == IS_STRING) {
						smart_str_0(&buf);
						ZVAL_STRINGL(z, buf.c, buf.len, 1);
						jp->state = OK;
						break;
					}
					/* fall through if not IS_STRING */
                default:
                    FREE_BUFFERS();
                    jp->error_code = PHP_JSON_ERROR_SYNTAX;
                    return false;
                }
                break;
/* , */
            case -3:
            {
                zval *mval;

                if (type != -1 &&
                    (jp->stack[jp->top] == MODE_OBJECT ||
                     jp->stack[jp->top] == MODE_ARRAY))
                {
                    smart_str_0(&buf);
                    json_create_zval(&mval, &buf, type, options);
                }

                switch (jp->stack[jp->top]) {
                    case MODE_OBJECT:
                        if (pop(jp, MODE_OBJECT) && push(jp, MODE_KEY)) {
                            if (type != -1) {
                                if (!assoc) {
                                    add_property_zval_ex(jp->the_zstack[jp->top], (key.len ? key.c : "_empty_"), (key.len ? (key.len + 1) : sizeof("_empty_")), mval TSRMLS_CC);
                                    Z_DELREF_P(mval);
                                } else {
                                    add_assoc_zval_ex(jp->the_zstack[jp->top], (key.len ? key.c : ""), (key.len ? (key.len + 1) : sizeof("")), mval);
                                }
                                key.len = 0;
                            }
                            jp->state = KE;
                        }
                        break;
                    case MODE_ARRAY:
                        if (type != -1) {
                            add_next_index_zval(jp->the_zstack[jp->top], mval);
                        }
                        jp->state = VA;
                        break;
                    default:
                        FREE_BUFFERS();
                        jp->error_code = PHP_JSON_ERROR_SYNTAX;
                        return false;
                }
                buf.len = 0;
                JSON_RESET_TYPE();
            }
            break;
/* : */
            case -2:
                if (pop(jp, MODE_KEY) && push(jp, MODE_OBJECT)) {
                    jp->state = VA;
                    break;
                }
/*
    syntax error
*/
            default:
                {
					jp->error_code = PHP_JSON_ERROR_SYNTAX;
                    FREE_BUFFERS();
                    return false;
                }
            }
        }
    }

    FREE_BUFFERS();
	if (jp->state == OK && pop(jp, MODE_DONE)) {
		return true;
	}

	jp->error_code = PHP_JSON_ERROR_SYNTAX;
	return false;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4
 */
