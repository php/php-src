/**********************************************************************

  sprintf.c -

  $Author$
  created at: Fri Oct 15 10:39:26 JST 1993

  Copyright (C) 1993-2007 Yukihiro Matsumoto
  Copyright (C) 2000  Network Applied Communication Laboratory, Inc.
  Copyright (C) 2000  Information-technology Promotion Agency, Japan

**********************************************************************/

#include "ruby/ruby.h"
#include "ruby/re.h"
#include "ruby/encoding.h"
#include <math.h>
#include <stdarg.h>

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

#define BIT_DIGITS(N)   (((N)*146)/485 + 1)  /* log2(10) =~ 146/485 */
#define BITSPERDIG (SIZEOF_BDIGITS*CHAR_BIT)
#define EXTENDSIGN(n, l) (((~0 << (n)) >> (((n)*(l)) % BITSPERDIG)) & ~(~0 << (n)))

static void fmt_setup(char*,size_t,int,int,int,int);

static char*
remove_sign_bits(char *str, int base)
{
    char *t = str;

    if (base == 16) {
	while (*t == 'f') {
	    t++;
	}
    }
    else if (base == 8) {
	*t |= EXTENDSIGN(3, strlen(t));
	while (*t == '7') {
	    t++;
	}
    }
    else if (base == 2) {
	while (*t == '1') {
	    t++;
	}
    }

    return t;
}

static char
sign_bits(int base, const char *p)
{
    char c = '.';

    switch (base) {
      case 16:
	if (*p == 'X') c = 'F';
	else c = 'f';
	break;
      case 8:
	c = '7'; break;
      case 2:
	c = '1'; break;
    }
    return c;
}

#define FNONE  0
#define FSHARP 1
#define FMINUS 2
#define FPLUS  4
#define FZERO  8
#define FSPACE 16
#define FWIDTH 32
#define FPREC  64
#define FPREC0 128

#define CHECK(l) do {\
    int cr = ENC_CODERANGE(result);\
    while (blen + (l) >= bsiz) {\
	bsiz*=2;\
    }\
    rb_str_resize(result, bsiz);\
    ENC_CODERANGE_SET(result, cr);\
    buf = RSTRING_PTR(result);\
} while (0)

#define PUSH(s, l) do { \
    CHECK(l);\
    memcpy(&buf[blen], (s), (l));\
    blen += (l);\
} while (0)

#define FILL(c, l) do { \
    CHECK(l);\
    memset(&buf[blen], (c), (l));\
    blen += (l);\
} while (0)

#define GETARG() (nextvalue != Qundef ? nextvalue : \
    posarg == -1 ? \
    (rb_raise(rb_eArgError, "unnumbered(%d) mixed with numbered", nextarg), 0) : \
    posarg == -2 ? \
    (rb_raise(rb_eArgError, "unnumbered(%d) mixed with named", nextarg), 0) : \
    (posarg = nextarg++, GETNTHARG(posarg)))

#define GETPOSARG(n) (posarg > 0 ? \
    (rb_raise(rb_eArgError, "numbered(%d) after unnumbered(%d)", (n), posarg), 0) : \
    posarg == -2 ? \
    (rb_raise(rb_eArgError, "numbered(%d) after named", (n)), 0) : \
    (((n) < 1) ? (rb_raise(rb_eArgError, "invalid index - %d$", (n)), 0) : \
	       (posarg = -1, GETNTHARG(n))))

#define GETNTHARG(nth) \
    (((nth) >= argc) ? (rb_raise(rb_eArgError, "too few arguments"), 0) : argv[(nth)])

#define GETNAMEARG(id, name, len) ( \
    posarg > 0 ? \
    (rb_raise(rb_eArgError, "named%.*s after unnumbered(%d)", (len), (name), posarg), 0) : \
    posarg == -1 ? \
    (rb_raise(rb_eArgError, "named%.*s after numbered", (len), (name)), 0) :	\
    (posarg = -2, rb_hash_lookup2(get_hash(&hash, argc, argv), (id), Qundef)))

#define GETNUM(n, val) \
    for (; p < end && rb_enc_isdigit(*p, enc); p++) {	\
	int next_n = 10 * (n) + (*p - '0'); \
        if (next_n / 10 != (n)) {\
	    rb_raise(rb_eArgError, #val " too big"); \
	} \
	(n) = next_n; \
    } \
    if (p >= end) { \
	rb_raise(rb_eArgError, "malformed format string - %%*[0-9]"); \
    }

#define GETASTER(val) do { \
    t = p++; \
    n = 0; \
    GETNUM(n, (val)); \
    if (*p == '$') { \
	tmp = GETPOSARG(n); \
    } \
    else { \
	tmp = GETARG(); \
	p = t; \
    } \
    (val) = NUM2INT(tmp); \
} while (0)

static VALUE
get_hash(volatile VALUE *hash, int argc, const VALUE *argv)
{
    VALUE tmp;

    if (*hash != Qundef) return *hash;
    if (argc != 2) {
	rb_raise(rb_eArgError, "one hash required");
    }
    tmp = rb_check_convert_type(argv[1], T_HASH, "Hash", "to_hash");
    if (NIL_P(tmp)) {
	rb_raise(rb_eArgError, "one hash required");
    }
    return (*hash = tmp);
}

