/************************************************

  gdbm.c -

  $Author$
  modified at: Mon Jan 24 15:59:52 JST 1994

  Documentation by Peter Adolphs < futzilogik at users dot sourceforge dot net >

************************************************/

#include "ruby.h"

#include <gdbm.h>
#include <fcntl.h>
#include <errno.h>

/*
 * Document-class: GDBM
 *
 * == Summary
 *
 * Ruby extension for GNU dbm (gdbm) -- a simple database engine for storing
 * key-value pairs on disk.
 *
 * == Description
 *
 * GNU dbm is a library for simple databases. A database is a file that stores
 * key-value pairs. Gdbm allows the user to store, retrieve, and delete data by
 * key. It furthermore allows a non-sorted traversal of all key-value pairs.
 * A gdbm database thus provides the same functionality as a hash. As
 * with objects of the Hash class, elements can be accessed with <tt>[]</tt>.
 * Furthermore, GDBM mixes in the Enumerable module, thus providing convenient
 * methods such as #find, #collect, #map, etc.
 *
 * A process is allowed to open several different databases at the same time.
 * A process can open a database as a "reader" or a "writer". Whereas a reader
 * has only read-access to the database, a writer has read- and write-access.
 * A database can be accessed either by any number of readers or by exactly one
 * writer at the same time.
 *
 * == Examples
 *
 * 1. Opening/creating a database, and filling it with some entries:
 *
 *      require 'gdbm'
 *
 *      gdbm = GDBM.new("fruitstore.db")
 *      gdbm["ananas"]    = "3"
 *      gdbm["banana"]    = "8"
 *      gdbm["cranberry"] = "4909"
 *      gdbm.close
 *
 * 2. Reading out a database:
 *
 *      require 'gdbm'
 *
 *      gdbm = GDBM.new("fruitstore.db")
 *      gdbm.each_pair do |key, value|
 *        print "#{key}: #{value}\n"
 *      end
 *      gdbm.close
 *
 *    produces
 *
 *      banana: 8
 *      ananas: 3
 *      cranberry: 4909
 *
 * == Links
 *
 * * http://www.gnu.org/software/gdbm/
 */
static VALUE rb_cGDBM, rb_eGDBMError, rb_eGDBMFatalError;

#if SIZEOF_LONG > SIZEOF_INT
#define TOO_LONG(n) ((long)(+(int)(n)) != (long)(n))
#else
#define TOO_LONG(n) 0
#endif

#define RUBY_GDBM_RW_BIT 0x20000000

#define MY_BLOCK_SIZE (2048)
#define MY_FATAL_FUNC rb_gdbm_fatal
static void
rb_gdbm_fatal(const char *msg)
{
    rb_raise(rb_eGDBMFatalError, "%s", msg);
}

struct dbmdata {
    int  di_size;
    GDBM_FILE di_dbm;
};

static void
closed_dbm(void)
{
    rb_raise(rb_eRuntimeError, "closed GDBM file");
}

#define GetDBM(obj, dbmp) do {\
    Data_Get_Struct((obj), struct dbmdata, (dbmp));\
    if ((dbmp) == 0) closed_dbm();\
    if ((dbmp)->di_dbm == 0) closed_dbm();\
} while (0)

#define GetDBM2(obj, data, dbm) {\
    GetDBM((obj), (data));\
    (dbm) = dbmp->di_dbm;\
}

static void
free_dbm(struct dbmdata *dbmp)
{
    if (dbmp) {
        if (dbmp->di_dbm) gdbm_close(dbmp->di_dbm);
        xfree(dbmp);
    }
}

/*
 * call-seq:
 *     gdbm.close -> nil
 *
 * Closes the associated database file.
 */
static VALUE
fgdbm_close(VALUE obj)
{
    struct dbmdata *dbmp;

    GetDBM(obj, dbmp);
    gdbm_close(dbmp->di_dbm);
    dbmp->di_dbm = 0;

    return Qnil;
}

/*
 * call-seq:
 *     gdbm.closed?  -> true or false
 *
 * Returns true if the associated database file has been closed.
 */
static VALUE
fgdbm_closed(VALUE obj)
{
    struct dbmdata *dbmp;

    Data_Get_Struct(obj, struct dbmdata, dbmp);
    if (dbmp == 0)
        return Qtrue;
    if (dbmp->di_dbm == 0)
        return Qtrue;

    return Qfalse;
}

static VALUE
fgdbm_s_alloc(VALUE klass)
{
    return Data_Wrap_Struct(klass, 0, free_dbm, 0);
}

/*
 * call-seq:
 *      GDBM.new(filename, mode = 0666, flags = nil)
 *
 * Creates a new GDBM instance by opening a gdbm file named _filename_.
 * If the file does not exist, a new file with file mode _mode_ will be
 * created. _flags_ may be one of the following:
 * * *READER*  - open as a reader
 * * *WRITER*  - open as a writer
 * * *WRCREAT* - open as a writer; if the database does not exist, create a new one
 * * *NEWDB*   - open as a writer; overwrite any existing databases
 *
 * The values *WRITER*, *WRCREAT* and *NEWDB* may be combined with the following
 * values by bitwise or:
 * * *SYNC*    - cause all database operations to be synchronized to the disk
 * * *NOLOCK*  - do not lock the database file
 *
 * If no _flags_ are specified, the GDBM object will try to open the database
 * file as a writer and will create it if it does not already exist
 * (cf. flag <tt>WRCREAT</tt>). If this fails (for instance, if another process
 * has already opened the database as a reader), it will try to open the
 * database file as a reader (cf. flag <tt>READER</tt>).
 */
