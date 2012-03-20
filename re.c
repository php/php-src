/**********************************************************************

  re.c -

  $Author$
  created at: Mon Aug  9 18:24:49 JST 1993

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

#include "ruby/ruby.h"
#include "ruby/re.h"
#include "ruby/encoding.h"
#include "ruby/util.h"
#include "internal.h"
#include "regint.h"
#include <ctype.h>

VALUE rb_eRegexpError;

typedef char onig_errmsg_buffer[ONIG_MAX_ERROR_MESSAGE_LEN];
#define errcpy(err, msg) strlcpy((err), (msg), ONIG_MAX_ERROR_MESSAGE_LEN)

#define BEG(no) (regs->beg[(no)])
#define END(no) (regs->end[(no)])

#if 'a' == 97   /* it's ascii */
static const char casetable[] = {
        '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
        '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
        '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
        '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
        /* ' '     '!'     '"'     '#'     '$'     '%'     '&'     ''' */
        '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
        /* '('     ')'     '*'     '+'     ','     '-'     '.'     '/' */
        '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
        /* '0'     '1'     '2'     '3'     '4'     '5'     '6'     '7' */
        '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
        /* '8'     '9'     ':'     ';'     '<'     '='     '>'     '?' */
        '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
        /* '@'     'A'     'B'     'C'     'D'     'E'     'F'     'G' */
        '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
        /* 'H'     'I'     'J'     'K'     'L'     'M'     'N'     'O' */
        '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
        /* 'P'     'Q'     'R'     'S'     'T'     'U'     'V'     'W' */
        '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
        /* 'X'     'Y'     'Z'     '['     '\'     ']'     '^'     '_' */
        '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
        /* '`'     'a'     'b'     'c'     'd'     'e'     'f'     'g' */
        '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
        /* 'h'     'i'     'j'     'k'     'l'     'm'     'n'     'o' */
        '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
        /* 'p'     'q'     'r'     's'     't'     'u'     'v'     'w' */
        '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
        /* 'x'     'y'     'z'     '{'     '|'     '}'     '~' */
        '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
        '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
        '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
        '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
        '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
        '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
        '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
        '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
        '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
        '\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
        '\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
        '\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
        '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
        '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
        '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
        '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
        '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};
#else
# error >>> "You lose. You will need a translation table for your character set." <<<
#endif

int
rb_memcicmp(const void *x, const void *y, long len)
{
    const unsigned char *p1 = x, *p2 = y;
    int tmp;

    while (len--) {
	if ((tmp = casetable[(unsigned)*p1++] - casetable[(unsigned)*p2++]))
	    return tmp;
    }
    return 0;
}

#undef rb_memcmp

int
rb_memcmp(const void *p1, const void *p2, long len)
{
    return memcmp(p1, p2, len);
}

static inline long
rb_memsearch_ss(const unsigned char *xs, long m, const unsigned char *ys, long n)
{
    const unsigned char *x = xs, *xe = xs + m;
    const unsigned char *y = ys, *ye = ys + n;
#ifndef VALUE_MAX
# if SIZEOF_VALUE == 8
#  define VALUE_MAX 0xFFFFFFFFFFFFFFFFULL
# elif SIZEOF_VALUE == 4
#  define VALUE_MAX 0xFFFFFFFFUL
# endif
#endif
    VALUE hx, hy, mask = VALUE_MAX >> ((SIZEOF_VALUE - m) * CHAR_BIT);

    if (m > SIZEOF_VALUE)
	rb_bug("!!too long pattern string!!");

    /* Prepare hash value */
    for (hx = *x++, hy = *y++; x < xe; ++x, ++y) {
	hx <<= CHAR_BIT;
	hy <<= CHAR_BIT;
	hx |= *x;
	hy |= *y;
    }
    /* Searching */
    while (hx != hy) {
	if (y == ye)
	    return -1;
	hy <<= CHAR_BIT;
	hy |= *y;
	hy &= mask;
	y++;
    }
    return y - ys - m;
}

static inline long
rb_memsearch_qs(const unsigned char *xs, long m, const unsigned char *ys, long n)
{
    const unsigned char *x = xs, *xe = xs + m;
    const unsigned char *y = ys;
    VALUE i, qstable[256];

    /* Preprocessing */
    for (i = 0; i < 256; ++i)
	qstable[i] = m + 1;
    for (; x < xe; ++x)
	qstable[*x] = xe - x;
    /* Searching */
    for (; y + m <= ys + n; y += *(qstable + y[m])) {
	if (*xs == *y && memcmp(xs, y, m) == 0)
	    return y - ys;
    }
    return -1;
}

static inline unsigned int
rb_memsearch_qs_utf8_hash(const unsigned char *x)
{
    register const unsigned int mix = 8353;
    register unsigned int h = *x;
    if (h < 0xC0) {
	return h + 256;
    }
    else if (h < 0xE0) {
	h *= mix;
	h += x[1];
    }
    else if (h < 0xF0) {
	h *= mix;
	h += x[1];
	h *= mix;
	h += x[2];
    }
    else if (h < 0xF5) {
	h *= mix;
	h += x[1];
	h *= mix;
	h += x[2];
	h *= mix;
	h += x[3];
    }
    else {
	return h + 256;
    }
    return (unsigned char)h;
}

static inline long
rb_memsearch_qs_utf8(const unsigned char *xs, long m, const unsigned char *ys, long n)
{
    const unsigned char *x = xs, *xe = xs + m;
    const unsigned char *y = ys;
    VALUE i, qstable[512];

    /* Preprocessing */
    for (i = 0; i < 512; ++i) {
	qstable[i] = m + 1;
    }
    for (; x < xe; ++x) {
	qstable[rb_memsearch_qs_utf8_hash(x)] = xe - x;
    }
    /* Searching */
    for (; y + m <= ys + n; y += qstable[rb_memsearch_qs_utf8_hash(y+m)]) {
	if (*xs == *y && memcmp(xs, y, m) == 0)
	    return y - ys;
    }
    return -1;
}

long
rb_memsearch(const void *x0, long m, const void *y0, long n, rb_encoding *enc)
{
    const unsigned char *x = x0, *y = y0;

    if (m > n) return -1;
    else if (m == n) {
	return memcmp(x0, y0, m) == 0 ? 0 : -1;
    }
    else if (m < 1) {
	return 0;
    }
    else if (m == 1) {
	const unsigned char *ys = y, *ye = ys + n;
	for (; y < ye; ++y) {
	    if (*x == *y)
		return y - ys;
	}
	return -1;
    }
    else if (m <= SIZEOF_VALUE) {
	return rb_memsearch_ss(x0, m, y0, n);
    }
    else if (enc == rb_utf8_encoding()){
	return rb_memsearch_qs_utf8(x0, m, y0, n);
    }
    else {
	return rb_memsearch_qs(x0, m, y0, n);
    }
}

#define REG_LITERAL FL_USER5
#define REG_ENCODING_NONE FL_USER6

#define KCODE_FIXED FL_USER4

#define ARG_REG_OPTION_MASK \
    (ONIG_OPTION_IGNORECASE|ONIG_OPTION_MULTILINE|ONIG_OPTION_EXTEND)
#define ARG_ENCODING_FIXED    16
#define ARG_ENCODING_NONE     32

static int
char_to_option(int c)
{
    int val;

    switch (c) {
      case 'i':
	val = ONIG_OPTION_IGNORECASE;
	break;
      case 'x':
	val = ONIG_OPTION_EXTEND;
	break;
      case 'm':
	val = ONIG_OPTION_MULTILINE;
	break;
      default:
	val = 0;
	break;
    }
    return val;
}

static char *
option_to_str(char str[4], int options)
{
    char *p = str;
    if (options & ONIG_OPTION_MULTILINE) *p++ = 'm';
    if (options & ONIG_OPTION_IGNORECASE) *p++ = 'i';
    if (options & ONIG_OPTION_EXTEND) *p++ = 'x';
    *p = 0;
    return str;
}

extern int
rb_char_to_option_kcode(int c, int *option, int *kcode)
{
    *option = 0;

    switch (c) {
      case 'n':
        *kcode = rb_ascii8bit_encindex();
        return (*option = ARG_ENCODING_NONE);
      case 'e':
	*kcode = rb_enc_find_index("EUC-JP");
	break;
      case 's':
	*kcode = rb_enc_find_index("Windows-31J");
	break;
      case 'u':
	*kcode = rb_utf8_encindex();
	break;
      default:
	*kcode = -1;
	return (*option = char_to_option(c));
    }
    *option = ARG_ENCODING_FIXED;
    return 1;
}

static void
rb_reg_check(VALUE re)
{
    if (!RREGEXP(re)->ptr || !RREGEXP_SRC(re) || !RREGEXP_SRC_PTR(re)) {
	rb_raise(rb_eTypeError, "uninitialized Regexp");
    }
}

static void
rb_reg_expr_str(VALUE str, const char *s, long len,
	rb_encoding *enc, rb_encoding *resenc)
{
    const char *p, *pend;
    int cr = ENC_CODERANGE_UNKNOWN;
    int need_escape = 0;
    int c, clen;

    p = s; pend = p + len;
    rb_str_coderange_scan_restartable(p, pend, enc, &cr);
    if (rb_enc_asciicompat(enc) &&
	(cr == ENC_CODERANGE_VALID || cr == ENC_CODERANGE_7BIT)) {
	while (p < pend) {
	    c = rb_enc_ascget(p, pend, &clen, enc);
	    if (c == -1) {
		if (enc == resenc) {
		    p += mbclen(p, pend, enc);
		}
		else {
		    need_escape = 1;
		    break;
		}
	    }
	    else if (c != '/' && rb_enc_isprint(c, enc)) {
		p += clen;
	    }
	    else {
		need_escape = 1;
		break;
	    }
	}
    }
    else {
	need_escape = 1;
    }

    if (!need_escape) {
	rb_str_buf_cat(str, s, len);
    }
    else {
	int unicode_p = rb_enc_unicode_p(enc);
	p = s;
	while (p<pend) {
            c = rb_enc_ascget(p, pend, &clen, enc);
	    if (c == '\\' && p+clen < pend) {
		int n = clen + mbclen(p+clen, pend, enc);
		rb_str_buf_cat(str, p, n);
		p += n;
		continue;
	    }
	    else if (c == '/') {
		char c = '\\';
		rb_str_buf_cat(str, &c, 1);
		rb_str_buf_cat(str, p, clen);
	    }
	    else if (c == -1) {
		clen = rb_enc_precise_mbclen(p, pend, enc);
		if (!MBCLEN_CHARFOUND_P(clen)) {
		    c = (unsigned char)*p;
		    clen = 1;
		    goto hex;
		}
		if (resenc) {
		    unsigned int c = rb_enc_mbc_to_codepoint(p, pend, enc);
		    rb_str_buf_cat_escaped_char(str, c, unicode_p);
		}
		else {
		    clen = MBCLEN_CHARFOUND_LEN(clen);
		    rb_str_buf_cat(str, p, clen);
		}
	    }
	    else if (rb_enc_isprint(c, enc)) {
		rb_str_buf_cat(str, p, clen);
	    }
	    else if (!rb_enc_isspace(c, enc)) {
		char b[8];

	      hex:
		snprintf(b, sizeof(b), "\\x%02X", c);
		rb_str_buf_cat(str, b, 4);
	    }
	    else {
		rb_str_buf_cat(str, p, clen);
	    }
	    p += clen;
	}
    }
}

static VALUE
rb_reg_desc(const char *s, long len, VALUE re)
{
    rb_encoding *enc = rb_enc_get(re);
    VALUE str = rb_str_buf_new2("/");
    rb_encoding *resenc = rb_default_internal_encoding();
    if (resenc == NULL) resenc = rb_default_external_encoding();

    if (re && rb_enc_asciicompat(enc)) {
	rb_enc_copy(str, re);
    }
    else {
	rb_enc_associate(str, rb_usascii_encoding());
    }
    rb_reg_expr_str(str, s, len, enc, resenc);
    rb_str_buf_cat2(str, "/");
    if (re) {
	char opts[4];
	rb_reg_check(re);
	if (*option_to_str(opts, RREGEXP(re)->ptr->options))
	    rb_str_buf_cat2(str, opts);
	if (RBASIC(re)->flags & REG_ENCODING_NONE)
	    rb_str_buf_cat2(str, "n");
    }
    OBJ_INFECT(str, re);
    return str;
}


/*
 *  call-seq:
 *      rxp.source   -> str
 *
 *  Returns the original string of the pattern.
 *
 *      /ab+c/ix.source #=> "ab+c"
 *
 *  Note that escape sequences are retained as is.
 *
 *     /\x20\+/.source  #=> "\\x20\\+"
 *
 */

static VALUE
rb_reg_source(VALUE re)
{
    VALUE str;

    rb_reg_check(re);
    str = rb_enc_str_new(RREGEXP_SRC_PTR(re),RREGEXP_SRC_LEN(re), rb_enc_get(re));
    if (OBJ_TAINTED(re)) OBJ_TAINT(str);
    return str;
}

/*
 * call-seq:
 *    rxp.inspect   -> string
 *
 * Produce a nicely formatted string-version of _rxp_. Perhaps surprisingly,
 * <code>#inspect</code> actually produces the more natural version of
 * the string than <code>#to_s</code>.
 *
 *      /ab+c/ix.inspect        #=> "/ab+c/ix"
 *
 */

static VALUE
rb_reg_inspect(VALUE re)
{
    if (!RREGEXP(re)->ptr || !RREGEXP_SRC(re) || !RREGEXP_SRC_PTR(re)) {
        return rb_any_to_s(re);
    }
    return rb_reg_desc(RREGEXP_SRC_PTR(re), RREGEXP_SRC_LEN(re), re);
}


/*
 *  call-seq:
 *     rxp.to_s   -> str
 *
 *  Returns a string containing the regular expression and its options (using the
 *  <code>(?opts:source)</code> notation. This string can be fed back in to
 *  <code>Regexp::new</code> to a regular expression with the same semantics as
 *  the original. (However, <code>Regexp#==</code> may not return true when
 *  comparing the two, as the source of the regular expression itself may
 *  differ, as the example shows).  <code>Regexp#inspect</code> produces a
 *  generally more readable version of <i>rxp</i>.
 *
 *      r1 = /ab+c/ix           #=> /ab+c/ix
 *      s1 = r1.to_s            #=> "(?ix-m:ab+c)"
 *      r2 = Regexp.new(s1)     #=> /(?ix-m:ab+c)/
 *      r1 == r2                #=> false
 *      r1.source               #=> "ab+c"
 *      r2.source               #=> "(?ix-m:ab+c)"
 */

static VALUE
rb_reg_to_s(VALUE re)
{
    int options, opt;
    const int embeddable = ONIG_OPTION_MULTILINE|ONIG_OPTION_IGNORECASE|ONIG_OPTION_EXTEND;
    long len;
    const UChar* ptr;
    VALUE str = rb_str_buf_new2("(?");
    char optbuf[5];
    rb_encoding *enc = rb_enc_get(re);

    rb_reg_check(re);

    rb_enc_copy(str, re);
    options = RREGEXP(re)->ptr->options;
    ptr = (UChar*)RREGEXP_SRC_PTR(re);
    len = RREGEXP_SRC_LEN(re);
  again:
    if (len >= 4 && ptr[0] == '(' && ptr[1] == '?') {
	int err = 1;
	ptr += 2;
	if ((len -= 2) > 0) {
	    do {
                opt = char_to_option((int )*ptr);
                if (opt != 0) {
                    options |= opt;
                }
                else {
                    break;
                }
		++ptr;
	    } while (--len > 0);
	}
	if (len > 1 && *ptr == '-') {
	    ++ptr;
	    --len;
	    do {
                opt = char_to_option((int )*ptr);
                if (opt != 0) {
                    options &= ~opt;
                }
                else {
                    break;
                }
		++ptr;
	    } while (--len > 0);
	}
	if (*ptr == ')') {
	    --len;
	    ++ptr;
	    goto again;
	}
	if (*ptr == ':' && ptr[len-1] == ')') {
	    Regexp *rp;

	    ++ptr;
	    len -= 2;
            err = onig_new(&rp, ptr, ptr + len, ONIG_OPTION_DEFAULT,
			   enc, OnigDefaultSyntax, NULL);
	    onig_free(rp);
	}
	if (err) {
	    options = RREGEXP(re)->ptr->options;
	    ptr = (UChar*)RREGEXP_SRC_PTR(re);
	    len = RREGEXP_SRC_LEN(re);
	}
    }

    if (*option_to_str(optbuf, options)) rb_str_buf_cat2(str, optbuf);

    if ((options & embeddable) != embeddable) {
	optbuf[0] = '-';
	option_to_str(optbuf + 1, ~options);
	rb_str_buf_cat2(str, optbuf);
    }

    rb_str_buf_cat2(str, ":");
    rb_reg_expr_str(str, (char*)ptr, len, enc, NULL);
    rb_str_buf_cat2(str, ")");
    rb_enc_copy(str, re);

    OBJ_INFECT(str, re);
    return str;
}

static void
rb_reg_raise(const char *s, long len, const char *err, VALUE re)
{
    volatile VALUE desc = rb_reg_desc(s, len, re);

    rb_raise(rb_eRegexpError, "%s: %s", err, RSTRING_PTR(desc));
}

static VALUE
rb_enc_reg_error_desc(const char *s, long len, rb_encoding *enc, int options, const char *err)
{
    char opts[6];
    VALUE desc = rb_str_buf_new2(err);
    rb_encoding *resenc = rb_default_internal_encoding();
    if (resenc == NULL) resenc = rb_default_external_encoding();

    rb_enc_associate(desc, enc);
    rb_str_buf_cat2(desc, ": /");
    rb_reg_expr_str(desc, s, len, enc, resenc);
    opts[0] = '/';
    option_to_str(opts + 1, options);
    rb_str_buf_cat2(desc, opts);
    return rb_exc_new3(rb_eRegexpError, desc);
}

static void
rb_enc_reg_raise(const char *s, long len, rb_encoding *enc, int options, const char *err)
{
    rb_exc_raise(rb_enc_reg_error_desc(s, len, enc, options, err));
}

static VALUE
rb_reg_error_desc(VALUE str, int options, const char *err)
{
    return rb_enc_reg_error_desc(RSTRING_PTR(str), RSTRING_LEN(str),
				 rb_enc_get(str), options, err);
}

static void
rb_reg_raise_str(VALUE str, int options, const char *err)
{
    rb_exc_raise(rb_reg_error_desc(str, options, err));
}


/*
 *  call-seq:
 *     rxp.casefold?   -> true or false
 *
 *  Returns the value of the case-insensitive flag.
 *
 *      /a/.casefold?           #=> false
 *      /a/i.casefold?          #=> true
 *      /(?i:a)/.casefold?      #=> false
 */

static VALUE
rb_reg_casefold_p(VALUE re)
{
    rb_reg_check(re);
    if (RREGEXP(re)->ptr->options & ONIG_OPTION_IGNORECASE) return Qtrue;
    return Qfalse;
}


/*
 *  call-seq:
 *     rxp.options   -> fixnum
 *
 *  Returns the set of bits corresponding to the options used when creating this
 *  Regexp (see <code>Regexp::new</code> for details. Note that additional bits
 *  may be set in the returned options: these are used internally by the regular
 *  expression code. These extra bits are ignored if the options are passed to
 *  <code>Regexp::new</code>.
 *
 *     Regexp::IGNORECASE                  #=> 1
 *     Regexp::EXTENDED                    #=> 2
 *     Regexp::MULTILINE                   #=> 4
 *
 *     /cat/.options                       #=> 0
 *     /cat/ix.options                     #=> 3
 *     Regexp.new('cat', true).options     #=> 1
 *     /\xa1\xa2/e.options                 #=> 16
 *
 *     r = /cat/ix
 *     Regexp.new(r.source, r.options)     #=> /cat/ix
 */

static VALUE
rb_reg_options_m(VALUE re)
{
    int options = rb_reg_options(re);
    return INT2NUM(options);
}

static int
reg_names_iter(const OnigUChar *name, const OnigUChar *name_end,
          int back_num, int *back_refs, OnigRegex regex, void *arg)
{
    VALUE ary = (VALUE)arg;
    rb_ary_push(ary, rb_str_new((const char *)name, name_end-name));
    return 0;
}

/*
 * call-seq:
 *    rxp.names   -> [name1, name2, ...]
 *
 * Returns a list of names of captures as an array of strings.
 *
 *     /(?<foo>.)(?<bar>.)(?<baz>.)/.names
 *     #=> ["foo", "bar", "baz"]
 *
 *     /(?<foo>.)(?<foo>.)/.names
 *     #=> ["foo"]
 *
 *     /(.)(.)/.names
 *     #=> []
 */

static VALUE
rb_reg_names(VALUE re)
{
    VALUE ary = rb_ary_new();
    rb_reg_check(re);
    onig_foreach_name(RREGEXP(re)->ptr, reg_names_iter, (void*)ary);
    return ary;
}

static int
reg_named_captures_iter(const OnigUChar *name, const OnigUChar *name_end,
          int back_num, int *back_refs, OnigRegex regex, void *arg)
{
    VALUE hash = (VALUE)arg;
    VALUE ary = rb_ary_new2(back_num);
    int i;

    for(i = 0; i < back_num; i++)
        rb_ary_store(ary, i, INT2NUM(back_refs[i]));

    rb_hash_aset(hash, rb_str_new((const char*)name, name_end-name),ary);

    return 0;
}

/*
 * call-seq:
 *    rxp.named_captures  -> hash
 *
 * Returns a hash representing information about named captures of <i>rxp</i>.
 *
 * A key of the hash is a name of the named captures.
 * A value of the hash is an array which is list of indexes of corresponding
 * named captures.
 *
 *    /(?<foo>.)(?<bar>.)/.named_captures
 *    #=> {"foo"=>[1], "bar"=>[2]}
 *
 *    /(?<foo>.)(?<foo>.)/.named_captures
 *    #=> {"foo"=>[1, 2]}
 *
 * If there are no named captures, an empty hash is returned.
 *
 *    /(.)(.)/.named_captures
 *    #=> {}
 */

static VALUE
rb_reg_named_captures(VALUE re)
{
    VALUE hash = rb_hash_new();
    rb_reg_check(re);
    onig_foreach_name(RREGEXP(re)->ptr, reg_named_captures_iter, (void*)hash);
    return hash;
}

static int
onig_new_with_source(regex_t** reg, const UChar* pattern, const UChar* pattern_end,
	  OnigOptionType option, OnigEncoding enc, const OnigSyntaxType* syntax,
	  OnigErrorInfo* einfo, const char *sourcefile, int sourceline)
{
  int r;

  *reg = (regex_t* )malloc(sizeof(regex_t));
  if (IS_NULL(*reg)) return ONIGERR_MEMORY;

  r = onig_reg_init(*reg, option, ONIGENC_CASE_FOLD_DEFAULT, enc, syntax);
  if (r) goto err;

  r = onig_compile(*reg, pattern, pattern_end, einfo, sourcefile, sourceline);
  if (r) {
  err:
    onig_free(*reg);
    *reg = NULL;
  }
  return r;
}

static Regexp*
make_regexp(const char *s, long len, rb_encoding *enc, int flags, onig_errmsg_buffer err,
	const char *sourcefile, int sourceline)
{
    Regexp *rp;
    int r;
    OnigErrorInfo einfo;

    /* Handle escaped characters first. */

    /* Build a copy of the string (in dest) with the
       escaped characters translated,  and generate the regex
       from that.
    */

    r = onig_new_with_source(&rp, (UChar*)s, (UChar*)(s + len), flags,
		 enc, OnigDefaultSyntax, &einfo, sourcefile, sourceline);
    if (r) {
	onig_error_code_to_str((UChar*)err, r, &einfo);
	return 0;
    }
    return rp;
}


/*
 *  Document-class: MatchData
 *
 *  <code>MatchData</code> is the type of the special variable <code>$~</code>,
 *  and is the type of the object returned by <code>Regexp#match</code> and
 *  <code>Regexp.last_match</code>. It encapsulates all the results of a pattern
 *  match, results normally accessed through the special variables
 *  <code>$&</code>, <code>$'</code>, <code>$`</code>, <code>$1</code>,
 *  <code>$2</code>, and so on.
 *
 */

VALUE rb_cMatch;

static VALUE
match_alloc(VALUE klass)
{
    NEWOBJ(match, struct RMatch);
    OBJSETUP(match, klass, T_MATCH);

    match->str = 0;
    match->rmatch = 0;
    match->regexp = 0;
    match->rmatch = ALLOC(struct rmatch);
    MEMZERO(match->rmatch, struct rmatch, 1);

    return (VALUE)match;
}

typedef struct {
    long byte_pos;
    long char_pos;
} pair_t;

static int
pair_byte_cmp(const void *pair1, const void *pair2)
{
    long diff = ((pair_t*)pair1)->byte_pos - ((pair_t*)pair2)->byte_pos;
#if SIZEOF_LONG > SIZEOF_INT
    return diff ? diff > 0 ? 1 : -1 : 0;
#else
    return (int)diff;
#endif
}

static void
update_char_offset(VALUE match)
{
    struct rmatch *rm = RMATCH(match)->rmatch;
    struct re_registers *regs;
    int i, num_regs, num_pos;
    long c;
    char *s, *p, *q;
    rb_encoding *enc;
    pair_t *pairs;

    if (rm->char_offset_updated)
        return;

    regs = &rm->regs;
    num_regs = rm->regs.num_regs;

    if (rm->char_offset_num_allocated < num_regs) {
        REALLOC_N(rm->char_offset, struct rmatch_offset, num_regs);
        rm->char_offset_num_allocated = num_regs;
    }

    enc = rb_enc_get(RMATCH(match)->str);
    if (rb_enc_mbmaxlen(enc) == 1) {
        for (i = 0; i < num_regs; i++) {
            rm->char_offset[i].beg = BEG(i);
            rm->char_offset[i].end = END(i);
        }
        rm->char_offset_updated = 1;
        return;
    }

    pairs = ALLOCA_N(pair_t, num_regs*2);
    num_pos = 0;
    for (i = 0; i < num_regs; i++) {
        if (BEG(i) < 0)
            continue;
        pairs[num_pos++].byte_pos = BEG(i);
        pairs[num_pos++].byte_pos = END(i);
    }
    qsort(pairs, num_pos, sizeof(pair_t), pair_byte_cmp);

    s = p = RSTRING_PTR(RMATCH(match)->str);
    c = 0;
    for (i = 0; i < num_pos; i++) {
        q = s + pairs[i].byte_pos;
        c += rb_enc_strlen(p, q, enc);
        pairs[i].char_pos = c;
        p = q;
    }

    for (i = 0; i < num_regs; i++) {
        pair_t key, *found;
        if (BEG(i) < 0) {
            rm->char_offset[i].beg = -1;
            rm->char_offset[i].end = -1;
            continue;
        }

        key.byte_pos = BEG(i);
        found = bsearch(&key, pairs, num_pos, sizeof(pair_t), pair_byte_cmp);
        rm->char_offset[i].beg = found->char_pos;

        key.byte_pos = END(i);
        found = bsearch(&key, pairs, num_pos, sizeof(pair_t), pair_byte_cmp);
        rm->char_offset[i].end = found->char_pos;
    }

    rm->char_offset_updated = 1;
}

static void
match_check(VALUE match)
{
    if (!RMATCH(match)->regexp) {
	rb_raise(rb_eTypeError, "uninitialized Match");
    }
}

/* :nodoc: */
static VALUE
match_init_copy(VALUE obj, VALUE orig)
{
    struct rmatch *rm;

    if (obj == orig) return obj;

    if (!rb_obj_is_instance_of(orig, rb_obj_class(obj))) {
	rb_raise(rb_eTypeError, "wrong argument class");
    }
    RMATCH(obj)->str = RMATCH(orig)->str;
    RMATCH(obj)->regexp = RMATCH(orig)->regexp;

    rm = RMATCH(obj)->rmatch;
    onig_region_copy(&rm->regs, RMATCH_REGS(orig));

    if (!RMATCH(orig)->rmatch->char_offset_updated) {
        rm->char_offset_updated = 0;
    }
    else {
        if (rm->char_offset_num_allocated < rm->regs.num_regs) {
            REALLOC_N(rm->char_offset, struct rmatch_offset, rm->regs.num_regs);
            rm->char_offset_num_allocated = rm->regs.num_regs;
        }
        MEMCPY(rm->char_offset, RMATCH(orig)->rmatch->char_offset,
               struct rmatch_offset, rm->regs.num_regs);
        rm->char_offset_updated = 1;
    }

    return obj;
}


/*
 * call-seq:
 *    mtch.regexp   -> regexp
 *
 * Returns the regexp.
 *
 *     m = /a.*b/.match("abc")
 *     m.regexp #=> /a.*b/
 */

static VALUE
match_regexp(VALUE match)
{
    match_check(match);
    return RMATCH(match)->regexp;
}

/*
 * call-seq:
 *    mtch.names   -> [name1, name2, ...]
 *
 * Returns a list of names of captures as an array of strings.
 * It is same as mtch.regexp.names.
 *
 *     /(?<foo>.)(?<bar>.)(?<baz>.)/.match("hoge").names
 *     #=> ["foo", "bar", "baz"]
 *
 *     m = /(?<x>.)(?<y>.)?/.match("a") #=> #<MatchData "a" x:"a" y:nil>
 *     m.names                          #=> ["x", "y"]
 */

static VALUE
match_names(VALUE match)
{
    match_check(match);
    return rb_reg_names(RMATCH(match)->regexp);
}

/*
 *  call-seq:
 *     mtch.length   -> integer
 *     mtch.size     -> integer
 *
 *  Returns the number of elements in the match array.
 *
 *     m = /(.)(.)(\d+)(\d)/.match("THX1138.")
 *     m.length   #=> 5
 *     m.size     #=> 5
 */

static VALUE
match_size(VALUE match)
{
    match_check(match);
    return INT2FIX(RMATCH_REGS(match)->num_regs);
}

static int
match_backref_number(VALUE match, VALUE backref)
{
    const char *name;
    int num;

    struct re_registers *regs = RMATCH_REGS(match);
    VALUE regexp = RMATCH(match)->regexp;

    match_check(match);
    switch(TYPE(backref)) {
      default:
        return NUM2INT(backref);

      case T_SYMBOL:
        name = rb_id2name(SYM2ID(backref));
        break;

      case T_STRING:
        name = StringValueCStr(backref);
        break;
    }

    num = onig_name_to_backref_number(RREGEXP(regexp)->ptr,
              (const unsigned char*)name,
              (const unsigned char*)name + strlen(name),
              regs);

    if (num < 1) {
        rb_raise(rb_eIndexError, "undefined group name reference: %s", name);
    }

    return num;
}

int
rb_reg_backref_number(VALUE match, VALUE backref)
{
    return match_backref_number(match, backref);
}

/*
 *  call-seq:
 *     mtch.offset(n)   -> array
 *
 *  Returns a two-element array containing the beginning and ending offsets of
 *  the <em>n</em>th match.
 *  <em>n</em> can be a string or symbol to reference a named capture.
 *
 *     m = /(.)(.)(\d+)(\d)/.match("THX1138.")
 *     m.offset(0)      #=> [1, 7]
 *     m.offset(4)      #=> [6, 7]
 *
 *     m = /(?<foo>.)(.)(?<bar>.)/.match("hoge")
 *     p m.offset(:foo) #=> [0, 1]
 *     p m.offset(:bar) #=> [2, 3]
 *
 */

static VALUE
match_offset(VALUE match, VALUE n)
{
    int i = match_backref_number(match, n);
    struct re_registers *regs = RMATCH_REGS(match);

    match_check(match);
    if (i < 0 || regs->num_regs <= i)
	rb_raise(rb_eIndexError, "index %d out of matches", i);

    if (BEG(i) < 0)
	return rb_assoc_new(Qnil, Qnil);

    update_char_offset(match);
    return rb_assoc_new(INT2FIX(RMATCH(match)->rmatch->char_offset[i].beg),
			INT2FIX(RMATCH(match)->rmatch->char_offset[i].end));
}


/*
 *  call-seq:
 *     mtch.begin(n)   -> integer
 *
 *  Returns the offset of the start of the <em>n</em>th element of the match
 *  array in the string.
 *  <em>n</em> can be a string or symbol to reference a named capture.
 *
 *     m = /(.)(.)(\d+)(\d)/.match("THX1138.")
 *     m.begin(0)       #=> 1
 *     m.begin(2)       #=> 2
 *
 *     m = /(?<foo>.)(.)(?<bar>.)/.match("hoge")
 *     p m.begin(:foo)  #=> 0
 *     p m.begin(:bar)  #=> 2
 */

static VALUE
match_begin(VALUE match, VALUE n)
{
    int i = match_backref_number(match, n);
    struct re_registers *regs = RMATCH_REGS(match);

    match_check(match);
    if (i < 0 || regs->num_regs <= i)
	rb_raise(rb_eIndexError, "index %d out of matches", i);

    if (BEG(i) < 0)
	return Qnil;

    update_char_offset(match);
    return INT2FIX(RMATCH(match)->rmatch->char_offset[i].beg);
}


/*
 *  call-seq:
 *     mtch.end(n)   -> integer
 *
 *  Returns the offset of the character immediately following the end of the
 *  <em>n</em>th element of the match array in the string.
 *  <em>n</em> can be a string or symbol to reference a named capture.
 *
 *     m = /(.)(.)(\d+)(\d)/.match("THX1138.")
 *     m.end(0)         #=> 7
 *     m.end(2)         #=> 3
 *
 *     m = /(?<foo>.)(.)(?<bar>.)/.match("hoge")
 *     p m.end(:foo)    #=> 1
 *     p m.end(:bar)    #=> 3
 */

static VALUE
match_end(VALUE match, VALUE n)
{
    int i = match_backref_number(match, n);
    struct re_registers *regs = RMATCH_REGS(match);

    match_check(match);
    if (i < 0 || regs->num_regs <= i)
	rb_raise(rb_eIndexError, "index %d out of matches", i);

    if (BEG(i) < 0)
	return Qnil;

    update_char_offset(match);
    return INT2FIX(RMATCH(match)->rmatch->char_offset[i].end);
}

#define MATCH_BUSY FL_USER2

void
rb_match_busy(VALUE match)
{
    FL_SET(match, MATCH_BUSY);
}

/*
 *  call-seq:
 *     rxp.fixed_encoding?   -> true or false
 *
 *  Returns false if rxp is applicable to
 *  a string with any ASCII compatible encoding.
 *  Returns true otherwise.
 *
 *      r = /a/
 *      r.fixed_encoding?                               #=> false
 *      r =~ "\u{6666} a"                               #=> 2
 *      r =~ "\xa1\xa2 a".force_encoding("euc-jp")      #=> 2
 *      r =~ "abc".force_encoding("euc-jp")             #=> 0
 *
 *      r = /a/u
 *      r.fixed_encoding?                               #=> true
 *      r.encoding                                      #=> #<Encoding:UTF-8>
 *      r =~ "\u{6666} a"                               #=> 2
 *      r =~ "\xa1\xa2".force_encoding("euc-jp")        #=> ArgumentError
 *      r =~ "abc".force_encoding("euc-jp")             #=> 0
 *
 *      r = /\u{6666}/
 *      r.fixed_encoding?                               #=> true
 *      r.encoding                                      #=> #<Encoding:UTF-8>
 *      r =~ "\u{6666} a"                               #=> 0
 *      r =~ "\xa1\xa2".force_encoding("euc-jp")        #=> ArgumentError
 *      r =~ "abc".force_encoding("euc-jp")             #=> nil
 */

static VALUE
rb_reg_fixed_encoding_p(VALUE re)
{
    if (FL_TEST(re, KCODE_FIXED))
        return Qtrue;
    else
        return Qfalse;
}

static VALUE
rb_reg_preprocess(const char *p, const char *end, rb_encoding *enc,
        rb_encoding **fixed_enc, onig_errmsg_buffer err);


static void
reg_enc_error(VALUE re, VALUE str)
{
    rb_raise(rb_eEncCompatError,
	     "incompatible encoding regexp match (%s regexp with %s string)",
	     rb_enc_name(rb_enc_get(re)),
	     rb_enc_name(rb_enc_get(str)));
}

static rb_encoding*
rb_reg_prepare_enc(VALUE re, VALUE str, int warn)
{
    rb_encoding *enc = 0;

    if (rb_enc_str_coderange(str) == ENC_CODERANGE_BROKEN) {
        rb_raise(rb_eArgError,
            "invalid byte sequence in %s",
            rb_enc_name(rb_enc_get(str)));
    }

    rb_reg_check(re);
    enc = rb_enc_get(str);
    if (!rb_enc_str_asciicompat_p(str)) {
        if (RREGEXP(re)->ptr->enc != enc) {
	    reg_enc_error(re, str);
	}
    }
    else if (rb_reg_fixed_encoding_p(re)) {
        if (RREGEXP(re)->ptr->enc != enc &&
	    (!rb_enc_asciicompat(RREGEXP(re)->ptr->enc) ||
	     rb_enc_str_coderange(str) != ENC_CODERANGE_7BIT)) {
	    reg_enc_error(re, str);
	}
	enc = RREGEXP(re)->ptr->enc;
    }
    if (warn && (RBASIC(re)->flags & REG_ENCODING_NONE) &&
	enc != rb_ascii8bit_encoding() &&
	rb_enc_str_coderange(str) != ENC_CODERANGE_7BIT) {
	rb_warn("regexp match /.../n against to %s string",
		rb_enc_name(enc));
    }
    return enc;
}

regex_t *
rb_reg_prepare_re(VALUE re, VALUE str)
{
    regex_t *reg = RREGEXP(re)->ptr;
    onig_errmsg_buffer err = "";
    int r;
    OnigErrorInfo einfo;
    const char *pattern;
    VALUE unescaped;
    rb_encoding *fixed_enc = 0;
    rb_encoding *enc = rb_reg_prepare_enc(re, str, 1);

    if (reg->enc == enc) return reg;

    rb_reg_check(re);
    reg = RREGEXP(re)->ptr;
    pattern = RREGEXP_SRC_PTR(re);

    unescaped = rb_reg_preprocess(
	pattern, pattern + RREGEXP_SRC_LEN(re), enc,
	&fixed_enc, err);

    if (unescaped == Qnil) {
	rb_raise(rb_eArgError, "regexp preprocess failed: %s", err);
    }

    r = onig_new(&reg, (UChar* )RSTRING_PTR(unescaped),
		 (UChar* )(RSTRING_PTR(unescaped) + RSTRING_LEN(unescaped)),
		 reg->options, enc,
		 OnigDefaultSyntax, &einfo);
    if (r) {
	onig_error_code_to_str((UChar*)err, r, &einfo);
	rb_reg_raise(pattern, RREGEXP_SRC_LEN(re), err, re);
    }

    RB_GC_GUARD(unescaped);
    return reg;
}

long
rb_reg_adjust_startpos(VALUE re, VALUE str, long pos, int reverse)
{
    long range;
    rb_encoding *enc;
    UChar *p, *string;

    enc = rb_reg_prepare_enc(re, str, 0);

    if (reverse) {
	range = -pos;
    }
    else {
	range = RSTRING_LEN(str) - pos;
    }

    if (pos > 0 && ONIGENC_MBC_MAXLEN(enc) != 1 && pos < RSTRING_LEN(str)) {
	 string = (UChar*)RSTRING_PTR(str);

	 if (range > 0) {
	      p = onigenc_get_right_adjust_char_head(enc, string, string + pos, string + RSTRING_LEN(str));
	 }
	 else {
	      p = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, string, string + pos, string + RSTRING_LEN(str));
	 }
	 return p - string;
    }

    return pos;
}

