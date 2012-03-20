/**********************************************************************

  marshal.c -

  $Author$
  created at: Thu Apr 27 16:30:01 JST 1995

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

#include "ruby/ruby.h"
#include "ruby/io.h"
#include "ruby/st.h"
#include "ruby/util.h"
#include "ruby/encoding.h"
#include "internal.h"

#include <math.h>
#ifdef HAVE_FLOAT_H
#include <float.h>
#endif
#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

#define BITSPERSHORT (2*CHAR_BIT)
#define SHORTMASK ((1<<BITSPERSHORT)-1)
#define SHORTDN(x) RSHIFT((x),BITSPERSHORT)

#if SIZEOF_SHORT == SIZEOF_BDIGITS
#define SHORTLEN(x) (x)
#else
static long
shortlen(long len, BDIGIT *ds)
{
    BDIGIT num;
    int offset = 0;

    num = ds[len-1];
    while (num) {
	num = SHORTDN(num);
	offset++;
    }
    return (len - 1)*sizeof(BDIGIT)/2 + offset;
}
#define SHORTLEN(x) shortlen((x),d)
#endif

#define MARSHAL_MAJOR   4
#define MARSHAL_MINOR   8

#define TYPE_NIL	'0'
#define TYPE_TRUE	'T'
#define TYPE_FALSE	'F'
#define TYPE_FIXNUM	'i'

#define TYPE_EXTENDED	'e'
#define TYPE_UCLASS	'C'
#define TYPE_OBJECT	'o'
#define TYPE_DATA       'd'
#define TYPE_USERDEF	'u'
#define TYPE_USRMARSHAL	'U'
#define TYPE_FLOAT	'f'
#define TYPE_BIGNUM	'l'
#define TYPE_STRING	'"'
#define TYPE_REGEXP	'/'
#define TYPE_ARRAY	'['
#define TYPE_HASH	'{'
#define TYPE_HASH_DEF	'}'
#define TYPE_STRUCT	'S'
#define TYPE_MODULE_OLD	'M'
#define TYPE_CLASS	'c'
#define TYPE_MODULE	'm'

#define TYPE_SYMBOL	':'
#define TYPE_SYMLINK	';'

#define TYPE_IVAR	'I'
#define TYPE_LINK	'@'

static ID s_dump, s_load, s_mdump, s_mload;
static ID s_dump_data, s_load_data, s_alloc, s_call;
static ID s_getbyte, s_read, s_write, s_binmode;

typedef struct {
    VALUE newclass;
    VALUE oldclass;
    VALUE (*dumper)(VALUE);
    VALUE (*loader)(VALUE, VALUE);
} marshal_compat_t;

static st_table *compat_allocator_tbl;
static VALUE compat_allocator_tbl_wrapper;

static int
mark_marshal_compat_i(st_data_t key, st_data_t value)
{
    marshal_compat_t *p = (marshal_compat_t *)value;
    rb_gc_mark(p->newclass);
    rb_gc_mark(p->oldclass);
    return ST_CONTINUE;
}

static void
mark_marshal_compat_t(void *tbl)
{
    if (!tbl) return;
    st_foreach(tbl, mark_marshal_compat_i, 0);
}

void
rb_marshal_define_compat(VALUE newclass, VALUE oldclass, VALUE (*dumper)(VALUE), VALUE (*loader)(VALUE, VALUE))
{
    marshal_compat_t *compat;
    rb_alloc_func_t allocator = rb_get_alloc_func(newclass);

    if (!allocator) {
        rb_raise(rb_eTypeError, "no allocator");
    }

    compat = ALLOC(marshal_compat_t);
    compat->newclass = Qnil;
    compat->oldclass = Qnil;
    compat->newclass = newclass;
    compat->oldclass = oldclass;
    compat->dumper = dumper;
    compat->loader = loader;

    st_insert(compat_allocator_tbl, (st_data_t)allocator, (st_data_t)compat);
}

#define MARSHAL_INFECTION (FL_TAINT|FL_UNTRUSTED)
typedef char ruby_check_marshal_viral_flags[MARSHAL_INFECTION == (int)MARSHAL_INFECTION ? 1 : -1];

struct dump_arg {
    VALUE str, dest;
    st_table *symbols;
    st_table *data;
    st_table *compat_tbl;
    st_table *encodings;
    int infection;
};

struct dump_call_arg {
    VALUE obj;
    struct dump_arg *arg;
    int limit;
};

static void
check_dump_arg(struct dump_arg *arg, ID sym)
{
    if (!arg->symbols) {
        rb_raise(rb_eRuntimeError, "Marshal.dump reentered at %s",
		 rb_id2name(sym));
    }
}

static void clear_dump_arg(struct dump_arg *arg);

static void
mark_dump_arg(void *ptr)
{
    struct dump_arg *p = ptr;
    if (!p->symbols)
        return;
    rb_mark_set(p->data);
    rb_mark_hash(p->compat_tbl);
    rb_gc_mark(p->str);
}

static void
free_dump_arg(void *ptr)
{
    clear_dump_arg(ptr);
    xfree(ptr);
}

static size_t
memsize_dump_arg(const void *ptr)
{
    return ptr ? sizeof(struct dump_arg) : 0;
}

static const rb_data_type_t dump_arg_data = {
    "dump_arg",
    {mark_dump_arg, free_dump_arg, memsize_dump_arg,},
};

static const char *
must_not_be_anonymous(const char *type, VALUE path)
{
    char *n = RSTRING_PTR(path);

    if (!rb_enc_asciicompat(rb_enc_get(path))) {
	/* cannot occur? */
	rb_raise(rb_eTypeError, "can't dump non-ascii %s name", type);
    }
    if (n[0] == '#') {
	rb_raise(rb_eTypeError, "can't dump anonymous %s %.*s", type,
		 (int)RSTRING_LEN(path), n);
    }
    return n;
}

static VALUE
class2path(VALUE klass)
{
    VALUE path = rb_class_path(klass);
    const char *n;

    n = must_not_be_anonymous((RB_TYPE_P(klass, T_CLASS) ? "class" : "module"), path);
    if (rb_path_to_class(path) != rb_class_real(klass)) {
	rb_raise(rb_eTypeError, "%s can't be referred to", n);
    }
    return path;
}

static void w_long(long, struct dump_arg*);
static void w_encoding(VALUE obj, long num, struct dump_call_arg *arg);

static void
w_nbyte(const char *s, long n, struct dump_arg *arg)
{
    VALUE buf = arg->str;
    rb_str_buf_cat(buf, s, n);
    RBASIC(buf)->flags |= arg->infection;
    if (arg->dest && RSTRING_LEN(buf) >= BUFSIZ) {
	rb_io_write(arg->dest, buf);
	rb_str_resize(buf, 0);
    }
}

static void
w_byte(char c, struct dump_arg *arg)
{
    w_nbyte(&c, 1, arg);
}

static void
w_bytes(const char *s, long n, struct dump_arg *arg)
{
    w_long(n, arg);
    w_nbyte(s, n, arg);
}

#define w_cstr(s, arg) w_bytes((s), strlen(s), (arg))

static void
w_short(int x, struct dump_arg *arg)
{
    w_byte((char)((x >> 0) & 0xff), arg);
    w_byte((char)((x >> 8) & 0xff), arg);
}

