#include "ruby.h"
#include "ruby/encoding.h"

static VALUE rb_cPathname;
static ID id_at_path, id_to_path;

static VALUE
get_strpath(VALUE obj)
{
    VALUE strpath;
    strpath = rb_ivar_get(obj, id_at_path);
    if (TYPE(strpath) != T_STRING)
        rb_raise(rb_eTypeError, "unexpected @path");
    return strpath;
}

static void
set_strpath(VALUE obj, VALUE val)
{
    rb_ivar_set(obj, id_at_path, val);
}

/*
 * Create a Pathname object from the given String (or String-like object).
 * If +path+ contains a NUL character (<tt>\0</tt>), an ArgumentError is raised.
 */
static VALUE
path_initialize(VALUE self, VALUE arg)
{
    VALUE str;
    if (TYPE(arg) == T_STRING) {
        str = arg;
    }
    else {
        str = rb_check_funcall(arg, id_to_path, 0, NULL);
        if (str == Qundef)
            str = arg;
        StringValue(str);
    }
    if (memchr(RSTRING_PTR(str), '\0', RSTRING_LEN(str)))
        rb_raise(rb_eArgError, "pathname contains null byte");
    str = rb_obj_dup(str);

    set_strpath(self, str);
    OBJ_INFECT(self, str);
    return self;
}

static VALUE
path_freeze(VALUE self)
{
    rb_call_super(0, 0);
    rb_str_freeze(get_strpath(self));
    return self;
}

static VALUE
path_taint(VALUE self)
{
    rb_call_super(0, 0);
    rb_obj_taint(get_strpath(self));
    return self;
}

static VALUE
path_untaint(VALUE self)
{
    rb_call_super(0, 0);
    rb_obj_untaint(get_strpath(self));
    return self;
}

/*
 *  Compare this pathname with +other+.  The comparison is string-based.
 *  Be aware that two different paths (<tt>foo.txt</tt> and <tt>./foo.txt</tt>)
 *  can refer to the same file.
 */
static VALUE
path_eq(VALUE self, VALUE other)
{
    if (!rb_obj_is_kind_of(other, rb_cPathname))
        return Qfalse;
    return rb_str_equal(get_strpath(self), get_strpath(other));
}

/*
 *  Provides for comparing pathnames, case-sensitively.
 */
static VALUE
path_cmp(VALUE self, VALUE other)
{
    VALUE s1, s2;
    char *p1, *p2;
    char *e1, *e2;
    if (!rb_obj_is_kind_of(other, rb_cPathname))
        return Qnil;
    s1 = get_strpath(self);
    s2 = get_strpath(other);
    p1 = RSTRING_PTR(s1);
    p2 = RSTRING_PTR(s2);
    e1 = p1 + RSTRING_LEN(s1);
    e2 = p2 + RSTRING_LEN(s2);
    while (p1 < e1 && p2 < e2) {
        int c1, c2;
        c1 = (unsigned char)*p1++;
        c2 = (unsigned char)*p2++;
        if (c1 == '/') c1 = '\0';
        if (c2 == '/') c2 = '\0';
        if (c1 != c2) {
            if (c1 < c2)
                return INT2FIX(-1);
            else
                return INT2FIX(1);
        }
    }
    if (p1 < e1)
        return INT2FIX(1);
    if (p2 < e2)
        return INT2FIX(-1);
    return INT2FIX(0);
}

/* :nodoc: */
static VALUE
path_hash(VALUE self)
{
    return INT2FIX(rb_str_hash(get_strpath(self)));
}

/*
 *  call-seq:
 *    pathname.to_s             -> string
 *    pathname.to_path          -> string
 *
 *  Return the path as a String.
 *
 *  to_path is implemented so Pathname objects are usable with File.open, etc.
 */
static VALUE
path_to_s(VALUE self)
{
    return rb_obj_dup(get_strpath(self));
}

/* :nodoc: */
static VALUE
path_inspect(VALUE self)
{
    const char *c = rb_obj_classname(self);
    VALUE str = get_strpath(self);
    return rb_sprintf("#<%s:%s>", c, RSTRING_PTR(str));
}

/*
 * Return a pathname which is substituted by String#sub.
 */
static VALUE
path_sub(int argc, VALUE *argv, VALUE self)
{
    VALUE str = get_strpath(self);

    if (rb_block_given_p()) {
        str = rb_block_call(str, rb_intern("sub"), argc, argv, 0, 0);
    }
    else {
        str = rb_funcall2(str, rb_intern("sub"), argc, argv);
    }
    return rb_class_new_instance(1, &str, rb_obj_class(self));
}

/*
 * Return a pathname which the extension of the basename is substituted by
 * <i>repl</i>.
 *
 * If self has no extension part, <i>repl</i> is appended.
 */