static VALUE
fgdbm_initialize(int argc, VALUE *argv, VALUE obj)
{
    VALUE file, vmode, vflags;
    GDBM_FILE dbm;
    struct dbmdata *dbmp;
    int mode, flags = 0;

    if (rb_scan_args(argc, argv, "12", &file, &vmode, &vflags) == 1) {
        mode = 0666;            /* default value */
    }
    else if (NIL_P(vmode)) {
        mode = -1;              /* return nil if DB does not exist */
    }
    else {
        mode = NUM2INT(vmode);
    }

    if (!NIL_P(vflags))
        flags = NUM2INT(vflags);

    SafeStringValue(file);

#ifdef GDBM_CLOEXEC
    /* GDBM_CLOEXEC is available since gdbm 1.10. */
    flags |= GDBM_CLOEXEC;
#endif

    if (flags & RUBY_GDBM_RW_BIT) {
        flags &= ~RUBY_GDBM_RW_BIT;
        dbm = gdbm_open(RSTRING_PTR(file), MY_BLOCK_SIZE,
                        flags, mode, MY_FATAL_FUNC);
    }
    else {
        dbm = 0;
        if (mode >= 0)
            dbm = gdbm_open(RSTRING_PTR(file), MY_BLOCK_SIZE,
                            GDBM_WRCREAT|flags, mode, MY_FATAL_FUNC);
        if (!dbm)
            dbm = gdbm_open(RSTRING_PTR(file), MY_BLOCK_SIZE,
                            GDBM_WRITER|flags, 0, MY_FATAL_FUNC);
        if (!dbm)
            dbm = gdbm_open(RSTRING_PTR(file), MY_BLOCK_SIZE,
                            GDBM_READER|flags, 0, MY_FATAL_FUNC);
    }

    if (dbm) {
        rb_fd_fix_cloexec(gdbm_fdesc(dbm));
    }

    if (!dbm) {
        if (mode == -1) return Qnil;

        if (gdbm_errno == GDBM_FILE_OPEN_ERROR ||
            gdbm_errno == GDBM_CANT_BE_READER ||
            gdbm_errno == GDBM_CANT_BE_WRITER)
            rb_sys_fail_str(file);
        else
            rb_raise(rb_eGDBMError, "%s", gdbm_strerror(gdbm_errno));
    }

    dbmp = ALLOC(struct dbmdata);
    free_dbm(DATA_PTR(obj));
    DATA_PTR(obj) = dbmp;
    dbmp->di_dbm = dbm;
    dbmp->di_size = -1;

    return obj;
}

/*
 * call-seq:
 *      GDBM.open(filename, mode = 0666, flags = nil)
 *      GDBM.open(filename, mode = 0666, flags = nil) { |gdbm| ... }
 *
 * If called without a block, this is synonymous to GDBM::new.
 * If a block is given, the new GDBM instance will be passed to the block
 * as a parameter, and the corresponding database file will be closed
 * after the execution of the block code has been finished.
 *
 * Example for an open call with a block:
 *
 *   require 'gdbm'
 *   GDBM.open("fruitstore.db") do |gdbm|
 *     gdbm.each_pair do |key, value|
 *       print "#{key}: #{value}\n"
 *     end
 *   end
 */
static VALUE
fgdbm_s_open(int argc, VALUE *argv, VALUE klass)
{
    VALUE obj = Data_Wrap_Struct(klass, 0, free_dbm, 0);

    if (NIL_P(fgdbm_initialize(argc, argv, obj))) {
        return Qnil;
    }

    if (rb_block_given_p()) {
        return rb_ensure(rb_yield, obj, fgdbm_close, obj);
    }

    return obj;
}

static VALUE
rb_gdbm_fetch(GDBM_FILE dbm, datum key)
{
    datum val;
    VALUE str;

    val = gdbm_fetch(dbm, key);
    if (val.dptr == 0)
        return Qnil;

    str = rb_str_new(val.dptr, val.dsize);
    free(val.dptr);
    OBJ_TAINT(str);
    return str;
}

static VALUE
rb_gdbm_fetch2(GDBM_FILE dbm, VALUE keystr)
{
    datum key;
    long len;

    StringValue(keystr);
    len = RSTRING_LEN(keystr);
    if (TOO_LONG(len)) return Qnil;
    key.dptr = RSTRING_PTR(keystr);
    key.dsize = (int)len;

    return rb_gdbm_fetch(dbm, key);
}

static VALUE
rb_gdbm_fetch3(VALUE obj, VALUE keystr)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;

    GetDBM2(obj, dbmp, dbm);
    return rb_gdbm_fetch2(dbm, keystr);
}

