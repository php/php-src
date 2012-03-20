/**********************************************************************

  pack.c -

  $Author$
  created at: Thu Feb 10 15:17:05 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

#include "ruby/ruby.h"
#include "ruby/encoding.h"
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>

#define GCC_VERSION_SINCE(major, minor, patchlevel) \
  (defined(__GNUC__) && !defined(__INTEL_COMPILER) && \
   ((__GNUC__ > (major)) ||  \
    (__GNUC__ == (major) && __GNUC_MINOR__ > (minor)) || \
    (__GNUC__ == (major) && __GNUC_MINOR__ == (minor) && __GNUC_PATCHLEVEL__ >= (patchlevel))))
#if SIZEOF_SHORT != 2 || SIZEOF_LONG != 4
# define NATINT_PACK
#endif

#ifdef DYNAMIC_ENDIAN
 /* for universal binary of NEXTSTEP and MacOS X */
 /* useless since autoconf 2.63? */
 static int
 is_bigendian(void)
 {
     static int init = 0;
     static int endian_value;
     char *p;

     if (init) return endian_value;
     init = 1;
     p = (char*)&init;
     return endian_value = p[0]?0:1;
 }
# define BIGENDIAN_P() (is_bigendian())
#elif defined(WORDS_BIGENDIAN)
# define BIGENDIAN_P() 1
#else
# define BIGENDIAN_P() 0
#endif

#ifdef NATINT_PACK
# define NATINT_LEN(type,len) (natint?(int)sizeof(type):(int)(len))
#else
# define NATINT_LEN(type,len) ((int)sizeof(type))
#endif

#if SIZEOF_LONG == 8
# define INT64toNUM(x) LONG2NUM(x)
# define UINT64toNUM(x) ULONG2NUM(x)
#elif defined(HAVE_LONG_LONG) && SIZEOF_LONG_LONG == 8
# define INT64toNUM(x) LL2NUM(x)
# define UINT64toNUM(x) ULL2NUM(x)
#endif

#define define_swapx(x, xtype)		\
static xtype				\
TOKEN_PASTE(swap,x)(xtype z)		\
{					\
    xtype r;				\
    xtype *zp;				\
    unsigned char *s, *t;		\
    int i;				\
					\
    zp = xmalloc(sizeof(xtype));	\
    *zp = z;				\
    s = (unsigned char*)zp;		\
    t = xmalloc(sizeof(xtype));		\
    for (i=0; i<sizeof(xtype); i++) {	\
	t[sizeof(xtype)-i-1] = s[i];	\
    }					\
    r = *(xtype *)t;			\
    xfree(t);				\
    xfree(zp);				\
    return r;				\
}

#if GCC_VERSION_SINCE(4,3,0)
# define swap32(x) __builtin_bswap32(x)
# define swap64(x) __builtin_bswap64(x)
#endif

#ifndef swap16
# define swap16(x)	((((x)&0xFF)<<8) | (((x)>>8)&0xFF))
#endif

#ifndef swap32
# define swap32(x)	((((x)&0xFF)<<24)	\
			|(((x)>>24)&0xFF)	\
			|(((x)&0x0000FF00)<<8)	\
			|(((x)&0x00FF0000)>>8)	)
#endif

#ifndef swap64
# ifdef HAVE_INT64_T
#  define byte_in_64bit(n) ((uint64_t)0xff << (n))
#  define swap64(x)       ((((x)&byte_in_64bit(0))<<56) 	\
			   |(((x)>>56)&0xFF)	                \
			   |(((x)&byte_in_64bit(8))<<40)	\
			   |(((x)&byte_in_64bit(48))>>40)	\
			   |(((x)&byte_in_64bit(16))<<24)	\
			   |(((x)&byte_in_64bit(40))>>24)	\
			   |(((x)&byte_in_64bit(24))<<8)	\
			   |(((x)&byte_in_64bit(32))>>8))
# endif
#endif

#if SIZEOF_SHORT == 2
# define swaps(x)	swap16(x)
#elif SIZEOF_SHORT == 4
# define swaps(x)	swap32(x)
#else
  define_swapx(s,short)
#endif

#if SIZEOF_INT == 2
# define swapi(x)	swap16(x)
#elif SIZEOF_INT == 4
# define swapi(x)	swap32(x)
#else
  define_swapx(i,int)
#endif

#if SIZEOF_LONG == 4
# define swapl(x)	swap32(x)
#elif SIZEOF_LONG == 8
# define swapl(x)        swap64(x)
#else
  define_swapx(l,long)
#endif

#ifdef HAVE_LONG_LONG
# if SIZEOF_LONG_LONG == 8
#  define swapll(x)        swap64(x)
# else
   define_swapx(ll,LONG_LONG)
# endif
#endif

#if SIZEOF_FLOAT == 4 && defined(HAVE_INT32_T)
#   define swapf(x)	swap32(x)
#   define FLOAT_SWAPPER	uint32_t
#else
    define_swapx(f,float)
#endif

#if SIZEOF_DOUBLE == 8 && defined(HAVE_INT64_T)
#   define swapd(x)	swap64(x)
#   define DOUBLE_SWAPPER	uint64_t
#elif SIZEOF_DOUBLE == 8 && defined(HAVE_INT32_T)
    static double
    swapd(const double d)
    {
	double dtmp = d;
	uint32_t utmp[2];
	uint32_t utmp0;

	utmp[0] = 0; utmp[1] = 0;
	memcpy(utmp,&dtmp,sizeof(double));
	utmp0 = utmp[0];
	utmp[0] = swap32(utmp[1]);
	utmp[1] = swap32(utmp0);
	memcpy(&dtmp,utmp,sizeof(double));
	return dtmp;
    }
#else
    define_swapx(d, double)
#endif

#undef define_swapx

#define rb_ntohf(x) (BIGENDIAN_P()?(x):swapf(x))
#define rb_ntohd(x) (BIGENDIAN_P()?(x):swapd(x))
#define rb_htonf(x) (BIGENDIAN_P()?(x):swapf(x))
#define rb_htond(x) (BIGENDIAN_P()?(x):swapd(x))
#define rb_htovf(x) (BIGENDIAN_P()?swapf(x):(x))
#define rb_htovd(x) (BIGENDIAN_P()?swapd(x):(x))
#define rb_vtohf(x) (BIGENDIAN_P()?swapf(x):(x))
#define rb_vtohd(x) (BIGENDIAN_P()?swapd(x):(x))

#ifdef FLOAT_SWAPPER
# define FLOAT_CONVWITH(y)	FLOAT_SWAPPER y;
# define HTONF(x,y)	(memcpy(&(y),&(x),sizeof(float)),	\
			 (y) = rb_htonf((FLOAT_SWAPPER)(y)),	\
			 memcpy(&(x),&(y),sizeof(float)),	\
			 (x))
# define HTOVF(x,y)	(memcpy(&(y),&(x),sizeof(float)),	\
			 (y) = rb_htovf((FLOAT_SWAPPER)(y)),	\
			 memcpy(&(x),&(y),sizeof(float)),	\
			 (x))
# define NTOHF(x,y)	(memcpy(&(y),&(x),sizeof(float)),	\
			 (y) = rb_ntohf((FLOAT_SWAPPER)(y)),	\
			 memcpy(&(x),&(y),sizeof(float)),	\
			 (x))
# define VTOHF(x,y)	(memcpy(&(y),&(x),sizeof(float)),	\
			 (y) = rb_vtohf((FLOAT_SWAPPER)(y)),	\
			 memcpy(&(x),&(y),sizeof(float)),	\
			 (x))
#else
# define FLOAT_CONVWITH(y)
# define HTONF(x,y)	rb_htonf(x)
# define HTOVF(x,y)	rb_htovf(x)
# define NTOHF(x,y)	rb_ntohf(x)
# define VTOHF(x,y)	rb_vtohf(x)
#endif

#ifdef DOUBLE_SWAPPER
# define DOUBLE_CONVWITH(y)	DOUBLE_SWAPPER y;
# define HTOND(x,y)	(memcpy(&(y),&(x),sizeof(double)),	\
			 (y) = rb_htond((DOUBLE_SWAPPER)(y)),	\
			 memcpy(&(x),&(y),sizeof(double)),	\
			 (x))
# define HTOVD(x,y)	(memcpy(&(y),&(x),sizeof(double)),	\
			 (y) = rb_htovd((DOUBLE_SWAPPER)(y)),	\
			 memcpy(&(x),&(y),sizeof(double)),	\
			 (x))
# define NTOHD(x,y)	(memcpy(&(y),&(x),sizeof(double)),	\
			 (y) = rb_ntohd((DOUBLE_SWAPPER)(y)),	\
			 memcpy(&(x),&(y),sizeof(double)),	\
			 (x))
# define VTOHD(x,y)	(memcpy(&(y),&(x),sizeof(double)),	\
			 (y) = rb_vtohd((DOUBLE_SWAPPER)(y)),	\
			 memcpy(&(x),&(y),sizeof(double)),	\
			 (x))
#else
# define DOUBLE_CONVWITH(y)
# define HTOND(x,y)	rb_htond(x)
# define HTOVD(x,y)	rb_htovd(x)
# define NTOHD(x,y)	rb_ntohd(x)
# define VTOHD(x,y)	rb_vtohd(x)
#endif

