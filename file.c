/**********************************************************************

  file.c -

  $Author$
  created at: Mon Nov 15 12:24:34 JST 1993

  Copyright (C) 1993-2007 Yukihiro Matsumoto
  Copyright (C) 2000  Network Applied Communication Laboratory, Inc.
  Copyright (C) 2000  Information-technology Promotion Agency, Japan

**********************************************************************/

#ifdef _WIN32
#include "missing/file.h"
#endif
#ifdef __CYGWIN__
#include <windows.h>
#include <sys/cygwin.h>
#include <wchar.h>
#endif

#include "ruby/ruby.h"
#include "ruby/io.h"
#include "ruby/util.h"
#include "dln.h"
#include "internal.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_FILE_H
# include <sys/file.h>
#else
int flock(int, int);
#endif

#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif
#ifndef MAXPATHLEN
# define MAXPATHLEN 1024
#endif

#include <ctype.h>

#include <time.h>

#ifdef HAVE_UTIME_H
#include <utime.h>
#elif defined HAVE_SYS_UTIME_H
#include <sys/utime.h>
#endif

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_SYS_MKDEV_H
#include <sys/mkdev.h>
#endif

#if defined(HAVE_FCNTL_H)
#include <fcntl.h>
#endif

#if !defined HAVE_LSTAT && !defined lstat
#define lstat stat
#endif

/* define system APIs */
#ifdef _WIN32
#define STAT(p, s)	rb_w32_ustati64((p), (s))
#undef lstat
#define lstat(p, s)	rb_w32_ustati64((p), (s))
#undef access
#define access(p, m)	rb_w32_uaccess((p), (m))
#undef chmod
#define chmod(p, m)	rb_w32_uchmod((p), (m))
#undef chown
#define chown(p, o, g)	rb_w32_uchown((p), (o), (g))
#undef utime
#define utime(p, t)	rb_w32_uutime((p), (t))
#undef link
#define link(f, t)	rb_w32_ulink((f), (t))
#undef unlink
#define unlink(p)	rb_w32_uunlink(p)
#undef rename
#define rename(f, t)	rb_w32_urename((f), (t))
#else
#define STAT(p, s)	stat((p), (s))
#endif

#define rb_sys_fail_path(path) rb_sys_fail_str(path)

#if defined(__BEOS__) || defined(__HAIKU__) /* should not change ID if -1 */
static int
be_chown(const char *path, uid_t owner, gid_t group)
{
    if (owner == (uid_t)-1 || group == (gid_t)-1) {
	struct stat st;
	if (STAT(path, &st) < 0) return -1;
	if (owner == (uid_t)-1) owner = st.st_uid;
	if (group == (gid_t)-1) group = st.st_gid;
    }
    return chown(path, owner, group);
}
#define chown be_chown
static int
be_fchown(int fd, uid_t owner, gid_t group)
{
    if (owner == (uid_t)-1 || group == (gid_t)-1) {
	struct stat st;
	if (fstat(fd, &st) < 0) return -1;
	if (owner == (uid_t)-1) owner = st.st_uid;
	if (group == (gid_t)-1) group = st.st_gid;
    }
    return fchown(fd, owner, group);
}
#define fchown be_fchown
#endif /* __BEOS__ || __HAIKU__ */

VALUE rb_cFile;
VALUE rb_mFileTest;
VALUE rb_cStat;

#define insecure_obj_p(obj, level) ((level) >= 4 || ((level) > 0 && OBJ_TAINTED(obj)))

static VALUE
file_path_convert(VALUE name)
{
#ifndef _WIN32 /* non Windows == Unix */
    rb_encoding *fname_encoding = rb_enc_from_index(ENCODING_GET(name));
    rb_encoding *fs_encoding;
    if (rb_default_internal_encoding() != NULL
	    && rb_usascii_encoding() != fname_encoding
	    && rb_ascii8bit_encoding() != fname_encoding
	    && (fs_encoding = rb_filesystem_encoding()) != fname_encoding
	    && !rb_enc_str_asciionly_p(name)) {
	/* Don't call rb_filesystem_encoding() before US-ASCII and ASCII-8BIT */
	/* fs_encoding should be ascii compatible */
	name = rb_str_conv_enc(name, fname_encoding, fs_encoding);
    }
#endif
    return name;
}

static VALUE
rb_get_path_check(VALUE obj, int level)
{
    VALUE tmp;
    ID to_path;
    rb_encoding *enc;

    if (insecure_obj_p(obj, level)) {
	rb_insecure_operation();
    }

    CONST_ID(to_path, "to_path");
    tmp = rb_check_funcall(obj, to_path, 0, 0);
    if (tmp == Qundef) {
	tmp = obj;
    }
    StringValue(tmp);

    tmp = file_path_convert(tmp);
    if (obj != tmp && insecure_obj_p(tmp, level)) {
	rb_insecure_operation();
    }
    enc = rb_enc_get(tmp);
    if (!rb_enc_asciicompat(enc)) {
	tmp = rb_str_inspect(tmp);
	rb_raise(rb_eEncCompatError, "path name must be ASCII-compatible (%s): %s",
		 rb_enc_name(enc), RSTRING_PTR(tmp));
    }
    return rb_str_new4(tmp);
}

VALUE
rb_get_path_no_checksafe(VALUE obj)
{
    return rb_get_path_check(obj, 0);
}

VALUE
rb_get_path(VALUE obj)
{
    return rb_get_path_check(obj, rb_safe_level());
}

VALUE
rb_str_encode_ospath(VALUE path)
{
#ifdef _WIN32
    rb_encoding *enc = rb_enc_get(path);
    if (enc != rb_ascii8bit_encoding()) {
	rb_encoding *utf8 = rb_utf8_encoding();
	if (enc != utf8)
	    path = rb_str_encode(path, rb_enc_from_encoding(utf8), 0, Qnil);
    }
    else if (RSTRING_LEN(path) > 0) {
	path = rb_str_dup(path);
	rb_enc_associate(path, rb_filesystem_encoding());
	path = rb_str_encode(path, rb_enc_from_encoding(rb_utf8_encoding()), 0, Qnil);
    }
#endif
    return path;
}

static long
apply2files(void (*func)(const char *, VALUE, void *), VALUE vargs, void *arg)
{
    long i;
    volatile VALUE path;

    rb_secure(4);
    for (i=0; i<RARRAY_LEN(vargs); i++) {
	const char *s;
	path = rb_get_path(RARRAY_PTR(vargs)[i]);
	path = rb_str_encode_ospath(path);
	s = RSTRING_PTR(path);
	(*func)(s, path, arg);
    }

    return RARRAY_LEN(vargs);
}

/*
 *  call-seq:
 *     file.path  ->  filename
 *
 *  Returns the pathname used to create <i>file</i> as a string. Does
 *  not normalize the name.
 *
 *     File.new("testfile").path               #=> "testfile"
 *     File.new("/tmp/../tmp/xxx", "w").path   #=> "/tmp/../tmp/xxx"
 *
 */

static VALUE
rb_file_path(VALUE obj)
{
    rb_io_t *fptr;

    fptr = RFILE(rb_io_taint_check(obj))->fptr;
    rb_io_check_initialized(fptr);
    if (NIL_P(fptr->pathv)) return Qnil;
    return rb_obj_taint(rb_str_dup(fptr->pathv));
}

static size_t
stat_memsize(const void *p)
{
    return p ? sizeof(struct stat) : 0;
}

static const rb_data_type_t stat_data_type = {
    "stat",
    {NULL, RUBY_TYPED_DEFAULT_FREE, stat_memsize,},
};

static VALUE
stat_new_0(VALUE klass, struct stat *st)
{
    struct stat *nst = 0;

    if (st) {
	nst = ALLOC(struct stat);
	*nst = *st;
    }
    return TypedData_Wrap_Struct(klass, &stat_data_type, nst);
}

static VALUE
stat_new(struct stat *st)
{
    return stat_new_0(rb_cStat, st);
}

static struct stat*
get_stat(VALUE self)
{
    struct stat* st;
    TypedData_Get_Struct(self, struct stat, &stat_data_type, st);
    if (!st) rb_raise(rb_eTypeError, "uninitialized File::Stat");
    return st;
}

static struct timespec stat_mtimespec(struct stat *st);

/*
 *  call-seq:
 *     stat <=> other_stat    -> -1, 0, 1, nil
 *
 *  Compares <code>File::Stat</code> objects by comparing their
 *  respective modification times.
 *
 *     f1 = File.new("f1", "w")
 *     sleep 1
 *     f2 = File.new("f2", "w")
 *     f1.stat <=> f2.stat   #=> -1
 */

static VALUE
rb_stat_cmp(VALUE self, VALUE other)
{
    if (rb_obj_is_kind_of(other, rb_obj_class(self))) {
        struct timespec ts1 = stat_mtimespec(get_stat(self));
        struct timespec ts2 = stat_mtimespec(get_stat(other));
        if (ts1.tv_sec == ts2.tv_sec) {
            if (ts1.tv_nsec == ts2.tv_nsec) return INT2FIX(0);
            if (ts1.tv_nsec < ts2.tv_nsec) return INT2FIX(-1);
            return INT2FIX(1);
        }
        if (ts1.tv_sec < ts2.tv_sec) return INT2FIX(-1);
        return INT2FIX(1);
    }
    return Qnil;
}

#define ST2UINT(val) ((val) & ~(~1UL << (sizeof(val) * CHAR_BIT - 1)))

#ifndef NUM2DEVT
# define NUM2DEVT(v) NUM2UINT(v)
#endif
#ifndef DEVT2NUM
# define DEVT2NUM(v) UINT2NUM(v)
#endif
#ifndef PRI_DEVT_PREFIX
# define PRI_DEVT_PREFIX ""
#endif

/*
 *  call-seq:
 *     stat.dev    -> fixnum
 *
 *  Returns an integer representing the device on which <i>stat</i>
 *  resides.
 *
 *     File.stat("testfile").dev   #=> 774
 */

static VALUE
rb_stat_dev(VALUE self)
{
    return DEVT2NUM(get_stat(self)->st_dev);
}

/*
 *  call-seq:
 *     stat.dev_major   -> fixnum
 *
 *  Returns the major part of <code>File_Stat#dev</code> or
 *  <code>nil</code>.
 *
 *     File.stat("/dev/fd1").dev_major   #=> 2
 *     File.stat("/dev/tty").dev_major   #=> 5
 */

static VALUE
rb_stat_dev_major(VALUE self)
{
#if defined(major)
    return INT2NUM(major(get_stat(self)->st_dev));
#else
    return Qnil;
#endif
}

/*
 *  call-seq:
 *     stat.dev_minor   -> fixnum
 *
 *  Returns the minor part of <code>File_Stat#dev</code> or
 *  <code>nil</code>.
 *
 *     File.stat("/dev/fd1").dev_minor   #=> 1
 *     File.stat("/dev/tty").dev_minor   #=> 0
 */

static VALUE
rb_stat_dev_minor(VALUE self)
{
#if defined(minor)
    return INT2NUM(minor(get_stat(self)->st_dev));
#else
    return Qnil;
#endif
}

/*
 *  call-seq:
 *     stat.ino   -> fixnum
 *
 *  Returns the inode number for <i>stat</i>.
 *
 *     File.stat("testfile").ino   #=> 1083669
 *
 */

static VALUE
rb_stat_ino(VALUE self)
{
#if SIZEOF_STRUCT_STAT_ST_INO > SIZEOF_LONG
    return ULL2NUM(get_stat(self)->st_ino);
#else
    return ULONG2NUM(get_stat(self)->st_ino);
#endif
}

/*
 *  call-seq:
 *     stat.mode   -> fixnum
 *
 *  Returns an integer representing the permission bits of
 *  <i>stat</i>. The meaning of the bits is platform dependent; on
 *  Unix systems, see <code>stat(2)</code>.
 *
 *     File.chmod(0644, "testfile")   #=> 1
 *     s = File.stat("testfile")
 *     sprintf("%o", s.mode)          #=> "100644"
 */

static VALUE
rb_stat_mode(VALUE self)
{
    return UINT2NUM(ST2UINT(get_stat(self)->st_mode));
}

/*
 *  call-seq:
 *     stat.nlink   -> fixnum
 *
 *  Returns the number of hard links to <i>stat</i>.
 *
 *     File.stat("testfile").nlink             #=> 1
 *     File.link("testfile", "testfile.bak")   #=> 0
 *     File.stat("testfile").nlink             #=> 2
 *
 */

static VALUE
rb_stat_nlink(VALUE self)
{
    return UINT2NUM(get_stat(self)->st_nlink);
}

/*
 *  call-seq:
 *     stat.uid    -> fixnum
 *
 *  Returns the numeric user id of the owner of <i>stat</i>.
 *
 *     File.stat("testfile").uid   #=> 501
 *
 */

static VALUE
rb_stat_uid(VALUE self)
{
    return UIDT2NUM(get_stat(self)->st_uid);
}

/*
 *  call-seq:
 *     stat.gid   -> fixnum
 *
 *  Returns the numeric group id of the owner of <i>stat</i>.
 *
 *     File.stat("testfile").gid   #=> 500
 *
 */

static VALUE
rb_stat_gid(VALUE self)
{
    return GIDT2NUM(get_stat(self)->st_gid);
}

/*
 *  call-seq:
 *     stat.rdev   ->  fixnum or nil
 *
 *  Returns an integer representing the device type on which
 *  <i>stat</i> resides. Returns <code>nil</code> if the operating
 *  system doesn't support this feature.
 *
 *     File.stat("/dev/fd1").rdev   #=> 513
 *     File.stat("/dev/tty").rdev   #=> 1280
 */

static VALUE
rb_stat_rdev(VALUE self)
{
#ifdef HAVE_ST_RDEV
    return DEVT2NUM(get_stat(self)->st_rdev);
#else
    return Qnil;
#endif
}

/*
 *  call-seq:
 *     stat.rdev_major   -> fixnum
 *
 *  Returns the major part of <code>File_Stat#rdev</code> or
 *  <code>nil</code>.
 *
 *     File.stat("/dev/fd1").rdev_major   #=> 2
 *     File.stat("/dev/tty").rdev_major   #=> 5
 */

static VALUE
rb_stat_rdev_major(VALUE self)
{
#if defined(HAVE_ST_RDEV) && defined(major)
    return DEVT2NUM(major(get_stat(self)->st_rdev));
#else
    return Qnil;
#endif
}

/*
 *  call-seq:
 *     stat.rdev_minor   -> fixnum
 *
 *  Returns the minor part of <code>File_Stat#rdev</code> or
 *  <code>nil</code>.
 *
 *     File.stat("/dev/fd1").rdev_minor   #=> 1
 *     File.stat("/dev/tty").rdev_minor   #=> 0
 */

static VALUE
rb_stat_rdev_minor(VALUE self)
{
#if defined(HAVE_ST_RDEV) && defined(minor)
    return DEVT2NUM(minor(get_stat(self)->st_rdev));
#else
    return Qnil;
#endif
}

/*
 *  call-seq:
 *     stat.size    -> fixnum
 *
 *  Returns the size of <i>stat</i> in bytes.
 *
 *     File.stat("testfile").size   #=> 66
 */

static VALUE
rb_stat_size(VALUE self)
{
    return OFFT2NUM(get_stat(self)->st_size);
}

/*
 *  call-seq:
 *     stat.blksize   -> integer or nil
 *
 *  Returns the native file system's block size. Will return <code>nil</code>
 *  on platforms that don't support this information.
 *
 *     File.stat("testfile").blksize   #=> 4096
 *
 */

static VALUE
rb_stat_blksize(VALUE self)
{
#ifdef HAVE_ST_BLKSIZE
    return ULONG2NUM(get_stat(self)->st_blksize);
#else
    return Qnil;
#endif
}

/*
 *  call-seq:
 *     stat.blocks    -> integer or nil
 *
 *  Returns the number of native file system blocks allocated for this
 *  file, or <code>nil</code> if the operating system doesn't
 *  support this feature.
 *
 *     File.stat("testfile").blocks   #=> 2
 */

static VALUE
rb_stat_blocks(VALUE self)
{
#ifdef HAVE_STRUCT_STAT_ST_BLOCKS
# if SIZEOF_STRUCT_STAT_ST_BLOCKS > SIZEOF_LONG
    return ULL2NUM(get_stat(self)->st_blocks);
# else
    return ULONG2NUM(get_stat(self)->st_blocks);
# endif
#else
    return Qnil;
#endif
}

static struct timespec
stat_atimespec(struct stat *st)
{
    struct timespec ts;
    ts.tv_sec = st->st_atime;
#if defined(HAVE_STRUCT_STAT_ST_ATIM)
    ts.tv_nsec = st->st_atim.tv_nsec;
#elif defined(HAVE_STRUCT_STAT_ST_ATIMESPEC)
    ts.tv_nsec = st->st_atimespec.tv_nsec;
#elif defined(HAVE_STRUCT_STAT_ST_ATIMENSEC)
    ts.tv_nsec = st->st_atimensec;
#else
    ts.tv_nsec = 0;
#endif
    return ts;
}

static VALUE
stat_atime(struct stat *st)
{
    struct timespec ts = stat_atimespec(st);
    return rb_time_nano_new(ts.tv_sec, ts.tv_nsec);
}

static struct timespec
stat_mtimespec(struct stat *st)
{
    struct timespec ts;
    ts.tv_sec = st->st_mtime;
#if defined(HAVE_STRUCT_STAT_ST_MTIM)
    ts.tv_nsec = st->st_mtim.tv_nsec;
#elif defined(HAVE_STRUCT_STAT_ST_MTIMESPEC)
    ts.tv_nsec = st->st_mtimespec.tv_nsec;
#elif defined(HAVE_STRUCT_STAT_ST_MTIMENSEC)
    ts.tv_nsec = st->st_mtimensec;
#else
    ts.tv_nsec = 0;
#endif
    return ts;
}

static VALUE
stat_mtime(struct stat *st)
{
    struct timespec ts = stat_mtimespec(st);
    return rb_time_nano_new(ts.tv_sec, ts.tv_nsec);
}

static struct timespec
stat_ctimespec(struct stat *st)
{
    struct timespec ts;
    ts.tv_sec = st->st_ctime;
#if defined(HAVE_STRUCT_STAT_ST_CTIM)
    ts.tv_nsec = st->st_ctim.tv_nsec;
#elif defined(HAVE_STRUCT_STAT_ST_CTIMESPEC)
    ts.tv_nsec = st->st_ctimespec.tv_nsec;
#elif defined(HAVE_STRUCT_STAT_ST_CTIMENSEC)
    ts.tv_nsec = st->st_ctimensec;
#else
    ts.tv_nsec = 0;
#endif
    return ts;
}

static VALUE
stat_ctime(struct stat *st)
{
    struct timespec ts = stat_ctimespec(st);
    return rb_time_nano_new(ts.tv_sec, ts.tv_nsec);
}

/*
 *  call-seq:
 *     stat.atime   -> time
 *
 *  Returns the last access time for this file as an object of class
 *  <code>Time</code>.
 *
 *     File.stat("testfile").atime   #=> Wed Dec 31 18:00:00 CST 1969
 *
 */

static VALUE
rb_stat_atime(VALUE self)
{
    return stat_atime(get_stat(self));
}

/*
 *  call-seq:
 *     stat.mtime  ->  aTime
 *
 *  Returns the modification time of <i>stat</i>.
 *
 *     File.stat("testfile").mtime   #=> Wed Apr 09 08:53:14 CDT 2003
 *
 */

static VALUE
rb_stat_mtime(VALUE self)
{
    return stat_mtime(get_stat(self));
}

/*
 *  call-seq:
 *     stat.ctime  ->  aTime
 *
 *  Returns the change time for <i>stat</i> (that is, the time
 *  directory information about the file was changed, not the file
 *  itself).
 *
 *  Note that on Windows (NTFS), returns creation time (birth time).
 *
 *     File.stat("testfile").ctime   #=> Wed Apr 09 08:53:14 CDT 2003
 *
 */

static VALUE
rb_stat_ctime(VALUE self)
{
    return stat_ctime(get_stat(self));
}

/*
 * call-seq:
 *   stat.inspect  ->  string
 *
 * Produce a nicely formatted description of <i>stat</i>.
 *
 *   File.stat("/etc/passwd").inspect
 *      #=> "#<File::Stat dev=0xe000005, ino=1078078, mode=0100644,
 *      #    nlink=1, uid=0, gid=0, rdev=0x0, size=1374, blksize=4096,
 *      #    blocks=8, atime=Wed Dec 10 10:16:12 CST 2003,
 *      #    mtime=Fri Sep 12 15:41:41 CDT 2003,
 *      #    ctime=Mon Oct 27 11:20:27 CST 2003>"
 */

