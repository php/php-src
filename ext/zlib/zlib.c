/*
 * zlib.c - An interface for zlib.
 *
 *   Copyright (C) UENO Katsuhiro 2000-2003
 *
 * $Id$
 */

#include <ruby.h>
#include <zlib.h>
#include <time.h>
#include <ruby/io.h>

#ifdef HAVE_VALGRIND_MEMCHECK_H
# include <valgrind/memcheck.h>
# ifndef VALGRIND_MAKE_MEM_DEFINED
#  define VALGRIND_MAKE_MEM_DEFINED(p, n) VALGRIND_MAKE_READABLE((p), (n))
# endif
# ifndef VALGRIND_MAKE_MEM_UNDEFINED
#  define VALGRIND_MAKE_MEM_UNDEFINED(p, n) VALGRIND_MAKE_WRITABLE((p), (n))
# endif
#else
# define VALGRIND_MAKE_MEM_DEFINED(p, n) /* empty */
# define VALGRIND_MAKE_MEM_UNDEFINED(p, n) /* empty */
#endif

#define RUBY_ZLIB_VERSION  "0.6.0"


#define OBJ_IS_FREED(val)  (RBASIC(val)->flags == 0)

#ifndef GZIP_SUPPORT
#define GZIP_SUPPORT  1
#endif

/* from zutil.h */
#ifndef DEF_MEM_LEVEL
#if MAX_MEM_LEVEL >= 8
#define DEF_MEM_LEVEL  8
#else
#define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif
#endif

#if SIZEOF_LONG > SIZEOF_INT
static inline uInt
max_uint(long n)
{
    if (n > UINT_MAX) n = UINT_MAX;
    return (uInt)n;
}
#define MAX_UINT(n) max_uint(n)
#else
#define MAX_UINT(n) (uInt)(n)
#endif

#define sizeof(x) ((int)sizeof(x))

static ID id_dictionaries;

/*--------- Prototypes --------*/

static NORETURN(void raise_zlib_error(int, const char*));
static VALUE rb_zlib_version(VALUE);
static VALUE do_checksum(int, VALUE*, uLong (*)(uLong, const Bytef*, uInt));
static VALUE rb_zlib_adler32(int, VALUE*, VALUE);
static VALUE rb_zlib_crc32(int, VALUE*, VALUE);
static VALUE rb_zlib_crc_table(VALUE);
static voidpf zlib_mem_alloc(voidpf, uInt, uInt);
static void zlib_mem_free(voidpf, voidpf);
static void finalizer_warn(const char*);

struct zstream;
struct zstream_funcs;
static void zstream_init(struct zstream*, const struct zstream_funcs*);
static void zstream_expand_buffer(struct zstream*);
static void zstream_expand_buffer_into(struct zstream*, unsigned long);
static void zstream_append_buffer(struct zstream*, const Bytef*, long);
static VALUE zstream_detach_buffer(struct zstream*);
static VALUE zstream_shift_buffer(struct zstream*, long);
static void zstream_buffer_ungets(struct zstream*, const Bytef*, unsigned long);
static void zstream_buffer_ungetbyte(struct zstream*, int);
static void zstream_append_input(struct zstream*, const Bytef*, long);
static void zstream_discard_input(struct zstream*, long);
static void zstream_reset_input(struct zstream*);
static void zstream_passthrough_input(struct zstream*);
static VALUE zstream_detach_input(struct zstream*);
static void zstream_reset(struct zstream*);
static VALUE zstream_end(struct zstream*);
static void zstream_run(struct zstream*, Bytef*, long, int);
static VALUE zstream_sync(struct zstream*, Bytef*, long);
static void zstream_mark(struct zstream*);
static void zstream_free(struct zstream*);
static VALUE zstream_new(VALUE, const struct zstream_funcs*);
static struct zstream *get_zstream(VALUE);
static void zstream_finalize(struct zstream*);

static VALUE rb_zstream_end(VALUE);
static VALUE rb_zstream_reset(VALUE);
static VALUE rb_zstream_finish(VALUE);
static VALUE rb_zstream_flush_next_in(VALUE);
static VALUE rb_zstream_flush_next_out(VALUE);
static VALUE rb_zstream_avail_out(VALUE);
static VALUE rb_zstream_set_avail_out(VALUE, VALUE);
static VALUE rb_zstream_avail_in(VALUE);
static VALUE rb_zstream_total_in(VALUE);
static VALUE rb_zstream_total_out(VALUE);
static VALUE rb_zstream_data_type(VALUE);
static VALUE rb_zstream_adler(VALUE);
static VALUE rb_zstream_finished_p(VALUE);
static VALUE rb_zstream_closed_p(VALUE);

static VALUE rb_deflate_s_allocate(VALUE);
static VALUE rb_deflate_initialize(int, VALUE*, VALUE);
static VALUE rb_deflate_init_copy(VALUE, VALUE);
static VALUE deflate_run(VALUE);
static VALUE rb_deflate_s_deflate(int, VALUE*, VALUE);
static void do_deflate(struct zstream*, VALUE, int);
static VALUE rb_deflate_deflate(int, VALUE*, VALUE);
static VALUE rb_deflate_addstr(VALUE, VALUE);
static VALUE rb_deflate_flush(int, VALUE*, VALUE);
static VALUE rb_deflate_params(VALUE, VALUE, VALUE);
static VALUE rb_deflate_set_dictionary(VALUE, VALUE);

static VALUE inflate_run(VALUE);
static VALUE rb_inflate_s_allocate(VALUE);
static VALUE rb_inflate_initialize(int, VALUE*, VALUE);
static VALUE rb_inflate_s_inflate(VALUE, VALUE);
static void do_inflate(struct zstream*, VALUE);
static VALUE rb_inflate_inflate(VALUE, VALUE);
static VALUE rb_inflate_addstr(VALUE, VALUE);
static VALUE rb_inflate_sync(VALUE, VALUE);
static VALUE rb_inflate_sync_point_p(VALUE);
static VALUE rb_inflate_set_dictionary(VALUE, VALUE);

#if GZIP_SUPPORT
struct gzfile;
static void gzfile_mark(struct gzfile*);
static void gzfile_free(struct gzfile*);
static VALUE gzfile_new(VALUE, const struct zstream_funcs*, void (*) _((struct gzfile*)));
static void gzfile_reset(struct gzfile*);
static void gzfile_close(struct gzfile*, int);
static void gzfile_write_raw(struct gzfile*);
static VALUE gzfile_read_raw_partial(VALUE);
static VALUE gzfile_read_raw_rescue(VALUE);
static VALUE gzfile_read_raw(struct gzfile*);
static int gzfile_read_raw_ensure(struct gzfile*, long);
static char *gzfile_read_raw_until_zero(struct gzfile*, long);
static unsigned int gzfile_get16(const unsigned char*);
static unsigned long gzfile_get32(const unsigned char*);
static void gzfile_set32(unsigned long n, unsigned char*);
static void gzfile_make_header(struct gzfile*);
static void gzfile_make_footer(struct gzfile*);
static void gzfile_read_header(struct gzfile*);
static void gzfile_check_footer(struct gzfile*);
static void gzfile_write(struct gzfile*, Bytef*, long);
static long gzfile_read_more(struct gzfile*);
static void gzfile_calc_crc(struct gzfile*, VALUE);
static VALUE gzfile_read(struct gzfile*, long);
static VALUE gzfile_read_all(struct gzfile*);
static void gzfile_ungets(struct gzfile*, const Bytef*, long);
static void gzfile_ungetbyte(struct gzfile*, int);
static VALUE gzfile_writer_end_run(VALUE);
static void gzfile_writer_end(struct gzfile*);
static VALUE gzfile_reader_end_run(VALUE);
static void gzfile_reader_end(struct gzfile*);
static void gzfile_reader_rewind(struct gzfile*);
static VALUE gzfile_reader_get_unused(struct gzfile*);
static struct gzfile *get_gzfile(VALUE);
static VALUE gzfile_ensure_close(VALUE);
static VALUE rb_gzfile_s_wrap(int, VALUE*, VALUE);
static VALUE gzfile_s_open(int, VALUE*, VALUE, const char*);
NORETURN(static void gzfile_raise(struct gzfile *, VALUE, const char *));
static VALUE gzfile_error_inspect(VALUE);

static VALUE rb_gzfile_to_io(VALUE);
static VALUE rb_gzfile_crc(VALUE);
static VALUE rb_gzfile_mtime(VALUE);
static VALUE rb_gzfile_level(VALUE);
static VALUE rb_gzfile_os_code(VALUE);
static VALUE rb_gzfile_orig_name(VALUE);
static VALUE rb_gzfile_comment(VALUE);
static VALUE rb_gzfile_lineno(VALUE);
static VALUE rb_gzfile_set_lineno(VALUE, VALUE);
static VALUE rb_gzfile_set_mtime(VALUE, VALUE);
static VALUE rb_gzfile_set_orig_name(VALUE, VALUE);
static VALUE rb_gzfile_set_comment(VALUE, VALUE);
static VALUE rb_gzfile_close(VALUE);
static VALUE rb_gzfile_finish(VALUE);
static VALUE rb_gzfile_closed_p(VALUE);
static VALUE rb_gzfile_eof_p(VALUE);
static VALUE rb_gzfile_sync(VALUE);
static VALUE rb_gzfile_set_sync(VALUE, VALUE);
static VALUE rb_gzfile_total_in(VALUE);
static VALUE rb_gzfile_total_out(VALUE);
static VALUE rb_gzfile_path(VALUE);

static VALUE rb_gzwriter_s_allocate(VALUE);
static VALUE rb_gzwriter_s_open(int, VALUE*, VALUE);
static VALUE rb_gzwriter_initialize(int, VALUE*, VALUE);
static VALUE rb_gzwriter_flush(int, VALUE*, VALUE);
static VALUE rb_gzwriter_write(VALUE, VALUE);
static VALUE rb_gzwriter_putc(VALUE, VALUE);

static VALUE rb_gzreader_s_allocate(VALUE);
static VALUE rb_gzreader_s_open(int, VALUE*, VALUE);
static VALUE rb_gzreader_initialize(int, VALUE*, VALUE);
static VALUE rb_gzreader_rewind(VALUE);
static VALUE rb_gzreader_unused(VALUE);
static VALUE rb_gzreader_read(int, VALUE*, VALUE);
static VALUE rb_gzreader_getc(VALUE);
static VALUE rb_gzreader_readchar(VALUE);
static VALUE rb_gzreader_each_byte(VALUE);
static VALUE rb_gzreader_ungetc(VALUE, VALUE);
static VALUE rb_gzreader_ungetbyte(VALUE, VALUE);
static void gzreader_skip_linebreaks(struct gzfile*);
static VALUE gzreader_gets(int, VALUE*, VALUE);
static VALUE rb_gzreader_gets(int, VALUE*, VALUE);
static VALUE rb_gzreader_readline(int, VALUE*, VALUE);
static VALUE rb_gzreader_each(int, VALUE*, VALUE);
static VALUE rb_gzreader_readlines(int, VALUE*, VALUE);
#endif /* GZIP_SUPPORT */

/*
 * Document-module: Zlib
 *
 * This module provides access to the {zlib library}[http://zlib.net]. Zlib is
 * designed to be a portable, free, general-purpose, legally unencumbered --
 * that is, not covered by any patents -- lossless data-compression library
 * for use on virtually any computer hardware and operating system.
 *
 * The zlib compression library provides in-memory compression and
 * decompression functions, including integrity checks of the uncompressed 
 * data.
 *
 * The zlib compressed data format is described in RFC 1950, which is a
 * wrapper around a deflate stream which is described in RFC 1951.
 *
 * The library also supports reading and writing files in gzip (.gz) format 
 * with an interface similar to that of IO. The gzip format is described in
 * RFC 1952 which is also a wrapper around a deflate stream.
 *
 * The zlib format was designed to be compact and fast for use in memory and on
 * communications channels. The gzip format was designed for single-file
 * compression on file systems, has a larger header than zlib to maintain 
 * directory information, and uses a different, slower check method than zlib.
 *
 * See your system's zlib.h for further information about zlib
 *
 * == Sample usage
 *
 * Using the wrapper to compress strings with default parameters is quite
 * simple: 
 *
 *   require "zlib"
 *
 *   data_to_compress = File.read("don_quixote.txt")
 *   
 *   puts "Input size: #{data_to_compress.size}"
 *   #=> Input size: 2347740
 *   
 *   data_compressed = Zlib::Deflate.deflate(data_to_compress)
 *
 *   puts "Compressed size: #{data_compressed.size}" 
 *   #=> Compressed size: 887238
 *
 *   uncompressed_data = Zlib::Inflate.inflate(data_compressed)
 *
 *   puts "Uncompressed data is: #{uncompressed_data}"
 *   #=> Uncompressed data is: The Project Gutenberg EBook of Don Quixote...
 *
 * == Class tree
 *
 * - Zlib::Deflate
 * - Zlib::Inflate
 * - Zlib::ZStream
 * - Zlib::Error
 *   - Zlib::StreamEnd
 *   - Zlib::NeedDict
 *   - Zlib::DataError
 *   - Zlib::StreamError
 *   - Zlib::MemError
 *   - Zlib::BufError
 *   - Zlib::VersionError
 *
 * (if you have GZIP_SUPPORT)
 * - Zlib::GzipReader
 * - Zlib::GzipWriter
 * - Zlib::GzipFile
 * - Zlib::GzipFile::Error
 *   - Zlib::GzipFile::LengthError
 *   - Zlib::GzipFile::CRCError
 *   - Zlib::GzipFile::NoFooter
 *
 */
void Init_zlib(void);

/*--------- Exceptions --------*/

static VALUE cZError, cStreamEnd, cNeedDict;
static VALUE cStreamError, cDataError, cMemError, cBufError, cVersionError;

static void
raise_zlib_error(int err, const char *msg)
{
    VALUE exc;

    if (!msg) {
	msg = zError(err);
    }

    switch(err) {
      case Z_STREAM_END:
	exc = rb_exc_new2(cStreamEnd, msg);
	break;
      case Z_NEED_DICT:
	exc = rb_exc_new2(cNeedDict, msg);
	break;
      case Z_STREAM_ERROR:
	exc = rb_exc_new2(cStreamError, msg);
	break;
      case Z_DATA_ERROR:
	exc = rb_exc_new2(cDataError, msg);
	break;
      case Z_BUF_ERROR:
	exc = rb_exc_new2(cBufError, msg);
	break;
      case Z_VERSION_ERROR:
	exc = rb_exc_new2(cVersionError, msg);
	break;
      case Z_MEM_ERROR:
	exc = rb_exc_new2(cMemError, msg);
	break;
      case Z_ERRNO:
	rb_sys_fail(msg);
	/* no return */
      default:
      {
	  char buf[BUFSIZ];
	  snprintf(buf, BUFSIZ, "unknown zlib error %d: %s", err, msg);
	  exc = rb_exc_new2(cZError, buf);
      }
    }

    rb_exc_raise(exc);
}


/*--- Warning (in finalizer) ---*/

static void
finalizer_warn(const char *msg)
{
    fprintf(stderr, "zlib(finalizer): %s\n", msg);
}


/*-------- module Zlib --------*/

/*
 * Document-method: Zlib.zlib_version
 *
 * Returns the string which represents the version of zlib library.
 */
static VALUE
rb_zlib_version(VALUE klass)
{
    VALUE str;

    str = rb_str_new2(zlibVersion());
    OBJ_TAINT(str);  /* for safe */
    return str;
}

#if SIZEOF_LONG > SIZEOF_INT
static uLong
checksum_long(uLong (*func)(uLong, const Bytef*, uInt), uLong sum, const Bytef *ptr, long len)
{
    if (len > UINT_MAX) {
	do {
	    sum = func(sum, ptr, UINT_MAX);
	    ptr += UINT_MAX;
	    len -= UINT_MAX;
	} while (len >= UINT_MAX);
    }
    if (len > 0) sum = func(sum, ptr, (uInt)len);
    return sum;
}
#else
#define checksum_long(func, sum, ptr, len) (func)((sum), (ptr), (len))
#endif

static VALUE
do_checksum(argc, argv, func)
    int argc;
    VALUE *argv;
    uLong (*func)(uLong, const Bytef*, uInt);
{
    VALUE str, vsum;
    unsigned long sum;

    rb_scan_args(argc, argv, "02", &str, &vsum);

    if (!NIL_P(vsum)) {
	sum = NUM2ULONG(vsum);
    }
    else if (NIL_P(str)) {
	sum = 0;
    }
    else {
	sum = func(0, Z_NULL, 0);
    }

    if (NIL_P(str)) {
	sum = func(sum, Z_NULL, 0);
    }
    else {
	StringValue(str);
	sum = checksum_long(func, sum, (Bytef*)RSTRING_PTR(str), RSTRING_LEN(str));
    }
    return rb_uint2inum(sum);
}

/*
 * Document-method: Zlib.adler32
 *
 * call-seq: Zlib.adler32(string, adler)
 *
 * Calculates Adler-32 checksum for +string+, and returns updated value of
 * +adler+. If +string+ is omitted, it returns the Adler-32 initial value. If
 * +adler+ is omitted, it assumes that the initial value is given to +adler+.
 *
 * FIXME: expression.
 */
static VALUE
rb_zlib_adler32(int argc, VALUE *argv, VALUE klass)
{
    return do_checksum(argc, argv, adler32);
}

#ifdef HAVE_ADLER32_COMBINE
/*
 * Document-method: Zlib.adler32_combine
 *
 * call-seq: Zlib.adler32_combine(adler1, adler2, len2)
 *
 * Combine two Adler-32 check values in to one.  +alder1+ is the first Adler-32
 * value, +adler2+ is the second Adler-32 value.  +len2+ is the length of the
 * string used to generate +adler2+.
 *
 */
static VALUE
rb_zlib_adler32_combine(VALUE klass, VALUE adler1, VALUE adler2, VALUE len2)
{
  return ULONG2NUM(
	adler32_combine(NUM2ULONG(adler1), NUM2ULONG(adler2), NUM2LONG(len2)));
}
#else
#define rb_zlib_adler32_combine rb_f_notimplement
#endif

/*
 * Document-method: Zlib.crc32
 *
 * call-seq: Zlib.crc32(string, adler)
 *
 * Calculates CRC checksum for +string+, and returns updated value of +crc+. If
 * +string+ is omitted, it returns the CRC initial value. If +crc+ is omitted, it
 * assumes that the initial value is given to +crc+.
 *
 * FIXME: expression.
 */