static unsigned long
num2i32(VALUE x)
{
    x = rb_to_int(x); /* is nil OK? (should not) */

    if (FIXNUM_P(x)) return FIX2LONG(x);
    if (RB_TYPE_P(x, T_BIGNUM)) {
	return rb_big2ulong_pack(x);
    }
    rb_raise(rb_eTypeError, "can't convert %s to `integer'", rb_obj_classname(x));
    return 0;			/* not reached */
}

#define MAX_INTEGER_PACK_SIZE 8
/* #define FORCE_BIG_PACK */

static const char toofew[] = "too few arguments";

static void encodes(VALUE,const char*,long,int,int);
static void qpencode(VALUE,VALUE,long);

static unsigned long utf8_to_uv(const char*,long*);

/*
 *  call-seq:
 *     arr.pack ( aTemplateString ) -> aBinaryString
 *
 *  Packs the contents of <i>arr</i> into a binary sequence according to
 *  the directives in <i>aTemplateString</i> (see the table below)
 *  Directives ``A,'' ``a,'' and ``Z'' may be followed by a count,
 *  which gives the width of the resulting field. The remaining
 *  directives also may take a count, indicating the number of array
 *  elements to convert. If the count is an asterisk
 *  (``<code>*</code>''), all remaining array elements will be
 *  converted. Any of the directives ``<code>sSiIlL</code>'' may be
 *  followed by an underscore (``<code>_</code>'') or
 *  exclamation mark (``<code>!</code>'') to use the underlying
 *  platform's native size for the specified type; otherwise, they use a
 *  platform-independent size. Spaces are ignored in the template
 *  string. See also <code>String#unpack</code>.
 *
 *     a = [ "a", "b", "c" ]
 *     n = [ 65, 66, 67 ]
 *     a.pack("A3A3A3")   #=> "a  b  c  "
 *     a.pack("a3a3a3")   #=> "a\000\000b\000\000c\000\000"
 *     n.pack("ccc")      #=> "ABC"
 *
 *  Directives for +pack+.
 *
 *   Integer      | Array   |
 *   Directive    | Element | Meaning
 *   ---------------------------------------------------------------------------
 *      C         | Integer | 8-bit unsigned (unsigned char)
 *      S         | Integer | 16-bit unsigned, native endian (uint16_t)
 *      L         | Integer | 32-bit unsigned, native endian (uint32_t)
 *      Q         | Integer | 64-bit unsigned, native endian (uint64_t)
 *                |         |
 *      c         | Integer | 8-bit signed (signed char)
 *      s         | Integer | 16-bit signed, native endian (int16_t)
 *      l         | Integer | 32-bit signed, native endian (int32_t)
 *      q         | Integer | 64-bit signed, native endian (int64_t)
 *                |         |
 *      S_, S!    | Integer | unsigned short, native endian
 *      I, I_, I! | Integer | unsigned int, native endian
 *      L_, L!    | Integer | unsigned long, native endian
 *                |         |
 *      s_, s!    | Integer | signed short, native endian
 *      i, i_, i! | Integer | signed int, native endian
 *      l_, l!    | Integer | signed long, native endian
 *                |         |
 *      S> L> Q>  | Integer | same as the directives without ">" except
 *      s> l> q>  |         | big endian
 *      S!> I!>   |         | (available since Ruby 1.9.3)
 *      L!>       |         | "S>" is same as "n"
 *      s!> i!>   |         | "L>" is same as "N"
 *      l!>       |         |
 *                |         |
 *      S< L< Q<  | Integer | same as the directives without "<" except
 *      s< l< q<  |         | little endian
 *      S!< I!<   |         | (available since Ruby 1.9.3)
 *      L!<       |         | "S<" is same as "v"
 *      s!< i!<   |         | "L<" is same as "V"
 *      l!<       |         |
 *                |         |
 *      n         | Integer | 16-bit unsigned, network (big-endian) byte order
 *      N         | Integer | 32-bit unsigned, network (big-endian) byte order
 *      v         | Integer | 16-bit unsigned, VAX (little-endian) byte order
 *      V         | Integer | 32-bit unsigned, VAX (little-endian) byte order
 *                |         |
 *      U         | Integer | UTF-8 character
 *      w         | Integer | BER-compressed integer
 *
 *   Float        |         |
 *   Directive    |         | Meaning
 *   ---------------------------------------------------------------------------
 *      D, d      | Float   | double-precision, native format
 *      F, f      | Float   | single-precision, native format
 *      E         | Float   | double-precision, little-endian byte order
 *      e         | Float   | single-precision, little-endian byte order
 *      G         | Float   | double-precision, network (big-endian) byte order
 *      g         | Float   | single-precision, network (big-endian) byte order
 *
 *   String       |         |
 *   Directive    |         | Meaning
 *   ---------------------------------------------------------------------------
 *      A         | String  | arbitrary binary string (space padded, count is width)
 *      a         | String  | arbitrary binary string (null padded, count is width)
 *      Z         | String  | same as ``a'', except that null is added with *
 *      B         | String  | bit string (MSB first)
 *      b         | String  | bit string (LSB first)
 *      H         | String  | hex string (high nibble first)
 *      h         | String  | hex string (low nibble first)
 *      u         | String  | UU-encoded string
 *      M         | String  | quoted printable, MIME encoding (see RFC2045)
 *      m         | String  | base64 encoded string (see RFC 2045, count is width)
 *                |         | (if count is 0, no line feed are added, see RFC 4648)
 *      P         | String  | pointer to a structure (fixed-length string)
 *      p         | String  | pointer to a null-terminated string
 *
 *   Misc.        |         |
 *   Directive    |         | Meaning
 *   ---------------------------------------------------------------------------
 *      @         | ---     | moves to absolute position
 *      X         | ---     | back up a byte
 *      x         | ---     | null byte
 */

