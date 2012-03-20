/* -*- mode:c; c-file-style:"ruby" -*- */
/**********************************************************************

  iconv.c -

  $Author$
  created at: Wed Dec  1 20:28:09 JST 1999

  All the files in this distribution are covered under the Ruby's
  license (see the file COPYING).

  Documentation by Yukihiro Matsumoto and Gavin Sinclair.

**********************************************************************/

#include "ruby/ruby.h"
#include <errno.h>
#include <iconv.h>
#include <assert.h>
#include "ruby/st.h"
#include "ruby/encoding.h"

/*
 * Document-class: Iconv
 *
 * == Summary
 *
 * Ruby extension for charset conversion.
 *
 * == Abstract
 *
 * Iconv is a wrapper class for the UNIX 95 <tt>iconv()</tt> function family,
 * which translates string between various encoding systems.
 *
 * See Open Group's on-line documents for more details.
 * * <tt>iconv.h</tt>:       http://www.opengroup.org/onlinepubs/007908799/xsh/iconv.h.html
 * * <tt>iconv_open()</tt>:  http://www.opengroup.org/onlinepubs/007908799/xsh/iconv_open.html
 * * <tt>iconv()</tt>:       http://www.opengroup.org/onlinepubs/007908799/xsh/iconv.html
 * * <tt>iconv_close()</tt>: http://www.opengroup.org/onlinepubs/007908799/xsh/iconv_close.html
 *
 * Which coding systems are available is platform-dependent.
 *
 * == Examples
 *
 * 1. Simple conversion between two charsets.
 *
 *      converted_text = Iconv.conv('iso-8859-15', 'utf-8', text)
 *
 * 2. Instantiate a new Iconv and use method Iconv#iconv.
 *
 *      cd = Iconv.new(to, from)
 *      begin
 *        input.each { |s| output << cd.iconv(s) }
 *        output << cd.iconv(nil)                   # Don't forget this!
 *      ensure
 *        cd.close
 *      end
 *
 * 3. Invoke Iconv.open with a block.
 *
 *      Iconv.open(to, from) do |cd|
 *        input.each { |s| output << cd.iconv(s) }
 *        output << cd.iconv(nil)
 *      end
 *
 * 4. Shorthand for (3).
 *
 *      Iconv.iconv(to, from, *input.to_a)
 *
 * == Attentions
 *
 * Even if some extentions of implementation dependent are useful,
 * DON'T USE those extentions in libraries and scripts to widely distribute.
 * If you want to use those feature, use String#encode.
 */

/* Invalid value for iconv_t is -1 but 0 for VALUE, I hope VALUE is
   big enough to keep iconv_t */
#define VALUE2ICONV(v) ((iconv_t)((VALUE)(v) ^ -1))
#define ICONV2VALUE(c) ((VALUE)(c) ^ -1)

struct iconv_env_t
{
    iconv_t cd;
    int argc;
    VALUE *argv;
    VALUE ret;
    int toidx;
    VALUE (*append)_((VALUE, VALUE));
};

struct rb_iconv_opt_t
{
    VALUE transliterate;
    VALUE discard_ilseq;
};

static ID id_transliterate, id_discard_ilseq;

static VALUE rb_eIconvInvalidEncoding;
static VALUE rb_eIconvFailure;
static VALUE rb_eIconvIllegalSeq;
static VALUE rb_eIconvInvalidChar;
static VALUE rb_eIconvOutOfRange;
static VALUE rb_eIconvBrokenLibrary;

static ID rb_success, rb_failed;
static VALUE iconv_fail _((VALUE error, VALUE success, VALUE failed, struct iconv_env_t* env, VALUE mesg));
static VALUE iconv_fail_retry _((VALUE error, VALUE success, VALUE failed, struct iconv_env_t* env, VALUE mesg));
static VALUE iconv_failure_initialize _((VALUE error, VALUE mesg, VALUE success, VALUE failed));
static VALUE iconv_failure_success _((VALUE self));
static VALUE iconv_failure_failed _((VALUE self));

static iconv_t iconv_create _((VALUE to, VALUE from, struct rb_iconv_opt_t *opt, int *idx));
static void iconv_dfree _((void *cd));
static VALUE iconv_free _((VALUE cd));
static VALUE iconv_try _((iconv_t cd, const char **inptr, size_t *inlen, char **outptr, size_t *outlen));
static VALUE rb_str_derive _((VALUE str, const char* ptr, long len));
static VALUE iconv_convert _((iconv_t cd, VALUE str, long start, long length, int toidx,
			      struct iconv_env_t* env));
static VALUE iconv_s_allocate _((VALUE klass));
static VALUE iconv_initialize _((int argc, VALUE *argv, VALUE self));
static VALUE iconv_s_open _((int argc, VALUE *argv, VALUE self));
static VALUE iconv_s_convert _((struct iconv_env_t* env));
static VALUE iconv_s_iconv _((int argc, VALUE *argv, VALUE self));
static VALUE iconv_init_state _((VALUE cd));
static VALUE iconv_finish _((VALUE self));
static VALUE iconv_iconv _((int argc, VALUE *argv, VALUE self));
static VALUE iconv_conv _((int argc, VALUE *argv, VALUE self));

static VALUE charset_map;

