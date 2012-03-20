/**********************************************************************

  stringio.c -

  $Author$
  $RoughId: stringio.c,v 1.13 2002/03/14 03:24:18 nobu Exp $
  created at: Tue Feb 19 04:10:38 JST 2002

  All the files in this distribution are covered under the Ruby's
  license (see the file COPYING).

**********************************************************************/

#include "ruby.h"
#include "ruby/io.h"
#include "ruby/encoding.h"
#if defined(HAVE_FCNTL_H) || defined(_WIN32)
#include <fcntl.h>
#elif defined(HAVE_SYS_FCNTL_H)
#include <sys/fcntl.h>
#endif

struct StringIO {
    VALUE string;
    long pos;
    long lineno;
    int flags;
    int count;
};

static void strio_init(int, VALUE *, struct StringIO *);

#define IS_STRIO(obj) (rb_typeddata_is_kind_of((obj), &strio_data_type))
#define error_inval(msg) (errno = EINVAL, rb_sys_fail(msg))

static struct StringIO *
strio_alloc(void)
{
    struct StringIO *ptr = ALLOC(struct StringIO);
    ptr->string = Qnil;
    ptr->pos = 0;
    ptr->lineno = 0;
    ptr->flags = 0;
    ptr->count = 1;
    return ptr;
}

static void
strio_mark(void *p)
{
    struct StringIO *ptr = p;
    if (ptr) {
	rb_gc_mark(ptr->string);
    }
}

static void
strio_free(void *p)
{
    struct StringIO *ptr = p;
    if (--ptr->count <= 0) {
	xfree(ptr);
    }
}

static size_t
strio_memsize(const void *p)
{
    const struct StringIO *ptr = p;
    if (!ptr) return 0;
    return sizeof(struct StringIO);
}

static const rb_data_type_t strio_data_type = {
    "strio",
    {
	strio_mark,
	strio_free,
	strio_memsize,
    },
};

#define check_strio(self) ((struct StringIO*)rb_check_typeddata((self), &strio_data_type))

static struct StringIO*
get_strio(VALUE self)
{
    struct StringIO *ptr = check_strio(rb_io_taint_check(self));

    if (!ptr) {
	rb_raise(rb_eIOError, "uninitialized stream");
    }
    return ptr;
}

static VALUE
strio_substr(struct StringIO *ptr, long pos, long len)
{
    VALUE str = ptr->string;
    rb_encoding *enc = rb_enc_get(str);
    long rlen = RSTRING_LEN(str) - pos;

    if (len > rlen) len = rlen;
    if (len < 0) len = 0;
    return rb_enc_str_new(RSTRING_PTR(str)+pos, len, enc);
}

#define StringIO(obj) get_strio(obj)

#define CLOSED(ptr) (!((ptr)->flags & FMODE_READWRITE))
#define READABLE(ptr) ((ptr)->flags & FMODE_READABLE)
#define WRITABLE(ptr) ((ptr)->flags & FMODE_WRITABLE)

static struct StringIO*
readable(struct StringIO *ptr)
{
    if (!READABLE(ptr)) {
	rb_raise(rb_eIOError, "not opened for reading");
    }
    return ptr;
}

static struct StringIO*
writable(struct StringIO *ptr)
{
    if (!WRITABLE(ptr)) {
	rb_raise(rb_eIOError, "not opened for writing");
    }
    if (!OBJ_TAINTED(ptr->string)) {
	rb_secure(4);
    }
    return ptr;
}

static void
check_modifiable(struct StringIO *ptr)
{
    if (OBJ_FROZEN(ptr->string)) {
	rb_raise(rb_eIOError, "not modifiable string");
    }
}

static VALUE
strio_s_allocate(VALUE klass)
{
    return TypedData_Wrap_Struct(klass, &strio_data_type, 0);
}

/*
 * call-seq: StringIO.new(string=""[, mode])
 *
 * Creates new StringIO instance from with _string_ and _mode_.
 */
static VALUE
strio_initialize(int argc, VALUE *argv, VALUE self)
{
    struct StringIO *ptr = check_strio(self);

    if (!ptr) {
	DATA_PTR(self) = ptr = strio_alloc();
    }
    rb_call_super(0, 0);
    strio_init(argc, argv, ptr);
    return self;
}

static void
strio_init(int argc, VALUE *argv, struct StringIO *ptr)
{
    VALUE string, mode;
    int trunc = 0;

    switch (rb_scan_args(argc, argv, "02", &string, &mode)) {
      case 2:
	if (FIXNUM_P(mode)) {
	    int flags = FIX2INT(mode);
	    ptr->flags = rb_io_modenum_flags(flags);
	    trunc = flags & O_TRUNC;
	}
	else {
	    const char *m = StringValueCStr(mode);
	    ptr->flags = rb_io_mode_flags(m);
	    trunc = *m == 'w';
	}
	StringValue(string);
	if ((ptr->flags & FMODE_WRITABLE) && OBJ_FROZEN(string)) {
	    errno = EACCES;
	    rb_sys_fail(0);
	}
	if (trunc) {
	    rb_str_resize(string, 0);
	}
	break;
      case 1:
	StringValue(string);
	ptr->flags = OBJ_FROZEN(string) ? FMODE_READABLE : FMODE_READWRITE;
	break;
      case 0:
	string = rb_enc_str_new("", 0, rb_default_external_encoding());
	ptr->flags = FMODE_READWRITE;
	break;
    }
    ptr->string = string;
    ptr->pos = 0;
    ptr->lineno = 0;
}