static VALUE
rb_stat_inspect(VALUE self)
{
    VALUE str;
    size_t i;
    static const struct {
	const char *name;
	VALUE (*func)(VALUE);
    } member[] = {
	{"dev",	    rb_stat_dev},
	{"ino",	    rb_stat_ino},
	{"mode",    rb_stat_mode},
	{"nlink",   rb_stat_nlink},
	{"uid",	    rb_stat_uid},
	{"gid",	    rb_stat_gid},
	{"rdev",    rb_stat_rdev},
	{"size",    rb_stat_size},
	{"blksize", rb_stat_blksize},
	{"blocks",  rb_stat_blocks},
	{"atime",   rb_stat_atime},
	{"mtime",   rb_stat_mtime},
	{"ctime",   rb_stat_ctime},
    };

    struct stat* st;
    TypedData_Get_Struct(self, struct stat, &stat_data_type, st);
    if (!st) {
        return rb_sprintf("#<%s: uninitialized>", rb_obj_classname(self));
    }

    str = rb_str_buf_new2("#<");
    rb_str_buf_cat2(str, rb_obj_classname(self));
    rb_str_buf_cat2(str, " ");

    for (i = 0; i < sizeof(member)/sizeof(member[0]); i++) {
	VALUE v;

	if (i > 0) {
	    rb_str_buf_cat2(str, ", ");
	}
	rb_str_buf_cat2(str, member[i].name);
	rb_str_buf_cat2(str, "=");
	v = (*member[i].func)(self);
	if (i == 2) {		/* mode */
	    rb_str_catf(str, "0%lo", (unsigned long)NUM2ULONG(v));
	}
	else if (i == 0 || i == 6) { /* dev/rdev */
	    rb_str_catf(str, "0x%"PRI_DEVT_PREFIX"x", NUM2DEVT(v));
	}
	else {
	    rb_str_append(str, rb_inspect(v));
	}
    }
    rb_str_buf_cat2(str, ">");
    OBJ_INFECT(str, self);

    return str;
}

static int
rb_stat(VALUE file, struct stat *st)
{
    VALUE tmp;

    rb_secure(2);
    tmp = rb_check_convert_type(file, T_FILE, "IO", "to_io");
    if (!NIL_P(tmp)) {
	rb_io_t *fptr;

	GetOpenFile(tmp, fptr);
	return fstat(fptr->fd, st);
    }
    FilePathValue(file);
    file = rb_str_encode_ospath(file);
    return STAT(StringValueCStr(file), st);
}

#ifdef _WIN32
static HANDLE
w32_io_info(VALUE *file, BY_HANDLE_FILE_INFORMATION *st)
{
    VALUE tmp;
    HANDLE f, ret = 0;

    tmp = rb_check_convert_type(*file, T_FILE, "IO", "to_io");
    if (!NIL_P(tmp)) {
	rb_io_t *fptr;

	GetOpenFile(tmp, fptr);
	f = (HANDLE)rb_w32_get_osfhandle(fptr->fd);
	if (f == (HANDLE)-1) return INVALID_HANDLE_VALUE;
    }
    else {
	VALUE tmp;
	WCHAR *ptr;
	int len;
	VALUE v;

	FilePathValue(*file);
	tmp = rb_str_encode_ospath(*file);
	len = MultiByteToWideChar(CP_UTF8, 0, RSTRING_PTR(tmp), -1, NULL, 0);
	ptr = ALLOCV_N(WCHAR, v, len);
	MultiByteToWideChar(CP_UTF8, 0, RSTRING_PTR(tmp), -1, ptr, len);
	f = CreateFileW(ptr, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
			rb_w32_iswin95() ? 0 : FILE_FLAG_BACKUP_SEMANTICS,
			NULL);
	ALLOCV_END(v);
	if (f == INVALID_HANDLE_VALUE) return f;
	ret = f;
    }
    if (GetFileType(f) == FILE_TYPE_DISK) {
	ZeroMemory(st, sizeof(*st));
	if (GetFileInformationByHandle(f, st)) return ret;
    }
    if (ret) CloseHandle(ret);
    return INVALID_HANDLE_VALUE;
}
#endif

/*
 *  call-seq:
 *     File.stat(file_name)   ->  stat
 *
 *  Returns a <code>File::Stat</code> object for the named file (see
 *  <code>File::Stat</code>).
 *
 *     File.stat("testfile").mtime   #=> Tue Apr 08 12:58:04 CDT 2003
 *
 */

static VALUE
rb_file_s_stat(VALUE klass, VALUE fname)
{
    struct stat st;

    rb_secure(4);
    FilePathValue(fname);
    if (rb_stat(fname, &st) < 0) {
	rb_sys_fail_path(fname);
    }
    return stat_new(&st);
}

/*
 *  call-seq:
 *     ios.stat    -> stat
 *
 *  Returns status information for <em>ios</em> as an object of type
 *  <code>File::Stat</code>.
 *
 *     f = File.new("testfile")
 *     s = f.stat
 *     "%o" % s.mode   #=> "100644"
 *     s.blksize       #=> 4096
 *     s.atime         #=> Wed Apr 09 08:53:54 CDT 2003
 *
 */

static VALUE
rb_io_stat(VALUE obj)
{
    rb_io_t *fptr;
    struct stat st;

    GetOpenFile(obj, fptr);
    if (fstat(fptr->fd, &st) == -1) {
	rb_sys_fail_path(fptr->pathv);
    }
    return stat_new(&st);
}

/*
 *  call-seq:
 *     File.lstat(file_name)   -> stat
 *
 *  Same as <code>File::stat</code>, but does not follow the last symbolic
 *  link. Instead, reports on the link itself.
 *
 *     File.symlink("testfile", "link2test")   #=> 0
 *     File.stat("testfile").size              #=> 66
 *     File.lstat("link2test").size            #=> 8
 *     File.stat("link2test").size             #=> 66
 *
 */

static VALUE
rb_file_s_lstat(VALUE klass, VALUE fname)
{
#ifdef HAVE_LSTAT
    struct stat st;

    rb_secure(2);
    FilePathValue(fname);
    fname = rb_str_encode_ospath(fname);
    if (lstat(StringValueCStr(fname), &st) == -1) {
	rb_sys_fail_path(fname);
    }
    return stat_new(&st);
#else
    return rb_file_s_stat(klass, fname);
#endif
}

/*
 *  call-seq:
 *     file.lstat   ->  stat
 *
 *  Same as <code>IO#stat</code>, but does not follow the last symbolic
 *  link. Instead, reports on the link itself.
 *
 *     File.symlink("testfile", "link2test")   #=> 0
 *     File.stat("testfile").size              #=> 66
 *     f = File.new("link2test")
 *     f.lstat.size                            #=> 8
 *     f.stat.size                             #=> 66
 */

static VALUE
rb_file_lstat(VALUE obj)
{
#ifdef HAVE_LSTAT
    rb_io_t *fptr;
    struct stat st;
    VALUE path;

    rb_secure(2);
    GetOpenFile(obj, fptr);
    if (NIL_P(fptr->pathv)) return Qnil;
    path = rb_str_encode_ospath(fptr->pathv);
    if (lstat(RSTRING_PTR(path), &st) == -1) {
	rb_sys_fail_path(fptr->pathv);
    }
    return stat_new(&st);
#else
    return rb_io_stat(obj);
#endif
}

static int
rb_group_member(GETGROUPS_T gid)
{
    int rv = FALSE;
#ifndef _WIN32
    if (getgid() == gid || getegid() == gid)
	return TRUE;

# ifdef HAVE_GETGROUPS
#  ifndef NGROUPS
#   ifdef NGROUPS_MAX
#    define NGROUPS NGROUPS_MAX
#   else
#    define NGROUPS 32
#   endif
#  endif
    {
	GETGROUPS_T *gary;
	int anum;

	gary = xmalloc(NGROUPS * sizeof(GETGROUPS_T));
	anum = getgroups(NGROUPS, gary);
	while (--anum >= 0) {
	    if (gary[anum] == gid) {
		rv = TRUE;
		break;
	    }
	}
	xfree(gary);
    }
# endif
#endif
    return rv;
}

#ifndef S_IXUGO
#  define S_IXUGO		(S_IXUSR | S_IXGRP | S_IXOTH)
#endif

#if defined(S_IXGRP) && !defined(_WIN32) && !defined(__CYGWIN__)
#define USE_GETEUID 1
#endif

#ifndef HAVE_EACCESS
int
eaccess(const char *path, int mode)
{
#ifdef USE_GETEUID
    struct stat st;
    rb_uid_t euid;

    if (STAT(path, &st) < 0)
	return -1;

    euid = geteuid();

    if (euid == 0) {
	/* Root can read or write any file. */
	if (!(mode & X_OK))
	    return 0;

	/* Root can execute any file that has any one of the execute
	   bits set. */
	if (st.st_mode & S_IXUGO)
	    return 0;

	return -1;
    }

    if (st.st_uid == euid)        /* owner */
	mode <<= 6;
    else if (rb_group_member(st.st_gid))
	mode <<= 3;

    if ((int)(st.st_mode & mode) == mode) return 0;

    return -1;
#else
    return access(path, mode);
#endif
}
#endif

static inline int
access_internal(const char *path, int mode)
{
    return access(path, mode);
}


/*
 * Document-class: FileTest
 *
 *  <code>FileTest</code> implements file test operations similar to
 *  those used in <code>File::Stat</code>. It exists as a standalone
 *  module, and its methods are also insinuated into the <code>File</code>
 *  class. (Note that this is not done by inclusion: the interpreter cheats).
 *
 */

/*
 * Document-method: exist?
 *
 * call-seq:
 *   Dir.exist?(file_name)   ->  true or false
 *   Dir.exists?(file_name)   ->  true or false
 *
 * Returns <code>true</code> if the named file is a directory,
 * <code>false</code> otherwise.
 *
 */

/*
 * Document-method: directory?
 *
 * call-seq:
 *   File.directory?(file_name)   ->  true or false
 *
 * Returns <code>true</code> if the named file is a directory,
 * or a symlink that points at a directory, and <code>false</code>
 * otherwise.
 *
 *    File.directory?(".")
 */

VALUE
rb_file_directory_p(VALUE obj, VALUE fname)
{
#ifndef S_ISDIR
#   define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

    struct stat st;

    if (rb_stat(fname, &st) < 0) return Qfalse;
    if (S_ISDIR(st.st_mode)) return Qtrue;
    return Qfalse;
}

/*
 * call-seq:
 *   File.pipe?(file_name)   ->  true or false
 *
 * Returns <code>true</code> if the named file is a pipe.
 */

static VALUE
rb_file_pipe_p(VALUE obj, VALUE fname)
{
#ifdef S_IFIFO
#  ifndef S_ISFIFO
#    define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#  endif

    struct stat st;

    if (rb_stat(fname, &st) < 0) return Qfalse;
    if (S_ISFIFO(st.st_mode)) return Qtrue;

#endif
    return Qfalse;
}

/*
 * call-seq:
 *   File.symlink?(file_name)   ->  true or false
 *
 * Returns <code>true</code> if the named file is a symbolic link.
 */

static VALUE
rb_file_symlink_p(VALUE obj, VALUE fname)
{
#ifndef S_ISLNK
#  ifdef _S_ISLNK
#    define S_ISLNK(m) _S_ISLNK(m)
#  else
#    ifdef _S_IFLNK
#      define S_ISLNK(m) (((m) & S_IFMT) == _S_IFLNK)
#    else
#      ifdef S_IFLNK
#	 define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#      endif
#    endif
#  endif
#endif

#ifdef S_ISLNK
    struct stat st;

    rb_secure(2);
    FilePathValue(fname);
    fname = rb_str_encode_ospath(fname);
    if (lstat(StringValueCStr(fname), &st) < 0) return Qfalse;
    if (S_ISLNK(st.st_mode)) return Qtrue;
#endif

    return Qfalse;
}

/*
 * call-seq:
 *   File.socket?(file_name)   ->  true or false
 *
 * Returns <code>true</code> if the named file is a socket.
 */

static VALUE
rb_file_socket_p(VALUE obj, VALUE fname)
{
#ifndef S_ISSOCK
#  ifdef _S_ISSOCK
#    define S_ISSOCK(m) _S_ISSOCK(m)
#  else
#    ifdef _S_IFSOCK
#      define S_ISSOCK(m) (((m) & S_IFMT) == _S_IFSOCK)
#    else
#      ifdef S_IFSOCK
#	 define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#      endif
#    endif
#  endif
#endif

#ifdef S_ISSOCK
    struct stat st;

    if (rb_stat(fname, &st) < 0) return Qfalse;
    if (S_ISSOCK(st.st_mode)) return Qtrue;

#endif
    return Qfalse;
}

/*
 * call-seq:
 *   File.blockdev?(file_name)   ->  true or false
 *
 * Returns <code>true</code> if the named file is a block device.
 */

static VALUE
rb_file_blockdev_p(VALUE obj, VALUE fname)
{
#ifndef S_ISBLK
#   ifdef S_IFBLK
#	define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#   else
#	define S_ISBLK(m) (0)  /* anytime false */
#   endif
#endif

#ifdef S_ISBLK
    struct stat st;

    if (rb_stat(fname, &st) < 0) return Qfalse;
    if (S_ISBLK(st.st_mode)) return Qtrue;

#endif
    return Qfalse;
}

/*
 * call-seq:
 *   File.chardev?(file_name)   ->  true or false
 *
 * Returns <code>true</code> if the named file is a character device.
 */
static VALUE
rb_file_chardev_p(VALUE obj, VALUE fname)
{
#ifndef S_ISCHR
#   define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#endif

    struct stat st;

    if (rb_stat(fname, &st) < 0) return Qfalse;
    if (S_ISCHR(st.st_mode)) return Qtrue;

    return Qfalse;
}

/*
 * call-seq:
 *    File.exist?(file_name)    ->  true or false
 *    File.exists?(file_name)   ->  true or false
 *
 * Return <code>true</code> if the named file exists.
 */

static VALUE
rb_file_exist_p(VALUE obj, VALUE fname)
{
    struct stat st;

    if (rb_stat(fname, &st) < 0) return Qfalse;
    return Qtrue;
}

/*
 * call-seq:
 *    File.readable?(file_name)   -> true or false
 *
 * Returns <code>true</code> if the named file is readable by the effective
 * user id of this process.
 */

static VALUE
rb_file_readable_p(VALUE obj, VALUE fname)
{
    rb_secure(2);
    FilePathValue(fname);
    fname = rb_str_encode_ospath(fname);
    if (eaccess(StringValueCStr(fname), R_OK) < 0) return Qfalse;
    return Qtrue;
}

/*
 * call-seq:
 *    File.readable_real?(file_name)   -> true or false
 *
 * Returns <code>true</code> if the named file is readable by the real
 * user id of this process.
 */

static VALUE
rb_file_readable_real_p(VALUE obj, VALUE fname)
{
    rb_secure(2);
    FilePathValue(fname);
    fname = rb_str_encode_ospath(fname);
    if (access_internal(StringValueCStr(fname), R_OK) < 0) return Qfalse;
    return Qtrue;
}

#ifndef S_IRUGO
#  define S_IRUGO		(S_IRUSR | S_IRGRP | S_IROTH)
#endif

#ifndef S_IWUGO
#  define S_IWUGO		(S_IWUSR | S_IWGRP | S_IWOTH)
#endif

/*
 * call-seq:
 *    File.world_readable?(file_name)   -> fixnum or nil
 *
 * If <i>file_name</i> is readable by others, returns an integer
 * representing the file permission bits of <i>file_name</i>. Returns
 * <code>nil</code> otherwise. The meaning of the bits is platform
 * dependent; on Unix systems, see <code>stat(2)</code>.
 *
 *    File.world_readable?("/etc/passwd")	    #=> 420
 *    m = File.world_readable?("/etc/passwd")
 *    sprintf("%o", m)				    #=> "644"
 */

static VALUE
rb_file_world_readable_p(VALUE obj, VALUE fname)
{
#ifdef S_IROTH
    struct stat st;

    if (rb_stat(fname, &st) < 0) return Qnil;
    if ((st.st_mode & (S_IROTH)) == S_IROTH) {
	return UINT2NUM(st.st_mode & (S_IRUGO|S_IWUGO|S_IXUGO));
    }
#endif
    return Qnil;
}

/*
 * call-seq:
 *    File.writable?(file_name)   -> true or false
 *
 * Returns <code>true</code> if the named file is writable by the effective
 * user id of this process.
 */

static VALUE
rb_file_writable_p(VALUE obj, VALUE fname)
{
    rb_secure(2);
    FilePathValue(fname);
    fname = rb_str_encode_ospath(fname);
    if (eaccess(StringValueCStr(fname), W_OK) < 0) return Qfalse;
    return Qtrue;
}

/*
 * call-seq:
 *    File.writable_real?(file_name)   -> true or false
 *
 * Returns <code>true</code> if the named file is writable by the real
 * user id of this process.
 */

static VALUE
rb_file_writable_real_p(VALUE obj, VALUE fname)
{
    rb_secure(2);
    FilePathValue(fname);
    fname = rb_str_encode_ospath(fname);
    if (access_internal(StringValueCStr(fname), W_OK) < 0) return Qfalse;
    return Qtrue;
}

/*
 * call-seq:
 *    File.world_writable?(file_name)   -> fixnum or nil
 *
 * If <i>file_name</i> is writable by others, returns an integer
 * representing the file permission bits of <i>file_name</i>. Returns
 * <code>nil</code> otherwise. The meaning of the bits is platform
 * dependent; on Unix systems, see <code>stat(2)</code>.
 *
 *    File.world_writable?("/tmp")		    #=> 511
 *    m = File.world_writable?("/tmp")
 *    sprintf("%o", m)				    #=> "777"
 */

static VALUE
rb_file_world_writable_p(VALUE obj, VALUE fname)
{
#ifdef S_IWOTH
    struct stat st;

    if (rb_stat(fname, &st) < 0) return Qnil;
    if ((st.st_mode & (S_IWOTH)) == S_IWOTH) {
	return UINT2NUM(st.st_mode & (S_IRUGO|S_IWUGO|S_IXUGO));
    }
#endif
    return Qnil;
}

/*
 * call-seq:
 *    File.executable?(file_name)   -> true or false
 *
 * Returns <code>true</code> if the named file is executable by the effective
 * user id of this process.
 */

static VALUE
rb_file_executable_p(VALUE obj, VALUE fname)
{
    rb_secure(2);
    FilePathValue(fname);
    fname = rb_str_encode_ospath(fname);
    if (eaccess(StringValueCStr(fname), X_OK) < 0) return Qfalse;
    return Qtrue;
}

/*
 * call-seq:
 *    File.executable_real?(file_name)   -> true or false
 *
 * Returns <code>true</code> if the named file is executable by the real
 * user id of this process.
 */

static VALUE
rb_file_executable_real_p(VALUE obj, VALUE fname)
{
    rb_secure(2);
    FilePathValue(fname);
    fname = rb_str_encode_ospath(fname);
    if (access_internal(StringValueCStr(fname), X_OK) < 0) return Qfalse;
    return Qtrue;
}

#ifndef S_ISREG
#   define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

/*
 * call-seq:
 *    File.file?(file_name)   -> true or false
 *
 * Returns <code>true</code> if the named file exists and is a
 * regular file.
 */

static VALUE
rb_file_file_p(VALUE obj, VALUE fname)
{
    struct stat st;

    if (rb_stat(fname, &st) < 0) return Qfalse;
    if (S_ISREG(st.st_mode)) return Qtrue;
    return Qfalse;
}

/*
 * call-seq:
 *    File.zero?(file_name)   -> true or false
 *
 * Returns <code>true</code> if the named file exists and has
 * a zero size.
 */

static VALUE
rb_file_zero_p(VALUE obj, VALUE fname)
{
    struct stat st;

    if (rb_stat(fname, &st) < 0) return Qfalse;
    if (st.st_size == 0) return Qtrue;
    return Qfalse;
}

/*
 * call-seq:
 *    File.size?(file_name)   -> Integer or nil
 *
 * Returns +nil+ if +file_name+ doesn't exist or has zero size, the size of the
 * file otherwise.
 */

static VALUE
rb_file_size_p(VALUE obj, VALUE fname)
{
    struct stat st;

    if (rb_stat(fname, &st) < 0) return Qnil;
    if (st.st_size == 0) return Qnil;
    return OFFT2NUM(st.st_size);
}

/*
 * call-seq:
 *    File.owned?(file_name)   -> true or false
 *
 * Returns <code>true</code> if the named file exists and the
 * effective used id of the calling process is the owner of
 * the file.
 */