static VALUE
rb_zlib_crc32(int argc, VALUE *argv, VALUE klass)
{
    return do_checksum(argc, argv, crc32);
}

#ifdef HAVE_CRC32_COMBINE
/*
 * Document-method: Zlib.crc32_combine
 *
 * call-seq: Zlib.crc32_combine(crc1, crc2, len2)
 *
 * Combine two CRC-32 check values in to one.  +crc1+ is the first CRC-32
 * value, +crc2+ is the second CRC-32 value.  +len2+ is the length of the
 * string used to generate +crc2+.
 *
 */
static VALUE
rb_zlib_crc32_combine(VALUE klass, VALUE crc1, VALUE crc2, VALUE len2)
{
  return ULONG2NUM(
	crc32_combine(NUM2ULONG(crc1), NUM2ULONG(crc2), NUM2LONG(len2)));
}
#else
#define rb_zlib_crc32_combine rb_f_notimplement
#endif

/*
 * Document-method: Zlib.crc_table
 *
 * Returns the table for calculating CRC checksum as an array.
 */
static VALUE
rb_zlib_crc_table(VALUE obj)
{
    const unsigned long *crctbl;
    VALUE dst;
    int i;

    crctbl = get_crc_table();
    dst = rb_ary_new2(256);

    for (i = 0; i < 256; i++) {
	rb_ary_push(dst, rb_uint2inum(crctbl[i]));
    }
    return dst;
}



/*-------- zstream - internal APIs --------*/

struct zstream {
    unsigned long flags;
    VALUE buf;
    long buf_filled;
    VALUE input;
    z_stream stream;
    const struct zstream_funcs {
	int (*reset)(z_streamp);
	int (*end)(z_streamp);
	int (*run)(z_streamp, int);
    } *func;
};

#define ZSTREAM_FLAG_READY      0x1
#define ZSTREAM_FLAG_IN_STREAM  0x2
#define ZSTREAM_FLAG_FINISHED   0x4
#define ZSTREAM_FLAG_CLOSING    0x8
#define ZSTREAM_FLAG_UNUSED     0x10

#define ZSTREAM_READY(z)       ((z)->flags |= ZSTREAM_FLAG_READY)
#define ZSTREAM_IS_READY(z)    ((z)->flags & ZSTREAM_FLAG_READY)
#define ZSTREAM_IS_FINISHED(z) ((z)->flags & ZSTREAM_FLAG_FINISHED)
#define ZSTREAM_IS_CLOSING(z)  ((z)->flags & ZSTREAM_FLAG_CLOSING)

/* I think that more better value should be found,
   but I gave up finding it. B) */
#define ZSTREAM_INITIAL_BUFSIZE       1024
#define ZSTREAM_AVAIL_OUT_STEP_MAX   16384
#define ZSTREAM_AVAIL_OUT_STEP_MIN    2048

static const struct zstream_funcs deflate_funcs = {
    deflateReset, deflateEnd, deflate,
};

static const struct zstream_funcs inflate_funcs = {
    inflateReset, inflateEnd, inflate,
};


static voidpf
zlib_mem_alloc(voidpf opaque, uInt items, uInt size)
{
    voidpf p = xmalloc(items * size);
    /* zlib FAQ: Valgrind (or some similar memory access checker) says that
       deflate is performing a conditional jump that depends on an
       uninitialized value.  Isn't that a bug?
       http://www.zlib.net/zlib_faq.html#faq36 */
    VALGRIND_MAKE_MEM_DEFINED(p, items * size);
    return p;
}

static void
zlib_mem_free(voidpf opaque, voidpf address)
{
    xfree(address);
}

static void
zstream_init(struct zstream *z, const struct zstream_funcs *func)
{
    z->flags = 0;
    z->buf = Qnil;
    z->buf_filled = 0;
    z->input = Qnil;
    z->stream.zalloc = zlib_mem_alloc;
    z->stream.zfree = zlib_mem_free;
    z->stream.opaque = Z_NULL;
    z->stream.msg = Z_NULL;
    z->stream.next_in = Z_NULL;
    z->stream.avail_in = 0;
    z->stream.next_out = Z_NULL;
    z->stream.avail_out = 0;
    z->func = func;
}

#define zstream_init_deflate(z)   zstream_init((z), &deflate_funcs)
#define zstream_init_inflate(z)   zstream_init((z), &inflate_funcs)

static void
zstream_expand_buffer(struct zstream *z)
{
    long inc;

    if (NIL_P(z->buf)) {
	    /* I uses rb_str_new here not rb_str_buf_new because
	       rb_str_buf_new makes a zero-length string. */
	z->buf = rb_str_new(0, ZSTREAM_INITIAL_BUFSIZE);
	z->buf_filled = 0;
	z->stream.next_out = (Bytef*)RSTRING_PTR(z->buf);
	z->stream.avail_out = ZSTREAM_INITIAL_BUFSIZE;
	RBASIC(z->buf)->klass = 0;
	return;
    }

    if (RSTRING_LEN(z->buf) - z->buf_filled >= ZSTREAM_AVAIL_OUT_STEP_MAX) {
	/* to keep other threads from freezing */
	z->stream.avail_out = ZSTREAM_AVAIL_OUT_STEP_MAX;
    }
    else {
	inc = z->buf_filled / 2;
	if (inc < ZSTREAM_AVAIL_OUT_STEP_MIN) {
	    inc = ZSTREAM_AVAIL_OUT_STEP_MIN;
	}
	rb_str_resize(z->buf, z->buf_filled + inc);
	z->stream.avail_out = (inc < ZSTREAM_AVAIL_OUT_STEP_MAX) ?
	    (int)inc : ZSTREAM_AVAIL_OUT_STEP_MAX;
    }
    z->stream.next_out = (Bytef*)RSTRING_PTR(z->buf) + z->buf_filled;
}

static void
zstream_expand_buffer_into(struct zstream *z, unsigned long size)
{
    if (NIL_P(z->buf)) {
	/* I uses rb_str_new here not rb_str_buf_new because
	   rb_str_buf_new makes a zero-length string. */
	z->buf = rb_str_new(0, size);
	z->buf_filled = 0;
	z->stream.next_out = (Bytef*)RSTRING_PTR(z->buf);
	z->stream.avail_out = MAX_UINT(size);
	RBASIC(z->buf)->klass = 0;
    }
    else if (z->stream.avail_out != size) {
	rb_str_resize(z->buf, z->buf_filled + size);
	z->stream.next_out = (Bytef*)RSTRING_PTR(z->buf) + z->buf_filled;
	z->stream.avail_out = MAX_UINT(size);
    }
}

static void
zstream_append_buffer(struct zstream *z, const Bytef *src, long len)
{
    if (NIL_P(z->buf)) {
	z->buf = rb_str_buf_new(len);
	rb_str_buf_cat(z->buf, (const char*)src, len);
	z->buf_filled = len;
	z->stream.next_out = (Bytef*)RSTRING_PTR(z->buf);
	z->stream.avail_out = 0;
	RBASIC(z->buf)->klass = 0;
	return;
    }

    if (RSTRING_LEN(z->buf) < z->buf_filled + len) {
	rb_str_resize(z->buf, z->buf_filled + len);
	z->stream.avail_out = 0;
    }
    else {
	if (z->stream.avail_out >= (uInt)len) {
	    z->stream.avail_out -= (uInt)len;
	}
	else {
	    z->stream.avail_out = 0;
	}
    }
    memcpy(RSTRING_PTR(z->buf) + z->buf_filled, src, len);
    z->buf_filled += len;
    z->stream.next_out = (Bytef*)RSTRING_PTR(z->buf) + z->buf_filled;
}

#define zstream_append_buffer2(z,v) \
    zstream_append_buffer((z),(Bytef*)RSTRING_PTR(v),RSTRING_LEN(v))

static VALUE
zstream_detach_buffer(struct zstream *z)
{
    VALUE dst;

    if (NIL_P(z->buf)) {
	dst = rb_str_new(0, 0);
    }
    else {
	dst = z->buf;
	rb_str_resize(dst, z->buf_filled);
	RBASIC(dst)->klass = rb_cString;
    }

    z->buf = Qnil;
    z->buf_filled = 0;
    z->stream.next_out = 0;
    z->stream.avail_out = 0;
    return dst;
}

static VALUE
zstream_shift_buffer(struct zstream *z, long len)
{
    VALUE dst;
    long buflen;

    if (z->buf_filled <= len) {
	return zstream_detach_buffer(z);
    }

    dst = rb_str_subseq(z->buf, 0, len);
    RBASIC(dst)->klass = rb_cString;
    z->buf_filled -= len;
    memmove(RSTRING_PTR(z->buf), RSTRING_PTR(z->buf) + len,
	    z->buf_filled);
    z->stream.next_out = (Bytef*)RSTRING_PTR(z->buf) + z->buf_filled;
    buflen = RSTRING_LEN(z->buf) - z->buf_filled;
    if (buflen > ZSTREAM_AVAIL_OUT_STEP_MAX) {
	buflen = ZSTREAM_AVAIL_OUT_STEP_MAX;
    }
    z->stream.avail_out = (uInt)buflen;

    return dst;
}

static void
zstream_buffer_ungets(struct zstream *z, const Bytef *b, unsigned long len)
{
    if (NIL_P(z->buf) || RSTRING_LEN(z->buf) - z->buf_filled == 0) {
	zstream_expand_buffer_into(z, len);
    }

    memmove(RSTRING_PTR(z->buf) + len, RSTRING_PTR(z->buf), z->buf_filled);
    memmove(RSTRING_PTR(z->buf), b, len);
    z->buf_filled+=len;
    if (z->stream.avail_out > 0) {
	if (len > z->stream.avail_out) len = z->stream.avail_out;
	z->stream.next_out+=len;
	z->stream.avail_out-=(uInt)len;
    }
}

static void
zstream_buffer_ungetbyte(struct zstream *z, int c)
{
    if (NIL_P(z->buf) || RSTRING_LEN(z->buf) - z->buf_filled == 0) {
	zstream_expand_buffer(z);
    }

    memmove(RSTRING_PTR(z->buf) + 1, RSTRING_PTR(z->buf), z->buf_filled);
    RSTRING_PTR(z->buf)[0] = (char)c;
    z->buf_filled++;
    if (z->stream.avail_out > 0) {
	z->stream.next_out++;
	z->stream.avail_out--;
    }
}

static void
zstream_append_input(struct zstream *z, const Bytef *src, long len)
{
    if (len <= 0) return;

    if (NIL_P(z->input)) {
	z->input = rb_str_buf_new(len);
	rb_str_buf_cat(z->input, (const char*)src, len);
	RBASIC(z->input)->klass = 0;
    }
    else {
	rb_str_buf_cat(z->input, (const char*)src, len);
    }
}

#define zstream_append_input2(z,v)\
    RB_GC_GUARD(v),\
    zstream_append_input((z), (Bytef*)RSTRING_PTR(v), RSTRING_LEN(v))

static void
zstream_discard_input(struct zstream *z, long len)
{
    if (NIL_P(z->input) || RSTRING_LEN(z->input) <= len) {
	z->input = Qnil;
    }
    else {
	memmove(RSTRING_PTR(z->input), RSTRING_PTR(z->input) + len,
		RSTRING_LEN(z->input) - len);
	rb_str_resize(z->input, RSTRING_LEN(z->input) - len);
    }
}

static void
zstream_reset_input(struct zstream *z)
{
    z->input = Qnil;
}

static void
zstream_passthrough_input(struct zstream *z)
{
    if (!NIL_P(z->input)) {
	zstream_append_buffer2(z, z->input);
	z->input = Qnil;
    }
}

static VALUE
zstream_detach_input(struct zstream *z)
{
    VALUE dst;

    if (NIL_P(z->input)) {
	dst = rb_str_new(0, 0);
    }
    else {
	dst = z->input;
	RBASIC(dst)->klass = rb_cString;
    }
    z->input = Qnil;
    RBASIC(dst)->klass = rb_cString;
    return dst;
}

static void
zstream_reset(struct zstream *z)
{
    int err;

    err = z->func->reset(&z->stream);
    if (err != Z_OK) {
	raise_zlib_error(err, z->stream.msg);
    }
    z->flags = ZSTREAM_FLAG_READY;
    z->buf = Qnil;
    z->buf_filled = 0;
    z->stream.next_out = 0;
    z->stream.avail_out = 0;
    zstream_reset_input(z);
}

static VALUE
zstream_end(struct zstream *z)
{
    int err;

    if (!ZSTREAM_IS_READY(z)) {
	rb_warning("attempt to close uninitialized zstream; ignored.");
	return Qnil;
    }
    if (z->flags & ZSTREAM_FLAG_IN_STREAM) {
	rb_warning("attempt to close unfinished zstream; reset forced.");
	zstream_reset(z);
    }

    zstream_reset_input(z);
    err = z->func->end(&z->stream);
    if (err != Z_OK) {
	raise_zlib_error(err, z->stream.msg);
    }
    z->flags = 0;
    return Qnil;
}

static void
zstream_run(struct zstream *z, Bytef *src, long len, int flush)
{
    uInt n;
    int err;
    volatile VALUE guard = Qnil;

    if (NIL_P(z->input) && len == 0) {
	z->stream.next_in = (Bytef*)"";
	z->stream.avail_in = 0;
    }
    else {
	zstream_append_input(z, src, len);
	z->stream.next_in = (Bytef*)RSTRING_PTR(z->input);
	z->stream.avail_in = MAX_UINT(RSTRING_LEN(z->input));
	/* keep reference to `z->input' so as not to be garbage collected
	   after zstream_reset_input() and prevent `z->stream.next_in'
	   from dangling. */
	guard = z->input;
    }

    if (z->stream.avail_out == 0) {
	zstream_expand_buffer(z);
    }

    for (;;) {
	/* VC allocates err and guard to same address.  accessing err and guard
	   in same scope prevents it. */
	RB_GC_GUARD(guard);
	n = z->stream.avail_out;
	err = z->func->run(&z->stream, flush);
	z->buf_filled += n - z->stream.avail_out;
	rb_thread_schedule();

	if (err == Z_STREAM_END) {
	    z->flags &= ~ZSTREAM_FLAG_IN_STREAM;
	    z->flags |= ZSTREAM_FLAG_FINISHED;
	    break;
	}
	if (err != Z_OK) {
	    if (flush != Z_FINISH && err == Z_BUF_ERROR
		&& z->stream.avail_out > 0) {
		z->flags |= ZSTREAM_FLAG_IN_STREAM;
		break;
	    }
	    zstream_reset_input(z);
	    if (z->stream.avail_in > 0) {
		zstream_append_input(z, z->stream.next_in, z->stream.avail_in);
	    }
	    if (err == Z_NEED_DICT) {
		VALUE self = (VALUE)z->stream.opaque;
		VALUE dicts = rb_ivar_get(self, id_dictionaries);
		VALUE dict = rb_hash_aref(dicts, rb_uint2inum(z->stream.adler));
		if (!NIL_P(dict)) {
		    rb_inflate_set_dictionary(self, dict);
		    continue;
		}
	    }
	    raise_zlib_error(err, z->stream.msg);
	}
	if (z->stream.avail_out > 0) {
	    z->flags |= ZSTREAM_FLAG_IN_STREAM;
	    break;
	}
	zstream_expand_buffer(z);
    }

    zstream_reset_input(z);
    if (z->stream.avail_in > 0) {
	zstream_append_input(z, z->stream.next_in, z->stream.avail_in);
        guard = Qnil; /* prevent tail call to make guard effective */
    }
}

static VALUE
zstream_sync(struct zstream *z, Bytef *src, long len)
{
    /* VALUE rest; */
    int err;

    if (!NIL_P(z->input)) {
	z->stream.next_in = (Bytef*)RSTRING_PTR(z->input);
	z->stream.avail_in = MAX_UINT(RSTRING_LEN(z->input));
	err = inflateSync(&z->stream);
	if (err == Z_OK) {
	    zstream_discard_input(z,
				  RSTRING_LEN(z->input) - z->stream.avail_in);
	    zstream_append_input(z, src, len);
	    return Qtrue;
	}
	zstream_reset_input(z);
	if (err != Z_DATA_ERROR) {
	    /* rest = rb_str_new((char*)z->stream.next_in, z->stream.avail_in); */
	    raise_zlib_error(err, z->stream.msg);
	}
    }

    if (len <= 0) return Qfalse;

    z->stream.next_in = src;
    z->stream.avail_in = MAX_UINT(len);
    err = inflateSync(&z->stream);
    if (err == Z_OK) {
	zstream_append_input(z, z->stream.next_in, z->stream.avail_in);
	return Qtrue;
    }
    if (err != Z_DATA_ERROR) {
	/* rest = rb_str_new((char*)z->stream.next_in, z->stream.avail_in); */
	raise_zlib_error(err, z->stream.msg);
    }
    return Qfalse;
}

static void
zstream_mark(struct zstream *z)
{
    rb_gc_mark(z->buf);
    rb_gc_mark(z->input);
}

static void
zstream_finalize(struct zstream *z)
{
    int err = z->func->end(&z->stream);
    if (err == Z_STREAM_ERROR)
	finalizer_warn("the stream state was inconsistent.");
    if (err == Z_DATA_ERROR)
	finalizer_warn("the stream was freed prematurely.");
}

static void
zstream_free(struct zstream *z)
{
    if (ZSTREAM_IS_READY(z)) {
	zstream_finalize(z);
    }
    xfree(z);
}

static VALUE
zstream_new(VALUE klass, const struct zstream_funcs *funcs)
{
    VALUE obj;
    struct zstream *z;

    obj = Data_Make_Struct(klass, struct zstream,
			   zstream_mark, zstream_free, z);
    zstream_init(z, funcs);
    z->stream.opaque = (voidpf)obj;
    return obj;
}

#define zstream_deflate_new(klass)  zstream_new((klass), &deflate_funcs)
#define zstream_inflate_new(klass)  zstream_new((klass), &inflate_funcs)

static struct zstream *
get_zstream(VALUE obj)
{
    struct zstream *z;

    Data_Get_Struct(obj, struct zstream, z);
    if (!ZSTREAM_IS_READY(z)) {
	rb_raise(cZError, "stream is not ready");
    }
    return z;
}


/* ------------------------------------------------------------------------- */