long
rb_reg_search(VALUE re, VALUE str, long pos, int reverse)
{
    long result;
    VALUE match;
    struct re_registers regi, *regs = &regi;
    char *range = RSTRING_PTR(str);
    regex_t *reg;
    int tmpreg;

    if (pos > RSTRING_LEN(str) || pos < 0) {
	rb_backref_set(Qnil);
	return -1;
    }

    reg = rb_reg_prepare_re(re, str);
    tmpreg = reg != RREGEXP(re)->ptr;
    if (!tmpreg) RREGEXP(re)->usecnt++;

    match = rb_backref_get();
    if (!NIL_P(match)) {
	if (FL_TEST(match, MATCH_BUSY)) {
	    match = Qnil;
	}
	else {
	    regs = RMATCH_REGS(match);
	}
    }
    if (NIL_P(match)) {
	MEMZERO(regs, struct re_registers, 1);
    }
    if (!reverse) {
	range += RSTRING_LEN(str);
    }
    result = onig_search(reg,
			 (UChar*)(RSTRING_PTR(str)),
			 ((UChar*)(RSTRING_PTR(str)) + RSTRING_LEN(str)),
			 ((UChar*)(RSTRING_PTR(str)) + pos),
			 ((UChar*)range),
			 regs, ONIG_OPTION_NONE);
    if (!tmpreg) RREGEXP(re)->usecnt--;
    if (tmpreg) {
	if (RREGEXP(re)->usecnt) {
	    onig_free(reg);
	}
	else {
	    onig_free(RREGEXP(re)->ptr);
	    RREGEXP(re)->ptr = reg;
	}
    }
    if (result < 0) {
	if (regs == &regi)
	    onig_region_free(regs, 0);
	if (result == ONIG_MISMATCH) {
	    rb_backref_set(Qnil);
	    return result;
	}
	else {
	    onig_errmsg_buffer err = "";
	    onig_error_code_to_str((UChar*)err, (int)result);
	    rb_reg_raise(RREGEXP_SRC_PTR(re), RREGEXP_SRC_LEN(re), err, re);
	}
    }

    if (NIL_P(match)) {
	match = match_alloc(rb_cMatch);
	onig_region_copy(RMATCH_REGS(match), regs);
	onig_region_free(regs, 0);
    }
    else {
	if (rb_safe_level() >= 3)
	    OBJ_TAINT(match);
	else
	    FL_UNSET(match, FL_TAINT);
    }

    RMATCH(match)->str = rb_str_new4(str);
    RMATCH(match)->regexp = re;
    RMATCH(match)->rmatch->char_offset_updated = 0;
    rb_backref_set(match);

    OBJ_INFECT(match, re);
    OBJ_INFECT(match, str);

    return result;
}