static VALUE
rb_file_owned_p(VALUE obj, VALUE fname)
{
    struct stat st;

    if (rb_stat(fname, &st) < 0) return Qfalse;
    if (st.st_uid == geteuid()) return Qtrue;
    return Qfalse;
}

static VALUE
rb_file_rowned_p(VALUE obj, VALUE fname)
{
    struct stat st;

    if (rb_stat(fname, &st) < 0) return Qfalse;
    if (st.st_uid == getuid()) return Qtrue;
    return Qfalse;
}

/*
 * call-seq:
 *    File.grpowned?(file_name)   -> true or false
 *
 * Returns <code>true</code> if the named file exists and the
 * effective group id of the calling process is the owner of
 * the file. Returns <code>false</code> on Windows.
 */

static VALUE
rb_file_grpowned_p(VALUE obj, VALUE fname)
{
#ifndef _WIN32
    struct stat st;

    if (rb_stat(fname, &st) < 0) return Qfalse;
    if (rb_group_member(st.st_gid)) return Qtrue;
#endif
    return Qfalse;
}

#if defined(S_ISUID) || defined(S_ISGID) || defined(S_ISVTX)
static VALUE
check3rdbyte(VALUE fname, int mode)
{
    struct stat st;

    rb_secure(2);
    FilePathValue(fname);
    fname = rb_str_encode_ospath(fname);
    if (STAT(StringValueCStr(fname), &st) < 0) return Qfalse;
    if (st.st_mode & mode) return Qtrue;
    return Qfalse;
}
#endif

/*
 * call-seq:
 *   File.setuid?(file_name)   ->  true or false
 *
 * Returns <code>true</code> if the named file has the setuid bit set.
 */

static VALUE
rb_file_suid_p(VALUE obj, VALUE fname)
{
#ifdef S_ISUID
    return check3rdbyte(fname, S_ISUID);
#else
    return Qfalse;
#endif
}

/*
 * call-seq:
 *   File.setgid?(file_name)   ->  true or false
 *
 * Returns <code>true</code> if the named file has the setgid bit set.
 */

static VALUE
rb_file_sgid_p(VALUE obj, VALUE fname)
{
#ifdef S_ISGID
    return check3rdbyte(fname, S_ISGID);
#else
    return Qfalse;
#endif
}

/*
 * call-seq:
 *   File.sticky?(file_name)   ->  true or false
 *
 * Returns <code>true</code> if the named file has the sticky bit set.
 */

static VALUE
rb_file_sticky_p(VALUE obj, VALUE fname)
{
#ifdef S_ISVTX
    return check3rdbyte(fname, S_ISVTX);
#else
    return Qnil;
#endif
}

/*
 * call-seq:
 *   File.identical?(file_1, file_2)   ->  true or false
 *
 * Returns <code>true</code> if the named files are identical.
 *
 *     open("a", "w") {}
 *     p File.identical?("a", "a")      #=> true
 *     p File.identical?("a", "./a")    #=> true
 *     File.link("a", "b")
 *     p File.identical?("a", "b")      #=> true
 *     File.symlink("a", "c")
 *     p File.identical?("a", "c")      #=> true
 *     open("d", "w") {}
 *     p File.identical?("a", "d")      #=> false
 */

static VALUE
rb_file_identical_p(VALUE obj, VALUE fname1, VALUE fname2)
{
#ifndef DOSISH
    struct stat st1, st2;

    if (rb_stat(fname1, &st1) < 0) return Qfalse;
    if (rb_stat(fname2, &st2) < 0) return Qfalse;
    if (st1.st_dev != st2.st_dev) return Qfalse;
    if (st1.st_ino != st2.st_ino) return Qfalse;
#else
# ifdef _WIN32
    BY_HANDLE_FILE_INFORMATION st1, st2;
    HANDLE f1 = 0, f2 = 0;
# endif

    rb_secure(2);
# ifdef _WIN32
    f1 = w32_io_info(&fname1, &st1);
    if (f1 == INVALID_HANDLE_VALUE) return Qfalse;
    f2 = w32_io_info(&fname2, &st2);
    if (f1) CloseHandle(f1);
    if (f2 == INVALID_HANDLE_VALUE) return Qfalse;
    if (f2) CloseHandle(f2);

    if (st1.dwVolumeSerialNumber == st2.dwVolumeSerialNumber &&
	st1.nFileIndexHigh == st2.nFileIndexHigh &&
	st1.nFileIndexLow == st2.nFileIndexLow)
	return Qtrue;
    if (!f1 || !f2) return Qfalse;
    if (rb_w32_iswin95()) return Qfalse;
# else
    FilePathValue(fname1);
    fname1 = rb_str_new4(fname1);
    fname1 = rb_str_encode_ospath(fname1);
    FilePathValue(fname2);
    fname2 = rb_str_encode_ospath(fname2);
    if (access(RSTRING_PTR(fname1), 0)) return Qfalse;
    if (access(RSTRING_PTR(fname2), 0)) return Qfalse;
# endif
    fname1 = rb_file_expand_path(fname1, Qnil);
    fname2 = rb_file_expand_path(fname2, Qnil);
    if (RSTRING_LEN(fname1) != RSTRING_LEN(fname2)) return Qfalse;
    if (rb_memcicmp(RSTRING_PTR(fname1), RSTRING_PTR(fname2), RSTRING_LEN(fname1)))
	return Qfalse;
#endif
    return Qtrue;
}

/*
 * call-seq:
 *    File.size(file_name)   -> integer
 *
 * Returns the size of <code>file_name</code>.
 */

static VALUE
rb_file_s_size(VALUE klass, VALUE fname)
{
    struct stat st;

    if (rb_stat(fname, &st) < 0) {
	FilePathValue(fname);
	rb_sys_fail_path(fname);
    }
    return OFFT2NUM(st.st_size);
}

static VALUE
rb_file_ftype(const struct stat *st)
{
    const char *t;

    if (S_ISREG(st->st_mode)) {
	t = "file";
    }
    else if (S_ISDIR(st->st_mode)) {
	t = "directory";
    }
    else if (S_ISCHR(st->st_mode)) {
	t = "characterSpecial";
    }
#ifdef S_ISBLK
    else if (S_ISBLK(st->st_mode)) {
	t = "blockSpecial";
    }
#endif
#ifdef S_ISFIFO
    else if (S_ISFIFO(st->st_mode)) {
	t = "fifo";
    }
#endif
#ifdef S_ISLNK
    else if (S_ISLNK(st->st_mode)) {
	t = "link";
    }
#endif
#ifdef S_ISSOCK
    else if (S_ISSOCK(st->st_mode)) {
	t = "socket";
    }
#endif
    else {
	t = "unknown";
    }

    return rb_usascii_str_new2(t);
}

/*
 *  call-seq:
 *     File.ftype(file_name)   -> string
 *
 *  Identifies the type of the named file; the return string is one of
 *  ``<code>file</code>'', ``<code>directory</code>'',
 *  ``<code>characterSpecial</code>'', ``<code>blockSpecial</code>'',
 *  ``<code>fifo</code>'', ``<code>link</code>'',
 *  ``<code>socket</code>'', or ``<code>unknown</code>''.
 *
 *     File.ftype("testfile")            #=> "file"
 *     File.ftype("/dev/tty")            #=> "characterSpecial"
 *     File.ftype("/tmp/.X11-unix/X0")   #=> "socket"
 */

static VALUE
rb_file_s_ftype(VALUE klass, VALUE fname)
{
    struct stat st;

    rb_secure(2);
    FilePathValue(fname);
    fname = rb_str_encode_ospath(fname);
    if (lstat(StringValueCStr(fname), &st) == -1) {
	rb_sys_fail_path(fname);
    }

    return rb_file_ftype(&st);
}

/*
 *  call-seq:
 *     File.atime(file_name)  ->  time
 *
 *  Returns the last access time for the named file as a Time object).
 *
 *     File.atime("testfile")   #=> Wed Apr 09 08:51:48 CDT 2003
 *
 */

static VALUE
rb_file_s_atime(VALUE klass, VALUE fname)
{
    struct stat st;

    if (rb_stat(fname, &st) < 0) {
	FilePathValue(fname);
	rb_sys_fail_path(fname);
    }
    return stat_atime(&st);
}

/*
 *  call-seq:
 *     file.atime    -> time
 *
 *  Returns the last access time (a <code>Time</code> object)
 *   for <i>file</i>, or epoch if <i>file</i> has not been accessed.
 *
 *     File.new("testfile").atime   #=> Wed Dec 31 18:00:00 CST 1969
 *
 */

static VALUE
rb_file_atime(VALUE obj)
{
    rb_io_t *fptr;
    struct stat st;

    GetOpenFile(obj, fptr);
    if (fstat(fptr->fd, &st) == -1) {
	rb_sys_fail_path(fptr->pathv);
    }
    return stat_atime(&st);
}

/*
 *  call-seq:
 *     File.mtime(file_name)  ->  time
 *
 *  Returns the modification time for the named file as a Time object.
 *
 *     File.mtime("testfile")   #=> Tue Apr 08 12:58:04 CDT 2003
 *
 */

static VALUE
rb_file_s_mtime(VALUE klass, VALUE fname)
{
    struct stat st;

    if (rb_stat(fname, &st) < 0) {
	FilePathValue(fname);
	rb_sys_fail_path(fname);
    }
    return stat_mtime(&st);
}

/*
 *  call-seq:
 *     file.mtime  ->  time
 *
 *  Returns the modification time for <i>file</i>.
 *
 *     File.new("testfile").mtime   #=> Wed Apr 09 08:53:14 CDT 2003
 *
 */

static VALUE
rb_file_mtime(VALUE obj)
{
    rb_io_t *fptr;
    struct stat st;

    GetOpenFile(obj, fptr);
    if (fstat(fptr->fd, &st) == -1) {
	rb_sys_fail_path(fptr->pathv);
    }
    return stat_mtime(&st);
}

/*
 *  call-seq:
 *     File.ctime(file_name)  -> time
 *
 *  Returns the change time for the named file (the time at which
 *  directory information about the file was changed, not the file
 *  itself).
 *
 *  Note that on Windows (NTFS), returns creation time (birth time).
 *
 *     File.ctime("testfile")   #=> Wed Apr 09 08:53:13 CDT 2003
 *
 */

static VALUE
rb_file_s_ctime(VALUE klass, VALUE fname)
{
    struct stat st;

    if (rb_stat(fname, &st) < 0) {
	FilePathValue(fname);
	rb_sys_fail_path(fname);
    }
    return stat_ctime(&st);
}

/*
 *  call-seq:
 *     file.ctime  ->  time
 *
 *  Returns the change time for <i>file</i> (that is, the time directory
 *  information about the file was changed, not the file itself).
 *
 *  Note that on Windows (NTFS), returns creation time (birth time).
 *
 *     File.new("testfile").ctime   #=> Wed Apr 09 08:53:14 CDT 2003
 *
 */

static VALUE
rb_file_ctime(VALUE obj)
{
    rb_io_t *fptr;
    struct stat st;

    GetOpenFile(obj, fptr);
    if (fstat(fptr->fd, &st) == -1) {
	rb_sys_fail_path(fptr->pathv);
    }
    return stat_ctime(&st);
}

/*
 *  call-seq:
 *     file.size    -> integer
 *
 *  Returns the size of <i>file</i> in bytes.
 *
 *     File.new("testfile").size   #=> 66
 *
 */

static VALUE
rb_file_size(VALUE obj)
{
    rb_io_t *fptr;
    struct stat st;

    GetOpenFile(obj, fptr);
    if (fptr->mode & FMODE_WRITABLE) {
	rb_io_flush(obj);
    }
    if (fstat(fptr->fd, &st) == -1) {
	rb_sys_fail_path(fptr->pathv);
    }
    return OFFT2NUM(st.st_size);
}

static void
chmod_internal(const char *path, VALUE pathv, void *mode)
{
    if (chmod(path, *(int *)mode) < 0)
	rb_sys_fail_path(pathv);
}

/*
 *  call-seq:
 *     File.chmod(mode_int, file_name, ... )  ->  integer
 *
 *  Changes permission bits on the named file(s) to the bit pattern
 *  represented by <i>mode_int</i>. Actual effects are operating system
 *  dependent (see the beginning of this section). On Unix systems, see
 *  <code>chmod(2)</code> for details. Returns the number of files
 *  processed.
 *
 *     File.chmod(0644, "testfile", "out")   #=> 2
 */

static VALUE
rb_file_s_chmod(int argc, VALUE *argv)
{
    VALUE vmode;
    VALUE rest;
    int mode;
    long n;

    rb_secure(2);
    rb_scan_args(argc, argv, "1*", &vmode, &rest);
    mode = NUM2INT(vmode);

    n = apply2files(chmod_internal, rest, &mode);
    return LONG2FIX(n);
}

/*
 *  call-seq:
 *     file.chmod(mode_int)   -> 0
 *
 *  Changes permission bits on <i>file</i> to the bit pattern
 *  represented by <i>mode_int</i>. Actual effects are platform
 *  dependent; on Unix systems, see <code>chmod(2)</code> for details.
 *  Follows symbolic links. Also see <code>File#lchmod</code>.
 *
 *     f = File.new("out", "w");
 *     f.chmod(0644)   #=> 0
 */

static VALUE
rb_file_chmod(VALUE obj, VALUE vmode)
{
    rb_io_t *fptr;
    int mode;
#ifndef HAVE_FCHMOD
    VALUE path;
#endif

    rb_secure(2);
    mode = NUM2INT(vmode);

    GetOpenFile(obj, fptr);
#ifdef HAVE_FCHMOD
    if (fchmod(fptr->fd, mode) == -1)
	rb_sys_fail_path(fptr->pathv);
#else
    if (NIL_P(fptr->pathv)) return Qnil;
    path = rb_str_encode_ospath(fptr->pathv);
    if (chmod(RSTRING_PTR(path), mode) == -1)
	rb_sys_fail_path(fptr->pathv);
#endif

    return INT2FIX(0);
}

#if defined(HAVE_LCHMOD)
static void
lchmod_internal(const char *path, VALUE pathv, void *mode)
{
    if (lchmod(path, (int)(VALUE)mode) < 0)
	rb_sys_fail_path(pathv);
}

/*
 *  call-seq:
 *     File.lchmod(mode_int, file_name, ...)  -> integer
 *
 *  Equivalent to <code>File::chmod</code>, but does not follow symbolic
 *  links (so it will change the permissions associated with the link,
 *  not the file referenced by the link). Often not available.
 *
 */

static VALUE
rb_file_s_lchmod(int argc, VALUE *argv)
{
    VALUE vmode;
    VALUE rest;
    long mode, n;

    rb_secure(2);
    rb_scan_args(argc, argv, "1*", &vmode, &rest);
    mode = NUM2INT(vmode);

    n = apply2files(lchmod_internal, rest, (void *)(long)mode);
    return LONG2FIX(n);
}
#else
#define rb_file_s_lchmod rb_f_notimplement
#endif

struct chown_args {
    rb_uid_t owner;
    rb_gid_t group;
};

static void
chown_internal(const char *path, VALUE pathv, void *arg)
{
    struct chown_args *args = arg;
    if (chown(path, args->owner, args->group) < 0)
	rb_sys_fail_path(pathv);
}

/*
 *  call-seq:
 *     File.chown(owner_int, group_int, file_name,... )  ->  integer
 *
 *  Changes the owner and group of the named file(s) to the given
 *  numeric owner and group id's. Only a process with superuser
 *  privileges may change the owner of a file. The current owner of a
 *  file may change the file's group to any group to which the owner
 *  belongs. A <code>nil</code> or -1 owner or group id is ignored.
 *  Returns the number of files processed.
 *
 *     File.chown(nil, 100, "testfile")
 *
 */

static VALUE
rb_file_s_chown(int argc, VALUE *argv)
{
    VALUE o, g, rest;
    struct chown_args arg;
    long n;

    rb_secure(2);
    rb_scan_args(argc, argv, "2*", &o, &g, &rest);
    if (NIL_P(o)) {
	arg.owner = -1;
    }
    else {
	arg.owner = NUM2UIDT(o);
    }
    if (NIL_P(g)) {
	arg.group = -1;
    }
    else {
	arg.group = NUM2GIDT(g);
    }

    n = apply2files(chown_internal, rest, &arg);
    return LONG2FIX(n);
}

/*
 *  call-seq:
 *     file.chown(owner_int, group_int )   -> 0
 *
 *  Changes the owner and group of <i>file</i> to the given numeric
 *  owner and group id's. Only a process with superuser privileges may
 *  change the owner of a file. The current owner of a file may change
 *  the file's group to any group to which the owner belongs. A
 *  <code>nil</code> or -1 owner or group id is ignored. Follows
 *  symbolic links. See also <code>File#lchown</code>.
 *
 *     File.new("testfile").chown(502, 1000)
 *
 */

static VALUE
rb_file_chown(VALUE obj, VALUE owner, VALUE group)
{
    rb_io_t *fptr;
    int o, g;
#ifndef HAVE_FCHOWN
    VALUE path;
#endif

    rb_secure(2);
    o = NIL_P(owner) ? -1 : NUM2INT(owner);
    g = NIL_P(group) ? -1 : NUM2INT(group);
    GetOpenFile(obj, fptr);
#ifndef HAVE_FCHOWN
    if (NIL_P(fptr->pathv)) return Qnil;
    path = rb_str_encode_ospath(fptr->pathv);
    if (chown(RSTRING_PTR(path), o, g) == -1)
	rb_sys_fail_path(fptr->pathv);
#else
    if (fchown(fptr->fd, o, g) == -1)
	rb_sys_fail_path(fptr->pathv);
#endif

    return INT2FIX(0);
}

#if defined(HAVE_LCHOWN)
static void
lchown_internal(const char *path, VALUE pathv, void *arg)
{
    struct chown_args *args = arg;
    if (lchown(path, args->owner, args->group) < 0)
	rb_sys_fail_path(pathv);
}

/*
 *  call-seq:
 *     file.lchown(owner_int, group_int, file_name,..) -> integer
 *
 *  Equivalent to <code>File::chown</code>, but does not follow symbolic
 *  links (so it will change the owner associated with the link, not the
 *  file referenced by the link). Often not available. Returns number
 *  of files in the argument list.
 *
 */

static VALUE
rb_file_s_lchown(int argc, VALUE *argv)
{
    VALUE o, g, rest;
    struct chown_args arg;
    long n;

    rb_secure(2);
    rb_scan_args(argc, argv, "2*", &o, &g, &rest);
    if (NIL_P(o)) {
	arg.owner = -1;
    }
    else {
	arg.owner = NUM2UIDT(o);
    }
    if (NIL_P(g)) {
	arg.group = -1;
    }
    else {
	arg.group = NUM2GIDT(g);
    }

    n = apply2files(lchown_internal, rest, &arg);
    return LONG2FIX(n);
}
#else
#define rb_file_s_lchown rb_f_notimplement
#endif

struct utime_args {
    const struct timespec* tsp;
    VALUE atime, mtime;
};

#if defined DOSISH || defined __CYGWIN__
NORETURN(static void utime_failed(VALUE, const struct timespec *, VALUE, VALUE));

static void
utime_failed(VALUE path, const struct timespec *tsp, VALUE atime, VALUE mtime)
{
    if (tsp && errno == EINVAL) {
	VALUE e[2], a = Qnil, m = Qnil;
	int d = 0;
	if (!NIL_P(atime)) {
	    a = rb_inspect(atime);
	}
	if (!NIL_P(mtime) && mtime != atime && !rb_equal(atime, mtime)) {
	    m = rb_inspect(mtime);
	}
	if (NIL_P(a)) e[0] = m;
	else if (NIL_P(m) || rb_str_cmp(a, m) == 0) e[0] = a;
	else {
	    e[0] = rb_str_plus(a, rb_str_new_cstr(" or "));
	    rb_str_append(e[0], m);
	    d = 1;
	}
	if (!NIL_P(e[0])) {
	    if (path) {
		if (!d) e[0] = rb_str_dup(e[0]);
		rb_str_append(rb_str_cat2(e[0], " for "), path);
	    }
	    e[1] = INT2FIX(EINVAL);
	    rb_exc_raise(rb_class_new_instance(2, e, rb_eSystemCallError));
	}
	errno = EINVAL;
    }
    rb_sys_fail_path(path);
}
#else
#define utime_failed(path, tsp, atime, mtime) rb_sys_fail_path(path)
#endif

#if defined(HAVE_UTIMES)