/*
 * Document-class: Zlib::ZStream
 *
 * Zlib::ZStream is the abstract class for the stream which handles the
 * compressed data. The operations are defined in the subclasses:
 * Zlib::Deflate for compression, and Zlib::Inflate for decompression.
 *
 * An instance of Zlib::ZStream has one stream (struct zstream in the source)
 * and two variable-length buffers which associated to the input (next_in) of
 * the stream and the output (next_out) of the stream. In this document,
 * "input buffer" means the buffer for input, and "output buffer" means the
 * buffer for output.
 *
 * Data input into an instance of Zlib::ZStream are temporally stored into
 * the end of input buffer, and then data in input buffer are processed from
 * the beginning of the buffer until no more output from the stream is
 * produced (i.e. until avail_out > 0 after processing).  During processing,
 * output buffer is allocated and expanded automatically to hold all output
 * data.
 *
 * Some particular instance methods consume the data in output buffer and
 * return them as a String.
 *
 * Here is an ascii art for describing above:
 *
 *    +================ an instance of Zlib::ZStream ================+
 *    ||                                                            ||
 *    ||     +--------+          +-------+          +--------+      ||
 *    ||  +--| output |<---------|zstream|<---------| input  |<--+  ||
 *    ||  |  | buffer |  next_out+-------+next_in   | buffer |   |  ||
 *    ||  |  +--------+                             +--------+   |  ||
 *    ||  |                                                      |  ||
 *    +===|======================================================|===+
 *        |                                                      |
 *        v                                                      |
 *    "output data"                                         "input data"
 *
 * If an error occurs during processing input buffer, an exception which is a
 * subclass of Zlib::Error is raised.  At that time, both input and output
 * buffer keep their conditions at the time when the error occurs.
 *
 * == Method Catalogue
 *
 * Many of the methods in this class are fairly low-level and unlikely to be
 * of interest to users.  In fact, users are unlikely to use this class
 * directly; rather they will be interested in Zlib::Inflate and
 * Zlib::Deflate.
 *
 * The higher level methods are listed below.
 *
 * - #total_in
 * - #total_out
 * - #data_type
 * - #adler
 * - #reset
 * - #finish
 * - #finished?
 * - #close
 * - #closed?
 */

/*
 * Closes the stream. All operations on the closed stream will raise an
 * exception.
 */
static VALUE
rb_zstream_end(VALUE obj)
{
    zstream_end(get_zstream(obj));
    return Qnil;
}

/*
 * Resets and initializes the stream. All data in both input and output buffer
 * are discarded.
 */
static VALUE
rb_zstream_reset(VALUE obj)
{
    zstream_reset(get_zstream(obj));
    return Qnil;
}

/*
 * Finishes the stream and flushes output buffer. See Zlib::Deflate#finish and
 * Zlib::Inflate#finish for details of this behavior.
 */
static VALUE
rb_zstream_finish(VALUE obj)
{
    struct zstream *z = get_zstream(obj);
    VALUE dst;

    zstream_run(z, (Bytef*)"", 0, Z_FINISH);
    dst = zstream_detach_buffer(z);

    OBJ_INFECT(dst, obj);
    return dst;
}

/*
 * Flushes input buffer and returns all data in that buffer.
 */
static VALUE
rb_zstream_flush_next_in(VALUE obj)
{
    struct zstream *z;
    VALUE dst;

    Data_Get_Struct(obj, struct zstream, z);
    dst = zstream_detach_input(z);
    OBJ_INFECT(dst, obj);
    return dst;
}

/*
 * Flushes output buffer and returns all data in that buffer.
 */
static VALUE
rb_zstream_flush_next_out(VALUE obj)
{
    struct zstream *z;
    VALUE dst;

    Data_Get_Struct(obj, struct zstream, z);
    dst = zstream_detach_buffer(z);
    OBJ_INFECT(dst, obj);
    return dst;
}

/*
 * Returns number of bytes of free spaces in output buffer.  Because the free
 * space is allocated automatically, this method returns 0 normally.
 */
static VALUE
rb_zstream_avail_out(VALUE obj)
{
    struct zstream *z;
    Data_Get_Struct(obj, struct zstream, z);
    return rb_uint2inum(z->stream.avail_out);
}

/*
 * Allocates +size+ bytes of free space in the output buffer. If there are more
 * than +size+ bytes already in the buffer, the buffer is truncated. Because
 * free space is allocated automatically, you usually don't need to use this
 * method.
 */
static VALUE
rb_zstream_set_avail_out(VALUE obj, VALUE size)
{
    struct zstream *z = get_zstream(obj);

    Check_Type(size, T_FIXNUM);
    zstream_expand_buffer_into(z, FIX2INT(size));
    return size;
}

/*
 * Returns bytes of data in the input buffer. Normally, returns 0.
 */
static VALUE
rb_zstream_avail_in(VALUE obj)
{
    struct zstream *z;
    Data_Get_Struct(obj, struct zstream, z);
    return INT2FIX(NIL_P(z->input) ? 0 : (int)(RSTRING_LEN(z->input)));
}

/*
 * Returns the total bytes of the input data to the stream.  FIXME
 */
static VALUE
rb_zstream_total_in(VALUE obj)
{
    return rb_uint2inum(get_zstream(obj)->stream.total_in);
}

/*
 * Returns the total bytes of the output data from the stream.  FIXME
 */
static VALUE
rb_zstream_total_out(VALUE obj)
{
    return rb_uint2inum(get_zstream(obj)->stream.total_out);
}

/*
 * Guesses the type of the data which have been inputed into the stream. The
 * returned value is either <tt>BINARY</tt>, <tt>ASCII</tt>, or
 * <tt>UNKNOWN</tt>.
 */
static VALUE
rb_zstream_data_type(VALUE obj)
{
    return INT2FIX(get_zstream(obj)->stream.data_type);
}

/*
 * Returns the adler-32 checksum.
 */
static VALUE
rb_zstream_adler(VALUE obj)
{
	return rb_uint2inum(get_zstream(obj)->stream.adler);
}

/*
 * Returns true if the stream is finished.
 */
static VALUE
rb_zstream_finished_p(VALUE obj)
{
    return ZSTREAM_IS_FINISHED(get_zstream(obj)) ? Qtrue : Qfalse;
}

/*
 * Returns true if the stream is closed.
 */
static VALUE
rb_zstream_closed_p(VALUE obj)
{
    struct zstream *z;
    Data_Get_Struct(obj, struct zstream, z);
    return ZSTREAM_IS_READY(z) ? Qfalse : Qtrue;
}


/* ------------------------------------------------------------------------- */

/*
 * Document-class: Zlib::Deflate
 *
 * Zlib::Deflate is the class for compressing data.  See Zlib::ZStream for more
 * information.
 */

#define FIXNUMARG(val, ifnil) \
    (NIL_P((val)) ? (ifnil) \
    : ((void)Check_Type((val), T_FIXNUM), FIX2INT((val))))

#define ARG_LEVEL(val)     FIXNUMARG((val), Z_DEFAULT_COMPRESSION)
#define ARG_WBITS(val)     FIXNUMARG((val), MAX_WBITS)
#define ARG_MEMLEVEL(val)  FIXNUMARG((val), DEF_MEM_LEVEL)
#define ARG_STRATEGY(val)  FIXNUMARG((val), Z_DEFAULT_STRATEGY)
#define ARG_FLUSH(val)     FIXNUMARG((val), Z_NO_FLUSH)


static VALUE
rb_deflate_s_allocate(VALUE klass)
{
    return zstream_deflate_new(klass);
}

/*
 * Document-method: Zlib::Deflate.new
 *
 * call-seq:
 *   Zlib::Deflate.new(level=DEFAULT_COMPRESSION, window_bits=MAX_WBITS, mem_level=DEF_MEM_LEVEL, strategy=DEFAULT_STRATEGY)
 *
 * Creates a new deflate stream for compression. If a given argument is nil,
 * the default value of that argument is used.
 *
 * The +level+ sets the compression level for the deflate stream between 0 (no
 * compression) and 9 (best compression. The following constants have been
 * defined to make code more readable:
 *   
 * * Zlib::NO_COMPRESSION = 0
 * * Zlib::BEST_SPEED =  1
 * * Zlib::DEFAULT_COMPRESSION = 6
 * * Zlib::BEST_COMPRESSION = 9
 *   
 * The +window_bits+ sets the size of the history buffer and should be between
 * 8 and 15.  Larger values of this parameter result in better compression at
 * the expense of memory usage.
 *
 * The +mem_level+ specifies how much memory should be allocated for the
 * internal compression state.  1 uses minimum memory but is slow and reduces
 * compression ratio while 9 uses maximum memory for optimal speed.  The
 * default value is 8. Two constants are defined:
 *
 * * Zlib::DEF_MEM_LEVEL
 * * Zlib::MAX_MEM_LEVEL
 *
 * The +strategy+ sets the deflate compression strategy.  The following
 * strategies are available:
 *
 * Zlib::DEFAULT_STRATEGY:: For normal data
 * Zlib::FILTERED:: For data produced by a filter or predictor
 * Zlib::FIXED:: Prevents dynamic Huffman codes
 * Zlib::HUFFMAN_ONLY:: Prevents string matching
 * Zlib::RLE:: Designed for better compression of PNG image data
 *
 * See the constants for further description.
 *
 * == Examples
 *
 * === Basic
 *
 *   open "compressed.file", "w+" do |io|
 *     io << Zlib::Deflate.new.deflate(File.read("big.file"))
 *   end
 *
 * === Custom compression
 *
 *   open "compressed.file", "w+" do |compressed_io|
 *     deflate = Zlib::Deflate.new(Zlib::BEST_COMPRESSION,
 *                                 Zlib::MAX_WBITS,
 *                                 Zlib::MAX_MEM_LEVEL,
 *                                 Zlib::HUFFMAN_ONLY)
 *
 *     begin
 *       open "big.file" do |big_io|
 *         until big_io.eof? do
 *           compressed_io << zd.deflate(big_io.read(16384))
 *         end
 *       end
 *     ensure
 *       deflate.close
 *     end
 *   end
 *
 * While this example will work, for best optimization review the flags for
 * your specific time, memory usage and output space requirements.
 */
static VALUE
rb_deflate_initialize(int argc, VALUE *argv, VALUE obj)
{
    struct zstream *z;
    VALUE level, wbits, memlevel, strategy;
    int err;

    rb_scan_args(argc, argv, "04", &level, &wbits, &memlevel, &strategy);
    Data_Get_Struct(obj, struct zstream, z);

    err = deflateInit2(&z->stream, ARG_LEVEL(level), Z_DEFLATED,
		       ARG_WBITS(wbits), ARG_MEMLEVEL(memlevel),
		       ARG_STRATEGY(strategy));
    if (err != Z_OK) {
	raise_zlib_error(err, z->stream.msg);
    }
    ZSTREAM_READY(z);

    return obj;
}

/*
 * Document-method: Zlib::Deflate#initialize_copy
 *
 * Duplicates the deflate stream.
 */
static VALUE
rb_deflate_init_copy(VALUE self, VALUE orig)
{
    struct zstream *z1, *z2;
    int err;

    Data_Get_Struct(self, struct zstream, z1);
    z2 = get_zstream(orig);

    err = deflateCopy(&z1->stream, &z2->stream);
    if (err != Z_OK) {
	raise_zlib_error(err, 0);
    }
    z1->input = NIL_P(z2->input) ? Qnil : rb_str_dup(z2->input);
    z1->buf   = NIL_P(z2->buf)   ? Qnil : rb_str_dup(z2->buf);
    z1->buf_filled = z2->buf_filled;
    z1->flags = z2->flags;

    return self;
}

static VALUE
deflate_run(VALUE args)
{
    struct zstream *z = (struct zstream*)((VALUE*)args)[0];
    VALUE src = ((VALUE*)args)[1];

    zstream_run(z, (Bytef*)RSTRING_PTR(src), RSTRING_LEN(src), Z_FINISH);
    return zstream_detach_buffer(z);
}

/*
 * Document-method: Zlib::Deflate.deflate
 *
 * call-seq: Zlib.deflate(string[, level])
 *           Zlib::Deflate.deflate(string[, level])
 *
 * Compresses the given +string+. Valid values of level are
 * <tt>NO_COMPRESSION</tt>, <tt>BEST_SPEED</tt>,
 * <tt>BEST_COMPRESSION</tt>, <tt>DEFAULT_COMPRESSION</tt>, and an
 * integer from 0 to 9 (the default is 6).
 *
 * This method is almost equivalent to the following code:
 *
 *   def deflate(string, level)
 *     z = Zlib::Deflate.new(level)
 *     dst = z.deflate(string, Zlib::NO_FLUSH)
 *     z.close
 *     dst
 *   end
 *
 * See also Zlib.inflate
 *
 */
static VALUE
rb_deflate_s_deflate(int argc, VALUE *argv, VALUE klass)
{
    struct zstream z;
    VALUE src, level, dst, args[2];
    int err, lev;

    rb_scan_args(argc, argv, "11", &src, &level);

    lev = ARG_LEVEL(level);
    StringValue(src);
    zstream_init_deflate(&z);
    err = deflateInit(&z.stream, lev);
    if (err != Z_OK) {
	raise_zlib_error(err, z.stream.msg);
    }
    ZSTREAM_READY(&z);

    args[0] = (VALUE)&z;
    args[1] = src;
    dst = rb_ensure(deflate_run, (VALUE)args, zstream_end, (VALUE)&z);

    OBJ_INFECT(dst, src);
    return dst;
}

static void
do_deflate(struct zstream *z, VALUE src, int flush)
{
    if (NIL_P(src)) {
	zstream_run(z, (Bytef*)"", 0, Z_FINISH);
	return;
    }
    StringValue(src);
    if (flush != Z_NO_FLUSH || RSTRING_LEN(src) > 0) { /* prevent BUF_ERROR */
	zstream_run(z, (Bytef*)RSTRING_PTR(src), RSTRING_LEN(src), flush);
    }
}

/*
 * Document-method: Zlib#deflate
 *
 * call-seq:
 *   deflate(string, flush = Zlib::NO_FLUSH)
 *
 * Inputs +string+ into the deflate stream and returns the output from the
 * stream.  On calling this method, both the input and the output buffers of
 * the stream are flushed.
 *
 * If +string+ is nil, this method finishes the stream, just like
 * Zlib::ZStream#finish.
 *
 * The +flush+ parameter specifies the flush mode.  The following constants
 * may be used:
 *
 * Zlib::NO_FLUSH:: The default
 * Zlib::SYNC_FLUSH:: Flushes the output to a byte boundary
 * Zlib::FULL_FLUSH:: SYNC_FLUSH + resets the compression state
 * Zlib::FINISH:: Pending input is processed, pending output is flushed.
 *
 * See the constants for further description.
 *
 */
static VALUE
rb_deflate_deflate(int argc, VALUE *argv, VALUE obj)
{
    struct zstream *z = get_zstream(obj);
    VALUE src, flush, dst;

    rb_scan_args(argc, argv, "11", &src, &flush);
    OBJ_INFECT(obj, src);
    do_deflate(z, src, ARG_FLUSH(flush));
    dst = zstream_detach_buffer(z);

    OBJ_INFECT(dst, obj);
    return dst;
}

/*
 * Document-method: Zlib::Deflate#<<
 *
 * call-seq: << string
 *
 * Inputs +string+ into the deflate stream just like Zlib::Deflate#deflate, but
 * returns the Zlib::Deflate object itself.  The output from the stream is
 * preserved in output buffer.
 */
static VALUE
rb_deflate_addstr(VALUE obj, VALUE src)
{
    OBJ_INFECT(obj, src);
    do_deflate(get_zstream(obj), src, Z_NO_FLUSH);
    return obj;
}

/*
 * Document-method: Zlib::Deflate#flush
 *
 * call-seq:
 *   flush(flush = Zlib::SYNC_FLUSH)
 *
 * This method is equivalent to <tt>deflate('', flush)</tt>. This method is
 * just provided to improve the readability of your Ruby program.
 *
 * See Zlib::Deflate#deflate for detail on the +flush+ constants NO_FLUSH,
 * SYNC_FLUSH, FULL_FLUSH and FINISH.
 */
static VALUE
rb_deflate_flush(int argc, VALUE *argv, VALUE obj)
{
    struct zstream *z = get_zstream(obj);
    VALUE v_flush, dst;
    int flush;

    rb_scan_args(argc, argv, "01", &v_flush);
    flush = FIXNUMARG(v_flush, Z_SYNC_FLUSH);
    if (flush != Z_NO_FLUSH) {  /* prevent Z_BUF_ERROR */
	zstream_run(z, (Bytef*)"", 0, flush);
    }
    dst = zstream_detach_buffer(z);

    OBJ_INFECT(dst, obj);
    return dst;
}

/*
 * Document-method: Zlib::Deflate.params
 *
 * call-seq: params(level, strategy)
 *
 * Changes the parameters of the deflate stream to allow changes between
 * different types of data that require different types of compression.  Any
 * unprocessed data is flushed before changing the params.
 *
 * See Zlib::Deflate.new for a description of +level+ and +strategy+.
 *
 */
static VALUE
rb_deflate_params(VALUE obj, VALUE v_level, VALUE v_strategy)
{
    struct zstream *z = get_zstream(obj);
    int level, strategy;
    int err;
    uInt n;

    level = ARG_LEVEL(v_level);
    strategy = ARG_STRATEGY(v_strategy);

    n = z->stream.avail_out;
    err = deflateParams(&z->stream, level, strategy);
    z->buf_filled += n - z->stream.avail_out;
    while (err == Z_BUF_ERROR) {
	rb_warning("deflateParams() returned Z_BUF_ERROR");
	zstream_expand_buffer(z);
	n = z->stream.avail_out;
	err = deflateParams(&z->stream, level, strategy);
	z->buf_filled += n - z->stream.avail_out;
    }
    if (err != Z_OK) {
	raise_zlib_error(err, z->stream.msg);
    }

    return Qnil;
}

/*
 * Document-method: Zlib::Deflate.set_dictionary
 *
 * call-seq: set_dictionary(string)
 *
 * Sets the preset dictionary and returns +string+. This method is available
 * just only after Zlib::Deflate.new or Zlib::ZStream#reset method was called.
 * See zlib.h for details.
 *
 * Can raise errors of Z_STREAM_ERROR if a parameter is invalid (such as
 * NULL dictionary) or the stream state is inconsistent, Z_DATA_ERROR if
 * the given dictionary doesn't match the expected one (incorrect adler32 value)
 *
 */
static VALUE
rb_deflate_set_dictionary(VALUE obj, VALUE dic)
{
    struct zstream *z = get_zstream(obj);
    VALUE src = dic;
    int err;

    OBJ_INFECT(obj, dic);
    StringValue(src);
    err = deflateSetDictionary(&z->stream,
			       (Bytef*)RSTRING_PTR(src), RSTRING_LENINT(src));
    if (err != Z_OK) {
	raise_zlib_error(err, z->stream.msg);
    }

    return dic;
}