/*
 * Document-method: charset_map
 * call-seq: Iconv.charset_map
 *
 * Returns the map from canonical name to system dependent name.
 */
static VALUE
charset_map_get(void)
{
    return charset_map;
}

static VALUE
strip_glibc_option(VALUE *code)
{
    VALUE val = StringValue(*code);
    const char *ptr = RSTRING_PTR(val), *pend = RSTRING_END(val);
    const char *slash = memchr(ptr, '/', pend - ptr);

    if (slash && slash < pend - 1 && slash[1] ==  '/') {
	VALUE opt = rb_str_subseq(val, slash - ptr, pend - slash);
	val = rb_str_subseq(val, 0, slash - ptr);
	*code = val;
	return opt;
    }
    return 0;
}

static char *
map_charset(VALUE *code)
{
    VALUE val = StringValue(*code);

    if (RHASH_SIZE(charset_map)) {
	st_data_t data;
	VALUE key = rb_funcall2(val, rb_intern("downcase"), 0, 0);
	StringValuePtr(key);
	if (st_lookup(RHASH_TBL(charset_map), key, &data)) {
	    *code = (VALUE)data;
	}
    }
    return StringValuePtr(*code);
}

NORETURN(static void rb_iconv_sys_fail_str(VALUE msg));
static void
rb_iconv_sys_fail_str(VALUE msg)
{
    if (errno == 0) {
	rb_exc_raise(iconv_fail(rb_eIconvBrokenLibrary, Qnil, Qnil, NULL, msg));
    }
    rb_sys_fail_str(msg);
}

#define rb_sys_fail_str(s) rb_iconv_sys_fail_str(s)

NORETURN(static void rb_iconv_sys_fail(const char *s));
static void
rb_iconv_sys_fail(const char *s)
{
    rb_iconv_sys_fail_str(rb_str_new_cstr(s));
}

#define rb_sys_fail(s) rb_iconv_sys_fail(s)

static iconv_t
iconv_create(VALUE to, VALUE from, struct rb_iconv_opt_t *opt, int *idx)
{
    VALUE toopt = strip_glibc_option(&to);
    VALUE fromopt = strip_glibc_option(&from);
    VALUE toenc = 0, fromenc = 0;
    const char* tocode = map_charset(&to);
    const char* fromcode = map_charset(&from);
    iconv_t cd;
    int retry = 0;

    *idx = rb_enc_find_index(tocode);

    if (toopt) {
	toenc = rb_str_plus(to, toopt);
	tocode = RSTRING_PTR(toenc);
    }
    if (fromopt) {
	fromenc = rb_str_plus(from, fromopt);
	fromcode = RSTRING_PTR(fromenc);
    }
    while ((cd = iconv_open(tocode, fromcode)) == (iconv_t)-1) {
	int inval = 0;
	switch (errno) {
	  case EMFILE:
	  case ENFILE:
	  case ENOMEM:
	    if (!retry++) {
		rb_gc();
		continue;
	    }
	    break;
	  case EINVAL:
	    retry = 0;
	    inval = 1;
	    if (toenc) {
		tocode = RSTRING_PTR(to);
		rb_str_resize(toenc, 0);
		toenc = 0;
		continue;
	    }
	    if (fromenc) {
		fromcode = RSTRING_PTR(from);
		rb_str_resize(fromenc, 0);
		fromenc = 0;
		continue;
	    }
	    break;
	}
	{
	    const char *s = inval ? "invalid encoding " : "iconv";
	    VALUE msg = rb_sprintf("%s(\"%s\", \"%s\")",
				   s, RSTRING_PTR(to), RSTRING_PTR(from));
	    if (!inval) rb_sys_fail_str(msg);
	    rb_exc_raise(iconv_fail(rb_eIconvInvalidEncoding, Qnil,
				    rb_ary_new3(2, to, from), NULL, msg));
	}
    }

    if (toopt || fromopt) {
	if (toopt && fromopt && RTEST(rb_str_equal(toopt, fromopt))) {
	    fromopt = 0;
	}
	if (toopt && fromopt) {
	    rb_warning("encoding option isn't portable: %s, %s",
		       RSTRING_PTR(toopt) + 2, RSTRING_PTR(fromopt) + 2);
	}
	else {
	    rb_warning("encoding option isn't portable: %s",
		       (toopt ? RSTRING_PTR(toopt) : RSTRING_PTR(fromopt)) + 2);
	}
    }

    if (opt) {
#ifdef ICONV_SET_TRANSLITERATE
	if (opt->transliterate != Qundef) {
	    int flag = RTEST(opt->transliterate);
	    rb_warning("encoding option isn't portable: transliterate");
	    if (iconvctl(cd, ICONV_SET_TRANSLITERATE, (void *)&flag))
		rb_sys_fail("ICONV_SET_TRANSLITERATE");
	}
#endif
#ifdef ICONV_SET_DISCARD_ILSEQ
	if (opt->discard_ilseq != Qundef) {
	    int flag = RTEST(opt->discard_ilseq);
	    rb_warning("encoding option isn't portable: discard_ilseq");
	    if (iconvctl(cd, ICONV_SET_DISCARD_ILSEQ, (void *)&flag))
		rb_sys_fail("ICONV_SET_DISCARD_ILSEQ");
	}
#endif
    }

    return cd;
}