static void
utime_internal(const char *path, VALUE pathv, void *arg)
{
    struct utime_args *v = arg;
    const struct timespec *tsp = v->tsp;
    struct timeval tvbuf[2], *tvp = NULL;

#ifdef HAVE_UTIMENSAT
    static int try_utimensat = 1;

    if (try_utimensat) {
        if (utimensat(AT_FDCWD, path, tsp, 0) < 0) {
            if (errno == ENOSYS) {
                try_utimensat = 0;
                goto no_utimensat;
            }
            utime_failed(pathv, tsp, v->atime, v->mtime);
        }
        return;
    }
no_utimensat:
#endif

    if (tsp) {
        tvbuf[0].tv_sec = tsp[0].tv_sec;
        tvbuf[0].tv_usec = (int)(tsp[0].tv_nsec / 1000);
        tvbuf[1].tv_sec = tsp[1].tv_sec;
        tvbuf[1].tv_usec = (int)(tsp[1].tv_nsec / 1000);
        tvp = tvbuf;
    }
    if (utimes(path, tvp) < 0)
	utime_failed(pathv, tsp, v->atime, v->mtime);
}

#else

#if !defined HAVE_UTIME_H && !defined HAVE_SYS_UTIME_H
struct utimbuf {
    long actime;
    long modtime;
};
#endif

static void
utime_internal(const char *path, VALUE pathv, void *arg)
{
    struct utime_args *v = arg;
    const struct timespec *tsp = v->tsp;
    struct utimbuf utbuf, *utp = NULL;
    if (tsp) {
        utbuf.actime = tsp[0].tv_sec;
        utbuf.modtime = tsp[1].tv_sec;
        utp = &utbuf;
    }
    if (utime(path, utp) < 0)
	utime_failed(pathv, tsp, v->atime, v->mtime);
}

#endif

/*
 * call-seq:
 *  File.utime(atime, mtime, file_name,...)   ->  integer
 *
 * Sets the access and modification times of each
 * named file to the first two arguments. Returns
 * the number of file names in the argument list.
 */

static VALUE
rb_file_s_utime(int argc, VALUE *argv)
{
    VALUE rest;
    struct utime_args args;
    struct timespec tss[2], *tsp = NULL;
    long n;

    rb_secure(2);
    rb_scan_args(argc, argv, "2*", &args.atime, &args.mtime, &rest);

    if (!NIL_P(args.atime) || !NIL_P(args.mtime)) {
	tsp = tss;
	tsp[0] = rb_time_timespec(args.atime);
	tsp[1] = rb_time_timespec(args.mtime);
    }
    args.tsp = tsp;

    n = apply2files(utime_internal, rest, &args);
    return LONG2FIX(n);
}

NORETURN(static void sys_fail2(VALUE,VALUE));
static void
sys_fail2(VALUE s1, VALUE s2)
{
    VALUE str;
#ifdef MAX_PATH
    const int max_pathlen = MAX_PATH;
#else
    const int max_pathlen = MAXPATHLEN;
#endif

    str = rb_str_new_cstr("(");
    rb_str_append(str, rb_str_ellipsize(s1, max_pathlen));
    rb_str_cat2(str, ", ");
    rb_str_append(str, rb_str_ellipsize(s2, max_pathlen));
    rb_str_cat2(str, ")");
    rb_sys_fail_path(str);
}

#ifdef HAVE_LINK
/*
 *  call-seq:
 *     File.link(old_name, new_name)    -> 0
 *
 *  Creates a new name for an existing file using a hard link. Will not
 *  overwrite <i>new_name</i> if it already exists (raising a subclass
 *  of <code>SystemCallError</code>). Not available on all platforms.
 *
 *     File.link("testfile", ".testfile")   #=> 0
 *     IO.readlines(".testfile")[0]         #=> "This is line one\n"
 */

static VALUE
rb_file_s_link(VALUE klass, VALUE from, VALUE to)
{
    rb_secure(2);
    FilePathValue(from);
    FilePathValue(to);
    from = rb_str_encode_ospath(from);
    to = rb_str_encode_ospath(to);

    if (link(StringValueCStr(from), StringValueCStr(to)) < 0) {
	sys_fail2(from, to);
    }
    return INT2FIX(0);
}
#else
#define rb_file_s_link rb_f_notimplement
#endif

#ifdef HAVE_SYMLINK
/*
 *  call-seq:
 *     File.symlink(old_name, new_name)   -> 0
 *
 *  Creates a symbolic link called <i>new_name</i> for the existing file
 *  <i>old_name</i>. Raises a <code>NotImplemented</code> exception on
 *  platforms that do not support symbolic links.
 *
 *     File.symlink("testfile", "link2test")   #=> 0
 *
 */

static VALUE
rb_file_s_symlink(VALUE klass, VALUE from, VALUE to)
{
    rb_secure(2);
    FilePathValue(from);
    FilePathValue(to);
    from = rb_str_encode_ospath(from);
    to = rb_str_encode_ospath(to);

    if (symlink(StringValueCStr(from), StringValueCStr(to)) < 0) {
	sys_fail2(from, to);
    }
    return INT2FIX(0);
}
#else
#define rb_file_s_symlink rb_f_notimplement
#endif

#ifdef HAVE_READLINK
static VALUE rb_readlink(VALUE path);

/*
 *  call-seq:
 *     File.readlink(link_name)  ->  file_name
 *
 *  Returns the name of the file referenced by the given link.
 *  Not available on all platforms.
 *
 *     File.symlink("testfile", "link2test")   #=> 0
 *     File.readlink("link2test")              #=> "testfile"
 */

static VALUE
rb_file_s_readlink(VALUE klass, VALUE path)
{
    return rb_readlink(path);
}

static VALUE
rb_readlink(VALUE path)
{
    char *buf;
    int size = 100;
    ssize_t rv;
    VALUE v;

    rb_secure(2);
    FilePathValue(path);
    path = rb_str_encode_ospath(path);
    buf = xmalloc(size);
    while ((rv = readlink(RSTRING_PTR(path), buf, size)) == size
#ifdef _AIX
	    || (rv < 0 && errno == ERANGE) /* quirky behavior of GPFS */
#endif
	) {
	size *= 2;
	buf = xrealloc(buf, size);
    }
    if (rv < 0) {
	xfree(buf);
	rb_sys_fail_path(path);
    }
    v = rb_filesystem_str_new(buf, rv);
    xfree(buf);

    return v;
}
#else
#define rb_file_s_readlink rb_f_notimplement
#endif

static void
unlink_internal(const char *path, VALUE pathv, void *arg)
{
    if (unlink(path) < 0)
	rb_sys_fail_path(pathv);
}

/*
 *  call-seq:
 *     File.delete(file_name, ...)  -> integer
 *     File.unlink(file_name, ...)  -> integer
 *
 *  Deletes the named files, returning the number of names
 *  passed as arguments. Raises an exception on any error.
 *  See also <code>Dir::rmdir</code>.
 */

static VALUE
rb_file_s_unlink(VALUE klass, VALUE args)
{
    long n;

    rb_secure(2);
    n = apply2files(unlink_internal, args, 0);
    return LONG2FIX(n);
}

/*
 *  call-seq:
 *     File.rename(old_name, new_name)   -> 0
 *
 *  Renames the given file to the new name. Raises a
 *  <code>SystemCallError</code> if the file cannot be renamed.
 *
 *     File.rename("afile", "afile.bak")   #=> 0
 */

static VALUE
rb_file_s_rename(VALUE klass, VALUE from, VALUE to)
{
    const char *src, *dst;
    VALUE f, t;

    rb_secure(2);
    FilePathValue(from);
    FilePathValue(to);
    f = rb_str_encode_ospath(from);
    t = rb_str_encode_ospath(to);
    src = StringValueCStr(f);
    dst = StringValueCStr(t);
#if defined __CYGWIN__
    errno = 0;
#endif
    if (rename(src, dst) < 0) {
#if defined DOSISH
	switch (errno) {
	  case EEXIST:
#if defined (__EMX__)
	  case EACCES:
#endif
	    if (chmod(dst, 0666) == 0 &&
		unlink(dst) == 0 &&
		rename(src, dst) == 0)
		return INT2FIX(0);
	}
#endif
	sys_fail2(from, to);
    }

    return INT2FIX(0);
}

/*
 *  call-seq:
 *     File.umask()          -> integer
 *     File.umask(integer)   -> integer
 *
 *  Returns the current umask value for this process. If the optional
 *  argument is given, set the umask to that value and return the
 *  previous value. Umask values are <em>subtracted</em> from the
 *  default permissions, so a umask of <code>0222</code> would make a
 *  file read-only for everyone.
 *
 *     File.umask(0006)   #=> 18
 *     File.umask         #=> 6
 */

static VALUE
rb_file_s_umask(int argc, VALUE *argv)
{
    int omask = 0;

    rb_secure(2);
    if (argc == 0) {
	omask = umask(0);
	umask(omask);
    }
    else if (argc == 1) {
	omask = umask(NUM2INT(argv[0]));
    }
    else {
	rb_check_arity(argc, 0, 1);
    }
    return INT2FIX(omask);
}

#ifdef __CYGWIN__
#undef DOSISH
#endif
#if defined __CYGWIN__ || defined DOSISH
#define DOSISH_UNC
#define DOSISH_DRIVE_LETTER
#define FILE_ALT_SEPARATOR '\\'
#endif
#ifdef FILE_ALT_SEPARATOR
#define isdirsep(x) ((x) == '/' || (x) == FILE_ALT_SEPARATOR)
static const char file_alt_separator[] = {FILE_ALT_SEPARATOR, '\0'};
#else
#define isdirsep(x) ((x) == '/')
#endif

#ifndef USE_NTFS
#if defined _WIN32 || defined __CYGWIN__
#define USE_NTFS 1
#else
#define USE_NTFS 0
#endif
#endif

#if USE_NTFS
#define istrailinggarbage(x) ((x) == '.' || (x) == ' ')
#else
#define istrailinggarbage(x) 0
#endif

#define Next(p, e, enc) ((p) + rb_enc_mbclen((p), (e), (enc)))
#define Inc(p, e, enc) ((p) = Next((p), (e), (enc)))

#if defined(DOSISH_UNC)
#define has_unc(buf) (isdirsep((buf)[0]) && isdirsep((buf)[1]))
#else
#define has_unc(buf) 0
#endif

#ifdef DOSISH_DRIVE_LETTER
static inline int
has_drive_letter(const char *buf)
{
    if (ISALPHA(buf[0]) && buf[1] == ':') {
	return 1;
    }
    else {
	return 0;
    }
}

static char*
getcwdofdrv(int drv)
{
    char drive[4];
    char *drvcwd, *oldcwd;

    drive[0] = drv;
    drive[1] = ':';
    drive[2] = '\0';

    /* the only way that I know to get the current directory
       of a particular drive is to change chdir() to that drive,
       so save the old cwd before chdir()
    */
    oldcwd = my_getcwd();
    if (chdir(drive) == 0) {
	drvcwd = my_getcwd();
	chdir(oldcwd);
	xfree(oldcwd);
    }
    else {
	/* perhaps the drive is not exist. we return only drive letter */
	drvcwd = strdup(drive);
    }
    return drvcwd;
}

static inline int
not_same_drive(VALUE path, int drive)
{
    const char *p = RSTRING_PTR(path);
    if (RSTRING_LEN(path) < 2) return 0;
    if (has_drive_letter(p)) {
	return TOLOWER(p[0]) != TOLOWER(drive);
    }
    else {
	return has_unc(p);
    }
}
#endif

static inline char *
skiproot(const char *path, const char *end, rb_encoding *enc)
{
#ifdef DOSISH_DRIVE_LETTER
    if (path + 2 <= end && has_drive_letter(path)) path += 2;
#endif
    while (path < end && isdirsep(*path)) path++;
    return (char *)path;
}

#define nextdirsep rb_enc_path_next
char *
rb_enc_path_next(const char *s, const char *e, rb_encoding *enc)
{
    while (s < e && !isdirsep(*s)) {
	Inc(s, e, enc);
    }
    return (char *)s;
}

#if defined(DOSISH_UNC) || defined(DOSISH_DRIVE_LETTER)
#define skipprefix rb_enc_path_skip_prefix
#else
#define skipprefix(path, end, enc) (path)
#endif
char *
rb_enc_path_skip_prefix(const char *path, const char *end, rb_encoding *enc)
{
#if defined(DOSISH_UNC) || defined(DOSISH_DRIVE_LETTER)
#ifdef DOSISH_UNC
    if (path + 2 <= end && isdirsep(path[0]) && isdirsep(path[1])) {
	path += 2;
	while (path < end && isdirsep(*path)) path++;
	if ((path = rb_enc_path_next(path, end, enc)) < end && path[0] && path[1] && !isdirsep(path[1]))
	    path = rb_enc_path_next(path + 1, end, enc);
	return (char *)path;
    }
#endif
#ifdef DOSISH_DRIVE_LETTER
    if (has_drive_letter(path))
	return (char *)(path + 2);
#endif
#endif
    return (char *)path;
}

static inline char *
skipprefixroot(const char *path, const char *end, rb_encoding *enc)
{
#if defined(DOSISH_UNC) || defined(DOSISH_DRIVE_LETTER)
    char *p = skipprefix(path, end, enc);
    while (isdirsep(*p)) p++;
    return p;
#else
    return skiproot(path, end, enc);
#endif
}

#define strrdirsep rb_enc_path_last_separator
char *
rb_enc_path_last_separator(const char *path, const char *end, rb_encoding *enc)
{
    char *last = NULL;
    while (path < end) {
	if (isdirsep(*path)) {
	    const char *tmp = path++;
	    while (path < end && isdirsep(*path)) path++;
	    if (path >= end) break;
	    last = (char *)tmp;
	}
	else {
	    Inc(path, end, enc);
	}
    }
    return last;
}

static char *
chompdirsep(const char *path, const char *end, rb_encoding *enc)
{
    while (path < end) {
	if (isdirsep(*path)) {
	    const char *last = path++;
	    while (path < end && isdirsep(*path)) path++;
	    if (path >= end) return (char *)last;
	}
	else {
	    Inc(path, end, enc);
	}
    }
    return (char *)path;
}

char *
rb_enc_path_end(const char *path, const char *end, rb_encoding *enc)
{
    if (path < end && isdirsep(*path)) path++;
    return chompdirsep(path, end, enc);
}

#if USE_NTFS
static char *
ntfs_tail(const char *path, const char *end, rb_encoding *enc)
{
    while (path < end && *path == '.') path++;
    while (path < end && *path != ':') {
	if (istrailinggarbage(*path)) {
	    const char *last = path++;
	    while (path < end && istrailinggarbage(*path)) path++;
	    if (path >= end || *path == ':') return (char *)last;
	}
	else if (isdirsep(*path)) {
	    const char *last = path++;
	    while (path < end && isdirsep(*path)) path++;
	    if (path >= end) return (char *)last;
	    if (*path == ':') path++;
	}
	else {
	    Inc(path, end, enc);
	}
    }
    return (char *)path;
}
#endif

#define BUFCHECK(cond) do {\
    bdiff = p - buf;\
    if (cond) {\
	do {buflen *= 2;} while (cond);\
	rb_str_resize(result, buflen);\
	buf = RSTRING_PTR(result);\
	p = buf + bdiff;\
	pend = buf + buflen;\
    }\
} while (0)

#define BUFINIT() (\
    p = buf = RSTRING_PTR(result),\
    buflen = RSTRING_LEN(result),\
    pend = p + buflen)

VALUE
rb_home_dir(const char *user, VALUE result)
{
    const char *dir;
    char *buf;
#if defined DOSISH || defined __CYGWIN__
    char *p, *bend;
#endif
    long dirlen;
    rb_encoding *enc;

    if (!user || !*user) {
	if (!(dir = getenv("HOME"))) {
	    rb_raise(rb_eArgError, "couldn't find HOME environment -- expanding `~'");
	}
	dirlen = strlen(dir);
	rb_str_resize(result, dirlen);
	memcpy(buf = RSTRING_PTR(result), dir, dirlen);
    }
    else {
#ifdef HAVE_PWD_H
	struct passwd *pwPtr = getpwnam(user);
	if (!pwPtr) {
	    endpwent();
	    rb_raise(rb_eArgError, "user %s doesn't exist", user);
	}
	dirlen = strlen(pwPtr->pw_dir);
	rb_str_resize(result, dirlen);
	memcpy(buf = RSTRING_PTR(result), pwPtr->pw_dir, dirlen + 1);
	endpwent();
#else
	return Qnil;
#endif
    }
    enc = rb_filesystem_encoding();
    rb_enc_associate(result, enc);
#if defined DOSISH || defined __CYGWIN__
    for (bend = (p = buf) + dirlen; p < bend; Inc(p, bend, enc)) {
	if (*p == '\\') {
	    *p = '/';
	}
    }
#endif
    return result;
}

static char *
append_fspath(VALUE result, VALUE fname, char *dir, rb_encoding **enc, rb_encoding *fsenc)
{
    char *buf, *cwdp = dir;
    VALUE dirname = Qnil;
    size_t dirlen = strlen(dir), buflen = rb_str_capacity(result);

    if (*enc != fsenc) {
	rb_encoding *direnc = rb_enc_check(fname, dirname = rb_enc_str_new(dir, dirlen, fsenc));
	if (direnc != fsenc) {
	    dirname = rb_str_conv_enc(dirname, fsenc, direnc);
	    RSTRING_GETMEM(dirname, cwdp, dirlen);
	}
	*enc = direnc;
    }
    do {buflen *= 2;} while (dirlen > buflen);
    rb_str_resize(result, buflen);
    buf = RSTRING_PTR(result);
    memcpy(buf, cwdp, dirlen);
    xfree(dir);
    if (!NIL_P(dirname)) rb_str_resize(dirname, 0);
    rb_enc_associate(result, *enc);
    return buf + dirlen;
}