static VALUE
path_sub_ext(VALUE self, VALUE repl)
{
    VALUE str = get_strpath(self);
    VALUE str2;
    long extlen;
    const char *ext;
    const char *p;

    StringValue(repl);
    p = RSTRING_PTR(str);
    extlen = RSTRING_LEN(str);
    ext = ruby_enc_find_extname(p, &extlen, rb_enc_get(str));
    if (ext == NULL) {
        ext = p + RSTRING_LEN(str);
    }
    else if (extlen <= 1) {
        ext += extlen;
    }
    str2 = rb_str_subseq(str, 0, ext-p);
    rb_str_append(str2, repl);
    OBJ_INFECT(str2, str);
    return rb_class_new_instance(1, &str2, rb_obj_class(self));
}

/* Facade for File */

/*
 * Returns the real (absolute) pathname of +self+ in the actual
 * filesystem not containing symlinks or useless dots.
 *
 * All components of the pathname must exist when this method is
 * called.
 *
 */
static VALUE
path_realpath(int argc, VALUE *argv, VALUE self)
{
    VALUE basedir, str;
    rb_scan_args(argc, argv, "01", &basedir);
    str = rb_funcall(rb_cFile, rb_intern("realpath"), 2, get_strpath(self), basedir);
    return rb_class_new_instance(1, &str, rb_obj_class(self));
}

/*
 * Returns the real (absolute) pathname of +self+ in the actual filesystem.
 * The real pathname doesn't contain symlinks or useless dots.
 *
 * The last component of the real pathname can be nonexistent.
 */
static VALUE
path_realdirpath(int argc, VALUE *argv, VALUE self)
{
    VALUE basedir, str;
    rb_scan_args(argc, argv, "01", &basedir);
    str = rb_funcall(rb_cFile, rb_intern("realdirpath"), 2, get_strpath(self), basedir);
    return rb_class_new_instance(1, &str, rb_obj_class(self));
}

/*
 * call-seq:
 *   pathname.each_line {|line| ... }
 *   pathname.each_line(sep=$/ [, open_args]) {|line| block }     -> nil
 *   pathname.each_line(limit [, open_args]) {|line| block }      -> nil
 *   pathname.each_line(sep, limit [, open_args]) {|line| block } -> nil
 *   pathname.each_line(...)                                      -> an_enumerator
 *
 * #each_line iterates over the line in the file.  It yields a String object
 * for each line.
 *
 * This method is availabel since 1.8.1.
 */
static VALUE
path_each_line(int argc, VALUE *argv, VALUE self)
{
    VALUE args[4];
    int n;

    args[0] = get_strpath(self);
    n = rb_scan_args(argc, argv, "03", &args[1], &args[2], &args[3]);
    if (rb_block_given_p()) {
        return rb_block_call(rb_cIO, rb_intern("foreach"), 1+n, args, 0, 0);
    }
    else {
        return rb_funcall2(rb_cIO, rb_intern("foreach"), 1+n, args);
    }
}

/*
 * call-seq:
 *   pathname.read([length [, offset]]) -> string
 *   pathname.read([length [, offset]], open_args) -> string
 *
 * See <tt>IO.read</tt>.  Returns all data from the file, or the first +N+ bytes
 * if specified.
 *
 */
static VALUE
path_read(int argc, VALUE *argv, VALUE self)
{
    VALUE args[4];
    int n;

    args[0] = get_strpath(self);
    n = rb_scan_args(argc, argv, "03", &args[1], &args[2], &args[3]);
    return rb_funcall2(rb_cIO, rb_intern("read"), 1+n, args);
}

/*
 * call-seq:
 *   pathname.binread([length [, offset]]) -> string
 *
 * See <tt>IO.binread</tt>.  Returns all the bytes from the file, or the first +N+
 * if specified.
 *
 */
static VALUE
path_binread(int argc, VALUE *argv, VALUE self)
{
    VALUE args[3];
    int n;

    args[0] = get_strpath(self);
    n = rb_scan_args(argc, argv, "02", &args[1], &args[2]);
    return rb_funcall2(rb_cIO, rb_intern("binread"), 1+n, args);
}

/*
 * call-seq:
 *   pathname.readlines(sep=$/ [, open_args])     -> array
 *   pathname.readlines(limit [, open_args])      -> array
 *   pathname.readlines(sep, limit [, open_args]) -> array
 *
 * See <tt>IO.readlines</tt>.  Returns all the lines from the file.
 *
 */
static VALUE
path_readlines(int argc, VALUE *argv, VALUE self)
{
    VALUE args[4];
    int n;

    args[0] = get_strpath(self);
    n = rb_scan_args(argc, argv, "03", &args[1], &args[2], &args[3]);
    return rb_funcall2(rb_cIO, rb_intern("readlines"), 1+n, args);
}