/*
 *  call-seq:
 *     format(format_string [, arguments...] )   -> string
 *     sprintf(format_string [, arguments...] )  -> string
 *
 *  Returns the string resulting from applying <i>format_string</i> to
 *  any additional arguments.  Within the format string, any characters
 *  other than format sequences are copied to the result.
 *
 *  The syntax of a format sequence is follows.
 *
 *    %[flags][width][.precision]type
 *
 *  A format
 *  sequence consists of a percent sign, followed by optional flags,
 *  width, and precision indicators, then terminated with a field type
 *  character.  The field type controls how the corresponding
 *  <code>sprintf</code> argument is to be interpreted, while the flags
 *  modify that interpretation.
 *
 *  The field type characters are:
 *
 *      Field |  Integer Format
 *      ------+--------------------------------------------------------------
 *        b   | Convert argument as a binary number.
 *            | Negative numbers will be displayed as a two's complement
 *            | prefixed with `..1'.
 *        B   | Equivalent to `b', but uses an uppercase 0B for prefix
 *            | in the alternative format by #.
 *        d   | Convert argument as a decimal number.
 *        i   | Identical to `d'.
 *        o   | Convert argument as an octal number.
 *            | Negative numbers will be displayed as a two's complement
 *            | prefixed with `..7'.
 *        u   | Identical to `d'.
 *        x   | Convert argument as a hexadecimal number.
 *            | Negative numbers will be displayed as a two's complement
 *            | prefixed with `..f' (representing an infinite string of
 *            | leading 'ff's).
 *        X   | Equivalent to `x', but uses uppercase letters.
 *
 *      Field |  Float Format
 *      ------+--------------------------------------------------------------
 *        e   | Convert floating point argument into exponential notation
 *            | with one digit before the decimal point as [-]d.dddddde[+-]dd.
 *            | The precision specifies the number of digits after the decimal
 *            | point (defaulting to six).
 *        E   | Equivalent to `e', but uses an uppercase E to indicate
 *            | the exponent.
 *        f   | Convert floating point argument as [-]ddd.dddddd,
 *            | where the precision specifies the number of digits after
 *            | the decimal point.
 *        g   | Convert a floating point number using exponential form
 *            | if the exponent is less than -4 or greater than or
 *            | equal to the precision, or in dd.dddd form otherwise.
 *            | The precision specifies the number of significant digits.
 *        G   | Equivalent to `g', but use an uppercase `E' in exponent form.
 *        a   | Convert floating point argument as [-]0xh.hhhhp[+-]dd,
 *            | which is consisted from optional sign, "0x", fraction part
 *            | as hexadecimal, "p", and exponential part as decimal.
 *        A   | Equivalent to `a', but use uppercase `X' and `P'.
 *
 *      Field |  Other Format
 *      ------+--------------------------------------------------------------
 *        c   | Argument is the numeric code for a single character or
 *            | a single character string itself.
 *        p   | The valuing of argument.inspect.
 *        s   | Argument is a string to be substituted.  If the format
 *            | sequence contains a precision, at most that many characters
 *            | will be copied.
 *        %   | A percent sign itself will be displayed.  No argument taken.
 *
 *  The flags modifies the behavior of the formats.
 *  The flag characters are:
 *
 *    Flag     | Applies to    | Meaning
 *    ---------+---------------+-----------------------------------------
 *    space    | bBdiouxX      | Leave a space at the start of
 *             | aAeEfgG       | non-negative numbers.
 *             | (numeric fmt) | For `o', `x', `X', `b' and `B', use
 *             |               | a minus sign with absolute value for
 *             |               | negative values.
 *    ---------+---------------+-----------------------------------------
 *    (digit)$ | all           | Specifies the absolute argument number
 *             |               | for this field.  Absolute and relative
 *             |               | argument numbers cannot be mixed in a
 *             |               | sprintf string.
 *    ---------+---------------+-----------------------------------------
 *     #       | bBoxX         | Use an alternative format.
 *             | aAeEfgG       | For the conversions `o', increase the precision
 *             |               | until the first digit will be `0' if
 *             |               | it is not formatted as complements.
 *             |               | For the conversions `x', `X', `b' and `B'
 *             |               | on non-zero, prefix the result with ``0x'',
 *             |               | ``0X'', ``0b'' and ``0B'', respectively.
 *             |               | For `a', `A', `e', `E', `f', `g', and 'G',
 *             |               | force a decimal point to be added,
 *             |               | even if no digits follow.
 *             |               | For `g' and 'G', do not remove trailing zeros.
 *    ---------+---------------+-----------------------------------------
 *    +        | bBdiouxX      | Add a leading plus sign to non-negative
 *             | aAeEfgG       | numbers.
 *             | (numeric fmt) | For `o', `x', `X', `b' and `B', use
 *             |               | a minus sign with absolute value for
 *             |               | negative values.
 *    ---------+---------------+-----------------------------------------
 *    -        | all           | Left-justify the result of this conversion.
 *    ---------+---------------+-----------------------------------------
 *    0 (zero) | bBdiouxX      | Pad with zeros, not spaces.
 *             | aAeEfgG       | For `o', `x', `X', `b' and `B', radix-1
 *             | (numeric fmt) | is used for negative numbers formatted as
 *             |               | complements.
 *    ---------+---------------+-----------------------------------------
 *    *        | all           | Use the next argument as the field width.
 *             |               | If negative, left-justify the result. If the
 *             |               | asterisk is followed by a number and a dollar
 *             |               | sign, use the indicated argument as the width.
 *
 *  Examples of flags:
 *
 *   # `+' and space flag specifies the sign of non-negative numbers.
 *   sprintf("%d", 123)  #=> "123"
 *   sprintf("%+d", 123) #=> "+123"
 *   sprintf("% d", 123) #=> " 123"
 *
 *   # `#' flag for `o' increases number of digits to show `0'.
 *   # `+' and space flag changes format of negative numbers.
 *   sprintf("%o", 123)   #=> "173"
 *   sprintf("%#o", 123)  #=> "0173"
 *   sprintf("%+o", -123) #=> "-173"
 *   sprintf("%o", -123)  #=> "..7605"
 *   sprintf("%#o", -123) #=> "..7605"
 *
 *   # `#' flag for `x' add a prefix `0x' for non-zero numbers.
 *   # `+' and space flag disables complements for negative numbers.
 *   sprintf("%x", 123)   #=> "7b"
 *   sprintf("%#x", 123)  #=> "0x7b"
 *   sprintf("%+x", -123) #=> "-7b"
 *   sprintf("%x", -123)  #=> "..f85"
 *   sprintf("%#x", -123) #=> "0x..f85"
 *   sprintf("%#x", 0)    #=> "0"
 *
 *   # `#' for `X' uses the prefix `0X'.
 *   sprintf("%X", 123)  #=> "7B"
 *   sprintf("%#X", 123) #=> "0X7B"
 *
 *   # `#' flag for `b' add a prefix `0b' for non-zero numbers.
 *   # `+' and space flag disables complements for negative numbers.
 *   sprintf("%b", 123)   #=> "1111011"
 *   sprintf("%#b", 123)  #=> "0b1111011"
 *   sprintf("%+b", -123) #=> "-1111011"
 *   sprintf("%b", -123)  #=> "..10000101"
 *   sprintf("%#b", -123) #=> "0b..10000101"
 *   sprintf("%#b", 0)    #=> "0"
 *
 *   # `#' for `B' uses the prefix `0B'.
 *   sprintf("%B", 123)  #=> "1111011"
 *   sprintf("%#B", 123) #=> "0B1111011"
 *
 *   # `#' for `e' forces to show the decimal point.
 *   sprintf("%.0e", 1)  #=> "1e+00"
 *   sprintf("%#.0e", 1) #=> "1.e+00"
 *
 *   # `#' for `f' forces to show the decimal point.
 *   sprintf("%.0f", 1234)  #=> "1234"
 *   sprintf("%#.0f", 1234) #=> "1234."
 *
 *   # `#' for `g' forces to show the decimal point.
 *   # It also disables stripping lowest zeros.
 *   sprintf("%g", 123.4)   #=> "123.4"
 *   sprintf("%#g", 123.4)  #=> "123.400"
 *   sprintf("%g", 123456)  #=> "123456"
 *   sprintf("%#g", 123456) #=> "123456."
 *
 *  The field width is an optional integer, followed optionally by a
 *  period and a precision.  The width specifies the minimum number of
 *  characters that will be written to the result for this field.
 *
 *  Examples of width:
 *
 *   # padding is done by spaces,       width=20
 *   # 0 or radix-1.             <------------------>
 *   sprintf("%20d", 123)   #=> "                 123"
 *   sprintf("%+20d", 123)  #=> "                +123"
 *   sprintf("%020d", 123)  #=> "00000000000000000123"
 *   sprintf("%+020d", 123) #=> "+0000000000000000123"
 *   sprintf("% 020d", 123) #=> " 0000000000000000123"
 *   sprintf("%-20d", 123)  #=> "123                 "
 *   sprintf("%-+20d", 123) #=> "+123                "
 *   sprintf("%- 20d", 123) #=> " 123                "
 *   sprintf("%020x", -123) #=> "..ffffffffffffffff85"
 *
 *  For
 *  numeric fields, the precision controls the number of decimal places
 *  displayed.  For string fields, the precision determines the maximum
 *  number of characters to be copied from the string.  (Thus, the format
 *  sequence <code>%10.10s</code> will always contribute exactly ten
 *  characters to the result.)
 *
 *  Examples of precisions:
 *
 *   # precision for `d', 'o', 'x' and 'b' is
 *   # minimum number of digits               <------>
 *   sprintf("%20.8d", 123)  #=> "            00000123"
 *   sprintf("%20.8o", 123)  #=> "            00000173"
 *   sprintf("%20.8x", 123)  #=> "            0000007b"
 *   sprintf("%20.8b", 123)  #=> "            01111011"
 *   sprintf("%20.8d", -123) #=> "           -00000123"
 *   sprintf("%20.8o", -123) #=> "            ..777605"
 *   sprintf("%20.8x", -123) #=> "            ..ffff85"
 *   sprintf("%20.8b", -11)  #=> "            ..110101"
 *
 *   # "0x" and "0b" for `#x' and `#b' is not counted for
 *   # precision but "0" for `#o' is counted.  <------>
 *   sprintf("%#20.8d", 123)  #=> "            00000123"
 *   sprintf("%#20.8o", 123)  #=> "            00000173"
 *   sprintf("%#20.8x", 123)  #=> "          0x0000007b"
 *   sprintf("%#20.8b", 123)  #=> "          0b01111011"
 *   sprintf("%#20.8d", -123) #=> "           -00000123"
 *   sprintf("%#20.8o", -123) #=> "            ..777605"
 *   sprintf("%#20.8x", -123) #=> "          0x..ffff85"
 *   sprintf("%#20.8b", -11)  #=> "          0b..110101"
 *
 *   # precision for `e' is number of
 *   # digits after the decimal point           <------>
 *   sprintf("%20.8e", 1234.56789) #=> "      1.23456789e+03"
 *
 *   # precision for `f' is number of
 *   # digits after the decimal point               <------>
 *   sprintf("%20.8f", 1234.56789) #=> "       1234.56789000"
 *
 *   # precision for `g' is number of
 *   # significant digits                          <------->
 *   sprintf("%20.8g", 1234.56789) #=> "           1234.5679"
 *
 *   #                                         <------->
 *   sprintf("%20.8g", 123456789)  #=> "       1.2345679e+08"
 *
 *   # precision for `s' is
 *   # maximum number of characters                    <------>
 *   sprintf("%20.8s", "string test") #=> "            string t"
 *
 *  Examples:
 *
 *     sprintf("%d %04x", 123, 123)               #=> "123 007b"
 *     sprintf("%08b '%4s'", 123, 123)            #=> "01111011 ' 123'"
 *     sprintf("%1$*2$s %2$d %1$s", "hello", 8)   #=> "   hello 8 hello"
 *     sprintf("%1$*2$s %2$d", "hello", -8)       #=> "hello    -8"
 *     sprintf("%+g:% g:%-g", 1.23, 1.23, 1.23)   #=> "+1.23: 1.23:1.23"
 *     sprintf("%u", -123)                        #=> "-123"
 *
 *  For more complex formatting, Ruby supports a reference by name.
 *  %<name>s style uses format style, but %{name} style doesn't.
 *
 *  Exapmles:
 *    sprintf("%<foo>d : %<bar>f", { :foo => 1, :bar => 2 })
 *      #=> 1 : 2.000000
 *    sprintf("%{foo}f", { :foo => 1 })
 *      # => "1f"
 */

