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


#include "JSON_parser.h"
#include <stdio.h>

#define true  1
#define false 0

/*
    Characters are mapped into these 32 symbol classes. This allows for
    significant reductions in the size of the state transition table.
*/

/* error */
#define S_ERR -1

/* space */
#define S_SPA 0

/* other whitespace */
#define S_WSP 1

/* {  */
#define S_LBE 2

/* } */
#define S_RBE 3

/* [ */
#define S_LBT 4

/* ] */
#define S_RBT 5

/* : */
#define S_COL 6

/* , */
#define S_COM 7

/* " */
#define S_QUO 8

/* \ */
#define S_BAC 9

/* / */
#define S_SLA 10

/* + */
#define S_PLU 11

/* - */
#define S_MIN 12

/* . */
#define S_DOT 13

/* 0 */
#define S_ZER 14

/* 123456789 */
#define S_DIG 15

/* a */
#define S__A_ 16

/* b */
#define S__B_ 17

/* c */
#define S__C_ 18

/* d */
#define S__D_ 19

/* e */
#define S__E_ 20

/* f */
#define S__F_ 21

/* l */
#define S__L_ 22

/* n */
#define S__N_ 23

/* r */
#define S__R_ 24

/* s */
#define S__S_ 25

/* t */
#define S__T_ 26

/* u */
#define S__U_ 27

/* ABCDF */
#define S_A_F 28

/* E */
#define S_E   29

/* everything else */
#define S_ETC 30


/*
    This table maps the 128 ASCII characters into the 32 character classes.
    The remaining Unicode characters should be mapped to S_ETC.
*/
static const int ascii_class[128] = {
    S_ERR, S_ERR, S_ERR, S_ERR, S_ERR, S_ERR, S_ERR, S_ERR,
    S_ERR, S_WSP, S_WSP, S_ERR, S_ERR, S_WSP, S_ERR, S_ERR,
    S_ERR, S_ERR, S_ERR, S_ERR, S_ERR, S_ERR, S_ERR, S_ERR,
    S_ERR, S_ERR, S_ERR, S_ERR, S_ERR, S_ERR, S_ERR, S_ERR,

    S_SPA, S_ETC, S_QUO, S_ETC, S_ETC, S_ETC, S_ETC, S_ETC,
    S_ETC, S_ETC, S_ETC, S_PLU, S_COM, S_MIN, S_DOT, S_SLA,
    S_ZER, S_DIG, S_DIG, S_DIG, S_DIG, S_DIG, S_DIG, S_DIG,
    S_DIG, S_DIG, S_COL, S_ETC, S_ETC, S_ETC, S_ETC, S_ETC,

    S_ETC, S_A_F, S_A_F, S_A_F, S_A_F, S_E  , S_A_F, S_ETC,
    S_ETC, S_ETC, S_ETC, S_ETC, S_ETC, S_ETC, S_ETC, S_ETC,
    S_ETC, S_ETC, S_ETC, S_ETC, S_ETC, S_ETC, S_ETC, S_ETC,
    S_ETC, S_ETC, S_ETC, S_LBT, S_BAC, S_RBT, S_ETC, S_ETC,

    S_ETC, S__A_, S__B_, S__C_, S__D_, S__E_, S__F_, S_ETC,
    S_ETC, S_ETC, S_ETC, S_ETC, S__L_, S_ETC, S__N_, S_ETC,
    S_ETC, S_ETC, S__R_, S__S_, S__T_, S__U_, S_ETC, S_ETC,
    S_ETC, S_ETC, S_ETC, S_LBE, S_ETC, S_RBE, S_ETC, S_ETC
};