static VALUE
strio_finalize(VALUE self)
{
    struct StringIO *ptr = StringIO(self);
    ptr->string = Qnil;
    ptr->flags &= ~FMODE_READWRITE;
    return self;
}

/*
 * call-seq: StringIO.open(string=""[, mode]) {|strio| ...}
 *
 * Equivalent to StringIO.new except that when it is called with a block, it
 * yields with the new instance and closes it, and returns the result which
 * returned from the block.
 */
static VALUE
strio_s_open(int argc, VALUE *argv, VALUE klass)
{
    VALUE obj = rb_class_new_instance(argc, argv, klass);
    if (!rb_block_given_p()) return obj;
    return rb_ensure(rb_yield, obj, strio_finalize, obj);
}

/*
 * Returns +false+.  Just for compatibility to IO.
 */
static VALUE
strio_false(VALUE self)
{
    StringIO(self);
    return Qfalse;
}

/*
 * Returns +nil+.  Just for compatibility to IO.
 */
static VALUE
strio_nil(VALUE self)
{
    StringIO(self);
    return Qnil;
}

/*
 * Returns *strio* itself.  Just for compatibility to IO.
 */
static VALUE
strio_self(VALUE self)
{
    StringIO(self);
    return self;
}

/*
 * Returns 0.  Just for compatibility to IO.
 */
static VALUE
strio_0(VALUE self)
{
    StringIO(self);
    return INT2FIX(0);
}

/*
 * Returns the argument unchanged.  Just for compatibility to IO.
 */
static VALUE
strio_first(VALUE self, VALUE arg)
{
    StringIO(self);
    return arg;
}

/*
 * Raises NotImplementedError.
 */
static VALUE
strio_unimpl(int argc, VALUE *argv, VALUE self)
{
    StringIO(self);
    rb_notimplement();
    return Qnil;		/* not reached */
}

/*
 * call-seq: strio.string     -> string
 *
 * Returns underlying String object, the subject of IO.
 */
static VALUE
strio_get_string(VALUE self)
{
    return StringIO(self)->string;
}

/*
 * call-seq:
 *   strio.string = string  -> string
 *
 * Changes underlying String object, the subject of IO.
 */
static VALUE
strio_set_string(VALUE self, VALUE string)
{
    struct StringIO *ptr = StringIO(self);

    rb_io_taint_check(self);
    ptr->flags &= ~FMODE_READWRITE;
    StringValue(string);
    ptr->flags = OBJ_FROZEN(string) ? FMODE_READABLE : FMODE_READWRITE;
    ptr->pos = 0;
    ptr->lineno = 0;
    return ptr->string = string;
}

/*
 * call-seq:
 *   strio.close  -> nil
 *
 * Closes strio.  The *strio* is unavailable for any further data
 * operations; an +IOError+ is raised if such an attempt is made.
 */
static VALUE
strio_close(VALUE self)
{
    struct StringIO *ptr = StringIO(self);
    if (CLOSED(ptr)) {
	rb_raise(rb_eIOError, "closed stream");
    }
    ptr->flags &= ~FMODE_READWRITE;
    return Qnil;
}

/*
 * call-seq:
 *   strio.close_read    -> nil
 *
 * Closes the read end of a StringIO.  Will raise an +IOError+ if the
 * *strio* is not readable.
 */
static VALUE
strio_close_read(VALUE self)
{
    struct StringIO *ptr = StringIO(self);
    if (!READABLE(ptr)) {
	rb_raise(rb_eIOError, "closing non-duplex IO for reading");
    }
    ptr->flags &= ~FMODE_READABLE;
    return Qnil;
}

/*
 * call-seq:
 *   strio.close_write    -> nil
 *
 * Closes the write end of a StringIO.  Will raise an  +IOError+ if the
 * *strio* is not writeable.
 */
static VALUE
strio_close_write(VALUE self)
{
    struct StringIO *ptr = StringIO(self);
    if (!WRITABLE(ptr)) {
	rb_raise(rb_eIOError, "closing non-duplex IO for writing");
    }
    ptr->flags &= ~FMODE_WRITABLE;
    return Qnil;
}

/*
 * call-seq:
 *   strio.closed?    -> true or false
 *
 * Returns +true+ if *strio* is completely closed, +false+ otherwise.
 */
static VALUE
strio_closed(VALUE self)
{
    struct StringIO *ptr = StringIO(self);
    if (!CLOSED(ptr)) return Qfalse;
    return Qtrue;
}

/*
 * call-seq:
 *   strio.closed_read?    -> true or false
 *
 * Returns +true+ if *strio* is not readable, +false+ otherwise.
 */
static VALUE
strio_closed_read(VALUE self)
{
    struct StringIO *ptr = StringIO(self);
    if (READABLE(ptr)) return Qfalse;
    return Qtrue;
}

/*
 * call-seq:
 *   strio.closed_write?    -> true or false
 *
 * Returns +true+ if *strio* is not writable, +false+ otherwise.
 */
static VALUE
strio_closed_write(VALUE self)
{
    struct StringIO *ptr = StringIO(self);
    if (WRITABLE(ptr)) return Qfalse;
    return Qtrue;
}

/*
 * call-seq:
 *   strio.eof     -> true or false
 *   strio.eof?    -> true or false
 *
 * Returns true if *strio* is at end of file. The stringio must be
 * opened for reading or an +IOError+ will be raised.
 */
static VALUE
strio_eof(VALUE self)
{
    struct StringIO *ptr = readable(StringIO(self));
    if (ptr->pos < RSTRING_LEN(ptr->string)) return Qfalse;
    return Qtrue;
}