/* ------------------------------------------------------------------------- */

/*
 * Document-class: Zlib::Inflate
 *
 * Zlib:Inflate is the class for decompressing compressed data.  Unlike
 * Zlib::Deflate, an instance of this class is not able to duplicate (clone,
 * dup) itself.
 */

static VALUE
rb_inflate_s_allocate(VALUE klass)
{
    VALUE inflate = zstream_inflate_new(klass);
    rb_ivar_set(inflate, id_dictionaries, rb_hash_new());
    return inflate;
}

/*
 * Document-method: Zlib::Inflate.new
 *
 * call-seq:
 *   Zlib::Inflate.new(window_bits = Zlib::MAX_WBITS)
 *
 * Creates a new inflate stream for decompression.  +window_bits+ sets the
 * size of the history buffer and can have the following values:
 *
 * 0::
 *   Have inflate use the window size from the zlib header of the compressed
 *   stream.
 *
 * (8..15)
 *   Overrides the window size of the inflate header in the compressed stream.
 *   The window size must be greater than or equal to the window size of the
 *   compressed stream.
 *
 * Greater than 15::
 *   Add 32 to window_bits to enable zlib and gzip decoding with automatic
 *   header detection, or add 16 to decode only the gzip format (a
 *   Zlib::DataError will be raised for a non-gzip stream). 
 *
 * (-8..-15)::
 *   Enables raw deflate mode which will not generate a check value, and will
 *   not look for any check values for comparison at the end of the stream.
 *
 *   This is for use with other formats that use the deflate compressed data
 *   format such as zip which provide their own check values.
 *
 * == Example
 *
 *   open "compressed.file" do |compressed_io|
 *     inflate = Zlib::Inflate.new(Zlib::MAX_WBITS + 32)
 *
 *     begin
 *       open "uncompressed.file", "w+" do |uncompressed_io|
 *         uncompressed_io << zi.inflate(compressed_io.read)
 *       }
 *     ensure
 *       zi.close
 *     end
 *   end
 *
 */
static VALUE
rb_inflate_initialize(int argc, VALUE *argv, VALUE obj)
{
    struct zstream *z;
    VALUE wbits;
    int err;

    rb_scan_args(argc, argv, "01", &wbits);
    Data_Get_Struct(obj, struct zstream, z);

    err = inflateInit2(&z->stream, ARG_WBITS(wbits));
    if (err != Z_OK) {
	raise_zlib_error(err, z->stream.msg);
    }
    ZSTREAM_READY(z);

    return obj;
}

static VALUE
inflate_run(VALUE args)
{
    struct zstream *z = (struct zstream*)((VALUE*)args)[0];
    VALUE src = ((VALUE*)args)[1];

    zstream_run(z, (Bytef*)RSTRING_PTR(src), RSTRING_LEN(src), Z_SYNC_FLUSH);
    zstream_run(z, (Bytef*)"", 0, Z_FINISH);  /* for checking errors */
    return zstream_detach_buffer(z);
}

/*
 * Document-method: Zlib::Inflate.inflate
 *
 * call-seq: Zlib::Inflate.inflate(string)
 *
 * Decompresses +string+. Raises a Zlib::NeedDict exception if a preset
 * dictionary is needed for decompression.
 *
 * This method is almost equivalent to the following code:
 *
 *   def inflate(string)
 *     zstream = Zlib::Inflate.new
 *     buf = zstream.inflate(string)
 *     zstream.finish
 *     zstream.close
 *     buf
 *   end
 *
 * See also Zlib.deflate
 *
 */
static VALUE
rb_inflate_s_inflate(VALUE obj, VALUE src)
{
    struct zstream z;
    VALUE dst, args[2];
    int err;

    StringValue(src);
    zstream_init_inflate(&z);
    err = inflateInit(&z.stream);
    if (err != Z_OK) {
	raise_zlib_error(err, z.stream.msg);
    }
    ZSTREAM_READY(&z);

    args[0] = (VALUE)&z;
    args[1] = src;
    dst = rb_ensure(inflate_run, (VALUE)args, zstream_end, (VALUE)&z);

    OBJ_INFECT(dst, src);
    return dst;
}

static void
do_inflate(struct zstream *z, VALUE src)
{
    if (NIL_P(src)) {
	zstream_run(z, (Bytef*)"", 0, Z_FINISH);
	return;
    }
    StringValue(src);
    if (RSTRING_LEN(src) > 0 || z->stream.avail_in > 0) { /* prevent Z_BUF_ERROR */
	zstream_run(z, (Bytef*)RSTRING_PTR(src), RSTRING_LEN(src), Z_SYNC_FLUSH);
    }
}

/* Document-method: Zlib::Inflate#add_dictionary
 *
 * call-seq: add_dictionary(string)
 *
 * Provide the inflate stream with a dictionary that may be required in the
 * future.  Multiple dictionaries may be provided.  The inflate stream will
 * automatically choose the correct user-provided dictionary based on the
 * stream's required dictionary.
 */
static VALUE
rb_inflate_add_dictionary(VALUE obj, VALUE dictionary) {
    VALUE dictionaries = rb_ivar_get(obj, id_dictionaries);
    VALUE checksum = do_checksum(1, &dictionary, adler32);

    rb_hash_aset(dictionaries, checksum, dictionary);

    return obj;
}

/*
 * Document-method: Zlib::Inflate#inflate
 *
 * call-seq: inflate(string)
 *
 * Inputs +string+ into the inflate stream and returns the output from the
 * stream.  Calling this method, both the input and the output buffer of the
 * stream are flushed.  If string is +nil+, this method finishes the stream,
 * just like Zlib::ZStream#finish.
 *
 * Raises a Zlib::NeedDict exception if a preset dictionary is needed to
 * decompress.  Set the dictionary by Zlib::Inflate#set_dictionary and then
 * call this method again with an empty string to flush the stream:
 *
 *   inflater = Zlib::Inflate.new
 *
 *   begin
 *     out = inflater.inflate compressed
 *   rescue Zlib::NeedDict
 *     # ensure the dictionary matches the stream's required dictionary
 *     raise unless inflater.adler == Zlib.adler32(dictionary)
 *
 *     inflater.set_dictionary dictionary
 *     inflater.inflate ''
 *   end
 *
 *   # ...
 *
 *   inflater.close
 *
 * See also Zlib::Inflate.new
 */
static VALUE
rb_inflate_inflate(VALUE obj, VALUE src)
{
    struct zstream *z = get_zstream(obj);
    VALUE dst;

    OBJ_INFECT(obj, src);

    if (ZSTREAM_IS_FINISHED(z)) {
	if (NIL_P(src)) {
	    dst = zstream_detach_buffer(z);
	}
	else {
	    StringValue(src);
	    zstream_append_buffer2(z, src);
	    dst = rb_str_new(0, 0);
	}
    }
    else {
	do_inflate(z, src);
	dst = zstream_detach_buffer(z);
	if (ZSTREAM_IS_FINISHED(z)) {
	    zstream_passthrough_input(z);
	}
    }

    OBJ_INFECT(dst, obj);
    return dst;
}

/*
 * call-seq: << string
 *
 * Inputs +string+ into the inflate stream just like Zlib::Inflate#inflate, but
 * returns the Zlib::Inflate object itself.  The output from the stream is
 * preserved in output buffer.
 */
static VALUE
rb_inflate_addstr(VALUE obj, VALUE src)
{
    struct zstream *z = get_zstream(obj);

    OBJ_INFECT(obj, src);

    if (ZSTREAM_IS_FINISHED(z)) {
	if (!NIL_P(src)) {
	    StringValue(src);
	    zstream_append_buffer2(z, src);
	}
    }
    else {
	do_inflate(z, src);
	if (ZSTREAM_IS_FINISHED(z)) {
	    zstream_passthrough_input(z);
	}
    }

    return obj;
}

/*
 * call-seq: sync(string)
 *
 * Inputs +string+ into the end of input buffer and skips data until a full
 * flush point can be found.  If the point is found in the buffer, this method
 * flushes the buffer and returns false.  Otherwise it returns +true+ and the
 * following data of full flush point is preserved in the buffer.
 */
static VALUE
rb_inflate_sync(VALUE obj, VALUE src)
{
    struct zstream *z = get_zstream(obj);

    OBJ_INFECT(obj, src);
    StringValue(src);
    return zstream_sync(z, (Bytef*)RSTRING_PTR(src), RSTRING_LEN(src));
}

/*
 * Quoted verbatim from original documentation:
 *
 *   What is this?
 *
 * <tt>:)</tt>
 */
static VALUE
rb_inflate_sync_point_p(VALUE obj)
{
    struct zstream *z = get_zstream(obj);
    int err;

    err = inflateSyncPoint(&z->stream);
    if (err == 1) {
	return Qtrue;
    }
    if (err != Z_OK) {
	raise_zlib_error(err, z->stream.msg);
    }
    return Qfalse;
}

/*
 * Document-method: Zlib::Inflate#set_dictionary
 *
 * Sets the preset dictionary and returns +string+.  This method is available just
 * only after a Zlib::NeedDict exception was raised.  See zlib.h for details.
 *
 */
static VALUE
rb_inflate_set_dictionary(VALUE obj, VALUE dic)
{
    struct zstream *z = get_zstream(obj);
    VALUE src = dic;
    int err;

    OBJ_INFECT(obj, dic);
    StringValue(src);
    err = inflateSetDictionary(&z->stream,
			       (Bytef*)RSTRING_PTR(src), RSTRING_LENINT(src));
    if (err != Z_OK) {
	raise_zlib_error(err, z->stream.msg);
    }

    return dic;
}



#if GZIP_SUPPORT

/* NOTE: Features for gzip files of Ruby/zlib are written from scratch
 *       and using undocumented feature of zlib, negative wbits.
 *       I don't think gzFile APIs of zlib are good for Ruby.
 */

/*------- .gz file header --------*/

#define GZ_MAGIC1             0x1f
#define GZ_MAGIC2             0x8b
#define GZ_METHOD_DEFLATE     8
#define GZ_FLAG_MULTIPART     0x2
#define GZ_FLAG_EXTRA         0x4
#define GZ_FLAG_ORIG_NAME     0x8
#define GZ_FLAG_COMMENT       0x10
#define GZ_FLAG_ENCRYPT       0x20
#define GZ_FLAG_UNKNOWN_MASK  0xc0

#define GZ_EXTRAFLAG_FAST     0x4
#define GZ_EXTRAFLAG_SLOW     0x2

/* from zutil.h */
#define OS_MSDOS    0x00
#define OS_AMIGA    0x01
#define OS_VMS      0x02
#define OS_UNIX     0x03
#define OS_ATARI    0x05
#define OS_OS2      0x06
#define OS_MACOS    0x07
#define OS_TOPS20   0x0a
#define OS_WIN32    0x0b

#define OS_VMCMS    0x04
#define OS_ZSYSTEM  0x08
#define OS_CPM      0x09
#define OS_QDOS     0x0c
#define OS_RISCOS   0x0d
#define OS_UNKNOWN  0xff

#ifndef OS_CODE
#define OS_CODE  OS_UNIX
#endif

static ID id_write, id_read, id_readpartial, id_flush, id_seek, id_close, id_path, id_input;
static VALUE cGzError, cNoFooter, cCRCError, cLengthError;



/*-------- gzfile internal APIs --------*/

struct gzfile {
    struct zstream z;
    VALUE io;
    int level;
    time_t mtime;       /* for header */
    int os_code;        /* for header */
    VALUE orig_name;    /* for header; must be a String */
    VALUE comment;      /* for header; must be a String */
    unsigned long crc;
    int lineno;
    long ungetc;
    void (*end)(struct gzfile *);
    rb_encoding *enc;
    rb_encoding *enc2;
    rb_econv_t *ec;
    int ecflags;
    VALUE ecopts;
    char *cbuf;
    VALUE path;
};
#define GZFILE_CBUF_CAPA 10

#define GZFILE_FLAG_SYNC             ZSTREAM_FLAG_UNUSED
#define GZFILE_FLAG_HEADER_FINISHED  (ZSTREAM_FLAG_UNUSED << 1)
#define GZFILE_FLAG_FOOTER_FINISHED  (ZSTREAM_FLAG_UNUSED << 2)

#define GZFILE_IS_FINISHED(gz) \
    (ZSTREAM_IS_FINISHED(&(gz)->z) && (gz)->z.buf_filled == 0)

#define GZFILE_READ_SIZE  2048


static void
gzfile_mark(struct gzfile *gz)
{
    rb_gc_mark(gz->io);
    rb_gc_mark(gz->orig_name);
    rb_gc_mark(gz->comment);
    zstream_mark(&gz->z);
    rb_gc_mark(gz->ecopts);
    rb_gc_mark(gz->path);
}

static void
gzfile_free(struct gzfile *gz)
{
    struct zstream *z = &gz->z;

    if (ZSTREAM_IS_READY(z)) {
	if (z->func == &deflate_funcs) {
	    finalizer_warn("Zlib::GzipWriter object must be closed explicitly.");
	}
	zstream_finalize(z);
    }
    if (gz->cbuf) {
	xfree(gz->cbuf);
    }
    xfree(gz);
}

static VALUE
gzfile_new(klass, funcs, endfunc)
    VALUE klass;
    const struct zstream_funcs *funcs;
    void (*endfunc)(struct gzfile *);
{
    VALUE obj;
    struct gzfile *gz;

    obj = Data_Make_Struct(klass, struct gzfile, gzfile_mark, gzfile_free, gz);
    zstream_init(&gz->z, funcs);
    gz->io = Qnil;
    gz->level = 0;
    gz->mtime = 0;
    gz->os_code = OS_CODE;
    gz->orig_name = Qnil;
    gz->comment = Qnil;
    gz->crc = crc32(0, Z_NULL, 0);
    gz->lineno = 0;
    gz->ungetc = 0;
    gz->end = endfunc;
    gz->enc = rb_default_external_encoding();
    gz->enc2 = 0;
    gz->ec = NULL;
    gz->ecflags = 0;
    gz->ecopts = Qnil;
    gz->cbuf = 0;
    gz->path = Qnil;

    return obj;
}

#define gzfile_writer_new(gz) gzfile_new((gz),&deflate_funcs,gzfile_writer_end)
#define gzfile_reader_new(gz) gzfile_new((gz),&inflate_funcs,gzfile_reader_end)

static void
gzfile_reset(struct gzfile *gz)
{
    zstream_reset(&gz->z);
    gz->crc = crc32(0, Z_NULL, 0);
    gz->lineno = 0;
    gz->ungetc = 0;
    if (gz->ec) {
	rb_econv_close(gz->ec);
	gz->ec = rb_econv_open_opts(gz->enc2->name, gz->enc->name,
				    gz->ecflags, gz->ecopts);
    }
}

static void
gzfile_close(struct gzfile *gz, int closeflag)
{
    VALUE io = gz->io;

    gz->end(gz);
    gz->io = Qnil;
    gz->orig_name = Qnil;
    gz->comment = Qnil;
    if (closeflag && rb_respond_to(io, id_close)) {
	rb_funcall(io, id_close, 0);
    }
}

static void
gzfile_write_raw(struct gzfile *gz)
{
    VALUE str;

    if (gz->z.buf_filled > 0) {
	str = zstream_detach_buffer(&gz->z);
	OBJ_TAINT(str);  /* for safe */
	rb_funcall(gz->io, id_write, 1, str);
	if ((gz->z.flags & GZFILE_FLAG_SYNC)
	    && rb_respond_to(gz->io, id_flush))
	    rb_funcall(gz->io, id_flush, 0);
    }
}

static VALUE
gzfile_read_raw_partial(VALUE arg)
{
    struct gzfile *gz = (struct gzfile*)arg;
    VALUE str;

    str = rb_funcall(gz->io, id_readpartial, 1, INT2FIX(GZFILE_READ_SIZE));
    Check_Type(str, T_STRING);
    return str;
}

static VALUE
gzfile_read_raw_rescue(VALUE arg)
{
    struct gzfile *gz = (struct gzfile*)arg;
    VALUE str = Qnil;
    if (rb_obj_is_kind_of(rb_errinfo(), rb_eNoMethodError)) {
        str = rb_funcall(gz->io, id_read, 1, INT2FIX(GZFILE_READ_SIZE));
        if (!NIL_P(str)) {
            Check_Type(str, T_STRING);
        }
    }
    return str; /* return nil when EOFError */
}

static VALUE
gzfile_read_raw(struct gzfile *gz)
{
    return rb_rescue2(gzfile_read_raw_partial, (VALUE)gz,
                      gzfile_read_raw_rescue, (VALUE)gz,
                      rb_eEOFError, rb_eNoMethodError, (VALUE)0);
}

static int
gzfile_read_raw_ensure(struct gzfile *gz, long size)
{
    VALUE str;

    while (NIL_P(gz->z.input) || RSTRING_LEN(gz->z.input) < size) {
	str = gzfile_read_raw(gz);
	if (NIL_P(str)) return 0;
	zstream_append_input2(&gz->z, str);
    }
    return 1;
}

static char *
gzfile_read_raw_until_zero(struct gzfile *gz, long offset)
{
    VALUE str;
    char *p;

    for (;;) {
	p = memchr(RSTRING_PTR(gz->z.input) + offset, '\0',
		   RSTRING_LEN(gz->z.input) - offset);
	if (p) break;
	str = gzfile_read_raw(gz);
	if (NIL_P(str)) {
	    rb_raise(cGzError, "unexpected end of file");
	}
	offset = RSTRING_LEN(gz->z.input);
	zstream_append_input2(&gz->z, str);
    }
    return p;
}

static unsigned int
gzfile_get16(const unsigned char *src)
{
    unsigned int n;
    n  = *(src++) & 0xff;
    n |= (*(src++) & 0xff) << 8;
    return n;
}

static unsigned long
gzfile_get32(const unsigned char *src)
{
    unsigned long n;
    n  = *(src++) & 0xff;
    n |= (*(src++) & 0xff) << 8;
    n |= (*(src++) & 0xff) << 16;
    n |= (*(src++) & 0xffU) << 24;
    return n;
}

static void
gzfile_set32(unsigned long n, unsigned char *dst)
{
    *(dst++) = n & 0xff;
    *(dst++) = (n >> 8) & 0xff;
    *(dst++) = (n >> 16) & 0xff;
    *dst     = (n >> 24) & 0xff;
}