/*
    The state transition table takes the current state and the current symbol,
    and returns either a new state or an action. A new state is a number between
    0 and 29. An action is a negative number between -1 and -9. A JSON text is
    accepted if the end of the text is in state 9 and mode is MODE_DONE.
*/
static const int state_transition_table[30][31] = {
/* 0*/ { 0, 0,-8,-1,-6,-1,-1,-1, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/* 1*/ { 1, 1,-1,-9,-1,-1,-1,-1, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/* 2*/ { 2, 2,-8,-1,-6,-5,-1,-1, 3,-1,-1,-1,20,-1,21,22,-1,-1,-1,-1,-1,13,-1,17,-1,-1,10,-1,-1,-1,-1},
/* 3*/ { 3,-1, 3, 3, 3, 3, 3, 3,-4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
/* 4*/ {-1,-1,-1,-1,-1,-1,-1,-1, 3, 3, 3,-1,-1,-1,-1,-1,-1, 3,-1,-1,-1, 3,-1, 3, 3,-1, 3, 5,-1,-1,-1},
/* 5*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 6, 6, 6, 6, 6, 6, 6, 6,-1,-1,-1,-1,-1,-1, 6, 6,-1},
/* 6*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 7, 7, 7, 7, 7, 7, 7, 7,-1,-1,-1,-1,-1,-1, 7, 7,-1},
/* 7*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 8, 8, 8, 8, 8, 8, 8, 8,-1,-1,-1,-1,-1,-1, 8, 8,-1},
/* 8*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 3, 3, 3, 3, 3, 3, 3, 3,-1,-1,-1,-1,-1,-1, 3, 3,-1},
/* 9*/ { 9, 9,-1,-7,-1,-5,-1,-3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*10*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,11,-1,-1,-1,-1,-1,-1},
/*11*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,12,-1,-1,-1},
/*12*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*13*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,14,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*14*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,15,-1,-1,-1,-1,-1,-1,-1,-1},
/*15*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,16,-1,-1,-1,-1,-1},
/*16*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*17*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,18,-1,-1,-1},
/*18*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,19,-1,-1,-1,-1,-1,-1,-1,-1},
/*19*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 9,-1,-1,-1,-1,-1,-1,-1,-1},
/*20*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,21,22,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*21*/ { 9, 9,-1,-7,-1,-5,-1,-3,-1,-1,-1,-1,-1,23,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*22*/ { 9, 9,-1,-7,-1,-5,-1,-3,-1,-1,-1,-1,-1,23,22,22,-1,-1,-1,-1,24,-1,-1,-1,-1,-1,-1,-1,-1,24,-1},
/*23*/ { 9, 9,-1,-7,-1,-5,-1,-3,-1,-1,-1,-1,-1,-1,23,23,-1,-1,-1,-1,24,-1,-1,-1,-1,-1,-1,-1,-1,24,-1},
/*24*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,25,25,-1,26,26,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*25*/ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,26,26,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*26*/ { 9, 9,-1,-7,-1,-5,-1,-3,-1,-1,-1,-1,-1,-1,26,26,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*27*/ {27,27,-1,-1,-1,-1,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*28*/ {28,28,-8,-1,-6,-1,-1,-1, 3,-1,-1,-1,20,-1,21,22,-1,-1,-1,-1,-1,13,-1,17,-1,-1,10,-1,-1,-1,-1},
/*29*/ {29,29,-1,-1,-1,-1,-1,-1, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};

#define JSON_PARSER_MAX_DEPTH 512


/*
   A stack maintains the states of nested structures.
*/

typedef struct json_parser
{
    int the_stack[JSON_PARSER_MAX_DEPTH];
    zval *the_zstack[JSON_PARSER_MAX_DEPTH];
    int the_top;
} json_parser;


/*
    These modes can be pushed on the PDA stack.
*/
#define MODE_DONE   1
#define MODE_KEY    2
#define MODE_OBJECT 3
#define MODE_ARRAY  4

/*
    Push a mode onto the stack. Return false if there is overflow.
*/
static int
push(json_parser *json, zval *z, int mode)
{
    json->the_top += 1;
    if (json->the_top >= JSON_PARSER_MAX_DEPTH) {
        return false;
    }

    json->the_stack[json->the_top] = mode;
    return true;
}


/*
    Pop the stack, assuring that the current mode matches the expectation.
    Return false if there is underflow or if the modes mismatch.
*/
static int
pop(json_parser *json, zval *z, int mode)
{
    if (json->the_top < 0 || json->the_stack[json->the_top] != mode) {
        return false;
    }
    json->the_stack[json->the_top] = 0;
    json->the_top -= 1;

    return true;
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


static void json_create_zval(zval **z, smart_str *buf, int type)
{
    ALLOC_INIT_ZVAL(*z);

    if (type == IS_LONG)
    {
	double d = zend_strtod(buf->c, NULL);
	if (d > LONG_MAX || d < LONG_MIN) {
		ZVAL_DOUBLE(*z, d);
	} else {
		ZVAL_LONG(*z, (long)d);
	}
    }
    else if (type == IS_DOUBLE)
    {
        ZVAL_DOUBLE(*z, zend_strtod(buf->c, NULL));
    }
    else if (type == IS_STRING)
    {
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

        smart_str_appendc(buf, 0xf0 | (utf32 >> 18));
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

static void attach_zval(json_parser *json, int up, int cur, smart_str *key, int assoc TSRMLS_DC)
{
    zval *root = json->the_zstack[up];
    zval *child =  json->the_zstack[cur];
    int up_mode = json->the_stack[up];

    if (up_mode == MODE_ARRAY)
    {
        add_next_index_zval(root, child);
    }
    else if (up_mode == MODE_OBJECT)
    {
        if (!assoc)
        {
            add_property_zval_ex(root, (key->len ? key->c : "_empty_"), (key->len ? (key->len + 1) : sizeof("_empty_")), child TSRMLS_CC);
#if PHP_MAJOR_VERSION >= 5
            ZVAL_DELREF(child);
#endif
        }
        else
        {
            add_assoc_zval_ex(root, (key->len ? key->c : ""), (key->len ? (key->len + 1) : sizeof("")), child);
        }
        key->len = 0;
    }
}


#define FREE_BUFFERS() do { smart_str_free(&buf); smart_str_free(&key); } while (0);
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
#define JSON_RESET_TYPE() do { type = -1; } while(0);
#define JSON(x) the_json.x


/*
    The JSON_parser takes a UTF-16 encoded string and determines if it is a
    syntactically correct JSON text. Along the way, it creates a PHP variable.

    It is implemented as a Pushdown Automaton; that means it is a finite state
    machine with a stack.
*/
int
JSON_parser(zval *z, unsigned short p[], int length, int assoc TSRMLS_DC)
{
    int b;  /* the next character */
    int c;  /* the next character class */
    int s;  /* the next state */
    json_parser the_json; /* the parser state */
    int the_state = 0;
    int the_index;

    smart_str buf = {0};
    smart_str key = {0};

    int type = -1;
    unsigned short utf16 = 0;

    JSON(the_top) = -1;
    push(&the_json, z, MODE_DONE);

    for (the_index = 0; the_index < length; the_index += 1) {
        b = p[the_index];
        if ((b & 127) == b) {
            c = ascii_class[b];
            if (c <= S_ERR) {
                FREE_BUFFERS();
                return false;
            }
        } else {
            c = S_ETC;
        }
/*
    Get the next state from the transition table.
*/
        s = state_transition_table[the_state][c];
        if (s < 0) {
/*
    Perform one of the predefined actions.
*/
            switch (s) {
/*
    empty }
*/
            case -9:
                if (!pop(&the_json, z, MODE_KEY)) {
                    FREE_BUFFERS();
                    return false;
                }
                the_state = 9;
                break;
/*
    {
*/
            case -8:
                if (!push(&the_json, z, MODE_KEY)) {
                    FREE_BUFFERS();
                    return false;
                }

                the_state = 1;
                if (JSON(the_top) > 0)
                {
                    zval *obj;

                    if (JSON(the_top) == 1)
                    {
                        obj = z;
                    }
                    else
                    {
                        ALLOC_INIT_ZVAL(obj);
                    }

                    if (!assoc)
                    {
                        object_init(obj);
                    }
                    else
                    {
                        array_init(obj);
                    }

                    JSON(the_zstack)[JSON(the_top)] = obj;

                    if (JSON(the_top) > 1)
                    {
                        attach_zval(&the_json, JSON(the_top-1), JSON(the_top), &key, assoc TSRMLS_CC);
                    }

                    JSON_RESET_TYPE();
                }

                break;
/*
    }
*/
            case -7:
                if (type != -1 &&
                    (JSON(the_stack)[JSON(the_top)] == MODE_OBJECT ||
                     JSON(the_stack)[JSON(the_top)] == MODE_ARRAY))
                {
                    zval *mval;
                    smart_str_0(&buf);

                    json_create_zval(&mval, &buf, type);

                    if (!assoc)
                    {
                        add_property_zval_ex(JSON(the_zstack)[JSON(the_top)], (key.len ? key.c : "_empty_"), (key.len ? (key.len + 1) : sizeof("_empty_")), mval TSRMLS_CC);
#if PHP_MAJOR_VERSION >= 5
                        ZVAL_DELREF(mval);
#endif
                    }
                    else
                    {
                        add_assoc_zval_ex(JSON(the_zstack)[JSON(the_top)], (key.len ? key.c : ""), (key.len ? (key.len + 1) : sizeof("")), mval);
                    }
                    key.len = 0;
                    buf.len = 0;
                    JSON_RESET_TYPE();
                }


                if (!pop(&the_json, z, MODE_OBJECT)) {
                    FREE_BUFFERS();
                    return false;
                }
                the_state = 9;
                break;
/*
    [
*/
            case -6:
                if (!push(&the_json, z, MODE_ARRAY)) {
                    FREE_BUFFERS();
                    return false;
                }
                the_state = 2;

                if (JSON(the_top) > 0)
                {
                    zval *arr;

                    if (JSON(the_top) == 1)
                    {
                        arr = z;
                    }
                    else
                    {
                        ALLOC_INIT_ZVAL(arr);
                    }

                    array_init(arr);
                    JSON(the_zstack)[JSON(the_top)] = arr;

                    if (JSON(the_top) > 1)
                    {
                        attach_zval(&the_json, JSON(the_top-1), JSON(the_top), &key, assoc TSRMLS_CC);
                    }

                    JSON_RESET_TYPE();
                }

                break;
/*
    ]
*/
            case -5:
            {
                if (type != -1 &&
                    (JSON(the_stack)[JSON(the_top)] == MODE_OBJECT ||
                     JSON(the_stack)[JSON(the_top)] == MODE_ARRAY))
                {
                    zval *mval;
                    smart_str_0(&buf);

                    json_create_zval(&mval, &buf, type);
                    add_next_index_zval(JSON(the_zstack)[JSON(the_top)], mval);
                    buf.len = 0;
                    JSON_RESET_TYPE();
                }

                if (!pop(&the_json, z, MODE_ARRAY)) {
                    FREE_BUFFERS();
                    return false;
                }
                the_state = 9;
            }
                break;
/*
    "
*/
            case -4:
                switch (JSON(the_stack)[JSON(the_top)]) {
                case MODE_KEY:
                    the_state = 27;
                    smart_str_0(&buf);
                    SWAP_BUFFERS(buf, key);
                    JSON_RESET_TYPE();
                    break;
                case MODE_ARRAY:
                case MODE_OBJECT:
                    the_state = 9;
                    break;
				case MODE_DONE:
					if (type == IS_STRING) {
						smart_str_0(&buf);
						ZVAL_STRINGL(z, buf.c, buf.len, 1);
						the_state = 9;
						break;
					}
					/* fall through if not IS_STRING */
                default:
                    FREE_BUFFERS();
                    return false;
                }
                break;
/*
    ,
*/
            case -3:
            {
                zval *mval;

                if (type != -1 &&
                    (JSON(the_stack)[JSON(the_top)] == MODE_OBJECT ||
                     JSON(the_stack[JSON(the_top)]) == MODE_ARRAY))
                {
                    smart_str_0(&buf);
                    json_create_zval(&mval, &buf, type);
                }

                switch (JSON(the_stack)[JSON(the_top)]) {
                    case MODE_OBJECT:
                        if (pop(&the_json, z, MODE_OBJECT) && push(&the_json, z, MODE_KEY)) {
                            if (type != -1)
                            {
                                if (!assoc)
                                {
                                    add_property_zval_ex(JSON(the_zstack)[JSON(the_top)], (key.len ? key.c : "_empty_"), (key.len ? (key.len + 1) : sizeof("_empty_")), mval TSRMLS_CC);
#if PHP_MAJOR_VERSION >= 5
                                    ZVAL_DELREF(mval);
#endif
                                }
                                else
                                {
                                    add_assoc_zval_ex(JSON(the_zstack)[JSON(the_top)], (key.len ? key.c : ""), (key.len ? (key.len + 1) : sizeof("")), mval);
                                }
                                key.len = 0;
                            }
                            the_state = 29;
                        }
                        break;
                    case MODE_ARRAY:
                        if (type != -1)
                        {
                            add_next_index_zval(JSON(the_zstack)[JSON(the_top)], mval);
                        }
                        the_state = 28;
                        break;
                    default:
                        FREE_BUFFERS();
                        return false;
                }
                buf.len = 0;
                JSON_RESET_TYPE();
            }
            break;
/*
    :
*/
            case -2:
                if (pop(&the_json, z, MODE_KEY) && push(&the_json, z, MODE_OBJECT)) {
                    the_state = 28;
                    break;
                }
/*
    syntax error
*/
            case -1:
                {
                    FREE_BUFFERS();
                    return false;
                }
            }
        } else {
/*
    Change the state and iterate.
*/
            if (type == IS_STRING)
            {
                if (s == 3 && the_state != 8)
                {
                    if (the_state != 4)
                    {
                        utf16_to_utf8(&buf, b);
                    }
                    else
                    {
                        switch (b)
                        {
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
                                utf16_to_utf8(&buf, b);
                                break;
                        }
                    }
                }
                else if (s == 6)
                {
                    utf16 = dehexchar(b) << 12;
                }
                else if (s == 7)
                {
                    utf16 += dehexchar(b) << 8;
                }
                else if (s == 8)
                {
                    utf16 += dehexchar(b) << 4;
                }
                else if (s == 3 && the_state == 8)
                {
                    utf16 += dehexchar(b);
                    utf16_to_utf8(&buf, utf16);
                }
            }
            else if (type < IS_LONG && (c == S_DIG || c == S_ZER))
            {
                type = IS_LONG;
                smart_str_appendc(&buf, b);
            }
            else if (type == IS_LONG && s == 24)
            {
                type = IS_DOUBLE;
                smart_str_appendc(&buf, b);
            }
            else if (type < IS_DOUBLE && c == S_DOT)
            {
                type = IS_DOUBLE;
                smart_str_appendc(&buf, b);
            }
            else if (type < IS_STRING && c == S_QUO)
            {
                type = IS_STRING;
            }
            else if (type < IS_BOOL && ((the_state == 12 && s == 9) || (the_state == 16 && s == 9)))
            {
                type = IS_BOOL;
            }
            else if (type < IS_NULL && the_state == 19 && s == 9)
            {
                type = IS_NULL;
            }
            else if (type != IS_STRING && c > S_WSP)
            {
                utf16_to_utf8(&buf, b);
            }

            the_state = s;
        }
    }

    FREE_BUFFERS();

    return the_state == 9 && pop(&the_json, z, MODE_DONE);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4
 */