static void
iconv_dfree(void *cd)
{
    iconv_close(VALUE2ICONV(cd));
}

#define ICONV_FREE iconv_dfree

static VALUE
iconv_free(VALUE cd)
{
    if (cd && iconv_close(VALUE2ICONV(cd)) == -1)
	rb_sys_fail("iconv_close");
    return Qnil;
}

static VALUE
check_iconv(VALUE obj)
{
    Check_Type(obj, T_DATA);
    if (RDATA(obj)->dfree != ICONV_FREE) {
	rb_raise(rb_eArgError, "Iconv expected (%s)", rb_class2name(CLASS_OF(obj)));
    }
    return (VALUE)DATA_PTR(obj);
}

static VALUE
iconv_try(iconv_t cd, const char **inptr, size_t *inlen, char **outptr, size_t *outlen)
{
#ifdef ICONV_INPTR_CONST
#define ICONV_INPTR_CAST
#else
#define ICONV_INPTR_CAST (char **)
#endif
    size_t ret;

    errno = 0;
    ret = iconv(cd, ICONV_INPTR_CAST inptr, inlen, outptr, outlen);
    if (ret == (size_t)-1) {
	if (!*inlen)
	    return Qfalse;
	switch (errno) {
	  case E2BIG:
	    /* try the left in next loop */
	    break;
	  case EILSEQ:
	    return rb_eIconvIllegalSeq;
	  case EINVAL:
	    return rb_eIconvInvalidChar;
	  case 0:
	    return rb_eIconvBrokenLibrary;
	  default:
	    rb_sys_fail("iconv");
	}
    }
    else if (*inlen > 0) {
	/* something goes wrong */
	return rb_eIconvIllegalSeq;
    }
    else if (ret) {
	return Qnil;		/* conversion */
    }
    return Qfalse;
}

#define FAILED_MAXLEN 16

static VALUE
iconv_failure_initialize(VALUE error, VALUE mesg, VALUE success, VALUE failed)
{
    rb_call_super(1, &mesg);
    rb_ivar_set(error, rb_success, success);
    rb_ivar_set(error, rb_failed, failed);
    return error;
}

static VALUE
iconv_fail(VALUE error, VALUE success, VALUE failed, struct iconv_env_t* env, VALUE mesg)
{
    VALUE args[3];

    if (!NIL_P(mesg)) {
	args[0] = mesg;
    }
    else if (TYPE(failed) != T_STRING || RSTRING_LEN(failed) < FAILED_MAXLEN) {
	args[0] = rb_inspect(failed);
    }
    else {
	args[0] = rb_inspect(rb_str_substr(failed, 0, FAILED_MAXLEN));
	rb_str_cat2(args[0], "...");
    }
    args[1] = success;
    args[2] = failed;
    if (env) {
	args[1] = env->append(rb_obj_dup(env->ret), success);
	if (env->argc > 0) {
	    *(env->argv) = failed;
	    args[2] = rb_ary_new4(env->argc, env->argv);
	}
    }
    return rb_class_new_instance(3, args, error);
}

static VALUE
iconv_fail_retry(VALUE error, VALUE success, VALUE failed, struct iconv_env_t* env, VALUE mesg)
{
    error = iconv_fail(error, success, failed, env, mesg);
    if (!rb_block_given_p()) rb_exc_raise(error);
    rb_set_errinfo(error);
    return rb_yield(failed);
}

static VALUE
rb_str_derive(VALUE str, const char* ptr, long len)
{
    VALUE ret;

    if (NIL_P(str))
	return rb_str_new(ptr, len);
    if (RSTRING_PTR(str) + RSTRING_LEN(str) == ptr + len)
	ret = rb_str_subseq(str, ptr - RSTRING_PTR(str), len);
    else
	ret = rb_str_new(ptr, len);
    OBJ_INFECT(ret, str);
    return ret;
}