/*
 * call-seq:
 *   pathname.sysopen([mode, [perm]])  -> fixnum
 *
 * See <tt>IO.sysopen</tt>.
 *
 */
static VALUE
path_sysopen(int argc, VALUE *argv, VALUE self)
{
    VALUE args[3];
    int n;

    args[0] = get_strpath(self);
    n = rb_scan_args(argc, argv, "02", &args[1], &args[2]);
    return rb_funcall2(rb_cIO, rb_intern("sysopen"), 1+n, args);
}

/*
 * See <tt>File.atime</tt>.  Returns last access time.
 */
static VALUE
path_atime(VALUE self)
{
    return rb_funcall(rb_cFile, rb_intern("atime"), 1, get_strpath(self));
}

/*
 * See <tt>File.ctime</tt>.  Returns last (directory entry, not file) change time.
 */
static VALUE
path_ctime(VALUE self)
{
    return rb_funcall(rb_cFile, rb_intern("ctime"), 1, get_strpath(self));
}

/*
 * See <tt>File.mtime</tt>.  Returns last modification time.
 */
static VALUE
path_mtime(VALUE self)
{
    return rb_funcall(rb_cFile, rb_intern("mtime"), 1, get_strpath(self));
}

/*
 * See <tt>File.chmod</tt>.  Changes permissions.
 */
static VALUE
path_chmod(VALUE self, VALUE mode)
{
    return rb_funcall(rb_cFile, rb_intern("chmod"), 2, mode, get_strpath(self));
}

/*
 * See <tt>File.lchmod</tt>.
 */
static VALUE
path_lchmod(VALUE self, VALUE mode)
{
    return rb_funcall(rb_cFile, rb_intern("lchmod"), 2, mode, get_strpath(self));
}

/*
 * See <tt>File.chown</tt>.  Change owner and group of file.
 */
static VALUE
path_chown(VALUE self, VALUE owner, VALUE group)
{
    return rb_funcall(rb_cFile, rb_intern("chown"), 3, owner, group, get_strpath(self));
}

/*
 * See <tt>File.lchown</tt>.
 */
static VALUE
path_lchown(VALUE self, VALUE owner, VALUE group)
{
    return rb_funcall(rb_cFile, rb_intern("lchown"), 3, owner, group, get_strpath(self));
}

/*
 * call-seq:
 *    pathname.fnmatch(pattern, [flags])        -> string
 *    pathname.fnmatch?(pattern, [flags])       -> string
 *
 * See <tt>File.fnmatch</tt>.  Return +true+ if the receiver matches the given
 * pattern.
 */
static VALUE
path_fnmatch(int argc, VALUE *argv, VALUE self)
{
    VALUE str = get_strpath(self);
    VALUE pattern, flags;
    if (rb_scan_args(argc, argv, "11", &pattern, &flags) == 1)
        return rb_funcall(rb_cFile, rb_intern("fnmatch"), 2, pattern, str);
    else
        return rb_funcall(rb_cFile, rb_intern("fnmatch"), 3, pattern, str, flags);
}

/*
 * See <tt>File.ftype</tt>.  Returns "type" of file ("file", "directory",
 * etc).
 */
static VALUE
path_ftype(VALUE self)
{
    return rb_funcall(rb_cFile, rb_intern("ftype"), 1, get_strpath(self));
}

/*
 * call-seq:
 *   pathname.make_link(old)
 *
 * See <tt>File.link</tt>.  Creates a hard link at _pathname_.
 */
static VALUE
path_make_link(VALUE self, VALUE old)
{
    return rb_funcall(rb_cFile, rb_intern("link"), 2, old, get_strpath(self));
}

/*
 * See <tt>File.open</tt>.  Opens the file for reading or writing.
 */
static VALUE
path_open(int argc, VALUE *argv, VALUE self)
{
    VALUE args[4];
    int n;

    args[0] = get_strpath(self);
    n = rb_scan_args(argc, argv, "03", &args[1], &args[2], &args[3]);
    if (rb_block_given_p()) {
        return rb_block_call(rb_cFile, rb_intern("open"), 1+n, args, 0, 0);
    }
    else {
        return rb_funcall2(rb_cFile, rb_intern("open"), 1+n, args);
    }
}

/*
 * See <tt>File.readlink</tt>.  Read symbolic link.
 */
static VALUE
path_readlink(VALUE self)
{
    VALUE str;
    str = rb_funcall(rb_cFile, rb_intern("readlink"), 1, get_strpath(self));
    return rb_class_new_instance(1, &str, rb_obj_class(self));
}

/*
 * See <tt>File.rename</tt>.  Rename the file.
 */
static VALUE
path_rename(VALUE self, VALUE to)
{
    return rb_funcall(rb_cFile, rb_intern("rename"), 2, get_strpath(self), to);
}