VALUE
rb_f_sprintf(int argc, const VALUE *argv)
{
    return rb_str_format(argc - 1, argv + 1, GETNTHARG(0));
}

VALUE
rb_str_format(int argc, const VALUE *argv, VALUE fmt)
{
    rb_encoding *enc;
    const char *p, *end;
    char *buf;
    long blen, bsiz;
    VALUE result;

    long scanned = 0;
    int coderange = ENC_CODERANGE_7BIT;
    int width, prec, flags = FNONE;
    int nextarg = 1;
    int posarg = 0;
    int tainted = 0;
    VALUE nextvalue;
    VALUE tmp;
    VALUE str;
    volatile VALUE hash = Qundef;

#define CHECK_FOR_WIDTH(f)				 \
    if ((f) & FWIDTH) {					 \
	rb_raise(rb_eArgError, "width given twice");	 \
    }							 \
    if ((f) & FPREC0) {					 \
	rb_raise(rb_eArgError, "width after precision"); \
    }
#define CHECK_FOR_FLAGS(f)				 \
    if ((f) & FWIDTH) {					 \
	rb_raise(rb_eArgError, "flag after width");	 \
    }							 \
    if ((f) & FPREC0) {					 \
	rb_raise(rb_eArgError, "flag after precision"); \
    }

    ++argc;
    --argv;
    if (OBJ_TAINTED(fmt)) tainted = 1;
    StringValue(fmt);
    enc = rb_enc_get(fmt);
    fmt = rb_str_new4(fmt);
    p = RSTRING_PTR(fmt);
    end = p + RSTRING_LEN(fmt);
    blen = 0;
    bsiz = 120;
    result = rb_str_buf_new(bsiz);
    rb_enc_copy(result, fmt);
    buf = RSTRING_PTR(result);
    memset(buf, 0, bsiz);
    ENC_CODERANGE_SET(result, coderange);

    for (; p < end; p++) {
	const char *t;
	int n;
	ID id = 0;

	for (t = p; t < end && *t != '%'; t++) ;
	PUSH(p, t - p);
	if (coderange != ENC_CODERANGE_BROKEN && scanned < blen) {
	    scanned += rb_str_coderange_scan_restartable(buf+scanned, buf+blen, enc, &coderange);
	    ENC_CODERANGE_SET(result, coderange);
	}
	if (t >= end) {
	    /* end of fmt string */
	    goto sprint_exit;
	}
	p = t + 1;		/* skip `%' */

	width = prec = -1;
	nextvalue = Qundef;
      retry:
	switch (*p) {
	  default:
	    if (rb_enc_isprint(*p, enc))
		rb_raise(rb_eArgError, "malformed format string - %%%c", *p);
	    else
		rb_raise(rb_eArgError, "malformed format string");
	    break;

	  case ' ':
	    CHECK_FOR_FLAGS(flags);
	    flags |= FSPACE;
	    p++;
	    goto retry;

	  case '#':
	    CHECK_FOR_FLAGS(flags);
	    flags |= FSHARP;
	    p++;
	    goto retry;

	  case '+':
	    CHECK_FOR_FLAGS(flags);
	    flags |= FPLUS;
	    p++;
	    goto retry;

	  case '-':
	    CHECK_FOR_FLAGS(flags);
	    flags |= FMINUS;
	    p++;
	    goto retry;

	  case '0':
	    CHECK_FOR_FLAGS(flags);
	    flags |= FZERO;
	    p++;
	    goto retry;

	  case '1': case '2': case '3': case '4':
	  case '5': case '6': case '7': case '8': case '9':
	    n = 0;
	    GETNUM(n, width);
	    if (*p == '$') {
		if (nextvalue != Qundef) {
		    rb_raise(rb_eArgError, "value given twice - %d$", n);
		}
		nextvalue = GETPOSARG(n);
		p++;
		goto retry;
	    }
	    CHECK_FOR_WIDTH(flags);
	    width = n;
	    flags |= FWIDTH;
	    goto retry;

	  case '<':
	  case '{':
	    {
		const char *start = p;
		char term = (*p == '<') ? '>' : '}';

		for (; p < end && *p != term; ) {
		    p += rb_enc_mbclen(p, end, enc);
		}
		if (p >= end) {
		    rb_raise(rb_eArgError, "malformed name - unmatched parenthesis");
		}
		if (id) {
		    rb_raise(rb_eArgError, "name%.*s after <%s>",
			     (int)(p - start + 1), start, rb_id2name(id));
		}
		id = rb_intern3(start + 1, p - start - 1, enc);
		nextvalue = GETNAMEARG(ID2SYM(id), start, (int)(p - start + 1));
		if (nextvalue == Qundef) {
		    rb_raise(rb_eKeyError, "key%.*s not found", (int)(p - start + 1), start);
		}
		if (term == '}') goto format_s;
		p++;
		goto retry;
	    }

	  case '*':
	    CHECK_FOR_WIDTH(flags);
	    flags |= FWIDTH;
	    GETASTER(width);
	    if (width < 0) {
		flags |= FMINUS;
		width = -width;
	    }
	    p++;
	    goto retry;

	  case '.':
	    if (flags & FPREC0) {
		rb_raise(rb_eArgError, "precision given twice");
	    }
	    flags |= FPREC|FPREC0;

	    prec = 0;
	    p++;
	    if (*p == '*') {
		GETASTER(prec);
		if (prec < 0) {	/* ignore negative precision */
		    flags &= ~FPREC;
		}
		p++;
		goto retry;
	    }

	    GETNUM(prec, precision);
	    goto retry;

	  case '\n':
	  case '\0':
	    p--;
	  case '%':
	    if (flags != FNONE) {
		rb_raise(rb_eArgError, "invalid format character - %%");
	    }
	    PUSH("%", 1);
	    break;

	  case 'c':
	    {
		VALUE val = GETARG();
		VALUE tmp;
		unsigned int c;
		int n;

		tmp = rb_check_string_type(val);
		if (!NIL_P(tmp)) {
		    if (rb_enc_strlen(RSTRING_PTR(tmp),RSTRING_END(tmp),enc) != 1) {
			rb_raise(rb_eArgError, "%%c requires a character");
		    }
		    c = rb_enc_codepoint_len(RSTRING_PTR(tmp), RSTRING_END(tmp), &n, enc);
		    RB_GC_GUARD(tmp);
		}
		else {
		    c = NUM2INT(val);
		    n = rb_enc_codelen(c, enc);
		}
		if (n <= 0) {
		    rb_raise(rb_eArgError, "invalid character");
		}
		if (!(flags & FWIDTH)) {
		    CHECK(n);
		    rb_enc_mbcput(c, &buf[blen], enc);
		    blen += n;
		}
		else if ((flags & FMINUS)) {
		    CHECK(n);
		    rb_enc_mbcput(c, &buf[blen], enc);
		    blen += n;
		    FILL(' ', width-1);
		}
		else {
		    FILL(' ', width-1);
		    CHECK(n);
		    rb_enc_mbcput(c, &buf[blen], enc);
		    blen += n;
		}
	    }
	    break;

	  case 's':
	  case 'p':
	  format_s:
	    {
		VALUE arg = GETARG();
		long len, slen;

		if (*p == 'p') arg = rb_inspect(arg);
		str = rb_obj_as_string(arg);
		if (OBJ_TAINTED(str)) tainted = 1;
		len = RSTRING_LEN(str);
		rb_str_set_len(result, blen);
		if (coderange != ENC_CODERANGE_BROKEN && scanned < blen) {
		    int cr = coderange;
		    scanned += rb_str_coderange_scan_restartable(buf+scanned, buf+blen, enc, &cr);
		    ENC_CODERANGE_SET(result,
				      (cr == ENC_CODERANGE_UNKNOWN ?
				       ENC_CODERANGE_BROKEN : (coderange = cr)));
		}
		enc = rb_enc_check(result, str);
		if (flags&(FPREC|FWIDTH)) {
		    slen = rb_enc_strlen(RSTRING_PTR(str),RSTRING_END(str),enc);
		    if (slen < 0) {
			rb_raise(rb_eArgError, "invalid mbstring sequence");
		    }
		    if ((flags&FPREC) && (prec < slen)) {
			char *p = rb_enc_nth(RSTRING_PTR(str), RSTRING_END(str),
					     prec, enc);
			slen = prec;
			len = p - RSTRING_PTR(str);
		    }
		    /* need to adjust multi-byte string pos */
		    if ((flags&FWIDTH) && (width > slen)) {
			width -= (int)slen;
			if (!(flags&FMINUS)) {
			    CHECK(width);
			    while (width--) {
				buf[blen++] = ' ';
			    }
			}
			CHECK(len);
			memcpy(&buf[blen], RSTRING_PTR(str), len);
			RB_GC_GUARD(str);
			blen += len;
			if (flags&FMINUS) {
			    CHECK(width);
			    while (width--) {
				buf[blen++] = ' ';
			    }
			}
			rb_enc_associate(result, enc);
			break;
		    }
		}
		PUSH(RSTRING_PTR(str), len);
		RB_GC_GUARD(str);
		rb_enc_associate(result, enc);
	    }
	    break;

	  case 'd':
	  case 'i':
	  case 'o':
	  case 'x':
	  case 'X':
	  case 'b':
	  case 'B':
	  case 'u':
	    {
		volatile VALUE val = GETARG();
		char fbuf[32], nbuf[64], *s;
		const char *prefix = 0;
		int sign = 0, dots = 0;
		char sc = 0;
		long v = 0;
		int base, bignum = 0;
		int len;

		switch (*p) {
		  case 'd':
		  case 'i':
		  case 'u':
		    sign = 1; break;
		  case 'o':
		  case 'x':
		  case 'X':
		  case 'b':
		  case 'B':
		    if (flags&(FPLUS|FSPACE)) sign = 1;
		    break;
		}
		if (flags & FSHARP) {
		    switch (*p) {
		      case 'o':
			prefix = "0"; break;
		      case 'x':
			prefix = "0x"; break;
		      case 'X':
			prefix = "0X"; break;
		      case 'b':
			prefix = "0b"; break;
		      case 'B':
			prefix = "0B"; break;
		    }
		}

	      bin_retry:
		switch (TYPE(val)) {
		  case T_FLOAT:
		    if (FIXABLE(RFLOAT_VALUE(val))) {
			val = LONG2FIX((long)RFLOAT_VALUE(val));
			goto bin_retry;
		    }
		    val = rb_dbl2big(RFLOAT_VALUE(val));
		    if (FIXNUM_P(val)) goto bin_retry;
		    bignum = 1;
		    break;
		  case T_STRING:
		    val = rb_str_to_inum(val, 0, TRUE);
		    goto bin_retry;
		  case T_BIGNUM:
		    bignum = 1;
		    break;
		  case T_FIXNUM:
		    v = FIX2LONG(val);
		    break;
		  default:
		    val = rb_Integer(val);
		    goto bin_retry;
		}

		switch (*p) {
		  case 'o':
		    base = 8; break;
		  case 'x':
		  case 'X':
		    base = 16; break;
		  case 'b':
		  case 'B':
		    base = 2; break;
		  case 'u':
		  case 'd':
		  case 'i':
		  default:
		    base = 10; break;
		}

		if (!bignum) {
		    if (base == 2) {
			val = rb_int2big(v);
			goto bin_retry;
		    }
		    if (sign) {
			char c = *p;
			if (c == 'i') c = 'd'; /* %d and %i are identical */
			if (v < 0) {
			    v = -v;
			    sc = '-';
			    width--;
			}
			else if (flags & FPLUS) {
			    sc = '+';
			    width--;
			}
			else if (flags & FSPACE) {
			    sc = ' ';
			    width--;
			}
			snprintf(fbuf, sizeof(fbuf), "%%l%c", c);
			snprintf(nbuf, sizeof(nbuf), fbuf, v);
			s = nbuf;
		    }
		    else {
			s = nbuf;
			if (v < 0) {
			    dots = 1;
			}
			snprintf(fbuf, sizeof(fbuf), "%%l%c", *p == 'X' ? 'x' : *p);
			snprintf(++s, sizeof(nbuf) - 1, fbuf, v);
			if (v < 0) {
			    char d = 0;

			    s = remove_sign_bits(s, base);
			    switch (base) {
			      case 16:
				d = 'f'; break;
			      case 8:
				d = '7'; break;
			    }
			    if (d && *s != d) {
				*--s = d;
			    }
			}
		    }
		    len = (int)strlen(s);
		}
		else {
		    if (sign) {
			tmp = rb_big2str(val, base);
			s = RSTRING_PTR(tmp);
			if (s[0] == '-') {
			    s++;
			    sc = '-';
			    width--;
			}
			else if (flags & FPLUS) {
			    sc = '+';
			    width--;
			}
			else if (flags & FSPACE) {
			    sc = ' ';
			    width--;
			}
		    }
		    else {
			if (!RBIGNUM_SIGN(val)) {
			    val = rb_big_clone(val);
			    rb_big_2comp(val);
			}
			tmp = rb_big2str0(val, base, RBIGNUM_SIGN(val));
			s = RSTRING_PTR(tmp);
			if (*s == '-') {
			    dots = 1;
			    if (base == 10) {
				rb_warning("negative number for %%u specifier");
			    }
			    s = remove_sign_bits(++s, base);
			    switch (base) {
			      case 16:
				if (s[0] != 'f') *--s = 'f'; break;
			      case 8:
				if (s[0] != '7') *--s = '7'; break;
			      case 2:
				if (s[0] != '1') *--s = '1'; break;
			    }
			}
		    }
		    len = rb_long2int(RSTRING_END(tmp) - s);
		}

		if (dots) {
		    prec -= 2;
		    width -= 2;
		}

		if (*p == 'X') {
		    char *pp = s;
		    int c;
		    while ((c = (int)(unsigned char)*pp) != 0) {
			*pp = rb_enc_toupper(c, enc);
			pp++;
		    }
		}
		if (prefix && !prefix[1]) { /* octal */
		    if (dots) {
			prefix = 0;
		    }
		    else if (len == 1 && *s == '0') {
			len = 0;
			if (flags & FPREC) prec--;
		    }
		    else if ((flags & FPREC) && (prec > len)) {
			prefix = 0;
		    }
		}
		else if (len == 1 && *s == '0') {
		    prefix = 0;
		}
		if (prefix) {
		    width -= (int)strlen(prefix);
		}
		if ((flags & (FZERO|FMINUS|FPREC)) == FZERO) {
		    prec = width;
		    width = 0;
		}
		else {
		    if (prec < len) {
			if (!prefix && prec == 0 && len == 1 && *s == '0') len = 0;
			prec = len;
		    }
		    width -= prec;
		}
		if (!(flags&FMINUS)) {
		    CHECK(width);
		    while (width-- > 0) {
			buf[blen++] = ' ';
		    }
		}
		if (sc) PUSH(&sc, 1);
		if (prefix) {
		    int plen = (int)strlen(prefix);
		    PUSH(prefix, plen);
		}
		CHECK(prec - len);
		if (dots) PUSH("..", 2);
		if (!bignum && v < 0) {
		    char c = sign_bits(base, p);
		    while (len < prec--) {
			buf[blen++] = c;
		    }
		}
		else if ((flags & (FMINUS|FPREC)) != FMINUS) {
		    char c;

		    if (!sign && bignum && !RBIGNUM_SIGN(val))
			c = sign_bits(base, p);
		    else
			c = '0';
		    while (len < prec--) {
			buf[blen++] = c;
		    }
		}
		PUSH(s, len);
		RB_GC_GUARD(tmp);
		CHECK(width);
		while (width-- > 0) {
		    buf[blen++] = ' ';
		}
	    }
	    break;

	  case 'f':
	  case 'g':
	  case 'G':
	  case 'e':
	  case 'E':
	  case 'a':
	  case 'A':
	    {
		VALUE val = GETARG();
		double fval;
		int i, need = 6;
		char fbuf[32];

		fval = RFLOAT_VALUE(rb_Float(val));
		if (isnan(fval) || isinf(fval)) {
		    const char *expr;

		    if (isnan(fval)) {
			expr = "NaN";
		    }
		    else {
			expr = "Inf";
		    }
		    need = (int)strlen(expr);
		    if ((!isnan(fval) && fval < 0.0) || (flags & FPLUS))
			need++;
		    if ((flags & FWIDTH) && need < width)
			need = width;

		    CHECK(need + 1);
		    snprintf(&buf[blen], need + 1, "%*s", need, "");
		    if (flags & FMINUS) {
			if (!isnan(fval) && fval < 0.0)
			    buf[blen++] = '-';
			else if (flags & FPLUS)
			    buf[blen++] = '+';
			else if (flags & FSPACE)
			    blen++;
			memcpy(&buf[blen], expr, strlen(expr));
		    }
		    else {
			if (!isnan(fval) && fval < 0.0)
			    buf[blen + need - strlen(expr) - 1] = '-';
			else if (flags & FPLUS)
			    buf[blen + need - strlen(expr) - 1] = '+';
			else if ((flags & FSPACE) && need > width)
			    blen++;
			memcpy(&buf[blen + need - strlen(expr)], expr,
			       strlen(expr));
		    }
		    blen += strlen(&buf[blen]);
		    break;
		}

		fmt_setup(fbuf, sizeof(fbuf), *p, flags, width, prec);
		need = 0;
		if (*p != 'e' && *p != 'E') {
		    i = INT_MIN;
		    frexp(fval, &i);
		    if (i > 0)
			need = BIT_DIGITS(i);
		}
		need += (flags&FPREC) ? prec : 6;
		if ((flags&FWIDTH) && need < width)
		    need = width;
		need += 20;

		CHECK(need);
		snprintf(&buf[blen], need, fbuf, fval);
		blen += strlen(&buf[blen]);
	    }
	    break;
	}
	flags = FNONE;
    }

  sprint_exit:
    RB_GC_GUARD(fmt);
    /* XXX - We cannot validate the number of arguments if (digit)$ style used.
     */
    if (posarg >= 0 && nextarg < argc) {
	const char *mesg = "too many arguments for format string";
	if (RTEST(ruby_debug)) rb_raise(rb_eArgError, "%s", mesg);
	if (RTEST(ruby_verbose)) rb_warn("%s", mesg);
    }
    rb_str_resize(result, blen);

    if (tainted) OBJ_TAINT(result);
    return result;
}