VALUE
rb_reg_nth_defined(int nth, VALUE match)
{
    struct re_registers *regs;
    if (NIL_P(match)) return Qnil;
    match_check(match);
    regs = RMATCH_REGS(match);
    if (nth >= regs->num_regs) {
	return Qnil;
    }
    if (nth < 0) {
	nth += regs->num_regs;
	if (nth <= 0) return Qnil;
    }
    if (BEG(nth) == -1) return Qfalse;
    return Qtrue;
}

VALUE
rb_reg_nth_match(int nth, VALUE match)
{
    VALUE str;
    long start, end, len;
    struct re_registers *regs;

    if (NIL_P(match)) return Qnil;
    match_check(match);
    regs = RMATCH_REGS(match);
    if (nth >= regs->num_regs) {
	return Qnil;
    }
    if (nth < 0) {
	nth += regs->num_regs;
	if (nth <= 0) return Qnil;
    }
    start = BEG(nth);
    if (start == -1) return Qnil;
    end = END(nth);
    len = end - start;
    str = rb_str_subseq(RMATCH(match)->str, start, len);
    OBJ_INFECT(str, match);
    return str;
}

VALUE
rb_reg_last_match(VALUE match)
{
    return rb_reg_nth_match(0, match);
}


/*
 *  call-seq:
 *     mtch.pre_match   -> str
 *
 *  Returns the portion of the original string before the current match.
 *  Equivalent to the special variable <code>$`</code>.
 *
 *     m = /(.)(.)(\d+)(\d)/.match("THX1138.")
 *     m.pre_match   #=> "T"
 */