/* :nodoc: */
static VALUE
strio_copy(VALUE copy, VALUE orig)
{
    struct StringIO *ptr;

    orig = rb_convert_type(orig, T_DATA, "StringIO", "to_strio");
    if (copy == orig) return copy;
    ptr = StringIO(orig);
    if (check_strio(copy)) {
	strio_free(DATA_PTR(copy));
    }
    DATA_PTR(copy) = ptr;
    OBJ_INFECT(copy, orig);
    ++ptr->count;
    return copy;
}

/*
 * call-seq:
 *   strio.lineno    -> integer
 *
 * Returns the current line number in *strio*. The stringio must be
 * opened for reading. +lineno+ counts the number of times  +gets+ is
 * called, rather than the number of newlines  encountered. The two
 * values will differ if +gets+ is  called with a separator other than
 * newline.  See also the  <code>$.</code> variable.
 */
static VALUE
strio_get_lineno(VALUE self)
{
    return LONG2NUM(StringIO(self)->lineno);
}

/*
 * call-seq:
 *   strio.lineno = integer    -> integer
 *
 * Manually sets the current line number to the given value.
 * <code>$.</code> is updated only on the next read.
 */
static VALUE
strio_set_lineno(VALUE self, VALUE lineno)
{
    StringIO(self)->lineno = NUM2LONG(lineno);
    return lineno;
}

/* call-seq: strio.binmode -> true */
#define strio_binmode strio_self

/* call-seq: strio.fcntl */
#define strio_fcntl strio_unimpl

/* call-seq: strio.flush -> strio */
#define strio_flush strio_self

/* call-seq: strio.fsync -> 0 */
#define strio_fsync strio_0

/*
 * call-seq:
 *   strio.reopen(other_StrIO)     -> strio
 *   strio.reopen(string, mode)    -> strio
 *
 * Reinitializes *strio* with the given <i>other_StrIO</i> or _string_
 * and _mode_ (see StringIO#new).
 */
static VALUE
strio_reopen(int argc, VALUE *argv, VALUE self)
{
    rb_io_taint_check(self);
    if (argc == 1 && TYPE(*argv) != T_STRING) {
	return strio_copy(self, *argv);
    }
    strio_init(argc, argv, StringIO(self));
    return self;
}

/*
 * call-seq:
 *   strio.pos     -> integer
 *   strio.tell    -> integer
 *
 * Returns the current offset (in bytes) of *strio*.
 */
static VALUE
strio_get_pos(VALUE self)
{
    return LONG2NUM(StringIO(self)->pos);
}

/*
 * call-seq:
 *   strio.pos = integer    -> integer
 *
 * Seeks to the given position (in bytes) in *strio*.
 */
static VALUE
strio_set_pos(VALUE self, VALUE pos)
{
    struct StringIO *ptr = StringIO(self);
    long p = NUM2LONG(pos);
    if (p < 0) {
	error_inval(0);
    }
    ptr->pos = p;
    return pos;
}

/*
 * call-seq:
 *   strio.rewind    -> 0
 *
 * Positions *strio* to the beginning of input, resetting
 * +lineno+ to zero.
 */
static VALUE
strio_rewind(VALUE self)
{
    struct StringIO *ptr = StringIO(self);
    ptr->pos = 0;
    ptr->lineno = 0;
    return INT2FIX(0);
}

/*
 * call-seq:
 *   strio.seek(amount, whence=SEEK_SET) -> 0
 *
 * Seeks to a given offset _amount_ in the stream according to
 * the value of _whence_ (see IO#seek).
 */
static VALUE
strio_seek(int argc, VALUE *argv, VALUE self)
{
    VALUE whence;
    struct StringIO *ptr = StringIO(self);
    long offset;

    rb_scan_args(argc, argv, "11", NULL, &whence);
    offset = NUM2LONG(argv[0]);
    if (CLOSED(ptr)) {
	rb_raise(rb_eIOError, "closed stream");
    }
    switch (NIL_P(whence) ? 0 : NUM2LONG(whence)) {
      case 0:
	break;
      case 1:
	offset += ptr->pos;
	break;
      case 2:
	offset += RSTRING_LEN(ptr->string);
	break;
      default:
	error_inval("invalid whence");
    }
    if (offset < 0) {
	error_inval(0);
    }
    ptr->pos = offset;
    return INT2FIX(0);
}

/*
 * call-seq:
 *   strio.sync    -> true
 *
 * Returns +true+ always.
 */
static VALUE
strio_get_sync(VALUE self)
{
    StringIO(self);
    return Qtrue;
}

/* call-seq: strio.sync = boolean -> boolean */
#define strio_set_sync strio_first

#define strio_tell strio_get_pos

/*
 * call-seq:
 *   strio.bytes {|byte| block }      -> strio
 *   strio.bytes                      -> anEnumerator
 *
 *   strio.each_byte {|byte| block }  -> strio
 *   strio.each_byte                  -> anEnumerator
 *
 * See IO#each_byte.
 */
static VALUE
strio_each_byte(VALUE self)
{
    struct StringIO *ptr = readable(StringIO(self));

    RETURN_ENUMERATOR(self, 0, 0);

    while (ptr->pos < RSTRING_LEN(ptr->string)) {
	char c = RSTRING_PTR(ptr->string)[ptr->pos++];
	rb_yield(CHR2FIX(c));
    }
    return self;
}