static VALUE
file_expand_path(VALUE fname, VALUE dname, int abs_mode, VALUE result)
{
    const char *s, *b, *fend;
    char *buf, *p, *pend, *root;
    size_t buflen, bdiff;
    int tainted;
    rb_encoding *enc, *fsenc = rb_filesystem_encoding();

    s = StringValuePtr(fname);
    fend = s + RSTRING_LEN(fname);
    enc = rb_enc_get(fname);
    BUFINIT();
    tainted = OBJ_TAINTED(fname);

    if (s[0] == '~' && abs_mode == 0) {      /* execute only if NOT absolute_path() */
	long userlen = 0;
	tainted = 1;
	if (isdirsep(s[1]) || s[1] == '\0') {
	    buf = 0;
	    b = 0;
	    rb_str_set_len(result, 0);
	    if (*++s) ++s;
	}
	else {
	    s = nextdirsep(b = s, fend, enc);
	    userlen = s - b;
	    BUFCHECK(bdiff + userlen >= buflen);
	    memcpy(p, b, userlen);
	    rb_str_set_len(result, userlen);
	    buf = p + 1;
	    p += userlen;
	}
	if (NIL_P(rb_home_dir(buf, result))) {
	    rb_raise(rb_eArgError, "can't find user %s", buf);
	}
	if (!rb_is_absolute_path(RSTRING_PTR(result))) {
	    if (userlen) {
		rb_raise(rb_eArgError, "non-absolute home of %.*s", (int)userlen, b);
	    }
	    else {
		rb_raise(rb_eArgError, "non-absolute home");
	    }
	}
	BUFINIT();
	p = pend;
    }
#ifdef DOSISH_DRIVE_LETTER
    /* skip drive letter */
    else if (has_drive_letter(s)) {
	if (isdirsep(s[2])) {
	    /* specified drive letter, and full path */
	    /* skip drive letter */
	    BUFCHECK(bdiff + 2 >= buflen);
	    memcpy(p, s, 2);
	    p += 2;
	    s += 2;
	    rb_enc_copy(result, fname);
	}
	else {
	    /* specified drive, but not full path */
	    int same = 0;
	    if (!NIL_P(dname) && !not_same_drive(dname, s[0])) {
		file_expand_path(dname, Qnil, abs_mode, result);
		BUFINIT();
		if (has_drive_letter(p) && TOLOWER(p[0]) == TOLOWER(s[0])) {
		    /* ok, same drive */
		    same = 1;
		}
	    }
	    if (!same) {
		char *e = append_fspath(result, fname, getcwdofdrv(*s), &enc, fsenc);
		tainted = 1;
		BUFINIT();
		p = e;
	    }
	    else {
		rb_enc_associate(result, enc = rb_enc_check(result, fname));
		p = pend;
	    }
	    p = chompdirsep(skiproot(buf, p, enc), p, enc);
	    s += 2;
	}
    }
#endif
    else if (!rb_is_absolute_path(s)) {
	if (!NIL_P(dname)) {
	    file_expand_path(dname, Qnil, abs_mode, result);
	    rb_enc_associate(result, rb_enc_check(result, fname));
	    BUFINIT();
	    p = pend;
	}
	else {
	    char *e = append_fspath(result, fname, my_getcwd(), &enc, fsenc);
	    tainted = 1;
	    BUFINIT();
	    p = e;
	}
#if defined DOSISH || defined __CYGWIN__
	if (isdirsep(*s)) {
	    /* specified full path, but not drive letter nor UNC */
	    /* we need to get the drive letter or UNC share name */
	    p = skipprefix(buf, p, enc);
	}
	else
#endif
	    p = chompdirsep(skiproot(buf, p, enc), p, enc);
    }
    else {
	size_t len;
	b = s;
	do s++; while (isdirsep(*s));
	len = s - b;
	p = buf + len;
	BUFCHECK(bdiff >= buflen);
	memset(buf, '/', len);
	rb_str_set_len(result, len);
	rb_enc_associate(result, rb_enc_check(result, fname));
    }
    if (p > buf && p[-1] == '/')
	--p;
    else {
	rb_str_set_len(result, p-buf);
	BUFCHECK(bdiff + 1 >= buflen);
	*p = '/';
    }

    rb_str_set_len(result, p-buf+1);
    BUFCHECK(bdiff + 1 >= buflen);
    p[1] = 0;
    root = skipprefix(buf, p+1, enc);

    b = s;
    while (*s) {
	switch (*s) {
	  case '.':
	    if (b == s++) {	/* beginning of path element */
		switch (*s) {
		  case '\0':
		    b = s;
		    break;
		  case '.':
		    if (*(s+1) == '\0' || isdirsep(*(s+1))) {
			/* We must go back to the parent */
			char *n;
			*p = '\0';
			if (!(n = strrdirsep(root, p, enc))) {
			    *p = '/';
			}
			else {
			    p = n;
			}
			b = ++s;
		    }
#if USE_NTFS
		    else {
			do ++s; while (istrailinggarbage(*s));
		    }
#endif
		    break;
		  case '/':
#if defined DOSISH || defined __CYGWIN__
		  case '\\':
#endif
		    b = ++s;
		    break;
		  default:
		    /* ordinary path element, beginning don't move */
		    break;
		}
	    }
#if USE_NTFS
	    else {
		--s;
	      case ' ': {
		const char *e = s;
		while (s < fend && istrailinggarbage(*s)) s++;
		if (!*s) {
		    s = e;
		    goto endpath;
		}
	      }
	    }
#endif
	    break;
	  case '/':
#if defined DOSISH || defined __CYGWIN__
	  case '\\':
#endif
	    if (s > b) {
		long rootdiff = root - buf;
		rb_str_set_len(result, p-buf+1);
		BUFCHECK(bdiff + (s-b+1) >= buflen);
		root = buf + rootdiff;
		memcpy(++p, b, s-b);
		p += s-b;
		*p = '/';
	    }
	    b = ++s;
	    break;
	  default:
	    Inc(s, fend, enc);
	    break;
	}
    }

    if (s > b) {
#if USE_NTFS
	static const char prime[] = ":$DATA";
	enum {prime_len = sizeof(prime) -1};
      endpath:
	if (s > b + prime_len && strncasecmp(s - prime_len, prime, prime_len) == 0) {
	    /* alias of stream */
	    /* get rid of a bug of x64 VC++ */
	    if (*(s - (prime_len+1)) == ':') {
		s -= prime_len + 1; /* prime */
	    }
	    else if (memchr(b, ':', s - prime_len - b)) {
		s -= prime_len;	/* alternative */
	    }
	}
#endif
	rb_str_set_len(result, p-buf+1);
	BUFCHECK(bdiff + (s-b) >= buflen);
	memcpy(++p, b, s-b);
	p += s-b;
	rb_str_set_len(result, p-buf);
    }
    if (p == skiproot(buf, p + !!*p, enc) - 1) p++;

#if USE_NTFS
    *p = '\0';
    if ((s = strrdirsep(b = buf, p, enc)) != 0 && !strpbrk(s, "*?")) {
	VALUE tmp, v;
	size_t len;
	rb_encoding *enc;
	WCHAR *wstr;
	WIN32_FIND_DATAW wfd;
	HANDLE h;
#ifdef __CYGWIN__
#ifdef HAVE_CYGWIN_CONV_PATH
	char *w32buf = NULL;
	const int flags = CCP_POSIX_TO_WIN_A | CCP_RELATIVE;
#else
	char w32buf[MAXPATHLEN];
#endif
	const char *path;
	ssize_t bufsize;
	int lnk_added = 0, is_symlink = 0;
	struct stat st;
	p = (char *)s;
	len = strlen(p);
	if (lstat(buf, &st) == 0 && S_ISLNK(st.st_mode)) {
	    is_symlink = 1;
	    if (len > 4 && STRCASECMP(p + len - 4, ".lnk") != 0) {
		lnk_added = 1;
	    }
	}
	path = *buf ? buf : "/";
#ifdef HAVE_CYGWIN_CONV_PATH
	bufsize = cygwin_conv_path(flags, path, NULL, 0);
	if (bufsize > 0) {
	    bufsize += len;
	    if (lnk_added) bufsize += 4;
	    w32buf = ALLOCA_N(char, bufsize);
	    if (cygwin_conv_path(flags, path, w32buf, bufsize) == 0) {
		b = w32buf;
	    }
	}
#else
	bufsize = MAXPATHLEN;
	if (cygwin_conv_to_win32_path(path, w32buf) == 0) {
	    b = w32buf;
	}
#endif
	if (is_symlink && b == w32buf) {
	    *p = '\\';
	    strlcat(w32buf, p, bufsize);
	    if (lnk_added) {
		strlcat(w32buf, ".lnk", bufsize);
	    }
	}
	else {
	    lnk_added = 0;
	}
	*p = '/';
#endif
	rb_str_set_len(result, p - buf + strlen(p));
	enc = rb_enc_get(result);
	tmp = result;
	if (enc != rb_utf8_encoding() && rb_enc_str_coderange(result) != ENC_CODERANGE_7BIT) {
	    tmp = rb_str_encode_ospath(result);
	}
	len = MultiByteToWideChar(CP_UTF8, 0, RSTRING_PTR(tmp), -1, NULL, 0);
	wstr = ALLOCV_N(WCHAR, v, len);
	MultiByteToWideChar(CP_UTF8, 0, RSTRING_PTR(tmp), -1, wstr, len);
	if (tmp != result) rb_str_resize(tmp, 0);
	h = FindFirstFileW(wstr, &wfd);
	ALLOCV_END(v);
	if (h != INVALID_HANDLE_VALUE) {
	    size_t wlen;
	    FindClose(h);
	    len = lstrlenW(wfd.cFileName);
#ifdef __CYGWIN__
	    if (lnk_added && len > 4 &&
		wcscasecmp(wfd.cFileName + len - 4, L".lnk") == 0) {
		wfd.cFileName[len -= 4] = L'\0';
	    }
#else
	    p = (char *)s;
#endif
	    ++p;
	    wlen = (int)len;
	    len = WideCharToMultiByte(CP_UTF8, 0, wfd.cFileName, wlen, NULL, 0, NULL, NULL);
	    BUFCHECK(bdiff + len >= buflen);
	    WideCharToMultiByte(CP_UTF8, 0, wfd.cFileName, wlen, p, len + 1, NULL, NULL);
	    if (tmp != result) {
		rb_str_buf_cat(tmp, p, len);
		tmp = rb_str_encode(tmp, rb_enc_from_encoding(enc), 0, Qnil);
		len = RSTRING_LEN(tmp);
		BUFCHECK(bdiff + len >= buflen);
		memcpy(p, RSTRING_PTR(tmp), len);
		rb_str_resize(tmp, 0);
	    }
	    p += len;
	}
#ifdef __CYGWIN__
	else {
	    p += strlen(p);
	}
#endif
    }
#endif

    if (tainted) OBJ_TAINT(result);
    rb_str_set_len(result, p - buf);
    rb_enc_check(fname, result);
    ENC_CODERANGE_CLEAR(result);
    return result;
}

#define EXPAND_PATH_BUFFER() rb_usascii_str_new(0, MAXPATHLEN + 2)

#define check_expand_path_args(fname, dname) \
    (((fname) = rb_get_path(fname)), \
     (void)(NIL_P(dname) ? (dname) : ((dname) = rb_get_path(dname))))

static VALUE
file_expand_path_1(VALUE fname)
{
    return file_expand_path(fname, Qnil, 0, EXPAND_PATH_BUFFER());
}

VALUE
rb_file_expand_path(VALUE fname, VALUE dname)
{
    check_expand_path_args(fname, dname);
    return file_expand_path(fname, dname, 0, EXPAND_PATH_BUFFER());
}

/*
 *  call-seq:
 *     File.expand_path(file_name [, dir_string] )  ->  abs_file_name
 *
 *  Converts a pathname to an absolute pathname. Relative paths are
 *  referenced from the current working directory of the process unless
 *  <i>dir_string</i> is given, in which case it will be used as the
 *  starting point. The given pathname may start with a
 *  ``<code>~</code>'', which expands to the process owner's home
 *  directory (the environment variable <code>HOME</code> must be set
 *  correctly). ``<code>~</code><i>user</i>'' expands to the named
 *  user's home directory.
 *
 *     File.expand_path("~oracle/bin")           #=> "/home/oracle/bin"
 *     File.expand_path("../../bin", "/tmp/x")   #=> "/bin"
 */

VALUE
rb_file_s_expand_path(int argc, VALUE *argv)
{
    VALUE fname, dname;

    if (argc == 1) {
	return rb_file_expand_path(argv[0], Qnil);
    }
    rb_scan_args(argc, argv, "11", &fname, &dname);

    return rb_file_expand_path(fname, dname);
}

VALUE
rb_file_absolute_path(VALUE fname, VALUE dname)
{
    check_expand_path_args(fname, dname);
    return file_expand_path(fname, dname, 1, EXPAND_PATH_BUFFER());
}

/*
 *  call-seq:
 *     File.absolute_path(file_name [, dir_string] )  ->  abs_file_name
 *
 *  Converts a pathname to an absolute pathname. Relative paths are
 *  referenced from the current working directory of the process unless
 *  <i>dir_string</i> is given, in which case it will be used as the
 *  starting point. If the given pathname starts with a ``<code>~</code>''
 *  it is NOT expanded, it is treated as a normal directory name.
 *
 *     File.absolute_path("~oracle/bin")       #=> "<relative_path>/~oracle/bin"
 */

VALUE
rb_file_s_absolute_path(int argc, VALUE *argv)
{
    VALUE fname, dname;

    if (argc == 1) {
	return rb_file_absolute_path(argv[0], Qnil);
    }
    rb_scan_args(argc, argv, "11", &fname, &dname);

    return rb_file_absolute_path(fname, dname);
}

static void
realpath_rec(long *prefixlenp, VALUE *resolvedp, const char *unresolved, VALUE loopcheck, int strict, int last)
{
    const char *pend = unresolved + strlen(unresolved);
    rb_encoding *enc = rb_enc_get(*resolvedp);
    ID resolving;
    CONST_ID(resolving, "resolving");
    while (unresolved < pend) {
	const char *testname = unresolved;
	const char *unresolved_firstsep = rb_enc_path_next(unresolved, pend, enc);
	long testnamelen = unresolved_firstsep - unresolved;
	const char *unresolved_nextname = unresolved_firstsep;
        while (unresolved_nextname < pend && isdirsep(*unresolved_nextname))
	    unresolved_nextname++;
        unresolved = unresolved_nextname;
        if (testnamelen == 1 && testname[0] == '.') {
        }
        else if (testnamelen == 2 && testname[0] == '.' && testname[1] == '.') {
            if (*prefixlenp < RSTRING_LEN(*resolvedp)) {
		const char *resolved_str = RSTRING_PTR(*resolvedp);
		const char *resolved_names = resolved_str + *prefixlenp;
		const char *lastsep = strrdirsep(resolved_names, resolved_str + RSTRING_LEN(*resolvedp), enc);
                long len = lastsep ? lastsep - resolved_names : 0;
                rb_str_resize(*resolvedp, *prefixlenp + len);
            }
        }
        else {
            VALUE checkval;
            VALUE testpath = rb_str_dup(*resolvedp);
            if (*prefixlenp < RSTRING_LEN(testpath))
                rb_str_cat2(testpath, "/");
            rb_str_cat(testpath, testname, testnamelen);
            checkval = rb_hash_aref(loopcheck, testpath);
            if (!NIL_P(checkval)) {
                if (checkval == ID2SYM(resolving)) {
                    errno = ELOOP;
                    rb_sys_fail_path(testpath);
                }
                else {
                    *resolvedp = rb_str_dup(checkval);
                }
            }
            else {
                struct stat sbuf;
                int ret;
                VALUE testpath2 = rb_str_encode_ospath(testpath);
                ret = lstat(RSTRING_PTR(testpath2), &sbuf);
                if (ret == -1) {
                    if (errno == ENOENT) {
                        if (strict || !last || *unresolved_firstsep)
                            rb_sys_fail_path(testpath);
                        *resolvedp = testpath;
                        break;
                    }
                    else {
                        rb_sys_fail_path(testpath);
                    }
                }
#ifdef HAVE_READLINK
                if (S_ISLNK(sbuf.st_mode)) {
		    VALUE link;
		    const char *link_prefix, *link_names;
                    long link_prefixlen;
                    rb_hash_aset(loopcheck, testpath, ID2SYM(resolving));
		    link = rb_readlink(testpath);
                    link_prefix = RSTRING_PTR(link);
		    link_names = skipprefixroot(link_prefix, link_prefix + RSTRING_LEN(link), rb_enc_get(link));
		    link_prefixlen = link_names - link_prefix;
		    if (link_prefixlen > 0) {
			rb_encoding *enc, *linkenc = rb_enc_get(link);
			link = rb_str_subseq(link, 0, link_prefixlen);
			enc = rb_enc_check(*resolvedp, link);
			if (enc != linkenc) link = rb_str_conv_enc(link, linkenc, enc);
			*resolvedp = link;
			*prefixlenp = link_prefixlen;
		    }
		    realpath_rec(prefixlenp, resolvedp, link_names, loopcheck, strict, *unresolved_firstsep == '\0');
		    rb_hash_aset(loopcheck, testpath, rb_str_dup_frozen(*resolvedp));
                }
                else
#endif
                {
                    VALUE s = rb_str_dup_frozen(testpath);
                    rb_hash_aset(loopcheck, s, s);
                    *resolvedp = testpath;
                }
            }
        }
    }
}

VALUE
rb_realpath_internal(VALUE basedir, VALUE path, int strict)
{
    long prefixlen;
    VALUE resolved;
    volatile VALUE unresolved_path;
    VALUE loopcheck;
    volatile VALUE curdir = Qnil;

    rb_encoding *enc;
    char *path_names = NULL, *basedir_names = NULL, *curdir_names = NULL;
    char *ptr, *prefixptr = NULL, *pend;
    long len;

    rb_secure(2);

    FilePathValue(path);
    unresolved_path = rb_str_dup_frozen(path);

    if (!NIL_P(basedir)) {
        FilePathValue(basedir);
        basedir = rb_str_dup_frozen(basedir);
    }

    RSTRING_GETMEM(unresolved_path, ptr, len);
    path_names = skipprefixroot(ptr, ptr + len, rb_enc_get(unresolved_path));
    if (ptr != path_names) {
        resolved = rb_str_subseq(unresolved_path, 0, path_names - ptr);
        goto root_found;
    }

    if (!NIL_P(basedir)) {
	RSTRING_GETMEM(basedir, ptr, len);
	basedir_names = skipprefixroot(ptr, ptr + len, rb_enc_get(basedir));
        if (ptr != basedir_names) {
	    resolved = rb_str_subseq(basedir, 0, basedir_names - ptr);
	    goto root_found;
        }
    }

    curdir = rb_dir_getwd();
    RSTRING_GETMEM(curdir, ptr, len);
    curdir_names = skipprefixroot(ptr, ptr + len, rb_enc_get(curdir));
    resolved = rb_str_subseq(curdir, 0, curdir_names - ptr);

  root_found:
    RSTRING_GETMEM(resolved, prefixptr, prefixlen);
    pend = prefixptr + prefixlen;
    enc = rb_enc_get(resolved);
    ptr = chompdirsep(prefixptr, pend, enc);
    if (ptr < pend) {
        prefixlen = ++ptr - prefixptr;
        rb_str_set_len(resolved, prefixlen);
    }
#ifdef FILE_ALT_SEPARATOR
    while (prefixptr < ptr) {
	if (*prefixptr == FILE_ALT_SEPARATOR) {
	    *prefixptr = '/';
	}
	Inc(prefixptr, pend, enc);
    }
#endif

    loopcheck = rb_hash_new();
    if (curdir_names)
        realpath_rec(&prefixlen, &resolved, curdir_names, loopcheck, 1, 0);
    if (basedir_names)
        realpath_rec(&prefixlen, &resolved, basedir_names, loopcheck, 1, 0);
    realpath_rec(&prefixlen, &resolved, path_names, loopcheck, strict, 1);

    OBJ_TAINT(resolved);
    return resolved;
}

/*
 * call-seq:
 *     File.realpath(pathname [, dir_string])  ->  real_pathname
 *
 *  Returns the real (absolute) pathname of _pathname_ in the actual
 *  filesystem not containing symlinks or useless dots.
 *
 *  If _dir_string_ is given, it is used as a base directory
 *  for interpreting relative pathname instead of the current directory.
 *
 *  All components of the pathname must exist when this method is
 *  called.
 */
static VALUE
rb_file_s_realpath(int argc, VALUE *argv, VALUE klass)
{
    VALUE path, basedir;
    rb_scan_args(argc, argv, "11", &path, &basedir);
    return rb_realpath_internal(basedir, path, 1);
}

/*
 * call-seq:
 *     File.realdirpath(pathname [, dir_string])  ->  real_pathname
 *
 *  Returns the real (absolute) pathname of _pathname_ in the actual filesystem.
 *  The real pathname doesn't contain symlinks or useless dots.
 *
 *  If _dir_string_ is given, it is used as a base directory
 *  for interpreting relative pathname instead of the current directory.
 *
 *  The last component of the real pathname can be nonexistent.
 */
static VALUE
rb_file_s_realdirpath(int argc, VALUE *argv, VALUE klass)
{
    VALUE path, basedir;
    rb_scan_args(argc, argv, "11", &path, &basedir);
    return rb_realpath_internal(basedir, path, 0);
}

static size_t
rmext(const char *p, long l0, long l1, const char *e, long l2, rb_encoding *enc)
{
    int len1, len2;
    unsigned int c;
    const char *s, *last;

    if (!e || !l2) return 0;

    c = rb_enc_codepoint_len(e, e + l2, &len1, enc);
    if (rb_enc_ascget(e + len1, e + l2, &len2, enc) == '*' && len1 + len2 == l2) {
	if (c == '.') return l0;
	s = p;
	e = p + l1;
	last = e;
	while (s < e) {
	    if (rb_enc_codepoint_len(s, e, &len1, enc) == c) last = s;
	    s += len1;
	}
	return last - p;
    }
    if (l1 < l2) return l1;

    s = p+l1-l2;
    if (rb_enc_left_char_head(p, s, p+l1, enc) != s) return 0;
#if CASEFOLD_FILESYSTEM
#define fncomp strncasecmp
#else
#define fncomp strncmp
#endif
    if (fncomp(s, e, l2) == 0) {
	return l1-l2;
    }
    return 0;
}

const char *
ruby_enc_find_basename(const char *name, long *baselen, long *alllen, rb_encoding *enc)
{
    const char *p, *q, *e, *end;
#if defined DOSISH_DRIVE_LETTER || defined DOSISH_UNC
    const char *root;
#endif
    long f = 0, n = -1;

    end = name + (alllen ? (size_t)*alllen : strlen(name));
    name = skipprefix(name, end, enc);
#if defined DOSISH_DRIVE_LETTER || defined DOSISH_UNC
    root = name;
#endif
    while (isdirsep(*name))
	name++;
    if (!*name) {
	p = name - 1;
	f = 1;
#if defined DOSISH_DRIVE_LETTER || defined DOSISH_UNC
	if (name != root) {
	    /* has slashes */
	}
#ifdef DOSISH_DRIVE_LETTER
	else if (*p == ':') {
	    p++;
	    f = 0;
	}
#endif
#ifdef DOSISH_UNC
	else {
	    p = "/";
	}
#endif
#endif
    }
    else {
	if (!(p = strrdirsep(name, end, enc))) {
	    p = name;
	}
	else {
	    while (isdirsep(*p)) p++; /* skip last / */
	}
#if USE_NTFS
	n = ntfs_tail(p, end, enc) - p;
#else
	n = chompdirsep(p, end, enc) - p;
#endif
	for (q = p; q - p < n && *q == '.'; q++);
	for (e = 0; q - p < n; Inc(q, end, enc)) {
	    if (*q == '.') e = q;
	}
	if (e) f = e - p;
	else f = n;
    }

    if (baselen)
	*baselen = f;
    if (alllen)
	*alllen = n;
    return p;
}