static VALUE
iconv_convert(iconv_t cd, VALUE str, long start, long length, int toidx, struct iconv_env_t* env)
{
    VALUE ret = Qfalse;
    VALUE error = Qfalse;
    VALUE rescue;
    const char *inptr, *instart;
    size_t inlen;
    /* I believe ONE CHARACTER never exceed this. */
    char buffer[BUFSIZ];
    char *outptr;
    size_t outlen;

    if (cd == (iconv_t)-1)
	rb_raise(rb_eArgError, "closed iconv");

    if (NIL_P(str)) {
	/* Reset output pointer or something. */
	inptr = "";
	inlen = 0;
	outptr = buffer;
	outlen = sizeof(buffer);
	error = iconv_try(cd, &inptr, &inlen, &outptr, &outlen);
	if (RTEST(error)) {
	    unsigned int i;
	    rescue = iconv_fail_retry(error, Qnil, Qnil, env, Qnil);
	    if (TYPE(rescue) == T_ARRAY) {
		str = RARRAY_LEN(rescue) > 0 ? RARRAY_PTR(rescue)[0] : Qnil;
	    }
	    if (FIXNUM_P(str) && (i = FIX2INT(str)) <= 0xff) {
		char c = i;
		str = rb_str_new(&c, 1);
	    }
	    else if (!NIL_P(str)) {
		StringValue(str);
	    }
	}

	inptr = NULL;
	length = 0;
    }
    else {
	long slen;

	StringValue(str);
	slen = RSTRING_LEN(str);
	inptr = RSTRING_PTR(str);

	inptr += start;
	if (length < 0 || length > start + slen)
	    length = slen - start;
    }
    instart = inptr;
    inlen = length;

    do {
	VALUE errmsg = Qnil;
	const char *tmpstart = inptr;
	outptr = buffer;
	outlen = sizeof(buffer);

	error = iconv_try(cd, &inptr, &inlen, &outptr, &outlen);

	if (
#if SIGNEDNESS_OF_SIZE_T < 0
	    0 <= outlen &&
#endif
	    outlen <= sizeof(buffer)) {
	    outlen = sizeof(buffer) - outlen;
	    if (NIL_P(error) ||	/* something converted */
		outlen > (size_t)(inptr - tmpstart) || /* input can't contain output */
		(outlen < (size_t)(inptr - tmpstart) && inlen > 0) || /* something skipped */
		memcmp(buffer, tmpstart, outlen)) /* something differs */
	    {
		if (NIL_P(str)) {
		    ret = rb_str_new(buffer, outlen);
		    if (toidx >= 0) rb_enc_associate_index(ret, toidx);
		}
		else {
		    if (ret) {
			ret = rb_str_buf_cat(ret, instart, tmpstart - instart);
		    }
		    else {
			ret = rb_str_new(instart, tmpstart - instart);
			if (toidx >= 0) rb_enc_associate_index(ret, toidx);
			OBJ_INFECT(ret, str);
		    }
		    ret = rb_str_buf_cat(ret, buffer, outlen);
		    instart = inptr;
		}
	    }
	    else if (!inlen) {
		inptr = tmpstart + outlen;
	    }
	}
	else {
	    /* Some iconv() have a bug, return *outlen out of range */
	    errmsg = rb_sprintf("bug?(output length = %ld)", (long)(sizeof(buffer) - outlen));
	    error = rb_eIconvOutOfRange;
	}

	if (RTEST(error)) {
	    long len = 0;

	    if (!ret) {
		ret = rb_str_derive(str, instart, inptr - instart);
		if (toidx >= 0) rb_enc_associate_index(ret, toidx);
	    }
	    else if (inptr > instart) {
		rb_str_cat(ret, instart, inptr - instart);
	    }
	    str = rb_str_derive(str, inptr, inlen);
	    rescue = iconv_fail_retry(error, ret, str, env, errmsg);
	    if (TYPE(rescue) == T_ARRAY) {
		if ((len = RARRAY_LEN(rescue)) > 0)
		    rb_str_concat(ret, RARRAY_PTR(rescue)[0]);
		if (len > 1 && !NIL_P(str = RARRAY_PTR(rescue)[1])) {
		    StringValue(str);
		    inlen = length = RSTRING_LEN(str);
		    instart = inptr = RSTRING_PTR(str);
		    continue;
		}
	    }
	    else if (!NIL_P(rescue)) {
		rb_str_concat(ret, rescue);
	    }
	    break;
	}
    } while (inlen > 0);

    if (!ret) {
	ret = rb_str_derive(str, instart, inptr - instart);
	if (toidx >= 0) rb_enc_associate_index(ret, toidx);
    }
    else if (inptr > instart) {
	rb_str_cat(ret, instart, inptr - instart);
    }
    return ret;
}

static VALUE
iconv_s_allocate(VALUE klass)
{
    return Data_Wrap_Struct(klass, 0, ICONV_FREE, 0);
}

static VALUE
get_iconv_opt_i(VALUE i, VALUE arg)
{
    VALUE name;
#if defined ICONV_SET_TRANSLITERATE || defined ICONV_SET_DISCARD_ILSEQ
    VALUE val;
    struct rb_iconv_opt_t *opt = (struct rb_iconv_opt_t *)arg;
#endif

    i = rb_Array(i);
    name = rb_ary_entry(i, 0);
#if defined ICONV_SET_TRANSLITERATE || defined ICONV_SET_DISCARD_ILSEQ
    val = rb_ary_entry(i, 1);
#endif
    do {
	if (SYMBOL_P(name)) {
	    ID id = SYM2ID(name);
	    if (id == id_transliterate) {
#ifdef ICONV_SET_TRANSLITERATE
		opt->transliterate = val;
#else
		rb_notimplement();
#endif
		break;
	    }
	    if (id == id_discard_ilseq) {
#ifdef ICONV_SET_DISCARD_ILSEQ
		opt->discard_ilseq = val;
#else
		rb_notimplement();
#endif
		break;
	    }
	}
	else {
	    const char *s = StringValueCStr(name);
	    if (strcmp(s, "transliterate") == 0) {
#ifdef ICONV_SET_TRANSLITERATE
		opt->transliterate = val;
#else
		rb_notimplement();
#endif
		break;
	    }
	    if (strcmp(s, "discard_ilseq") == 0) {
#ifdef ICONV_SET_DISCARD_ILSEQ
		opt->discard_ilseq = val;
#else
		rb_notimplement();
#endif
		break;
	    }
	}
	name = rb_inspect(name);
	rb_raise(rb_eArgError, "unknown option - %s", StringValueCStr(name));
    } while (0);
    return Qnil;
}