/*
 * call-seq:
 *   strio.getc   -> string or nil
 *
 * See IO#getc.
 */
static VALUE
strio_getc(VALUE self)
{
    struct StringIO *ptr = readable(StringIO(self));
    rb_encoding *enc = rb_enc_get(ptr->string);
    int len;
    char *p;

    if (ptr->pos >= RSTRING_LEN(ptr->string)) {
	return Qnil;
    }
    p = RSTRING_PTR(ptr->string)+ptr->pos;
    len = rb_enc_mbclen(p, RSTRING_END(ptr->string), enc);
    ptr->pos += len;
    return rb_enc_str_new(p, len, rb_enc_get(ptr->string));
}

/*
 * call-seq:
 *   strio.getbyte   -> fixnum or nil
 *
 * See IO#getbyte.
 */
static VALUE
strio_getbyte(VALUE self)
{
    struct StringIO *ptr = readable(StringIO(self));
    int c;
    if (ptr->pos >= RSTRING_LEN(ptr->string)) {
	return Qnil;
    }
    c = RSTRING_PTR(ptr->string)[ptr->pos++];
    return CHR2FIX(c);
}

static void
strio_extend(struct StringIO *ptr, long pos, long len)
{
    long olen;

    check_modifiable(ptr);
    olen = RSTRING_LEN(ptr->string);
    if (pos + len > olen) {
	rb_str_resize(ptr->string, pos + len);
	if (pos > olen)
	    MEMZERO(RSTRING_PTR(ptr->string) + olen, char, pos - olen);
    }
    else {
	rb_str_modify(ptr->string);
    }
}

/*
 * call-seq:
 *   strio.ungetc(string)   -> nil
 *
 * Pushes back one character (passed as a parameter) onto *strio*
 * such that a subsequent buffered read will return it.  There is no
 * limitation for multiple pushbacks including pushing back behind the
 * beginning of the buffer string.
 */
static VALUE
strio_ungetc(VALUE self, VALUE c)
{
    struct StringIO *ptr = readable(StringIO(self));
    long lpos, clen;
    char *p, *pend;
    rb_encoding *enc, *enc2;

    if (NIL_P(c)) return Qnil;
    if (FIXNUM_P(c)) {
	int cc = FIX2INT(c);
	char buf[16];

	enc = rb_enc_get(ptr->string);
	rb_enc_mbcput(cc, buf, enc);
	c = rb_enc_str_new(buf, rb_enc_codelen(cc, enc), enc);
    }
    else {
	SafeStringValue(c);
	enc = rb_enc_get(ptr->string);
	enc2 = rb_enc_get(c);
	if (enc != enc2 && enc != rb_ascii8bit_encoding()) {
	    c = rb_str_conv_enc(c, enc2, enc);
	}
    }
    if (RSTRING_LEN(ptr->string) < ptr->pos) {
	long len = RSTRING_LEN(ptr->string);
	rb_str_resize(ptr->string, ptr->pos - 1);
	memset(RSTRING_PTR(ptr->string) + len, 0, ptr->pos - len - 1);
	rb_str_concat(ptr->string, c);
	ptr->pos--;
    }
    else {
	/* get logical position */
	lpos = 0; p = RSTRING_PTR(ptr->string); pend = p + ptr->pos;
	for (;;) {
	    clen = rb_enc_mbclen(p, pend, enc);
	    if (p+clen >= pend) break;
	    p += clen;
	    lpos++;
	}
	clen = p - RSTRING_PTR(ptr->string);
	rb_str_update(ptr->string, lpos, ptr->pos ? 1 : 0, c);
	ptr->pos = clen;
    }

    return Qnil;
}

/*
 * call-seq:
 *   strio.ungetbyte(fixnum)   -> nil
 *
 * See IO#ungetbyte
 */
static VALUE
strio_ungetbyte(VALUE self, VALUE c)
{
    struct StringIO *ptr = readable(StringIO(self));
    char buf[1], *cp = buf;
    long pos = ptr->pos, cl = 1;
    VALUE str = ptr->string;

    if (NIL_P(c)) return Qnil;
    if (FIXNUM_P(c)) {
	buf[0] = (char)FIX2INT(c);
    }
    else {
	SafeStringValue(c);
	cp = RSTRING_PTR(c);
	cl = RSTRING_LEN(c);
	if (cl == 0) return Qnil;
    }
    rb_str_modify(str);
    if (cl > pos) {
	char *s;
	long rest = RSTRING_LEN(str) - pos;
	rb_str_resize(str, rest + cl);
	s = RSTRING_PTR(str);
	memmove(s + cl, s + pos, rest);
	pos = 0;
    }
    else {
	pos -= cl;
    }
    memcpy(RSTRING_PTR(str) + pos, cp, cl);
    ptr->pos = pos;
    RB_GC_GUARD(c);
    return Qnil;
}

/*
 * call-seq:
 *   strio.readchar   -> string
 *
 * See IO#readchar.
 */
static VALUE
strio_readchar(VALUE self)
{
    VALUE c = rb_funcall2(self, rb_intern("getc"), 0, 0);
    if (NIL_P(c)) rb_eof_error();
    return c;
}

/*
 * call-seq:
 *   strio.readbyte   -> fixnum
 *
 * See IO#readbyte.
 */
static VALUE
strio_readbyte(VALUE self)
{
    VALUE c = rb_funcall2(self, rb_intern("getbyte"), 0, 0);
    if (NIL_P(c)) rb_eof_error();
    return c;
}