static void
w_long(long x, struct dump_arg *arg)
{
    char buf[sizeof(long)+1];
    int i, len = 0;

#if SIZEOF_LONG > 4
    if (!(RSHIFT(x, 31) == 0 || RSHIFT(x, 31) == -1)) {
	/* big long does not fit in 4 bytes */
	rb_raise(rb_eTypeError, "long too big to dump");
    }
#endif

    if (x == 0) {
	w_byte(0, arg);
	return;
    }
    if (0 < x && x < 123) {
	w_byte((char)(x + 5), arg);
	return;
    }
    if (-124 < x && x < 0) {
	w_byte((char)((x - 5)&0xff), arg);
	return;
    }
    for (i=1;i<(int)sizeof(long)+1;i++) {
	buf[i] = (char)(x & 0xff);
	x = RSHIFT(x,8);
	if (x == 0) {
	    buf[0] = i;
	    break;
	}
	if (x == -1) {
	    buf[0] = -i;
	    break;
	}
    }
    len = i;
    for (i=0;i<=len;i++) {
	w_byte(buf[i], arg);
    }
}

#ifdef DBL_MANT_DIG
#define DECIMAL_MANT (53-16)	/* from IEEE754 double precision */

#if DBL_MANT_DIG > 32
#define MANT_BITS 32
#elif DBL_MANT_DIG > 24
#define MANT_BITS 24
#elif DBL_MANT_DIG > 16
#define MANT_BITS 16
#else
#define MANT_BITS 8
#endif

static double
load_mantissa(double d, const char *buf, long len)
{
    if (!len) return d;
    if (--len > 0 && !*buf++) {	/* binary mantissa mark */
	int e, s = d < 0, dig = 0;
	unsigned long m;

	modf(ldexp(frexp(fabs(d), &e), DECIMAL_MANT), &d);
	do {
	    m = 0;
	    switch (len) {
	      default: m = *buf++ & 0xff;
#if MANT_BITS > 24
	      case 3: m = (m << 8) | (*buf++ & 0xff);
#endif
#if MANT_BITS > 16
	      case 2: m = (m << 8) | (*buf++ & 0xff);
#endif
#if MANT_BITS > 8
	      case 1: m = (m << 8) | (*buf++ & 0xff);
#endif
	    }
	    dig -= len < MANT_BITS / 8 ? 8 * (unsigned)len : MANT_BITS;
	    d += ldexp((double)m, dig);
	} while ((len -= MANT_BITS / 8) > 0);
	d = ldexp(d, e - DECIMAL_MANT);
	if (s) d = -d;
    }
    return d;
}
#else
#define load_mantissa(d, buf, len) (d)
#endif

#ifdef DBL_DIG
#define FLOAT_DIG (DBL_DIG+2)
#else
#define FLOAT_DIG 17
#endif

static void
w_float(double d, struct dump_arg *arg)
{
    char *ruby_dtoa(double d_, int mode, int ndigits, int *decpt, int *sign, char **rve);
    char buf[FLOAT_DIG + (DECIMAL_MANT + 7) / 8 + 10];

    if (isinf(d)) {
	if (d < 0) w_cstr("-inf", arg);
	else       w_cstr("inf", arg);
    }
    else if (isnan(d)) {
	w_cstr("nan", arg);
    }
    else if (d == 0.0) {
	if (1.0/d < 0) w_cstr("-0", arg);
	else           w_cstr("0", arg);
    }
    else {
	int decpt, sign, digs, len = 0;
	char *e, *p = ruby_dtoa(d, 0, 0, &decpt, &sign, &e);
	if (sign) buf[len++] = '-';
	digs = (int)(e - p);
	if (decpt < -3 || decpt > digs) {
	    buf[len++] = p[0];
	    if (--digs > 0) buf[len++] = '.';
	    memcpy(buf + len, p + 1, digs);
	    len += digs;
	    len += snprintf(buf + len, sizeof(buf) - len, "e%d", decpt - 1);
	}
	else if (decpt > 0) {
	    memcpy(buf + len, p, decpt);
	    len += decpt;
	    if ((digs -= decpt) > 0) {
		buf[len++] = '.';
		memcpy(buf + len, p + decpt, digs);
		len += digs;
	    }
	}
	else {
	    buf[len++] = '0';
	    buf[len++] = '.';
	    if (decpt) {
		memset(buf + len, '0', -decpt);
		len -= decpt;
	    }
	    memcpy(buf + len, p, digs);
	    len += digs;
	}
	xfree(p);
	w_bytes(buf, len, arg);
    }
}

static void
w_symbol(ID id, struct dump_arg *arg)
{
    VALUE sym;
    st_data_t num;
    int encidx = -1;

    if (st_lookup(arg->symbols, id, &num)) {
	w_byte(TYPE_SYMLINK, arg);
	w_long((long)num, arg);
    }
    else {
	sym = rb_id2str(id);
	if (!sym) {
	    rb_raise(rb_eTypeError, "can't dump anonymous ID %"PRIdVALUE, id);
	}
	encidx = rb_enc_get_index(sym);
	if (encidx == rb_usascii_encindex() ||
	    rb_enc_str_coderange(sym) == ENC_CODERANGE_7BIT) {
	    encidx = -1;
	}
	else {
	    w_byte(TYPE_IVAR, arg);
	}
	w_byte(TYPE_SYMBOL, arg);
	w_bytes(RSTRING_PTR(sym), RSTRING_LEN(sym), arg);
	st_add_direct(arg->symbols, id, arg->symbols->num_entries);
	if (encidx != -1) {
	    struct dump_call_arg c_arg;
	    c_arg.limit = 1;
	    c_arg.arg = arg;
	    w_encoding(sym, 0, &c_arg);
	}
    }
}

static void
w_unique(VALUE s, struct dump_arg *arg)
{
    must_not_be_anonymous("class", s);
    w_symbol(rb_intern_str(s), arg);
}

static void w_object(VALUE,struct dump_arg*,int);

static int
hash_each(VALUE key, VALUE value, struct dump_call_arg *arg)
{
    w_object(key, arg->arg, arg->limit);
    w_object(value, arg->arg, arg->limit);
    return ST_CONTINUE;
}

static void
w_extended(VALUE klass, struct dump_arg *arg, int check)
{
    if (check && FL_TEST(klass, FL_SINGLETON)) {
	if (RCLASS_M_TBL(klass)->num_entries ||
	    (RCLASS_IV_TBL(klass) && RCLASS_IV_TBL(klass)->num_entries > 1)) {
	    rb_raise(rb_eTypeError, "singleton can't be dumped");
	}
	klass = RCLASS_SUPER(klass);
    }
    while (BUILTIN_TYPE(klass) == T_ICLASS) {
	VALUE path = rb_class_name(RBASIC(klass)->klass);
	w_byte(TYPE_EXTENDED, arg);
	w_unique(path, arg);
	klass = RCLASS_SUPER(klass);
    }
}

static void
w_class(char type, VALUE obj, struct dump_arg *arg, int check)
{
    VALUE path;
    st_data_t real_obj;
    VALUE klass;

    if (st_lookup(arg->compat_tbl, (st_data_t)obj, &real_obj)) {
        obj = (VALUE)real_obj;
    }
    klass = CLASS_OF(obj);
    w_extended(klass, arg, check);
    w_byte(type, arg);
    path = class2path(rb_class_real(klass));
    w_unique(path, arg);
}

static void
w_uclass(VALUE obj, VALUE super, struct dump_arg *arg)
{
    VALUE klass = CLASS_OF(obj);

    w_extended(klass, arg, TRUE);
    klass = rb_class_real(klass);
    if (klass != super) {
	w_byte(TYPE_UCLASS, arg);
	w_unique(class2path(klass), arg);
    }
}