VALUE
rb_reg_match_pre(VALUE match)
{
    VALUE str;
    struct re_registers *regs;

    if (NIL_P(match)) return Qnil;
    match_check(match);
    regs = RMATCH_REGS(match);
    if (BEG(0) == -1) return Qnil;
    str = rb_str_subseq(RMATCH(match)->str, 0, BEG(0));
    if (OBJ_TAINTED(match)) OBJ_TAINT(str);
    return str;
}


/*
 *  call-seq:
 *     mtch.post_match   -> str
 *
 *  Returns the portion of the original string after the current match.
 *  Equivalent to the special variable <code>$'</code>.
 *
 *     m = /(.)(.)(\d+)(\d)/.match("THX1138: The Movie")
 *     m.post_match   #=> ": The Movie"
 */

VALUE
rb_reg_match_post(VALUE match)
{
    VALUE str;
    long pos;
    struct re_registers *regs;

    if (NIL_P(match)) return Qnil;
    match_check(match);
    regs = RMATCH_REGS(match);
    if (BEG(0) == -1) return Qnil;
    str = RMATCH(match)->str;
    pos = END(0);
    str = rb_str_subseq(str, pos, RSTRING_LEN(str) - pos);
    if (OBJ_TAINTED(match)) OBJ_TAINT(str);
    return str;
}

VALUE
rb_reg_match_last(VALUE match)
{
    int i;
    struct re_registers *regs;

    if (NIL_P(match)) return Qnil;
    match_check(match);
    regs = RMATCH_REGS(match);
    if (BEG(0) == -1) return Qnil;

    for (i=regs->num_regs-1; BEG(i) == -1 && i > 0; i--)
	;
    if (i == 0) return Qnil;
    return rb_reg_nth_match(i, match);
}

static VALUE
last_match_getter(void)
{
    return rb_reg_last_match(rb_backref_get());
}

static VALUE
prematch_getter(void)
{
    return rb_reg_match_pre(rb_backref_get());
}

static VALUE
postmatch_getter(void)
{
    return rb_reg_match_post(rb_backref_get());
}

static VALUE
last_paren_match_getter(void)
{
    return rb_reg_match_last(rb_backref_get());
}

static VALUE
match_array(VALUE match, int start)
{
    struct re_registers *regs;
    VALUE ary;
    VALUE target;
    int i;
    int taint = OBJ_TAINTED(match);

    match_check(match);
    regs = RMATCH_REGS(match);
    ary = rb_ary_new2(regs->num_regs);
    target = RMATCH(match)->str;

    for (i=start; i<regs->num_regs; i++) {
	if (regs->beg[i] == -1) {
	    rb_ary_push(ary, Qnil);
	}
	else {
	    VALUE str = rb_str_subseq(target, regs->beg[i], regs->end[i]-regs->beg[i]);
	    if (taint) OBJ_TAINT(str);
	    rb_ary_push(ary, str);
	}
    }
    return ary;
}


/* [MG]:FIXME: I put parens around the /.../.match() in the first line of the
   second example to prevent the '*' followed by a '/' from ending the
   comment. */

/*
 *  call-seq:
 *     mtch.to_a   -> anArray
 *
 *  Returns the array of matches.
 *
 *     m = /(.)(.)(\d+)(\d)/.match("THX1138.")
 *     m.to_a   #=> ["HX1138", "H", "X", "113", "8"]
 *
 *  Because <code>to_a</code> is called when expanding
 *  <code>*</code><em>variable</em>, there's a useful assignment
 *  shortcut for extracting matched fields. This is slightly slower than
 *  accessing the fields directly (as an intermediate array is
 *  generated).
 *
 *     all,f1,f2,f3 = *(/(.)(.)(\d+)(\d)/.match("THX1138."))
 *     all   #=> "HX1138"
 *     f1    #=> "H"
 *     f2    #=> "X"
 *     f3    #=> "113"
 */

static VALUE
match_to_a(VALUE match)
{
    return match_array(match, 0);
}


/*
 *  call-seq:
 *     mtch.captures   -> array
 *
 *  Returns the array of captures; equivalent to <code>mtch.to_a[1..-1]</code>.
 *
 *     f1,f2,f3,f4 = /(.)(.)(\d+)(\d)/.match("THX1138.").captures
 *     f1    #=> "H"
 *     f2    #=> "X"
 *     f3    #=> "113"
 *     f4    #=> "8"
 */
static VALUE
match_captures(VALUE match)
{
    return match_array(match, 1);
}

static int
name_to_backref_number(struct re_registers *regs, VALUE regexp, const char* name, const char* name_end)
{
    int num;

    num = onig_name_to_backref_number(RREGEXP(regexp)->ptr,
	(const unsigned char* )name, (const unsigned char* )name_end, regs);
    if (num >= 1) {
	return num;
    }
    else {
	VALUE s = rb_str_new(name, (long )(name_end - name));
	rb_raise(rb_eIndexError, "undefined group name reference: %s",
				 StringValuePtr(s));
    }
}

/*
 *  call-seq:
 *     mtch[i]               -> str or nil
 *     mtch[start, length]   -> array
 *     mtch[range]           -> array
 *     mtch[name]            -> str or nil
 *
 *  Match Reference -- <code>MatchData</code> acts as an array, and may be
 *  accessed using the normal array indexing techniques.  <code>mtch[0]</code>
 *  is equivalent to the special variable <code>$&</code>, and returns the
 *  entire matched string.  <code>mtch[1]</code>, <code>mtch[2]</code>, and so
 *  on return the values of the matched backreferences (portions of the
 *  pattern between parentheses).
 *
 *     m = /(.)(.)(\d+)(\d)/.match("THX1138.")
 *     m          #=> #<MatchData "HX1138" 1:"H" 2:"X" 3:"113" 4:"8">
 *     m[0]       #=> "HX1138"
 *     m[1, 2]    #=> ["H", "X"]
 *     m[1..3]    #=> ["H", "X", "113"]
 *     m[-3, 2]   #=> ["X", "113"]
 *
 *     m = /(?<foo>a+)b/.match("ccaaab")
 *     m          #=> #<MatchData "aaab" foo:"aaa">
 *     m["foo"]   #=> "aaa"
 *     m[:foo]    #=> "aaa"
 */

static VALUE
match_aref(int argc, VALUE *argv, VALUE match)
{
    VALUE idx, rest;

    match_check(match);
    rb_scan_args(argc, argv, "11", &idx, &rest);

    if (NIL_P(rest)) {
	if (FIXNUM_P(idx)) {
	    if (FIX2INT(idx) >= 0) {
		return rb_reg_nth_match(FIX2INT(idx), match);
	    }
	}
	else {
	    const char *p;
	    int num;

	    switch (TYPE(idx)) {
	      case T_SYMBOL:
		p = rb_id2name(SYM2ID(idx));
		goto name_to_backref;
		break;
	      case T_STRING:
		p = StringValuePtr(idx);

	      name_to_backref:
		num = name_to_backref_number(RMATCH_REGS(match),
					     RMATCH(match)->regexp, p, p + strlen(p));
		return rb_reg_nth_match(num, match);
		break;

	      default:
		break;
	    }
	}
    }

    return rb_ary_aref(argc, argv, match_to_a(match));
}

static VALUE
match_entry(VALUE match, long n)
{
    /* n should not exceed num_regs */
    return rb_reg_nth_match((int)n, match);
}


/*
 *  call-seq:
 *
 *     mtch.values_at([index]*)   -> array
 *
 *  Uses each <i>index</i> to access the matching values, returning an array of
 *  the corresponding matches.
 *
 *     m = /(.)(.)(\d+)(\d)/.match("THX1138: The Movie")
 *     m.to_a               #=> ["HX1138", "H", "X", "113", "8"]
 *     m.values_at(0, 2, -2)   #=> ["HX1138", "X", "113"]
 */

static VALUE
match_values_at(int argc, VALUE *argv, VALUE match)
{
    struct re_registers *regs;

    match_check(match);
    regs = RMATCH_REGS(match);
    return rb_get_values_at(match, regs->num_regs, argc, argv, match_entry);
}


/*
 *  call-seq:
 *     mtch.to_s   -> str
 *
 *  Returns the entire matched string.
 *
 *     m = /(.)(.)(\d+)(\d)/.match("THX1138.")
 *     m.to_s   #=> "HX1138"
 */

static VALUE
match_to_s(VALUE match)
{
    VALUE str = rb_reg_last_match(match);

    match_check(match);
    if (NIL_P(str)) str = rb_str_new(0,0);
    if (OBJ_TAINTED(match)) OBJ_TAINT(str);
    if (OBJ_TAINTED(RMATCH(match)->str)) OBJ_TAINT(str);
    return str;
}


/*
 *  call-seq:
 *     mtch.string   -> str
 *
 *  Returns a frozen copy of the string passed in to <code>match</code>.
 *
 *     m = /(.)(.)(\d+)(\d)/.match("THX1138.")
 *     m.string   #=> "THX1138."
 */

static VALUE
match_string(VALUE match)
{
    match_check(match);
    return RMATCH(match)->str;	/* str is frozen */
}

struct backref_name_tag {
    const UChar *name;
    long len;
};

static int
match_inspect_name_iter(const OnigUChar *name, const OnigUChar *name_end,
          int back_num, int *back_refs, OnigRegex regex, void *arg0)
{
    struct backref_name_tag *arg = (struct backref_name_tag *)arg0;
    int i;

    for (i = 0; i < back_num; i++) {
        arg[back_refs[i]].name = name;
        arg[back_refs[i]].len = name_end - name;
    }
    return 0;
}

/*
 * call-seq:
 *    mtch.inspect   -> str
 *
 * Returns a printable version of <i>mtch</i>.
 *
 *     puts /.$/.match("foo").inspect
 *     #=> #<MatchData "o">
 *
 *     puts /(.)(.)(.)/.match("foo").inspect
 *     #=> #<MatchData "foo" 1:"f" 2:"o" 3:"o">
 *
 *     puts /(.)(.)?(.)/.match("fo").inspect
 *     #=> #<MatchData "fo" 1:"f" 2:nil 3:"o">
 *
 *     puts /(?<foo>.)(?<bar>.)(?<baz>.)/.match("hoge").inspect
 *     #=> #<MatchData "hog" foo:"h" bar:"o" baz:"g">
 *
 */

static VALUE
match_inspect(VALUE match)
{
    const char *cname = rb_obj_classname(match);
    VALUE str;
    int i;
    struct re_registers *regs = RMATCH_REGS(match);
    int num_regs = regs->num_regs;
    struct backref_name_tag *names;
    VALUE regexp = RMATCH(match)->regexp;

    if (regexp == 0) {
        return rb_sprintf("#<%s:%p>", cname, (void*)match);
    }

    names = ALLOCA_N(struct backref_name_tag, num_regs);
    MEMZERO(names, struct backref_name_tag, num_regs);

    onig_foreach_name(RREGEXP(regexp)->ptr,
            match_inspect_name_iter, names);

    str = rb_str_buf_new2("#<");
    rb_str_buf_cat2(str, cname);

    for (i = 0; i < num_regs; i++) {
        VALUE v;
        rb_str_buf_cat2(str, " ");
        if (0 < i) {
            if (names[i].name)
                rb_str_buf_cat(str, (const char *)names[i].name, names[i].len);
            else {
                rb_str_catf(str, "%d", i);
            }
            rb_str_buf_cat2(str, ":");
        }
        v = rb_reg_nth_match(i, match);
        if (v == Qnil)
            rb_str_buf_cat2(str, "nil");
        else
            rb_str_buf_append(str, rb_str_inspect(v));
    }
    rb_str_buf_cat2(str, ">");

    return str;
}