/*
 * call-seq:
 *   strio.chars {|char| block }      -> strio
 *   strio.chars                      -> anEnumerator
 *
 *   strio.each_char {|char| block }  -> strio
 *   strio.each_char                  -> anEnumerator
 *
 * See IO#each_char.
 */
static VALUE
strio_each_char(VALUE self)
{
    VALUE c;

    RETURN_ENUMERATOR(self, 0, 0);

    while (!NIL_P(c = strio_getc(self))) {
	rb_yield(c);
    }
    return self;
}

/*
 * call-seq:
 *   strio.codepoints {|c| block }      -> strio
 *   strio.codepoints                   -> anEnumerator
 *
 *   strio.each_codepoint {|c| block }  -> strio
 *   strio.each_codepoint               -> anEnumerator
 *
 * See IO#each_codepoint.
 */
static VALUE
strio_each_codepoint(VALUE self)
{
    struct StringIO *ptr;
    rb_encoding *enc;
    unsigned int c;
    int n;

    RETURN_ENUMERATOR(self, 0, 0);

    ptr = readable(StringIO(self));
    enc = rb_enc_get(ptr->string);
    for (;;) {
	if (ptr->pos >= RSTRING_LEN(ptr->string)) {
	    return self;
	}

	c = rb_enc_codepoint_len(RSTRING_PTR(ptr->string)+ptr->pos,
				 RSTRING_END(ptr->string), &n, enc);
	rb_yield(UINT2NUM(c));
	ptr->pos += n;
    }
    return self;
}

/* Boyer-Moore search: copied from regex.c */
static void
bm_init_skip(long *skip, const char *pat, long m)
{
    int c;

    for (c = 0; c < (1 << CHAR_BIT); c++) {
	skip[c] = m;
    }
    while (--m) {
	skip[(unsigned char)*pat++] = m;
    }
}

static long
bm_search(const char *little, long llen, const char *big, long blen, const long *skip)
{
    long i, j, k;

    i = llen - 1;
    while (i < blen) {
	k = i;
	j = llen - 1;
	while (j >= 0 && big[k] == little[j]) {
	    k--;
	    j--;
	}
	if (j < 0) return k + 1;
	i += skip[(unsigned char)big[i]];
    }
    return -1;
}

static VALUE
strio_getline(int argc, VALUE *argv, struct StringIO *ptr)
{
    const char *s, *e, *p;
    long n, limit = 0;
    VALUE str, lim;

    rb_scan_args(argc, argv, "02", &str, &lim);
    switch (argc) {
      case 0:
	str = rb_rs;
	break;

      case 1:
	if (!NIL_P(str) && TYPE(str) != T_STRING) {
	    VALUE tmp = rb_check_string_type(str);
	    if (NIL_P(tmp)) {
		limit = NUM2LONG(str);
		if (limit == 0) return rb_str_new(0,0);
		str = rb_rs;
	    }
	    else {
		str = tmp;
	    }
	}
	break;

      case 2:
	if (!NIL_P(str)) StringValue(str);
	limit = NUM2LONG(lim);
	break;
    }

    if (ptr->pos >= (n = RSTRING_LEN(ptr->string))) {
	return Qnil;
    }
    s = RSTRING_PTR(ptr->string);
    e = s + RSTRING_LEN(ptr->string);
    s += ptr->pos;
    if (limit > 0 && s + limit < e) {
	e = rb_enc_right_char_head(s, s + limit, e, rb_enc_get(ptr->string));
    }
    if (NIL_P(str)) {
	str = strio_substr(ptr, ptr->pos, e - s);
    }
    else if ((n = RSTRING_LEN(str)) == 0) {
	p = s;
	while (*p == '\n') {
	    if (++p == e) {
		return Qnil;
	    }
	}
	s = p;
	while ((p = memchr(p, '\n', e - p)) && (p != e)) {
	    if (*++p == '\n') {
		e = p + 1;
		break;
	    }
	}
	str = strio_substr(ptr, s - RSTRING_PTR(ptr->string), e - s);
    }
    else if (n == 1) {
	if ((p = memchr(s, RSTRING_PTR(str)[0], e - s)) != 0) {
	    e = p + 1;
	}
	str = strio_substr(ptr, ptr->pos, e - s);
    }
    else {
	if (n < e - s) {
	    if (e - s < 1024) {
		for (p = s; p + n <= e; ++p) {
		    if (MEMCMP(p, RSTRING_PTR(str), char, n) == 0) {
			e = p + n;
			break;
		    }
		}
	    }
	    else {
		long skip[1 << CHAR_BIT], pos;
		p = RSTRING_PTR(str);
		bm_init_skip(skip, p, n);
		if ((pos = bm_search(p, n, s, e - s, skip)) >= 0) {
		    e = s + pos + n;
		}
	    }
	}
	str = strio_substr(ptr, ptr->pos, e - s);
    }
    ptr->pos = e - RSTRING_PTR(ptr->string);
    ptr->lineno++;
    return str;
}

/*
 * call-seq:
 *   strio.gets(sep=$/)     -> string or nil
 *   strio.gets(limit)      -> string or nil
 *   strio.gets(sep, limit) -> string or nil
 *
 * See IO#gets.
 */
static VALUE
strio_gets(int argc, VALUE *argv, VALUE self)
{
    VALUE str = strio_getline(argc, argv, readable(StringIO(self)));

    rb_lastline_set(str);
    return str;
}

/*
 * call-seq:
 *   strio.readline(sep=$/)     -> string
 *   strio.readline(limit)      -> string or nil
 *   strio.readline(sep, limit) -> string or nil
 *
 * See IO#readline.
 */