static int
w_obj_each(ID id, VALUE value, struct dump_call_arg *arg)
{
    if (id == rb_id_encoding()) return ST_CONTINUE;
    if (id == rb_intern("E")) return ST_CONTINUE;
    w_symbol(id, arg->arg);
    w_object(value, arg->arg, arg->limit);
    return ST_CONTINUE;
}

static void
w_encoding(VALUE obj, long num, struct dump_call_arg *arg)
{
    int encidx = rb_enc_get_index(obj);
    rb_encoding *enc = 0;
    st_data_t name;

    if (encidx <= 0 || !(enc = rb_enc_from_index(encidx))) {
	w_long(num, arg->arg);
	return;
    }
    w_long(num + 1, arg->arg);

    /* special treatment for US-ASCII and UTF-8 */
    if (encidx == rb_usascii_encindex()) {
	w_symbol(rb_intern("E"), arg->arg);
	w_object(Qfalse, arg->arg, arg->limit + 1);
	return;
    }
    else if (encidx == rb_utf8_encindex()) {
	w_symbol(rb_intern("E"), arg->arg);
	w_object(Qtrue, arg->arg, arg->limit + 1);
	return;
    }

    w_symbol(rb_id_encoding(), arg->arg);
    do {
	if (!arg->arg->encodings)
	    arg->arg->encodings = st_init_strcasetable();
	else if (st_lookup(arg->arg->encodings, (st_data_t)rb_enc_name(enc), &name))
	    break;
	name = (st_data_t)rb_str_new2(rb_enc_name(enc));
	st_insert(arg->arg->encodings, (st_data_t)rb_enc_name(enc), name);
    } while (0);
    w_object(name, arg->arg, arg->limit + 1);
}

static void
w_ivar(VALUE obj, st_table *tbl, struct dump_call_arg *arg)
{
    long num = tbl ? tbl->num_entries : 0;

    w_encoding(obj, num, arg);
    if (tbl) {
	st_foreach_safe(tbl, w_obj_each, (st_data_t)arg);
    }
}

static void
w_objivar(VALUE obj, struct dump_call_arg *arg)
{
    VALUE *ptr;
    long i, len, num;

    len = ROBJECT_NUMIV(obj);
    ptr = ROBJECT_IVPTR(obj);
    num = 0;
    for (i = 0; i < len; i++)
        if (ptr[i] != Qundef)
            num += 1;

    w_encoding(obj, num, arg);
    if (num != 0) {
        rb_ivar_foreach(obj, w_obj_each, (st_data_t)arg);
    }
}

static void
w_object(VALUE obj, struct dump_arg *arg, int limit)
{
    struct dump_call_arg c_arg;
    st_table *ivtbl = 0;
    st_data_t num;
    int hasiv = 0;
#define has_ivars(obj, ivtbl) (((ivtbl) = rb_generic_ivar_table(obj)) != 0 || \
			       (!SPECIAL_CONST_P(obj) && !ENCODING_IS_ASCII8BIT(obj)))

    if (limit == 0) {
	rb_raise(rb_eArgError, "exceed depth limit");
    }

    limit--;
    c_arg.limit = limit;
    c_arg.arg = arg;

    if (st_lookup(arg->data, obj, &num)) {
	w_byte(TYPE_LINK, arg);
	w_long((long)num, arg);
	return;
    }

    if (obj == Qnil) {
	w_byte(TYPE_NIL, arg);
    }
    else if (obj == Qtrue) {
	w_byte(TYPE_TRUE, arg);
    }
    else if (obj == Qfalse) {
	w_byte(TYPE_FALSE, arg);
    }
    else if (FIXNUM_P(obj)) {
#if SIZEOF_LONG <= 4
	w_byte(TYPE_FIXNUM, arg);
	w_long(FIX2INT(obj), arg);
#else
	if (RSHIFT((long)obj, 31) == 0 || RSHIFT((long)obj, 31) == -1) {
	    w_byte(TYPE_FIXNUM, arg);
	    w_long(FIX2LONG(obj), arg);
	}
	else {
	    w_object(rb_int2big(FIX2LONG(obj)), arg, limit);
	}
#endif
    }
    else if (SYMBOL_P(obj)) {
	w_symbol(SYM2ID(obj), arg);
    }
    else {
	arg->infection |= (int)FL_TEST(obj, MARSHAL_INFECTION);

	if (rb_respond_to(obj, s_mdump)) {
	    volatile VALUE v;

            st_add_direct(arg->data, obj, arg->data->num_entries);

	    v = rb_funcall(obj, s_mdump, 0, 0);
	    check_dump_arg(arg, s_mdump);
	    hasiv = has_ivars(obj, ivtbl);
	    if (hasiv) w_byte(TYPE_IVAR, arg);
	    w_class(TYPE_USRMARSHAL, obj, arg, FALSE);
	    w_object(v, arg, limit);
	    if (hasiv) w_ivar(obj, ivtbl, &c_arg);
	    return;
	}
	if (rb_respond_to(obj, s_dump)) {
	    VALUE v;
            st_table *ivtbl2 = 0;
            int hasiv2;

	    v = rb_funcall(obj, s_dump, 1, INT2NUM(limit));
	    check_dump_arg(arg, s_dump);
	    if (!RB_TYPE_P(v, T_STRING)) {
		rb_raise(rb_eTypeError, "_dump() must return string");
	    }
	    hasiv = has_ivars(obj, ivtbl);
	    if (hasiv) w_byte(TYPE_IVAR, arg);
	    if ((hasiv2 = has_ivars(v, ivtbl2)) != 0 && !hasiv) {
		w_byte(TYPE_IVAR, arg);
	    }
	    w_class(TYPE_USERDEF, obj, arg, FALSE);
	    w_bytes(RSTRING_PTR(v), RSTRING_LEN(v), arg);
            if (hasiv2) {
		w_ivar(v, ivtbl2, &c_arg);
            }
            else if (hasiv) {
		w_ivar(obj, ivtbl, &c_arg);
	    }
            st_add_direct(arg->data, obj, arg->data->num_entries);
	    return;
	}

        st_add_direct(arg->data, obj, arg->data->num_entries);

	hasiv = has_ivars(obj, ivtbl);
        {
            st_data_t compat_data;
            rb_alloc_func_t allocator = rb_get_alloc_func(RBASIC(obj)->klass);
            if (st_lookup(compat_allocator_tbl,
                          (st_data_t)allocator,
                          &compat_data)) {
                marshal_compat_t *compat = (marshal_compat_t*)compat_data;
                VALUE real_obj = obj;
                obj = compat->dumper(real_obj);
                st_insert(arg->compat_tbl, (st_data_t)obj, (st_data_t)real_obj);
		if (obj != real_obj && !ivtbl) hasiv = 0;
            }
        }
	if (hasiv) w_byte(TYPE_IVAR, arg);

	switch (BUILTIN_TYPE(obj)) {
	  case T_CLASS:
	    if (FL_TEST(obj, FL_SINGLETON)) {
		rb_raise(rb_eTypeError, "singleton class can't be dumped");
	    }
	    w_byte(TYPE_CLASS, arg);
	    {
		volatile VALUE path = class2path(obj);
		w_bytes(RSTRING_PTR(path), RSTRING_LEN(path), arg);
	    }
	    break;

	  case T_MODULE:
	    w_byte(TYPE_MODULE, arg);
	    {
		VALUE path = class2path(obj);
		w_bytes(RSTRING_PTR(path), RSTRING_LEN(path), arg);
	    }
	    break;

	  case T_FLOAT:
	    w_byte(TYPE_FLOAT, arg);
	    w_float(RFLOAT_VALUE(obj), arg);
	    break;

	  case T_BIGNUM:
	    w_byte(TYPE_BIGNUM, arg);
	    {
		char sign = RBIGNUM_SIGN(obj) ? '+' : '-';
		long len = RBIGNUM_LEN(obj);
		BDIGIT *d = RBIGNUM_DIGITS(obj);

		w_byte(sign, arg);
		w_long(SHORTLEN(len), arg); /* w_short? */
		while (len--) {
#if SIZEOF_BDIGITS > SIZEOF_SHORT
		    BDIGIT num = *d;
		    int i;

		    for (i=0; i<SIZEOF_BDIGITS; i+=SIZEOF_SHORT) {
			w_short(num & SHORTMASK, arg);
			num = SHORTDN(num);
			if (len == 0 && num == 0) break;
		    }
#else
		    w_short(*d, arg);
#endif
		    d++;
		}
	    }
	    break;

	  case T_STRING:
	    w_uclass(obj, rb_cString, arg);
	    w_byte(TYPE_STRING, arg);
	    w_bytes(RSTRING_PTR(obj), RSTRING_LEN(obj), arg);
	    break;

	  case T_REGEXP:
            w_uclass(obj, rb_cRegexp, arg);
            w_byte(TYPE_REGEXP, arg);
            {
                int opts = rb_reg_options(obj);
                w_bytes(RREGEXP_SRC_PTR(obj), RREGEXP_SRC_LEN(obj), arg);
                w_byte((char)opts, arg);
            }
	    break;

	  case T_ARRAY:
	    w_uclass(obj, rb_cArray, arg);
	    w_byte(TYPE_ARRAY, arg);
	    {
		long i, len = RARRAY_LEN(obj);

		w_long(len, arg);
		for (i=0; i<RARRAY_LEN(obj); i++) {
		    w_object(RARRAY_PTR(obj)[i], arg, limit);
		    if (len != RARRAY_LEN(obj)) {
			rb_raise(rb_eRuntimeError, "array modified during dump");
		    }
		}
	    }
	    break;

	  case T_HASH:
	    w_uclass(obj, rb_cHash, arg);
	    if (NIL_P(RHASH_IFNONE(obj))) {
		w_byte(TYPE_HASH, arg);
	    }
	    else if (FL_TEST(obj, FL_USER2)) {
		/* FL_USER2 means HASH_PROC_DEFAULT (see hash.c) */
		rb_raise(rb_eTypeError, "can't dump hash with default proc");
	    }
	    else {
		w_byte(TYPE_HASH_DEF, arg);
	    }
	    w_long(RHASH_SIZE(obj), arg);
	    rb_hash_foreach(obj, hash_each, (st_data_t)&c_arg);
	    if (!NIL_P(RHASH_IFNONE(obj))) {
		w_object(RHASH_IFNONE(obj), arg, limit);
	    }
	    break;

	  case T_STRUCT:
	    w_class(TYPE_STRUCT, obj, arg, TRUE);
	    {
		long len = RSTRUCT_LEN(obj);
		VALUE mem;
		long i;

		w_long(len, arg);
		mem = rb_struct_members(obj);
		for (i=0; i<len; i++) {
		    w_symbol(SYM2ID(RARRAY_PTR(mem)[i]), arg);
		    w_object(RSTRUCT_PTR(obj)[i], arg, limit);
		}
	    }
	    break;

	  case T_OBJECT:
	    w_class(TYPE_OBJECT, obj, arg, TRUE);
	    w_objivar(obj, &c_arg);
	    break;

	  case T_DATA:
	    {
		VALUE v;

		if (!rb_respond_to(obj, s_dump_data)) {
		    rb_raise(rb_eTypeError,
			     "no _dump_data is defined for class %s",
			     rb_obj_classname(obj));
		}
		v = rb_funcall(obj, s_dump_data, 0);
		check_dump_arg(arg, s_dump_data);
		w_class(TYPE_DATA, obj, arg, TRUE);
		w_object(v, arg, limit);
	    }
	    break;

	  default:
	    rb_raise(rb_eTypeError, "can't dump %s",
		     rb_obj_classname(obj));
	    break;
	}
    }
    if (hasiv) {
	w_ivar(obj, ivtbl, &c_arg);
    }
}