/*
 * See <tt>File.stat</tt>.  Returns a <tt>File::Stat</tt> object.
 */
static VALUE
path_stat(VALUE self)
{
    return rb_funcall(rb_cFile, rb_intern("stat"), 1, get_strpath(self));
}

/*
 * See <tt>File.lstat</tt>.
 */
static VALUE
path_lstat(VALUE self)
{
    return rb_funcall(rb_cFile, rb_intern("lstat"), 1, get_strpath(self));
}

/*
 * call-seq:
 *   pathname.make_symlink(old)
 *
 * See <tt>File.symlink</tt>.  Creates a symbolic link.
 */
static VALUE
path_make_symlink(VALUE self, VALUE old)
{
    return rb_funcall(rb_cFile, rb_intern("symlink"), 2, old, get_strpath(self));
}

/*
 * See <tt>File.truncate</tt>.  Truncate the file to +length+ bytes.
 */
static VALUE
path_truncate(VALUE self, VALUE length)
{
    return rb_funcall(rb_cFile, rb_intern("truncate"), 2, get_strpath(self), length);
}

/*
 * See <tt>File.utime</tt>.  Update the access and modification times.
 */
static VALUE
path_utime(VALUE self, VALUE atime, VALUE mtime)
{
    return rb_funcall(rb_cFile, rb_intern("utime"), 3, atime, mtime, get_strpath(self));
}

/*
 * See <tt>File.basename</tt>.  Returns the last component of the path.
 */
static VALUE
path_basename(int argc, VALUE *argv, VALUE self)
{
    VALUE str = get_strpath(self);
    VALUE fext;
    if (rb_scan_args(argc, argv, "01", &fext) == 0)
        str = rb_funcall(rb_cFile, rb_intern("basename"), 1, str);
    else
        str = rb_funcall(rb_cFile, rb_intern("basename"), 2, str, fext);
    return rb_class_new_instance(1, &str, rb_obj_class(self));
}

/*
 * See <tt>File.dirname</tt>.  Returns all but the last component of the path.
 */
static VALUE
path_dirname(VALUE self)
{
    VALUE str = get_strpath(self);
    str = rb_funcall(rb_cFile, rb_intern("dirname"), 1, str);
    return rb_class_new_instance(1, &str, rb_obj_class(self));
}

/*
 * See <tt>File.extname</tt>.  Returns the file's extension.
 */
static VALUE
path_extname(VALUE self)
{
    VALUE str = get_strpath(self);
    return rb_funcall(rb_cFile, rb_intern("extname"), 1, str);
}

/*
 * See <tt>File.expand_path</tt>.
 */
static VALUE
path_expand_path(int argc, VALUE *argv, VALUE self)
{
    VALUE str = get_strpath(self);
    VALUE dname;
    if (rb_scan_args(argc, argv, "01", &dname) == 0)
        str = rb_funcall(rb_cFile, rb_intern("expand_path"), 1, str);
    else
        str = rb_funcall(rb_cFile, rb_intern("expand_path"), 2, str, dname);
    return rb_class_new_instance(1, &str, rb_obj_class(self));
}

/*
 * See <tt>File.split</tt>.  Returns the #dirname and the #basename in an Array.
 */
static VALUE
path_split(VALUE self)
{
    VALUE str = get_strpath(self);
    VALUE ary, dirname, basename;
    ary = rb_funcall(rb_cFile, rb_intern("split"), 1, str);
    ary = rb_check_array_type(ary);
    dirname = rb_ary_entry(ary, 0);
    basename = rb_ary_entry(ary, 1);
    dirname = rb_class_new_instance(1, &dirname, rb_obj_class(self));
    basename = rb_class_new_instance(1, &basename, rb_obj_class(self));
    return rb_ary_new3(2, dirname, basename);
}

/*
 * See <tt>FileTest.blockdev?</tt>.
 */