static void
fmt_setup(char *buf, size_t size, int c, int flags, int width, int prec)
{
    char *end = buf + size;
    *buf++ = '%';
    if (flags & FSHARP) *buf++ = '#';
    if (flags & FPLUS)  *buf++ = '+';
    if (flags & FMINUS) *buf++ = '-';
    if (flags & FZERO)  *buf++ = '0';
    if (flags & FSPACE) *buf++ = ' ';

    if (flags & FWIDTH) {
	snprintf(buf, end - buf, "%d", width);
	buf += strlen(buf);
    }

    if (flags & FPREC) {
	snprintf(buf, end - buf, ".%d", prec);
	buf += strlen(buf);
    }

    *buf++ = c;
    *buf = '\0';
}

#undef FILE
#define FILE rb_printf_buffer
#define __sbuf rb_printf_sbuf
#define __sFILE rb_printf_sfile
#undef feof
#undef ferror
#undef clearerr
#undef fileno
#if SIZEOF_LONG < SIZEOF_VOIDP
# if  SIZEOF_LONG_LONG == SIZEOF_VOIDP
#  define _HAVE_SANE_QUAD_
#  define _HAVE_LLP64_
#  define quad_t LONG_LONG
#  define u_quad_t unsigned LONG_LONG
# endif
#elif SIZEOF_LONG != SIZEOF_LONG_LONG && SIZEOF_LONG_LONG == 8
# define _HAVE_SANE_QUAD_
# define quad_t LONG_LONG
# define u_quad_t unsigned LONG_LONG
#endif
#define FLOATING_POINT 1
#define BSD__dtoa ruby_dtoa
#define BSD__hdtoa ruby_hdtoa
#include "vsnprintf.c"