static void
get_iconv_opt(struct rb_iconv_opt_t *opt, VALUE options)
{
    opt->transliterate = Qundef;
    opt->discard_ilseq = Qundef;
    if (!NIL_P(options)) {
	rb_block_call(options, rb_intern("each"), 0, 0, get_iconv_opt_i, (VALUE)opt);
    }
}

#define iconv_ctl(self, func, val) (\
	iconvctl(VALUE2ICONV(check_iconv(self)), func, (void *)&(val)) ? \
	rb_sys_fail(#func) : (void)0)

/*
 * Document-method: new
 * call-seq: Iconv.new(to, from, [options])
 *
 * Creates new code converter from a coding-system designated with +from+
 * to another one designated with +to+.
 *
 * === Parameters
 *
 * +to+::   encoding name for destination
 * +from+:: encoding name for source
 * +options+:: options for converter
 *
 * === Exceptions
 *
 * TypeError::       if +to+ or +from+ aren't String
 * InvalidEncoding:: if designated converter couldn't find out
 * SystemCallError:: if <tt>iconv_open(3)</tt> fails
 */
static VALUE
iconv_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE to, from, options;
    struct rb_iconv_opt_t opt;
    int idx;

    rb_scan_args(argc, argv, "21", &to, &from, &options);
    get_iconv_opt(&opt, options);
    iconv_free(check_iconv(self));
    DATA_PTR(self) = NULL;
    DATA_PTR(self) = (void *)ICONV2VALUE(iconv_create(to, from, &opt, &idx));
    if (idx >= 0) ENCODING_SET(self, idx);
    return self;
}

/*
 * Document-method: open
 * call-seq: Iconv.open(to, from) { |iconv| ... }
 *
 * Equivalent to Iconv.new except that when it is called with a block, it
 * yields with the new instance and closes it, and returns the result which
 * returned from the block.
 */
static VALUE
iconv_s_open(int argc, VALUE *argv, VALUE self)
{
    VALUE to, from, options, cd;
    struct rb_iconv_opt_t opt;
    int idx;

    rb_scan_args(argc, argv, "21", &to, &from, &options);
    get_iconv_opt(&opt, options);
    cd = ICONV2VALUE(iconv_create(to, from, &opt, &idx));

    self = Data_Wrap_Struct(self, NULL, ICONV_FREE, (void *)cd);
    if (idx >= 0) ENCODING_SET(self, idx);

    if (rb_block_given_p()) {
	return rb_ensure(rb_yield, self, (VALUE(*)())iconv_finish, self);
    }
    else {
	return self;
    }
}

static VALUE
iconv_s_convert(struct iconv_env_t* env)
{
    VALUE last = 0;

    for (; env->argc > 0; --env->argc, ++env->argv) {
	VALUE s = iconv_convert(env->cd, last = *(env->argv),
				0, -1, env->toidx, env);
	env->append(env->ret, s);
    }

    if (!NIL_P(last)) {
	VALUE s = iconv_convert(env->cd, Qnil, 0, 0, env->toidx, env);
	if (RSTRING_LEN(s))
	    env->append(env->ret, s);
    }

    return env->ret;
}

/*
 * Document-method: Iconv::iconv
 * call-seq: Iconv.iconv(to, from, *strs)
 *
 * Shorthand for
 *   Iconv.open(to, from) { |cd|
 *     (strs + [nil]).collect { |s| cd.iconv(s) }
 *   }
 *
 * === Parameters
 *
 * <tt>to, from</tt>:: see Iconv.new
 * <tt>strs</tt>:: strings to be converted
 *
 * === Exceptions
 *
 * Exceptions thrown by Iconv.new, Iconv.open and Iconv#iconv.
 */
static VALUE
iconv_s_iconv(int argc, VALUE *argv, VALUE self)
{
    struct iconv_env_t arg;

    if (argc < 2)		/* needs `to' and `from' arguments at least */
	rb_raise(rb_eArgError, "wrong number of arguments (%d for %d)", argc, 2);

    arg.argc = argc -= 2;
    arg.argv = argv + 2;
    arg.append = rb_ary_push;
    arg.ret = rb_ary_new2(argc);
    arg.cd = iconv_create(argv[0], argv[1], NULL, &arg.toidx);
    return rb_ensure(iconv_s_convert, (VALUE)&arg, iconv_free, ICONV2VALUE(arg.cd));
}

/*
 * Document-method: Iconv::conv
 * call-seq: Iconv.conv(to, from, str)
 *
 * Shorthand for
 *   Iconv.iconv(to, from, str).join
 * See Iconv.iconv.
 */
static VALUE
iconv_s_conv(VALUE self, VALUE to, VALUE from, VALUE str)
{
    struct iconv_env_t arg;

    arg.argc = 1;
    arg.argv = &str;
    arg.append = rb_str_append;
    arg.ret = rb_str_new(0, 0);
    arg.cd = iconv_create(to, from, NULL, &arg.toidx);
    return rb_ensure(iconv_s_convert, (VALUE)&arg, iconv_free, ICONV2VALUE(arg.cd));
}

/*
 * Document-method: list
 * call-seq: Iconv.list {|*aliases| ... }
 *
 * Iterates each alias sets.
 */

#ifdef HAVE_ICONVLIST
struct iconv_name_list
{
    unsigned int namescount;
    const char *const *names;
    VALUE array;
};