static VALUE
pack_pack(VALUE ary, VALUE fmt)
{
    static const char nul10[] = "\0\0\0\0\0\0\0\0\0\0";
    static const char spc10[] = "          ";
    const char *p, *pend;
    VALUE res, from, associates = 0;
    char type;
    long items, len, idx, plen;
    const char *ptr;
    int enc_info = 1;		/* 0 - BINARY, 1 - US-ASCII, 2 - UTF-8 */
#ifdef NATINT_PACK
    int natint;		/* native integer */
#endif
    int integer_size, bigendian_p;

    StringValue(fmt);
    p = RSTRING_PTR(fmt);
    pend = p + RSTRING_LEN(fmt);
    res = rb_str_buf_new(0);

    items = RARRAY_LEN(ary);
    idx = 0;

#define TOO_FEW (rb_raise(rb_eArgError, toofew), 0)
#define THISFROM (items > 0 ? RARRAY_PTR(ary)[idx] : TOO_FEW)
#define NEXTFROM (items-- > 0 ? RARRAY_PTR(ary)[idx++] : TOO_FEW)

    while (p < pend) {
	int explicit_endian = 0;
	if (RSTRING_PTR(fmt) + RSTRING_LEN(fmt) != pend) {
	    rb_raise(rb_eRuntimeError, "format string modified");
	}
	type = *p++;		/* get data type */
#ifdef NATINT_PACK
	natint = 0;
#endif

	if (ISSPACE(type)) continue;
	if (type == '#') {
	    while ((p < pend) && (*p != '\n')) {
		p++;
	    }
	    continue;
	}

	{
	    static const char natstr[] = "sSiIlL";
	    static const char endstr[] = "sSiIlLqQ";

          modifiers:
	    switch (*p) {
	      case '_':
	      case '!':
		if (strchr(natstr, type)) {
#ifdef NATINT_PACK
		    natint = 1;
#endif
		    p++;
		}
		else {
		    rb_raise(rb_eArgError, "'%c' allowed only after types %s", *p, natstr);
		}
		goto modifiers;

	      case '<':
	      case '>':
		if (!strchr(endstr, type)) {
		    rb_raise(rb_eArgError, "'%c' allowed only after types %s", *p, endstr);
		}
		if (explicit_endian) {
		    rb_raise(rb_eRangeError, "Can't use both '<' and '>'");
		}
		explicit_endian = *p++;
		goto modifiers;
	    }
	}

	if (*p == '*') {	/* set data length */
	    len = strchr("@Xxu", type) ? 0
                : strchr("PMm", type) ? 1
                : items;
	    p++;
	}
	else if (ISDIGIT(*p)) {
	    errno = 0;
	    len = STRTOUL(p, (char**)&p, 10);
	    if (errno) {
		rb_raise(rb_eRangeError, "pack length too big");
	    }
	}
	else {
	    len = 1;
	}

	switch (type) {
	  case 'U':
	    /* if encoding is US-ASCII, upgrade to UTF-8 */
	    if (enc_info == 1) enc_info = 2;
	    break;
	  case 'm': case 'M': case 'u':
	    /* keep US-ASCII (do nothing) */
	    break;
	  default:
	    /* fall back to BINARY */
	    enc_info = 0;
	    break;
	}
	switch (type) {
	  case 'A': case 'a': case 'Z':
	  case 'B': case 'b':
	  case 'H': case 'h':
	    from = NEXTFROM;
	    if (NIL_P(from)) {
		ptr = "";
		plen = 0;
	    }
	    else {
		StringValue(from);
		ptr = RSTRING_PTR(from);
		plen = RSTRING_LEN(from);
		OBJ_INFECT(res, from);
	    }

	    if (p[-1] == '*')
		len = plen;

	    switch (type) {
	      case 'a':		/* arbitrary binary string (null padded)  */
	      case 'A':         /* arbitrary binary string (ASCII space padded) */
	      case 'Z':         /* null terminated string  */
		if (plen >= len) {
		    rb_str_buf_cat(res, ptr, len);
		    if (p[-1] == '*' && type == 'Z')
			rb_str_buf_cat(res, nul10, 1);
		}
		else {
		    rb_str_buf_cat(res, ptr, plen);
		    len -= plen;
		    while (len >= 10) {
			rb_str_buf_cat(res, (type == 'A')?spc10:nul10, 10);
			len -= 10;
		    }
		    rb_str_buf_cat(res, (type == 'A')?spc10:nul10, len);
		}
		break;

	      case 'b':		/* bit string (ascending) */
		{
		    int byte = 0;
		    long i, j = 0;

		    if (len > plen) {
			j = (len - plen + 1)/2;
			len = plen;
		    }
		    for (i=0; i++ < len; ptr++) {
			if (*ptr & 1)
			    byte |= 128;
			if (i & 7)
			    byte >>= 1;
			else {
			    char c = byte & 0xff;
			    rb_str_buf_cat(res, &c, 1);
			    byte = 0;
			}
		    }
		    if (len & 7) {
			char c;
			byte >>= 7 - (len & 7);
			c = byte & 0xff;
			rb_str_buf_cat(res, &c, 1);
		    }
		    len = j;
		    goto grow;
		}
		break;

	      case 'B':		/* bit string (descending) */
		{
		    int byte = 0;
		    long i, j = 0;

		    if (len > plen) {
			j = (len - plen + 1)/2;
			len = plen;
		    }
		    for (i=0; i++ < len; ptr++) {
			byte |= *ptr & 1;
			if (i & 7)
			    byte <<= 1;
			else {
			    char c = byte & 0xff;
			    rb_str_buf_cat(res, &c, 1);
			    byte = 0;
			}
		    }
		    if (len & 7) {
			char c;
			byte <<= 7 - (len & 7);
			c = byte & 0xff;
			rb_str_buf_cat(res, &c, 1);
		    }
		    len = j;
		    goto grow;
		}
		break;

	      case 'h':		/* hex string (low nibble first) */
		{
		    int byte = 0;
		    long i, j = 0;

		    if (len > plen) {
			j = (len + 1) / 2 - (plen + 1) / 2;
			len = plen;
		    }
		    for (i=0; i++ < len; ptr++) {
			if (ISALPHA(*ptr))
			    byte |= (((*ptr & 15) + 9) & 15) << 4;
			else
			    byte |= (*ptr & 15) << 4;
			if (i & 1)
			    byte >>= 4;
			else {
			    char c = byte & 0xff;
			    rb_str_buf_cat(res, &c, 1);
			    byte = 0;
			}
		    }
		    if (len & 1) {
			char c = byte & 0xff;
			rb_str_buf_cat(res, &c, 1);
		    }
		    len = j;
		    goto grow;
		}
		break;

	      case 'H':		/* hex string (high nibble first) */
		{
		    int byte = 0;
		    long i, j = 0;

		    if (len > plen) {
			j = (len + 1) / 2 - (plen + 1) / 2;
			len = plen;
		    }
		    for (i=0; i++ < len; ptr++) {
			if (ISALPHA(*ptr))
			    byte |= ((*ptr & 15) + 9) & 15;
			else
			    byte |= *ptr & 15;
			if (i & 1)
			    byte <<= 4;
			else {
			    char c = byte & 0xff;
			    rb_str_buf_cat(res, &c, 1);
			    byte = 0;
			}
		    }
		    if (len & 1) {
			char c = byte & 0xff;
			rb_str_buf_cat(res, &c, 1);
		    }
		    len = j;
		    goto grow;
		}
		break;
	    }
	    break;

	  case 'c':		/* signed char */
	  case 'C':		/* unsigned char */
	    while (len-- > 0) {
		char c;

		from = NEXTFROM;
		c = (char)num2i32(from);
		rb_str_buf_cat(res, &c, sizeof(char));
	    }
	    break;

	  case 's':		/* signed short */
            integer_size = NATINT_LEN(short, 2);
            bigendian_p = BIGENDIAN_P();
            goto pack_integer;

	  case 'S':		/* unsigned short */
            integer_size = NATINT_LEN(short, 2);
            bigendian_p = BIGENDIAN_P();
            goto pack_integer;

	  case 'i':		/* signed int */
            integer_size = (int)sizeof(int);
            bigendian_p = BIGENDIAN_P();
            goto pack_integer;

	  case 'I':		/* unsigned int */
            integer_size = (int)sizeof(int);
            bigendian_p = BIGENDIAN_P();
            goto pack_integer;

	  case 'l':		/* signed long */
            integer_size = NATINT_LEN(long, 4);
            bigendian_p = BIGENDIAN_P();
            goto pack_integer;

	  case 'L':		/* unsigned long */
            integer_size = NATINT_LEN(long, 4);
            bigendian_p = BIGENDIAN_P();
            goto pack_integer;

	  case 'q':		/* signed quad (64bit) int */
	    integer_size = 8;
            bigendian_p = BIGENDIAN_P();
            goto pack_integer;

	  case 'Q':		/* unsigned quad (64bit) int */
	    integer_size = 8;
            bigendian_p = BIGENDIAN_P();
            goto pack_integer;

	  case 'n':		/* unsigned short (network byte-order)  */
            integer_size = 2;
            bigendian_p = 1;
            goto pack_integer;

	  case 'N':		/* unsigned long (network byte-order) */
            integer_size = 4;
            bigendian_p = 1;
            goto pack_integer;

	  case 'v':		/* unsigned short (VAX byte-order) */
            integer_size = 2;
            bigendian_p = 0;
            goto pack_integer;

	  case 'V':		/* unsigned long (VAX byte-order) */
            integer_size = 4;
            bigendian_p = 0;
            goto pack_integer;

          pack_integer:
	    if (explicit_endian) {
		bigendian_p = explicit_endian == '>';
	    }

            switch (integer_size) {
#if defined(HAVE_INT16_T) && !defined(FORCE_BIG_PACK)
              case SIZEOF_INT16_T:
		while (len-- > 0) {
                    union {
                        int16_t i;
                        char a[sizeof(int16_t)];
                    } v;

		    from = NEXTFROM;
		    v.i = (int16_t)num2i32(from);
		    if (bigendian_p != BIGENDIAN_P()) v.i = swap16(v.i);
		    rb_str_buf_cat(res, v.a, sizeof(int16_t));
		}
		break;
#endif

#if defined(HAVE_INT32_T) && !defined(FORCE_BIG_PACK)
              case SIZEOF_INT32_T:
		while (len-- > 0) {
		    union {
                        int32_t i;
                        char a[sizeof(int32_t)];
                    } v;

		    from = NEXTFROM;
		    v.i = (int32_t)num2i32(from);
		    if (bigendian_p != BIGENDIAN_P()) v.i = swap32(v.i);
		    rb_str_buf_cat(res, v.a, sizeof(int32_t));
		}
		break;
#endif

#if defined(HAVE_INT64_T) && SIZEOF_LONG == SIZEOF_INT64_T && !defined(FORCE_BIG_PACK)
              case SIZEOF_INT64_T:
		while (len-- > 0) {
		    union {
                        int64_t i;
                        char a[sizeof(int64_t)];
                    } v;

		    from = NEXTFROM;
		    v.i = num2i32(from); /* can return 64bit value if SIZEOF_LONG == SIZEOF_INT64_T */
		    if (bigendian_p != BIGENDIAN_P()) v.i = swap64(v.i);
		    rb_str_buf_cat(res, v.a, sizeof(int64_t));
		}
		break;
#endif

	      default:
                if (integer_size > MAX_INTEGER_PACK_SIZE)
                    rb_bug("unexpected intger size for pack: %d", integer_size);
                while (len-- > 0) {
                    union {
                        unsigned long i[(MAX_INTEGER_PACK_SIZE+SIZEOF_LONG-1)/SIZEOF_LONG];
                        char a[(MAX_INTEGER_PACK_SIZE+SIZEOF_LONG-1)/SIZEOF_LONG*SIZEOF_LONG];
                    } v;
                    int num_longs = (integer_size+SIZEOF_LONG-1)/SIZEOF_LONG;
                    int i;

                    from = NEXTFROM;
                    rb_big_pack(from, v.i, num_longs);
                    if (bigendian_p) {
                        for (i = 0; i < num_longs/2; i++) {
                            unsigned long t = v.i[i];
                            v.i[i] = v.i[num_longs-1-i];
                            v.i[num_longs-1-i] = t;
                        }
                    }
		    if (bigendian_p != BIGENDIAN_P()) {
                        for (i = 0; i < num_longs; i++)
                            v.i[i] = swapl(v.i[i]);
                    }
                    rb_str_buf_cat(res,
                                   bigendian_p ?
                                     v.a + sizeof(long)*num_longs - integer_size :
                                     v.a,
                                   integer_size);
                }
                break;
	    }
	    break;

	  case 'f':		/* single precision float in native format */
	  case 'F':		/* ditto */
	    while (len-- > 0) {
		float f;

		from = NEXTFROM;
		f = (float)RFLOAT_VALUE(rb_to_float(from));
		rb_str_buf_cat(res, (char*)&f, sizeof(float));
	    }
	    break;

	  case 'e':		/* single precision float in VAX byte-order */
	    while (len-- > 0) {
		float f;
		FLOAT_CONVWITH(ftmp);

		from = NEXTFROM;
		f = (float)RFLOAT_VALUE(rb_to_float(from));
		f = HTOVF(f,ftmp);
		rb_str_buf_cat(res, (char*)&f, sizeof(float));
	    }
	    break;

	  case 'E':		/* double precision float in VAX byte-order */
	    while (len-- > 0) {
		double d;
		DOUBLE_CONVWITH(dtmp);

		from = NEXTFROM;
		d = RFLOAT_VALUE(rb_to_float(from));
		d = HTOVD(d,dtmp);
		rb_str_buf_cat(res, (char*)&d, sizeof(double));
	    }
	    break;

	  case 'd':		/* double precision float in native format */
	  case 'D':		/* ditto */
	    while (len-- > 0) {
		double d;

		from = NEXTFROM;
		d = RFLOAT_VALUE(rb_to_float(from));
		rb_str_buf_cat(res, (char*)&d, sizeof(double));
	    }
	    break;

	  case 'g':		/* single precision float in network byte-order */
	    while (len-- > 0) {
		float f;
		FLOAT_CONVWITH(ftmp);

		from = NEXTFROM;
		f = (float)RFLOAT_VALUE(rb_to_float(from));
		f = HTONF(f,ftmp);
		rb_str_buf_cat(res, (char*)&f, sizeof(float));
	    }
	    break;

	  case 'G':		/* double precision float in network byte-order */
	    while (len-- > 0) {
		double d;
		DOUBLE_CONVWITH(dtmp);

		from = NEXTFROM;
		d = RFLOAT_VALUE(rb_to_float(from));
		d = HTOND(d,dtmp);
		rb_str_buf_cat(res, (char*)&d, sizeof(double));
	    }
	    break;

	  case 'x':		/* null byte */
	  grow:
	    while (len >= 10) {
		rb_str_buf_cat(res, nul10, 10);
		len -= 10;
	    }
	    rb_str_buf_cat(res, nul10, len);
	    break;

	  case 'X':		/* back up byte */
	  shrink:
	    plen = RSTRING_LEN(res);
	    if (plen < len)
		rb_raise(rb_eArgError, "X outside of string");
	    rb_str_set_len(res, plen - len);
	    break;

	  case '@':		/* null fill to absolute position */
	    len -= RSTRING_LEN(res);
	    if (len > 0) goto grow;
	    len = -len;
	    if (len > 0) goto shrink;
	    break;

	  case '%':
	    rb_raise(rb_eArgError, "%% is not supported");
	    break;

	  case 'U':		/* Unicode character */
	    while (len-- > 0) {
		SIGNED_VALUE l;
		char buf[8];
		int le;

		from = NEXTFROM;
		from = rb_to_int(from);
		l = NUM2LONG(from);
		if (l < 0) {
		    rb_raise(rb_eRangeError, "pack(U): value out of range");
		}
		le = rb_uv_to_utf8(buf, l);
		rb_str_buf_cat(res, (char*)buf, le);
	    }
	    break;

	  case 'u':		/* uuencoded string */
	  case 'm':		/* base64 encoded string */
	    from = NEXTFROM;
	    StringValue(from);
	    ptr = RSTRING_PTR(from);
	    plen = RSTRING_LEN(from);

	    if (len == 0 && type == 'm') {
		encodes(res, ptr, plen, type, 0);
		ptr += plen;
		break;
	    }
	    if (len <= 2)
		len = 45;
	    else
		len = len / 3 * 3;
	    while (plen > 0) {
		long todo;

		if (plen > len)
		    todo = len;
		else
		    todo = plen;
		encodes(res, ptr, todo, type, 1);
		plen -= todo;
		ptr += todo;
	    }
	    break;

	  case 'M':		/* quoted-printable encoded string */
	    from = rb_obj_as_string(NEXTFROM);
	    if (len <= 1)
		len = 72;
	    qpencode(res, from, len);
	    break;

	  case 'P':		/* pointer to packed byte string */
	    from = THISFROM;
	    if (!NIL_P(from)) {
		StringValue(from);
		if (RSTRING_LEN(from) < len) {
		    rb_raise(rb_eArgError, "too short buffer for P(%ld for %ld)",
			     RSTRING_LEN(from), len);
		}
	    }
	    len = 1;
	    /* FALL THROUGH */
	  case 'p':		/* pointer to string */
	    while (len-- > 0) {
		char *t;
		from = NEXTFROM;
		if (NIL_P(from)) {
		    t = 0;
		}
		else {
		    t = StringValuePtr(from);
		}
		if (!associates) {
		    associates = rb_ary_new();
		}
		rb_ary_push(associates, from);
		rb_obj_taint(from);
		rb_str_buf_cat(res, (char*)&t, sizeof(char*));
	    }
	    break;

	  case 'w':		/* BER compressed integer  */
	    while (len-- > 0) {
		unsigned long ul;
		VALUE buf = rb_str_new(0, 0);
		char c, *bufs, *bufe;

		from = NEXTFROM;
		if (RB_TYPE_P(from, T_BIGNUM)) {
		    VALUE big128 = rb_uint2big(128);
		    while (RB_TYPE_P(from, T_BIGNUM)) {
			from = rb_big_divmod(from, big128);
			c = NUM2INT(RARRAY_PTR(from)[1]) | 0x80; /* mod */
			rb_str_buf_cat(buf, &c, sizeof(char));
			from = RARRAY_PTR(from)[0]; /* div */
		    }
		}

		{
		    long l = NUM2LONG(from);
		    if (l < 0) {
			rb_raise(rb_eArgError, "can't compress negative numbers");
		    }
		    ul = l;
		}

		while (ul) {
		    c = (char)(ul & 0x7f) | 0x80;
		    rb_str_buf_cat(buf, &c, sizeof(char));
		    ul >>=  7;
		}

		if (RSTRING_LEN(buf)) {
		    bufs = RSTRING_PTR(buf);
		    bufe = bufs + RSTRING_LEN(buf) - 1;
		    *bufs &= 0x7f; /* clear continue bit */
		    while (bufs < bufe) { /* reverse */
			c = *bufs;
			*bufs++ = *bufe;
			*bufe-- = c;
		    }
		    rb_str_buf_cat(res, RSTRING_PTR(buf), RSTRING_LEN(buf));
		}
		else {
		    c = 0;
		    rb_str_buf_cat(res, &c, sizeof(char));
		}
	    }
	    break;

	  default:
	    break;
	}
    }

    if (associates) {
	rb_str_associate(res, associates);
    }
    OBJ_INFECT(res, fmt);
    switch (enc_info) {
      case 1:
	ENCODING_CODERANGE_SET(res, rb_usascii_encindex(), ENC_CODERANGE_7BIT);
	break;
      case 2:
	rb_enc_set_index(res, rb_utf8_encindex());
	break;
      default:
	/* do nothing, keep ASCII-8BIT */
	break;
    }
    return res;
}