static void
clear_dump_arg(struct dump_arg *arg)
{
    if (!arg->symbols) return;
    st_free_table(arg->symbols);
    arg->symbols = 0;
    st_free_table(arg->data);
    arg->data = 0;
    st_free_table(arg->compat_tbl);
    arg->compat_tbl = 0;
    if (arg->encodings) {
	st_free_table(arg->encodings);
	arg->encodings = 0;
    }
}

/*
 * call-seq:
 *      dump( obj [, anIO] , limit=-1 ) -> anIO
 *
 * Serializes obj and all descendant objects. If anIO is
 * specified, the serialized data will be written to it, otherwise the
 * data will be returned as a String. If limit is specified, the
 * traversal of subobjects will be limited to that depth. If limit is
 * negative, no checking of depth will be performed.
 *
 *     class Klass
 *       def initialize(str)
 *         @str = str
 *       end
 *       def say_hello
 *         @str
 *       end
 *     end
 *
 * (produces no output)
 *
 *     o = Klass.new("hello\n")
 *     data = Marshal.dump(o)
 *     obj = Marshal.load(data)
 *     obj.say_hello  #=> "hello\n"
 *
 * Marshal can't dump following objects:
 * * anonymous Class/Module.
 * * objects which related to its system (ex: Dir, File::Stat, IO, File, Socket
 *   and so on)
 * * an instance of MatchData, Data, Method, UnboundMethod, Proc, Thread,
 *   ThreadGroup, Continuation
 * * objects which defines singleton methods
 */
static VALUE
marshal_dump(int argc, VALUE *argv)
{
    VALUE obj, port, a1, a2;
    int limit = -1;
    struct dump_arg *arg;
    volatile VALUE wrapper;

    port = Qnil;
    rb_scan_args(argc, argv, "12", &obj, &a1, &a2);
    if (argc == 3) {
	if (!NIL_P(a2)) limit = NUM2INT(a2);
	if (NIL_P(a1)) goto type_error;
	port = a1;
    }
    else if (argc == 2) {
	if (FIXNUM_P(a1)) limit = FIX2INT(a1);
	else if (NIL_P(a1)) goto type_error;
	else port = a1;
    }
    wrapper = TypedData_Make_Struct(rb_cData, struct dump_arg, &dump_arg_data, arg);
    arg->dest = 0;
    arg->symbols = st_init_numtable();
    arg->data    = st_init_numtable();
    arg->infection = 0;
    arg->compat_tbl = st_init_numtable();
    arg->encodings = 0;
    arg->str = rb_str_buf_new(0);
    if (!NIL_P(port)) {
	if (!rb_respond_to(port, s_write)) {
	  type_error:
	    rb_raise(rb_eTypeError, "instance of IO needed");
	}
	arg->dest = port;
	if (rb_respond_to(port, s_binmode)) {
	    rb_funcall2(port, s_binmode, 0, 0);
	    check_dump_arg(arg, s_binmode);
	}
    }
    else {
	port = arg->str;
    }

    w_byte(MARSHAL_MAJOR, arg);
    w_byte(MARSHAL_MINOR, arg);

    w_object(obj, arg, limit);
    if (arg->dest) {
	rb_io_write(arg->dest, arg->str);
	rb_str_resize(arg->str, 0);
    }
    clear_dump_arg(arg);
    RB_GC_GUARD(wrapper);

    return port;
}

struct load_arg {
    VALUE src;
    long offset;
    st_table *symbols;
    st_table *data;
    VALUE proc;
    st_table *compat_tbl;
    int infection;
};