static VALUE
list_iconv_i(VALUE ptr)
{
    struct iconv_name_list *p = (struct iconv_name_list *)ptr;
    unsigned int i, namescount = p->namescount;
    const char *const *names = p->names;
    VALUE ary = rb_ary_new2(namescount);

    for (i = 0; i < namescount; i++) {
	rb_ary_push(ary, rb_str_new2(names[i]));
    }
    if (p->array) {
	return rb_ary_push(p->array, ary);
    }
    return rb_yield(ary);
}

static int
list_iconv(unsigned int namescount, const char *const *names, void *data)
{
    int *state = data;
    struct iconv_name_list list;

    list.namescount = namescount;
    list.names = names;
    list.array = ((VALUE *)data)[1];
    rb_protect(list_iconv_i, (VALUE)&list, state);
    return *state;
}
#endif

#if defined(HAVE_ICONVLIST) || defined(HAVE___ICONV_FREE_LIST)
static VALUE
iconv_s_list(void)
{
#ifdef HAVE_ICONVLIST
    int state;
    VALUE args[2];

    args[1] = rb_block_given_p() ? 0 : rb_ary_new();
    iconvlist(list_iconv, args);
    state = *(int *)args;
    if (state) rb_jump_tag(state);
    if (args[1]) return args[1];
#elif defined(HAVE___ICONV_FREE_LIST)
    char **list;
    size_t sz, i;
    VALUE ary;

    if (__iconv_get_list(&list, &sz)) return Qnil;

    ary = rb_ary_new2(sz);
    for (i = 0; i < sz; i++) {
	rb_ary_push(ary, rb_str_new2(list[i]));
    }
    __iconv_free_list(list, sz);

    if (!rb_block_given_p())
	return ary;
    for (i = 0; i < RARRAY_LEN(ary); i++) {
	rb_yield(RARRAY_PTR(ary)[i]);
    }
#endif
    return Qnil;
}
#else
#define iconv_s_list rb_f_notimplement
#endif

/*
 * Document-method: close
 *
 * Finishes conversion.
 *
 * After calling this, calling Iconv#iconv will cause an exception, but
 * multiple calls of #close are guaranteed to end successfully.
 *
 * Returns a string containing the byte sequence to change the output buffer to
 * its initial shift state.
 */
static VALUE
iconv_init_state(VALUE self)
{
    iconv_t cd = VALUE2ICONV((VALUE)DATA_PTR(self));
    DATA_PTR(self) = NULL;
    return iconv_convert(cd, Qnil, 0, 0, ENCODING_GET(self), NULL);
}

static VALUE
iconv_finish(VALUE self)
{
    VALUE cd = check_iconv(self);

    if (!cd) return Qnil;
    return rb_ensure(iconv_init_state, self, iconv_free, cd);
}

/*
 * Document-method: Iconv#iconv
 * call-seq: iconv(str, start=0, length=-1)
 *
 * Converts string and returns the result.
 * * If +str+ is a String, converts <tt>str[start, length]</tt> and returns the converted string.
 * * If +str+ is +nil+, places converter itself into initial shift state and
 *   just returns a string containing the byte sequence to change the output
 *   buffer to its initial shift state.
 * * Otherwise, raises an exception.
 *
 * === Parameters
 *
 * str::    string to be converted, or nil
 * start::  starting offset
 * length:: conversion length; nil or -1 means whole the string from start
 *
 * === Exceptions
 *
 * * IconvIllegalSequence
 * * IconvInvalidCharacter
 * * IconvOutOfRange
 *
 * === Examples
 *
 * See the Iconv documentation.
 */
static VALUE
iconv_iconv(int argc, VALUE *argv, VALUE self)
{
    VALUE str, n1, n2;
    VALUE cd = check_iconv(self);
    long start = 0, length = 0, slen = 0;

    rb_scan_args(argc, argv, "12", &str, &n1, &n2);
    if (!NIL_P(str)) {
	VALUE n = rb_str_length(StringValue(str));
	slen = NUM2LONG(n);
    }
    if (argc != 2 || !RTEST(rb_range_beg_len(n1, &start, &length, slen, 0))) {
	if (NIL_P(n1) || ((start = NUM2LONG(n1)) < 0 ? (start += slen) >= 0 : start < slen)) {
	    length = NIL_P(n2) ? -1 : NUM2LONG(n2);
	}
    }
    if (start > 0 || length > 0) {
	rb_encoding *enc = rb_enc_get(str);
	const char *s = RSTRING_PTR(str), *e = s + RSTRING_LEN(str);
	const char *ps = s;
	if (start > 0) {
	    start = (ps = rb_enc_nth(s, e, start, enc)) - s;
	}
	if (length > 0) {
	    length = rb_enc_nth(ps, e, length, enc) - ps;
	}
    }

    return iconv_convert(VALUE2ICONV(cd), str, start, length, ENCODING_GET(self), NULL);
}

/*
 * Document-method: conv
 * call-seq: conv(str...)
 *
 * Equivalent to
 *
 *   iconv(nil, str..., nil).join
 */