static VALUE
strio_readline(int argc, VALUE *argv, VALUE self)
{
    VALUE line = rb_funcall2(self, rb_intern("gets"), argc, argv);
    if (NIL_P(line)) rb_eof_error();
    return line;
}

/*
 * call-seq:
 *   strio.each(sep=$/) {|line| block }         -> strio
 *   strio.each(limit) {|line| block }          -> strio
 *   strio.each(sep, limit) {|line| block }     -> strio
 *   strio.each(...)                            -> anEnumerator
 *
 *   strio.each_line(sep=$/) {|line| block }    -> strio
 *   strio.each_line(limit) {|line| block }     -> strio
 *   strio.each_line(sep,limit) {|line| block } -> strio
 *   strio.each_line(...)                       -> anEnumerator
 *
 *   strio.lines(sep=$/) {|line| block }        -> strio
 *   strio.lines(limit) {|line| block }         -> strio
 *   strio.lines(sep,limit) {|line| block }     -> strio
 *   strio.lines(...)                           -> anEnumerator
 *
 * See IO#each.
 */
static VALUE
strio_each(int argc, VALUE *argv, VALUE self)
{
    struct StringIO *ptr = StringIO(self);
    VALUE line;

    RETURN_ENUMERATOR(self, argc, argv);

    if (argc > 0 && !NIL_P(argv[argc-1]) && NIL_P(rb_check_string_type(argv[argc-1])) &&
	NUM2LONG(argv[argc-1]) == 0) {
	rb_raise(rb_eArgError, "invalid limit: 0 for each_line");
    }

    while (!NIL_P(line = strio_getline(argc, argv, readable(ptr)))) {
	rb_yield(line);
    }
    return self;
}

/*
 * call-seq:
 *   strio.readlines(sep=$/)    ->   array
 *   strio.readlines(limit)     ->   array
 *   strio.readlines(sep,limit) ->   array
 *
 * See IO#readlines.
 */
static VALUE
strio_readlines(int argc, VALUE *argv, VALUE self)
{
    struct StringIO *ptr = StringIO(self);
    VALUE ary = rb_ary_new(), line;

    if (argc > 0 && !NIL_P(argv[argc-1]) && NIL_P(rb_check_string_type(argv[argc-1])) &&
	NUM2LONG(argv[argc-1]) == 0) {
	rb_raise(rb_eArgError, "invalid limit: 0 for readlines");
    }

    while (!NIL_P(line = strio_getline(argc, argv, readable(ptr)))) {
	rb_ary_push(ary, line);
    }
    return ary;
}

/*
 * call-seq:
 *   strio.write(string)    -> integer
 *   strio.syswrite(string) -> integer
 *
 * Appends the given string to the underlying buffer string of *strio*.
 * The stream must be opened for writing.  If the argument is not a
 * string, it will be converted to a string using <code>to_s</code>.
 * Returns the number of bytes written.  See IO#write.
 */
static VALUE
strio_write(VALUE self, VALUE str)
{
    struct StringIO *ptr = writable(StringIO(self));
    long len, olen;
    rb_encoding *enc, *enc2;

    RB_GC_GUARD(str);
    if (TYPE(str) != T_STRING)
	str = rb_obj_as_string(str);
    enc = rb_enc_get(ptr->string);
    enc2 = rb_enc_get(str);
    if (enc != enc2 && enc != rb_ascii8bit_encoding()) {
	str = rb_str_conv_enc(str, enc2, enc);
    }
    len = RSTRING_LEN(str);
    if (len == 0) return INT2FIX(0);
    check_modifiable(ptr);
    olen = RSTRING_LEN(ptr->string);
    if (ptr->flags & FMODE_APPEND) {
	ptr->pos = olen;
    }
    if (ptr->pos == olen) {
	rb_str_cat(ptr->string, RSTRING_PTR(str), len);
    }
    else {
	strio_extend(ptr, ptr->pos, len);
	memmove(RSTRING_PTR(ptr->string)+ptr->pos, RSTRING_PTR(str), len);
	OBJ_INFECT(ptr->string, str);
    }
    OBJ_INFECT(ptr->string, self);
    ptr->pos += len;
    return LONG2NUM(len);
}

/*
 * call-seq:
 *   strio << obj     -> strio
 *
 * See IO#<<.
 */
#define strio_addstr rb_io_addstr

/*
 * call-seq:
 *   strio.print()             -> nil
 *   strio.print(obj, ...)     -> nil
 *
 * See IO#print.
 */
#define strio_print rb_io_print

/*
 * call-seq:
 *   strio.printf(format_string [, obj, ...] )   -> nil
 *
 * See IO#printf.
 */
#define strio_printf rb_io_printf

/*
 * call-seq:
 *   strio.putc(obj)    -> obj
 *
 * See IO#putc.
 */
static VALUE
strio_putc(VALUE self, VALUE ch)
{
    struct StringIO *ptr = writable(StringIO(self));
    int c = NUM2CHR(ch);
    long olen;

    check_modifiable(ptr);
    olen = RSTRING_LEN(ptr->string);
    if (ptr->flags & FMODE_APPEND) {
	ptr->pos = olen;
    }
    strio_extend(ptr, ptr->pos, 1);
    RSTRING_PTR(ptr->string)[ptr->pos++] = c;
    OBJ_INFECT(ptr->string, self);
    return ch;
}

/*
 * call-seq:
 *   strio.puts(obj, ...)    -> nil
 *
 * See IO#puts.
 */