VALUE rb_cRegexp;

static int
read_escaped_byte(const char **pp, const char *end, onig_errmsg_buffer err)
{
    const char *p = *pp;
    int code;
    int meta_prefix = 0, ctrl_prefix = 0;
    size_t len;

    if (p == end || *p++ != '\\') {
        errcpy(err, "too short escaped multibyte character");
        return -1;
    }

again:
    if (p == end) {
        errcpy(err, "too short escape sequence");
        return -1;
    }
    switch (*p++) {
      case '\\': code = '\\'; break;
      case 'n': code = '\n'; break;
      case 't': code = '\t'; break;
      case 'r': code = '\r'; break;
      case 'f': code = '\f'; break;
      case 'v': code = '\013'; break;
      case 'a': code = '\007'; break;
      case 'e': code = '\033'; break;

      /* \OOO */
      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7':
        p--;
        code = scan_oct(p, end < p+3 ? end-p : 3, &len);
        p += len;
        break;

      case 'x': /* \xHH */
        code = scan_hex(p, end < p+2 ? end-p : 2, &len);
        if (len < 1) {
            errcpy(err, "invalid hex escape");
            return -1;
        }
        p += len;
        break;

      case 'M': /* \M-X, \M-\C-X, \M-\cX */
        if (meta_prefix) {
            errcpy(err, "duplicate meta escape");
            return -1;
        }
        meta_prefix = 1;
        if (p+1 < end && *p++ == '-' && (*p & 0x80) == 0) {
            if (*p == '\\') {
                p++;
                goto again;
            }
            else {
                code = *p++;
                break;
            }
        }
        errcpy(err, "too short meta escape");
        return -1;

      case 'C': /* \C-X, \C-\M-X */
        if (p == end || *p++ != '-') {
            errcpy(err, "too short control escape");
            return -1;
        }
      case 'c': /* \cX, \c\M-X */
        if (ctrl_prefix) {
            errcpy(err, "duplicate control escape");
            return -1;
        }
        ctrl_prefix = 1;
        if (p < end && (*p & 0x80) == 0) {
            if (*p == '\\') {
                p++;
                goto again;
            }
            else {
                code = *p++;
                break;
            }
        }
        errcpy(err, "too short control escape");
        return -1;

      default:
        errcpy(err, "unexpected escape sequence");
        return -1;
    }
    if (code < 0 || 0xff < code) {
        errcpy(err, "invalid escape code");
        return -1;
    }

    if (ctrl_prefix)
        code &= 0x1f;
    if (meta_prefix)
        code |= 0x80;

    *pp = p;
    return code;
}

static int
unescape_escaped_nonascii(const char **pp, const char *end, rb_encoding *enc,
        VALUE buf, rb_encoding **encp, onig_errmsg_buffer err)
{
    const char *p = *pp;
    int chmaxlen = rb_enc_mbmaxlen(enc);
    char *chbuf = ALLOCA_N(char, chmaxlen);
    int chlen = 0;
    int byte;
    int l;

    memset(chbuf, 0, chmaxlen);

    byte = read_escaped_byte(&p, end, err);
    if (byte == -1) {
        return -1;
    }

    chbuf[chlen++] = byte;
    while (chlen < chmaxlen &&
           MBCLEN_NEEDMORE_P(rb_enc_precise_mbclen(chbuf, chbuf+chlen, enc))) {
        byte = read_escaped_byte(&p, end, err);
        if (byte == -1) {
            return -1;
        }
        chbuf[chlen++] = byte;
    }

    l = rb_enc_precise_mbclen(chbuf, chbuf+chlen, enc);
    if (MBCLEN_INVALID_P(l)) {
        errcpy(err, "invalid multibyte escape");
        return -1;
    }
    if (1 < chlen || (chbuf[0] & 0x80)) {
        rb_str_buf_cat(buf, chbuf, chlen);

        if (*encp == 0)
            *encp = enc;
        else if (*encp != enc) {
            errcpy(err, "escaped non ASCII character in UTF-8 regexp");
            return -1;
        }
    }
    else {
        char escbuf[5];
        snprintf(escbuf, sizeof(escbuf), "\\x%02X", chbuf[0]&0xff);
        rb_str_buf_cat(buf, escbuf, 4);
    }
    *pp = p;
    return 0;
}

static int
check_unicode_range(unsigned long code, onig_errmsg_buffer err)
{
    if ((0xd800 <= code && code <= 0xdfff) || /* Surrogates */
        0x10ffff < code) {
        errcpy(err, "invalid Unicode range");
        return -1;
    }
    return 0;
}

static int
append_utf8(unsigned long uv,
        VALUE buf, rb_encoding **encp, onig_errmsg_buffer err)
{
    if (check_unicode_range(uv, err) != 0)
        return -1;
    if (uv < 0x80) {
        char escbuf[5];
        snprintf(escbuf, sizeof(escbuf), "\\x%02X", (int)uv);
        rb_str_buf_cat(buf, escbuf, 4);
    }
    else {
        int len;
        char utf8buf[6];
        len = rb_uv_to_utf8(utf8buf, uv);
        rb_str_buf_cat(buf, utf8buf, len);

        if (*encp == 0)
            *encp = rb_utf8_encoding();
        else if (*encp != rb_utf8_encoding()) {
            errcpy(err, "UTF-8 character in non UTF-8 regexp");
            return -1;
        }
    }
    return 0;
}

static int
unescape_unicode_list(const char **pp, const char *end,
        VALUE buf, rb_encoding **encp, onig_errmsg_buffer err)
{
    const char *p = *pp;
    int has_unicode = 0;
    unsigned long code;
    size_t len;

    while (p < end && ISSPACE(*p)) p++;

    while (1) {
        code = ruby_scan_hex(p, end-p, &len);
        if (len == 0)
            break;
        if (6 < len) { /* max 10FFFF */
            errcpy(err, "invalid Unicode range");
            return -1;
        }
        p += len;
        if (append_utf8(code, buf, encp, err) != 0)
            return -1;
        has_unicode = 1;

        while (p < end && ISSPACE(*p)) p++;
    }

    if (has_unicode == 0) {
        errcpy(err, "invalid Unicode list");
        return -1;
    }

    *pp = p;

    return 0;
}

static int
unescape_unicode_bmp(const char **pp, const char *end,
        VALUE buf, rb_encoding **encp, onig_errmsg_buffer err)
{
    const char *p = *pp;
    size_t len;
    unsigned long code;

    if (end < p+4) {
        errcpy(err, "invalid Unicode escape");
        return -1;
    }
    code = ruby_scan_hex(p, 4, &len);
    if (len != 4) {
        errcpy(err, "invalid Unicode escape");
        return -1;
    }
    if (append_utf8(code, buf, encp, err) != 0)
        return -1;
    *pp = p + 4;
    return 0;
}

static int
unescape_nonascii(const char *p, const char *end, rb_encoding *enc,
        VALUE buf, rb_encoding **encp, int *has_property,
        onig_errmsg_buffer err)
{
    char c;
    char smallbuf[2];

    while (p < end) {
        int chlen = rb_enc_precise_mbclen(p, end, enc);
        if (!MBCLEN_CHARFOUND_P(chlen)) {
            errcpy(err, "invalid multibyte character");
            return -1;
        }
        chlen = MBCLEN_CHARFOUND_LEN(chlen);
        if (1 < chlen || (*p & 0x80)) {
            rb_str_buf_cat(buf, p, chlen);
            p += chlen;
            if (*encp == 0)
                *encp = enc;
            else if (*encp != enc) {
                errcpy(err, "non ASCII character in UTF-8 regexp");
                return -1;
            }
            continue;
        }

        switch (c = *p++) {
          case '\\':
            if (p == end) {
                errcpy(err, "too short escape sequence");
                return -1;
            }
            switch (c = *p++) {
              case '1': case '2': case '3':
              case '4': case '5': case '6': case '7': /* \O, \OO, \OOO or backref */
                {
                    size_t octlen;
                    if (ruby_scan_oct(p-1, end-(p-1), &octlen) <= 0177) {
                        /* backref or 7bit octal.
                           no need to unescape anyway.
                           re-escaping may break backref */
                        goto escape_asis;
                    }
                }
                /* xxx: How about more than 199 subexpressions? */

              case '0': /* \0, \0O, \0OO */

              case 'x': /* \xHH */
              case 'c': /* \cX, \c\M-X */
              case 'C': /* \C-X, \C-\M-X */
              case 'M': /* \M-X, \M-\C-X, \M-\cX */
                p = p-2;
                if (unescape_escaped_nonascii(&p, end, enc, buf, encp, err) != 0)
                    return -1;
                break;

              case 'u':
                if (p == end) {
                    errcpy(err, "too short escape sequence");
                    return -1;
                }
                if (*p == '{') {
                    /* \u{H HH HHH HHHH HHHHH HHHHHH ...} */
                    p++;
                    if (unescape_unicode_list(&p, end, buf, encp, err) != 0)
                        return -1;
                    if (p == end || *p++ != '}') {
                        errcpy(err, "invalid Unicode list");
                        return -1;
                    }
                    break;
                }
                else {
                    /* \uHHHH */
                    if (unescape_unicode_bmp(&p, end, buf, encp, err) != 0)
                        return -1;
                    break;
                }

              case 'p': /* \p{Hiragana} */
              case 'P':
                if (!*encp) {
                    *has_property = 1;
                }
                goto escape_asis;

              default: /* \n, \\, \d, \9, etc. */
escape_asis:
                smallbuf[0] = '\\';
                smallbuf[1] = c;
                rb_str_buf_cat(buf, smallbuf, 2);
                break;
            }
            break;

          default:
            rb_str_buf_cat(buf, &c, 1);
            break;
        }
    }

    return 0;
}

static VALUE
rb_reg_preprocess(const char *p, const char *end, rb_encoding *enc,
        rb_encoding **fixed_enc, onig_errmsg_buffer err)
{
    VALUE buf;
    int has_property = 0;

    buf = rb_str_buf_new(0);

    if (rb_enc_asciicompat(enc))
        *fixed_enc = 0;
    else {
        *fixed_enc = enc;
        rb_enc_associate(buf, enc);
    }

    if (unescape_nonascii(p, end, enc, buf, fixed_enc, &has_property, err) != 0)
        return Qnil;

    if (has_property && !*fixed_enc) {
        *fixed_enc = enc;
    }

    if (*fixed_enc) {
        rb_enc_associate(buf, *fixed_enc);
    }

    return buf;
}

VALUE
rb_reg_check_preprocess(VALUE str)
{
    rb_encoding *fixed_enc = 0;
    onig_errmsg_buffer err = "";
    VALUE buf;
    char *p, *end;
    rb_encoding *enc;

    StringValue(str);
    p = RSTRING_PTR(str);
    end = p + RSTRING_LEN(str);
    enc = rb_enc_get(str);

    buf = rb_reg_preprocess(p, end, enc, &fixed_enc, err);
    RB_GC_GUARD(str);

    if (buf == Qnil) {
	return rb_reg_error_desc(str, 0, err);
    }
    return Qnil;
}

static VALUE
rb_reg_preprocess_dregexp(VALUE ary, int options)
{
    rb_encoding *fixed_enc = 0;
    rb_encoding *regexp_enc = 0;
    onig_errmsg_buffer err = "";
    int i;
    VALUE result = 0;
    rb_encoding *ascii8bit = rb_ascii8bit_encoding();

    if (RARRAY_LEN(ary) == 0) {
        rb_raise(rb_eArgError, "no arguments given");
    }

    for (i = 0; i < RARRAY_LEN(ary); i++) {
        VALUE str = RARRAY_PTR(ary)[i];
        VALUE buf;
        char *p, *end;
        rb_encoding *src_enc;

	src_enc = rb_enc_get(str);
	if (options & ARG_ENCODING_NONE &&
		src_enc != ascii8bit) {
	    if (rb_enc_str_coderange(str) != ENC_CODERANGE_7BIT)
		rb_raise(rb_eRegexpError, "/.../n has a non escaped non ASCII character in non ASCII-8BIT script");
	    else
		src_enc = ascii8bit;
	}

        StringValue(str);
        p = RSTRING_PTR(str);
        end = p + RSTRING_LEN(str);

        buf = rb_reg_preprocess(p, end, src_enc, &fixed_enc, err);

        if (buf == Qnil)
            rb_raise(rb_eArgError, "%s", err);

        if (fixed_enc != 0) {
            if (regexp_enc != 0 && regexp_enc != fixed_enc) {
                rb_raise(rb_eRegexpError, "encoding mismatch in dynamic regexp : %s and %s",
                         rb_enc_name(regexp_enc), rb_enc_name(fixed_enc));
            }
            regexp_enc = fixed_enc;
        }

        if (!result)
            result = rb_str_new3(str);
        else
            rb_str_buf_append(result, str);
    }
    if (regexp_enc) {
        rb_enc_associate(result, regexp_enc);
    }

    return result;
}