static void
gzfile_raise(struct gzfile *gz, VALUE klass, const char *message)
{
    VALUE exc = rb_exc_new2(klass, message);
    if (!NIL_P(gz->z.input)) {
	rb_ivar_set(exc, id_input, rb_str_resurrect(gz->z.input));
    }
    rb_exc_raise(exc);
}

/*
 * Document-method: Zlib::GzipFile::Error#inspect
 *
 * Constructs a String of the GzipFile Error
 */
static VALUE
gzfile_error_inspect(VALUE error)
{
    VALUE str = rb_call_super(0, 0);
    VALUE input = rb_attr_get(error, id_input);

    if (!NIL_P(input)) {
	rb_str_resize(str, RSTRING_LEN(str)-1);
	rb_str_cat2(str, ", input=");
	rb_str_append(str, rb_str_inspect(input));
	rb_str_cat2(str, ">");
    }
    return str;
}

static void
gzfile_make_header(struct gzfile *gz)
{
    Bytef buf[10];  /* the size of gzip header */
    unsigned char flags = 0, extraflags = 0;

    if (!NIL_P(gz->orig_name)) {
	flags |= GZ_FLAG_ORIG_NAME;
    }
    if (!NIL_P(gz->comment)) {
	flags |= GZ_FLAG_COMMENT;
    }
    if (gz->mtime == 0) {
	gz->mtime = time(0);
    }

    if (gz->level == Z_BEST_SPEED) {
	extraflags |= GZ_EXTRAFLAG_FAST;
    }
    else if (gz->level == Z_BEST_COMPRESSION) {
	extraflags |= GZ_EXTRAFLAG_SLOW;
    }

    buf[0] = GZ_MAGIC1;
    buf[1] = GZ_MAGIC2;
    buf[2] = GZ_METHOD_DEFLATE;
    buf[3] = flags;
    gzfile_set32((unsigned long)gz->mtime, &buf[4]);
    buf[8] = extraflags;
    buf[9] = gz->os_code;
    zstream_append_buffer(&gz->z, buf, sizeof(buf));

    if (!NIL_P(gz->orig_name)) {
	zstream_append_buffer2(&gz->z, gz->orig_name);
	zstream_append_buffer(&gz->z, (Bytef*)"\0", 1);
    }
    if (!NIL_P(gz->comment)) {
	zstream_append_buffer2(&gz->z, gz->comment);
	zstream_append_buffer(&gz->z, (Bytef*)"\0", 1);
    }

    gz->z.flags |= GZFILE_FLAG_HEADER_FINISHED;
}

static void
gzfile_make_footer(struct gzfile *gz)
{
    Bytef buf[8];  /* 8 is the size of gzip footer */

    gzfile_set32(gz->crc, buf);
    gzfile_set32(gz->z.stream.total_in, &buf[4]);
    zstream_append_buffer(&gz->z, buf, sizeof(buf));
    gz->z.flags |= GZFILE_FLAG_FOOTER_FINISHED;
}

static void
gzfile_read_header(struct gzfile *gz)
{
    const unsigned char *head;
    long len;
    char flags, *p;

    if (!gzfile_read_raw_ensure(gz, 10)) {  /* 10 is the size of gzip header */
	gzfile_raise(gz, cGzError, "not in gzip format");
    }

    head = (unsigned char*)RSTRING_PTR(gz->z.input);

    if (head[0] != GZ_MAGIC1 || head[1] != GZ_MAGIC2) {
	gzfile_raise(gz, cGzError, "not in gzip format");
    }
    if (head[2] != GZ_METHOD_DEFLATE) {
	rb_raise(cGzError, "unsupported compression method %d", head[2]);
    }

    flags = head[3];
    if (flags & GZ_FLAG_MULTIPART) {
	rb_raise(cGzError, "multi-part gzip file is not supported");
    }
    else if (flags & GZ_FLAG_ENCRYPT) {
	rb_raise(cGzError, "encrypted gzip file is not supported");
    }
    else if (flags & GZ_FLAG_UNKNOWN_MASK) {
	rb_raise(cGzError, "unknown flags 0x%02x", flags);
    }

    if (head[8] & GZ_EXTRAFLAG_FAST) {
	gz->level = Z_BEST_SPEED;
    }
    else if (head[8] & GZ_EXTRAFLAG_SLOW) {
	gz->level = Z_BEST_COMPRESSION;
    }
    else {
	gz->level = Z_DEFAULT_COMPRESSION;
    }

    gz->mtime = gzfile_get32(&head[4]);
    gz->os_code = head[9];
    zstream_discard_input(&gz->z, 10);

    if (flags & GZ_FLAG_EXTRA) {
	if (!gzfile_read_raw_ensure(gz, 2)) {
	    rb_raise(cGzError, "unexpected end of file");
	}
	len = gzfile_get16((Bytef*)RSTRING_PTR(gz->z.input));
	if (!gzfile_read_raw_ensure(gz, 2 + len)) {
	    rb_raise(cGzError, "unexpected end of file");
	}
	zstream_discard_input(&gz->z, 2 + len);
    }
    if (flags & GZ_FLAG_ORIG_NAME) {
	if (!gzfile_read_raw_ensure(gz, 1)) {
	    rb_raise(cGzError, "unexpected end of file");
	}
	p = gzfile_read_raw_until_zero(gz, 0);
	len = p - RSTRING_PTR(gz->z.input);
	gz->orig_name = rb_str_new(RSTRING_PTR(gz->z.input), len);
	OBJ_TAINT(gz->orig_name);  /* for safe */
	zstream_discard_input(&gz->z, len + 1);
    }
    if (flags & GZ_FLAG_COMMENT) {
	if (!gzfile_read_raw_ensure(gz, 1)) {
	    rb_raise(cGzError, "unexpected end of file");
	}
	p = gzfile_read_raw_until_zero(gz, 0);
	len = p - RSTRING_PTR(gz->z.input);
	gz->comment = rb_str_new(RSTRING_PTR(gz->z.input), len);
	OBJ_TAINT(gz->comment);  /* for safe */
	zstream_discard_input(&gz->z, len + 1);
    }

    if (gz->z.input != Qnil && RSTRING_LEN(gz->z.input) > 0) {
	zstream_run(&gz->z, 0, 0, Z_SYNC_FLUSH);
    }
}

static void
gzfile_check_footer(struct gzfile *gz)
{
    unsigned long crc, length;

    gz->z.flags |= GZFILE_FLAG_FOOTER_FINISHED;

    if (!gzfile_read_raw_ensure(gz, 8)) { /* 8 is the size of gzip footer */
	gzfile_raise(gz, cNoFooter, "footer is not found");
    }

    crc = gzfile_get32((Bytef*)RSTRING_PTR(gz->z.input));
    length = gzfile_get32((Bytef*)RSTRING_PTR(gz->z.input) + 4);

    gz->z.stream.total_in += 8;  /* to rewind correctly */
    zstream_discard_input(&gz->z, 8);

    if (gz->crc != crc) {
	rb_raise(cCRCError, "invalid compressed data -- crc error");
    }
    if ((uint32_t)gz->z.stream.total_out != length) {
	rb_raise(cLengthError, "invalid compressed data -- length error");
    }
}

static void
gzfile_write(struct gzfile *gz, Bytef *str, long len)
{
    if (!(gz->z.flags & GZFILE_FLAG_HEADER_FINISHED)) {
	gzfile_make_header(gz);
    }

    if (len > 0 || (gz->z.flags & GZFILE_FLAG_SYNC)) {
	gz->crc = checksum_long(crc32, gz->crc, str, len);
	zstream_run(&gz->z, str, len, (gz->z.flags & GZFILE_FLAG_SYNC)
		    ? Z_SYNC_FLUSH : Z_NO_FLUSH);
    }
    gzfile_write_raw(gz);
}

static long
gzfile_read_more(struct gzfile *gz)
{
    volatile VALUE str;

    while (!ZSTREAM_IS_FINISHED(&gz->z)) {
	str = gzfile_read_raw(gz);
	if (NIL_P(str)) {
	    if (!ZSTREAM_IS_FINISHED(&gz->z)) {
		rb_raise(cGzError, "unexpected end of file");
	    }
	    break;
	}
	if (RSTRING_LEN(str) > 0) { /* prevent Z_BUF_ERROR */
	    zstream_run(&gz->z, (Bytef*)RSTRING_PTR(str), RSTRING_LEN(str),
			Z_SYNC_FLUSH);
	}
	if (gz->z.buf_filled > 0) break;
    }
    return gz->z.buf_filled;
}

static void
gzfile_calc_crc(struct gzfile *gz, VALUE str)
{
    if (RSTRING_LEN(str) <= gz->ungetc) {
	gz->ungetc -= RSTRING_LEN(str);
    }
    else {
	gz->crc = checksum_long(crc32, gz->crc, (Bytef*)RSTRING_PTR(str) + gz->ungetc,
			RSTRING_LEN(str) - gz->ungetc);
	gz->ungetc = 0;
    }
}

static VALUE
gzfile_newstr(struct gzfile *gz, VALUE str)
{
    if (!gz->enc2) {
	rb_enc_associate(str, gz->enc);
	OBJ_TAINT(str);  /* for safe */
	return str;
    }
    if (gz->ec && rb_enc_dummy_p(gz->enc2)) {
        str = rb_econv_str_convert(gz->ec, str, ECONV_PARTIAL_INPUT);
	rb_enc_associate(str, gz->enc);
	OBJ_TAINT(str);
	return str;
    }
    return rb_str_conv_enc_opts(str, gz->enc2, gz->enc,
				gz->ecflags, gz->ecopts);
}

static long
gzfile_fill(struct gzfile *gz, long len)
{
    if (len < 0)
        rb_raise(rb_eArgError, "negative length %ld given", len);
    if (len == 0)
	return 0;
    while (!ZSTREAM_IS_FINISHED(&gz->z) && gz->z.buf_filled < len) {
	gzfile_read_more(gz);
    }
    if (GZFILE_IS_FINISHED(gz)) {
	if (!(gz->z.flags & GZFILE_FLAG_FOOTER_FINISHED)) {
	    gzfile_check_footer(gz);
	}
	return -1;
    }
    return len < gz->z.buf_filled ? len : gz->z.buf_filled;
}

static VALUE
gzfile_read(struct gzfile *gz, long len)
{
    VALUE dst;

    len = gzfile_fill(gz, len);
    if (len == 0) return rb_str_new(0, 0);
    if (len < 0) return Qnil;
    dst = zstream_shift_buffer(&gz->z, len);
    gzfile_calc_crc(gz, dst);
    return dst;
}

static VALUE
gzfile_readpartial(struct gzfile *gz, long len, VALUE outbuf)
{
    VALUE dst;

    if (len < 0)
        rb_raise(rb_eArgError, "negative length %ld given", len);

    if (!NIL_P(outbuf))
	OBJ_TAINT(outbuf);

    if (len == 0) {
        if (NIL_P(outbuf))
            return rb_str_new(0, 0);
        else {
            rb_str_resize(outbuf, 0);
            return outbuf;
        }
    }
    while (!ZSTREAM_IS_FINISHED(&gz->z) && gz->z.buf_filled == 0) {
	gzfile_read_more(gz);
    }
    if (GZFILE_IS_FINISHED(gz)) {
	if (!(gz->z.flags & GZFILE_FLAG_FOOTER_FINISHED)) {
	    gzfile_check_footer(gz);
	}
        if (!NIL_P(outbuf))
            rb_str_resize(outbuf, 0);
	rb_raise(rb_eEOFError, "end of file reached");
    }

    dst = zstream_shift_buffer(&gz->z, len);
    gzfile_calc_crc(gz, dst);

    if (!NIL_P(outbuf)) {
        rb_str_resize(outbuf, RSTRING_LEN(dst));
        memcpy(RSTRING_PTR(outbuf), RSTRING_PTR(dst), RSTRING_LEN(dst));
	dst = outbuf;
    }
    OBJ_TAINT(dst);  /* for safe */
    return dst;
}

static VALUE
gzfile_read_all(struct gzfile *gz)
{
    VALUE dst;

    while (!ZSTREAM_IS_FINISHED(&gz->z)) {
	gzfile_read_more(gz);
    }
    if (GZFILE_IS_FINISHED(gz)) {
	if (!(gz->z.flags & GZFILE_FLAG_FOOTER_FINISHED)) {
	    gzfile_check_footer(gz);
	}
	return rb_str_new(0, 0);
    }

    dst = zstream_detach_buffer(&gz->z);
    gzfile_calc_crc(gz, dst);
    OBJ_TAINT(dst);
    return gzfile_newstr(gz, dst);
}

static VALUE
gzfile_getc(struct gzfile *gz)
{
    VALUE buf, dst = 0;
    int len;

    len = rb_enc_mbmaxlen(gz->enc);
    while (!ZSTREAM_IS_FINISHED(&gz->z) && gz->z.buf_filled < len) {
	gzfile_read_more(gz);
    }
    if (GZFILE_IS_FINISHED(gz)) {
	if (!(gz->z.flags & GZFILE_FLAG_FOOTER_FINISHED)) {
	    gzfile_check_footer(gz);
	}
	return Qnil;
    }

    if (gz->ec && rb_enc_dummy_p(gz->enc2)) {
	const unsigned char *ss, *sp, *se;
	unsigned char *ds, *dp, *de;

	if (!gz->cbuf) {
	    gz->cbuf = ALLOC_N(char, GZFILE_CBUF_CAPA);
	}
        ss = sp = (const unsigned char*)RSTRING_PTR(gz->z.buf);
        se = sp + gz->z.buf_filled;
        ds = dp = (unsigned char *)gz->cbuf;
        de = (unsigned char *)ds + GZFILE_CBUF_CAPA;
        (void)rb_econv_convert(gz->ec, &sp, se, &dp, de, ECONV_PARTIAL_INPUT|ECONV_AFTER_OUTPUT);
        rb_econv_check_error(gz->ec);
	dst = zstream_shift_buffer(&gz->z, sp - ss);
	gzfile_calc_crc(gz, dst);
	dst = rb_str_new(gz->cbuf, dp - ds);
	rb_enc_associate(dst, gz->enc);
	OBJ_TAINT(dst);
	return dst;
    }
    else {
	buf = gz->z.buf;
	len = rb_enc_mbclen(RSTRING_PTR(buf), RSTRING_END(buf), gz->enc);
	dst = gzfile_read(gz, len);
	return gzfile_newstr(gz, dst);
    }
}

static void
gzfile_ungets(struct gzfile *gz, const Bytef *b, long len)
{
    zstream_buffer_ungets(&gz->z, b, len);
    gz->ungetc+=len;
}

static void
gzfile_ungetbyte(struct gzfile *gz, int c)
{
    zstream_buffer_ungetbyte(&gz->z, c);
    gz->ungetc++;
}

static VALUE
gzfile_writer_end_run(VALUE arg)
{
    struct gzfile *gz = (struct gzfile *)arg;

    if (!(gz->z.flags & GZFILE_FLAG_HEADER_FINISHED)) {
	gzfile_make_header(gz);
    }

    zstream_run(&gz->z, (Bytef*)"", 0, Z_FINISH);
    gzfile_make_footer(gz);
    gzfile_write_raw(gz);

    return Qnil;
}

static void
gzfile_writer_end(struct gzfile *gz)
{
    if (ZSTREAM_IS_CLOSING(&gz->z)) return;
    gz->z.flags |= ZSTREAM_FLAG_CLOSING;

    rb_ensure(gzfile_writer_end_run, (VALUE)gz, zstream_end, (VALUE)&gz->z);
}

static VALUE
gzfile_reader_end_run(VALUE arg)
{
    struct gzfile *gz = (struct gzfile *)arg;

    if (GZFILE_IS_FINISHED(gz)
	&& !(gz->z.flags & GZFILE_FLAG_FOOTER_FINISHED)) {
	gzfile_check_footer(gz);
    }

    return Qnil;
}

static void
gzfile_reader_end(struct gzfile *gz)
{
    if (ZSTREAM_IS_CLOSING(&gz->z)) return;
    gz->z.flags |= ZSTREAM_FLAG_CLOSING;

    rb_ensure(gzfile_reader_end_run, (VALUE)gz, zstream_end, (VALUE)&gz->z);
}

static void
gzfile_reader_rewind(struct gzfile *gz)
{
    long n;

    n = gz->z.stream.total_in;
    if (!NIL_P(gz->z.input)) {
	n += RSTRING_LEN(gz->z.input);
    }

    rb_funcall(gz->io, id_seek, 2, rb_int2inum(-n), INT2FIX(1));
    gzfile_reset(gz);
}

static VALUE
gzfile_reader_get_unused(struct gzfile *gz)
{
    VALUE str;

    if (!ZSTREAM_IS_READY(&gz->z)) return Qnil;
    if (!GZFILE_IS_FINISHED(gz)) return Qnil;
    if (!(gz->z.flags & GZFILE_FLAG_FOOTER_FINISHED)) {
	gzfile_check_footer(gz);
    }
    if (NIL_P(gz->z.input)) return Qnil;

    str = rb_str_resurrect(gz->z.input);
    OBJ_TAINT(str);  /* for safe */
    return str;
}

static struct gzfile *
get_gzfile(VALUE obj)
{
    struct gzfile *gz;

    Data_Get_Struct(obj, struct gzfile, gz);
    if (!ZSTREAM_IS_READY(&gz->z)) {
	rb_raise(cGzError, "closed gzip stream");
    }
    return gz;
}


/* ------------------------------------------------------------------------- */

/*
 * Document-class: Zlib::GzipFile
 *
 * Zlib::GzipFile is an abstract class for handling a gzip formatted
 * compressed file. The operations are defined in the subclasses,
 * Zlib::GzipReader for reading, and Zlib::GzipWriter for writing.
 *
 * GzipReader should be used by associating an IO, or IO-like, object.
 *
 * == Method Catalogue
 *
 * - ::wrap
 * - ::open (Zlib::GzipReader::open and Zlib::GzipWriter::open)
 * - #close
 * - #closed?
 * - #comment
 * - comment= (Zlib::GzipWriter#comment=)
 * - #crc
 * - eof? (Zlib::GzipReader#eof?)
 * - #finish
 * - #level
 * - lineno (Zlib::GzipReader#lineno)
 * - lineno= (Zlib::GzipReader#lineno=)
 * - #mtime
 * - mtime= (Zlib::GzipWriter#mtime=)
 * - #orig_name
 * - orig_name (Zlib::GzipWriter#orig_name=)
 * - #os_code
 * - path (when the underlying IO supports #path)
 * - #sync
 * - #sync=
 * - #to_io
 *
 * (due to internal structure, documentation may appear under Zlib::GzipReader
 * or Zlib::GzipWriter)
 */