#define strio_puts rb_io_puts

/*
 * call-seq:
 *   strio.read([length [, buffer]])    -> string, buffer, or nil
 *
 * See IO#read.
 */
static VALUE
strio_read(int argc, VALUE *argv, VALUE self)
{
    struct StringIO *ptr = readable(StringIO(self));
    VALUE str = Qnil;
    long len;
    int binary = 0;

    switch (argc) {
      case 2:
	str = argv[1];
	if (!NIL_P(str)) {
	    StringValue(str);
	    rb_str_modify(str);
	}
      case 1:
	if (!NIL_P(argv[0])) {
	    len = NUM2LONG(argv[0]);
	    if (len < 0) {
		rb_raise(rb_eArgError, "negative length %ld given", len);
	    }
	    if (len > 0 && ptr->pos >= RSTRING_LEN(ptr->string)) {
		if (!NIL_P(str)) rb_str_resize(str, 0);
		return Qnil;
	    }
	    binary = 1;
	    break;
	}
	/* fall through */
      case 0:
	len = RSTRING_LEN(ptr->string);
	if (len <= ptr->pos) {
	    if (NIL_P(str)) {
		str = rb_str_new(0, 0);
	    }
	    else {
		rb_str_resize(str, 0);
	    }
	    return str;
	}
	else {
	    len -= ptr->pos;
	}
	break;
      default:
	rb_raise(rb_eArgError, "wrong number of arguments (%d for 0)", argc);
    }
    if (NIL_P(str)) {
	str = strio_substr(ptr, ptr->pos, len);
	if (binary) rb_enc_associate(str, rb_ascii8bit_encoding());
    }
    else {
	long rest = RSTRING_LEN(ptr->string) - ptr->pos;
	if (len > rest) len = rest;
	rb_str_resize(str, len);
	MEMCPY(RSTRING_PTR(str), RSTRING_PTR(ptr->string) + ptr->pos, char, len);
	if (binary)
	    rb_enc_associate(str, rb_ascii8bit_encoding());
	else
	    rb_enc_copy(str, ptr->string);
    }
    ptr->pos += RSTRING_LEN(str);
    return str;
}

/*
 * call-seq:
 *   strio.sysread(integer[, outbuf])    -> string
 *
 * Similar to #read, but raises +EOFError+ at end of string instead of
 * returning +nil+, as well as IO#sysread does.
 */
static VALUE
strio_sysread(int argc, VALUE *argv, VALUE self)
{
    VALUE val = rb_funcall2(self, rb_intern("read"), argc, argv);
    if (NIL_P(val)) {
	rb_eof_error();
    }
    return val;
}

#define strio_syswrite rb_io_write

/*
 * call-seq:
 *   strio.isatty -> nil
 *   strio.tty? -> nil
 *
 */
#define strio_isatty strio_false

/* call-seq: strio.pid -> nil */
#define strio_pid strio_nil

/* call-seq: strio.fileno -> nil */
#define strio_fileno strio_nil

/*
 * call-seq:
 *   strio.size   -> integer
 *
 * Returns the size of the buffer string.
 */
static VALUE
strio_size(VALUE self)
{
    VALUE string = StringIO(self)->string;
    if (NIL_P(string)) {
	rb_raise(rb_eIOError, "not opened");
    }
    return ULONG2NUM(RSTRING_LEN(string));
}

/*
 * call-seq:
 *   strio.truncate(integer)    -> 0
 *
 * Truncates the buffer string to at most _integer_ bytes. The *strio*
 * must be opened for writing.
 */
static VALUE
strio_truncate(VALUE self, VALUE len)
{
    VALUE string = writable(StringIO(self))->string;
    long l = NUM2LONG(len);
    long plen = RSTRING_LEN(string);
    if (l < 0) {
	error_inval("negative length");
    }
    rb_str_resize(string, l);
    if (plen < l) {
	MEMZERO(RSTRING_PTR(string) + plen, char, l - plen);
    }
    return len;
}

/*
 *  call-seq:
 *     strio.external_encoding   => encoding
 *
 *  Returns the Encoding object that represents the encoding of the file.
 *  If strio is write mode and no encoding is specified, returns <code>nil</code>.
 */

static VALUE
strio_external_encoding(VALUE self)
{
    return rb_enc_from_encoding(rb_enc_get(StringIO(self)->string));
}

/*
 *  call-seq:
 *     strio.internal_encoding   => encoding
 *
 *  Returns the Encoding of the internal string if conversion is
 *  specified.  Otherwise returns nil.
 */

static VALUE
strio_internal_encoding(VALUE self)
{
     return Qnil;
}

/*
 *  call-seq:
 *     strio.set_encoding(ext_enc, [int_enc[, opt]])  => strio
 *
 *  Specify the encoding of the StringIO as <i>ext_enc</i>.
 *  Use the default external encoding if <i>ext_enc</i> is nil.
 *  2nd argument <i>int_enc</i> and optional hash <i>opt</i> argument
 *  are ignored; they are for API compatibility to IO.
 */

static VALUE
strio_set_encoding(int argc, VALUE *argv, VALUE self)
{
    rb_encoding* enc;
    VALUE str = StringIO(self)->string;
    VALUE ext_enc, int_enc, opt;

    argc = rb_scan_args(argc, argv, "11:", &ext_enc, &int_enc, &opt);

    if (NIL_P(ext_enc)) {
	enc = rb_default_external_encoding();
    }
    else {
	enc = rb_to_encoding(ext_enc);
    }
    rb_enc_associate(str, enc);
    return self;
}