static int
ruby__sfvwrite(register rb_printf_buffer *fp, register struct __suio *uio)
{
    struct __siov *iov;
    VALUE result = (VALUE)fp->_bf._base;
    char *buf = (char*)fp->_p;
    size_t len, n;
    size_t blen = buf - RSTRING_PTR(result), bsiz = fp->_w;

    if (RBASIC(result)->klass) {
	rb_raise(rb_eRuntimeError, "rb_vsprintf reentered");
    }
    if ((len = uio->uio_resid) == 0)
	return 0;
    CHECK(len);
    buf += blen;
    fp->_w = bsiz;
    for (iov = uio->uio_iov; len > 0; ++iov) {
	MEMCPY(buf, iov->iov_base, char, n = iov->iov_len);
	buf += n;
	len -= n;
    }
    fp->_p = (unsigned char *)buf;
    return 0;
}

VALUE
rb_enc_vsprintf(rb_encoding *enc, const char *fmt, va_list ap)
{
    rb_printf_buffer f;
    VALUE result;

    f._flags = __SWR | __SSTR;
    f._bf._size = 0;
    f._w = 120;
    result = rb_str_buf_new(f._w);
    if (enc) {
	if (rb_enc_mbminlen(enc) > 1) {
	    /* the implementation deeply depends on plain char */
	    rb_raise(rb_eArgError, "cannot construct wchar_t based encoding string: %s",
		     rb_enc_name(enc));
	}
	rb_enc_associate(result, enc);
    }
    f._bf._base = (unsigned char *)result;
    f._p = (unsigned char *)RSTRING_PTR(result);
    RBASIC(result)->klass = 0;
    f.vwrite = ruby__sfvwrite;
    BSD_vfprintf(&f, fmt, ap);
    RBASIC(result)->klass = rb_cString;
    rb_str_resize(result, (char *)f._p - RSTRING_PTR(result));

    return result;
}