static const char uu_table[] =
"`!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_";
static const char b64_table[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void
encodes(VALUE str, const char *s, long len, int type, int tail_lf)
{
    char buff[4096];
    long i = 0;
    const char *trans = type == 'u' ? uu_table : b64_table;
    int padding;

    if (type == 'u') {
	buff[i++] = (char)len + ' ';
	padding = '`';
    }
    else {
	padding = '=';
    }
    while (len >= 3) {
        while (len >= 3 && sizeof(buff)-i >= 4) {
            buff[i++] = trans[077 & (*s >> 2)];
            buff[i++] = trans[077 & (((*s << 4) & 060) | ((s[1] >> 4) & 017))];
            buff[i++] = trans[077 & (((s[1] << 2) & 074) | ((s[2] >> 6) & 03))];
            buff[i++] = trans[077 & s[2]];
            s += 3;
            len -= 3;
        }
        if (sizeof(buff)-i < 4) {
            rb_str_buf_cat(str, buff, i);
            i = 0;
        }
    }

    if (len == 2) {
	buff[i++] = trans[077 & (*s >> 2)];
	buff[i++] = trans[077 & (((*s << 4) & 060) | ((s[1] >> 4) & 017))];
	buff[i++] = trans[077 & (((s[1] << 2) & 074) | (('\0' >> 6) & 03))];
	buff[i++] = padding;
    }
    else if (len == 1) {
	buff[i++] = trans[077 & (*s >> 2)];
	buff[i++] = trans[077 & (((*s << 4) & 060) | (('\0' >> 4) & 017))];
	buff[i++] = padding;
	buff[i++] = padding;
    }
    if (tail_lf) buff[i++] = '\n';
    rb_str_buf_cat(str, buff, i);
}

static const char hex_table[] = "0123456789ABCDEF";

static void
qpencode(VALUE str, VALUE from, long len)
{
    char buff[1024];
    long i = 0, n = 0, prev = EOF;
    unsigned char *s = (unsigned char*)RSTRING_PTR(from);
    unsigned char *send = s + RSTRING_LEN(from);

    while (s < send) {
        if ((*s > 126) ||
	    (*s < 32 && *s != '\n' && *s != '\t') ||
	    (*s == '=')) {
	    buff[i++] = '=';
	    buff[i++] = hex_table[*s >> 4];
	    buff[i++] = hex_table[*s & 0x0f];
            n += 3;
            prev = EOF;
        }
	else if (*s == '\n') {
            if (prev == ' ' || prev == '\t') {
		buff[i++] = '=';
		buff[i++] = *s;
            }
	    buff[i++] = *s;
            n = 0;
            prev = *s;
        }
	else {
	    buff[i++] = *s;
            n++;
            prev = *s;
        }
        if (n > len) {
	    buff[i++] = '=';
	    buff[i++] = '\n';
            n = 0;
            prev = '\n';
        }
	if (i > 1024 - 5) {
	    rb_str_buf_cat(str, buff, i);
	    i = 0;
	}
	s++;
    }
    if (n > 0) {
	buff[i++] = '=';
	buff[i++] = '\n';
    }
    if (i > 0) {
	rb_str_buf_cat(str, buff, i);
    }
}

static inline int
hex2num(char c)
{
    switch (c) {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        return c - '0';
      case 'a': case 'b': case 'c':
      case 'd': case 'e': case 'f':
	return c - 'a' + 10;
      case 'A': case 'B': case 'C':
      case 'D': case 'E': case 'F':
	return c - 'A' + 10;
      default:
	return -1;
    }
}

#define PACK_LENGTH_ADJUST_SIZE(sz) do {	\
    tmp_len = 0;				\
    if (len > (long)((send-s)/(sz))) {		\
        if (!star) {				\
	    tmp_len = len-(send-s)/(sz);		\
        }					\
	len = (send-s)/(sz);			\
    }						\
} while (0)