static VALUE
path_blockdev_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("blockdev?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.chardev?</tt>.
 */
static VALUE
path_chardev_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("chardev?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.executable?</tt>.
 */
static VALUE
path_executable_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("executable?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.executable_real?</tt>.
 */
static VALUE
path_executable_real_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("executable_real?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.exist?</tt>.
 */
static VALUE
path_exist_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("exist?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.grpowned?</tt>.
 */
static VALUE
path_grpowned_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("grpowned?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.directory?</tt>.
 */
static VALUE
path_directory_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("directory?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.file?</tt>.
 */
static VALUE
path_file_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("file?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.pipe?</tt>.
 */
static VALUE
path_pipe_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("pipe?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.socket?</tt>.
 */
static VALUE
path_socket_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("socket?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.owned?</tt>.
 */
static VALUE
path_owned_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("owned?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.readable?</tt>.
 */
static VALUE
path_readable_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("readable?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.world_readable?</tt>.
 */
static VALUE
path_world_readable_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("world_readable?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.readable_real?</tt>.
 */
static VALUE
path_readable_real_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("readable_real?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.setuid?</tt>.
 */
static VALUE
path_setuid_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("setuid?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.setgid?</tt>.
 */
static VALUE
path_setgid_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("setgid?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.size</tt>.
 */
static VALUE
path_size(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("size"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.size?</tt>.
 */
static VALUE
path_size_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("size?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.sticky?</tt>.
 */
static VALUE
path_sticky_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("sticky?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.symlink?</tt>.
 */
static VALUE
path_symlink_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("symlink?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.writable?</tt>.
 */
static VALUE
path_writable_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("writable?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.world_writable?</tt>.
 */
static VALUE
path_world_writable_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("world_writable?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.writable_real?</tt>.
 */
static VALUE
path_writable_real_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("writable_real?"), 1, get_strpath(self));
}

/*
 * See <tt>FileTest.zero?</tt>.
 */
static VALUE
path_zero_p(VALUE self)
{
    return rb_funcall(rb_mFileTest, rb_intern("zero?"), 1, get_strpath(self));
}

static VALUE
glob_i(VALUE elt, VALUE klass, int argc, VALUE *argv)
{
    return rb_yield(rb_class_new_instance(1, &elt, klass));
}

/*
 * See <tt>Dir.glob</tt>.  Returns or yields Pathname objects.
 */
static VALUE
path_s_glob(int argc, VALUE *argv, VALUE klass)
{
    VALUE args[2];
    int n;

    n = rb_scan_args(argc, argv, "11", &args[0], &args[1]);
    if (rb_block_given_p()) {
        return rb_block_call(rb_cDir, rb_intern("glob"), n, args, glob_i, klass);
    }
    else {
        VALUE ary;
        long i;
        ary = rb_funcall2(rb_cDir, rb_intern("glob"), n, args);
        ary = rb_convert_type(ary, T_ARRAY, "Array", "to_ary");
        for (i = 0; i < RARRAY_LEN(ary); i++) {
            VALUE elt = RARRAY_PTR(ary)[i];
            elt = rb_class_new_instance(1, &elt, klass);
            rb_ary_store(ary, i, elt);
        }
        return ary;
    }
}

/*
 * See <tt>Dir.getwd</tt>.  Returns the current working directory as a Pathname.
 */
static VALUE
path_s_getwd(VALUE klass)
{
    VALUE str;
    str = rb_funcall(rb_cDir, rb_intern("getwd"), 0);
    return rb_class_new_instance(1, &str, klass);
}

/*
 * Return the entries (files and subdirectories) in the directory, each as a
 * Pathname object.
 *
 * The result contains just a filename which has no directory.
 *
 * The result may contain the current directory #<Pathname:.> and the parent
 * directory #<Pathname:..>.
 *
 * If you don't want #<Pathname:.> and #<Pathname:..> and
 * want directory, consider Pathname#children.
 *
 *   pp Pathname.new('/usr/local').entries
 *   #=> [#<Pathname:share>,
 *   #    #<Pathname:lib>,
 *   #    #<Pathname:..>,
 *   #    #<Pathname:include>,
 *   #    #<Pathname:etc>,
 *   #    #<Pathname:bin>,
 *   #    #<Pathname:man>,
 *   #    #<Pathname:games>,
 *   #    #<Pathname:.>,
 *   #    #<Pathname:sbin>,
 *   #    #<Pathname:src>]
 *
 */
static VALUE
path_entries(VALUE self)
{
    VALUE klass, str, ary;
    long i;
    klass = rb_obj_class(self);
    str = get_strpath(self);
    ary = rb_funcall(rb_cDir, rb_intern("entries"), 1, str);
    ary = rb_convert_type(ary, T_ARRAY, "Array", "to_ary");
    for (i = 0; i < RARRAY_LEN(ary); i++) {
        VALUE elt = RARRAY_PTR(ary)[i];
        elt = rb_class_new_instance(1, &elt, klass);
        rb_ary_store(ary, i, elt);
    }
    return ary;
}

/*
 * See <tt>Dir.mkdir</tt>.  Create the referenced directory.
 */
static VALUE
path_mkdir(int argc, VALUE *argv, VALUE self)
{
    VALUE str = get_strpath(self);
    VALUE vmode;
    if (rb_scan_args(argc, argv, "01", &vmode) == 0)
        return rb_funcall(rb_cDir, rb_intern("mkdir"), 1, str);
    else
        return rb_funcall(rb_cDir, rb_intern("mkdir"), 2, str, vmode);
}

/*
 * See <tt>Dir.rmdir</tt>.  Remove the referenced directory.
 */
static VALUE
path_rmdir(VALUE self)
{
    return rb_funcall(rb_cDir, rb_intern("rmdir"), 1, get_strpath(self));
}

/*
 * See <tt>Dir.open</tt>.
 */
static VALUE
path_opendir(VALUE self)
{
    VALUE args[1];

    args[0] = get_strpath(self);
    return rb_block_call(rb_cDir, rb_intern("open"), 1, args, 0, 0);
}

static VALUE
each_entry_i(VALUE elt, VALUE klass, int argc, VALUE *argv)
{
    return rb_yield(rb_class_new_instance(1, &elt, klass));
}

/*
 * Iterates over the entries (files and subdirectories) in the directory.  It
 * yields a Pathname object for each entry.
 *
 * This method has available since 1.8.1.
 */
static VALUE
path_each_entry(VALUE self)
{
    VALUE args[1];

    args[0] = get_strpath(self);
    return rb_block_call(rb_cDir, rb_intern("foreach"), 1, args, each_entry_i, rb_obj_class(self));
}

static VALUE
unlink_body(VALUE str)
{
    return rb_funcall(rb_cDir, rb_intern("unlink"), 1, str);
}

static VALUE
unlink_rescue(VALUE str, VALUE errinfo)
{
    return rb_funcall(rb_cFile, rb_intern("unlink"), 1, str);
}

/*
 * Removes a file or directory, using <tt>File.unlink</tt> or
 * <tt>Dir.unlink</tt> as necessary.
 */
static VALUE
path_unlink(VALUE self)
{
    VALUE eENOTDIR = rb_const_get_at(rb_mErrno, rb_intern("ENOTDIR"));
    VALUE str = get_strpath(self);
    return rb_rescue2(unlink_body, str, unlink_rescue, str, eENOTDIR, (VALUE)0);
}

/*
 * create a pathname object.
 *
 * This method is available since 1.8.5.
 */
static VALUE
path_f_pathname(VALUE self, VALUE str)
{
    return rb_class_new_instance(1, &str, rb_cPathname);
}

/*
 * == Pathname
 *
 * Pathname represents a pathname which locates a file in a filesystem.
 * The pathname depends on OS: Unix, Windows, etc.
 * Pathname library works with pathnames of local OS.
 * However non-Unix pathnames are supported experimentally.
 *
 * It does not represent the file itself.
 * A Pathname can be relative or absolute.  It's not until you try to
 * reference the file that it even matters whether the file exists or not.
 *
 * Pathname is immutable.  It has no method for destructive update.
 *
 * The value of this class is to manipulate file path information in a neater
 * way than standard Ruby provides.  The examples below demonstrate the
 * difference.  *All* functionality from File, FileTest, and some from Dir and
 * FileUtils is included, in an unsurprising way.  It is essentially a facade for
 * all of these, and more.
 *
 * == Examples
 *
 * === Example 1: Using Pathname
 *
 *   require 'pathname'
 *   pn = Pathname.new("/usr/bin/ruby")
 *   size = pn.size              # 27662
 *   isdir = pn.directory?       # false
 *   dir  = pn.dirname           # Pathname:/usr/bin
 *   base = pn.basename          # Pathname:ruby
 *   dir, base = pn.split        # [Pathname:/usr/bin, Pathname:ruby]
 *   data = pn.read
 *   pn.open { |f| _ }
 *   pn.each_line { |line| _ }
 *
 * === Example 2: Using standard Ruby
 *
 *   pn = "/usr/bin/ruby"
 *   size = File.size(pn)        # 27662
 *   isdir = File.directory?(pn) # false
 *   dir  = File.dirname(pn)     # "/usr/bin"
 *   base = File.basename(pn)    # "ruby"
 *   dir, base = File.split(pn)  # ["/usr/bin", "ruby"]
 *   data = File.read(pn)
 *   File.open(pn) { |f| _ }
 *   File.foreach(pn) { |line| _ }
 *
 * === Example 3: Special features
 *
 *   p1 = Pathname.new("/usr/lib")   # Pathname:/usr/lib
 *   p2 = p1 + "ruby/1.8"            # Pathname:/usr/lib/ruby/1.8
 *   p3 = p1.parent                  # Pathname:/usr
 *   p4 = p2.relative_path_from(p3)  # Pathname:lib/ruby/1.8
 *   pwd = Pathname.pwd              # Pathname:/home/gavin
 *   pwd.absolute?                   # true
 *   p5 = Pathname.new "."           # Pathname:.
 *   p5 = p5 + "music/../articles"   # Pathname:music/../articles
 *   p5.cleanpath                    # Pathname:articles
 *   p5.realpath                     # Pathname:/home/gavin/articles
 *   p5.children                     # [Pathname:/home/gavin/articles/linux, ...]
 *
 * == Breakdown of functionality
 *
 * === Core methods
 *
 * These methods are effectively manipulating a String, because that's
 * all a path is.  Except for #mountpoint?, #children, #each_child,
 * #realdirpath and #realpath, they don't access the filesystem.
 *
 * - +
 * - #join
 * - #parent
 * - #root?
 * - #absolute?
 * - #relative?
 * - #relative_path_from
 * - #each_filename
 * - #cleanpath
 * - #realpath
 * - #realdirpath
 * - #children
 * - #each_child
 * - #mountpoint?
 *
 * === File status predicate methods
 *
 * These methods are a facade for FileTest:
 * - #blockdev?
 * - #chardev?
 * - #directory?
 * - #executable?
 * - #executable_real?
 * - #exist?
 * - #file?
 * - #grpowned?
 * - #owned?
 * - #pipe?
 * - #readable?
 * - #world_readable?
 * - #readable_real?
 * - #setgid?
 * - #setuid?
 * - #size
 * - #size?
 * - #socket?
 * - #sticky?
 * - #symlink?
 * - #writable?
 * - #world_writable?
 * - #writable_real?
 * - #zero?
 *
 * === File property and manipulation methods
 *
 * These methods are a facade for File:
 * - #atime
 * - #ctime
 * - #mtime
 * - #chmod(mode)
 * - #lchmod(mode)
 * - #chown(owner, group)
 * - #lchown(owner, group)
 * - #fnmatch(pattern, *args)
 * - #fnmatch?(pattern, *args)
 * - #ftype
 * - #make_link(old)
 * - #open(*args, &block)
 * - #readlink
 * - #rename(to)
 * - #stat
 * - #lstat
 * - #make_symlink(old)
 * - #truncate(length)
 * - #utime(atime, mtime)
 * - #basename(*args)
 * - #dirname
 * - #extname
 * - #expand_path(*args)
 * - #split
 *
 * === Directory methods
 *
 * These methods are a facade for Dir:
 * - Pathname.glob(*args)
 * - Pathname.getwd / Pathname.pwd
 * - #rmdir
 * - #entries
 * - #each_entry(&block)
 * - #mkdir(*args)
 * - #opendir(*args)
 *
 * === IO
 *
 * These methods are a facade for IO:
 * - #each_line(*args, &block)
 * - #read(*args)
 * - #binread(*args)
 * - #readlines(*args)
 * - #sysopen(*args)
 *
 * === Utilities
 *
 * These methods are a mixture of Find, FileUtils, and others:
 * - #find(&block)
 * - #mkpath
 * - #rmtree
 * - #unlink / #delete
 *
 *
 * == Method documentation
 *
 * As the above section shows, most of the methods in Pathname are facades.  The
 * documentation for these methods generally just says, for instance, "See
 * FileTest.writable?", as you should be familiar with the original method
 * anyway, and its documentation (e.g. through +ri+) will contain more
 * information.  In some cases, a brief description will follow.
 */
void
Init_pathname()
{
    id_at_path = rb_intern("@path");
    id_to_path = rb_intern("to_path");

    rb_cPathname = rb_define_class("Pathname", rb_cObject);
    rb_define_method(rb_cPathname, "initialize", path_initialize, 1);
    rb_define_method(rb_cPathname, "freeze", path_freeze, 0);
    rb_define_method(rb_cPathname, "taint", path_taint, 0);
    rb_define_method(rb_cPathname, "untaint", path_untaint, 0);
    rb_define_method(rb_cPathname, "==", path_eq, 1);
    rb_define_method(rb_cPathname, "===", path_eq, 1);
    rb_define_method(rb_cPathname, "eql?", path_eq, 1);
    rb_define_method(rb_cPathname, "<=>", path_cmp, 1);
    rb_define_method(rb_cPathname, "hash", path_hash, 0);
    rb_define_method(rb_cPathname, "to_s", path_to_s, 0);
    rb_define_method(rb_cPathname, "to_path", path_to_s, 0);
    rb_define_method(rb_cPathname, "inspect", path_inspect, 0);
    rb_define_method(rb_cPathname, "sub", path_sub, -1);
    rb_define_method(rb_cPathname, "sub_ext", path_sub_ext, 1);
    rb_define_method(rb_cPathname, "realpath", path_realpath, -1);
    rb_define_method(rb_cPathname, "realdirpath", path_realdirpath, -1);
    rb_define_method(rb_cPathname, "each_line", path_each_line, -1);
    rb_define_method(rb_cPathname, "read", path_read, -1);
    rb_define_method(rb_cPathname, "binread", path_binread, -1);
    rb_define_method(rb_cPathname, "readlines", path_readlines, -1);
    rb_define_method(rb_cPathname, "sysopen", path_sysopen, -1);
    rb_define_method(rb_cPathname, "atime", path_atime, 0);
    rb_define_method(rb_cPathname, "ctime", path_ctime, 0);
    rb_define_method(rb_cPathname, "mtime", path_mtime, 0);
    rb_define_method(rb_cPathname, "chmod", path_chmod, 1);
    rb_define_method(rb_cPathname, "lchmod", path_lchmod, 1);
    rb_define_method(rb_cPathname, "chown", path_chown, 2);
    rb_define_method(rb_cPathname, "lchown", path_lchown, 2);
    rb_define_method(rb_cPathname, "fnmatch", path_fnmatch, -1);
    rb_define_method(rb_cPathname, "fnmatch?", path_fnmatch, -1);
    rb_define_method(rb_cPathname, "ftype", path_ftype, 0);
    rb_define_method(rb_cPathname, "make_link", path_make_link, 1);
    rb_define_method(rb_cPathname, "open", path_open, -1);
    rb_define_method(rb_cPathname, "readlink", path_readlink, 0);
    rb_define_method(rb_cPathname, "rename", path_rename, 1);
    rb_define_method(rb_cPathname, "stat", path_stat, 0);
    rb_define_method(rb_cPathname, "lstat", path_lstat, 0);
    rb_define_method(rb_cPathname, "make_symlink", path_make_symlink, 1);
    rb_define_method(rb_cPathname, "truncate", path_truncate, 1);
    rb_define_method(rb_cPathname, "utime", path_utime, 2);
    rb_define_method(rb_cPathname, "basename", path_basename, -1);
    rb_define_method(rb_cPathname, "dirname", path_dirname, 0);
    rb_define_method(rb_cPathname, "extname", path_extname, 0);
    rb_define_method(rb_cPathname, "expand_path", path_expand_path, -1);
    rb_define_method(rb_cPathname, "split", path_split, 0);
    rb_define_method(rb_cPathname, "blockdev?", path_blockdev_p, 0);
    rb_define_method(rb_cPathname, "chardev?", path_chardev_p, 0);
    rb_define_method(rb_cPathname, "executable?", path_executable_p, 0);
    rb_define_method(rb_cPathname, "executable_real?", path_executable_real_p, 0);
    rb_define_method(rb_cPathname, "exist?", path_exist_p, 0);
    rb_define_method(rb_cPathname, "grpowned?", path_grpowned_p, 0);
    rb_define_method(rb_cPathname, "directory?", path_directory_p, 0);
    rb_define_method(rb_cPathname, "file?", path_file_p, 0);
    rb_define_method(rb_cPathname, "pipe?", path_pipe_p, 0);
    rb_define_method(rb_cPathname, "socket?", path_socket_p, 0);
    rb_define_method(rb_cPathname, "owned?", path_owned_p, 0);
    rb_define_method(rb_cPathname, "readable?", path_readable_p, 0);
    rb_define_method(rb_cPathname, "world_readable?", path_world_readable_p, 0);
    rb_define_method(rb_cPathname, "readable_real?", path_readable_real_p, 0);
    rb_define_method(rb_cPathname, "setuid?", path_setuid_p, 0);
    rb_define_method(rb_cPathname, "setgid?", path_setgid_p, 0);
    rb_define_method(rb_cPathname, "size", path_size, 0);
    rb_define_method(rb_cPathname, "size?", path_size_p, 0);
    rb_define_method(rb_cPathname, "sticky?", path_sticky_p, 0);
    rb_define_method(rb_cPathname, "symlink?", path_symlink_p, 0);
    rb_define_method(rb_cPathname, "writable?", path_writable_p, 0);
    rb_define_method(rb_cPathname, "world_writable?", path_world_writable_p, 0);
    rb_define_method(rb_cPathname, "writable_real?", path_writable_real_p, 0);
    rb_define_method(rb_cPathname, "zero?", path_zero_p, 0);
    rb_define_singleton_method(rb_cPathname, "glob", path_s_glob, -1);
    rb_define_singleton_method(rb_cPathname, "getwd", path_s_getwd, 0);
    rb_define_singleton_method(rb_cPathname, "pwd", path_s_getwd, 0);
    rb_define_method(rb_cPathname, "entries", path_entries, 0);
    rb_define_method(rb_cPathname, "mkdir", path_mkdir, -1);
    rb_define_method(rb_cPathname, "rmdir", path_rmdir, 0);
    rb_define_method(rb_cPathname, "opendir", path_opendir, 0);
    rb_define_method(rb_cPathname, "each_entry", path_each_entry, 0);
    rb_define_method(rb_cPathname, "unlink", path_unlink, 0);
    rb_define_method(rb_cPathname, "delete", path_unlink, 0);
    rb_undef_method(rb_cPathname, "=~");
    rb_define_global_function("Pathname", path_f_pathname, 1);
}