VALUE
rb_enc_sprintf(rb_encoding *enc, const char *format, ...)
{
    VALUE result;
    va_list ap;

    va_start(ap, format);
    result = rb_enc_vsprintf(enc, format, ap);
    va_end(ap);

    return result;
}

VALUE
rb_vsprintf(const char *fmt, va_list ap)
{
    return rb_enc_vsprintf(NULL, fmt, ap);
}

VALUE
rb_sprintf(const char *format, ...)
{
    VALUE result;
    va_list ap;

    va_start(ap, format);
    result = rb_vsprintf(format, ap);
    va_end(ap);

    return result;
}

VALUE
rb_str_vcatf(VALUE str, const char *fmt, va_list ap)
{
    rb_printf_buffer f;
    VALUE klass;

    StringValue(str);
    rb_str_modify(str);
    f._flags = __SWR | __SSTR;
    f._bf._size = 0;
    f._w = rb_str_capacity(str);
    f._bf._base = (unsigned char *)str;
    f._p = (unsigned char *)RSTRING_END(str);
    klass = RBASIC(str)->klass;
    RBASIC(str)->klass = 0;
    f.vwrite = ruby__sfvwrite;
    BSD_vfprintf(&f, fmt, ap);
    RBASIC(str)->klass = klass;
    rb_str_resize(str, (char *)f._p - RSTRING_PTR(str));

    return str;
}

VALUE
rb_str_catf(VALUE str, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    str = rb_str_vcatf(str, format, ap);
    va_end(ap);

    return str;
}