static int
rb_reg_initialize(VALUE obj, const char *s, long len, rb_encoding *enc,
		  int options, onig_errmsg_buffer err,
		  const char *sourcefile, int sourceline)
{
    struct RRegexp *re = RREGEXP(obj);
    VALUE unescaped;
    rb_encoding *fixed_enc = 0;
    rb_encoding *a_enc = rb_ascii8bit_encoding();

    if (!OBJ_UNTRUSTED(obj) && rb_safe_level() >= 4)
	rb_raise(rb_eSecurityError, "Insecure: can't modify regexp");
    rb_check_frozen(obj);
    if (FL_TEST(obj, REG_LITERAL))
	rb_raise(rb_eSecurityError, "can't modify literal regexp");
    if (re->ptr)
        rb_raise(rb_eTypeError, "already initialized regexp");
    re->ptr = 0;

    if (rb_enc_dummy_p(enc)) {
	errcpy(err, "can't make regexp with dummy encoding");
	return -1;
    }

    unescaped = rb_reg_preprocess(s, s+len, enc, &fixed_enc, err);
    if (unescaped == Qnil)
        return -1;

    if (fixed_enc) {
	if ((fixed_enc != enc && (options & ARG_ENCODING_FIXED)) ||
            (fixed_enc != a_enc && (options & ARG_ENCODING_NONE))) {
	    errcpy(err, "incompatible character encoding");
	    return -1;
	}
        if (fixed_enc != a_enc) {
	    options |= ARG_ENCODING_FIXED;
	    enc = fixed_enc;
	}
    }
    else if (!(options & ARG_ENCODING_FIXED)) {
       enc = rb_usascii_encoding();
    }

    rb_enc_associate((VALUE)re, enc);
    if ((options & ARG_ENCODING_FIXED) || fixed_enc) {
	re->basic.flags |= KCODE_FIXED;
    }
    if (options & ARG_ENCODING_NONE) {
        re->basic.flags |= REG_ENCODING_NONE;
    }

    re->ptr = make_regexp(RSTRING_PTR(unescaped), RSTRING_LEN(unescaped), enc,
			  options & ARG_REG_OPTION_MASK, err,
			  sourcefile, sourceline);
    if (!re->ptr) return -1;
    re->src = rb_enc_str_new(s, len, enc);
    OBJ_FREEZE(re->src);
    RB_GC_GUARD(unescaped);
    return 0;
}

static int
rb_reg_initialize_str(VALUE obj, VALUE str, int options, onig_errmsg_buffer err,
	const char *sourcefile, int sourceline)
{
    int ret;
    rb_encoding *enc = rb_enc_get(str);
    if (options & ARG_ENCODING_NONE) {
        rb_encoding *ascii8bit = rb_ascii8bit_encoding();
        if (enc != ascii8bit) {
            if (rb_enc_str_coderange(str) != ENC_CODERANGE_7BIT) {
                errcpy(err, "/.../n has a non escaped non ASCII character in non ASCII-8BIT script");
                return -1;
            }
            enc = ascii8bit;
        }
    }
    ret = rb_reg_initialize(obj, RSTRING_PTR(str), RSTRING_LEN(str), enc,
			    options, err, sourcefile, sourceline);
    OBJ_INFECT(obj, str);
    RB_GC_GUARD(str);
    return ret;
}

static VALUE
rb_reg_s_alloc(VALUE klass)
{
    NEWOBJ(re, struct RRegexp);
    OBJSETUP(re, klass, T_REGEXP);

    re->ptr = 0;
    re->src = 0;
    re->usecnt = 0;

    return (VALUE)re;
}

VALUE
rb_reg_alloc(void)
{
    return rb_reg_s_alloc(rb_cRegexp);
}

VALUE
rb_reg_new_str(VALUE s, int options)
{
    return rb_reg_init_str(rb_reg_alloc(), s, options);
}

VALUE
rb_reg_init_str(VALUE re, VALUE s, int options)
{
    onig_errmsg_buffer err = "";

    if (rb_reg_initialize_str(re, s, options, err, NULL, 0) != 0) {
	rb_reg_raise_str(s, options, err);
    }

    return re;
}

VALUE
rb_reg_new_ary(VALUE ary, int opt)
{
    return rb_reg_new_str(rb_reg_preprocess_dregexp(ary, opt), opt);
}

VALUE
rb_enc_reg_new(const char *s, long len, rb_encoding *enc, int options)
{
    VALUE re = rb_reg_alloc();
    onig_errmsg_buffer err = "";

    if (rb_reg_initialize(re, s, len, enc, options, err, NULL, 0) != 0) {
	rb_enc_reg_raise(s, len, enc, options, err);
    }

    return re;
}

VALUE
rb_reg_new(const char *s, long len, int options)
{
    return rb_enc_reg_new(s, len, rb_ascii8bit_encoding(), options);
}

VALUE
rb_reg_compile(VALUE str, int options, const char *sourcefile, int sourceline)
{
    VALUE re = rb_reg_alloc();
    onig_errmsg_buffer err = "";

    if (!str) str = rb_str_new(0,0);
    if (rb_reg_initialize_str(re, str, options, err, sourcefile, sourceline) != 0) {
	rb_set_errinfo(rb_reg_error_desc(str, options, err));
	return Qnil;
    }
    FL_SET(re, REG_LITERAL);
    return re;
}

static VALUE reg_cache;

VALUE
rb_reg_regcomp(VALUE str)
{
    volatile VALUE save_str = str;
    if (reg_cache && RREGEXP_SRC_LEN(reg_cache) == RSTRING_LEN(str)
	&& ENCODING_GET(reg_cache) == ENCODING_GET(str)
	&& memcmp(RREGEXP_SRC_PTR(reg_cache), RSTRING_PTR(str), RSTRING_LEN(str)) == 0)
	return reg_cache;

    return reg_cache = rb_reg_new_str(save_str, 0);
}

static st_index_t reg_hash(VALUE re);
/*
 * call-seq:
 *   rxp.hash   -> fixnum
 *
 * Produce a hash based on the text and options of this regular expression.
 */

static VALUE
rb_reg_hash(VALUE re)
{
    st_index_t hashval = reg_hash(re);
    return LONG2FIX(hashval);
}

static st_index_t
reg_hash(VALUE re)
{
    st_index_t hashval;

    rb_reg_check(re);
    hashval = RREGEXP(re)->ptr->options;
    hashval = rb_hash_uint(hashval, rb_memhash(RREGEXP_SRC_PTR(re), RREGEXP_SRC_LEN(re)));
    return rb_hash_end(hashval);
}


/*
 *  call-seq:
 *     rxp == other_rxp      -> true or false
 *     rxp.eql?(other_rxp)   -> true or false
 *
 *  Equality---Two regexps are equal if their patterns are identical, they have
 *  the same character set code, and their <code>casefold?</code> values are the
 *  same.
 *
 *     /abc/  == /abc/x   #=> false
 *     /abc/  == /abc/i   #=> false
 *     /abc/  == /abc/n   #=> false
 *     /abc/u == /abc/n   #=> false
 */

static VALUE
rb_reg_equal(VALUE re1, VALUE re2)
{
    if (re1 == re2) return Qtrue;
    if (!RB_TYPE_P(re2, T_REGEXP)) return Qfalse;
    rb_reg_check(re1); rb_reg_check(re2);
    if (FL_TEST(re1, KCODE_FIXED) != FL_TEST(re2, KCODE_FIXED)) return Qfalse;
    if (RREGEXP(re1)->ptr->options != RREGEXP(re2)->ptr->options) return Qfalse;
    if (RREGEXP_SRC_LEN(re1) != RREGEXP_SRC_LEN(re2)) return Qfalse;
    if (ENCODING_GET(re1) != ENCODING_GET(re2)) return Qfalse;
    if (memcmp(RREGEXP_SRC_PTR(re1), RREGEXP_SRC_PTR(re2), RREGEXP_SRC_LEN(re1)) == 0) {
	return Qtrue;
    }
    return Qfalse;
}

/*
 * call-seq:
 *    mtch.hash   -> integer
 *
 * Produce a hash based on the target string, regexp and matched
 * positions of this matchdata.
 */

static VALUE
match_hash(VALUE match)
{
    const struct re_registers *regs;
    st_index_t hashval = rb_hash_start(rb_str_hash(RMATCH(match)->str));

    rb_hash_uint(hashval, reg_hash(RMATCH(match)->regexp));
    regs = RMATCH_REGS(match);
    hashval = rb_hash_uint(hashval, regs->num_regs);
    hashval = rb_hash_uint(hashval, rb_memhash(regs->beg, regs->num_regs * sizeof(*regs->beg)));
    hashval = rb_hash_uint(hashval, rb_memhash(regs->end, regs->num_regs * sizeof(*regs->end)));
    hashval = rb_hash_end(hashval);
    return LONG2FIX(hashval);
}

/*
 * call-seq:
 *    mtch == mtch2   -> true or false
 *
 *  Equality---Two matchdata are equal if their target strings,
 *  patterns, and matched positions are identical.
 */

static VALUE
match_equal(VALUE match1, VALUE match2)
{
    const struct re_registers *regs1, *regs2;
    if (match1 == match2) return Qtrue;
    if (!RB_TYPE_P(match2, T_MATCH)) return Qfalse;
    if (!rb_str_equal(RMATCH(match1)->str, RMATCH(match2)->str)) return Qfalse;
    if (!rb_reg_equal(RMATCH(match1)->regexp, RMATCH(match2)->regexp)) return Qfalse;
    regs1 = RMATCH_REGS(match1);
    regs2 = RMATCH_REGS(match2);
    if (regs1->num_regs != regs2->num_regs) return Qfalse;
    if (memcmp(regs1->beg, regs2->beg, regs1->num_regs * sizeof(*regs1->beg))) return Qfalse;
    if (memcmp(regs1->end, regs2->end, regs1->num_regs * sizeof(*regs1->end))) return Qfalse;
    return Qtrue;
}

static VALUE
reg_operand(VALUE s, int check)
{
    if (SYMBOL_P(s)) {
	return rb_sym_to_s(s);
    }
    else {
	VALUE tmp = rb_check_string_type(s);
	if (check && NIL_P(tmp)) {
	    rb_raise(rb_eTypeError, "can't convert %s to String",
		     rb_obj_classname(s));
	}
	return tmp;
    }
}

static long
reg_match_pos(VALUE re, VALUE *strp, long pos)
{
    VALUE str = *strp;

    if (NIL_P(str)) {
	rb_backref_set(Qnil);
	return -1;
    }
    *strp = str = reg_operand(str, TRUE);
    if (pos != 0) {
	if (pos < 0) {
	    VALUE l = rb_str_length(str);
	    pos += NUM2INT(l);
	    if (pos < 0) {
		return pos;
	    }
	}
	pos = rb_str_offset(str, pos);
    }
    return rb_reg_search(re, str, pos, 0);
}

/*
 *  call-seq:
 *     rxp =~ str    -> integer or nil
 *
 *  Match---Matches <i>rxp</i> against <i>str</i>.
 *
 *     /at/ =~ "input data"   #=> 7
 *     /ax/ =~ "input data"   #=> nil
 *
 *  If <code>=~</code> is used with a regexp literal with named captures,
 *  captured strings (or nil) is assigned to local variables named by
 *  the capture names.
 *
 *     /(?<lhs>\w+)\s*=\s*(?<rhs>\w+)/ =~ "  x = y  "
 *     p lhs    #=> "x"
 *     p rhs    #=> "y"
 *
 *  If it is not matched, nil is assigned for the variables.
 *
 *     /(?<lhs>\w+)\s*=\s*(?<rhs>\w+)/ =~ "  x = "
 *     p lhs    #=> nil
 *     p rhs    #=> nil
 *
 *  This assignment is implemented in the Ruby parser.
 *  The parser detects 'regexp-literal =~ expression' for the assignment.
 *  The regexp must be a literal without interpolation and placed at left hand side.
 *
 *  The assignment does not occur if the regexp is not a literal.
 *
 *     re = /(?<lhs>\w+)\s*=\s*(?<rhs>\w+)/
 *     re =~ "  x = y  "
 *     p lhs    # undefined local variable
 *     p rhs    # undefined local variable
 *
 *  A regexp interpolation, <code>#{}</code>, also disables
 *  the assignment.
 *
 *     rhs_pat = /(?<rhs>\w+)/
 *     /(?<lhs>\w+)\s*=\s*#{rhs_pat}/ =~ "x = y"
 *     p lhs    # undefined local variable
 *
 *  The assignment does not occur if the regexp is placed at the right hand side.
 *
 *    "  x = y  " =~ /(?<lhs>\w+)\s*=\s*(?<rhs>\w+)/
 *    p lhs, rhs # undefined local variable
 *
 */

VALUE
rb_reg_match(VALUE re, VALUE str)
{
    long pos = reg_match_pos(re, &str, 0);
    if (pos < 0) return Qnil;
    pos = rb_str_sublen(str, pos);
    return LONG2FIX(pos);
}

/*
 *  call-seq:
 *     rxp === str   -> true or false
 *
 *  Case Equality---Synonym for <code>Regexp#=~</code> used in case statements.
 *
 *     a = "HELLO"
 *     case a
 *     when /^[a-z]*$/; print "Lower case\n"
 *     when /^[A-Z]*$/; print "Upper case\n"
 *     else;            print "Mixed case\n"
 *     end
 *
 *  <em>produces:</em>
 *
 *     Upper case
 */

VALUE
rb_reg_eqq(VALUE re, VALUE str)
{
    long start;

    str = reg_operand(str, FALSE);
    if (NIL_P(str)) {
	rb_backref_set(Qnil);
	return Qfalse;
    }
    start = rb_reg_search(re, str, 0, 0);
    if (start < 0) {
	return Qfalse;
    }
    return Qtrue;
}


/*
 *  call-seq:
 *     ~ rxp   -> integer or nil
 *
 *  Match---Matches <i>rxp</i> against the contents of <code>$_</code>.
 *  Equivalent to <code><i>rxp</i> =~ $_</code>.
 *
 *     $_ = "input data"
 *     ~ /at/   #=> 7
 */

VALUE
rb_reg_match2(VALUE re)
{
    long start;
    VALUE line = rb_lastline_get();

    if (!RB_TYPE_P(line, T_STRING)) {
	rb_backref_set(Qnil);
	return Qnil;
    }

    start = rb_reg_search(re, line, 0, 0);
    if (start < 0) {
	return Qnil;
    }
    start = rb_str_sublen(line, start);
    return LONG2FIX(start);
}