static void
check_load_arg(struct load_arg *arg, ID sym)
{
    if (!arg->symbols) {
        rb_raise(rb_eRuntimeError, "Marshal.load reentered at %s",
		 rb_id2name(sym));
    }
}

static void clear_load_arg(struct load_arg *arg);

static void
mark_load_arg(void *ptr)
{
    struct load_arg *p = ptr;
    if (!p->symbols)
        return;
    rb_mark_tbl(p->data);
    rb_mark_hash(p->compat_tbl);
}

static void
free_load_arg(void *ptr)
{
    clear_load_arg(ptr);
    xfree(ptr);
}

static size_t
memsize_load_arg(const void *ptr)
{
    return ptr ? sizeof(struct load_arg) : 0;
}

static const rb_data_type_t load_arg_data = {
    "load_arg",
    {mark_load_arg, free_load_arg, memsize_load_arg,},
};

#define r_entry(v, arg) r_entry0((v), (arg)->data->num_entries, (arg))
static VALUE r_entry0(VALUE v, st_index_t num, struct load_arg *arg);
static VALUE r_object(struct load_arg *arg);
static ID r_symbol(struct load_arg *arg);
static VALUE path2class(VALUE path);

static st_index_t
r_prepare(struct load_arg *arg)
{
    st_index_t idx = arg->data->num_entries;

    st_insert(arg->data, (st_data_t)idx, (st_data_t)Qundef);
    return idx;
}

static int
r_byte(struct load_arg *arg)
{
    int c;

    if (TYPE(arg->src) == T_STRING) {
	if (RSTRING_LEN(arg->src) > arg->offset) {
	    c = (unsigned char)RSTRING_PTR(arg->src)[arg->offset++];
	}
	else {
	    rb_raise(rb_eArgError, "marshal data too short");
	}
    }
    else {
	VALUE src = arg->src;
	VALUE v = rb_funcall2(src, s_getbyte, 0, 0);
	check_load_arg(arg, s_getbyte);
	if (NIL_P(v)) rb_eof_error();
	c = (unsigned char)NUM2CHR(v);
    }
    return c;
}

static void
long_toobig(int size)
{
    rb_raise(rb_eTypeError, "long too big for this architecture (size "
	     STRINGIZE(SIZEOF_LONG)", given %d)", size);
}

#undef SIGN_EXTEND_CHAR
#if __STDC__
# define SIGN_EXTEND_CHAR(c) ((signed char)(c))
#else  /* not __STDC__ */
/* As in Harbison and Steele.  */
# define SIGN_EXTEND_CHAR(c) ((((unsigned char)(c)) ^ 128) - 128)
#endif

static long
r_long(struct load_arg *arg)
{
    register long x;
    int c = SIGN_EXTEND_CHAR(r_byte(arg));
    long i;

    if (c == 0) return 0;
    if (c > 0) {
	if (4 < c && c < 128) {
	    return c - 5;
	}
	if (c > (int)sizeof(long)) long_toobig(c);
	x = 0;
	for (i=0;i<c;i++) {
	    x |= (long)r_byte(arg) << (8*i);
	}
    }
    else {
	if (-129 < c && c < -4) {
	    return c + 5;
	}
	c = -c;
	if (c > (int)sizeof(long)) long_toobig(c);
	x = -1;
	for (i=0;i<c;i++) {
	    x &= ~((long)0xff << (8*i));
	    x |= (long)r_byte(arg) << (8*i);
	}
    }
    return x;
}

#define r_bytes(arg) r_bytes0(r_long(arg), (arg))

static VALUE
r_bytes0(long len, struct load_arg *arg)
{
    VALUE str;

    if (len == 0) return rb_str_new(0, 0);
    if (TYPE(arg->src) == T_STRING) {
	if (RSTRING_LEN(arg->src) - arg->offset >= len) {
	    str = rb_str_new(RSTRING_PTR(arg->src)+arg->offset, len);
	    arg->offset += len;
	}
	else {
	  too_short:
	    rb_raise(rb_eArgError, "marshal data too short");
	}
    }
    else {
	VALUE src = arg->src;
	VALUE n = LONG2NUM(len);
	str = rb_funcall2(src, s_read, 1, &n);
	check_load_arg(arg, s_read);
	if (NIL_P(str)) goto too_short;
	StringValue(str);
	if (RSTRING_LEN(str) != len) goto too_short;
	arg->infection |= (int)FL_TEST(str, MARSHAL_INFECTION);
    }
    return str;
}

static int
id2encidx(ID id, VALUE val)
{
    if (id == rb_id_encoding()) {
	int idx = rb_enc_find_index(StringValueCStr(val));
	return idx;
    }
    else if (id == rb_intern("E")) {
	if (val == Qfalse) return rb_usascii_encindex();
	else if (val == Qtrue) return rb_utf8_encindex();
	/* bogus ignore */
    }
    return -1;
}

static ID
r_symlink(struct load_arg *arg)
{
    st_data_t id;
    long num = r_long(arg);

    if (st_lookup(arg->symbols, num, &id)) {
	return (ID)id;
    }
    rb_raise(rb_eArgError, "bad symbol");
}

static ID
r_symreal(struct load_arg *arg, int ivar)
{
    volatile VALUE s = r_bytes(arg);
    ID id;
    int idx = -1;
    st_index_t n = arg->symbols->num_entries;

    st_insert(arg->symbols, (st_data_t)n, (st_data_t)0);
    if (ivar) {
	long num = r_long(arg);
	while (num-- > 0) {
	    id = r_symbol(arg);
	    idx = id2encidx(id, r_object(arg));
	}
    }
    if (idx < 0) idx = rb_usascii_encindex();
    rb_enc_associate_index(s, idx);
    id = rb_intern_str(s);
    st_insert(arg->symbols, (st_data_t)n, (st_data_t)id);

    return id;
}

static ID
r_symbol(struct load_arg *arg)
{
    int type, ivar = 0;

  again:
    switch ((type = r_byte(arg))) {
      case TYPE_IVAR:
	ivar = 1;
	goto again;
      case TYPE_SYMBOL:
	return r_symreal(arg, ivar);
      case TYPE_SYMLINK:
	if (ivar) {
	    rb_raise(rb_eArgError, "dump format error (symlink with encoding)");
	}
	return r_symlink(arg);
      default:
	rb_raise(rb_eArgError, "dump format error for symbol(0x%x)", type);
	break;
    }
}

static VALUE
r_unique(struct load_arg *arg)
{
    return rb_id2str(r_symbol(arg));
}

static VALUE
r_string(struct load_arg *arg)
{
    return r_bytes(arg);
}

static VALUE
r_entry0(VALUE v, st_index_t num, struct load_arg *arg)
{
    st_data_t real_obj = (VALUE)Qundef;
    if (st_lookup(arg->compat_tbl, v, &real_obj)) {
        st_insert(arg->data, num, (st_data_t)real_obj);
    }
    else {
        st_insert(arg->data, num, (st_data_t)v);
    }
    if (arg->infection) {
	FL_SET(v, arg->infection);
	if ((VALUE)real_obj != Qundef)
	    FL_SET((VALUE)real_obj, arg->infection);
    }
    return v;
}