static VALUE
rb_gdbm_firstkey(GDBM_FILE dbm)
{
    datum key;
    VALUE str;

    key = gdbm_firstkey(dbm);
    if (key.dptr == 0)
        return Qnil;

    str = rb_str_new(key.dptr, key.dsize);
    free(key.dptr);
    OBJ_TAINT(str);
    return str;
}

static VALUE
rb_gdbm_nextkey(GDBM_FILE dbm, VALUE keystr)
{
    datum key, key2;
    VALUE str;
    long len;

    len = RSTRING_LEN(keystr);
    if (TOO_LONG(len)) return Qnil;
    key.dptr = RSTRING_PTR(keystr);
    key.dsize = (int)len;
    key2 = gdbm_nextkey(dbm, key);
    if (key2.dptr == 0)
        return Qnil;

    str = rb_str_new(key2.dptr, key2.dsize);
    free(key2.dptr);
    OBJ_TAINT(str);
    return str;
}

static VALUE
fgdbm_fetch(VALUE obj, VALUE keystr, VALUE ifnone)
{
    VALUE valstr;

    valstr = rb_gdbm_fetch3(obj, keystr);
    if (NIL_P(valstr)) {
        if (ifnone == Qnil && rb_block_given_p())
            return rb_yield(keystr);
        return ifnone;
    }
    return valstr;
}

/*
 * call-seq:
 *      gdbm[key] -> value
 *
 * Retrieves the _value_ corresponding to _key_.
 */
static VALUE
fgdbm_aref(VALUE obj, VALUE keystr)
{
    return rb_gdbm_fetch3(obj, keystr);
}

/*
 * call-seq:
 *      gdbm.fetch(key [, default]) -> value
 *
 * Retrieves the _value_ corresponding to _key_. If there is no value
 * associated with _key_, _default_ will be returned instead.
 */
static VALUE
fgdbm_fetch_m(int argc, VALUE *argv, VALUE obj)
{
    VALUE keystr, valstr, ifnone;

    rb_scan_args(argc, argv, "11", &keystr, &ifnone);
    valstr = fgdbm_fetch(obj, keystr, ifnone);
    if (argc == 1 && !rb_block_given_p() && NIL_P(valstr))
        rb_raise(rb_eIndexError, "key not found");

    return valstr;
}

/*
 * call-seq:
 *      gdbm.key(value) -> key
 *
 * Returns the _key_ for a given _value_. If several keys may map to the
 * same value, the key that is found first will be returned.
 */
static VALUE
fgdbm_key(VALUE obj, VALUE valstr)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    VALUE keystr, valstr2;

    StringValue(valstr);
    GetDBM2(obj, dbmp, dbm);
    for (keystr = rb_gdbm_firstkey(dbm); RTEST(keystr);
         keystr = rb_gdbm_nextkey(dbm, keystr)) {

        valstr2 = rb_gdbm_fetch2(dbm, keystr);
        if (!NIL_P(valstr2) &&
            (int)RSTRING_LEN(valstr) == (int)RSTRING_LEN(valstr2) &&
            memcmp(RSTRING_PTR(valstr), RSTRING_PTR(valstr2),
                   (int)RSTRING_LEN(valstr)) == 0) {
            return keystr;
        }
    }
    return Qnil;
}

/* :nodoc: */
static VALUE
fgdbm_index(VALUE obj, VALUE value)
{
    rb_warn("GDBM#index is deprecated; use GDBM#key");
    return fgdbm_key(obj, value);
}

/*
 * call-seq:
 *      gdbm.select { |key, value| block } -> array
 *
 * Returns a new array of all key-value pairs of the database for which _block_
 * evaluates to true.
 */
static VALUE
fgdbm_select(VALUE obj)
{
    VALUE new = rb_ary_new();
    GDBM_FILE dbm;
    struct dbmdata *dbmp;
    VALUE keystr;

    GetDBM2(obj, dbmp, dbm);
    for (keystr = rb_gdbm_firstkey(dbm); RTEST(keystr);
         keystr = rb_gdbm_nextkey(dbm, keystr)) {
        VALUE assoc = rb_assoc_new(keystr, rb_gdbm_fetch2(dbm, keystr));
        VALUE v = rb_yield(assoc);

        if (RTEST(v)) {
            rb_ary_push(new, assoc);
        }
        GetDBM2(obj, dbmp, dbm);
    }

    return new;
}

/*
 * call-seq:
 *      gdbm.values_at(key, ...) -> array
 *
 * Returns an array of the values associated with each specified _key_.
 */
static VALUE
fgdbm_values_at(int argc, VALUE *argv, VALUE obj)
{
    VALUE new = rb_ary_new2(argc);
    int i;

    for (i=0; i<argc; i++) {
        rb_ary_push(new, rb_gdbm_fetch3(obj, argv[i]));
    }

    return new;
}

static void
rb_gdbm_modify(VALUE obj)
{
    rb_secure(4);
    if (OBJ_FROZEN(obj)) rb_error_frozen("GDBM");
}