/*
 * Pseudo I/O on String object.
 */
void
Init_stringio()
{
    VALUE StringIO = rb_define_class("StringIO", rb_cData);

    rb_include_module(StringIO, rb_mEnumerable);
    rb_define_alloc_func(StringIO, strio_s_allocate);
    rb_define_singleton_method(StringIO, "open", strio_s_open, -1);
    rb_define_method(StringIO, "initialize", strio_initialize, -1);
    rb_define_method(StringIO, "initialize_copy", strio_copy, 1);
    rb_define_method(StringIO, "reopen", strio_reopen, -1);

    rb_define_method(StringIO, "string", strio_get_string, 0);
    rb_define_method(StringIO, "string=", strio_set_string, 1);
    rb_define_method(StringIO, "lineno", strio_get_lineno, 0);
    rb_define_method(StringIO, "lineno=", strio_set_lineno, 1);

    rb_define_method(StringIO, "binmode", strio_binmode, 0);
    rb_define_method(StringIO, "close", strio_close, 0);
    rb_define_method(StringIO, "close_read", strio_close_read, 0);
    rb_define_method(StringIO, "close_write", strio_close_write, 0);
    rb_define_method(StringIO, "closed?", strio_closed, 0);
    rb_define_method(StringIO, "closed_read?", strio_closed_read, 0);
    rb_define_method(StringIO, "closed_write?", strio_closed_write, 0);
    rb_define_method(StringIO, "eof", strio_eof, 0);
    rb_define_method(StringIO, "eof?", strio_eof, 0);
    rb_define_method(StringIO, "fcntl", strio_fcntl, -1);
    rb_define_method(StringIO, "flush", strio_flush, 0);
    rb_define_method(StringIO, "fsync", strio_fsync, 0);
    rb_define_method(StringIO, "pos", strio_get_pos, 0);
    rb_define_method(StringIO, "pos=", strio_set_pos, 1);
    rb_define_method(StringIO, "rewind", strio_rewind, 0);
    rb_define_method(StringIO, "seek", strio_seek, -1);
    rb_define_method(StringIO, "sync", strio_get_sync, 0);
    rb_define_method(StringIO, "sync=", strio_set_sync, 1);
    rb_define_method(StringIO, "tell", strio_tell, 0);

    rb_define_method(StringIO, "each", strio_each, -1);
    rb_define_method(StringIO, "each_line", strio_each, -1);
    rb_define_method(StringIO, "lines", strio_each, -1);
    rb_define_method(StringIO, "each_byte", strio_each_byte, 0);
    rb_define_method(StringIO, "bytes", strio_each_byte, 0);
    rb_define_method(StringIO, "each_char", strio_each_char, 0);
    rb_define_method(StringIO, "chars", strio_each_char, 0);
    rb_define_method(StringIO, "each_codepoint", strio_each_codepoint, 0);
    rb_define_method(StringIO, "codepoints", strio_each_codepoint, 0);
    rb_define_method(StringIO, "getc", strio_getc, 0);
    rb_define_method(StringIO, "ungetc", strio_ungetc, 1);
    rb_define_method(StringIO, "ungetbyte", strio_ungetbyte, 1);
    rb_define_method(StringIO, "getbyte", strio_getbyte, 0);
    rb_define_method(StringIO, "gets", strio_gets, -1);
    rb_define_method(StringIO, "readlines", strio_readlines, -1);
    rb_define_method(StringIO, "read", strio_read, -1);

    rb_define_method(StringIO, "write", strio_write, 1);
    rb_define_method(StringIO, "putc", strio_putc, 1);

    rb_define_method(StringIO, "isatty", strio_isatty, 0);
    rb_define_method(StringIO, "tty?", strio_isatty, 0);
    rb_define_method(StringIO, "pid", strio_pid, 0);
    rb_define_method(StringIO, "fileno", strio_fileno, 0);
    rb_define_method(StringIO, "size", strio_size, 0);
    rb_define_method(StringIO, "length", strio_size, 0);
    rb_define_method(StringIO, "truncate", strio_truncate, 1);

    rb_define_method(StringIO, "external_encoding", strio_external_encoding, 0);
    rb_define_method(StringIO, "internal_encoding", strio_internal_encoding, 0);
    rb_define_method(StringIO, "set_encoding", strio_set_encoding, -1);

    {
	VALUE mReadable = rb_define_module_under(rb_cIO, "readable");
	rb_define_method(mReadable, "readchar", strio_readchar, 0);
	rb_define_method(mReadable, "readbyte", strio_readbyte, 0);
	rb_define_method(mReadable, "readline", strio_readline, -1);
	rb_define_method(mReadable, "sysread", strio_sysread, -1);
	rb_define_method(mReadable, "readpartial", strio_sysread, -1);
	rb_define_method(mReadable, "read_nonblock", strio_sysread, -1);
	rb_include_module(StringIO, mReadable);
    }
    {
	VALUE mWritable = rb_define_module_under(rb_cIO, "writable");
	rb_define_method(mWritable, "<<", strio_addstr, 1);
	rb_define_method(mWritable, "print", strio_print, -1);
	rb_define_method(mWritable, "printf", strio_printf, -1);
	rb_define_method(mWritable, "puts", strio_puts, -1);
	rb_define_method(mWritable, "syswrite", strio_syswrite, 1);
	rb_define_method(mWritable, "write_nonblock", strio_syswrite, 1);
	rb_include_module(StringIO, mWritable);
    }
}