typedef struct {
    int argc;
    VALUE *argv;
    VALUE klass;
} new_wrap_arg_t;

static VALUE
new_wrap(VALUE tmp)
{
    new_wrap_arg_t *arg = (new_wrap_arg_t *)tmp;
    return rb_class_new_instance(arg->argc, arg->argv, arg->klass);
}

static VALUE
gzfile_ensure_close(VALUE obj)
{
    struct gzfile *gz;

    Data_Get_Struct(obj, struct gzfile, gz);
    if (ZSTREAM_IS_READY(&gz->z)) {
	gzfile_close(gz, 1);
    }
    return Qnil;
}

static VALUE
gzfile_wrap(int argc, VALUE *argv, VALUE klass, int close_io_on_error)
{
    VALUE obj;

    if (close_io_on_error) {
	int state = 0;
	new_wrap_arg_t arg;
	arg.argc = argc;
	arg.argv = argv;
	arg.klass = klass;
	obj = rb_protect(new_wrap, (VALUE)&arg, &state);
	if (state) {
	    rb_io_close(argv[0]);
	    rb_jump_tag(state);
	}
    }
    else {
	obj = rb_class_new_instance(argc, argv, klass);
    }

    if (rb_block_given_p()) {
	return rb_ensure(rb_yield, obj, gzfile_ensure_close, obj);
    }
    else {
	return obj;
    }
}

/*
 * Document-method: Zlib::GzipFile.wrap
 *
 * call-seq:
 *   Zlib::GzipReader.wrap(io, ...) { |gz| ... }
 *   Zlib::GzipWriter.wrap(io, ...) { |gz| ... }
 *
 * Creates a GzipReader or GzipWriter associated with +io+, passing in any
 * necessary extra options, and executes the block with the newly created
 * object just like File.open.
 *
 * The GzipFile object will be closed automatically after executing the block.
 * If you want to keep the associated IO object open, you may call
 * Zlib::GzipFile#finish method in the block.
 */
static VALUE
rb_gzfile_s_wrap(int argc, VALUE *argv, VALUE klass)
{
    return gzfile_wrap(argc, argv, klass, 0);
}

/*
 * Document-method: Zlib::GzipFile.open
 *
 * See Zlib::GzipReader#open and Zlib::GzipWriter#open.
 */
static VALUE
gzfile_s_open(int argc, VALUE *argv, VALUE klass, const char *mode)
{
    VALUE io, filename;

    if (argc < 1) {
	rb_raise(rb_eArgError, "wrong number of arguments (0 for 1)");
    }
    filename = argv[0];
    io = rb_file_open_str(filename, mode);
    argv[0] = io;
    return gzfile_wrap(argc, argv, klass, 1);
}

/*
 * Document-method: Zlib::GzipFile#to_io
 *
 * Same as IO.
 */
static VALUE
rb_gzfile_to_io(VALUE obj)
{
    return get_gzfile(obj)->io;
}

/*
 * Document-method: Zlib::GzipFile#crc
 *
 * Returns CRC value of the uncompressed data.
 */
static VALUE
rb_gzfile_crc(VALUE obj)
{
    return rb_uint2inum(get_gzfile(obj)->crc);
}

/*
 * Document-method: Zlib::GzipFile#mtime
 *
 * Returns last modification time recorded in the gzip file header.
 */
static VALUE
rb_gzfile_mtime(VALUE obj)
{
    return rb_time_new(get_gzfile(obj)->mtime, (time_t)0);
}

/*
 * Document-method: Zlib::GzipFile#level
 *
 * Returns compression level.
 */
static VALUE
rb_gzfile_level(VALUE obj)
{
    return INT2FIX(get_gzfile(obj)->level);
}

/*
 * Document-method: Zlib::GzipFile#os_code
 *
 * Returns OS code number recorded in the gzip file header.
 */
static VALUE
rb_gzfile_os_code(VALUE obj)
{
    return INT2FIX(get_gzfile(obj)->os_code);
}

/*
 * Document-method: Zlib::GzipFile#orig_name
 *
 * Returns original filename recorded in the gzip file header, or +nil+ if
 * original filename is not present.
 */
static VALUE
rb_gzfile_orig_name(VALUE obj)
{
    VALUE str = get_gzfile(obj)->orig_name;
    if (!NIL_P(str)) {
	str = rb_str_dup(str);
    }
    OBJ_TAINT(str);  /* for safe */
    return str;
}

/*
 * Document-method: Zlib::GzipFile#comment
 *
 * Returns comments recorded in the gzip file header, or nil if the comments
 * is not present.
 */
static VALUE
rb_gzfile_comment(VALUE obj)
{
    VALUE str = get_gzfile(obj)->comment;
    if (!NIL_P(str)) {
	str = rb_str_dup(str);
    }
    OBJ_TAINT(str);  /* for safe */
    return str;
}

/*
 * Document-method: Zlib::GzipFile#lineno
 *
 * The line number of the last row read from this file.
 */
static VALUE
rb_gzfile_lineno(VALUE obj)
{
    return INT2NUM(get_gzfile(obj)->lineno);
}

/*
 * Document-method: Zlib::GzipReader#lineno=
 *
 * Specify line number of the last row read from this file.
 */
static VALUE
rb_gzfile_set_lineno(VALUE obj, VALUE lineno)
{
    struct gzfile *gz = get_gzfile(obj);
    gz->lineno = NUM2INT(lineno);
    return lineno;
}

/*
 * Document-method: Zlib::GzipWriter#mtime=
 *
 * Specify the modification time (+mtime+) in the gzip header.
 * Using a Fixnum or Integer
 */
static VALUE
rb_gzfile_set_mtime(VALUE obj, VALUE mtime)
{
    struct gzfile *gz = get_gzfile(obj);
    VALUE val;

    if (gz->z.flags & GZFILE_FLAG_HEADER_FINISHED) {
	rb_raise(cGzError, "header is already written");
    }

    if (FIXNUM_P(mtime)) {
	gz->mtime = FIX2INT(mtime);
    }
    else {
	val = rb_Integer(mtime);
	gz->mtime = FIXNUM_P(val) ? FIX2UINT(val) : rb_big2ulong(val);
    }
    return mtime;
}

/*
 * Document-method: Zlib::GzipFile#orig_name=
 *
 * Specify the original name (+str+) in the gzip header.
 */
static VALUE
rb_gzfile_set_orig_name(VALUE obj, VALUE str)
{
    struct gzfile *gz = get_gzfile(obj);
    VALUE s;
    char *p;

    if (gz->z.flags & GZFILE_FLAG_HEADER_FINISHED) {
	rb_raise(cGzError, "header is already written");
    }
    s = rb_str_dup(rb_str_to_str(str));
    p = memchr(RSTRING_PTR(s), '\0', RSTRING_LEN(s));
    if (p) {
	rb_str_resize(s, p - RSTRING_PTR(s));
    }
    gz->orig_name = s;
    return str;
}

/*
 * Document-method: Zlib::GzipFile#comment=
 *
 * Specify the comment (+str+) in the gzip header.
 */
static VALUE
rb_gzfile_set_comment(VALUE obj, VALUE str)
{
    struct gzfile *gz = get_gzfile(obj);
    VALUE s;
    char *p;

    if (gz->z.flags & GZFILE_FLAG_HEADER_FINISHED) {
	rb_raise(cGzError, "header is already written");
    }
    s = rb_str_dup(rb_str_to_str(str));
    p = memchr(RSTRING_PTR(s), '\0', RSTRING_LEN(s));
    if (p) {
	rb_str_resize(s, p - RSTRING_PTR(s));
    }
    gz->comment = s;
    return str;
}

/*
 * Document-method: Zlib::GzipFile#close
 *
 * Closes the GzipFile object. This method calls close method of the
 * associated IO object. Returns the associated IO object.
 */
static VALUE
rb_gzfile_close(VALUE obj)
{
    struct gzfile *gz = get_gzfile(obj);
    VALUE io;

    io = gz->io;
    gzfile_close(gz, 1);
    return io;
}

/*
 * Document-method: Zlib::GzipFile#finish
 *
 * Closes the GzipFile object. Unlike Zlib::GzipFile#close, this method never
 * calls the close method of the associated IO object. Returns the associated IO
 * object.
 */
static VALUE
rb_gzfile_finish(VALUE obj)
{
    struct gzfile *gz = get_gzfile(obj);
    VALUE io;

    io = gz->io;
    gzfile_close(gz, 0);
    return io;
}

/*
 * Document-method: Zlib::GzipFile#closed?
 *
 * Same as IO#closed?
 *
 */
static VALUE
rb_gzfile_closed_p(VALUE obj)
{
    struct gzfile *gz;
    Data_Get_Struct(obj, struct gzfile, gz);
    return NIL_P(gz->io) ? Qtrue : Qfalse;
}

/*
 * Document-method: Zlib::GzipFile#eof?
 *
 * Returns +true+ or +false+ whether the stream has reached the end.
 */
static VALUE
rb_gzfile_eof_p(VALUE obj)
{
    struct gzfile *gz = get_gzfile(obj);
    return GZFILE_IS_FINISHED(gz) ? Qtrue : Qfalse;
}

/*
 * Document-method: Zlib::GzipFile#sync
 *
 * Same as IO#sync
 *
 */
static VALUE
rb_gzfile_sync(VALUE obj)
{
    return (get_gzfile(obj)->z.flags & GZFILE_FLAG_SYNC) ? Qtrue : Qfalse;
}

/*
 * Document-method: Zlib::GzipFile#sync=
 *
 * call-seq: sync = flag
 *
 * Same as IO.  If flag is +true+, the associated IO object must respond to the
 * +flush+ method.  While +sync+ mode is +true+, the compression ratio
 * decreases sharply.
 */
static VALUE
rb_gzfile_set_sync(VALUE obj, VALUE mode)
{
    struct gzfile *gz = get_gzfile(obj);

    if (RTEST(mode)) {
	gz->z.flags |= GZFILE_FLAG_SYNC;
    }
    else {
	gz->z.flags &= ~GZFILE_FLAG_SYNC;
    }
    return mode;
}

/*
 * Document-method: Zlib::GzipFile#total_in
 *
 * Total number of input bytes read so far.
 */
static VALUE
rb_gzfile_total_in(VALUE obj)
{
    return rb_uint2inum(get_gzfile(obj)->z.stream.total_in);
}

/*
 * Document-method: Zlib::GzipFile#total_out
 *
 * Total number of output bytes output so far.
 */
static VALUE
rb_gzfile_total_out(VALUE obj)
{
    struct gzfile *gz = get_gzfile(obj);
    return rb_uint2inum(gz->z.stream.total_out - gz->z.buf_filled);
}

/*
 * Document-method: Zlib::GzipFile#path
 *
 * call-seq: path
 *
 * Returns the path string of the associated IO-like object.  This
 * method is only defined when the IO-like object responds to #path().
 */
static VALUE
rb_gzfile_path(VALUE obj)
{
    struct gzfile *gz;
    Data_Get_Struct(obj, struct gzfile, gz);
    return gz->path;
}

static void
rb_gzfile_ecopts(struct gzfile *gz, VALUE opts)
{
    if (!NIL_P(opts)) {
	rb_io_extract_encoding_option(opts, &gz->enc, &gz->enc2, NULL);
    }
    if (gz->enc2) {
	gz->ecflags = rb_econv_prepare_opts(opts, &opts);
	gz->ec = rb_econv_open_opts(gz->enc2->name, gz->enc->name,
				    gz->ecflags, opts);
	gz->ecopts = opts;
    }
}

/* ------------------------------------------------------------------------- */

/*
 * Document-class: Zlib::GzipWriter
 *
 * Zlib::GzipWriter is a class for writing gzipped files.  GzipWriter should
 * be used with an instance of IO, or IO-like, object.
 *
 * Following two example generate the same result.
 *
 *   Zlib::GzipWriter.open('hoge.gz') do |gz|
 *     gz.write 'jugemu jugemu gokou no surikire...'
 *   end
 *
 *   File.open('hoge.gz', 'w') do |f|
 *     gz = Zlib::GzipWriter.new(f)
 *     gz.write 'jugemu jugemu gokou no surikire...'
 *     gz.close
 *   end
 *
 * To make like gzip(1) does, run following:
 *
 *   orig = 'hoge.txt'
 *   Zlib::GzipWriter.open('hoge.gz') do |gz|
 *     gz.mtime = File.mtime(orig)
 *     gz.orig_name = orig
 *     gz.write IO.binread(orig)
 *   end
 *
 * NOTE: Due to the limitation of Ruby's finalizer, you must explicitly close
 * GzipWriter objects by Zlib::GzipWriter#close etc.  Otherwise, GzipWriter
 * will be not able to write the gzip footer and will generate a broken gzip
 * file.
 */

static VALUE
rb_gzwriter_s_allocate(VALUE klass)
{
    return gzfile_writer_new(klass);
}

/*
 * call-seq: Zlib::GzipWriter.open(filename, level=nil, strategy=nil) { |gz| ... }
 *
 * Opens a file specified by +filename+ for writing gzip compressed data, and
 * returns a GzipWriter object associated with that file.  Further details of
 * this method are found in Zlib::GzipWriter.new and Zlib::GzipFile.wrap.
 */
static VALUE
rb_gzwriter_s_open(int argc, VALUE *argv, VALUE klass)
{
    return gzfile_s_open(argc, argv, klass, "wb");
}

/*
 * call-seq:
 *   Zlib::GzipWriter.new(io, level = nil, strategy = nil, options = {})
 *
 * Creates a GzipWriter object associated with +io+. +level+ and +strategy+
 * should be the same as the arguments of Zlib::Deflate.new.  The GzipWriter
 * object writes gzipped data to +io+.  +io+ must respond to the
 * +write+ method that behaves the same as IO#write.
 *
 * The +options+ hash may be used to set the encoding of the data.
 * +:external_encoding+, +:internal_encoding+ and +:encoding+ may be set as in
 * IO::new.
 */
static VALUE
rb_gzwriter_initialize(int argc, VALUE *argv, VALUE obj)
{
    struct gzfile *gz;
    VALUE io, level, strategy, opt = Qnil;
    int err;

    if (argc > 1) {
	opt = rb_check_convert_type(argv[argc-1], T_HASH, "Hash", "to_hash");
	if (!NIL_P(opt)) argc--;
    }

    rb_scan_args(argc, argv, "12", &io, &level, &strategy);
    Data_Get_Struct(obj, struct gzfile, gz);

    /* this is undocumented feature of zlib */
    gz->level = ARG_LEVEL(level);
    err = deflateInit2(&gz->z.stream, gz->level, Z_DEFLATED,
		       -MAX_WBITS, DEF_MEM_LEVEL, ARG_STRATEGY(strategy));
    if (err != Z_OK) {
	raise_zlib_error(err, gz->z.stream.msg);
    }
    gz->io = io;
    ZSTREAM_READY(&gz->z);
    rb_gzfile_ecopts(gz, opt);

    if (rb_respond_to(io, id_path)) {
	gz->path = rb_funcall(gz->io, id_path, 0);
	rb_define_singleton_method(obj, "path", rb_gzfile_path, 0);
    }

    return obj;
}

/*
 * call-seq: flush(flush=nil)
 *
 * Flushes all the internal buffers of the GzipWriter object.  The meaning of
 * +flush+ is same as in Zlib::Deflate#deflate.  <tt>Zlib::SYNC_FLUSH</tt> is used if
 * +flush+ is omitted.  It is no use giving flush <tt>Zlib::NO_FLUSH</tt>.
 */
static VALUE
rb_gzwriter_flush(int argc, VALUE *argv, VALUE obj)
{
    struct gzfile *gz = get_gzfile(obj);
    VALUE v_flush;
    int flush;

    rb_scan_args(argc, argv, "01", &v_flush);

    flush = FIXNUMARG(v_flush, Z_SYNC_FLUSH);
    if (flush != Z_NO_FLUSH) {  /* prevent Z_BUF_ERROR */
	zstream_run(&gz->z, (Bytef*)"", 0, flush);
    }

    gzfile_write_raw(gz);
    if (rb_respond_to(gz->io, id_flush)) {
	rb_funcall(gz->io, id_flush, 0);
    }
    return obj;
}

/*
 * Same as IO.
 */
static VALUE
rb_gzwriter_write(VALUE obj, VALUE str)
{
    struct gzfile *gz = get_gzfile(obj);

    if (TYPE(str) != T_STRING)
	str = rb_obj_as_string(str);
    if (gz->enc2 && gz->enc2 != rb_ascii8bit_encoding()) {
	str = rb_str_conv_enc(str, rb_enc_get(str), gz->enc2);
    }
    gzfile_write(gz, (Bytef*)RSTRING_PTR(str), RSTRING_LEN(str));
    return INT2FIX(RSTRING_LEN(str));
}

/*
 * Same as IO.
 */
static VALUE
rb_gzwriter_putc(VALUE obj, VALUE ch)
{
    struct gzfile *gz = get_gzfile(obj);
    char c = NUM2CHR(ch);

    gzfile_write(gz, (Bytef*)&c, 1);
    return ch;
}



/*
 * Document-method: <<
 * Same as IO.
 */
#define rb_gzwriter_addstr  rb_io_addstr
/*
 * Document-method: printf
 * Same as IO.
 */
#define rb_gzwriter_printf  rb_io_printf
/*
 * Document-method: print
 * Same as IO.
 */
#define rb_gzwriter_print  rb_io_print
/*
 * Document-method: puts
 * Same as IO.
 */
#define rb_gzwriter_puts  rb_io_puts


/* ------------------------------------------------------------------------- */

/*
 * Document-class: Zlib::GzipReader
 *
 * Zlib::GzipReader is the class for reading a gzipped file.  GzipReader should
 * be used an IO, or -IO-lie, object.
 *
 *   Zlib::GzipReader.open('hoge.gz') {|gz|
 *     print gz.read
 *   }
 *
 *   File.open('hoge.gz') do |f|
 *     gz = Zlib::GzipReader.new(f)
 *     print gz.read
 *     gz.close
 *   end
 *
 * == Method Catalogue
 *
 * The following methods in Zlib::GzipReader are just like their counterparts
 * in IO, but they raise Zlib::Error or Zlib::GzipFile::Error exception if an
 * error was found in the gzip file.
 * - #each
 * - #each_line
 * - #each_byte
 * - #gets
 * - #getc
 * - #lineno
 * - #lineno=
 * - #read
 * - #readchar
 * - #readline
 * - #readlines
 * - #ungetc
 *
 * Be careful of the footer of the gzip file. A gzip file has the checksum of
 * pre-compressed data in its footer. GzipReader checks all uncompressed data
 * against that checksum at the following cases, and if it fails, raises
 * <tt>Zlib::GzipFile::NoFooter</tt>, <tt>Zlib::GzipFile::CRCError</tt>, or
 * <tt>Zlib::GzipFile::LengthError</tt> exception.
 *
 * - When an reading request is received beyond the end of file (the end of
 *   compressed data). That is, when Zlib::GzipReader#read,
 *   Zlib::GzipReader#gets, or some other methods for reading returns nil.
 * - When Zlib::GzipFile#close method is called after the object reaches the
 *   end of file.
 * - When Zlib::GzipReader#unused method is called after the object reaches
 *   the end of file.
 *
 * The rest of the methods are adequately described in their own
 * documentation.
 */