/*
 *  call-seq:
 *     File.basename(file_name [, suffix] )  ->  base_name
 *
 *  Returns the last component of the filename given in <i>file_name</i>,
 *  which must be formed using forward slashes (``<code>/</code>'')
 *  regardless of the separator used on the local file system. If
 *  <i>suffix</i> is given and present at the end of <i>file_name</i>,
 *  it is removed.
 *
 *     File.basename("/home/gumby/work/ruby.rb")          #=> "ruby.rb"
 *     File.basename("/home/gumby/work/ruby.rb", ".rb")   #=> "ruby"
 */

static VALUE
rb_file_s_basename(int argc, VALUE *argv)
{
    VALUE fname, fext, basename;
    const char *name, *p;
    long f, n;
    rb_encoding *enc;

    if (rb_scan_args(argc, argv, "11", &fname, &fext) == 2) {
	rb_encoding *enc;
	StringValue(fext);
	if (!rb_enc_asciicompat(enc = rb_enc_get(fext))) {
	    rb_raise(rb_eEncCompatError, "ascii incompatible character encodings: %s",
		     rb_enc_name(enc));
	}
    }
    FilePathStringValue(fname);
    if (NIL_P(fext) || !(enc = rb_enc_compatible(fname, fext))) {
	enc = rb_enc_get(fname);
	fext = Qnil;
    }
    if ((n = RSTRING_LEN(fname)) == 0 || !*(name = RSTRING_PTR(fname)))
	return rb_str_new_shared(fname);

    p = ruby_enc_find_basename(name, &f, &n, enc);
    if (n >= 0) {
	if (NIL_P(fext)) {
	    f = n;
	}
	else {
	    const char *fp;
	    fp = StringValueCStr(fext);
	    if (!(f = rmext(p, f, n, fp, RSTRING_LEN(fext), enc))) {
		f = n;
	    }
	    RB_GC_GUARD(fext);
	}
	if (f == RSTRING_LEN(fname)) return rb_str_new_shared(fname);
    }

    basename = rb_str_new(p, f);
    rb_enc_copy(basename, fname);
    OBJ_INFECT(basename, fname);
    return basename;
}

/*
 *  call-seq:
 *     File.dirname(file_name )  ->  dir_name
 *
 *  Returns all components of the filename given in <i>file_name</i>
 *  except the last one. The filename must be formed using forward
 *  slashes (``<code>/</code>'') regardless of the separator used on the
 *  local file system.
 *
 *     File.dirname("/home/gumby/work/ruby.rb")   #=> "/home/gumby/work"
 */

static VALUE
rb_file_s_dirname(VALUE klass, VALUE fname)
{
    return rb_file_dirname(fname);
}

VALUE
rb_file_dirname(VALUE fname)
{
    const char *name, *root, *p, *end;
    VALUE dirname;
    rb_encoding *enc;

    FilePathStringValue(fname);
    name = StringValueCStr(fname);
    end = name + RSTRING_LEN(fname);
    enc = rb_enc_get(fname);
    root = skiproot(name, end, enc);
#ifdef DOSISH_UNC
    if (root > name + 1 && isdirsep(*name))
	root = skipprefix(name = root - 2, end, enc);
#else
    if (root > name + 1)
	name = root - 1;
#endif
    p = strrdirsep(root, end, enc);
    if (!p) {
	p = root;
    }
    if (p == name)
	return rb_usascii_str_new2(".");
#ifdef DOSISH_DRIVE_LETTER
    if (has_drive_letter(name) && isdirsep(*(name + 2))) {
	const char *top = skiproot(name + 2, end, enc);
	dirname = rb_str_new(name, 3);
	rb_str_cat(dirname, top, p - top);
    }
    else
#endif
    dirname = rb_str_new(name, p - name);
#ifdef DOSISH_DRIVE_LETTER
    if (has_drive_letter(name) && root == name + 2 && p - name == 2)
	rb_str_cat(dirname, ".", 1);
#endif
    rb_enc_copy(dirname, fname);
    OBJ_INFECT(dirname, fname);
    return dirname;
}

/*
 * accept a String, and return the pointer of the extension.
 * if len is passed, set the length of extension to it.
 * returned pointer is in ``name'' or NULL.
 *                 returns   *len
 *   no dot        NULL      0
 *   dotfile       top       0
 *   end with dot  dot       1
 *   .ext          dot       len of .ext
 *   .ext:stream   dot       len of .ext without :stream (NT only)
 *
 */
const char *
ruby_enc_find_extname(const char *name, long *len, rb_encoding *enc)
{
    const char *p, *e, *end = name + (len ? *len : (long)strlen(name));

    p = strrdirsep(name, end, enc);	/* get the last path component */
    if (!p)
	p = name;
    else
	do name = ++p; while (isdirsep(*p));

    e = 0;
    while (*p && *p == '.') p++;
    while (*p) {
	if (*p == '.' || istrailinggarbage(*p)) {
#if USE_NTFS
	    const char *last = p++, *dot = last;
	    while (istrailinggarbage(*p)) {
		if (*p == '.') dot = p;
		p++;
	    }
	    if (!*p || *p == ':') {
		p = last;
		break;
	    }
	    if (*last == '.' || dot > last) e = dot;
	    continue;
#else
	    e = p;	  /* get the last dot of the last component */
#endif
	}
#if USE_NTFS
	else if (*p == ':') {
	    break;
	}
#endif
	else if (isdirsep(*p))
	    break;
	Inc(p, end, enc);
    }

    if (len) {
	/* no dot, or the only dot is first or end? */
	if (!e || e == name)
	    *len = 0;
	else if (e+1 == p)
	    *len = 1;
	else
	    *len = p - e;
    }
    return e;
}

/*
 *  call-seq:
 *     File.extname(path)  ->  string
 *
 *  Returns the extension (the portion of file name in <i>path</i>
 *  after the period).
 *
 *     File.extname("test.rb")         #=> ".rb"
 *     File.extname("a/b/d/test.rb")   #=> ".rb"
 *     File.extname("test")            #=> ""
 *     File.extname(".profile")        #=> ""
 *
 */

static VALUE
rb_file_s_extname(VALUE klass, VALUE fname)
{
    const char *name, *e;
    long len;
    VALUE extname;

    FilePathStringValue(fname);
    name = StringValueCStr(fname);
    len = RSTRING_LEN(fname);
    e = ruby_enc_find_extname(name, &len, rb_enc_get(fname));
    if (len <= 1)
	return rb_str_new(0, 0);
    extname = rb_str_subseq(fname, e - name, len); /* keep the dot, too! */
    OBJ_INFECT(extname, fname);
    return extname;
}

/*
 *  call-seq:
 *     File.path(path)  ->  string
 *
 *  Returns the string representation of the path
 *
 *     File.path("/dev/null")          #=> "/dev/null"
 *     File.path(Pathname.new("/tmp")) #=> "/tmp"
 *
 */

static VALUE
rb_file_s_path(VALUE klass, VALUE fname)
{
    return rb_get_path(fname);
}

/*
 *  call-seq:
 *     File.split(file_name)   -> array
 *
 *  Splits the given string into a directory and a file component and
 *  returns them in a two-element array. See also
 *  <code>File::dirname</code> and <code>File::basename</code>.
 *
 *     File.split("/home/gumby/.profile")   #=> ["/home/gumby", ".profile"]
 */

static VALUE
rb_file_s_split(VALUE klass, VALUE path)
{
    FilePathStringValue(path);		/* get rid of converting twice */
    return rb_assoc_new(rb_file_s_dirname(Qnil, path), rb_file_s_basename(1,&path));
}

static VALUE separator;

static VALUE rb_file_join(VALUE ary, VALUE sep);

static VALUE
file_inspect_join(VALUE ary, VALUE argp, int recur)
{
    VALUE *arg = (VALUE *)argp;
    if (recur || ary == arg[0]) rb_raise(rb_eArgError, "recursive array");
    return rb_file_join(arg[0], arg[1]);
}

static VALUE
rb_file_join(VALUE ary, VALUE sep)
{
    long len, i;
    VALUE result, tmp;
    const char *name, *tail;

    if (RARRAY_LEN(ary) == 0) return rb_str_new(0, 0);

    len = 1;
    for (i=0; i<RARRAY_LEN(ary); i++) {
	tmp = RARRAY_PTR(ary)[i];
	if (RB_TYPE_P(tmp, T_STRING)) {
	    len += RSTRING_LEN(tmp);
	}
	else {
	    len += 10;
	}
    }
    if (!NIL_P(sep)) {
	StringValue(sep);
	len += RSTRING_LEN(sep) * (RARRAY_LEN(ary) - 1);
    }
    result = rb_str_buf_new(len);
    OBJ_INFECT(result, ary);
    for (i=0; i<RARRAY_LEN(ary); i++) {
	tmp = RARRAY_PTR(ary)[i];
	switch (TYPE(tmp)) {
	  case T_STRING:
	    break;
	  case T_ARRAY:
	    if (ary == tmp) {
		rb_raise(rb_eArgError, "recursive array");
	    }
	    else {
		VALUE args[2];

		args[0] = tmp;
		args[1] = sep;
		tmp = rb_exec_recursive(file_inspect_join, ary, (VALUE)args);
	    }
	    break;
	  default:
	    FilePathStringValue(tmp);
	}
	name = StringValueCStr(result);
	len = RSTRING_LEN(result);
	if (i == 0) {
	    rb_enc_copy(result, tmp);
	}
	else if (!NIL_P(sep)) {
	    tail = chompdirsep(name, name + len, rb_enc_get(result));
	    if (RSTRING_PTR(tmp) && isdirsep(RSTRING_PTR(tmp)[0])) {
		rb_str_set_len(result, tail - name);
	    }
	    else if (!*tail) {
		rb_str_buf_append(result, sep);
	    }
	}
	rb_str_buf_append(result, tmp);
    }

    return result;
}

/*
 *  call-seq:
 *     File.join(string, ...)  ->  path
 *
 *  Returns a new string formed by joining the strings using
 *  <code>File::SEPARATOR</code>.
 *
 *     File.join("usr", "mail", "gumby")   #=> "usr/mail/gumby"
 *
 */

static VALUE
rb_file_s_join(VALUE klass, VALUE args)
{
    return rb_file_join(args, separator);
}

#if defined(HAVE_TRUNCATE) || defined(HAVE_CHSIZE)
/*
 *  call-seq:
 *     File.truncate(file_name, integer)  -> 0
 *
 *  Truncates the file <i>file_name</i> to be at most <i>integer</i>
 *  bytes long. Not available on all platforms.
 *
 *     f = File.new("out", "w")
 *     f.write("1234567890")     #=> 10
 *     f.close                   #=> nil
 *     File.truncate("out", 5)   #=> 0
 *     File.size("out")          #=> 5
 *
 */

static VALUE
rb_file_s_truncate(VALUE klass, VALUE path, VALUE len)
{
    off_t pos;

    rb_secure(2);
    pos = NUM2OFFT(len);
    FilePathValue(path);
    path = rb_str_encode_ospath(path);
#ifdef HAVE_TRUNCATE
    if (truncate(StringValueCStr(path), pos) < 0)
	rb_sys_fail_path(path);
#else /* defined(HAVE_CHSIZE) */
    {
	int tmpfd;

	if ((tmpfd = rb_cloexec_open(StringValueCStr(path), 0, 0)) < 0) {
	    rb_sys_fail_path(path);
	}
        rb_update_max_fd(tmpfd);
	if (chsize(tmpfd, pos) < 0) {
	    close(tmpfd);
	    rb_sys_fail_path(path);
	}
	close(tmpfd);
    }
#endif
    return INT2FIX(0);
}
#else
#define rb_file_s_truncate rb_f_notimplement
#endif

#if defined(HAVE_FTRUNCATE) || defined(HAVE_CHSIZE)
/*
 *  call-seq:
 *     file.truncate(integer)    -> 0
 *
 *  Truncates <i>file</i> to at most <i>integer</i> bytes. The file
 *  must be opened for writing. Not available on all platforms.
 *
 *     f = File.new("out", "w")
 *     f.syswrite("1234567890")   #=> 10
 *     f.truncate(5)              #=> 0
 *     f.close()                  #=> nil
 *     File.size("out")           #=> 5
 */

static VALUE
rb_file_truncate(VALUE obj, VALUE len)
{
    rb_io_t *fptr;
    off_t pos;

    rb_secure(2);
    pos = NUM2OFFT(len);
    GetOpenFile(obj, fptr);
    if (!(fptr->mode & FMODE_WRITABLE)) {
	rb_raise(rb_eIOError, "not opened for writing");
    }
    rb_io_flush(obj);
#ifdef HAVE_FTRUNCATE
    if (ftruncate(fptr->fd, pos) < 0)
	rb_sys_fail_path(fptr->pathv);
#else /* defined(HAVE_CHSIZE) */
    if (chsize(fptr->fd, pos) < 0)
	rb_sys_fail_path(fptr->pathv);
#endif
    return INT2FIX(0);
}
#else
#define rb_file_truncate rb_f_notimplement
#endif

# ifndef LOCK_SH
#  define LOCK_SH 1
# endif
# ifndef LOCK_EX
#  define LOCK_EX 2
# endif
# ifndef LOCK_NB
#  define LOCK_NB 4
# endif
# ifndef LOCK_UN
#  define LOCK_UN 8
# endif

#ifdef __CYGWIN__
#include <winerror.h>
extern unsigned long __attribute__((stdcall)) GetLastError(void);
#endif

static VALUE
rb_thread_flock(void *data)
{
#ifdef __CYGWIN__
    int old_errno = errno;
#endif
    int *op = data, ret = flock(op[0], op[1]);

#ifdef __CYGWIN__
    if (GetLastError() == ERROR_NOT_LOCKED) {
	ret = 0;
	errno = old_errno;
    }
#endif
    return (VALUE)ret;
}

/*
 *  call-seq:
 *     file.flock (locking_constant )-> 0 or false
 *
 *  Locks or unlocks a file according to <i>locking_constant</i> (a
 *  logical <em>or</em> of the values in the table below).
 *  Returns <code>false</code> if <code>File::LOCK_NB</code> is
 *  specified and the operation would otherwise have blocked. Not
 *  available on all platforms.
 *
 *  Locking constants (in class File):
 *
 *     LOCK_EX   | Exclusive lock. Only one process may hold an
 *               | exclusive lock for a given file at a time.
 *     ----------+------------------------------------------------
 *     LOCK_NB   | Don't block when locking. May be combined
 *               | with other lock options using logical or.
 *     ----------+------------------------------------------------
 *     LOCK_SH   | Shared lock. Multiple processes may each hold a
 *               | shared lock for a given file at the same time.
 *     ----------+------------------------------------------------
 *     LOCK_UN   | Unlock.
 *
 *  Example:
 *
 *     # update a counter using write lock
 *     # don't use "w" because it truncates the file before lock.
 *     File.open("counter", File::RDWR|File::CREAT, 0644) {|f|
 *       f.flock(File::LOCK_EX)
 *       value = f.read.to_i + 1
 *       f.rewind
 *       f.write("#{value}\n")
 *       f.flush
 *       f.truncate(f.pos)
 *     }
 *
 *     # read the counter using read lock
 *     File.open("counter", "r") {|f|
 *       f.flock(File::LOCK_SH)
 *       p f.read
 *     }
 *
 */

static VALUE
rb_file_flock(VALUE obj, VALUE operation)
{
    rb_io_t *fptr;
    int op[2], op1;

    rb_secure(2);
    op[1] = op1 = NUM2INT(operation);
    GetOpenFile(obj, fptr);
    op[0] = fptr->fd;

    if (fptr->mode & FMODE_WRITABLE) {
	rb_io_flush(obj);
    }
    while ((int)rb_thread_io_blocking_region(rb_thread_flock, op, fptr->fd) < 0) {
	switch (errno) {
	  case EAGAIN:
	  case EACCES:
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
	  case EWOULDBLOCK:
#endif
	    if (op1 & LOCK_NB) return Qfalse;
	    rb_thread_polling();
	    rb_io_check_closed(fptr);
	    continue;

	  case EINTR:
#if defined(ERESTART)
	  case ERESTART:
#endif
	    break;

	  default:
	    rb_sys_fail_path(fptr->pathv);
	}
    }
    return INT2FIX(0);
}
#undef flock

static void
test_check(int n, int argc, VALUE *argv)
{
    int i;

    rb_secure(2);
    n+=1;
    rb_check_arity(argc, n, n);
    for (i=1; i<n; i++) {
	switch (TYPE(argv[i])) {
	  case T_STRING:
	  default:
	    FilePathValue(argv[i]);
	    break;
	  case T_FILE:
	    break;
	}
    }
}

#define CHECK(n) test_check((n), argc, argv)

/*
 *  call-seq:
 *     test(int_cmd, file1 [, file2] ) -> obj
 *
 *  Uses the integer +int_cmd+ to perform various tests on +file1+ (first
 *  table below) or on +file1+ and +file2+ (second table).
 *
 *  File tests on a single file:
 *
 *    Test   Returns   Meaning
 *    "A"  | Time    | Last access time for file1
 *    "b"  | boolean | True if file1 is a block device
 *    "c"  | boolean | True if file1 is a character device
 *    "C"  | Time    | Last change time for file1
 *    "d"  | boolean | True if file1 exists and is a directory
 *    "e"  | boolean | True if file1 exists
 *    "f"  | boolean | True if file1 exists and is a regular file
 *    "g"  | boolean | True if file1 has the \CF{setgid} bit
 *         |         | set (false under NT)
 *    "G"  | boolean | True if file1 exists and has a group
 *         |         | ownership equal to the caller's group
 *    "k"  | boolean | True if file1 exists and has the sticky bit set
 *    "l"  | boolean | True if file1 exists and is a symbolic link
 *    "M"  | Time    | Last modification time for file1
 *    "o"  | boolean | True if file1 exists and is owned by
 *         |         | the caller's effective uid
 *    "O"  | boolean | True if file1 exists and is owned by
 *         |         | the caller's real uid
 *    "p"  | boolean | True if file1 exists and is a fifo
 *    "r"  | boolean | True if file1 is readable by the effective
 *         |         | uid/gid of the caller
 *    "R"  | boolean | True if file is readable by the real
 *         |         | uid/gid of the caller
 *    "s"  | int/nil | If file1 has nonzero size, return the size,
 *         |         | otherwise return nil
 *    "S"  | boolean | True if file1 exists and is a socket
 *    "u"  | boolean | True if file1 has the setuid bit set
 *    "w"  | boolean | True if file1 exists and is writable by
 *         |         | the effective uid/gid
 *    "W"  | boolean | True if file1 exists and is writable by
 *         |         | the real uid/gid
 *    "x"  | boolean | True if file1 exists and is executable by
 *         |         | the effective uid/gid
 *    "X"  | boolean | True if file1 exists and is executable by
 *         |         | the real uid/gid
 *    "z"  | boolean | True if file1 exists and has a zero length
 *
 *  Tests that take two files:
 *
 *    "-"  | boolean | True if file1 and file2 are identical
 *    "="  | boolean | True if the modification times of file1
 *         |         | and file2 are equal
 *    "<"  | boolean | True if the modification time of file1
 *         |         | is prior to that of file2
 *    ">"  | boolean | True if the modification time of file1
 *         |         | is after that of file2
 */