#define PACK_ITEM_ADJUST() do { \
    if (tmp_len > 0 && !block_p) \
	rb_ary_store(ary, RARRAY_LEN(ary)+tmp_len-1, Qnil); \
} while (0)

static VALUE
infected_str_new(const char *ptr, long len, VALUE str)
{
    VALUE s = rb_str_new(ptr, len);

    OBJ_INFECT(s, str);
    return s;
}

/*
 *  call-seq:
 *     str.unpack(format)    ->  anArray
 *
 *  Decodes <i>str</i> (which may contain binary data) according to the
 *  format string, returning an array of each value extracted. The
 *  format string consists of a sequence of single-character directives,
 *  summarized in the table at the end of this entry.
 *  Each directive may be followed
 *  by a number, indicating the number of times to repeat with this
 *  directive. An asterisk (``<code>*</code>'') will use up all
 *  remaining elements. The directives <code>sSiIlL</code> may each be
 *  followed by an underscore (``<code>_</code>'') or
 *  exclamation mark (``<code>!</code>'') to use the underlying
 *  platform's native size for the specified type; otherwise, it uses a
 *  platform-independent consistent size. Spaces are ignored in the
 *  format string. See also <code>Array#pack</code>.
 *
 *     "abc \0\0abc \0\0".unpack('A6Z6')   #=> ["abc", "abc "]
 *     "abc \0\0".unpack('a3a3')           #=> ["abc", " \000\000"]
 *     "abc \0abc \0".unpack('Z*Z*')       #=> ["abc ", "abc "]
 *     "aa".unpack('b8B8')                 #=> ["10000110", "01100001"]
 *     "aaa".unpack('h2H2c')               #=> ["16", "61", 97]
 *     "\xfe\xff\xfe\xff".unpack('sS')     #=> [-2, 65534]
 *     "now=20is".unpack('M*')             #=> ["now is"]
 *     "whole".unpack('xax2aX2aX1aX2a')    #=> ["h", "e", "l", "l", "o"]
 *
 *  This table summarizes the various formats and the Ruby classes
 *  returned by each.
 *
 *   Integer      |         |
 *   Directive    | Returns | Meaning
 *   -----------------------------------------------------------------
 *      C         | Integer | 8-bit unsigned (unsigned char)
 *      S         | Integer | 16-bit unsigned, native endian (uint16_t)
 *      L         | Integer | 32-bit unsigned, native endian (uint32_t)
 *      Q         | Integer | 64-bit unsigned, native endian (uint64_t)
 *                |         |
 *      c         | Integer | 8-bit signed (signed char)
 *      s         | Integer | 16-bit signed, native endian (int16_t)
 *      l         | Integer | 32-bit signed, native endian (int32_t)
 *      q         | Integer | 64-bit signed, native endian (int64_t)
 *                |         |
 *      S_, S!    | Integer | unsigned short, native endian
 *      I, I_, I! | Integer | unsigned int, native endian
 *      L_, L!    | Integer | unsigned long, native endian
 *                |         |
 *      s_, s!    | Integer | signed short, native endian
 *      i, i_, i! | Integer | signed int, native endian
 *      l_, l!    | Integer | signed long, native endian
 *                |         |
 *      S> L> Q>  | Integer | same as the directives without ">" except
 *      s> l> q>  |         | big endian
 *      S!> I!>   |         | (available since Ruby 1.9.3)
 *      L!> Q!>   |         | "S>" is same as "n"
 *      s!> i!>   |         | "L>" is same as "N"
 *      l!> q!>   |         |
 *                |         |
 *      S< L< Q<  | Integer | same as the directives without "<" except
 *      s< l< q<  |         | little endian
 *      S!< I!<   |         | (available since Ruby 1.9.3)
 *      L!< Q!<   |         | "S<" is same as "v"
 *      s!< i!<   |         | "L<" is same as "V"
 *      l!< q!<   |         |
 *                |         |
 *      n         | Integer | 16-bit unsigned, network (big-endian) byte order
 *      N         | Integer | 32-bit unsigned, network (big-endian) byte order
 *      v         | Integer | 16-bit unsigned, VAX (little-endian) byte order
 *      V         | Integer | 32-bit unsigned, VAX (little-endian) byte order
 *                |         |
 *      U         | Integer | UTF-8 character
 *      w         | Integer | BER-compressed integer (see Array.pack)
 *
 *   Float        |         |
 *   Directive    | Returns | Meaning
 *   -----------------------------------------------------------------
 *      D, d      | Float   | double-precision, native format
 *      F, f      | Float   | single-precision, native format
 *      E         | Float   | double-precision, little-endian byte order
 *      e         | Float   | single-precision, little-endian byte order
 *      G         | Float   | double-precision, network (big-endian) byte order
 *      g         | Float   | single-precision, network (big-endian) byte order
 *
 *   String       |         |
 *   Directive    | Returns | Meaning
 *   -----------------------------------------------------------------
 *      A         | String  | arbitrary binary string (remove trailing nulls and ASCII spaces)
 *      a         | String  | arbitrary binary string
 *      Z         | String  | null-terminated string
 *      B         | String  | bit string (MSB first)
 *      b         | String  | bit string (LSB first)
 *      H         | String  | hex string (high nibble first)
 *      h         | String  | hex string (low nibble first)
 *      u         | String  | UU-encoded string
 *      M         | String  | quoted-printable, MIME encoding (see RFC2045)
 *      m         | String  | base64 encoded string (RFC 2045) (default)
 *                |         | base64 encoded string (RFC 4648) if followed by 0
 *      P         | String  | pointer to a structure (fixed-length string)
 *      p         | String  | pointer to a null-terminated string
 *
 *   Misc.        |         |
 *   Directive    | Returns | Meaning
 *   -----------------------------------------------------------------
 *      @         | ---     | skip to the offset given by the length argument
 *      X         | ---     | skip backward one byte
 *      x         | ---     | skip forward one byte
 */