static VALUE
rb_gzreader_s_allocate(VALUE klass)
{
    return gzfile_reader_new(klass);
}

/*
 * Document-method: Zlib::GzipReader.open
 *
 * call-seq: Zlib::GzipReader.open(filename) {|gz| ... }
 *
 * Opens a file specified by +filename+ as a gzipped file, and returns a
 * GzipReader object associated with that file.  Further details of this method
 * are in Zlib::GzipReader.new and ZLib::GzipFile.wrap.
 */
static VALUE
rb_gzreader_s_open(int argc, VALUE *argv, VALUE klass)
{
    return gzfile_s_open(argc, argv, klass, "rb");
}

/*
 * Document-method: Zlib::GzipReader.new
 *
 * call-seq:
 *   Zlib::GzipReader.new(io, options = {})
 *
 * Creates a GzipReader object associated with +io+. The GzipReader object reads
 * gzipped data from +io+, and parses/decompresses it.  The +io+ must
 * have a +read+ method that behaves same as the IO#read.
 *
 * The +options+ hash may be used to set the encoding of the data.
 * +:external_encoding+, +:internal_encoding+ and +:encoding+ may be set as in
 * IO::new.
 *
 * If the gzip file header is incorrect, raises an Zlib::GzipFile::Error
 * exception.
 */
static VALUE
rb_gzreader_initialize(int argc, VALUE *argv, VALUE obj)
{
    VALUE io, opt = Qnil;
    struct gzfile *gz;
    int err;

    Data_Get_Struct(obj, struct gzfile, gz);
    rb_scan_args(argc, argv, "1:", &io, &opt);

    /* this is undocumented feature of zlib */
    err = inflateInit2(&gz->z.stream, -MAX_WBITS);
    if (err != Z_OK) {
	raise_zlib_error(err, gz->z.stream.msg);
    }
    gz->io = io;
    ZSTREAM_READY(&gz->z);
    gzfile_read_header(gz);
    rb_gzfile_ecopts(gz, opt);

    if (rb_respond_to(io, id_path)) {
	gz->path = rb_funcall(gz->io, id_path, 0);
	rb_define_singleton_method(obj, "path", rb_gzfile_path, 0);
    }

    return obj;
}

/*
 * Document-method: Zlib::GzipReader#rewind
 *
 * Resets the position of the file pointer to the point created the GzipReader
 * object.  The associated IO object needs to respond to the +seek+ method.
 */
static VALUE
rb_gzreader_rewind(VALUE obj)
{
    struct gzfile *gz = get_gzfile(obj);
    gzfile_reader_rewind(gz);
    return INT2FIX(0);
}

/*
 * Document-method: Zlib::GzipReader#unused
 *
 * Returns the rest of the data which had read for parsing gzip format, or
 * +nil+ if the whole gzip file is not parsed yet.
 */
static VALUE
rb_gzreader_unused(VALUE obj)
{
    struct gzfile *gz;
    Data_Get_Struct(obj, struct gzfile, gz);
    return gzfile_reader_get_unused(gz);
}

/*
 * Document-method: Zlib::GzipReader#read
 *
 * See Zlib::GzipReader documentation for a description.
 */
static VALUE
rb_gzreader_read(int argc, VALUE *argv, VALUE obj)
{
    struct gzfile *gz = get_gzfile(obj);
    VALUE vlen;
    long len;

    rb_scan_args(argc, argv, "01", &vlen);
    if (NIL_P(vlen)) {
	return gzfile_read_all(gz);
    }

    len = NUM2INT(vlen);
    if (len < 0) {
	rb_raise(rb_eArgError, "negative length %ld given", len);
    }
    return gzfile_read(gz, len);
}

/*
 * Document-method: Zlib::GzipReader#readpartial
 *
 *  call-seq:
 *     gzipreader.readpartial(maxlen [, outbuf]) => string, outbuf
 *
 *  Reads at most <i>maxlen</i> bytes from the gziped stream but
 *  it blocks only if <em>gzipreader</em> has no data immediately available.
 *  If the optional <i>outbuf</i> argument is present,
 *  it must reference a String, which will receive the data.
 *  It raises <code>EOFError</code> on end of file.
 */
static VALUE
rb_gzreader_readpartial(int argc, VALUE *argv, VALUE obj)
{
    struct gzfile *gz = get_gzfile(obj);
    VALUE vlen, outbuf;
    long len;

    rb_scan_args(argc, argv, "11", &vlen, &outbuf);

    len = NUM2INT(vlen);
    if (len < 0) {
	rb_raise(rb_eArgError, "negative length %ld given", len);
    }
    if (!NIL_P(outbuf))
        Check_Type(outbuf, T_STRING);
    return gzfile_readpartial(gz, len, outbuf);
}

/*
 * Document-method: Zlib::GzipReader#getc
 *
 * See Zlib::GzipReader documentation for a description.
 */
static VALUE
rb_gzreader_getc(VALUE obj)
{
    struct gzfile *gz = get_gzfile(obj);

    return gzfile_getc(gz);
}

/*
 * Document-method: Zlib::GzipReader#readchar
 *
 * See Zlib::GzipReader documentation for a description.
 */
static VALUE
rb_gzreader_readchar(VALUE obj)
{
    VALUE dst;
    dst = rb_gzreader_getc(obj);
    if (NIL_P(dst)) {
	rb_raise(rb_eEOFError, "end of file reached");
    }
    return dst;
}

/*
 * Document-method: Zlib::GzipReader#getbyte
 *
 * See Zlib::GzipReader documentation for a description.
 */
static VALUE
rb_gzreader_getbyte(VALUE obj)
{
    struct gzfile *gz = get_gzfile(obj);
    VALUE dst;

    dst = gzfile_read(gz, 1);
    if (!NIL_P(dst)) {
	dst = INT2FIX((unsigned int)(RSTRING_PTR(dst)[0]) & 0xff);
    }
    return dst;
}

/*
 * Document-method: Zlib::GzipReader#readbyte
 *
 * See Zlib::GzipReader documentation for a description.
 */
static VALUE
rb_gzreader_readbyte(VALUE obj)
{
    VALUE dst;
    dst = rb_gzreader_getbyte(obj);
    if (NIL_P(dst)) {
	rb_raise(rb_eEOFError, "end of file reached");
    }
    return dst;
}

/*
 * Document-method: Zlib::GzipReader#each_char
 *
 * See Zlib::GzipReader documentation for a description.
 */
static VALUE
rb_gzreader_each_char(VALUE obj)
{
    VALUE c;

    RETURN_ENUMERATOR(obj, 0, 0);

    while (!NIL_P(c = rb_gzreader_getc(obj))) {
	rb_yield(c);
    }
    return Qnil;
}

/*
 * Document-method: Zlib::GzipReader#each_byte
 *
 * See Zlib::GzipReader documentation for a description.
 */
static VALUE
rb_gzreader_each_byte(VALUE obj)
{
    VALUE c;

    RETURN_ENUMERATOR(obj, 0, 0);

    while (!NIL_P(c = rb_gzreader_getbyte(obj))) {
	rb_yield(c);
    }
    return Qnil;
}

/*
 * Document-method: Zlib::GzipReader#ungetc
 *
 * See Zlib::GzipReader documentation for a description.
 */
static VALUE
rb_gzreader_ungetc(VALUE obj, VALUE s)
{
    struct gzfile *gz;

    if (FIXNUM_P(s))
	return rb_gzreader_ungetbyte(obj, s);
    gz = get_gzfile(obj);
    StringValue(s);
    if (gz->enc2 && gz->enc2 != rb_ascii8bit_encoding()) {
	s = rb_str_conv_enc(s, rb_enc_get(s), gz->enc2);
    }
    gzfile_ungets(gz, (const Bytef*)RSTRING_PTR(s), RSTRING_LEN(s));
    return Qnil;
}

/*
 * Document-method: Zlib::GzipReader#ungetbyte
 *
 * See Zlib::GzipReader documentation for a description.
 */
static VALUE
rb_gzreader_ungetbyte(VALUE obj, VALUE ch)
{
    struct gzfile *gz = get_gzfile(obj);
    gzfile_ungetbyte(gz, NUM2CHR(ch));
    return Qnil;
}

static void
gzreader_skip_linebreaks(struct gzfile *gz)
{
    VALUE str;
    char *p;
    int n;

    while (gz->z.buf_filled == 0) {
	if (GZFILE_IS_FINISHED(gz)) return;
	gzfile_read_more(gz);
    }
    n = 0;
    p = RSTRING_PTR(gz->z.buf);

    while (n++, *(p++) == '\n') {
	if (n >= gz->z.buf_filled) {
	    str = zstream_detach_buffer(&gz->z);
	    gzfile_calc_crc(gz, str);
	    while (gz->z.buf_filled == 0) {
		if (GZFILE_IS_FINISHED(gz)) return;
		gzfile_read_more(gz);
	    }
	    n = 0;
	    p = RSTRING_PTR(gz->z.buf);
	}
    }

    str = zstream_shift_buffer(&gz->z, n - 1);
    gzfile_calc_crc(gz, str);
}

static void
rscheck(const char *rsptr, long rslen, VALUE rs)
{
    if (RSTRING_PTR(rs) != rsptr && RSTRING_LEN(rs) != rslen)
	rb_raise(rb_eRuntimeError, "rs modified");
}

static long
gzreader_charboundary(struct gzfile *gz, long n)
{
    char *s = RSTRING_PTR(gz->z.buf);
    char *e = s + gz->z.buf_filled;
    char *p = rb_enc_left_char_head(s, s + n, e, gz->enc);
    long l = p - s;
    if (l < n) {
	n = rb_enc_precise_mbclen(p, e, gz->enc);
	if (MBCLEN_NEEDMORE_P(n)) {
	    if ((l = gzfile_fill(gz, l + MBCLEN_NEEDMORE_LEN(n))) > 0) {
		return l;
	    }
	}
	else if (MBCLEN_CHARFOUND_P(n)) {
	    return l + MBCLEN_CHARFOUND_LEN(n);
	}
    }
    return n;
}

static VALUE
gzreader_gets(int argc, VALUE *argv, VALUE obj)
{
    struct gzfile *gz = get_gzfile(obj);
    volatile VALUE rs;
    VALUE dst;
    const char *rsptr;
    char *p, *res;
    long rslen, n, limit = -1;
    int rspara;
    rb_encoding *enc = gz->enc;
    int maxlen = rb_enc_mbmaxlen(enc);

    if (argc == 0) {
	rs = rb_rs;
    }
    else {
	VALUE lim, tmp;

	rb_scan_args(argc, argv, "11", &rs, &lim);
	if (!NIL_P(lim)) {
	    if (!NIL_P(rs)) StringValue(rs);
	}
	else if (!NIL_P(rs)) {
	    tmp = rb_check_string_type(rs);
	    if (NIL_P(tmp)) {
		lim = rs;
		rs = rb_rs;
	    }
	    else {
		rs = tmp;
	    }
	}
	if (!NIL_P(lim)) {
	    limit = NUM2LONG(lim);
	    if (limit == 0) return rb_str_new(0,0);
	}
    }

    if (NIL_P(rs)) {
	if (limit < 0) {
	    dst = gzfile_read_all(gz);
	    if (RSTRING_LEN(dst) == 0) return Qnil;
	}
	else if ((n = gzfile_fill(gz, limit)) <= 0) {
	    return Qnil;
	}
	else {
	    if (maxlen > 1 && n >= limit && !GZFILE_IS_FINISHED(gz)) {
		n = gzreader_charboundary(gz, n);
	    }
	    else {
		n = limit;
	    }
	    dst = zstream_shift_buffer(&gz->z, n);
	    gzfile_calc_crc(gz, dst);
	    dst = gzfile_newstr(gz, dst);
	}
	gz->lineno++;
	return dst;
    }

    if (RSTRING_LEN(rs) == 0) {
	rsptr = "\n\n";
	rslen = 2;
	rspara = 1;
    } else {
	rsptr = RSTRING_PTR(rs);
	rslen = RSTRING_LEN(rs);
	rspara = 0;
    }

    if (rspara) {
	gzreader_skip_linebreaks(gz);
    }

    while (gz->z.buf_filled < rslen) {
	if (ZSTREAM_IS_FINISHED(&gz->z)) {
	    if (gz->z.buf_filled > 0) gz->lineno++;
	    return gzfile_read(gz, rslen);
	}
	gzfile_read_more(gz);
    }

    p = RSTRING_PTR(gz->z.buf);
    n = rslen;
    for (;;) {
	long filled;
	if (n > gz->z.buf_filled) {
	    if (ZSTREAM_IS_FINISHED(&gz->z)) break;
	    gzfile_read_more(gz);
	    p = RSTRING_PTR(gz->z.buf) + n - rslen;
	}
	if (!rspara) rscheck(rsptr, rslen, rs);
	filled = gz->z.buf_filled;
	if (limit > 0 && filled >= limit) {
	    filled = limit;
	}
	res = memchr(p, rsptr[0], (filled - n + 1));
	if (!res) {
	    n = filled;
	    if (limit > 0 && filled >= limit) break;
	    n++;
	} else {
	    n += (long)(res - p);
	    p = res;
	    if (rslen == 1 || memcmp(p, rsptr, rslen) == 0) break;
	    p++, n++;
	}
    }
    if (maxlen > 1 && n == limit && (gz->z.buf_filled > n || !ZSTREAM_IS_FINISHED(&gz->z))) {
	n = gzreader_charboundary(gz, n);
    }

    gz->lineno++;
    dst = gzfile_read(gz, n);
    if (rspara) {
	gzreader_skip_linebreaks(gz);
    }

    return gzfile_newstr(gz, dst);
}

/*
 * Document-method: Zlib::GzipReader#gets
 *
 * See Zlib::GzipReader documentation for a description.
 */
static VALUE
rb_gzreader_gets(int argc, VALUE *argv, VALUE obj)
{
    VALUE dst;
    dst = gzreader_gets(argc, argv, obj);
    if (!NIL_P(dst)) {
	rb_lastline_set(dst);
    }
    return dst;
}

/*
 * Document-method: Zlib::GzipReader#readline
 *
 * See Zlib::GzipReader documentation for a description.
 */
static VALUE
rb_gzreader_readline(int argc, VALUE *argv, VALUE obj)
{
    VALUE dst;
    dst = rb_gzreader_gets(argc, argv, obj);
    if (NIL_P(dst)) {
	rb_raise(rb_eEOFError, "end of file reached");
    }
    return dst;
}

/*
 * Document-method: Zlib::GzipReader#each
 *
 * See Zlib::GzipReader documentation for a description.
 */
static VALUE
rb_gzreader_each(int argc, VALUE *argv, VALUE obj)
{
    VALUE str;

    RETURN_ENUMERATOR(obj, 0, 0);

    while (!NIL_P(str = gzreader_gets(argc, argv, obj))) {
	rb_yield(str);
    }
    return obj;
}

/*
 * Document-method: Zlib::GzipReader#readlines
 *
 * See Zlib::GzipReader documentation for a description.
 */
static VALUE
rb_gzreader_readlines(int argc, VALUE *argv, VALUE obj)
{
    VALUE str, dst;
    dst = rb_ary_new();
    while (!NIL_P(str = gzreader_gets(argc, argv, obj))) {
	rb_ary_push(dst, str);
    }
    return dst;
}

#endif /* GZIP_SUPPORT */