static VALUE
rb_gdbm_delete(VALUE obj, VALUE keystr)
{
    datum key;
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    long len;

    rb_gdbm_modify(obj);
    StringValue(keystr);
    len = RSTRING_LEN(keystr);
    if (TOO_LONG(len)) return Qnil;
    key.dptr = RSTRING_PTR(keystr);
    key.dsize = (int)len;

    GetDBM2(obj, dbmp, dbm);
    if (!gdbm_exists(dbm, key)) {
        return Qnil;
    }

    if (gdbm_delete(dbm, key)) {
        dbmp->di_size = -1;
        rb_raise(rb_eGDBMError, "%s", gdbm_strerror(gdbm_errno));
    }
    else if (dbmp->di_size >= 0) {
        dbmp->di_size--;
    }
    return obj;
}

/*
 * call-seq:
 *      gdbm.delete(key) -> value or nil
 *
 * Removes the key-value-pair with the specified _key_ from this database and
 * returns the corresponding _value_. Returns nil if the database is empty.
 */
static VALUE
fgdbm_delete(VALUE obj, VALUE keystr)
{
    VALUE valstr;

    valstr = fgdbm_fetch(obj, keystr, Qnil);
    rb_gdbm_delete(obj, keystr);
    return valstr;
}

/*
 * call-seq:
 *      gdbm.shift -> (key, value) or nil
 *
 * Removes a key-value-pair from this database and returns it as a
 * two-item array [ _key_, _value_ ]. Returns nil if the database is empty.
 */
static VALUE
fgdbm_shift(VALUE obj)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    VALUE keystr, valstr;

    rb_gdbm_modify(obj);
    GetDBM2(obj, dbmp, dbm);
    keystr = rb_gdbm_firstkey(dbm);
    if (NIL_P(keystr)) return Qnil;
    valstr = rb_gdbm_fetch2(dbm, keystr);
    rb_gdbm_delete(obj, keystr);

    return rb_assoc_new(keystr, valstr);
}

/*
 * call-seq:
 *      gdbm.delete_if { |key, value| block } -> gdbm
 *      gdbm.reject! { |key, value| block } -> gdbm
 *
 * Deletes every key-value pair from _gdbm_ for which _block_ evaluates to true.
 */
static VALUE
fgdbm_delete_if(VALUE obj)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    VALUE keystr, valstr;
    VALUE ret, ary = rb_ary_tmp_new(0);
    int i, status = 0, n;

    rb_gdbm_modify(obj);
    GetDBM2(obj, dbmp, dbm);
    n = dbmp->di_size;
    dbmp->di_size = -1;

    for (keystr = rb_gdbm_firstkey(dbm); RTEST(keystr);
         keystr = rb_gdbm_nextkey(dbm, keystr)) {

	OBJ_FREEZE(keystr);
        valstr = rb_gdbm_fetch2(dbm, keystr);
        ret = rb_protect(rb_yield, rb_assoc_new(rb_str_dup(keystr), valstr), &status);
        if (status != 0) break;
        if (RTEST(ret)) rb_ary_push(ary, keystr);
        GetDBM2(obj, dbmp, dbm);
    }

    for (i = 0; i < RARRAY_LEN(ary); i++)
        rb_gdbm_delete(obj, RARRAY_PTR(ary)[i]);
    if (status) rb_jump_tag(status);
    if (n > 0) dbmp->di_size = n - (int)RARRAY_LEN(ary);
    rb_ary_clear(ary);

    return obj;
}

/*
 * call-seq:
 *      gdbm.clear -> gdbm
 *
 * Removes all the key-value pairs within _gdbm_.
 */
static VALUE
fgdbm_clear(VALUE obj)
{
    datum key, nextkey;
    struct dbmdata *dbmp;
    GDBM_FILE dbm;

    rb_gdbm_modify(obj);
    GetDBM2(obj, dbmp, dbm);
    dbmp->di_size = -1;

#if 0
    while (key = gdbm_firstkey(dbm), key.dptr) {
        if (gdbm_delete(dbm, key)) {
            free(key.dptr);
            rb_raise(rb_eGDBMError, "%s", gdbm_strerror(gdbm_errno));
        }
        free(key.dptr);
    }
#else
    while (key = gdbm_firstkey(dbm), key.dptr) {
        for (; key.dptr; key = nextkey) {
            nextkey = gdbm_nextkey(dbm, key);
            if (gdbm_delete(dbm, key)) {
                free(key.dptr);
                if (nextkey.dptr) free(nextkey.dptr);
                rb_raise(rb_eGDBMError, "%s", gdbm_strerror(gdbm_errno));
            }
            free(key.dptr);
        }
    }
#endif
    dbmp->di_size = 0;

    return obj;
}

/*
 * call-seq:
 *     gdbm.invert  -> hash
 *
 * Returns a hash created by using _gdbm_'s values as keys, and the keys
 * as values.
 */