static VALUE
iconv_conv(int argc, VALUE *argv, VALUE self)
{
    iconv_t cd = VALUE2ICONV(check_iconv(self));
    VALUE str, s;
    int toidx = ENCODING_GET(self);

    str = iconv_convert(cd, Qnil, 0, 0, toidx, NULL);
    if (argc > 0) {
	do {
	    s = iconv_convert(cd, *argv++, 0, -1, toidx, NULL);
	    if (RSTRING_LEN(s))
		rb_str_buf_append(str, s);
	} while (--argc);
	s = iconv_convert(cd, Qnil, 0, 0, toidx, NULL);
	if (RSTRING_LEN(s))
	    rb_str_buf_append(str, s);
    }

    return str;
}

#ifdef ICONV_TRIVIALP
/*
 * Document-method: trivial?
 * call-seq: trivial?
 *
 * Returns trivial flag.
 */
static VALUE
iconv_trivialp(VALUE self)
{
    int trivial = 0;
    iconv_ctl(self, ICONV_TRIVIALP, trivial);
    if (trivial) return Qtrue;
    return Qfalse;
}
#else
#define iconv_trivialp rb_f_notimplement
#endif

#ifdef ICONV_GET_TRANSLITERATE
/*
 * Document-method: transliterate?
 * call-seq: transliterate?
 *
 * Returns transliterate flag.
 */
static VALUE
iconv_get_transliterate(VALUE self)
{
    int trans = 0;
    iconv_ctl(self, ICONV_GET_TRANSLITERATE, trans);
    if (trans) return Qtrue;
    return Qfalse;
}
#else
#define iconv_get_transliterate rb_f_notimplement
#endif

#ifdef ICONV_SET_TRANSLITERATE
/*
 * Document-method: transliterate=
 * call-seq: cd.transliterate = flag
 *
 * Sets transliterate flag.
 */
static VALUE
iconv_set_transliterate(VALUE self, VALUE transliterate)
{
    int trans = RTEST(transliterate);
    iconv_ctl(self, ICONV_SET_TRANSLITERATE, trans);
    return self;
}
#else
#define iconv_set_transliterate rb_f_notimplement
#endif

#ifdef ICONV_GET_DISCARD_ILSEQ
/*
 * Document-method: discard_ilseq?
 * call-seq: discard_ilseq?
 *
 * Returns discard_ilseq flag.
 */
static VALUE
iconv_get_discard_ilseq(VALUE self)
{
    int dis = 0;
    iconv_ctl(self, ICONV_GET_DISCARD_ILSEQ, dis);
    if (dis) return Qtrue;
    return Qfalse;
}
#else
#define iconv_get_discard_ilseq rb_f_notimplement
#endif

#ifdef ICONV_SET_DISCARD_ILSEQ
/*
 * Document-method: discard_ilseq=
 * call-seq: cd.discard_ilseq = flag
 *
 * Sets discard_ilseq flag.
 */
static VALUE
iconv_set_discard_ilseq(VALUE self, VALUE discard_ilseq)
{
    int dis = RTEST(discard_ilseq);
    iconv_ctl(self, ICONV_SET_DISCARD_ILSEQ, dis);
    return self;
}
#else
#define iconv_set_discard_ilseq rb_f_notimplement
#endif

/*
 * Document-method: ctlmethods
 * call-seq: Iconv.ctlmethods => array
 *
 * Returns available iconvctl() method list.
 */
static VALUE
iconv_s_ctlmethods(VALUE klass)
{
    VALUE ary = rb_ary_new();
#ifdef ICONV_TRIVIALP
    rb_ary_push(ary, ID2SYM(rb_intern("trivial?")));
#endif
#ifdef ICONV_GET_TRANSLITERATE
    rb_ary_push(ary, ID2SYM(rb_intern("transliterate?")));
#endif
#ifdef ICONV_SET_TRANSLITERATE
    rb_ary_push(ary, ID2SYM(rb_intern("transliterate=")));
#endif
#ifdef ICONV_GET_DISCARD_ILSEQ
    rb_ary_push(ary, ID2SYM(rb_intern("discard_ilseq?")));
#endif
#ifdef ICONV_SET_DISCARD_ILSEQ
    rb_ary_push(ary, ID2SYM(rb_intern("discard_ilseq=")));
#endif
    return ary;
}

/*
 * Document-class: Iconv::Failure
 *
 * Base attributes for Iconv exceptions.
 */

/*
 * Document-method: success
 * call-seq: success
 *
 * Returns string(s) translated successfully until the exception occurred.
 * * In the case of failure occurred within Iconv.iconv, returned
 *   value is an array of strings translated successfully preceding
 *   failure and the last element is string on the way.
 */
static VALUE
iconv_failure_success(VALUE self)
{
    return rb_attr_get(self, rb_success);
}

/*
 * Document-method: failed
 * call-seq: failed
 *
 * Returns substring of the original string passed to Iconv that starts at the
 * character caused the exception.
 */
static VALUE
iconv_failure_failed(VALUE self)
{
    return rb_attr_get(self, rb_failed);
}

/*
 * Document-method: inspect
 * call-seq: inspect
 *
 * Returns inspected string like as: #<_class_: _success_, _failed_>
 */
static VALUE
iconv_failure_inspect(VALUE self)
{
    const char *cname = rb_class2name(CLASS_OF(self));
    VALUE success = rb_attr_get(self, rb_success);
    VALUE failed = rb_attr_get(self, rb_failed);
    VALUE str = rb_str_buf_cat2(rb_str_new2("#<"), cname);
    str = rb_str_buf_cat(str, ": ", 2);
    str = rb_str_buf_append(str, rb_inspect(success));
    str = rb_str_buf_cat(str, ", ", 2);
    str = rb_str_buf_append(str, rb_inspect(failed));
    return rb_str_buf_cat(str, ">", 1);
}