/*
 *  call-seq:
 *     rxp.match(str)       -> matchdata or nil
 *     rxp.match(str,pos)   -> matchdata or nil
 *
 *  Returns a <code>MatchData</code> object describing the match, or
 *  <code>nil</code> if there was no match. This is equivalent to retrieving the
 *  value of the special variable <code>$~</code> following a normal match.
 *  If the second parameter is present, it specifies the position in the string
 *  to begin the search.
 *
 *     /(.)(.)(.)/.match("abc")[2]   #=> "b"
 *     /(.)(.)/.match("abc", 1)[2]   #=> "c"
 *
 *  If a block is given, invoke the block with MatchData if match succeed, so
 *  that you can write
 *
 *     pat.match(str) {|m| ...}
 *
 *  instead of
 *
 *     if m = pat.match(str)
 *       ...
 *     end
 *
 *  The return value is a value from block execution in this case.
 */

static VALUE
rb_reg_match_m(int argc, VALUE *argv, VALUE re)
{
    VALUE result, str, initpos;
    long pos;

    if (rb_scan_args(argc, argv, "11", &str, &initpos) == 2) {
	pos = NUM2LONG(initpos);
    }
    else {
	pos = 0;
    }

    pos = reg_match_pos(re, &str, pos);
    if (pos < 0) {
	rb_backref_set(Qnil);
	return Qnil;
    }
    result = rb_backref_get();
    rb_match_busy(result);
    if (!NIL_P(result) && rb_block_given_p()) {
	return rb_yield(result);
    }
    return result;
}

/*
 * Document-method: compile
 *
 * Synonym for <code>Regexp.new</code>
 */


/*
 *  call-seq:
 *     Regexp.new(string, [options [, lang]])        -> regexp
 *     Regexp.new(regexp)                            -> regexp
 *     Regexp.compile(string, [options [, lang]])    -> regexp
 *     Regexp.compile(regexp)                        -> regexp
 *
 *  Constructs a new regular expression from <i>pattern</i>, which can be either
 *  a <code>String</code> or a <code>Regexp</code> (in which case that regexp's
 *  options are propagated, and new options may not be specified (a change as of
 *  Ruby 1.8). If <i>options</i> is a <code>Fixnum</code>, it should be one or
 *  more of the constants <code>Regexp::EXTENDED</code>,
 *  <code>Regexp::IGNORECASE</code>, and <code>Regexp::MULTILINE</code>,
 *  <em>or</em>-ed together. Otherwise, if <i>options</i> is not
 *  <code>nil</code>, the regexp will be case insensitive.
 *  When the <i>lang</i> parameter is `n' or `N' sets the regexp no encoding.
 *
 *     r1 = Regexp.new('^a-z+:\\s+\w+')           #=> /^a-z+:\s+\w+/
 *     r2 = Regexp.new('cat', true)               #=> /cat/i
 *     r3 = Regexp.new('dog', Regexp::EXTENDED)   #=> /dog/x
 *     r4 = Regexp.new(r2)                        #=> /cat/i
 */

static VALUE
rb_reg_initialize_m(int argc, VALUE *argv, VALUE self)
{
    onig_errmsg_buffer err = "";
    int flags = 0;
    VALUE str;
    rb_encoding *enc;
    const char *ptr;
    long len;

    rb_check_arity(argc, 1, 3);
    if (TYPE(argv[0]) == T_REGEXP) {
	VALUE re = argv[0];

	if (argc > 1) {
	    rb_warn("flags ignored");
	}
	rb_reg_check(re);
	flags = rb_reg_options(re);
	ptr = RREGEXP_SRC_PTR(re);
	len = RREGEXP_SRC_LEN(re);
	enc = rb_enc_get(re);
	if (rb_reg_initialize(self, ptr, len, enc, flags, err, NULL, 0)) {
	    str = rb_enc_str_new(ptr, len, enc);
	    rb_reg_raise_str(str, flags, err);
	}
    }
    else {
	if (argc >= 2) {
	    if (FIXNUM_P(argv[1])) flags = FIX2INT(argv[1]);
	    else if (RTEST(argv[1])) flags = ONIG_OPTION_IGNORECASE;
	}
	enc = 0;
	if (argc == 3 && !NIL_P(argv[2])) {
	    char *kcode = StringValuePtr(argv[2]);
	    if (kcode[0] == 'n' || kcode[0] == 'N') {
		enc = rb_ascii8bit_encoding();
		flags |= ARG_ENCODING_NONE;
	    }
	    else {
		rb_warn("encoding option is ignored - %s", kcode);
	    }
	}
	str = argv[0];
	ptr = StringValuePtr(str);
	if (enc
	    ? rb_reg_initialize(self, ptr, RSTRING_LEN(str), enc, flags, err, NULL, 0)
	    : rb_reg_initialize_str(self, str, flags, err, NULL, 0)) {
	    rb_reg_raise_str(str, flags, err);
	}
    }
    return self;
}

VALUE
rb_reg_quote(VALUE str)
{
    rb_encoding *enc = rb_enc_get(str);
    char *s, *send, *t;
    VALUE tmp;
    int c, clen;
    int ascii_only = rb_enc_str_asciionly_p(str);

    s = RSTRING_PTR(str);
    send = s + RSTRING_LEN(str);
    while (s < send) {
        c = rb_enc_ascget(s, send, &clen, enc);
	if (c == -1) {
            s += mbclen(s, send, enc);
	    continue;
	}
	switch (c) {
	  case '[': case ']': case '{': case '}':
	  case '(': case ')': case '|': case '-':
	  case '*': case '.': case '\\':
	  case '?': case '+': case '^': case '$':
	  case ' ': case '#':
	  case '\t': case '\f': case '\v': case '\n': case '\r':
	    goto meta_found;
	}
        s += clen;
    }
    tmp = rb_str_new3(str);
    if (ascii_only) {
        rb_enc_associate(tmp, rb_usascii_encoding());
    }
    return tmp;

  meta_found:
    tmp = rb_str_new(0, RSTRING_LEN(str)*2);
    if (ascii_only) {
        rb_enc_associate(tmp, rb_usascii_encoding());
    }
    else {
        rb_enc_copy(tmp, str);
    }
    t = RSTRING_PTR(tmp);
    /* copy upto metacharacter */
    memcpy(t, RSTRING_PTR(str), s - RSTRING_PTR(str));
    t += s - RSTRING_PTR(str);

    while (s < send) {
        c = rb_enc_ascget(s, send, &clen, enc);
	if (c == -1) {
	    int n = mbclen(s, send, enc);

	    while (n--)
		*t++ = *s++;
	    continue;
	}
        s += clen;
	switch (c) {
	  case '[': case ']': case '{': case '}':
	  case '(': case ')': case '|': case '-':
	  case '*': case '.': case '\\':
	  case '?': case '+': case '^': case '$':
	  case '#':
            t += rb_enc_mbcput('\\', t, enc);
	    break;
	  case ' ':
            t += rb_enc_mbcput('\\', t, enc);
            t += rb_enc_mbcput(' ', t, enc);
	    continue;
	  case '\t':
            t += rb_enc_mbcput('\\', t, enc);
            t += rb_enc_mbcput('t', t, enc);
	    continue;
	  case '\n':
            t += rb_enc_mbcput('\\', t, enc);
            t += rb_enc_mbcput('n', t, enc);
	    continue;
	  case '\r':
            t += rb_enc_mbcput('\\', t, enc);
            t += rb_enc_mbcput('r', t, enc);
	    continue;
	  case '\f':
            t += rb_enc_mbcput('\\', t, enc);
            t += rb_enc_mbcput('f', t, enc);
	    continue;
	  case '\v':
            t += rb_enc_mbcput('\\', t, enc);
            t += rb_enc_mbcput('v', t, enc);
	    continue;
	}
        t += rb_enc_mbcput(c, t, enc);
    }
    rb_str_resize(tmp, t - RSTRING_PTR(tmp));
    OBJ_INFECT(tmp, str);
    return tmp;
}


/*
 *  call-seq:
 *     Regexp.escape(str)   -> string
 *     Regexp.quote(str)    -> string
 *
 *  Escapes any characters that would have special meaning in a regular
 *  expression. Returns a new escaped string, or self if no characters are
 *  escaped.  For any string,
 *  <code>Regexp.new(Regexp.escape(<i>str</i>))=~<i>str</i></code> will be true.
 *
 *     Regexp.escape('\*?{}.')   #=> \\\*\?\{\}\.
 *
 */

static VALUE
rb_reg_s_quote(VALUE c, VALUE str)
{
    return rb_reg_quote(reg_operand(str, TRUE));
}

int
rb_reg_options(VALUE re)
{
    int options;

    rb_reg_check(re);
    options = RREGEXP(re)->ptr->options & ARG_REG_OPTION_MASK;
    if (RBASIC(re)->flags & KCODE_FIXED) options |= ARG_ENCODING_FIXED;
    if (RBASIC(re)->flags & REG_ENCODING_NONE) options |= ARG_ENCODING_NONE;
    return options;
}

VALUE
rb_check_regexp_type(VALUE re)
{
    return rb_check_convert_type(re, T_REGEXP, "Regexp", "to_regexp");
}

/*
 *  call-seq:
 *     Regexp.try_convert(obj) -> re or nil
 *
 *  Try to convert <i>obj</i> into a Regexp, using to_regexp method.
 *  Returns converted regexp or nil if <i>obj</i> cannot be converted
 *  for any reason.
 *
 *     Regexp.try_convert(/re/)         #=> /re/
 *     Regexp.try_convert("re")         #=> nil
 *
 *     o = Object.new
 *     Regexp.try_convert(o)            #=> nil
 *     def o.to_regexp() /foo/ end
 *     Regexp.try_convert(o)            #=> /foo/
 *
 */
static VALUE
rb_reg_s_try_convert(VALUE dummy, VALUE re)
{
    return rb_check_regexp_type(re);
}

static VALUE
rb_reg_s_union(VALUE self, VALUE args0)
{
    long argc = RARRAY_LEN(args0);

    if (argc == 0) {
        VALUE args[1];
        args[0] = rb_str_new2("(?!)");
        return rb_class_new_instance(1, args, rb_cRegexp);
    }
    else if (argc == 1) {
        VALUE arg = rb_ary_entry(args0, 0);
        VALUE re = rb_check_regexp_type(arg);
        if (!NIL_P(re))
            return re;
        else {
            VALUE quoted;
            quoted = rb_reg_s_quote(Qnil, arg);
            return rb_reg_new_str(quoted, 0);
        }
    }
    else {
	int i;
	VALUE source = rb_str_buf_new(0);
	rb_encoding *result_enc;

        int has_asciionly = 0;
        rb_encoding *has_ascii_compat_fixed = 0;
        rb_encoding *has_ascii_incompat = 0;

	for (i = 0; i < argc; i++) {
	    volatile VALUE v;
	    VALUE e = rb_ary_entry(args0, i);

	    if (0 < i)
		rb_str_buf_cat_ascii(source, "|");

	    v = rb_check_regexp_type(e);
	    if (!NIL_P(v)) {
                rb_encoding *enc = rb_enc_get(v);
                if (!rb_enc_asciicompat(enc)) {
                    if (!has_ascii_incompat)
                        has_ascii_incompat = enc;
                    else if (has_ascii_incompat != enc)
                        rb_raise(rb_eArgError, "incompatible encodings: %s and %s",
                            rb_enc_name(has_ascii_incompat), rb_enc_name(enc));
                }
                else if (rb_reg_fixed_encoding_p(v)) {
                    if (!has_ascii_compat_fixed)
                        has_ascii_compat_fixed = enc;
                    else if (has_ascii_compat_fixed != enc)
                        rb_raise(rb_eArgError, "incompatible encodings: %s and %s",
                            rb_enc_name(has_ascii_compat_fixed), rb_enc_name(enc));
                }
                else {
                    has_asciionly = 1;
                }
		v = rb_reg_to_s(v);
	    }
	    else {
                rb_encoding *enc;
                StringValue(e);
                enc = rb_enc_get(e);
                if (!rb_enc_str_asciicompat_p(e)) {
                    if (!has_ascii_incompat)
                        has_ascii_incompat = enc;
                    else if (has_ascii_incompat != enc)
                        rb_raise(rb_eArgError, "incompatible encodings: %s and %s",
                            rb_enc_name(has_ascii_incompat), rb_enc_name(enc));
                }
                else if (rb_enc_str_asciionly_p(e)) {
                    has_asciionly = 1;
                }
                else {
                    if (!has_ascii_compat_fixed)
                        has_ascii_compat_fixed = enc;
                    else if (has_ascii_compat_fixed != enc)
                        rb_raise(rb_eArgError, "incompatible encodings: %s and %s",
                            rb_enc_name(has_ascii_compat_fixed), rb_enc_name(enc));
                }
		v = rb_reg_s_quote(Qnil, e);
	    }
            if (has_ascii_incompat) {
                if (has_asciionly) {
                    rb_raise(rb_eArgError, "ASCII incompatible encoding: %s",
                        rb_enc_name(has_ascii_incompat));
                }
                if (has_ascii_compat_fixed) {
                    rb_raise(rb_eArgError, "incompatible encodings: %s and %s",
                        rb_enc_name(has_ascii_incompat), rb_enc_name(has_ascii_compat_fixed));
                }
            }

            if (i == 0) {
                rb_enc_copy(source, v);
            }
	    rb_str_append(source, v);
	}

        if (has_ascii_incompat) {
            result_enc = has_ascii_incompat;
        }
        else if (has_ascii_compat_fixed) {
            result_enc = has_ascii_compat_fixed;
        }
        else {
            result_enc = rb_ascii8bit_encoding();
        }

        rb_enc_associate(source, result_enc);
        return rb_class_new_instance(1, &source, rb_cRegexp);
    }
}

/*
 *  call-seq:
 *     Regexp.union(pat1, pat2, ...)            -> new_regexp
 *     Regexp.union(pats_ary)                   -> new_regexp
 *
 *  Return a <code>Regexp</code> object that is the union of the given
 *  <em>pattern</em>s, i.e., will match any of its parts. The <em>pattern</em>s
 *  can be Regexp objects, in which case their options will be preserved, or
 *  Strings. If no patterns are given, returns <code>/(?!)/</code>.
 *  The behavior is unspecified if any given <em>pattern</em> contains capture.
 *
 *     Regexp.union                         #=> /(?!)/
 *     Regexp.union("penzance")             #=> /penzance/
 *     Regexp.union("a+b*c")                #=> /a\+b\*c/
 *     Regexp.union("skiing", "sledding")   #=> /skiing|sledding/
 *     Regexp.union(["skiing", "sledding"]) #=> /skiing|sledding/
 *     Regexp.union(/dogs/, /cats/i)        #=> /(?-mix:dogs)|(?i-mx:cats)/
 */