static VALUE
fgdbm_invert(VALUE obj)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    VALUE keystr, valstr;
    VALUE hash = rb_hash_new();

    GetDBM2(obj, dbmp, dbm);
    for (keystr = rb_gdbm_firstkey(dbm); RTEST(keystr);
         keystr = rb_gdbm_nextkey(dbm, keystr)) {
        valstr = rb_gdbm_fetch2(dbm, keystr);

        rb_hash_aset(hash, valstr, keystr);
    }
    return hash;
}

/*
 * call-seq:
 *      gdbm[key]= value -> value
 *      gdbm.store(key, value) -> value
 *
 * Associates the value _value_ with the specified _key_.
 */
static VALUE
fgdbm_store(VALUE obj, VALUE keystr, VALUE valstr)
{
    datum key, val;
    struct dbmdata *dbmp;
    GDBM_FILE dbm;

    rb_gdbm_modify(obj);
    StringValue(keystr);
    StringValue(valstr);

    key.dptr = RSTRING_PTR(keystr);
    key.dsize = RSTRING_LENINT(keystr);

    val.dptr = RSTRING_PTR(valstr);
    val.dsize = RSTRING_LENINT(valstr);

    GetDBM2(obj, dbmp, dbm);
    dbmp->di_size = -1;
    if (gdbm_store(dbm, key, val, GDBM_REPLACE)) {
        if (errno == EPERM) rb_sys_fail(0);
        rb_raise(rb_eGDBMError, "%s", gdbm_strerror(gdbm_errno));
    }

    return valstr;
}

static VALUE
update_i(VALUE pair, VALUE dbm)
{
    Check_Type(pair, T_ARRAY);
    if (RARRAY_LEN(pair) < 2) {
        rb_raise(rb_eArgError, "pair must be [key, value]");
    }
    fgdbm_store(dbm, RARRAY_PTR(pair)[0], RARRAY_PTR(pair)[1]);
    return Qnil;
}

/*
 * call-seq:
 *     gdbm.update(other) -> gdbm
 *
 * Adds the key-value pairs of _other_ to _gdbm_, overwriting entries with
 * duplicate keys with those from _other_. _other_ must have an each_pair
 * method.
 */
static VALUE
fgdbm_update(VALUE obj, VALUE other)
{
    rb_block_call(other, rb_intern("each_pair"), 0, 0, update_i, obj);
    return obj;
}

/*
 * call-seq:
 *     gdbm.replace(other) -> gdbm
 *
 * Replaces the content of _gdbm_ with the key-value pairs of _other_.
 * _other_ must have an each_pair method.
 */
static VALUE
fgdbm_replace(VALUE obj, VALUE other)
{
    fgdbm_clear(obj);
    rb_block_call(other, rb_intern("each_pair"), 0, 0, update_i, obj);
    return obj;
}

/*
 * call-seq:
 *      gdbm.length -> fixnum
 *      gdbm.size -> fixnum
 *
 * Returns the number of key-value pairs in this database.
 */
static VALUE
fgdbm_length(VALUE obj)
{
    datum key, nextkey;
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    int i = 0;

    GetDBM2(obj, dbmp, dbm);
    if (dbmp->di_size > 0) return INT2FIX(dbmp->di_size);

    for (key = gdbm_firstkey(dbm); key.dptr; key = nextkey) {
        nextkey = gdbm_nextkey(dbm, key);
        free(key.dptr);
        i++;
    }
    dbmp->di_size = i;

    return INT2FIX(i);
}

/*
 * call-seq:
 *      gdbm.empty? -> true or false
 *
 * Returns true if the database is empty.
 */
static VALUE
fgdbm_empty_p(VALUE obj)
{
    datum key;
    struct dbmdata *dbmp;
    GDBM_FILE dbm;

    GetDBM(obj, dbmp);
    if (dbmp->di_size < 0) {
        dbm = dbmp->di_dbm;

        key = gdbm_firstkey(dbm);
        if (key.dptr) {
            free(key.dptr);
            return Qfalse;
        }
        return Qtrue;
    }

    if (dbmp->di_size == 0) return Qtrue;
    return Qfalse;
}

/*
 * call-seq:
 *      gdbm.each_value { |value| block } -> gdbm
 *
 * Executes _block_ for each key in the database, passing the corresponding
 * _value_ as a parameter.
 */
static VALUE
fgdbm_each_value(VALUE obj)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    VALUE keystr;

    RETURN_ENUMERATOR(obj, 0, 0);

    GetDBM2(obj, dbmp, dbm);
    for (keystr = rb_gdbm_firstkey(dbm); RTEST(keystr);
         keystr = rb_gdbm_nextkey(dbm, keystr)) {

        rb_yield(rb_gdbm_fetch2(dbm, keystr));
        GetDBM2(obj, dbmp, dbm);
    }
    return obj;
}

/*
 * call-seq:
 *      gdbm.each_key { |key| block } -> gdbm
 *
 * Executes _block_ for each key in the database, passing the
 * _key_ as a parameter.
 */
static VALUE
fgdbm_each_key(VALUE obj)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    VALUE keystr;

    RETURN_ENUMERATOR(obj, 0, 0);

    GetDBM2(obj, dbmp, dbm);
    for (keystr = rb_gdbm_firstkey(dbm); RTEST(keystr);
         keystr = rb_gdbm_nextkey(dbm, keystr)) {

        rb_yield(keystr);
        GetDBM2(obj, dbmp, dbm);
    }
    return obj;
}