/*
 * Document-class: Iconv::InvalidEncoding
 *
 * Requested coding-system is not available on this system.
 */

/*
 * Document-class: Iconv::IllegalSequence
 *
 * Input conversion stopped due to an input byte that does not belong to
 * the input codeset, or the output codeset does not contain the
 * character.
 */

/*
 * Document-class: Iconv::InvalidCharacter
 *
 * Input conversion stopped due to an incomplete character or shift
 * sequence at the end of the input buffer.
 */

/*
 * Document-class: Iconv::OutOfRange
 *
 * Iconv library internal error.  Must not occur.
 */

/*
 * Document-class: Iconv::BrokenLibrary
 *
 * Detected a bug of underlying iconv(3) libray.
 * * returns an error without setting errno properly
 */

static void
warn_deprecated(void)
{
    static const char message[] =
	": iconv will be deprecated in the future, use String#encode instead.\n";
    VALUE msg = Qnil, caller = rb_make_backtrace();
    long i;

    for (i = 1; i < RARRAY_LEN(caller); ++i) {
	VALUE s = RARRAY_PTR(caller)[i];
	if (strncmp(RSTRING_PTR(s), "<internal:", 10) != 0) {
	    msg = s;
	    break;
	}
    }
    if (NIL_P(msg)) {
	msg = rb_str_new_cstr(message + 2);
    }
    else {
	rb_str_cat(msg, message, sizeof(message) - 1);
    }
    rb_io_puts(1, &msg, rb_stderr);
}

void
Init_iconv(void)
{
    VALUE rb_cIconv = rb_define_class("Iconv", rb_cData);

    if (!NIL_P(ruby_verbose)) {
	warn_deprecated();
    }
    rb_define_alloc_func(rb_cIconv, iconv_s_allocate);
    rb_define_singleton_method(rb_cIconv, "open", iconv_s_open, -1);
    rb_define_singleton_method(rb_cIconv, "iconv", iconv_s_iconv, -1);
    rb_define_singleton_method(rb_cIconv, "conv", iconv_s_conv, 3);
    rb_define_singleton_method(rb_cIconv, "list", iconv_s_list, 0);
    rb_define_singleton_method(rb_cIconv, "ctlmethods", iconv_s_ctlmethods, 0);
    rb_define_method(rb_cIconv, "initialize", iconv_initialize, -1);
    rb_define_method(rb_cIconv, "close", iconv_finish, 0);
    rb_define_method(rb_cIconv, "iconv", iconv_iconv, -1);
    rb_define_method(rb_cIconv, "conv", iconv_conv, -1);
    rb_define_method(rb_cIconv, "trivial?", iconv_trivialp, 0);
    rb_define_method(rb_cIconv, "transliterate?", iconv_get_transliterate, 0);
    rb_define_method(rb_cIconv, "transliterate=", iconv_set_transliterate, 1);
    rb_define_method(rb_cIconv, "discard_ilseq?", iconv_get_discard_ilseq, 0);
    rb_define_method(rb_cIconv, "discard_ilseq=", iconv_set_discard_ilseq, 1);

    rb_eIconvFailure = rb_define_module_under(rb_cIconv, "Failure");
    rb_define_method(rb_eIconvFailure, "initialize", iconv_failure_initialize, 3);
    rb_define_method(rb_eIconvFailure, "success", iconv_failure_success, 0);
    rb_define_method(rb_eIconvFailure, "failed", iconv_failure_failed, 0);
    rb_define_method(rb_eIconvFailure, "inspect", iconv_failure_inspect, 0);

    rb_eIconvInvalidEncoding = rb_define_class_under(rb_cIconv, "InvalidEncoding", rb_eArgError);
    rb_eIconvIllegalSeq = rb_define_class_under(rb_cIconv, "IllegalSequence", rb_eArgError);
    rb_eIconvInvalidChar = rb_define_class_under(rb_cIconv, "InvalidCharacter", rb_eArgError);
    rb_eIconvOutOfRange = rb_define_class_under(rb_cIconv, "OutOfRange", rb_eRuntimeError);
    rb_eIconvBrokenLibrary = rb_define_class_under(rb_cIconv, "BrokenLibrary", rb_eRuntimeError);
    rb_include_module(rb_eIconvInvalidEncoding, rb_eIconvFailure);
    rb_include_module(rb_eIconvIllegalSeq, rb_eIconvFailure);
    rb_include_module(rb_eIconvInvalidChar, rb_eIconvFailure);
    rb_include_module(rb_eIconvOutOfRange, rb_eIconvFailure);
    rb_include_module(rb_eIconvBrokenLibrary, rb_eIconvFailure);

    rb_success = rb_intern("success");
    rb_failed = rb_intern("failed");
    id_transliterate = rb_intern("transliterate");
    id_discard_ilseq = rb_intern("discard_ilseq");

    rb_gc_register_address(&charset_map);
    charset_map = rb_hash_new();
    rb_define_singleton_method(rb_cIconv, "charset_map", charset_map_get, 0);
}