static VALUE
rb_f_test(int argc, VALUE *argv)
{
    int cmd;

    if (argc == 0) rb_check_arity(argc, 2, 3);
    cmd = NUM2CHR(argv[0]);
    if (cmd == 0) goto unknown;
    if (strchr("bcdefgGkloOprRsSuwWxXz", cmd)) {
	CHECK(1);
	switch (cmd) {
	  case 'b':
	    return rb_file_blockdev_p(0, argv[1]);

	  case 'c':
	    return rb_file_chardev_p(0, argv[1]);

	  case 'd':
	    return rb_file_directory_p(0, argv[1]);

	  case 'a':
	  case 'e':
	    return rb_file_exist_p(0, argv[1]);

	  case 'f':
	    return rb_file_file_p(0, argv[1]);

	  case 'g':
	    return rb_file_sgid_p(0, argv[1]);

	  case 'G':
	    return rb_file_grpowned_p(0, argv[1]);

	  case 'k':
	    return rb_file_sticky_p(0, argv[1]);

	  case 'l':
	    return rb_file_symlink_p(0, argv[1]);

	  case 'o':
	    return rb_file_owned_p(0, argv[1]);

	  case 'O':
	    return rb_file_rowned_p(0, argv[1]);

	  case 'p':
	    return rb_file_pipe_p(0, argv[1]);

	  case 'r':
	    return rb_file_readable_p(0, argv[1]);

	  case 'R':
	    return rb_file_readable_real_p(0, argv[1]);

	  case 's':
	    return rb_file_size_p(0, argv[1]);

	  case 'S':
	    return rb_file_socket_p(0, argv[1]);

	  case 'u':
	    return rb_file_suid_p(0, argv[1]);

	  case 'w':
	    return rb_file_writable_p(0, argv[1]);

	  case 'W':
	    return rb_file_writable_real_p(0, argv[1]);

	  case 'x':
	    return rb_file_executable_p(0, argv[1]);

	  case 'X':
	    return rb_file_executable_real_p(0, argv[1]);

	  case 'z':
	    return rb_file_zero_p(0, argv[1]);
	}
    }

    if (strchr("MAC", cmd)) {
	struct stat st;
	VALUE fname = argv[1];

	CHECK(1);
	if (rb_stat(fname, &st) == -1) {
	    FilePathValue(fname);
	    rb_sys_fail_path(fname);
	}

	switch (cmd) {
	  case 'A':
	    return stat_atime(&st);
	  case 'M':
	    return stat_mtime(&st);
	  case 'C':
	    return stat_ctime(&st);
	}
    }

    if (cmd == '-') {
	CHECK(2);
	return rb_file_identical_p(0, argv[1], argv[2]);
    }

    if (strchr("=<>", cmd)) {
	struct stat st1, st2;

	CHECK(2);
	if (rb_stat(argv[1], &st1) < 0) return Qfalse;
	if (rb_stat(argv[2], &st2) < 0) return Qfalse;

	switch (cmd) {
	  case '=':
	    if (st1.st_mtime == st2.st_mtime) return Qtrue;
	    return Qfalse;

	  case '>':
	    if (st1.st_mtime > st2.st_mtime) return Qtrue;
	    return Qfalse;

	  case '<':
	    if (st1.st_mtime < st2.st_mtime) return Qtrue;
	    return Qfalse;
	}
    }
  unknown:
    /* unknown command */
    if (ISPRINT(cmd)) {
	rb_raise(rb_eArgError, "unknown command '%s%c'", cmd == '\'' || cmd == '\\' ? "\\" : "", cmd);
    }
    else {
	rb_raise(rb_eArgError, "unknown command \"\\x%02X\"", cmd);
    }
    return Qnil;		/* not reached */
}


/*
 *  Document-class: File::Stat
 *
 *  Objects of class <code>File::Stat</code> encapsulate common status
 *  information for <code>File</code> objects. The information is
 *  recorded at the moment the <code>File::Stat</code> object is
 *  created; changes made to the file after that point will not be
 *  reflected. <code>File::Stat</code> objects are returned by
 *  <code>IO#stat</code>, <code>File::stat</code>,
 *  <code>File#lstat</code>, and <code>File::lstat</code>. Many of these
 *  methods return platform-specific values, and not all values are
 *  meaningful on all systems. See also <code>Kernel#test</code>.
 */

static VALUE
rb_stat_s_alloc(VALUE klass)
{
    return stat_new_0(klass, 0);
}

/*
 * call-seq:
 *
 *   File::Stat.new(file_name)  -> stat
 *
 * Create a File::Stat object for the given file name (raising an
 * exception if the file doesn't exist).
 */

static VALUE
rb_stat_init(VALUE obj, VALUE fname)
{
    struct stat st, *nst;

    rb_secure(2);
    FilePathValue(fname);
    fname = rb_str_encode_ospath(fname);
    if (STAT(StringValueCStr(fname), &st) == -1) {
	rb_sys_fail_path(fname);
    }
    if (DATA_PTR(obj)) {
	xfree(DATA_PTR(obj));
	DATA_PTR(obj) = NULL;
    }
    nst = ALLOC(struct stat);
    *nst = st;
    DATA_PTR(obj) = nst;

    return Qnil;
}

/* :nodoc: */
static VALUE
rb_stat_init_copy(VALUE copy, VALUE orig)
{
    struct stat *nst;

    if (copy == orig) return orig;
    rb_check_frozen(copy);
    /* need better argument type check */
    if (!rb_obj_is_instance_of(orig, rb_obj_class(copy))) {
	rb_raise(rb_eTypeError, "wrong argument class");
    }
    if (DATA_PTR(copy)) {
	xfree(DATA_PTR(copy));
	DATA_PTR(copy) = 0;
    }
    if (DATA_PTR(orig)) {
	nst = ALLOC(struct stat);
	*nst = *(struct stat*)DATA_PTR(orig);
	DATA_PTR(copy) = nst;
    }

    return copy;
}

/*
 *  call-seq:
 *     stat.ftype   -> string
 *
 *  Identifies the type of <i>stat</i>. The return string is one of:
 *  ``<code>file</code>'', ``<code>directory</code>'',
 *  ``<code>characterSpecial</code>'', ``<code>blockSpecial</code>'',
 *  ``<code>fifo</code>'', ``<code>link</code>'',
 *  ``<code>socket</code>'', or ``<code>unknown</code>''.
 *
 *     File.stat("/dev/tty").ftype   #=> "characterSpecial"
 *
 */

static VALUE
rb_stat_ftype(VALUE obj)
{
    return rb_file_ftype(get_stat(obj));
}

/*
 *  call-seq:
 *     stat.directory?   -> true or false
 *
 *  Returns <code>true</code> if <i>stat</i> is a directory,
 *  <code>false</code> otherwise.
 *
 *     File.stat("testfile").directory?   #=> false
 *     File.stat(".").directory?          #=> true
 */

static VALUE
rb_stat_d(VALUE obj)
{
    if (S_ISDIR(get_stat(obj)->st_mode)) return Qtrue;
    return Qfalse;
}

/*
 *  call-seq:
 *     stat.pipe?    -> true or false
 *
 *  Returns <code>true</code> if the operating system supports pipes and
 *  <i>stat</i> is a pipe; <code>false</code> otherwise.
 */

static VALUE
rb_stat_p(VALUE obj)
{
#ifdef S_IFIFO
    if (S_ISFIFO(get_stat(obj)->st_mode)) return Qtrue;

#endif
    return Qfalse;
}

/*
 *  call-seq:
 *     stat.symlink?    -> true or false
 *
 *  Returns <code>true</code> if <i>stat</i> is a symbolic link,
 *  <code>false</code> if it isn't or if the operating system doesn't
 *  support this feature. As <code>File::stat</code> automatically
 *  follows symbolic links, <code>symlink?</code> will always be
 *  <code>false</code> for an object returned by
 *  <code>File::stat</code>.
 *
 *     File.symlink("testfile", "alink")   #=> 0
 *     File.stat("alink").symlink?         #=> false
 *     File.lstat("alink").symlink?        #=> true
 *
 */

static VALUE
rb_stat_l(VALUE obj)
{
#ifdef S_ISLNK
    if (S_ISLNK(get_stat(obj)->st_mode)) return Qtrue;
#endif
    return Qfalse;
}

/*
 *  call-seq:
 *     stat.socket?    -> true or false
 *
 *  Returns <code>true</code> if <i>stat</i> is a socket,
 *  <code>false</code> if it isn't or if the operating system doesn't
 *  support this feature.
 *
 *     File.stat("testfile").socket?   #=> false
 *
 */

static VALUE
rb_stat_S(VALUE obj)
{
#ifdef S_ISSOCK
    if (S_ISSOCK(get_stat(obj)->st_mode)) return Qtrue;

#endif
    return Qfalse;
}

/*
 *  call-seq:
 *     stat.blockdev?   -> true or false
 *
 *  Returns <code>true</code> if the file is a block device,
 *  <code>false</code> if it isn't or if the operating system doesn't
 *  support this feature.
 *
 *     File.stat("testfile").blockdev?    #=> false
 *     File.stat("/dev/hda1").blockdev?   #=> true
 *
 */

static VALUE
rb_stat_b(VALUE obj)
{
#ifdef S_ISBLK
    if (S_ISBLK(get_stat(obj)->st_mode)) return Qtrue;

#endif
    return Qfalse;
}

/*
 *  call-seq:
 *     stat.chardev?    -> true or false
 *
 *  Returns <code>true</code> if the file is a character device,
 *  <code>false</code> if it isn't or if the operating system doesn't
 *  support this feature.
 *
 *     File.stat("/dev/tty").chardev?   #=> true
 *
 */

static VALUE
rb_stat_c(VALUE obj)
{
    if (S_ISCHR(get_stat(obj)->st_mode)) return Qtrue;

    return Qfalse;
}

/*
 *  call-seq:
 *     stat.owned?    -> true or false
 *
 *  Returns <code>true</code> if the effective user id of the process is
 *  the same as the owner of <i>stat</i>.
 *
 *     File.stat("testfile").owned?      #=> true
 *     File.stat("/etc/passwd").owned?   #=> false
 *
 */

static VALUE
rb_stat_owned(VALUE obj)
{
    if (get_stat(obj)->st_uid == geteuid()) return Qtrue;
    return Qfalse;
}

static VALUE
rb_stat_rowned(VALUE obj)
{
    if (get_stat(obj)->st_uid == getuid()) return Qtrue;
    return Qfalse;
}

/*
 *  call-seq:
 *     stat.grpowned?   -> true or false
 *
 *  Returns true if the effective group id of the process is the same as
 *  the group id of <i>stat</i>. On Windows NT, returns <code>false</code>.
 *
 *     File.stat("testfile").grpowned?      #=> true
 *     File.stat("/etc/passwd").grpowned?   #=> false
 *
 */

static VALUE
rb_stat_grpowned(VALUE obj)
{
#ifndef _WIN32
    if (rb_group_member(get_stat(obj)->st_gid)) return Qtrue;
#endif
    return Qfalse;
}

/*
 *  call-seq:
 *     stat.readable?    -> true or false
 *
 *  Returns <code>true</code> if <i>stat</i> is readable by the
 *  effective user id of this process.
 *
 *     File.stat("testfile").readable?   #=> true
 *
 */

static VALUE
rb_stat_r(VALUE obj)
{
    struct stat *st = get_stat(obj);

#ifdef USE_GETEUID
    if (geteuid() == 0) return Qtrue;
#endif
#ifdef S_IRUSR
    if (rb_stat_owned(obj))
	return st->st_mode & S_IRUSR ? Qtrue : Qfalse;
#endif
#ifdef S_IRGRP
    if (rb_stat_grpowned(obj))
	return st->st_mode & S_IRGRP ? Qtrue : Qfalse;
#endif
#ifdef S_IROTH
    if (!(st->st_mode & S_IROTH)) return Qfalse;
#endif
    return Qtrue;
}

/*
 *  call-seq:
 *     stat.readable_real?  ->  true or false
 *
 *  Returns <code>true</code> if <i>stat</i> is readable by the real
 *  user id of this process.
 *
 *     File.stat("testfile").readable_real?   #=> true
 *
 */

static VALUE
rb_stat_R(VALUE obj)
{
    struct stat *st = get_stat(obj);

#ifdef USE_GETEUID
    if (getuid() == 0) return Qtrue;
#endif
#ifdef S_IRUSR
    if (rb_stat_rowned(obj))
	return st->st_mode & S_IRUSR ? Qtrue : Qfalse;
#endif
#ifdef S_IRGRP
    if (rb_group_member(get_stat(obj)->st_gid))
	return st->st_mode & S_IRGRP ? Qtrue : Qfalse;
#endif
#ifdef S_IROTH
    if (!(st->st_mode & S_IROTH)) return Qfalse;
#endif
    return Qtrue;
}

/*
 * call-seq:
 *    stat.world_readable? -> fixnum or nil
 *
 * If <i>stat</i> is readable by others, returns an integer
 * representing the file permission bits of <i>stat</i>. Returns
 * <code>nil</code> otherwise. The meaning of the bits is platform
 * dependent; on Unix systems, see <code>stat(2)</code>.
 *
 *    m = File.stat("/etc/passwd").world_readable?  #=> 420
 *    sprintf("%o", m)				    #=> "644"
 */

static VALUE
rb_stat_wr(VALUE obj)
{
#ifdef S_IROTH
    if ((get_stat(obj)->st_mode & (S_IROTH)) == S_IROTH) {
	return UINT2NUM(get_stat(obj)->st_mode & (S_IRUGO|S_IWUGO|S_IXUGO));
    }
    else {
	return Qnil;
    }
#endif
}

/*
 *  call-seq:
 *     stat.writable?  ->  true or false
 *
 *  Returns <code>true</code> if <i>stat</i> is writable by the
 *  effective user id of this process.
 *
 *     File.stat("testfile").writable?   #=> true
 *
 */

static VALUE
rb_stat_w(VALUE obj)
{
    struct stat *st = get_stat(obj);

#ifdef USE_GETEUID
    if (geteuid() == 0) return Qtrue;
#endif
#ifdef S_IWUSR
    if (rb_stat_owned(obj))
	return st->st_mode & S_IWUSR ? Qtrue : Qfalse;
#endif
#ifdef S_IWGRP
    if (rb_stat_grpowned(obj))
	return st->st_mode & S_IWGRP ? Qtrue : Qfalse;
#endif
#ifdef S_IWOTH
    if (!(st->st_mode & S_IWOTH)) return Qfalse;
#endif
    return Qtrue;
}

/*
 *  call-seq:
 *     stat.writable_real?  ->  true or false
 *
 *  Returns <code>true</code> if <i>stat</i> is writable by the real
 *  user id of this process.
 *
 *     File.stat("testfile").writable_real?   #=> true
 *
 */

static VALUE
rb_stat_W(VALUE obj)
{
    struct stat *st = get_stat(obj);

#ifdef USE_GETEUID
    if (getuid() == 0) return Qtrue;
#endif
#ifdef S_IWUSR
    if (rb_stat_rowned(obj))
	return st->st_mode & S_IWUSR ? Qtrue : Qfalse;
#endif
#ifdef S_IWGRP
    if (rb_group_member(get_stat(obj)->st_gid))
	return st->st_mode & S_IWGRP ? Qtrue : Qfalse;
#endif
#ifdef S_IWOTH
    if (!(st->st_mode & S_IWOTH)) return Qfalse;
#endif
    return Qtrue;
}

/*
 * call-seq:
 *    stat.world_writable?  ->  fixnum or nil
 *
 * If <i>stat</i> is writable by others, returns an integer
 * representing the file permission bits of <i>stat</i>. Returns
 * <code>nil</code> otherwise. The meaning of the bits is platform
 * dependent; on Unix systems, see <code>stat(2)</code>.
 *
 *    m = File.stat("/tmp").world_writable?	    #=> 511
 *    sprintf("%o", m)				    #=> "777"
 */

static VALUE
rb_stat_ww(VALUE obj)
{
#ifdef S_IROTH
    if ((get_stat(obj)->st_mode & (S_IWOTH)) == S_IWOTH) {
	return UINT2NUM(get_stat(obj)->st_mode & (S_IRUGO|S_IWUGO|S_IXUGO));
    }
    else {
	return Qnil;
    }
#endif
}

/*
 *  call-seq:
 *     stat.executable?    -> true or false
 *
 *  Returns <code>true</code> if <i>stat</i> is executable or if the
 *  operating system doesn't distinguish executable files from
 *  nonexecutable files. The tests are made using the effective owner of
 *  the process.
 *
 *     File.stat("testfile").executable?   #=> false
 *
 */

static VALUE
rb_stat_x(VALUE obj)
{
    struct stat *st = get_stat(obj);

#ifdef USE_GETEUID
    if (geteuid() == 0) {
	return st->st_mode & S_IXUGO ? Qtrue : Qfalse;
    }
#endif
#ifdef S_IXUSR
    if (rb_stat_owned(obj))
	return st->st_mode & S_IXUSR ? Qtrue : Qfalse;
#endif
#ifdef S_IXGRP
    if (rb_stat_grpowned(obj))
	return st->st_mode & S_IXGRP ? Qtrue : Qfalse;
#endif
#ifdef S_IXOTH
    if (!(st->st_mode & S_IXOTH)) return Qfalse;
#endif
    return Qtrue;
}

/*
 *  call-seq:
 *     stat.executable_real?    -> true or false
 *
 *  Same as <code>executable?</code>, but tests using the real owner of
 *  the process.
 */

static VALUE
rb_stat_X(VALUE obj)
{
    struct stat *st = get_stat(obj);

#ifdef USE_GETEUID
    if (getuid() == 0) {
	return st->st_mode & S_IXUGO ? Qtrue : Qfalse;
    }
#endif
#ifdef S_IXUSR
    if (rb_stat_rowned(obj))
	return st->st_mode & S_IXUSR ? Qtrue : Qfalse;
#endif
#ifdef S_IXGRP
    if (rb_group_member(get_stat(obj)->st_gid))
	return st->st_mode & S_IXGRP ? Qtrue : Qfalse;
#endif
#ifdef S_IXOTH
    if (!(st->st_mode & S_IXOTH)) return Qfalse;
#endif
    return Qtrue;
}

/*
 *  call-seq:
 *     stat.file?    -> true or false
 *
 *  Returns <code>true</code> if <i>stat</i> is a regular file (not
 *  a device file, pipe, socket, etc.).
 *
 *     File.stat("testfile").file?   #=> true
 *
 */

static VALUE
rb_stat_f(VALUE obj)
{
    if (S_ISREG(get_stat(obj)->st_mode)) return Qtrue;
    return Qfalse;
}

/*
 *  call-seq:
 *     stat.zero?    -> true or false
 *
 *  Returns <code>true</code> if <i>stat</i> is a zero-length file;
 *  <code>false</code> otherwise.
 *
 *     File.stat("testfile").zero?   #=> false
 *
 */

static VALUE
rb_stat_z(VALUE obj)
{
    if (get_stat(obj)->st_size == 0) return Qtrue;
    return Qfalse;
}

/*
 *  call-seq:
 *     state.size    -> integer
 *
 *  Returns the size of <i>stat</i> in bytes.
 *
 *     File.stat("testfile").size   #=> 66
 *
 */

static VALUE
rb_stat_s(VALUE obj)
{
    off_t size = get_stat(obj)->st_size;

    if (size == 0) return Qnil;
    return OFFT2NUM(size);
}

/*
 *  call-seq:
 *     stat.setuid?    -> true or false
 *
 *  Returns <code>true</code> if <i>stat</i> has the set-user-id
 *  permission bit set, <code>false</code> if it doesn't or if the
 *  operating system doesn't support this feature.
 *
 *     File.stat("/bin/su").setuid?   #=> true
 */

static VALUE
rb_stat_suid(VALUE obj)
{
#ifdef S_ISUID
    if (get_stat(obj)->st_mode & S_ISUID) return Qtrue;
#endif
    return Qfalse;
}

/*
 *  call-seq:
 *     stat.setgid?   -> true or false
 *
 *  Returns <code>true</code> if <i>stat</i> has the set-group-id
 *  permission bit set, <code>false</code> if it doesn't or if the
 *  operating system doesn't support this feature.
 *
 *     File.stat("/usr/sbin/lpc").setgid?   #=> true
 *
 */

static VALUE
rb_stat_sgid(VALUE obj)
{
#ifdef S_ISGID
    if (get_stat(obj)->st_mode & S_ISGID) return Qtrue;
#endif
    return Qfalse;
}

/*
 *  call-seq:
 *     stat.sticky?    -> true or false
 *
 *  Returns <code>true</code> if <i>stat</i> has its sticky bit set,
 *  <code>false</code> if it doesn't or if the operating system doesn't
 *  support this feature.
 *
 *     File.stat("testfile").sticky?   #=> false
 *
 */

static VALUE
rb_stat_sticky(VALUE obj)
{
#ifdef S_ISVTX
    if (get_stat(obj)->st_mode & S_ISVTX) return Qtrue;
#endif
    return Qfalse;
}

VALUE rb_mFConst;

void
rb_file_const(const char *name, VALUE value)
{
    rb_define_const(rb_mFConst, name, value);
}

int
rb_is_absolute_path(const char *path)
{
#ifdef DOSISH_DRIVE_LETTER
    if (has_drive_letter(path) && isdirsep(path[2])) return 1;
#endif
#ifdef DOSISH_UNC
    if (isdirsep(path[0]) && isdirsep(path[1])) return 1;
#endif
#ifndef DOSISH
    if (path[0] == '/') return 1;
#endif
    return 0;
}