/*
 * call-seq:
 *      gdbm.each_pair { |key, value| block } -> gdbm
 *
 * Executes _block_ for each key in the database, passing the _key_ and the
 * correspoding _value_ as a parameter.
 */
static VALUE
fgdbm_each_pair(VALUE obj)
{
    GDBM_FILE dbm;
    struct dbmdata *dbmp;
    VALUE keystr;

    RETURN_ENUMERATOR(obj, 0, 0);

    GetDBM2(obj, dbmp, dbm);
    for (keystr = rb_gdbm_firstkey(dbm); RTEST(keystr);
         keystr = rb_gdbm_nextkey(dbm, keystr)) {

        rb_yield(rb_assoc_new(keystr, rb_gdbm_fetch2(dbm, keystr)));
        GetDBM2(obj, dbmp, dbm);
    }

    return obj;
}

/*
 * call-seq:
 *      gdbm.keys -> array
 *
 * Returns an array of all keys of this database.
 */
static VALUE
fgdbm_keys(VALUE obj)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    VALUE keystr, ary;

    GetDBM2(obj, dbmp, dbm);
    ary = rb_ary_new();
    for (keystr = rb_gdbm_firstkey(dbm); RTEST(keystr);
         keystr = rb_gdbm_nextkey(dbm, keystr)) {

        rb_ary_push(ary, keystr);
    }

    return ary;
}

/*
 * call-seq:
 *      gdbm.values -> array
 *
 * Returns an array of all values of this database.
 */
static VALUE
fgdbm_values(VALUE obj)
{
    datum key, nextkey;
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    VALUE valstr, ary;

    GetDBM2(obj, dbmp, dbm);
    ary = rb_ary_new();
    for (key = gdbm_firstkey(dbm); key.dptr; key = nextkey) {
        nextkey = gdbm_nextkey(dbm, key);
        valstr = rb_gdbm_fetch(dbm, key);
        free(key.dptr);
        rb_ary_push(ary, valstr);
    }

    return ary;
}

/*
 * call-seq:
 *      gdbm.has_key?(k) -> true or false
 *      gdbm.key?(k) -> true or false
 *
 * Returns true if the given key _k_ exists within the database.
 * Returns false otherwise.
 */
static VALUE
fgdbm_has_key(VALUE obj, VALUE keystr)
{
    datum key;
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    long len;

    StringValue(keystr);
    len = RSTRING_LENINT(keystr);
    if (TOO_LONG(len)) return Qfalse;
    key.dptr = RSTRING_PTR(keystr);
    key.dsize = (int)len;

    GetDBM2(obj, dbmp, dbm);
    if (gdbm_exists(dbm, key))
        return Qtrue;
    return Qfalse;
}

/*
 * call-seq:
 *      gdbm.has_value?(v) -> true or false
 *      gdbm.value?(v) -> true or false
 *
 * Returns true if the given value _v_ exists within the database.
 * Returns false otherwise.
 */
static VALUE
fgdbm_has_value(VALUE obj, VALUE valstr)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    VALUE keystr, valstr2;

    StringValue(valstr);
    GetDBM2(obj, dbmp, dbm);
    for (keystr = rb_gdbm_firstkey(dbm); RTEST(keystr);
         keystr = rb_gdbm_nextkey(dbm, keystr)) {

        valstr2 = rb_gdbm_fetch2(dbm, keystr);

        if (!NIL_P(valstr2) &&
            (int)RSTRING_LEN(valstr) == (int)RSTRING_LEN(valstr2) &&
            memcmp(RSTRING_PTR(valstr), RSTRING_PTR(valstr2),
                   (int)RSTRING_LEN(valstr)) == 0) {
            return Qtrue;
        }
    }
    return Qfalse;
}

/*
 * call-seq:
 *     gdbm.to_a -> array
 *
 * Returns an array of all key-value pairs contained in the database.
 */
static VALUE
fgdbm_to_a(VALUE obj)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    VALUE keystr, ary;

    GetDBM2(obj, dbmp, dbm);
    ary = rb_ary_new();
    for (keystr = rb_gdbm_firstkey(dbm); RTEST(keystr);
         keystr = rb_gdbm_nextkey(dbm, keystr)) {

        rb_ary_push(ary, rb_assoc_new(keystr, rb_gdbm_fetch2(dbm, keystr)));
    }

    return ary;
}

/*
 * call-seq:
 *     gdbm.reorganize -> gdbm
 *
 * Reorganizes the database file. This operation removes reserved space of
 * elements that have already been deleted. It is only useful after a lot of
 * deletions in the database.
 */
static VALUE
fgdbm_reorganize(VALUE obj)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;

    rb_gdbm_modify(obj);
    GetDBM2(obj, dbmp, dbm);
    gdbm_reorganize(dbm);
    rb_fd_fix_cloexec(gdbm_fdesc(dbm));
    return obj;
}