static VALUE
pack_unpack(VALUE str, VALUE fmt)
{
    static const char hexdigits[] = "0123456789abcdef";
    char *s, *send;
    char *p, *pend;
    VALUE ary;
    char type;
    long len, tmp_len;
    int star;
#ifdef NATINT_PACK
    int natint;			/* native integer */
#endif
    int block_p = rb_block_given_p();
    int signed_p, integer_size, bigendian_p;
#define UNPACK_PUSH(item) do {\
	VALUE item_val = (item);\
	if (block_p) {\
	    rb_yield(item_val);\
	}\
	else {\
	    rb_ary_push(ary, item_val);\
	}\
    } while (0)

    StringValue(str);
    StringValue(fmt);
    s = RSTRING_PTR(str);
    send = s + RSTRING_LEN(str);
    p = RSTRING_PTR(fmt);
    pend = p + RSTRING_LEN(fmt);

    ary = block_p ? Qnil : rb_ary_new();
    while (p < pend) {
	int explicit_endian = 0;
	type = *p++;
#ifdef NATINT_PACK
	natint = 0;
#endif

	if (ISSPACE(type)) continue;
	if (type == '#') {
	    while ((p < pend) && (*p != '\n')) {
		p++;
	    }
	    continue;
	}

	star = 0;
	{
	    static const char natstr[] = "sSiIlL";
	    static const char endstr[] = "sSiIlLqQ";

          modifiers:
	    switch (*p) {
	      case '_':
	      case '!':

		if (strchr(natstr, type)) {
#ifdef NATINT_PACK
		    natint = 1;
#endif
		    p++;
		}
		else {
		    rb_raise(rb_eArgError, "'%c' allowed only after types %s", *p, natstr);
		}
		goto modifiers;

	      case '<':
	      case '>':
		if (!strchr(endstr, type)) {
		    rb_raise(rb_eArgError, "'%c' allowed only after types %s", *p, endstr);
		}
		if (explicit_endian) {
		    rb_raise(rb_eRangeError, "Can't use both '<' and '>'");
		}
		explicit_endian = *p++;
		goto modifiers;
	    }
	}

	if (p >= pend)
	    len = 1;
	else if (*p == '*') {
	    star = 1;
	    len = send - s;
	    p++;
	}
	else if (ISDIGIT(*p)) {
	    errno = 0;
	    len = STRTOUL(p, (char**)&p, 10);
	    if (errno) {
		rb_raise(rb_eRangeError, "pack length too big");
	    }
	}
	else {
	    len = (type != '@');
	}

	switch (type) {
	  case '%':
	    rb_raise(rb_eArgError, "%% is not supported");
	    break;

	  case 'A':
	    if (len > send - s) len = send - s;
	    {
		long end = len;
		char *t = s + len - 1;

		while (t >= s) {
		    if (*t != ' ' && *t != '\0') break;
		    t--; len--;
		}
		UNPACK_PUSH(infected_str_new(s, len, str));
		s += end;
	    }
	    break;

	  case 'Z':
	    {
		char *t = s;

		if (len > send-s) len = send-s;
		while (t < s+len && *t) t++;
		UNPACK_PUSH(infected_str_new(s, t-s, str));
		if (t < send) t++;
		s = star ? t : s+len;
	    }
	    break;

	  case 'a':
	    if (len > send - s) len = send - s;
	    UNPACK_PUSH(infected_str_new(s, len, str));
	    s += len;
	    break;

	  case 'b':
	    {
		VALUE bitstr;
		char *t;
		int bits;
		long i;

		if (p[-1] == '*' || len > (send - s) * 8)
		    len = (send - s) * 8;
		bits = 0;
		UNPACK_PUSH(bitstr = rb_str_new(0, len));
		t = RSTRING_PTR(bitstr);
		for (i=0; i<len; i++) {
		    if (i & 7) bits >>= 1;
		    else bits = *s++;
		    *t++ = (bits & 1) ? '1' : '0';
		}
	    }
	    break;

	  case 'B':
	    {
		VALUE bitstr;
		char *t;
		int bits;
		long i;

		if (p[-1] == '*' || len > (send - s) * 8)
		    len = (send - s) * 8;
		bits = 0;
		UNPACK_PUSH(bitstr = rb_str_new(0, len));
		t = RSTRING_PTR(bitstr);
		for (i=0; i<len; i++) {
		    if (i & 7) bits <<= 1;
		    else bits = *s++;
		    *t++ = (bits & 128) ? '1' : '0';
		}
	    }
	    break;

	  case 'h':
	    {
		VALUE bitstr;
		char *t;
		int bits;
		long i;

		if (p[-1] == '*' || len > (send - s) * 2)
		    len = (send - s) * 2;
		bits = 0;
		UNPACK_PUSH(bitstr = rb_str_new(0, len));
		t = RSTRING_PTR(bitstr);
		for (i=0; i<len; i++) {
		    if (i & 1)
			bits >>= 4;
		    else
			bits = *s++;
		    *t++ = hexdigits[bits & 15];
		}
	    }
	    break;

	  case 'H':
	    {
		VALUE bitstr;
		char *t;
		int bits;
		long i;

		if (p[-1] == '*' || len > (send - s) * 2)
		    len = (send - s) * 2;
		bits = 0;
		UNPACK_PUSH(bitstr = rb_str_new(0, len));
		t = RSTRING_PTR(bitstr);
		for (i=0; i<len; i++) {
		    if (i & 1)
			bits <<= 4;
		    else
			bits = *s++;
		    *t++ = hexdigits[(bits >> 4) & 15];
		}
	    }
	    break;

	  case 'c':
	    PACK_LENGTH_ADJUST_SIZE(sizeof(char));
	    while (len-- > 0) {
                int c = *s++;
                if (c > (char)127) c-=256;
		UNPACK_PUSH(INT2FIX(c));
	    }
	    PACK_ITEM_ADJUST();
	    break;

	  case 'C':
	    PACK_LENGTH_ADJUST_SIZE(sizeof(unsigned char));
	    while (len-- > 0) {
		unsigned char c = *s++;
		UNPACK_PUSH(INT2FIX(c));
	    }
	    PACK_ITEM_ADJUST();
	    break;

	  case 's':
	    signed_p = 1;
	    integer_size = NATINT_LEN(short, 2);
	    bigendian_p = BIGENDIAN_P();
	    goto unpack_integer;

	  case 'S':
	    signed_p = 0;
	    integer_size = NATINT_LEN(short, 2);
	    bigendian_p = BIGENDIAN_P();
	    goto unpack_integer;

	  case 'i':
	    signed_p = 1;
	    integer_size = (int)sizeof(int);
	    bigendian_p = BIGENDIAN_P();
	    goto unpack_integer;

	  case 'I':
	    signed_p = 0;
	    integer_size = (int)sizeof(int);
	    bigendian_p = BIGENDIAN_P();
	    goto unpack_integer;

	  case 'l':
	    signed_p = 1;
	    integer_size = NATINT_LEN(long, 4);
	    bigendian_p = BIGENDIAN_P();
	    goto unpack_integer;

	  case 'L':
	    signed_p = 0;
	    integer_size = NATINT_LEN(long, 4);
	    bigendian_p = BIGENDIAN_P();
	    goto unpack_integer;

	  case 'q':
	    signed_p = 1;
	    integer_size = 8;
	    bigendian_p = BIGENDIAN_P();
	    goto unpack_integer;

	  case 'Q':
	    signed_p = 0;
	    integer_size = 8;
	    bigendian_p = BIGENDIAN_P();
	    goto unpack_integer;

	  case 'n':
	    signed_p = 0;
	    integer_size = 2;
	    bigendian_p = 1;
	    goto unpack_integer;

	  case 'N':
	    signed_p = 0;
	    integer_size = 4;
	    bigendian_p = 1;
	    goto unpack_integer;

	  case 'v':
	    signed_p = 0;
	    integer_size = 2;
	    bigendian_p = 0;
	    goto unpack_integer;

	  case 'V':
	    signed_p = 0;
	    integer_size = 4;
	    bigendian_p = 0;
	    goto unpack_integer;

	  unpack_integer:
	    if (explicit_endian) {
		bigendian_p = explicit_endian == '>';
	    }

	    switch (integer_size) {
#if defined(HAVE_INT16_T) && !defined(FORCE_BIG_PACK)
	      case SIZEOF_INT16_T:
		if (signed_p) {
		    PACK_LENGTH_ADJUST_SIZE(sizeof(int16_t));
		    while (len-- > 0) {
			union {
                            int16_t i;
                            char a[sizeof(int16_t)];
                        } v;
			memcpy(v.a, s, sizeof(int16_t));
			if (bigendian_p != BIGENDIAN_P()) v.i = swap16(v.i);
			s += sizeof(int16_t);
			UNPACK_PUSH(INT2FIX(v.i));
		    }
		    PACK_ITEM_ADJUST();
		}
		else {
		    PACK_LENGTH_ADJUST_SIZE(sizeof(uint16_t));
		    while (len-- > 0) {
			union {
                            uint16_t i;
                            char a[sizeof(uint16_t)];
                        } v;
			memcpy(v.a, s, sizeof(uint16_t));
			if (bigendian_p != BIGENDIAN_P()) v.i = swap16(v.i);
			s += sizeof(uint16_t);
			UNPACK_PUSH(INT2FIX(v.i));
		    }
		    PACK_ITEM_ADJUST();
		}
		break;
#endif

#if defined(HAVE_INT32_T) && !defined(FORCE_BIG_PACK)
	      case SIZEOF_INT32_T:
		if (signed_p) {
		    PACK_LENGTH_ADJUST_SIZE(sizeof(int32_t));
		    while (len-- > 0) {
			union {
                            int32_t i;
                            char a[sizeof(int32_t)];
                        } v;
			memcpy(v.a, s, sizeof(int32_t));
			if (bigendian_p != BIGENDIAN_P()) v.i = swap32(v.i);
			s += sizeof(int32_t);
			UNPACK_PUSH(INT2NUM(v.i));
		    }
		    PACK_ITEM_ADJUST();
		}
		else {
		    PACK_LENGTH_ADJUST_SIZE(sizeof(uint32_t));
		    while (len-- > 0) {
			union {
                            uint32_t i;
                            char a[sizeof(uint32_t)];
                        } v;
			memcpy(v.a, s, sizeof(uint32_t));
			if (bigendian_p != BIGENDIAN_P()) v.i = swap32(v.i);
			s += sizeof(uint32_t);
			UNPACK_PUSH(UINT2NUM(v.i));
		    }
		    PACK_ITEM_ADJUST();
		}
		break;
#endif

#if defined(HAVE_INT64_T) && !defined(FORCE_BIG_PACK)
	      case SIZEOF_INT64_T:
		if (signed_p) {
		    PACK_LENGTH_ADJUST_SIZE(sizeof(int64_t));
		    while (len-- > 0) {
			union {
                            int64_t i;
                            char a[sizeof(int64_t)];
                        } v;
			memcpy(v.a, s, sizeof(int64_t));
			if (bigendian_p != BIGENDIAN_P()) v.i = swap64(v.i);
			s += sizeof(int64_t);
			UNPACK_PUSH(INT64toNUM(v.i));
		    }
		    PACK_ITEM_ADJUST();
		}
		else {
		    PACK_LENGTH_ADJUST_SIZE(sizeof(uint64_t));
		    while (len-- > 0) {
			union {
                            uint64_t i;
                            char a[sizeof(uint64_t)];
                        } v;
			memcpy(v.a, s, sizeof(uint64_t));
			if (bigendian_p != BIGENDIAN_P()) v.i = swap64(v.i);
			s += sizeof(uint64_t);
			UNPACK_PUSH(UINT64toNUM(v.i));
		    }
		    PACK_ITEM_ADJUST();
		}
		break;
#endif

              default:
                if (integer_size > MAX_INTEGER_PACK_SIZE)
                    rb_bug("unexpected intger size for pack: %d", integer_size);
                PACK_LENGTH_ADJUST_SIZE(integer_size);
                while (len-- > 0) {
                    union {
                        unsigned long i[(MAX_INTEGER_PACK_SIZE+SIZEOF_LONG)/SIZEOF_LONG];
                        char a[(MAX_INTEGER_PACK_SIZE+SIZEOF_LONG)/SIZEOF_LONG*SIZEOF_LONG];
                    } v;
                    int num_longs = (integer_size+SIZEOF_LONG)/SIZEOF_LONG;
                    int i;

                    if (signed_p && (signed char)s[bigendian_p ? 0 : (integer_size-1)] < 0)
                        memset(v.a, 0xff, sizeof(long)*num_longs);
                    else
                        memset(v.a, 0, sizeof(long)*num_longs);
                    if (bigendian_p)
                        memcpy(v.a + sizeof(long)*num_longs - integer_size, s, integer_size);
                    else
                        memcpy(v.a, s, integer_size);
                    if (bigendian_p) {
                        for (i = 0; i < num_longs/2; i++) {
                            unsigned long t = v.i[i];
                            v.i[i] = v.i[num_longs-1-i];
                            v.i[num_longs-1-i] = t;
                        }
                    }
                    if (bigendian_p != BIGENDIAN_P()) {
                        for (i = 0; i < num_longs; i++)
                            v.i[i] = swapl(v.i[i]);
                    }
                    s += integer_size;
                    UNPACK_PUSH(rb_big_unpack(v.i, num_longs));
                }
                PACK_ITEM_ADJUST();
		break;
	    }
            break;

	  case 'f':
	  case 'F':
	    PACK_LENGTH_ADJUST_SIZE(sizeof(float));
	    while (len-- > 0) {
		float tmp;
		memcpy(&tmp, s, sizeof(float));
		s += sizeof(float);
		UNPACK_PUSH(DBL2NUM((double)tmp));
	    }
	    PACK_ITEM_ADJUST();
	    break;

	  case 'e':
	    PACK_LENGTH_ADJUST_SIZE(sizeof(float));
	    while (len-- > 0) {
	        float tmp;
		FLOAT_CONVWITH(ftmp);

		memcpy(&tmp, s, sizeof(float));
		s += sizeof(float);
		tmp = VTOHF(tmp,ftmp);
		UNPACK_PUSH(DBL2NUM((double)tmp));
	    }
	    PACK_ITEM_ADJUST();
	    break;

	  case 'E':
	    PACK_LENGTH_ADJUST_SIZE(sizeof(double));
	    while (len-- > 0) {
		double tmp;
		DOUBLE_CONVWITH(dtmp);

		memcpy(&tmp, s, sizeof(double));
		s += sizeof(double);
		tmp = VTOHD(tmp,dtmp);
		UNPACK_PUSH(DBL2NUM(tmp));
	    }
	    PACK_ITEM_ADJUST();
	    break;

	  case 'D':
	  case 'd':
	    PACK_LENGTH_ADJUST_SIZE(sizeof(double));
	    while (len-- > 0) {
		double tmp;
		memcpy(&tmp, s, sizeof(double));
		s += sizeof(double);
		UNPACK_PUSH(DBL2NUM(tmp));
	    }
	    PACK_ITEM_ADJUST();
	    break;

	  case 'g':
	    PACK_LENGTH_ADJUST_SIZE(sizeof(float));
	    while (len-- > 0) {
	        float tmp;
		FLOAT_CONVWITH(ftmp);

		memcpy(&tmp, s, sizeof(float));
		s += sizeof(float);
		tmp = NTOHF(tmp,ftmp);
		UNPACK_PUSH(DBL2NUM((double)tmp));
	    }
	    PACK_ITEM_ADJUST();
	    break;

	  case 'G':
	    PACK_LENGTH_ADJUST_SIZE(sizeof(double));
	    while (len-- > 0) {
		double tmp;
		DOUBLE_CONVWITH(dtmp);

		memcpy(&tmp, s, sizeof(double));
		s += sizeof(double);
		tmp = NTOHD(tmp,dtmp);
		UNPACK_PUSH(DBL2NUM(tmp));
	    }
	    PACK_ITEM_ADJUST();
	    break;

	  case 'U':
	    if (len > send - s) len = send - s;
	    while (len > 0 && s < send) {
		long alen = send - s;
		unsigned long l;

		l = utf8_to_uv(s, &alen);
		s += alen; len--;
		UNPACK_PUSH(ULONG2NUM(l));
	    }
	    break;

	  case 'u':
	    {
		VALUE buf = infected_str_new(0, (send - s)*3/4, str);
		char *ptr = RSTRING_PTR(buf);
		long total = 0;

		while (s < send && *s > ' ' && *s < 'a') {
		    long a,b,c,d;
		    char hunk[4];

		    hunk[3] = '\0';
		    len = (*s++ - ' ') & 077;
		    total += len;
		    if (total > RSTRING_LEN(buf)) {
			len -= total - RSTRING_LEN(buf);
			total = RSTRING_LEN(buf);
		    }

		    while (len > 0) {
			long mlen = len > 3 ? 3 : len;

			if (s < send && *s >= ' ')
			    a = (*s++ - ' ') & 077;
			else
			    a = 0;
			if (s < send && *s >= ' ')
			    b = (*s++ - ' ') & 077;
			else
			    b = 0;
			if (s < send && *s >= ' ')
			    c = (*s++ - ' ') & 077;
			else
			    c = 0;
			if (s < send && *s >= ' ')
			    d = (*s++ - ' ') & 077;
			else
			    d = 0;
			hunk[0] = (char)(a << 2 | b >> 4);
			hunk[1] = (char)(b << 4 | c >> 2);
			hunk[2] = (char)(c << 6 | d);
			memcpy(ptr, hunk, mlen);
			ptr += mlen;
			len -= mlen;
		    }
		    if (*s == '\r') s++;
		    if (*s == '\n') s++;
		    else if (s < send && (s+1 == send || s[1] == '\n'))
			s += 2;	/* possible checksum byte */
		}

		rb_str_set_len(buf, total);
		UNPACK_PUSH(buf);
	    }
	    break;

	  case 'm':
	    {
		VALUE buf = infected_str_new(0, (send - s)*3/4, str);
		char *ptr = RSTRING_PTR(buf);
		int a = -1,b = -1,c = 0,d = 0;
		static signed char b64_xtable[256];

		if (b64_xtable['/'] <= 0) {
		    int i;

		    for (i = 0; i < 256; i++) {
			b64_xtable[i] = -1;
		    }
		    for (i = 0; i < 64; i++) {
			b64_xtable[(unsigned char)b64_table[i]] = i;
		    }
		}
		if (len == 0) {
		    while (s < send) {
			a = b = c = d = -1;
			a = b64_xtable[(unsigned char)*s++];
			if (s >= send || a == -1) rb_raise(rb_eArgError, "invalid base64");
			b = b64_xtable[(unsigned char)*s++];
			if (s >= send || b == -1) rb_raise(rb_eArgError, "invalid base64");
			if (*s == '=') {
			    if (s + 2 == send && *(s + 1) == '=') break;
			    rb_raise(rb_eArgError, "invalid base64");
			}
			c = b64_xtable[(unsigned char)*s++];
			if (s >= send || c == -1) rb_raise(rb_eArgError, "invalid base64");
			if (s + 1 == send && *s == '=') break;
			d = b64_xtable[(unsigned char)*s++];
			if (d == -1) rb_raise(rb_eArgError, "invalid base64");
			*ptr++ = a << 2 | b >> 4;
			*ptr++ = b << 4 | c >> 2;
			*ptr++ = c << 6 | d;
		    }
		    if (c == -1) {
			*ptr++ = a << 2 | b >> 4;
			if (b & 0xf) rb_raise(rb_eArgError, "invalid base64");
		    }
		    else if (d == -1) {
			*ptr++ = a << 2 | b >> 4;
			*ptr++ = b << 4 | c >> 2;
			if (c & 0x3) rb_raise(rb_eArgError, "invalid base64");
		    }
		}
		else {
		    while (s < send) {
			a = b = c = d = -1;
			while ((a = b64_xtable[(unsigned char)*s]) == -1 && s < send) {s++;}
			if (s >= send) break;
			s++;
			while ((b = b64_xtable[(unsigned char)*s]) == -1 && s < send) {s++;}
			if (s >= send) break;
			s++;
			while ((c = b64_xtable[(unsigned char)*s]) == -1 && s < send) {if (*s == '=') break; s++;}
			if (*s == '=' || s >= send) break;
			s++;
			while ((d = b64_xtable[(unsigned char)*s]) == -1 && s < send) {if (*s == '=') break; s++;}
			if (*s == '=' || s >= send) break;
			s++;
			*ptr++ = a << 2 | b >> 4;
			*ptr++ = b << 4 | c >> 2;
			*ptr++ = c << 6 | d;
		    }
		    if (a != -1 && b != -1) {
			if (c == -1 && *s == '=')
			    *ptr++ = a << 2 | b >> 4;
			else if (c != -1 && *s == '=') {
			    *ptr++ = a << 2 | b >> 4;
			    *ptr++ = b << 4 | c >> 2;
			}
		    }
		}
		rb_str_set_len(buf, ptr - RSTRING_PTR(buf));
		UNPACK_PUSH(buf);
	    }
	    break;

	  case 'M':
	    {
		VALUE buf = infected_str_new(0, send - s, str);
		char *ptr = RSTRING_PTR(buf), *ss = s;
		int c1, c2;

		while (s < send) {
		    if (*s == '=') {
			if (++s == send) break;
                       if (s+1 < send && *s == '\r' && *(s+1) == '\n')
                         s++;
			if (*s != '\n') {
			    if ((c1 = hex2num(*s)) == -1) break;
			    if (++s == send) break;
			    if ((c2 = hex2num(*s)) == -1) break;
			    *ptr++ = c1 << 4 | c2;
			}
		    }
		    else {
			*ptr++ = *s;
		    }
		    s++;
		    ss = s;
		}
		rb_str_set_len(buf, ptr - RSTRING_PTR(buf));
		rb_str_buf_cat(buf, ss, send-ss);
		ENCODING_CODERANGE_SET(buf, rb_ascii8bit_encindex(), ENC_CODERANGE_VALID);
		UNPACK_PUSH(buf);
	    }
	    break;

	  case '@':
	    if (len > RSTRING_LEN(str))
		rb_raise(rb_eArgError, "@ outside of string");
	    s = RSTRING_PTR(str) + len;
	    break;

	  case 'X':
	    if (len > s - RSTRING_PTR(str))
		rb_raise(rb_eArgError, "X outside of string");
	    s -= len;
	    break;

	  case 'x':
	    if (len > send - s)
		rb_raise(rb_eArgError, "x outside of string");
	    s += len;
	    break;

	  case 'P':
	    if (sizeof(char *) <= (size_t)(send - s)) {
		VALUE tmp = Qnil;
		char *t;

		memcpy(&t, s, sizeof(char *));
		s += sizeof(char *);

		if (t) {
		    VALUE a, *p, *pend;

		    if (!(a = rb_str_associated(str))) {
			rb_raise(rb_eArgError, "no associated pointer");
		    }
		    p = RARRAY_PTR(a);
		    pend = p + RARRAY_LEN(a);
		    while (p < pend) {
			if (TYPE(*p) == T_STRING && RSTRING_PTR(*p) == t) {
			    if (len < RSTRING_LEN(*p)) {
				tmp = rb_tainted_str_new(t, len);
				rb_str_associate(tmp, a);
			    }
			    else {
				tmp = *p;
			    }
			    break;
			}
			p++;
		    }
		    if (p == pend) {
			rb_raise(rb_eArgError, "non associated pointer");
		    }
		}
		UNPACK_PUSH(tmp);
	    }
	    break;

	  case 'p':
	    if (len > (long)((send - s) / sizeof(char *)))
		len = (send - s) / sizeof(char *);
	    while (len-- > 0) {
		if ((size_t)(send - s) < sizeof(char *))
		    break;
		else {
		    VALUE tmp = Qnil;
		    char *t;

		    memcpy(&t, s, sizeof(char *));
		    s += sizeof(char *);

		    if (t) {
			VALUE a, *p, *pend;

			if (!(a = rb_str_associated(str))) {
			    rb_raise(rb_eArgError, "no associated pointer");
			}
			p = RARRAY_PTR(a);
			pend = p + RARRAY_LEN(a);
			while (p < pend) {
			    if (TYPE(*p) == T_STRING && RSTRING_PTR(*p) == t) {
				tmp = *p;
				break;
			    }
			    p++;
			}
			if (p == pend) {
			    rb_raise(rb_eArgError, "non associated pointer");
			}
		    }
		    UNPACK_PUSH(tmp);
		}
	    }
	    break;

	  case 'w':
	    {
		unsigned long ul = 0;
		unsigned long ulmask = 0xfeUL << ((sizeof(unsigned long) - 1) * 8);

		while (len > 0 && s < send) {
		    ul <<= 7;
		    ul |= (*s & 0x7f);
		    if (!(*s++ & 0x80)) {
			UNPACK_PUSH(ULONG2NUM(ul));
			len--;
			ul = 0;
		    }
		    else if (ul & ulmask) {
			VALUE big = rb_uint2big(ul);
			VALUE big128 = rb_uint2big(128);
			while (s < send) {
			    big = rb_big_mul(big, big128);
			    big = rb_big_plus(big, rb_uint2big(*s & 0x7f));
			    if (!(*s++ & 0x80)) {
				UNPACK_PUSH(big);
				len--;
				ul = 0;
				break;
			    }
			}
		    }
		}
	    }
	    break;

	  default:
	    break;
	}
    }

    return ary;
}