void
Init_zlib()
{
    VALUE mZlib, cZStream, cDeflate, cInflate;
#if GZIP_SUPPORT
    VALUE cGzipFile, cGzipWriter, cGzipReader;
#endif

    mZlib = rb_define_module("Zlib");

    id_dictionaries = rb_intern("@dictionaries");

    cZError = rb_define_class_under(mZlib, "Error", rb_eStandardError);
    cStreamEnd    = rb_define_class_under(mZlib, "StreamEnd", cZError);
    cNeedDict     = rb_define_class_under(mZlib, "NeedDict", cZError);
    cDataError    = rb_define_class_under(mZlib, "DataError", cZError);
    cStreamError  = rb_define_class_under(mZlib, "StreamError", cZError);
    cMemError     = rb_define_class_under(mZlib, "MemError", cZError);
    cBufError     = rb_define_class_under(mZlib, "BufError", cZError);
    cVersionError = rb_define_class_under(mZlib, "VersionError", cZError);

    rb_define_module_function(mZlib, "zlib_version", rb_zlib_version, 0);
    rb_define_module_function(mZlib, "adler32", rb_zlib_adler32, -1);
    rb_define_module_function(mZlib, "adler32_combine", rb_zlib_adler32_combine, 3);
    rb_define_module_function(mZlib, "crc32", rb_zlib_crc32, -1);
    rb_define_module_function(mZlib, "crc32_combine", rb_zlib_crc32_combine, 3);
    rb_define_module_function(mZlib, "crc_table", rb_zlib_crc_table, 0);

    /* The Ruby/zlib version string. */
    rb_define_const(mZlib, "VERSION", rb_str_new2(RUBY_ZLIB_VERSION));
    /*  The string which represents the version of zlib.h */
    rb_define_const(mZlib, "ZLIB_VERSION", rb_str_new2(ZLIB_VERSION));

    cZStream = rb_define_class_under(mZlib, "ZStream", rb_cObject);
    rb_undef_alloc_func(cZStream);
    rb_define_method(cZStream, "avail_out", rb_zstream_avail_out, 0);
    rb_define_method(cZStream, "avail_out=", rb_zstream_set_avail_out, 1);
    rb_define_method(cZStream, "avail_in", rb_zstream_avail_in, 0);
    rb_define_method(cZStream, "total_in", rb_zstream_total_in, 0);
    rb_define_method(cZStream, "total_out", rb_zstream_total_out, 0);
    rb_define_method(cZStream, "data_type", rb_zstream_data_type, 0);
    rb_define_method(cZStream, "adler", rb_zstream_adler, 0);
    rb_define_method(cZStream, "finished?", rb_zstream_finished_p, 0);
    rb_define_method(cZStream, "stream_end?", rb_zstream_finished_p, 0);
    rb_define_method(cZStream, "closed?", rb_zstream_closed_p, 0);
    rb_define_method(cZStream, "ended?", rb_zstream_closed_p, 0);
    rb_define_method(cZStream, "close", rb_zstream_end, 0);
    rb_define_method(cZStream, "end", rb_zstream_end, 0);
    rb_define_method(cZStream, "reset", rb_zstream_reset, 0);
    rb_define_method(cZStream, "finish", rb_zstream_finish, 0);
    rb_define_method(cZStream, "flush_next_in", rb_zstream_flush_next_in, 0);
    rb_define_method(cZStream, "flush_next_out", rb_zstream_flush_next_out, 0);

    /* Represents binary data as guessed by deflate.
     *
     * See Zlib::Deflate#data_type. */
    rb_define_const(mZlib, "BINARY", INT2FIX(Z_BINARY));

    /* Represents text data as guessed by deflate.
     *
     * NOTE: The underlying constant Z_ASCII was deprecated in favor of Z_TEXT
     * in zlib 1.2.2.  New applications should not use this constant.
     *
     * See Zlib::Deflate#data_type. */
    rb_define_const(mZlib, "ASCII", INT2FIX(Z_ASCII));

#ifdef Z_TEXT
    /* Represents text data as guessed by deflate.
     *
     * See Zlib::Deflate#data_type. */
    rb_define_const(mZlib, "TEXT", INT2FIX(Z_TEXT));
#endif

    /* Represents an unknown data type as guessed by deflate.
     *
     * See Zlib::Deflate#data_type. */
    rb_define_const(mZlib, "UNKNOWN", INT2FIX(Z_UNKNOWN));

    cDeflate = rb_define_class_under(mZlib, "Deflate", cZStream);
    rb_define_singleton_method(cDeflate, "deflate", rb_deflate_s_deflate, -1);
    rb_define_singleton_method(mZlib, "deflate", rb_deflate_s_deflate, -1);
    rb_define_alloc_func(cDeflate, rb_deflate_s_allocate);
    rb_define_method(cDeflate, "initialize", rb_deflate_initialize, -1);
    rb_define_method(cDeflate, "initialize_copy", rb_deflate_init_copy, 1);
    rb_define_method(cDeflate, "deflate", rb_deflate_deflate, -1);
    rb_define_method(cDeflate, "<<", rb_deflate_addstr, 1);
    rb_define_method(cDeflate, "flush", rb_deflate_flush, -1);
    rb_define_method(cDeflate, "params", rb_deflate_params, 2);
    rb_define_method(cDeflate, "set_dictionary", rb_deflate_set_dictionary, 1);

    cInflate = rb_define_class_under(mZlib, "Inflate", cZStream);
    rb_define_singleton_method(cInflate, "inflate", rb_inflate_s_inflate, 1);
    rb_define_singleton_method(mZlib, "inflate", rb_inflate_s_inflate, 1);
    rb_define_alloc_func(cInflate, rb_inflate_s_allocate);
    rb_define_method(cInflate, "initialize", rb_inflate_initialize, -1);
    rb_define_method(cInflate, "add_dictionary", rb_inflate_add_dictionary, 1);
    rb_define_method(cInflate, "inflate", rb_inflate_inflate, 1);
    rb_define_method(cInflate, "<<", rb_inflate_addstr, 1);
    rb_define_method(cInflate, "sync", rb_inflate_sync, 1);
    rb_define_method(cInflate, "sync_point?", rb_inflate_sync_point_p, 0);
    rb_define_method(cInflate, "set_dictionary", rb_inflate_set_dictionary, 1);

    /* No compression, passes through data untouched.  Use this for appending
     * pre-compressed data to a deflate stream.
     */
    rb_define_const(mZlib, "NO_COMPRESSION", INT2FIX(Z_NO_COMPRESSION));
    /* Fastest compression level, but with with lowest space savings. */
    rb_define_const(mZlib, "BEST_SPEED", INT2FIX(Z_BEST_SPEED));
    /* Slowest compression level, but with the best space savings. */
    rb_define_const(mZlib, "BEST_COMPRESSION", INT2FIX(Z_BEST_COMPRESSION));
    /* Default compression level which is a good trade-off between space and
     * time
     */
    rb_define_const(mZlib, "DEFAULT_COMPRESSION",
		    INT2FIX(Z_DEFAULT_COMPRESSION));

    /* Deflate strategy for data produced by a filter (or predictor). The
     * effect of FILTERED is to force more Huffman codes and less string
     * matching; it is somewhat intermediate between DEFAULT_STRATEGY and
     * HUFFMAN_ONLY. Filtered data consists mostly of small values with a
     * somewhat random distribution.
     */
    rb_define_const(mZlib, "FILTERED", INT2FIX(Z_FILTERED));

    /* Deflate strategy which uses Huffman codes only (no string matching). */
    rb_define_const(mZlib, "HUFFMAN_ONLY", INT2FIX(Z_HUFFMAN_ONLY));

#ifdef Z_RLE
    /* Deflate compression strategy designed to be almost as fast as
     * HUFFMAN_ONLY, but give better compression for PNG image data.
     */
    rb_define_const(mZlib, "RLE", INT2FIX(Z_RLE));
#endif

#ifdef Z_FIXED
    /* Deflate strategy which prevents the use of dynamic Huffman codes,
     * allowing for a simpler decoder for specialized applications.
     */
    rb_define_const(mZlib, "FIXED", INT2FIX(Z_FIXED));
#endif

    /* Default deflate strategy which is used for normal data. */
    rb_define_const(mZlib, "DEFAULT_STRATEGY", INT2FIX(Z_DEFAULT_STRATEGY));

    /* The maximum size of the zlib history buffer.  Note that zlib allows
     * larger values to enable different inflate modes.  See Zlib::Inflate.new
     * for details.
     */
    rb_define_const(mZlib, "MAX_WBITS", INT2FIX(MAX_WBITS));

    /* The default memory level for allocating zlib deflate compression state.
     */
    rb_define_const(mZlib, "DEF_MEM_LEVEL", INT2FIX(DEF_MEM_LEVEL));

    /* The maximum memory level for allocating zlib deflate compression state.
     */
    rb_define_const(mZlib, "MAX_MEM_LEVEL", INT2FIX(MAX_MEM_LEVEL));

    /* NO_FLUSH is the default flush method and allows deflate to decide how
     * much data to accumulate before producing output in order to maximize
     * compression.
     */
    rb_define_const(mZlib, "NO_FLUSH", INT2FIX(Z_NO_FLUSH));

    /* The SYNC_FLUSH method flushes all pending output to the output buffer
     * and the output is aligned on a byte boundary. Flushing may degrade
     * compression so it should be used only when necessary, such as at a
     * request or response boundary for a network stream.
     */
    rb_define_const(mZlib, "SYNC_FLUSH", INT2FIX(Z_SYNC_FLUSH));

    /* Flushes all output as with SYNC_FLUSH, and the compression state is
     * reset so that decompression can restart from this point if previous
     * compressed data has been damaged or if random access is desired. Like
     * SYNC_FLUSH, using FULL_FLUSH too often can seriously degrade
     * compression.
     */
    rb_define_const(mZlib, "FULL_FLUSH", INT2FIX(Z_FULL_FLUSH));

    /* Processes all pending input and flushes pending output. */
    rb_define_const(mZlib, "FINISH", INT2FIX(Z_FINISH));

#if GZIP_SUPPORT
    id_write = rb_intern("write");
    id_read = rb_intern("read");
    id_readpartial = rb_intern("readpartial");
    id_flush = rb_intern("flush");
    id_seek = rb_intern("seek");
    id_close = rb_intern("close");
    id_path = rb_intern("path");
    id_input = rb_intern("@input");

    cGzipFile = rb_define_class_under(mZlib, "GzipFile", rb_cObject);
    cGzError = rb_define_class_under(cGzipFile, "Error", cZError);

    /* input gzipped string */
    rb_define_attr(cGzError, "input", 1, 0);
    rb_define_method(cGzError, "inspect", gzfile_error_inspect, 0);

    cNoFooter = rb_define_class_under(cGzipFile, "NoFooter", cGzError);
    cCRCError = rb_define_class_under(cGzipFile, "CRCError", cGzError);
    cLengthError = rb_define_class_under(cGzipFile,"LengthError",cGzError);

    cGzipWriter = rb_define_class_under(mZlib, "GzipWriter", cGzipFile);
    cGzipReader = rb_define_class_under(mZlib, "GzipReader", cGzipFile);
    rb_include_module(cGzipReader, rb_mEnumerable);

    rb_define_singleton_method(cGzipFile, "wrap", rb_gzfile_s_wrap, -1);
    rb_undef_alloc_func(cGzipFile);
    rb_define_method(cGzipFile, "to_io", rb_gzfile_to_io, 0);
    rb_define_method(cGzipFile, "crc", rb_gzfile_crc, 0);
    rb_define_method(cGzipFile, "mtime", rb_gzfile_mtime, 0);
    rb_define_method(cGzipFile, "level", rb_gzfile_level, 0);
    rb_define_method(cGzipFile, "os_code", rb_gzfile_os_code, 0);
    rb_define_method(cGzipFile, "orig_name", rb_gzfile_orig_name, 0);
    rb_define_method(cGzipFile, "comment", rb_gzfile_comment, 0);
    rb_define_method(cGzipReader, "lineno", rb_gzfile_lineno, 0);
    rb_define_method(cGzipReader, "lineno=", rb_gzfile_set_lineno, 1);
    rb_define_method(cGzipWriter, "mtime=", rb_gzfile_set_mtime, 1);
    rb_define_method(cGzipWriter, "orig_name=", rb_gzfile_set_orig_name,1);
    rb_define_method(cGzipWriter, "comment=", rb_gzfile_set_comment, 1);
    rb_define_method(cGzipFile, "close", rb_gzfile_close, 0);
    rb_define_method(cGzipFile, "finish", rb_gzfile_finish, 0);
    rb_define_method(cGzipFile, "closed?", rb_gzfile_closed_p, 0);
    rb_define_method(cGzipReader, "eof", rb_gzfile_eof_p, 0);
    rb_define_method(cGzipReader, "eof?", rb_gzfile_eof_p, 0);
    rb_define_method(cGzipFile, "sync", rb_gzfile_sync, 0);
    rb_define_method(cGzipFile, "sync=", rb_gzfile_set_sync, 1);
    rb_define_method(cGzipReader, "pos", rb_gzfile_total_out, 0);
    rb_define_method(cGzipWriter, "pos", rb_gzfile_total_in, 0);
    rb_define_method(cGzipReader, "tell", rb_gzfile_total_out, 0);
    rb_define_method(cGzipWriter, "tell", rb_gzfile_total_in, 0);

    rb_define_singleton_method(cGzipWriter, "open", rb_gzwriter_s_open,-1);
    rb_define_alloc_func(cGzipWriter, rb_gzwriter_s_allocate);
    rb_define_method(cGzipWriter, "initialize", rb_gzwriter_initialize,-1);
    rb_define_method(cGzipWriter, "flush", rb_gzwriter_flush, -1);
    rb_define_method(cGzipWriter, "write", rb_gzwriter_write, 1);
    rb_define_method(cGzipWriter, "putc", rb_gzwriter_putc, 1);
    rb_define_method(cGzipWriter, "<<", rb_gzwriter_addstr, 1);
    rb_define_method(cGzipWriter, "printf", rb_gzwriter_printf, -1);
    rb_define_method(cGzipWriter, "print", rb_gzwriter_print, -1);
    rb_define_method(cGzipWriter, "puts", rb_gzwriter_puts, -1);

    rb_define_singleton_method(cGzipReader, "open", rb_gzreader_s_open,-1);
    rb_define_alloc_func(cGzipReader, rb_gzreader_s_allocate);
    rb_define_method(cGzipReader, "initialize", rb_gzreader_initialize, -1);
    rb_define_method(cGzipReader, "rewind", rb_gzreader_rewind, 0);
    rb_define_method(cGzipReader, "unused", rb_gzreader_unused, 0);
    rb_define_method(cGzipReader, "read", rb_gzreader_read, -1);
    rb_define_method(cGzipReader, "readpartial", rb_gzreader_readpartial, -1);
    rb_define_method(cGzipReader, "getc", rb_gzreader_getc, 0);
    rb_define_method(cGzipReader, "getbyte", rb_gzreader_getbyte, 0);
    rb_define_method(cGzipReader, "readchar", rb_gzreader_readchar, 0);
    rb_define_method(cGzipReader, "readbyte", rb_gzreader_readbyte, 0);
    rb_define_method(cGzipReader, "each_byte", rb_gzreader_each_byte, 0);
    rb_define_method(cGzipReader, "each_char", rb_gzreader_each_char, 0);
    rb_define_method(cGzipReader, "bytes", rb_gzreader_each_byte, 0);
    rb_define_method(cGzipReader, "ungetc", rb_gzreader_ungetc, 1);
    rb_define_method(cGzipReader, "ungetbyte", rb_gzreader_ungetbyte, 1);
    rb_define_method(cGzipReader, "gets", rb_gzreader_gets, -1);
    rb_define_method(cGzipReader, "readline", rb_gzreader_readline, -1);
    rb_define_method(cGzipReader, "each", rb_gzreader_each, -1);
    rb_define_method(cGzipReader, "each_line", rb_gzreader_each, -1);
    rb_define_method(cGzipReader, "lines", rb_gzreader_each, -1);
    rb_define_method(cGzipReader, "readlines", rb_gzreader_readlines, -1);

    /* The OS code of current host */
    rb_define_const(mZlib, "OS_CODE", INT2FIX(OS_CODE));
    /* OS code for MSDOS hosts */
    rb_define_const(mZlib, "OS_MSDOS", INT2FIX(OS_MSDOS));
    /* OS code for Amiga hosts */
    rb_define_const(mZlib, "OS_AMIGA", INT2FIX(OS_AMIGA));
    /* OS code for VMS hosts */
    rb_define_const(mZlib, "OS_VMS", INT2FIX(OS_VMS));
    /* OS code for UNIX hosts */
    rb_define_const(mZlib, "OS_UNIX", INT2FIX(OS_UNIX));
    /* OS code for Atari hosts */
    rb_define_const(mZlib, "OS_ATARI", INT2FIX(OS_ATARI));
    /* OS code for OS2 hosts */
    rb_define_const(mZlib, "OS_OS2", INT2FIX(OS_OS2));
    /* OS code for Mac OS hosts */
    rb_define_const(mZlib, "OS_MACOS", INT2FIX(OS_MACOS));
    /* OS code for TOPS-20 hosts */
    rb_define_const(mZlib, "OS_TOPS20", INT2FIX(OS_TOPS20));
    /* OS code for Win32 hosts */
    rb_define_const(mZlib, "OS_WIN32", INT2FIX(OS_WIN32));
    /* OS code for VM OS hosts */
    rb_define_const(mZlib, "OS_VMCMS", INT2FIX(OS_VMCMS));
    /* OS code for Z-System hosts */
    rb_define_const(mZlib, "OS_ZSYSTEM", INT2FIX(OS_ZSYSTEM));
    /* OS code for CP/M hosts */
    rb_define_const(mZlib, "OS_CPM", INT2FIX(OS_CPM));
    /* OS code for QDOS hosts */
    rb_define_const(mZlib, "OS_QDOS", INT2FIX(OS_QDOS));
    /* OS code for RISC OS hosts */
    rb_define_const(mZlib, "OS_RISCOS", INT2FIX(OS_RISCOS));
    /* OS code for unknown hosts */
    rb_define_const(mZlib, "OS_UNKNOWN", INT2FIX(OS_UNKNOWN));

#endif /* GZIP_SUPPORT */
}

/* Document error classes. */

/*
 * Document-class: Zlib::Error
 *
 * The superclass for all exceptions raised by Ruby/zlib.
 *
 * The following exceptions are defined as subclasses of Zlib::Error. These
 * exceptions are raised when zlib library functions return with an error
 * status.
 *
 * - Zlib::StreamEnd
 * - Zlib::NeedDict
 * - Zlib::DataError
 * - Zlib::StreamError
 * - Zlib::MemError
 * - Zlib::BufError
 * - Zlib::VersionError
 *
 */

/*
 * Document-class: Zlib::StreamEnd
 *
 * Subclass of Zlib::Error
 *
 * When zlib returns a Z_STREAM_END
 * is return if the end of the compressed data has been reached
 * and all uncompressed out put has been produced.
 *
 */

/*
 * Document-class: Zlib::NeedDict
 *
 * Subclass of Zlib::Error
 *
 * When zlib returns a Z_NEED_DICT
 * if a preset dictionary is needed at this point.
 *
 * Used by Zlib::Inflate.inflate and <tt>Zlib.inflate</tt>
 */

/*
 * Document-class: Zlib::VersionError
 *
 * Subclass of Zlib::Error
 *
 * When zlib returns a Z_VERSION_ERROR,
 * usually if the zlib library version is incompatible with the
 * version assumed by the caller.
 *
 */

/*
 * Document-class: Zlib::MemError
 *
 * Subclass of Zlib::Error
 *
 * When zlib returns a Z_MEM_ERROR,
 * usually if there was not enough memory.
 *
 */

/*
 * Document-class: Zlib::StreamError
 *
 * Subclass of Zlib::Error
 *
 * When zlib returns a Z_STREAM_ERROR,
 * usually if the stream state was inconsistent.
 *
 */

/*
 * Document-class: Zlib::BufError
 *
 * Subclass of Zlib::Error when zlib returns a Z_BUF_ERROR.
 *
 * Usually if no progress is possible.
 *
 */

/*
 * Document-class: Zlib::DataError
 *
 * Subclass of Zlib::Error when zlib returns a Z_DATA_ERROR.
 *
 * Usually if a stream was prematurely freed.
 *
 */

/*
 * Document-class: Zlib::GzipFile::Error
 *
 * Base class of errors that occur when processing GZIP files.
 */

/*
 * Document-class: Zlib::GzipFile::NoFooter
 *
 * Raised when gzip file footer is not found.
 */

/*
 * Document-class: Zlib::GzipFile::CRCError
 *
 * Raised when the CRC checksum recorded in gzip file footer is not equivalent
 * to the CRC checksum of the actual uncompressed data.
 */

/*
 * Document-class: Zlib::GzipFile::LengthError
 *
 * Raised when the data length recorded in the gzip file footer is not equivalent
 * to the length of the actual uncompressed data.
 */