/*
 * call-seq:
 *     gdbm.sync -> gdbm
 *
 * Unless the _gdbm_ object has been opened with the *SYNC* flag, it is not
 * guarenteed that database modification operations are immediately applied to
 * the database file. This method ensures that all recent modifications
 * to the database are written to the file. Blocks until all writing operations
 * to the disk have been finished.
 */
static VALUE
fgdbm_sync(VALUE obj)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;

    rb_gdbm_modify(obj);
    GetDBM2(obj, dbmp, dbm);
    gdbm_sync(dbm);
    return obj;
}

/*
 * call-seq:
 *     gdbm.cachesize = size -> size
 *
 * Sets the size of the internal bucket cache to _size_.
 */
static VALUE
fgdbm_set_cachesize(VALUE obj, VALUE val)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    int optval;

    GetDBM2(obj, dbmp, dbm);
    optval = FIX2INT(val);
    if (gdbm_setopt(dbm, GDBM_CACHESIZE, &optval, sizeof(optval)) == -1) {
        rb_raise(rb_eGDBMError, "%s", gdbm_strerror(gdbm_errno));
    }
    return val;
}

/*
 * call-seq:
 *     gdbm.fastmode = boolean -> boolean
 *
 * Turns the database's fast mode on or off. If fast mode is turned on, gdbm
 * does not wait for writes to be flushed to the disk before continuing.
 *
 * This option is obsolete for gdbm >= 1.8 since fast mode is turned on by
 * default. See also: #syncmode=
 */
static VALUE
fgdbm_set_fastmode(VALUE obj, VALUE val)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    int optval;

    GetDBM2(obj, dbmp, dbm);
    optval = 0;
    if (RTEST(val))
        optval = 1;

    if (gdbm_setopt(dbm, GDBM_FASTMODE, &optval, sizeof(optval)) == -1) {
        rb_raise(rb_eGDBMError, "%s", gdbm_strerror(gdbm_errno));
    }
    return val;
}

/*
 * call-seq:
 *     gdbm.syncmode = boolean -> boolean
 *
 * Turns the database's synchronization mode on or off. If the synchronization
 * mode is turned on, the database's in-memory state will be synchronized to
 * disk after every database modification operation. If the synchronization
 * mode is turned off, GDBM does not wait for writes to be flushed to the disk
 * before continuing.
 *
 * This option is only available for gdbm >= 1.8 where syncmode is turned off
 * by default. See also: #fastmode=
 */
static VALUE
fgdbm_set_syncmode(VALUE obj, VALUE val)
{
#if !defined(GDBM_SYNCMODE)
    fgdbm_set_fastmode(obj, RTEST(val) ? Qfalse : Qtrue);
    return val;
#else
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    int optval;

    GetDBM2(obj, dbmp, dbm);
    optval = 0;
    if (RTEST(val))
        optval = 1;

    if (gdbm_setopt(dbm, GDBM_FASTMODE, &optval, sizeof(optval)) == -1) {
        rb_raise(rb_eGDBMError, "%s", gdbm_strerror(gdbm_errno));
    }
    return val;
#endif
}

/*
 * call-seq:
 *     gdbm.to_hash -> hash
 *
 * Returns a hash of all key-value pairs contained in the database.
 */
static VALUE
fgdbm_to_hash(VALUE obj)
{
    struct dbmdata *dbmp;
    GDBM_FILE dbm;
    VALUE keystr, hash;

    GetDBM2(obj, dbmp, dbm);
    hash = rb_hash_new();
    for (keystr = rb_gdbm_firstkey(dbm); RTEST(keystr);
         keystr = rb_gdbm_nextkey(dbm, keystr)) {

        rb_hash_aset(hash, keystr, rb_gdbm_fetch2(dbm, keystr));
    }

    return hash;
}

/*
 * call-seq:
 *      gdbm.reject { |key, value| block } -> hash
 *
 * Returns a hash copy of _gdbm_ where all key-value pairs from _gdbm_ for
 * which _block_ evaluates to true are removed. See also: #delete_if
 */
static VALUE
fgdbm_reject(VALUE obj)
{
    return rb_hash_delete_if(fgdbm_to_hash(obj));
}