static VALUE
r_leave(VALUE v, struct load_arg *arg)
{
    st_data_t data;
    if (st_lookup(arg->compat_tbl, v, &data)) {
        VALUE real_obj = (VALUE)data;
        rb_alloc_func_t allocator = rb_get_alloc_func(CLASS_OF(real_obj));
        st_data_t key = v;
        if (st_lookup(compat_allocator_tbl, (st_data_t)allocator, &data)) {
            marshal_compat_t *compat = (marshal_compat_t*)data;
            compat->loader(real_obj, v);
        }
        st_delete(arg->compat_tbl, &key, 0);
        v = real_obj;
    }
    if (arg->proc) {
	v = rb_funcall(arg->proc, s_call, 1, v);
	check_load_arg(arg, s_call);
    }
    return v;
}

static void
r_ivar(VALUE obj, int *has_encoding, struct load_arg *arg)
{
    long len;

    len = r_long(arg);
    if (len > 0) {
	do {
	    ID id = r_symbol(arg);
	    VALUE val = r_object(arg);
	    int idx = id2encidx(id, val);
	    if (idx >= 0) {
		rb_enc_associate_index(obj, idx);
		if (has_encoding) *has_encoding = TRUE;
	    }
	    else {
		rb_ivar_set(obj, id, val);
	    }
	} while (--len > 0);
    }
}

static VALUE
path2class(VALUE path)
{
    VALUE v = rb_path_to_class(path);

    if (!RB_TYPE_P(v, T_CLASS)) {
	rb_raise(rb_eArgError, "%.*s does not refer to class",
		 (int)RSTRING_LEN(path), RSTRING_PTR(path));
    }
    return v;
}

static VALUE
path2module(VALUE path)
{
    VALUE v = rb_path_to_class(path);

    if (!RB_TYPE_P(v, T_MODULE)) {
	rb_raise(rb_eArgError, "%.*s does not refer to module",
		 (int)RSTRING_LEN(path), RSTRING_PTR(path));
    }
    return v;
}

static VALUE
obj_alloc_by_path(VALUE path, struct load_arg *arg)
{
    VALUE klass;
    st_data_t data;
    rb_alloc_func_t allocator;

    klass = path2class(path);

    allocator = rb_get_alloc_func(klass);
    if (st_lookup(compat_allocator_tbl, (st_data_t)allocator, &data)) {
        marshal_compat_t *compat = (marshal_compat_t*)data;
        VALUE real_obj = rb_obj_alloc(klass);
        VALUE obj = rb_obj_alloc(compat->oldclass);
        st_insert(arg->compat_tbl, (st_data_t)obj, (st_data_t)real_obj);
        return obj;
    }

    return rb_obj_alloc(klass);
}