static VALUE
rb_reg_s_union_m(VALUE self, VALUE args)
{
    VALUE v;
    if (RARRAY_LEN(args) == 1 &&
        !NIL_P(v = rb_check_array_type(rb_ary_entry(args, 0)))) {
        return rb_reg_s_union(self, v);
    }
    return rb_reg_s_union(self, args);
}

/* :nodoc: */
static VALUE
rb_reg_init_copy(VALUE copy, VALUE re)
{
    onig_errmsg_buffer err = "";
    const char *s;
    long len;

    if (copy == re) return copy;
    rb_check_frozen(copy);
    /* need better argument type check */
    if (!rb_obj_is_instance_of(re, rb_obj_class(copy))) {
	rb_raise(rb_eTypeError, "wrong argument type");
    }
    rb_reg_check(re);
    s = RREGEXP_SRC_PTR(re);
    len = RREGEXP_SRC_LEN(re);
    if (rb_reg_initialize(copy, s, len, rb_enc_get(re), rb_reg_options(re),
		err, NULL, 0) != 0) {
	rb_reg_raise(s, len, err, re);
    }
    return copy;
}

VALUE
rb_reg_regsub(VALUE str, VALUE src, struct re_registers *regs, VALUE regexp)
{
    VALUE val = 0;
    char *p, *s, *e;
    int no, clen;
    rb_encoding *str_enc = rb_enc_get(str);
    rb_encoding *src_enc = rb_enc_get(src);
    int acompat = rb_enc_asciicompat(str_enc);
#define ASCGET(s,e,cl) (acompat ? (*(cl)=1,ISASCII((s)[0])?(s)[0]:-1) : rb_enc_ascget((s), (e), (cl), str_enc))

    p = s = RSTRING_PTR(str);
    e = s + RSTRING_LEN(str);

    while (s < e) {
        int c = ASCGET(s, e, &clen);
	char *ss;

	if (c == -1) {
	    s += mbclen(s, e, str_enc);
	    continue;
	}
	ss = s;
        s += clen;

	if (c != '\\' || s == e) continue;

	if (!val) {
	    val = rb_str_buf_new(ss-p);
	}
        rb_enc_str_buf_cat(val, p, ss-p, str_enc);

        c = ASCGET(s, e, &clen);
        if (c == -1) {
            s += mbclen(s, e, str_enc);
	    rb_enc_str_buf_cat(val, ss, s-ss, str_enc);
            p = s;
	    continue;
        }
        s += clen;

	p = s;
	switch (c) {
	  case '1': case '2': case '3': case '4':
	  case '5': case '6': case '7': case '8': case '9':
            if (onig_noname_group_capture_is_active(RREGEXP(regexp)->ptr)) {
                no = c - '0';
            }
            else {
                continue;
            }
	    break;

          case 'k':
            if (s < e && ASCGET(s, e, &clen) == '<') {
                char *name, *name_end;

                name_end = name = s + clen;
                while (name_end < e) {
                    c = ASCGET(name_end, e, &clen);
                    if (c == '>') break;
                    name_end += c == -1 ? mbclen(name_end, e, str_enc) : clen;
                }
                if (name_end < e) {
                    no = name_to_backref_number(regs, regexp, name, name_end);
                    p = s = name_end + clen;
                    break;
                }
                else {
                    rb_raise(rb_eRuntimeError, "invalid group name reference format");
                }
            }

            rb_enc_str_buf_cat(val, ss, s-ss, str_enc);
            continue;

          case '0':
	  case '&':
	    no = 0;
	    break;

	  case '`':
	    rb_enc_str_buf_cat(val, RSTRING_PTR(src), BEG(0), src_enc);
	    continue;

	  case '\'':
	    rb_enc_str_buf_cat(val, RSTRING_PTR(src)+END(0), RSTRING_LEN(src)-END(0), src_enc);
	    continue;

	  case '+':
	    no = regs->num_regs-1;
	    while (BEG(no) == -1 && no > 0) no--;
	    if (no == 0) continue;
	    break;

	  case '\\':
	    rb_enc_str_buf_cat(val, s-clen, clen, str_enc);
	    continue;

	  default:
	    rb_enc_str_buf_cat(val, ss, s-ss, str_enc);
	    continue;
	}

	if (no >= 0) {
	    if (no >= regs->num_regs) continue;
	    if (BEG(no) == -1) continue;
	    rb_enc_str_buf_cat(val, RSTRING_PTR(src)+BEG(no), END(no)-BEG(no), src_enc);
	}
    }

    if (!val) return str;
    if (p < e) {
        rb_enc_str_buf_cat(val, p, e-p, str_enc);
    }

    return val;
}

static VALUE
kcode_getter(void)
{
    rb_warn("variable $KCODE is no longer effective");
    return Qnil;
}

static void
kcode_setter(VALUE val, ID id)
{
    rb_warn("variable $KCODE is no longer effective; ignored");
}

static VALUE
ignorecase_getter(void)
{
    rb_warn("variable $= is no longer effective");
    return Qfalse;
}

static void
ignorecase_setter(VALUE val, ID id)
{
    rb_warn("variable $= is no longer effective; ignored");
}

static VALUE
match_getter(void)
{
    VALUE match = rb_backref_get();

    if (NIL_P(match)) return Qnil;
    rb_match_busy(match);
    return match;
}

static void
match_setter(VALUE val)
{
    if (!NIL_P(val)) {
	Check_Type(val, T_MATCH);
    }
    rb_backref_set(val);
}

/*
 *  call-seq:
 *     Regexp.last_match           -> matchdata
 *     Regexp.last_match(n)        -> str
 *
 *  The first form returns the <code>MatchData</code> object generated by the
 *  last successful pattern match. Equivalent to reading the global variable
 *  <code>$~</code>. The second form returns the <i>n</i>th field in this
 *  <code>MatchData</code> object.
 *  <em>n</em> can be a string or symbol to reference a named capture.
 *
 *  Note that the <code>last_match</code> is local to the thread and method scope
 *  of the method that did the pattern match.
 *
 *     /c(.)t/ =~ 'cat'        #=> 0
 *     Regexp.last_match       #=> #<MatchData "cat" 1:"a">
 *     Regexp.last_match(0)    #=> "cat"
 *     Regexp.last_match(1)    #=> "a"
 *     Regexp.last_match(2)    #=> nil
 *
 *     /(?<lhs>\w+)\s*=\s*(?<rhs>\w+)/ =~ "var = val"
 *     Regexp.last_match       #=> #<MatchData "var = val" lhs:"var" rhs:"val">
 *     Regexp.last_match(:lhs) #=> "var"
 *     Regexp.last_match(:rhs) #=> "val"
 */

static VALUE
rb_reg_s_last_match(int argc, VALUE *argv)
{
    VALUE nth;

    if (argc > 0 && rb_scan_args(argc, argv, "01", &nth) == 1) {
        VALUE match = rb_backref_get();
        int n;
        if (NIL_P(match)) return Qnil;
        n = match_backref_number(match, nth);
	return rb_reg_nth_match(n, match);
    }
    return match_getter();
}

static void
re_warn(const char *s)
{
    rb_warn("%s", s);
}

/*
 *  Document-class: RegexpError
 *
 *  Raised when given an invalid regexp expression.
 *
 *     Regexp.new("?")
 *
 *  <em>raises the exception:</em>
 *
 *     RegexpError: target of repeat operator is not specified: /?/
 */

/*
 *  Document-class: Regexp
 *
 *  A <code>Regexp</code> holds a regular expression, used to match a pattern
 *  against strings. Regexps are created using the <code>/.../</code> and
 *  <code>%r{...}</code> literals, and by the <code>Regexp::new</code>
 *  constructor.
 *
 *  :include: doc/re.rdoc
 */

void
Init_Regexp(void)
{
    rb_eRegexpError = rb_define_class("RegexpError", rb_eStandardError);

    onigenc_set_default_caseconv_table((UChar*)casetable);
    onigenc_set_default_encoding(ONIG_ENCODING_ASCII);
    onig_set_warn_func(re_warn);
    onig_set_verb_warn_func(re_warn);

    rb_define_virtual_variable("$~", match_getter, match_setter);
    rb_define_virtual_variable("$&", last_match_getter, 0);
    rb_define_virtual_variable("$`", prematch_getter, 0);
    rb_define_virtual_variable("$'", postmatch_getter, 0);
    rb_define_virtual_variable("$+", last_paren_match_getter, 0);

    rb_define_virtual_variable("$=", ignorecase_getter, ignorecase_setter);
    rb_define_virtual_variable("$KCODE", kcode_getter, kcode_setter);
    rb_define_virtual_variable("$-K", kcode_getter, kcode_setter);

    rb_cRegexp = rb_define_class("Regexp", rb_cObject);
    rb_define_alloc_func(rb_cRegexp, rb_reg_s_alloc);
    rb_define_singleton_method(rb_cRegexp, "compile", rb_class_new_instance, -1);
    rb_define_singleton_method(rb_cRegexp, "quote", rb_reg_s_quote, 1);
    rb_define_singleton_method(rb_cRegexp, "escape", rb_reg_s_quote, 1);
    rb_define_singleton_method(rb_cRegexp, "union", rb_reg_s_union_m, -2);
    rb_define_singleton_method(rb_cRegexp, "last_match", rb_reg_s_last_match, -1);
    rb_define_singleton_method(rb_cRegexp, "try_convert", rb_reg_s_try_convert, 1);

    rb_define_method(rb_cRegexp, "initialize", rb_reg_initialize_m, -1);
    rb_define_method(rb_cRegexp, "initialize_copy", rb_reg_init_copy, 1);
    rb_define_method(rb_cRegexp, "hash", rb_reg_hash, 0);
    rb_define_method(rb_cRegexp, "eql?", rb_reg_equal, 1);
    rb_define_method(rb_cRegexp, "==", rb_reg_equal, 1);
    rb_define_method(rb_cRegexp, "=~", rb_reg_match, 1);
    rb_define_method(rb_cRegexp, "===", rb_reg_eqq, 1);
    rb_define_method(rb_cRegexp, "~", rb_reg_match2, 0);
    rb_define_method(rb_cRegexp, "match", rb_reg_match_m, -1);
    rb_define_method(rb_cRegexp, "to_s", rb_reg_to_s, 0);
    rb_define_method(rb_cRegexp, "inspect", rb_reg_inspect, 0);
    rb_define_method(rb_cRegexp, "source", rb_reg_source, 0);
    rb_define_method(rb_cRegexp, "casefold?", rb_reg_casefold_p, 0);
    rb_define_method(rb_cRegexp, "options", rb_reg_options_m, 0);
    rb_define_method(rb_cRegexp, "encoding", rb_obj_encoding, 0); /* in encoding.c */
    rb_define_method(rb_cRegexp, "fixed_encoding?", rb_reg_fixed_encoding_p, 0);
    rb_define_method(rb_cRegexp, "names", rb_reg_names, 0);
    rb_define_method(rb_cRegexp, "named_captures", rb_reg_named_captures, 0);

    /* see Regexp.options and Regexp.new */
    rb_define_const(rb_cRegexp, "IGNORECASE", INT2FIX(ONIG_OPTION_IGNORECASE));
    /* see Regexp.options and Regexp.new */
    rb_define_const(rb_cRegexp, "EXTENDED", INT2FIX(ONIG_OPTION_EXTEND));
    /* see Regexp.options and Regexp.new */
    rb_define_const(rb_cRegexp, "MULTILINE", INT2FIX(ONIG_OPTION_MULTILINE));
    /* see Regexp.options and Regexp.new */
    rb_define_const(rb_cRegexp, "FIXEDENCODING", INT2FIX(ARG_ENCODING_FIXED));
    /* see Regexp.options and Regexp.new */
    rb_define_const(rb_cRegexp, "NOENCODING", INT2FIX(ARG_ENCODING_NONE));

    rb_global_variable(&reg_cache);

    rb_cMatch  = rb_define_class("MatchData", rb_cObject);
    rb_define_alloc_func(rb_cMatch, match_alloc);
    rb_undef_method(CLASS_OF(rb_cMatch), "new");

    rb_define_method(rb_cMatch, "initialize_copy", match_init_copy, 1);
    rb_define_method(rb_cMatch, "regexp", match_regexp, 0);
    rb_define_method(rb_cMatch, "names", match_names, 0);
    rb_define_method(rb_cMatch, "size", match_size, 0);
    rb_define_method(rb_cMatch, "length", match_size, 0);
    rb_define_method(rb_cMatch, "offset", match_offset, 1);
    rb_define_method(rb_cMatch, "begin", match_begin, 1);
    rb_define_method(rb_cMatch, "end", match_end, 1);
    rb_define_method(rb_cMatch, "to_a", match_to_a, 0);
    rb_define_method(rb_cMatch, "[]", match_aref, -1);
    rb_define_method(rb_cMatch, "captures", match_captures, 0);
    rb_define_method(rb_cMatch, "values_at", match_values_at, -1);
    rb_define_method(rb_cMatch, "pre_match", rb_reg_match_pre, 0);
    rb_define_method(rb_cMatch, "post_match", rb_reg_match_post, 0);
    rb_define_method(rb_cMatch, "to_s", match_to_s, 0);
    rb_define_method(rb_cMatch, "inspect", match_inspect, 0);
    rb_define_method(rb_cMatch, "string", match_string, 0);
    rb_define_method(rb_cMatch, "hash", match_hash, 0);
    rb_define_method(rb_cMatch, "eql?", match_equal, 1);
    rb_define_method(rb_cMatch, "==", match_equal, 1);
}