void
Init_gdbm(void)
{
    rb_cGDBM = rb_define_class("GDBM", rb_cObject);
    rb_eGDBMError = rb_define_class("GDBMError", rb_eStandardError);
    rb_eGDBMFatalError = rb_define_class("GDBMFatalError", rb_eException);
    rb_include_module(rb_cGDBM, rb_mEnumerable);

    rb_define_alloc_func(rb_cGDBM, fgdbm_s_alloc);
    rb_define_singleton_method(rb_cGDBM, "open", fgdbm_s_open, -1);

    rb_define_method(rb_cGDBM, "initialize", fgdbm_initialize, -1);
    rb_define_method(rb_cGDBM, "close", fgdbm_close, 0);
    rb_define_method(rb_cGDBM, "closed?", fgdbm_closed, 0);
    rb_define_method(rb_cGDBM, "[]", fgdbm_aref, 1);
    rb_define_method(rb_cGDBM, "fetch", fgdbm_fetch_m, -1);
    rb_define_method(rb_cGDBM, "[]=", fgdbm_store, 2);
    rb_define_method(rb_cGDBM, "store", fgdbm_store, 2);
    rb_define_method(rb_cGDBM, "index",  fgdbm_index, 1);
    rb_define_method(rb_cGDBM, "key",  fgdbm_key, 1);
    rb_define_method(rb_cGDBM, "select",  fgdbm_select, 0);
    rb_define_method(rb_cGDBM, "values_at",  fgdbm_values_at, -1);
    rb_define_method(rb_cGDBM, "length", fgdbm_length, 0);
    rb_define_method(rb_cGDBM, "size", fgdbm_length, 0);
    rb_define_method(rb_cGDBM, "empty?", fgdbm_empty_p, 0);
    rb_define_method(rb_cGDBM, "each", fgdbm_each_pair, 0);
    rb_define_method(rb_cGDBM, "each_value", fgdbm_each_value, 0);
    rb_define_method(rb_cGDBM, "each_key", fgdbm_each_key, 0);
    rb_define_method(rb_cGDBM, "each_pair", fgdbm_each_pair, 0);
    rb_define_method(rb_cGDBM, "keys", fgdbm_keys, 0);
    rb_define_method(rb_cGDBM, "values", fgdbm_values, 0);
    rb_define_method(rb_cGDBM, "shift", fgdbm_shift, 0);
    rb_define_method(rb_cGDBM, "delete", fgdbm_delete, 1);
    rb_define_method(rb_cGDBM, "delete_if", fgdbm_delete_if, 0);
    rb_define_method(rb_cGDBM, "reject!", fgdbm_delete_if, 0);
    rb_define_method(rb_cGDBM, "reject", fgdbm_reject, 0);
    rb_define_method(rb_cGDBM, "clear", fgdbm_clear, 0);
    rb_define_method(rb_cGDBM, "invert", fgdbm_invert, 0);
    rb_define_method(rb_cGDBM, "update", fgdbm_update, 1);
    rb_define_method(rb_cGDBM, "replace", fgdbm_replace, 1);
    rb_define_method(rb_cGDBM, "reorganize", fgdbm_reorganize, 0);
    rb_define_method(rb_cGDBM, "sync", fgdbm_sync, 0);
    /* rb_define_method(rb_cGDBM, "setopt", fgdbm_setopt, 2); */
    rb_define_method(rb_cGDBM, "cachesize=", fgdbm_set_cachesize, 1);
    rb_define_method(rb_cGDBM, "fastmode=", fgdbm_set_fastmode, 1);
    rb_define_method(rb_cGDBM, "syncmode=", fgdbm_set_syncmode, 1);

    rb_define_method(rb_cGDBM, "include?", fgdbm_has_key, 1);
    rb_define_method(rb_cGDBM, "has_key?", fgdbm_has_key, 1);
    rb_define_method(rb_cGDBM, "member?", fgdbm_has_key, 1);
    rb_define_method(rb_cGDBM, "has_value?", fgdbm_has_value, 1);
    rb_define_method(rb_cGDBM, "key?", fgdbm_has_key, 1);
    rb_define_method(rb_cGDBM, "value?", fgdbm_has_value, 1);

    rb_define_method(rb_cGDBM, "to_a", fgdbm_to_a, 0);
    rb_define_method(rb_cGDBM, "to_hash", fgdbm_to_hash, 0);

    /* flag for #new and #open: open database as a reader */
    rb_define_const(rb_cGDBM, "READER",  INT2FIX(GDBM_READER|RUBY_GDBM_RW_BIT));
    /* flag for #new and #open: open database as a writer */
    rb_define_const(rb_cGDBM, "WRITER",  INT2FIX(GDBM_WRITER|RUBY_GDBM_RW_BIT));
    /* flag for #new and #open: open database as a writer; if the database does not exist, create a new one */
    rb_define_const(rb_cGDBM, "WRCREAT", INT2FIX(GDBM_WRCREAT|RUBY_GDBM_RW_BIT));
    /* flag for #new and #open: open database as a writer; overwrite any existing databases  */
    rb_define_const(rb_cGDBM, "NEWDB",   INT2FIX(GDBM_NEWDB|RUBY_GDBM_RW_BIT));

    /* flag for #new and #open. this flag is obsolete for gdbm >= 1.8 */
    rb_define_const(rb_cGDBM, "FAST", INT2FIX(GDBM_FAST));
    /* this flag is obsolete in gdbm 1.8.
       On gdbm 1.8, fast mode is default behavior. */

    /* gdbm version 1.8 specific */
#if defined(GDBM_SYNC)
    /* flag for #new and #open. only for gdbm >= 1.8 */
    rb_define_const(rb_cGDBM, "SYNC",    INT2FIX(GDBM_SYNC));
#endif
#if defined(GDBM_NOLOCK)
    /* flag for #new and #open */
    rb_define_const(rb_cGDBM, "NOLOCK",  INT2FIX(GDBM_NOLOCK));
#endif
    /* version of the gdbm library*/
    rb_define_const(rb_cGDBM, "VERSION",  rb_str_new2(gdbm_version));
}