#define BYTEWIDTH 8

int
rb_uv_to_utf8(char buf[6], unsigned long uv)
{
    if (uv <= 0x7f) {
	buf[0] = (char)uv;
	return 1;
    }
    if (uv <= 0x7ff) {
	buf[0] = (char)((uv>>6)&0xff)|0xc0;
	buf[1] = (char)(uv&0x3f)|0x80;
	return 2;
    }
    if (uv <= 0xffff) {
	buf[0] = (char)((uv>>12)&0xff)|0xe0;
	buf[1] = (char)((uv>>6)&0x3f)|0x80;
	buf[2] = (char)(uv&0x3f)|0x80;
	return 3;
    }
    if (uv <= 0x1fffff) {
	buf[0] = (char)((uv>>18)&0xff)|0xf0;
	buf[1] = (char)((uv>>12)&0x3f)|0x80;
	buf[2] = (char)((uv>>6)&0x3f)|0x80;
	buf[3] = (char)(uv&0x3f)|0x80;
	return 4;
    }
    if (uv <= 0x3ffffff) {
	buf[0] = (char)((uv>>24)&0xff)|0xf8;
	buf[1] = (char)((uv>>18)&0x3f)|0x80;
	buf[2] = (char)((uv>>12)&0x3f)|0x80;
	buf[3] = (char)((uv>>6)&0x3f)|0x80;
	buf[4] = (char)(uv&0x3f)|0x80;
	return 5;
    }
    if (uv <= 0x7fffffff) {
	buf[0] = (char)((uv>>30)&0xff)|0xfc;
	buf[1] = (char)((uv>>24)&0x3f)|0x80;
	buf[2] = (char)((uv>>18)&0x3f)|0x80;
	buf[3] = (char)((uv>>12)&0x3f)|0x80;
	buf[4] = (char)((uv>>6)&0x3f)|0x80;
	buf[5] = (char)(uv&0x3f)|0x80;
	return 6;
    }
    rb_raise(rb_eRangeError, "pack(U): value out of range");
}