#ifndef ENABLE_PATH_CHECK
# if defined DOSISH || defined __CYGWIN__
#   define ENABLE_PATH_CHECK 0
# else
#   define ENABLE_PATH_CHECK 1
# endif
#endif

#if ENABLE_PATH_CHECK
static int
path_check_0(VALUE path, int execpath)
{
    struct stat st;
    const char *p0 = StringValueCStr(path);
    const char *e0;
    rb_encoding *enc;
    char *p = 0, *s;

    if (!rb_is_absolute_path(p0)) {
	char *buf = my_getcwd();
	VALUE newpath;

	newpath = rb_str_new2(buf);
	xfree(buf);

	rb_str_cat2(newpath, "/");
	rb_str_cat2(newpath, p0);
	path = newpath;
	p0 = RSTRING_PTR(path);
    }
    e0 = p0 + RSTRING_LEN(path);
    enc = rb_enc_get(path);
    for (;;) {
#ifndef S_IWOTH
# define S_IWOTH 002
#endif
	if (STAT(p0, &st) == 0 && S_ISDIR(st.st_mode) && (st.st_mode & S_IWOTH)
#ifdef S_ISVTX
	    && !(p && execpath && (st.st_mode & S_ISVTX))
#endif
	    && !access(p0, W_OK)) {
	    rb_warn("Insecure world writable dir %s in %sPATH, mode 0%"
		    PRI_MODET_PREFIX"o",
		    p0, (execpath ? "" : "LOAD_"), st.st_mode);
	    if (p) *p = '/';
	    RB_GC_GUARD(path);
	    return 0;
	}
	s = strrdirsep(p0, e0, enc);
	if (p) *p = '/';
	if (!s || s == p0) return 1;
	p = s;
	e0 = p;
	*p = '\0';
    }
}
#endif

#if ENABLE_PATH_CHECK
#define fpath_check(path) path_check_0((path), FALSE)
#else
#define fpath_check(path) 1
#endif

int
rb_path_check(const char *path)
{
#if ENABLE_PATH_CHECK
    const char *p0, *p, *pend;
    const char sep = PATH_SEP_CHAR;

    if (!path) return 1;

    pend = path + strlen(path);
    p0 = path;
    p = strchr(path, sep);
    if (!p) p = pend;

    for (;;) {
	if (!path_check_0(rb_str_new(p0, p - p0), TRUE)) {
	    return 0;		/* not safe */
	}
	p0 = p + 1;
	if (p0 > pend) break;
	p = strchr(p0, sep);
	if (!p) p = pend;
    }
#endif
    return 1;
}

#ifndef _WIN32
int
rb_file_load_ok(const char *path)
{
    int ret = 1;
    int fd = rb_cloexec_open(path, O_RDONLY, 0);
    if (fd == -1) return 0;
    rb_update_max_fd(fd);
#if !defined DOSISH
    {
	struct stat st;
	if (fstat(fd, &st) || !S_ISREG(st.st_mode)) {
	    ret = 0;
	}
    }
#endif
    (void)close(fd);
    return ret;
}
#endif

static int
is_explicit_relative(const char *path)
{
    if (*path++ != '.') return 0;
    if (*path == '.') path++;
    return isdirsep(*path);
}

static VALUE
copy_path_class(VALUE path, VALUE orig)
{
    RBASIC(path)->klass = rb_obj_class(orig);
    OBJ_FREEZE(path);
    return path;
}

int
rb_find_file_ext(VALUE *filep, const char *const *ext)
{
    return rb_find_file_ext_safe(filep, ext, rb_safe_level());
}

int
rb_find_file_ext_safe(VALUE *filep, const char *const *ext, int safe_level)
{
    const char *f = StringValueCStr(*filep);
    VALUE fname = *filep, load_path, tmp;
    long i, j, fnlen;
    int expanded = 0;

    if (!ext[0]) return 0;

    if (f[0] == '~') {
	fname = file_expand_path_1(fname);
	if (safe_level >= 1 && OBJ_TAINTED(fname)) {
	    rb_raise(rb_eSecurityError, "loading from unsafe file %s", f);
	}
	f = RSTRING_PTR(fname);
	*filep = fname;
	expanded = 1;
    }

    if (expanded || rb_is_absolute_path(f) || is_explicit_relative(f)) {
	if (safe_level >= 1 && !fpath_check(fname)) {
	    rb_raise(rb_eSecurityError, "loading from unsafe path %s", f);
	}
	if (!expanded) fname = file_expand_path_1(fname);
	fnlen = RSTRING_LEN(fname);
	for (i=0; ext[i]; i++) {
	    rb_str_cat2(fname, ext[i]);
	    if (rb_file_load_ok(RSTRING_PTR(fname))) {
		*filep = copy_path_class(fname, *filep);
		return (int)(i+1);
	    }
	    rb_str_set_len(fname, fnlen);
	}
	return 0;
    }

    if (safe_level >= 4) {
	rb_raise(rb_eSecurityError, "loading from non-absolute path %s", f);
    }

    RB_GC_GUARD(load_path) = rb_get_load_path();
    if (!load_path) return 0;

    fname = rb_str_dup(*filep);
    RBASIC(fname)->klass = 0;
    fnlen = RSTRING_LEN(fname);
    tmp = rb_str_tmp_new(MAXPATHLEN + 2);
    for (j=0; ext[j]; j++) {
	rb_str_cat2(fname, ext[j]);
	for (i = 0; i < RARRAY_LEN(load_path); i++) {
	    VALUE str = RARRAY_PTR(load_path)[i];

	    RB_GC_GUARD(str) = rb_get_path_check(str, safe_level);
	    if (RSTRING_LEN(str) == 0) continue;
	    file_expand_path(fname, str, 0, tmp);
	    if (rb_file_load_ok(RSTRING_PTR(tmp))) {
		*filep = copy_path_class(tmp, *filep);
		return (int)(j+1);
	    }
	    FL_UNSET(tmp, FL_TAINT | FL_UNTRUSTED);
	}
	rb_str_set_len(fname, fnlen);
    }
    RB_GC_GUARD(load_path);
    return 0;
}

VALUE
rb_find_file(VALUE path)
{
    return rb_find_file_safe(path, rb_safe_level());
}

VALUE
rb_find_file_safe(VALUE path, int safe_level)
{
    VALUE tmp, load_path;
    const char *f = StringValueCStr(path);
    int expanded = 0;

    if (f[0] == '~') {
	tmp = file_expand_path_1(path);
	if (safe_level >= 1 && OBJ_TAINTED(tmp)) {
	    rb_raise(rb_eSecurityError, "loading from unsafe file %s", f);
	}
	path = copy_path_class(tmp, path);
	f = RSTRING_PTR(path);
	expanded = 1;
    }

    if (expanded || rb_is_absolute_path(f) || is_explicit_relative(f)) {
	if (safe_level >= 1 && !fpath_check(path)) {
	    rb_raise(rb_eSecurityError, "loading from unsafe path %s", f);
	}
	if (!rb_file_load_ok(f)) return 0;
	if (!expanded)
	    path = copy_path_class(file_expand_path_1(path), path);
	return path;
    }

    if (safe_level >= 4) {
	rb_raise(rb_eSecurityError, "loading from non-absolute path %s", f);
    }

    RB_GC_GUARD(load_path) = rb_get_load_path();
    if (load_path) {
	long i;

	tmp = rb_str_tmp_new(MAXPATHLEN + 2);
	for (i = 0; i < RARRAY_LEN(load_path); i++) {
	    VALUE str = RARRAY_PTR(load_path)[i];
	    RB_GC_GUARD(str) = rb_get_path_check(str, safe_level);
	    if (RSTRING_LEN(str) > 0) {
		file_expand_path(path, str, 0, tmp);
		f = RSTRING_PTR(tmp);
		if (rb_file_load_ok(f)) goto found;
	    }
	}
	return 0;
    }
    else {
	return 0;		/* no path, no load */
    }

  found:
    if (safe_level >= 1 && !fpath_check(tmp)) {
	rb_raise(rb_eSecurityError, "loading from unsafe file %s", f);
    }

    return copy_path_class(tmp, path);
}

static void
define_filetest_function(const char *name, VALUE (*func)(ANYARGS), int argc)
{
    rb_define_module_function(rb_mFileTest, name, func, argc);
    rb_define_singleton_method(rb_cFile, name, func, argc);
}

static const char null_device[] =
#if defined DOSISH
    "NUL"
#elif defined AMIGA || defined __amigaos__
    "NIL"
#elif defined __VMS
    "NL:"
#else
    "/dev/null"
#endif
    ;

/*
 *  A <code>File</code> is an abstraction of any file object accessible
 *  by the program and is closely associated with class <code>IO</code>
 *  <code>File</code> includes the methods of module
 *  <code>FileTest</code> as class methods, allowing you to write (for
 *  example) <code>File.exist?("foo")</code>.
 *
 *  In the description of File methods,
 *  <em>permission bits</em> are a platform-specific
 *  set of bits that indicate permissions of a file. On Unix-based
 *  systems, permissions are viewed as a set of three octets, for the
 *  owner, the group, and the rest of the world. For each of these
 *  entities, permissions may be set to read, write, or execute the
 *  file:
 *
 *  The permission bits <code>0644</code> (in octal) would thus be
 *  interpreted as read/write for owner, and read-only for group and
 *  other. Higher-order bits may also be used to indicate the type of
 *  file (plain, directory, pipe, socket, and so on) and various other
 *  special features. If the permissions are for a directory, the
 *  meaning of the execute bit changes; when set the directory can be
 *  searched.
 *
 *  On non-Posix operating systems, there may be only the ability to
 *  make a file read-only or read-write. In this case, the remaining
 *  permission bits will be synthesized to resemble typical values. For
 *  instance, on Windows NT the default permission bits are
 *  <code>0644</code>, which means read/write for owner, read-only for
 *  all others. The only change that can be made is to make the file
 *  read-only, which is reported as <code>0444</code>.
 */

void
Init_File(void)
{
    rb_mFileTest = rb_define_module("FileTest");
    rb_cFile = rb_define_class("File", rb_cIO);

    define_filetest_function("directory?", rb_file_directory_p, 1);
    define_filetest_function("exist?", rb_file_exist_p, 1);
    define_filetest_function("exists?", rb_file_exist_p, 1);
    define_filetest_function("readable?", rb_file_readable_p, 1);
    define_filetest_function("readable_real?", rb_file_readable_real_p, 1);
    define_filetest_function("world_readable?", rb_file_world_readable_p, 1);
    define_filetest_function("writable?", rb_file_writable_p, 1);
    define_filetest_function("writable_real?", rb_file_writable_real_p, 1);
    define_filetest_function("world_writable?", rb_file_world_writable_p, 1);
    define_filetest_function("executable?", rb_file_executable_p, 1);
    define_filetest_function("executable_real?", rb_file_executable_real_p, 1);
    define_filetest_function("file?", rb_file_file_p, 1);
    define_filetest_function("zero?", rb_file_zero_p, 1);
    define_filetest_function("size?", rb_file_size_p, 1);
    define_filetest_function("size", rb_file_s_size, 1);
    define_filetest_function("owned?", rb_file_owned_p, 1);
    define_filetest_function("grpowned?", rb_file_grpowned_p, 1);

    define_filetest_function("pipe?", rb_file_pipe_p, 1);
    define_filetest_function("symlink?", rb_file_symlink_p, 1);
    define_filetest_function("socket?", rb_file_socket_p, 1);

    define_filetest_function("blockdev?", rb_file_blockdev_p, 1);
    define_filetest_function("chardev?", rb_file_chardev_p, 1);

    define_filetest_function("setuid?", rb_file_suid_p, 1);
    define_filetest_function("setgid?", rb_file_sgid_p, 1);
    define_filetest_function("sticky?", rb_file_sticky_p, 1);

    define_filetest_function("identical?", rb_file_identical_p, 2);

    rb_define_singleton_method(rb_cFile, "stat",  rb_file_s_stat, 1);
    rb_define_singleton_method(rb_cFile, "lstat", rb_file_s_lstat, 1);
    rb_define_singleton_method(rb_cFile, "ftype", rb_file_s_ftype, 1);

    rb_define_singleton_method(rb_cFile, "atime", rb_file_s_atime, 1);
    rb_define_singleton_method(rb_cFile, "mtime", rb_file_s_mtime, 1);
    rb_define_singleton_method(rb_cFile, "ctime", rb_file_s_ctime, 1);

    rb_define_singleton_method(rb_cFile, "utime", rb_file_s_utime, -1);
    rb_define_singleton_method(rb_cFile, "chmod", rb_file_s_chmod, -1);
    rb_define_singleton_method(rb_cFile, "chown", rb_file_s_chown, -1);
    rb_define_singleton_method(rb_cFile, "lchmod", rb_file_s_lchmod, -1);
    rb_define_singleton_method(rb_cFile, "lchown", rb_file_s_lchown, -1);

    rb_define_singleton_method(rb_cFile, "link", rb_file_s_link, 2);
    rb_define_singleton_method(rb_cFile, "symlink", rb_file_s_symlink, 2);
    rb_define_singleton_method(rb_cFile, "readlink", rb_file_s_readlink, 1);

    rb_define_singleton_method(rb_cFile, "unlink", rb_file_s_unlink, -2);
    rb_define_singleton_method(rb_cFile, "delete", rb_file_s_unlink, -2);
    rb_define_singleton_method(rb_cFile, "rename", rb_file_s_rename, 2);
    rb_define_singleton_method(rb_cFile, "umask", rb_file_s_umask, -1);
    rb_define_singleton_method(rb_cFile, "truncate", rb_file_s_truncate, 2);
    rb_define_singleton_method(rb_cFile, "expand_path", rb_file_s_expand_path, -1);
    rb_define_singleton_method(rb_cFile, "absolute_path", rb_file_s_absolute_path, -1);
    rb_define_singleton_method(rb_cFile, "realpath", rb_file_s_realpath, -1);
    rb_define_singleton_method(rb_cFile, "realdirpath", rb_file_s_realdirpath, -1);
    rb_define_singleton_method(rb_cFile, "basename", rb_file_s_basename, -1);
    rb_define_singleton_method(rb_cFile, "dirname", rb_file_s_dirname, 1);
    rb_define_singleton_method(rb_cFile, "extname", rb_file_s_extname, 1);
    rb_define_singleton_method(rb_cFile, "path", rb_file_s_path, 1);

    separator = rb_obj_freeze(rb_usascii_str_new2("/"));
    rb_define_const(rb_cFile, "Separator", separator);
    rb_define_const(rb_cFile, "SEPARATOR", separator);
    rb_define_singleton_method(rb_cFile, "split",  rb_file_s_split, 1);
    rb_define_singleton_method(rb_cFile, "join",   rb_file_s_join, -2);

#ifdef DOSISH
    rb_define_const(rb_cFile, "ALT_SEPARATOR", rb_obj_freeze(rb_usascii_str_new2(file_alt_separator)));
#else
    rb_define_const(rb_cFile, "ALT_SEPARATOR", Qnil);
#endif
    rb_define_const(rb_cFile, "PATH_SEPARATOR", rb_obj_freeze(rb_str_new2(PATH_SEP)));

    rb_define_method(rb_cIO, "stat",  rb_io_stat, 0); /* this is IO's method */
    rb_define_method(rb_cFile, "lstat",  rb_file_lstat, 0);

    rb_define_method(rb_cFile, "atime", rb_file_atime, 0);
    rb_define_method(rb_cFile, "mtime", rb_file_mtime, 0);
    rb_define_method(rb_cFile, "ctime", rb_file_ctime, 0);
    rb_define_method(rb_cFile, "size", rb_file_size, 0);

    rb_define_method(rb_cFile, "chmod", rb_file_chmod, 1);
    rb_define_method(rb_cFile, "chown", rb_file_chown, 2);
    rb_define_method(rb_cFile, "truncate", rb_file_truncate, 1);

    rb_define_method(rb_cFile, "flock", rb_file_flock, 1);

    rb_mFConst = rb_define_module_under(rb_cFile, "Constants");
    rb_include_module(rb_cIO, rb_mFConst);
    rb_file_const("LOCK_SH", INT2FIX(LOCK_SH));
    rb_file_const("LOCK_EX", INT2FIX(LOCK_EX));
    rb_file_const("LOCK_UN", INT2FIX(LOCK_UN));
    rb_file_const("LOCK_NB", INT2FIX(LOCK_NB));

    rb_file_const("NULL", rb_obj_freeze(rb_usascii_str_new2(null_device)));

    rb_define_method(rb_cFile, "path",  rb_file_path, 0);
    rb_define_method(rb_cFile, "to_path",  rb_file_path, 0);
    rb_define_global_function("test", rb_f_test, -1);

    rb_cStat = rb_define_class_under(rb_cFile, "Stat", rb_cObject);
    rb_define_alloc_func(rb_cStat,  rb_stat_s_alloc);
    rb_define_method(rb_cStat, "initialize", rb_stat_init, 1);
    rb_define_method(rb_cStat, "initialize_copy", rb_stat_init_copy, 1);

    rb_include_module(rb_cStat, rb_mComparable);

    rb_define_method(rb_cStat, "<=>", rb_stat_cmp, 1);

    rb_define_method(rb_cStat, "dev", rb_stat_dev, 0);
    rb_define_method(rb_cStat, "dev_major", rb_stat_dev_major, 0);
    rb_define_method(rb_cStat, "dev_minor", rb_stat_dev_minor, 0);
    rb_define_method(rb_cStat, "ino", rb_stat_ino, 0);
    rb_define_method(rb_cStat, "mode", rb_stat_mode, 0);
    rb_define_method(rb_cStat, "nlink", rb_stat_nlink, 0);
    rb_define_method(rb_cStat, "uid", rb_stat_uid, 0);
    rb_define_method(rb_cStat, "gid", rb_stat_gid, 0);
    rb_define_method(rb_cStat, "rdev", rb_stat_rdev, 0);
    rb_define_method(rb_cStat, "rdev_major", rb_stat_rdev_major, 0);
    rb_define_method(rb_cStat, "rdev_minor", rb_stat_rdev_minor, 0);
    rb_define_method(rb_cStat, "size", rb_stat_size, 0);
    rb_define_method(rb_cStat, "blksize", rb_stat_blksize, 0);
    rb_define_method(rb_cStat, "blocks", rb_stat_blocks, 0);
    rb_define_method(rb_cStat, "atime", rb_stat_atime, 0);
    rb_define_method(rb_cStat, "mtime", rb_stat_mtime, 0);
    rb_define_method(rb_cStat, "ctime", rb_stat_ctime, 0);

    rb_define_method(rb_cStat, "inspect", rb_stat_inspect, 0);

    rb_define_method(rb_cStat, "ftype", rb_stat_ftype, 0);

    rb_define_method(rb_cStat, "directory?",  rb_stat_d, 0);
    rb_define_method(rb_cStat, "readable?",  rb_stat_r, 0);
    rb_define_method(rb_cStat, "readable_real?",  rb_stat_R, 0);
    rb_define_method(rb_cStat, "world_readable?", rb_stat_wr, 0);
    rb_define_method(rb_cStat, "writable?",  rb_stat_w, 0);
    rb_define_method(rb_cStat, "writable_real?",  rb_stat_W, 0);
    rb_define_method(rb_cStat, "world_writable?", rb_stat_ww, 0);
    rb_define_method(rb_cStat, "executable?",  rb_stat_x, 0);
    rb_define_method(rb_cStat, "executable_real?",  rb_stat_X, 0);
    rb_define_method(rb_cStat, "file?",  rb_stat_f, 0);
    rb_define_method(rb_cStat, "zero?",  rb_stat_z, 0);
    rb_define_method(rb_cStat, "size?",  rb_stat_s, 0);
    rb_define_method(rb_cStat, "owned?",  rb_stat_owned, 0);
    rb_define_method(rb_cStat, "grpowned?",  rb_stat_grpowned, 0);

    rb_define_method(rb_cStat, "pipe?",  rb_stat_p, 0);
    rb_define_method(rb_cStat, "symlink?",  rb_stat_l, 0);
    rb_define_method(rb_cStat, "socket?",  rb_stat_S, 0);

    rb_define_method(rb_cStat, "blockdev?",  rb_stat_b, 0);
    rb_define_method(rb_cStat, "chardev?",  rb_stat_c, 0);

    rb_define_method(rb_cStat, "setuid?",  rb_stat_suid, 0);
    rb_define_method(rb_cStat, "setgid?",  rb_stat_sgid, 0);
    rb_define_method(rb_cStat, "sticky?",  rb_stat_sticky, 0);
}