static VALUE
r_object0(struct load_arg *arg, int *ivp, VALUE extmod)
{
    VALUE v = Qnil;
    int type = r_byte(arg);
    long id;
    st_data_t link;

    switch (type) {
      case TYPE_LINK:
	id = r_long(arg);
	if (!st_lookup(arg->data, (st_data_t)id, &link)) {
	    rb_raise(rb_eArgError, "dump format error (unlinked)");
	}
	v = (VALUE)link;
	if (arg->proc) {
	    v = rb_funcall(arg->proc, s_call, 1, v);
	    check_load_arg(arg, s_call);
	}
	break;

      case TYPE_IVAR:
        {
	    int ivar = TRUE;

	    v = r_object0(arg, &ivar, extmod);
	    if (ivar) r_ivar(v, NULL, arg);
	}
	break;

      case TYPE_EXTENDED:
	{
	    VALUE m = path2module(r_unique(arg));

            if (NIL_P(extmod)) extmod = rb_ary_new2(0);
            rb_ary_push(extmod, m);

	    v = r_object0(arg, 0, extmod);
            while (RARRAY_LEN(extmod) > 0) {
                m = rb_ary_pop(extmod);
                rb_extend_object(v, m);
            }
	}
	break;

      case TYPE_UCLASS:
	{
	    VALUE c = path2class(r_unique(arg));

	    if (FL_TEST(c, FL_SINGLETON)) {
		rb_raise(rb_eTypeError, "singleton can't be loaded");
	    }
	    v = r_object0(arg, 0, extmod);
	    if (rb_special_const_p(v) || RB_TYPE_P(v, T_OBJECT) || RB_TYPE_P(v, T_CLASS)) {
	      format_error:
		rb_raise(rb_eArgError, "dump format error (user class)");
	    }
	    if (RB_TYPE_P(v, T_MODULE) || !RTEST(rb_class_inherited_p(c, RBASIC(v)->klass))) {
		VALUE tmp = rb_obj_alloc(c);

		if (TYPE(v) != TYPE(tmp)) goto format_error;
	    }
	    RBASIC(v)->klass = c;
	}
	break;

      case TYPE_NIL:
	v = Qnil;
	v = r_leave(v, arg);
	break;

      case TYPE_TRUE:
	v = Qtrue;
	v = r_leave(v, arg);
	break;

      case TYPE_FALSE:
	v = Qfalse;
	v = r_leave(v, arg);
	break;

      case TYPE_FIXNUM:
	{
	    long i = r_long(arg);
	    v = LONG2FIX(i);
	}
	v = r_leave(v, arg);
	break;

      case TYPE_FLOAT:
	{
	    double d;
	    VALUE str = r_bytes(arg);
	    const char *ptr = RSTRING_PTR(str);

	    if (strcmp(ptr, "nan") == 0) {
		d = NAN;
	    }
	    else if (strcmp(ptr, "inf") == 0) {
		d = INFINITY;
	    }
	    else if (strcmp(ptr, "-inf") == 0) {
		d = -INFINITY;
	    }
	    else {
		char *e;
		d = strtod(ptr, &e);
		d = load_mantissa(d, e, RSTRING_LEN(str) - (e - ptr));
	    }
	    v = DBL2NUM(d);
	    v = r_entry(v, arg);
            v = r_leave(v, arg);
	}
	break;

      case TYPE_BIGNUM:
	{
	    long len;
	    BDIGIT *digits;
	    volatile VALUE data;

	    NEWOBJ(big, struct RBignum);
	    OBJSETUP(big, rb_cBignum, T_BIGNUM);
	    RBIGNUM_SET_SIGN(big, (r_byte(arg) == '+'));
	    len = r_long(arg);
	    data = r_bytes0(len * 2, arg);
#if SIZEOF_BDIGITS == SIZEOF_SHORT
            rb_big_resize((VALUE)big, len);
#else
            rb_big_resize((VALUE)big, (len + 1) * 2 / sizeof(BDIGIT));
#endif
            digits = RBIGNUM_DIGITS(big);
	    MEMCPY(digits, RSTRING_PTR(data), char, len * 2);
#if SIZEOF_BDIGITS > SIZEOF_SHORT
	    MEMZERO((char *)digits + len * 2, char,
		    RBIGNUM_LEN(big) * sizeof(BDIGIT) - len * 2);
#endif
	    len = RBIGNUM_LEN(big);
	    while (len > 0) {
		unsigned char *p = (unsigned char *)digits;
		BDIGIT num = 0;
#if SIZEOF_BDIGITS > SIZEOF_SHORT
		int shift = 0;
		int i;

		for (i=0; i<SIZEOF_BDIGITS; i++) {
		    num |= (int)p[i] << shift;
		    shift += 8;
		}
#else
		num = p[0] | (p[1] << 8);
#endif
		*digits++ = num;
		len--;
	    }
	    v = rb_big_norm((VALUE)big);
	    v = r_entry(v, arg);
            v = r_leave(v, arg);
	}
	break;

      case TYPE_STRING:
	v = r_entry(r_string(arg), arg);
        v = r_leave(v, arg);
	break;

      case TYPE_REGEXP:
	{
	    volatile VALUE str = r_bytes(arg);
	    int options = r_byte(arg);
	    int has_encoding = FALSE;
	    st_index_t idx = r_prepare(arg);

	    if (ivp) {
		r_ivar(str, &has_encoding, arg);
		*ivp = FALSE;
	    }
	    if (!has_encoding) {
		/* 1.8 compatibility; remove escapes undefined in 1.8 */
		char *ptr = RSTRING_PTR(str), *dst = ptr, *src = ptr;
		long len = RSTRING_LEN(str);
		long bs = 0;
		for (; len-- > 0; *dst++ = *src++) {
		    switch (*src) {
		      case '\\': bs++; break;
		      case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
		      case 'm': case 'o': case 'p': case 'q': case 'u': case 'y':
		      case 'E': case 'F': case 'H': case 'I': case 'J': case 'K':
		      case 'L': case 'N': case 'O': case 'P': case 'Q': case 'R':
		      case 'S': case 'T': case 'U': case 'V': case 'X': case 'Y':
			if (bs & 1) --dst;
		      default: bs = 0; break;
		    }
		}
		rb_str_set_len(str, dst - ptr);
	    }
	    v = r_entry0(rb_reg_new_str(str, options), idx, arg);
	    v = r_leave(v, arg);
	}
	break;

      case TYPE_ARRAY:
	{
	    volatile long len = r_long(arg); /* gcc 2.7.2.3 -O2 bug?? */

	    v = rb_ary_new2(len);
	    v = r_entry(v, arg);
	    while (len--) {
		rb_ary_push(v, r_object(arg));
	    }
            v = r_leave(v, arg);
	}
	break;

      case TYPE_HASH:
      case TYPE_HASH_DEF:
	{
	    long len = r_long(arg);

	    v = rb_hash_new();
	    v = r_entry(v, arg);
	    while (len--) {
		VALUE key = r_object(arg);
		VALUE value = r_object(arg);
		rb_hash_aset(v, key, value);
	    }
	    if (type == TYPE_HASH_DEF) {
		RHASH_IFNONE(v) = r_object(arg);
	    }
            v = r_leave(v, arg);
	}
	break;

      case TYPE_STRUCT:
	{
	    VALUE mem, values;
	    volatile long i;	/* gcc 2.7.2.3 -O2 bug?? */
	    ID slot;
	    st_index_t idx = r_prepare(arg);
	    VALUE klass = path2class(r_unique(arg));
	    long len = r_long(arg);

            v = rb_obj_alloc(klass);
	    if (!RB_TYPE_P(v, T_STRUCT)) {
		rb_raise(rb_eTypeError, "class %s not a struct", rb_class2name(klass));
	    }
	    mem = rb_struct_s_members(klass);
            if (RARRAY_LEN(mem) != len) {
                rb_raise(rb_eTypeError, "struct %s not compatible (struct size differs)",
                         rb_class2name(klass));
            }

	    v = r_entry0(v, idx, arg);
	    values = rb_ary_new2(len);
	    for (i=0; i<len; i++) {
		slot = r_symbol(arg);

		if (RARRAY_PTR(mem)[i] != ID2SYM(slot)) {
		    rb_raise(rb_eTypeError, "struct %s not compatible (:%s for :%s)",
			     rb_class2name(klass),
			     rb_id2name(slot),
			     rb_id2name(SYM2ID(RARRAY_PTR(mem)[i])));
		}
                rb_ary_push(values, r_object(arg));
	    }
            rb_struct_initialize(v, values);
            v = r_leave(v, arg);
	}
	break;

      case TYPE_USERDEF:
        {
	    VALUE klass = path2class(r_unique(arg));
	    VALUE data;

	    if (!rb_respond_to(klass, s_load)) {
		rb_raise(rb_eTypeError, "class %s needs to have method `_load'",
			 rb_class2name(klass));
	    }
	    data = r_string(arg);
	    if (ivp) {
		r_ivar(data, NULL, arg);
		*ivp = FALSE;
	    }
	    v = rb_funcall(klass, s_load, 1, data);
	    check_load_arg(arg, s_load);
	    v = r_entry(v, arg);
            v = r_leave(v, arg);
	}
        break;

      case TYPE_USRMARSHAL:
        {
	    VALUE klass = path2class(r_unique(arg));
	    VALUE data;

	    v = rb_obj_alloc(klass);
            if (!NIL_P(extmod)) {
                while (RARRAY_LEN(extmod) > 0) {
                    VALUE m = rb_ary_pop(extmod);
                    rb_extend_object(v, m);
                }
            }
	    if (!rb_respond_to(v, s_mload)) {
		rb_raise(rb_eTypeError, "instance of %s needs to have method `marshal_load'",
			 rb_class2name(klass));
	    }
	    v = r_entry(v, arg);
	    data = r_object(arg);
	    rb_funcall(v, s_mload, 1, data);
	    check_load_arg(arg, s_mload);
            v = r_leave(v, arg);
	}
        break;

      case TYPE_OBJECT:
	{
	    st_index_t idx = r_prepare(arg);
            v = obj_alloc_by_path(r_unique(arg), arg);
	    if (!RB_TYPE_P(v, T_OBJECT)) {
		rb_raise(rb_eArgError, "dump format error");
	    }
	    v = r_entry0(v, idx, arg);
	    r_ivar(v, NULL, arg);
	    v = r_leave(v, arg);
	}
	break;

      case TYPE_DATA:
       {
           VALUE klass = path2class(r_unique(arg));
           if (rb_respond_to(klass, s_alloc)) {
	       static int warn = TRUE;
	       if (warn) {
		   rb_warn("define `allocate' instead of `_alloc'");
		   warn = FALSE;
	       }
	       v = rb_funcall(klass, s_alloc, 0);
	       check_load_arg(arg, s_alloc);
           }
	   else {
	       v = rb_obj_alloc(klass);
	   }
           if (!RB_TYPE_P(v, T_DATA)) {
               rb_raise(rb_eArgError, "dump format error");
           }
           v = r_entry(v, arg);
           if (!rb_respond_to(v, s_load_data)) {
               rb_raise(rb_eTypeError,
                        "class %s needs to have instance method `_load_data'",
                        rb_class2name(klass));
           }
           rb_funcall(v, s_load_data, 1, r_object0(arg, 0, extmod));
	   check_load_arg(arg, s_load_data);
           v = r_leave(v, arg);
       }
       break;

      case TYPE_MODULE_OLD:
        {
	    volatile VALUE str = r_bytes(arg);

	    v = rb_path_to_class(str);
	    v = r_entry(v, arg);
            v = r_leave(v, arg);
	}
	break;

      case TYPE_CLASS:
        {
	    volatile VALUE str = r_bytes(arg);

	    v = path2class(str);
	    v = r_entry(v, arg);
            v = r_leave(v, arg);
	}
	break;

      case TYPE_MODULE:
        {
	    volatile VALUE str = r_bytes(arg);

	    v = path2module(str);
	    v = r_entry(v, arg);
            v = r_leave(v, arg);
	}
	break;

      case TYPE_SYMBOL:
	if (ivp) {
	    v = ID2SYM(r_symreal(arg, *ivp));
	    *ivp = FALSE;
	}
	else {
	    v = ID2SYM(r_symreal(arg, 0));
	}
	v = r_leave(v, arg);
	break;

      case TYPE_SYMLINK:
	v = ID2SYM(r_symlink(arg));
	break;

      default:
	rb_raise(rb_eArgError, "dump format error(0x%x)", type);
	break;
    }
    return v;
}

static VALUE
r_object(struct load_arg *arg)
{
    return r_object0(arg, 0, Qnil);
}