static const unsigned long utf8_limits[] = {
    0x0,			/* 1 */
    0x80,			/* 2 */
    0x800,			/* 3 */
    0x10000,			/* 4 */
    0x200000,			/* 5 */
    0x4000000,			/* 6 */
    0x80000000,			/* 7 */
};

static unsigned long
utf8_to_uv(const char *p, long *lenp)
{
    int c = *p++ & 0xff;
    unsigned long uv = c;
    long n;

    if (!(uv & 0x80)) {
	*lenp = 1;
        return uv;
    }
    if (!(uv & 0x40)) {
	*lenp = 1;
	rb_raise(rb_eArgError, "malformed UTF-8 character");
    }

    if      (!(uv & 0x20)) { n = 2; uv &= 0x1f; }
    else if (!(uv & 0x10)) { n = 3; uv &= 0x0f; }
    else if (!(uv & 0x08)) { n = 4; uv &= 0x07; }
    else if (!(uv & 0x04)) { n = 5; uv &= 0x03; }
    else if (!(uv & 0x02)) { n = 6; uv &= 0x01; }
    else {
	*lenp = 1;
	rb_raise(rb_eArgError, "malformed UTF-8 character");
    }
    if (n > *lenp) {
	rb_raise(rb_eArgError, "malformed UTF-8 character (expected %ld bytes, given %ld bytes)",
		 n, *lenp);
    }
    *lenp = n--;
    if (n != 0) {
	while (n--) {
	    c = *p++ & 0xff;
	    if ((c & 0xc0) != 0x80) {
		*lenp -= n + 1;
		rb_raise(rb_eArgError, "malformed UTF-8 character");
	    }
	    else {
		c &= 0x3f;
		uv = uv << 6 | c;
	    }
	}
    }
    n = *lenp - 1;
    if (uv < utf8_limits[n]) {
	rb_raise(rb_eArgError, "redundant UTF-8 sequence");
    }
    return uv;
}

void
Init_pack(void)
{
    rb_define_method(rb_cArray, "pack", pack_pack, 1);
    rb_define_method(rb_cString, "unpack", pack_unpack, 1);
}