static void
clear_load_arg(struct load_arg *arg)
{
    if (!arg->symbols) return;
    st_free_table(arg->symbols);
    arg->symbols = 0;
    st_free_table(arg->data);
    arg->data = 0;
    st_free_table(arg->compat_tbl);
    arg->compat_tbl = 0;
}

/*
 * call-seq:
 *     load( source [, proc] ) -> obj
 *     restore( source [, proc] ) -> obj
 *
 * Returns the result of converting the serialized data in source into a
 * Ruby object (possibly with associated subordinate objects). source
 * may be either an instance of IO or an object that responds to
 * to_str. If proc is specified, it will be passed each object as it
 * is deserialized.
 */
static VALUE
marshal_load(int argc, VALUE *argv)
{
    VALUE port, proc;
    int major, minor, infection = 0;
    VALUE v;
    volatile VALUE wrapper;
    struct load_arg *arg;

    rb_scan_args(argc, argv, "11", &port, &proc);
    v = rb_check_string_type(port);
    if (!NIL_P(v)) {
	infection = (int)FL_TEST(port, MARSHAL_INFECTION); /* original taintedness */
	port = v;
    }
    else if (rb_respond_to(port, s_getbyte) && rb_respond_to(port, s_read)) {
	if (rb_respond_to(port, s_binmode)) {
	    rb_funcall2(port, s_binmode, 0, 0);
	}
	infection = (int)(FL_TAINT | FL_TEST(port, FL_UNTRUSTED));
    }
    else {
	rb_raise(rb_eTypeError, "instance of IO needed");
    }
    wrapper = TypedData_Make_Struct(rb_cData, struct load_arg, &load_arg_data, arg);
    arg->infection = infection;
    arg->src = port;
    arg->offset = 0;
    arg->symbols = st_init_numtable();
    arg->data    = st_init_numtable();
    arg->compat_tbl = st_init_numtable();
    arg->proc = 0;

    major = r_byte(arg);
    minor = r_byte(arg);
    if (major != MARSHAL_MAJOR || minor > MARSHAL_MINOR) {
	clear_load_arg(arg);
	rb_raise(rb_eTypeError, "incompatible marshal file format (can't be read)\n\
\tformat version %d.%d required; %d.%d given",
		 MARSHAL_MAJOR, MARSHAL_MINOR, major, minor);
    }
    if (RTEST(ruby_verbose) && minor != MARSHAL_MINOR) {
	rb_warn("incompatible marshal file format (can be read)\n\
\tformat version %d.%d required; %d.%d given",
		MARSHAL_MAJOR, MARSHAL_MINOR, major, minor);
    }

    if (!NIL_P(proc)) arg->proc = proc;
    v = r_object(arg);
    clear_load_arg(arg);
    RB_GC_GUARD(wrapper);

    return v;
}

/*
 * The marshaling library converts collections of Ruby objects into a
 * byte stream, allowing them to be stored outside the currently
 * active script. This data may subsequently be read and the original
 * objects reconstituted.
 *
 * Marshaled data has major and minor version numbers stored along
 * with the object information. In normal use, marshaling can only
 * load data written with the same major version number and an equal
 * or lower minor version number. If Ruby's ``verbose'' flag is set
 * (normally using -d, -v, -w, or --verbose) the major and minor
 * numbers must match exactly. Marshal versioning is independent of
 * Ruby's version numbers. You can extract the version by reading the
 * first two bytes of marshaled data.
 *
 *     str = Marshal.dump("thing")
 *     RUBY_VERSION   #=> "1.9.0"
 *     str[0].ord     #=> 4
 *     str[1].ord     #=> 8
 *
 * Some objects cannot be dumped: if the objects to be dumped include
 * bindings, procedure or method objects, instances of class IO, or
 * singleton objects, a TypeError will be raised.
 *
 * If your class has special serialization needs (for example, if you
 * want to serialize in some specific format), or if it contains
 * objects that would otherwise not be serializable, you can implement
 * your own serialization strategy.
 *
 * There are two methods of doing this, your object can define either
 * marshal_dump and marshal_load or _dump and _load.  marshal_dump will take
 * precedence over _dump if both are defined.  marshal_dump may result in
 * smaller Marshal strings.
 *
 * == marshal_dump and marshal_load
 *
 * When dumping an object the method marshal_dump will be called.
 * marshal_dump must return a result containing the information necessary for
 * marshal_load to reconstitute the object.  The result can be any object.
 *
 * When loading an object dumped using marshal_dump the object is first
 * allocated then marshal_load is called with the result from marshal_dump.
 * marshal_load must recreate the object from the information in the result.
 *
 * Example:
 *
 *   class MyObj
 *     def initialize name, version, data
 *       @name    = name
 *       @version = version
 *       @data    = data
 *     end
 *
 *     def marshal_dump
 *       [@name, @version]
 *     end
 *
 *     def marshal_load array
 *       @name, @version = array
 *     end
 *   end
 *
 * == _dump and _load
 *
 * Use _dump and _load when you need to allocate the object you're restoring
 * yourself.
 *
 * When dumping an object the instance method _dump is called with an Integer
 * which indicates the maximum depth of objects to dump (a value of -1 implies
 * that you should disable depth checking).  _dump must return a String
 * containing the information necessary to reconstitute the object.
 *
 * The class method _load should take a String and use it to return an object
 * of the same class.
 *
 * Example:
 *
 *   class MyObj
 *     def initialize name, version, data
 *       @name    = name
 *       @version = version
 *       @data    = data
 *     end
 *
 *     def _dump level
 *       [@name, @version].join ':'
 *     end
 *
 *     def self._load args
 *       new(*args.split(':'))
 *     end
 *   end
 *
 * Since Marhsal.dump outputs a string you can have _dump return a Marshal
 * string which is Marshal.loaded in _load for complex objects.
 */
void
Init_marshal(void)
{
#undef rb_intern
#define rb_intern(str) rb_intern_const(str)

    VALUE rb_mMarshal = rb_define_module("Marshal");

    s_dump = rb_intern("_dump");
    s_load = rb_intern("_load");
    s_mdump = rb_intern("marshal_dump");
    s_mload = rb_intern("marshal_load");
    s_dump_data = rb_intern("_dump_data");
    s_load_data = rb_intern("_load_data");
    s_alloc = rb_intern("_alloc");
    s_call = rb_intern("call");
    s_getbyte = rb_intern("getbyte");
    s_read = rb_intern("read");
    s_write = rb_intern("write");
    s_binmode = rb_intern("binmode");

    rb_define_module_function(rb_mMarshal, "dump", marshal_dump, -1);
    rb_define_module_function(rb_mMarshal, "load", marshal_load, -1);
    rb_define_module_function(rb_mMarshal, "restore", marshal_load, -1);

    rb_define_const(rb_mMarshal, "MAJOR_VERSION", INT2FIX(MARSHAL_MAJOR));
    rb_define_const(rb_mMarshal, "MINOR_VERSION", INT2FIX(MARSHAL_MINOR));

    compat_allocator_tbl = st_init_numtable();
    compat_allocator_tbl_wrapper =
	Data_Wrap_Struct(rb_cData, mark_marshal_compat_t, 0, compat_allocator_tbl);
    rb_gc_register_mark_object(compat_allocator_tbl_wrapper);
}

VALUE
rb_marshal_dump(VALUE obj, VALUE port)
{
    int argc = 1;
    VALUE argv[2];

    argv[0] = obj;
    argv[1] = port;
    if (!NIL_P(port)) argc = 2;
    return marshal_dump(argc, argv);
}

VALUE
rb_marshal_load(VALUE port)
{
    return marshal_load(1, &port);
}
