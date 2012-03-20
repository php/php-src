/**********************************************************************

  hash.c -

  $Author$
  created at: Mon Nov 22 18:51:18 JST 1993

  Copyright (C) 1993-2007 Yukihiro Matsumoto
  Copyright (C) 2000  Network Applied Communication Laboratory, Inc.
  Copyright (C) 2000  Information-technology Promotion Agency, Japan

**********************************************************************/

#include "ruby/ruby.h"
#include "ruby/st.h"
#include "ruby/util.h"
#include "ruby/encoding.h"
#include <errno.h>

#ifdef __APPLE__
#include <crt_externs.h>
#endif

static VALUE rb_hash_s_try_convert(VALUE, VALUE);

#define HASH_DELETED  FL_USER1
#define HASH_PROC_DEFAULT FL_USER2

VALUE
rb_hash_freeze(VALUE hash)
{
    return rb_obj_freeze(hash);
}

VALUE rb_cHash;

static VALUE envtbl;
static ID id_hash, id_yield, id_default;

static int
rb_any_cmp(VALUE a, VALUE b)
{
    if (a == b) return 0;
    if (FIXNUM_P(a) && FIXNUM_P(b)) {
	return a != b;
    }
    if (RB_TYPE_P(a, T_STRING) && RBASIC(a)->klass == rb_cString &&
	TYPE(b) == T_STRING && RBASIC(b)->klass == rb_cString) {
	return rb_str_hash_cmp(a, b);
    }
    if (a == Qundef || b == Qundef) return -1;
    if (SYMBOL_P(a) && SYMBOL_P(b)) {
	return a != b;
    }

    return !rb_eql(a, b);
}

VALUE
rb_hash(VALUE obj)
{
    VALUE hval = rb_funcall(obj, id_hash, 0);
  retry:
    switch (TYPE(hval)) {
      case T_FIXNUM:
	return hval;

      case T_BIGNUM:
	return LONG2FIX(((long*)(RBIGNUM_DIGITS(hval)))[0]);

      default:
	hval = rb_to_int(hval);
	goto retry;
    }
}

static st_index_t
rb_any_hash(VALUE a)
{
    VALUE hval;
    st_index_t hnum;

    if (SPECIAL_CONST_P(a)) {
	if (a == Qundef) return 0;
	hnum = rb_hash_end(rb_hash_start((st_index_t)a));
    }
    else if (BUILTIN_TYPE(a) == T_STRING) {
	hnum = rb_str_hash(a);
    }
    else {
        hval = rb_hash(a);
	hnum = FIX2LONG(hval);
    }
    hnum <<= 1;
    return (st_index_t)RSHIFT(hnum, 1);
}

static const struct st_hash_type objhash = {
    rb_any_cmp,
    rb_any_hash,
};

extern const struct st_hash_type st_hashtype_num;
#define identhash st_hashtype_num

typedef int st_foreach_func(st_data_t, st_data_t, st_data_t);

struct foreach_safe_arg {
    st_table *tbl;
    st_foreach_func *func;
    st_data_t arg;
};

static int
foreach_safe_i(st_data_t key, st_data_t value, struct foreach_safe_arg *arg)
{
    int status;

    status = (*arg->func)(key, value, arg->arg);
    if (status == ST_CONTINUE) {
	return ST_CHECK;
    }
    return status;
}

void
st_foreach_safe(st_table *table, int (*func)(ANYARGS), st_data_t a)
{
    struct foreach_safe_arg arg;

    arg.tbl = table;
    arg.func = (st_foreach_func *)func;
    arg.arg = a;
    if (st_foreach_check(table, foreach_safe_i, (st_data_t)&arg, Qundef)) {
	rb_raise(rb_eRuntimeError, "hash modified during iteration");
    }
}

typedef int rb_foreach_func(VALUE, VALUE, VALUE);

struct hash_foreach_arg {
    VALUE hash;
    rb_foreach_func *func;
    VALUE arg;
};

static int
hash_foreach_iter(st_data_t key, st_data_t value, st_data_t argp, int err)
{
    struct hash_foreach_arg *arg = (struct hash_foreach_arg *)argp;
    int status;
    st_table *tbl;

    tbl = RHASH(arg->hash)->ntbl;
    if ((VALUE)key == Qundef) return ST_CONTINUE;
    status = (*arg->func)((VALUE)key, (VALUE)value, arg->arg);
    if (RHASH(arg->hash)->ntbl != tbl) {
	rb_raise(rb_eRuntimeError, "rehash occurred during iteration");
    }
    switch (status) {
      case ST_DELETE:
	st_delete_safe(tbl, &key, 0, Qundef);
	FL_SET(arg->hash, HASH_DELETED);
      case ST_CONTINUE:
	break;
      case ST_STOP:
	return ST_STOP;
    }
    return ST_CHECK;
}

static VALUE
hash_foreach_ensure(VALUE hash)
{
    RHASH(hash)->iter_lev--;

    if (RHASH(hash)->iter_lev == 0) {
	if (FL_TEST(hash, HASH_DELETED)) {
	    st_cleanup_safe(RHASH(hash)->ntbl, Qundef);
	    FL_UNSET(hash, HASH_DELETED);
	}
    }
    return 0;
}

static VALUE
hash_foreach_call(struct hash_foreach_arg *arg)
{
    if (st_foreach_check(RHASH(arg->hash)->ntbl, hash_foreach_iter, (st_data_t)arg, Qundef)) {
	rb_raise(rb_eRuntimeError, "hash modified during iteration");
    }
    return Qnil;
}

void
rb_hash_foreach(VALUE hash, int (*func)(ANYARGS), VALUE farg)
{
    struct hash_foreach_arg arg;

    if (!RHASH(hash)->ntbl)
        return;
    RHASH(hash)->iter_lev++;
    arg.hash = hash;
    arg.func = (rb_foreach_func *)func;
    arg.arg  = farg;
    rb_ensure(hash_foreach_call, (VALUE)&arg, hash_foreach_ensure, hash);
}

static VALUE
hash_alloc(VALUE klass)
{
    NEWOBJ(hash, struct RHash);
    OBJSETUP(hash, klass, T_HASH);

    RHASH_IFNONE(hash) = Qnil;

    return (VALUE)hash;
}

VALUE
rb_hash_new(void)
{
    return hash_alloc(rb_cHash);
}

VALUE
rb_hash_dup(VALUE hash)
{
    NEWOBJ(ret, struct RHash);
    DUPSETUP(ret, hash);

    if (!RHASH_EMPTY_P(hash))
        ret->ntbl = st_copy(RHASH(hash)->ntbl);
    if (FL_TEST(hash, HASH_PROC_DEFAULT)) {
        FL_SET(ret, HASH_PROC_DEFAULT);
    }
    RHASH_IFNONE(ret) = RHASH_IFNONE(hash);
    return (VALUE)ret;
}

static void
rb_hash_modify_check(VALUE hash)
{
    rb_check_frozen(hash);
    if (!OBJ_UNTRUSTED(hash) && rb_safe_level() >= 4)
	rb_raise(rb_eSecurityError, "Insecure: can't modify hash");
}

struct st_table *
rb_hash_tbl(VALUE hash)
{
    if (!RHASH(hash)->ntbl) {
        RHASH(hash)->ntbl = st_init_table(&objhash);
    }
    return RHASH(hash)->ntbl;
}

static void
rb_hash_modify(VALUE hash)
{
    rb_hash_modify_check(hash);
    rb_hash_tbl(hash);
}

static void
hash_update(VALUE hash, VALUE key)
{
    if (RHASH(hash)->iter_lev > 0 && !st_lookup(RHASH(hash)->ntbl, key, 0)) {
	rb_raise(rb_eRuntimeError, "can't add a new key into hash during iteration");
    }
}

static void
default_proc_arity_check(VALUE proc)
{
    int n = rb_proc_arity(proc);

    if (rb_proc_lambda_p(proc) && n != 2 && (n >= 0 || n < -3)) {
	if (n < 0) n = -n-1;
	rb_raise(rb_eTypeError, "default_proc takes two arguments (2 for %d)", n);
    }
}

/*
 *  call-seq:
 *     Hash.new                          -> new_hash
 *     Hash.new(obj)                     -> new_hash
 *     Hash.new {|hash, key| block }     -> new_hash
 *
 *  Returns a new, empty hash. If this hash is subsequently accessed by
 *  a key that doesn't correspond to a hash entry, the value returned
 *  depends on the style of <code>new</code> used to create the hash. In
 *  the first form, the access returns <code>nil</code>. If
 *  <i>obj</i> is specified, this single object will be used for
 *  all <em>default values</em>. If a block is specified, it will be
 *  called with the hash object and the key, and should return the
 *  default value. It is the block's responsibility to store the value
 *  in the hash if required.
 *
 *     h = Hash.new("Go Fish")
 *     h["a"] = 100
 *     h["b"] = 200
 *     h["a"]           #=> 100
 *     h["c"]           #=> "Go Fish"
 *     # The following alters the single default object
 *     h["c"].upcase!   #=> "GO FISH"
 *     h["d"]           #=> "GO FISH"
 *     h.keys           #=> ["a", "b"]
 *
 *     # While this creates a new default object each time
 *     h = Hash.new { |hash, key| hash[key] = "Go Fish: #{key}" }
 *     h["c"]           #=> "Go Fish: c"
 *     h["c"].upcase!   #=> "GO FISH: C"
 *     h["d"]           #=> "Go Fish: d"
 *     h.keys           #=> ["c", "d"]
 *
 */

static VALUE
rb_hash_initialize(int argc, VALUE *argv, VALUE hash)
{
    VALUE ifnone;

    rb_hash_modify(hash);
    if (rb_block_given_p()) {
	rb_check_arity(argc, 0, 0);
	ifnone = rb_block_proc();
	default_proc_arity_check(ifnone);
	RHASH_IFNONE(hash) = ifnone;
	FL_SET(hash, HASH_PROC_DEFAULT);
    }
    else {
	rb_scan_args(argc, argv, "01", &ifnone);
	RHASH_IFNONE(hash) = ifnone;
    }

    return hash;
}

/*
 *  call-seq:
 *     Hash[ key, value, ... ]         -> new_hash
 *     Hash[ [ [key, value], ... ] ]   -> new_hash
 *     Hash[ object ]                  -> new_hash
 *
 *  Creates a new hash populated with the given objects. Equivalent to
 *  the literal <code>{ <i>key</i> => <i>value</i>, ... }</code>. In the first
 *  form, keys and values occur in pairs, so there must be an even number of arguments.
 *  The second and third form take a single argument which is either
 *  an array of key-value pairs or an object convertible to a hash.
 *
 *     Hash["a", 100, "b", 200]             #=> {"a"=>100, "b"=>200}
 *     Hash[ [ ["a", 100], ["b", 200] ] ]   #=> {"a"=>100, "b"=>200}
 *     Hash["a" => 100, "b" => 200]         #=> {"a"=>100, "b"=>200}
 */

static VALUE
rb_hash_s_create(int argc, VALUE *argv, VALUE klass)
{
    VALUE hash, tmp;
    int i;

    if (argc == 1) {
	tmp = rb_hash_s_try_convert(Qnil, argv[0]);
	if (!NIL_P(tmp)) {
	    hash = hash_alloc(klass);
	    if (RHASH(tmp)->ntbl) {
		RHASH(hash)->ntbl = st_copy(RHASH(tmp)->ntbl);
	    }
	    return hash;
	}

	tmp = rb_check_array_type(argv[0]);
	if (!NIL_P(tmp)) {
	    long i;

	    hash = hash_alloc(klass);
	    for (i = 0; i < RARRAY_LEN(tmp); ++i) {
		VALUE v = rb_check_array_type(RARRAY_PTR(tmp)[i]);
		VALUE key, val = Qnil;

		if (NIL_P(v)) continue;
		switch (RARRAY_LEN(v)) {
		  case 2:
		    val = RARRAY_PTR(v)[1];
		  case 1:
		    key = RARRAY_PTR(v)[0];
		    rb_hash_aset(hash, key, val);
		}
	    }
	    return hash;
	}
    }
    if (argc % 2 != 0) {
	rb_raise(rb_eArgError, "odd number of arguments for Hash");
    }

    hash = hash_alloc(klass);
    for (i=0; i<argc; i+=2) {
        rb_hash_aset(hash, argv[i], argv[i + 1]);
    }

    return hash;
}

static VALUE
to_hash(VALUE hash)
{
    return rb_convert_type(hash, T_HASH, "Hash", "to_hash");
}

VALUE
rb_check_hash_type(VALUE hash)
{
    return rb_check_convert_type(hash, T_HASH, "Hash", "to_hash");
}

/*
 *  call-seq:
 *     Hash.try_convert(obj) -> hash or nil
 *
 *  Try to convert <i>obj</i> into a hash, using to_hash method.
 *  Returns converted hash or nil if <i>obj</i> cannot be converted
 *  for any reason.
 *
 *     Hash.try_convert({1=>2})   # => {1=>2}
 *     Hash.try_convert("1=>2")   # => nil
 */
static VALUE
rb_hash_s_try_convert(VALUE dummy, VALUE hash)
{
    return rb_check_hash_type(hash);
}

static int
rb_hash_rehash_i(VALUE key, VALUE value, VALUE arg)
{
    st_table *tbl = (st_table *)arg;

    if (key != Qundef) st_insert(tbl, key, value);
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.rehash -> hsh
 *
 *  Rebuilds the hash based on the current hash values for each key. If
 *  values of key objects have changed since they were inserted, this
 *  method will reindex <i>hsh</i>. If <code>Hash#rehash</code> is
 *  called while an iterator is traversing the hash, an
 *  <code>RuntimeError</code> will be raised in the iterator.
 *
 *     a = [ "a", "b" ]
 *     c = [ "c", "d" ]
 *     h = { a => 100, c => 300 }
 *     h[a]       #=> 100
 *     a[0] = "z"
 *     h[a]       #=> nil
 *     h.rehash   #=> {["z", "b"]=>100, ["c", "d"]=>300}
 *     h[a]       #=> 100
 */

static VALUE
rb_hash_rehash(VALUE hash)
{
    st_table *tbl;

    if (RHASH(hash)->iter_lev > 0) {
	rb_raise(rb_eRuntimeError, "rehash during iteration");
    }
    rb_hash_modify_check(hash);
    if (!RHASH(hash)->ntbl)
        return hash;
    tbl = st_init_table_with_size(RHASH(hash)->ntbl->type, RHASH(hash)->ntbl->num_entries);
    rb_hash_foreach(hash, rb_hash_rehash_i, (VALUE)tbl);
    st_free_table(RHASH(hash)->ntbl);
    RHASH(hash)->ntbl = tbl;

    return hash;
}

/*
 *  call-seq:
 *     hsh[key]    ->  value
 *
 *  Element Reference---Retrieves the <i>value</i> object corresponding
 *  to the <i>key</i> object. If not found, returns the default value (see
 *  <code>Hash::new</code> for details).
 *
 *     h = { "a" => 100, "b" => 200 }
 *     h["a"]   #=> 100
 *     h["c"]   #=> nil
 *
 */

VALUE
rb_hash_aref(VALUE hash, VALUE key)
{
    st_data_t val;

    if (!RHASH(hash)->ntbl || !st_lookup(RHASH(hash)->ntbl, key, &val)) {
	if (!FL_TEST(hash, HASH_PROC_DEFAULT) &&
	    rb_method_basic_definition_p(CLASS_OF(hash), id_default)) {
	    return RHASH_IFNONE(hash);
	}
	else {
	    return rb_funcall(hash, id_default, 1, key);
	}
    }
    return (VALUE)val;
}

VALUE
rb_hash_lookup2(VALUE hash, VALUE key, VALUE def)
{
    st_data_t val;

    if (!RHASH(hash)->ntbl || !st_lookup(RHASH(hash)->ntbl, key, &val)) {
	return def; /* without Hash#default */
    }
    return (VALUE)val;
}

VALUE
rb_hash_lookup(VALUE hash, VALUE key)
{
    return rb_hash_lookup2(hash, key, Qnil);
}

/*
 *  call-seq:
 *     hsh.fetch(key [, default] )       -> obj
 *     hsh.fetch(key) {| key | block }   -> obj
 *
 *  Returns a value from the hash for the given key. If the key can't be
 *  found, there are several options: With no other arguments, it will
 *  raise an <code>KeyError</code> exception; if <i>default</i> is
 *  given, then that will be returned; if the optional code block is
 *  specified, then that will be run and its result returned.
 *
 *     h = { "a" => 100, "b" => 200 }
 *     h.fetch("a")                            #=> 100
 *     h.fetch("z", "go fish")                 #=> "go fish"
 *     h.fetch("z") { |el| "go fish, #{el}"}   #=> "go fish, z"
 *
 *  The following example shows that an exception is raised if the key
 *  is not found and a default value is not supplied.
 *
 *     h = { "a" => 100, "b" => 200 }
 *     h.fetch("z")
 *
 *  <em>produces:</em>
 *
 *     prog.rb:2:in `fetch': key not found (KeyError)
 *      from prog.rb:2
 *
 */

static VALUE
rb_hash_fetch_m(int argc, VALUE *argv, VALUE hash)
{
    VALUE key, if_none;
    st_data_t val;
    long block_given;

    rb_scan_args(argc, argv, "11", &key, &if_none);

    block_given = rb_block_given_p();
    if (block_given && argc == 2) {
	rb_warn("block supersedes default value argument");
    }
    if (!RHASH(hash)->ntbl || !st_lookup(RHASH(hash)->ntbl, key, &val)) {
	if (block_given) return rb_yield(key);
	if (argc == 1) {
	    volatile VALUE desc = rb_protect(rb_inspect, key, 0);
	    if (NIL_P(desc)) {
		desc = rb_any_to_s(key);
	    }
	    desc = rb_str_ellipsize(desc, 65);
	    rb_raise(rb_eKeyError, "key not found: %s", RSTRING_PTR(desc));
	}
	return if_none;
    }
    return (VALUE)val;
}

VALUE
rb_hash_fetch(VALUE hash, VALUE key)
{
    return rb_hash_fetch_m(1, &key, hash);
}

/*
 *  call-seq:
 *     hsh.default(key=nil)   -> obj
 *
 *  Returns the default value, the value that would be returned by
 *  <i>hsh</i>[<i>key</i>] if <i>key</i> did not exist in <i>hsh</i>.
 *  See also <code>Hash::new</code> and <code>Hash#default=</code>.
 *
 *     h = Hash.new                            #=> {}
 *     h.default                               #=> nil
 *     h.default(2)                            #=> nil
 *
 *     h = Hash.new("cat")                     #=> {}
 *     h.default                               #=> "cat"
 *     h.default(2)                            #=> "cat"
 *
 *     h = Hash.new {|h,k| h[k] = k.to_i*10}   #=> {}
 *     h.default                               #=> nil
 *     h.default(2)                            #=> 20
 */

static VALUE
rb_hash_default(int argc, VALUE *argv, VALUE hash)
{
    VALUE key, ifnone;

    rb_scan_args(argc, argv, "01", &key);
    ifnone = RHASH_IFNONE(hash);
    if (FL_TEST(hash, HASH_PROC_DEFAULT)) {
	if (argc == 0) return Qnil;
	return rb_funcall(ifnone, id_yield, 2, hash, key);
    }
    return ifnone;
}

/*
 *  call-seq:
 *     hsh.default = obj     -> obj
 *
 *  Sets the default value, the value returned for a key that does not
 *  exist in the hash. It is not possible to set the default to a
 *  <code>Proc</code> that will be executed on each key lookup.
 *
 *     h = { "a" => 100, "b" => 200 }
 *     h.default = "Go fish"
 *     h["a"]     #=> 100
 *     h["z"]     #=> "Go fish"
 *     # This doesn't do what you might hope...
 *     h.default = proc do |hash, key|
 *       hash[key] = key + key
 *     end
 *     h[2]       #=> #<Proc:0x401b3948@-:6>
 *     h["cat"]   #=> #<Proc:0x401b3948@-:6>
 */

static VALUE
rb_hash_set_default(VALUE hash, VALUE ifnone)
{
    rb_hash_modify(hash);
    RHASH_IFNONE(hash) = ifnone;
    FL_UNSET(hash, HASH_PROC_DEFAULT);
    return ifnone;
}

/*
 *  call-seq:
 *     hsh.default_proc -> anObject
 *
 *  If <code>Hash::new</code> was invoked with a block, return that
 *  block, otherwise return <code>nil</code>.
 *
 *     h = Hash.new {|h,k| h[k] = k*k }   #=> {}
 *     p = h.default_proc                 #=> #<Proc:0x401b3d08@-:1>
 *     a = []                             #=> []
 *     p.call(a, 2)
 *     a                                  #=> [nil, nil, 4]
 */


static VALUE
rb_hash_default_proc(VALUE hash)
{
    if (FL_TEST(hash, HASH_PROC_DEFAULT)) {
	return RHASH_IFNONE(hash);
    }
    return Qnil;
}

/*
 *  call-seq:
 *     hsh.default_proc = proc_obj     -> proc_obj
 *
 *  Sets the default proc to be executed on each key lookup.
 *
 *     h.default_proc = proc do |hash, key|
 *       hash[key] = key + key
 *     end
 *     h[2]       #=> 4
 *     h["cat"]   #=> "catcat"
 */

static VALUE
rb_hash_set_default_proc(VALUE hash, VALUE proc)
{
    VALUE b;

    rb_hash_modify(hash);
    b = rb_check_convert_type(proc, T_DATA, "Proc", "to_proc");
    if (NIL_P(b) || !rb_obj_is_proc(b)) {
	rb_raise(rb_eTypeError,
		 "wrong default_proc type %s (expected Proc)",
		 rb_obj_classname(proc));
    }
    proc = b;
    default_proc_arity_check(proc);
    RHASH_IFNONE(hash) = proc;
    FL_SET(hash, HASH_PROC_DEFAULT);
    return proc;
}

static int
key_i(VALUE key, VALUE value, VALUE arg)
{
    VALUE *args = (VALUE *)arg;

    if (rb_equal(value, args[0])) {
	args[1] = key;
	return ST_STOP;
    }
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.key(value)    -> key
 *
 *  Returns the key of an occurrence of a given value. If the value is
 *  not found, returns <code>nil</code>.
 *
 *     h = { "a" => 100, "b" => 200, "c" => 300, "d" => 300 }
 *     h.key(200)   #=> "b"
 *     h.key(300)   #=> "c"
 *     h.key(999)   #=> nil
 *
 */

static VALUE
rb_hash_key(VALUE hash, VALUE value)
{
    VALUE args[2];

    args[0] = value;
    args[1] = Qnil;

    rb_hash_foreach(hash, key_i, (VALUE)args);

    return args[1];
}

/* :nodoc: */
static VALUE
rb_hash_index(VALUE hash, VALUE value)
{
    rb_warn("Hash#index is deprecated; use Hash#key");
    return rb_hash_key(hash, value);
}

static VALUE
rb_hash_delete_key(VALUE hash, VALUE key)
{
    st_data_t ktmp = (st_data_t)key, val;

    if (!RHASH(hash)->ntbl)
        return Qundef;
    if (RHASH(hash)->iter_lev > 0) {
	if (st_delete_safe(RHASH(hash)->ntbl, &ktmp, &val, Qundef)) {
	    FL_SET(hash, HASH_DELETED);
	    return (VALUE)val;
	}
    }
    else if (st_delete(RHASH(hash)->ntbl, &ktmp, &val))
	return (VALUE)val;
    return Qundef;
}

/*
 *  call-seq:
 *     hsh.delete(key)                   -> value
 *     hsh.delete(key) {| key | block }  -> value
 *
 *  Deletes and returns a key-value pair from <i>hsh</i> whose key is
 *  equal to <i>key</i>. If the key is not found, returns the
 *  <em>default value</em>. If the optional code block is given and the
 *  key is not found, pass in the key and return the result of
 *  <i>block</i>.
 *
 *     h = { "a" => 100, "b" => 200 }
 *     h.delete("a")                              #=> 100
 *     h.delete("z")                              #=> nil
 *     h.delete("z") { |el| "#{el} not found" }   #=> "z not found"
 *
 */

VALUE
rb_hash_delete(VALUE hash, VALUE key)
{
    VALUE val;

    rb_hash_modify(hash);
    val = rb_hash_delete_key(hash, key);
    if (val != Qundef) return val;
    if (rb_block_given_p()) {
	return rb_yield(key);
    }
    return Qnil;
}

struct shift_var {
    VALUE key;
    VALUE val;
};

static int
shift_i(VALUE key, VALUE value, VALUE arg)
{
    struct shift_var *var = (struct shift_var *)arg;

    if (key == Qundef) return ST_CONTINUE;
    if (var->key != Qundef) return ST_STOP;
    var->key = key;
    var->val = value;
    return ST_DELETE;
}

static int
shift_i_safe(VALUE key, VALUE value, VALUE arg)
{
    struct shift_var *var = (struct shift_var *)arg;

    if (key == Qundef) return ST_CONTINUE;
    var->key = key;
    var->val = value;
    return ST_STOP;
}

/*
 *  call-seq:
 *     hsh.shift -> anArray or obj
 *
 *  Removes a key-value pair from <i>hsh</i> and returns it as the
 *  two-item array <code>[</code> <i>key, value</i> <code>]</code>, or
 *  the hash's default value if the hash is empty.
 *
 *     h = { 1 => "a", 2 => "b", 3 => "c" }
 *     h.shift   #=> [1, "a"]
 *     h         #=> {2=>"b", 3=>"c"}
 */

static VALUE
rb_hash_shift(VALUE hash)
{
    struct shift_var var;

    rb_hash_modify(hash);
    var.key = Qundef;
    rb_hash_foreach(hash, RHASH(hash)->iter_lev > 0 ? shift_i_safe : shift_i,
		    (VALUE)&var);

    if (var.key != Qundef) {
	if (RHASH(hash)->iter_lev > 0) {
	    rb_hash_delete_key(hash, var.key);
	}
	return rb_assoc_new(var.key, var.val);
    }
    else if (FL_TEST(hash, HASH_PROC_DEFAULT)) {
	return rb_funcall(RHASH_IFNONE(hash), id_yield, 2, hash, Qnil);
    }
    else {
	return RHASH_IFNONE(hash);
    }
}

static int
delete_if_i(VALUE key, VALUE value, VALUE hash)
{
    if (key == Qundef) return ST_CONTINUE;
    if (RTEST(rb_yield_values(2, key, value))) {
	rb_hash_delete_key(hash, key);
    }
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.delete_if {| key, value | block }  -> hsh
 *     hsh.delete_if                          -> an_enumerator
 *
 *  Deletes every key-value pair from <i>hsh</i> for which <i>block</i>
 *  evaluates to <code>true</code>.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     h = { "a" => 100, "b" => 200, "c" => 300 }
 *     h.delete_if {|key, value| key >= "b" }   #=> {"a"=>100}
 *
 */

VALUE
rb_hash_delete_if(VALUE hash)
{
    RETURN_ENUMERATOR(hash, 0, 0);
    rb_hash_modify(hash);
    rb_hash_foreach(hash, delete_if_i, hash);
    return hash;
}

/*
 *  call-seq:
 *     hsh.reject! {| key, value | block }  -> hsh or nil
 *     hsh.reject!                          -> an_enumerator
 *
 *  Equivalent to <code>Hash#delete_if</code>, but returns
 *  <code>nil</code> if no changes were made.
 */

VALUE
rb_hash_reject_bang(VALUE hash)
{
    st_index_t n;

    RETURN_ENUMERATOR(hash, 0, 0);
    rb_hash_modify(hash);
    if (!RHASH(hash)->ntbl)
        return Qnil;
    n = RHASH(hash)->ntbl->num_entries;
    rb_hash_foreach(hash, delete_if_i, hash);
    if (n == RHASH(hash)->ntbl->num_entries) return Qnil;
    return hash;
}

/*
 *  call-seq:
 *     hsh.reject {| key, value | block }  -> a_hash
 *     hsh.reject                          -> an_enumerator
 *
 *  Same as <code>Hash#delete_if</code>, but works on (and returns) a
 *  copy of the <i>hsh</i>. Equivalent to
 *  <code><i>hsh</i>.dup.delete_if</code>.
 *
 */

static VALUE
rb_hash_reject(VALUE hash)
{
    return rb_hash_delete_if(rb_obj_dup(hash));
}

/*
 * call-seq:
 *   hsh.values_at(key, ...)   -> array
 *
 * Return an array containing the values associated with the given keys.
 * Also see <code>Hash.select</code>.
 *
 *   h = { "cat" => "feline", "dog" => "canine", "cow" => "bovine" }
 *   h.values_at("cow", "cat")  #=> ["bovine", "feline"]
 */

VALUE
rb_hash_values_at(int argc, VALUE *argv, VALUE hash)
{
    VALUE result = rb_ary_new2(argc);
    long i;

    for (i=0; i<argc; i++) {
	rb_ary_push(result, rb_hash_aref(hash, argv[i]));
    }
    return result;
}

static int
select_i(VALUE key, VALUE value, VALUE result)
{
    if (key == Qundef) return ST_CONTINUE;
    if (RTEST(rb_yield_values(2, key, value)))
	rb_hash_aset(result, key, value);
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.select {|key, value| block}   -> a_hash
 *     hsh.select                        -> an_enumerator
 *
 *  Returns a new hash consisting of entries for which the block returns true.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     h = { "a" => 100, "b" => 200, "c" => 300 }
 *     h.select {|k,v| k > "a"}  #=> {"b" => 200, "c" => 300}
 *     h.select {|k,v| v < 200}  #=> {"a" => 100}
 */

VALUE
rb_hash_select(VALUE hash)
{
    VALUE result;

    RETURN_ENUMERATOR(hash, 0, 0);
    result = rb_hash_new();
    rb_hash_foreach(hash, select_i, result);
    return result;
}

static int
keep_if_i(VALUE key, VALUE value, VALUE hash)
{
    if (key == Qundef) return ST_CONTINUE;
    if (!RTEST(rb_yield_values(2, key, value))) {
	return ST_DELETE;
    }
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.select! {| key, value | block }  -> hsh or nil
 *     hsh.select!                          -> an_enumerator
 *
 *  Equivalent to <code>Hash#keep_if</code>, but returns
 *  <code>nil</code> if no changes were made.
 */

VALUE
rb_hash_select_bang(VALUE hash)
{
    st_index_t n;

    RETURN_ENUMERATOR(hash, 0, 0);
    rb_hash_modify(hash);
    if (!RHASH(hash)->ntbl)
        return Qnil;
    n = RHASH(hash)->ntbl->num_entries;
    rb_hash_foreach(hash, keep_if_i, hash);
    if (n == RHASH(hash)->ntbl->num_entries) return Qnil;
    return hash;
}

/*
 *  call-seq:
 *     hsh.keep_if {| key, value | block }  -> hsh
 *     hsh.keep_if                          -> an_enumerator
 *
 *  Deletes every key-value pair from <i>hsh</i> for which <i>block</i>
 *  evaluates to false.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 */

VALUE
rb_hash_keep_if(VALUE hash)
{
    RETURN_ENUMERATOR(hash, 0, 0);
    rb_hash_modify(hash);
    rb_hash_foreach(hash, keep_if_i, hash);
    return hash;
}

static int
clear_i(VALUE key, VALUE value, VALUE dummy)
{
    return ST_DELETE;
}

/*
 *  call-seq:
 *     hsh.clear -> hsh
 *
 *  Removes all key-value pairs from <i>hsh</i>.
 *
 *     h = { "a" => 100, "b" => 200 }   #=> {"a"=>100, "b"=>200}
 *     h.clear                          #=> {}
 *
 */

static VALUE
rb_hash_clear(VALUE hash)
{
    rb_hash_modify_check(hash);
    if (!RHASH(hash)->ntbl)
        return hash;
    if (RHASH(hash)->ntbl->num_entries > 0) {
	if (RHASH(hash)->iter_lev > 0)
	    rb_hash_foreach(hash, clear_i, 0);
	else
	    st_clear(RHASH(hash)->ntbl);
    }

    return hash;
}

static st_data_t
copy_str_key(st_data_t str)
{
    return (st_data_t)rb_str_new4((VALUE)str);
}

/*
 *  call-seq:
 *     hsh[key] = value        -> value
 *     hsh.store(key, value)   -> value
 *
 *  Element Assignment---Associates the value given by
 *  <i>value</i> with the key given by <i>key</i>.
 *  <i>key</i> should not have its value changed while it is in
 *  use as a key (a <code>String</code> passed as a key will be
 *  duplicated and frozen).
 *
 *     h = { "a" => 100, "b" => 200 }
 *     h["a"] = 9
 *     h["c"] = 4
 *     h   #=> {"a"=>9, "b"=>200, "c"=>4}
 *
 */

VALUE
rb_hash_aset(VALUE hash, VALUE key, VALUE val)
{
    rb_hash_modify(hash);
    hash_update(hash, key);
    if (RHASH(hash)->ntbl->type == &identhash || rb_obj_class(key) != rb_cString) {
	st_insert(RHASH(hash)->ntbl, key, val);
    }
    else {
	st_insert2(RHASH(hash)->ntbl, key, val, copy_str_key);
    }
    return val;
}

static int
replace_i(VALUE key, VALUE val, VALUE hash)
{
    if (key != Qundef) {
	rb_hash_aset(hash, key, val);
    }

    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.replace(other_hash) -> hsh
 *
 *  Replaces the contents of <i>hsh</i> with the contents of
 *  <i>other_hash</i>.
 *
 *     h = { "a" => 100, "b" => 200 }
 *     h.replace({ "c" => 300, "d" => 400 })   #=> {"c"=>300, "d"=>400}
 *
 */

static VALUE
rb_hash_replace(VALUE hash, VALUE hash2)
{
    rb_hash_modify_check(hash);
    hash2 = to_hash(hash2);
    if (hash == hash2) return hash;
    rb_hash_clear(hash);
    if (RHASH(hash2)->ntbl) {
	rb_hash_tbl(hash);
	RHASH(hash)->ntbl->type = RHASH(hash2)->ntbl->type;
    }
    rb_hash_foreach(hash2, replace_i, hash);
    RHASH_IFNONE(hash) = RHASH_IFNONE(hash2);
    if (FL_TEST(hash2, HASH_PROC_DEFAULT)) {
	FL_SET(hash, HASH_PROC_DEFAULT);
    }
    else {
	FL_UNSET(hash, HASH_PROC_DEFAULT);
    }

    return hash;
}

/*
 *  call-seq:
 *     hsh.length    ->  fixnum
 *     hsh.size      ->  fixnum
 *
 *  Returns the number of key-value pairs in the hash.
 *
 *     h = { "d" => 100, "a" => 200, "v" => 300, "e" => 400 }
 *     h.length        #=> 4
 *     h.delete("a")   #=> 200
 *     h.length        #=> 3
 */

static VALUE
rb_hash_size(VALUE hash)
{
    if (!RHASH(hash)->ntbl)
        return INT2FIX(0);
    return INT2FIX(RHASH(hash)->ntbl->num_entries);
}


/*
 *  call-seq:
 *     hsh.empty?    -> true or false
 *
 *  Returns <code>true</code> if <i>hsh</i> contains no key-value pairs.
 *
 *     {}.empty?   #=> true
 *
 */

static VALUE
rb_hash_empty_p(VALUE hash)
{
    return RHASH_EMPTY_P(hash) ? Qtrue : Qfalse;
}

static int
each_value_i(VALUE key, VALUE value)
{
    if (key == Qundef) return ST_CONTINUE;
    rb_yield(value);
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.each_value {| value | block } -> hsh
 *     hsh.each_value                    -> an_enumerator
 *
 *  Calls <i>block</i> once for each key in <i>hsh</i>, passing the
 *  value as a parameter.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     h = { "a" => 100, "b" => 200 }
 *     h.each_value {|value| puts value }
 *
 *  <em>produces:</em>
 *
 *     100
 *     200
 */

static VALUE
rb_hash_each_value(VALUE hash)
{
    RETURN_ENUMERATOR(hash, 0, 0);
    rb_hash_foreach(hash, each_value_i, 0);
    return hash;
}

static int
each_key_i(VALUE key, VALUE value)
{
    if (key == Qundef) return ST_CONTINUE;
    rb_yield(key);
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.each_key {| key | block } -> hsh
 *     hsh.each_key                  -> an_enumerator
 *
 *  Calls <i>block</i> once for each key in <i>hsh</i>, passing the key
 *  as a parameter.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     h = { "a" => 100, "b" => 200 }
 *     h.each_key {|key| puts key }
 *
 *  <em>produces:</em>
 *
 *     a
 *     b
 */
static VALUE
rb_hash_each_key(VALUE hash)
{
    RETURN_ENUMERATOR(hash, 0, 0);
    rb_hash_foreach(hash, each_key_i, 0);
    return hash;
}

static int
each_pair_i(VALUE key, VALUE value)
{
    if (key == Qundef) return ST_CONTINUE;
    rb_yield(rb_assoc_new(key, value));
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.each      {| key, value | block } -> hsh
 *     hsh.each_pair {| key, value | block } -> hsh
 *     hsh.each                              -> an_enumerator
 *     hsh.each_pair                         -> an_enumerator
 *
 *  Calls <i>block</i> once for each key in <i>hsh</i>, passing the key-value
 *  pair as parameters.
 *
 *  If no block is given, an enumerator is returned instead.
 *
 *     h = { "a" => 100, "b" => 200 }
 *     h.each {|key, value| puts "#{key} is #{value}" }
 *
 *  <em>produces:</em>
 *
 *     a is 100
 *     b is 200
 *
 */

static VALUE
rb_hash_each_pair(VALUE hash)
{
    RETURN_ENUMERATOR(hash, 0, 0);
    rb_hash_foreach(hash, each_pair_i, 0);
    return hash;
}

static int
to_a_i(VALUE key, VALUE value, VALUE ary)
{
    if (key == Qundef) return ST_CONTINUE;
    rb_ary_push(ary, rb_assoc_new(key, value));
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.to_a -> array
 *
 *  Converts <i>hsh</i> to a nested array of <code>[</code> <i>key,
 *  value</i> <code>]</code> arrays.
 *
 *     h = { "c" => 300, "a" => 100, "d" => 400, "c" => 300  }
 *     h.to_a   #=> [["c", 300], ["a", 100], ["d", 400]]
 */

static VALUE
rb_hash_to_a(VALUE hash)
{
    VALUE ary;

    ary = rb_ary_new();
    rb_hash_foreach(hash, to_a_i, ary);
    OBJ_INFECT(ary, hash);

    return ary;
}

static int
inspect_i(VALUE key, VALUE value, VALUE str)
{
    VALUE str2;

    if (key == Qundef) return ST_CONTINUE;
    str2 = rb_inspect(key);
    if (RSTRING_LEN(str) > 1) {
	rb_str_cat2(str, ", ");
    }
    else {
	rb_enc_copy(str, str2);
    }
    rb_str_buf_append(str, str2);
    OBJ_INFECT(str, str2);
    rb_str_buf_cat2(str, "=>");
    str2 = rb_inspect(value);
    rb_str_buf_append(str, str2);
    OBJ_INFECT(str, str2);

    return ST_CONTINUE;
}

static VALUE
inspect_hash(VALUE hash, VALUE dummy, int recur)
{
    VALUE str;

    if (recur) return rb_usascii_str_new2("{...}");
    str = rb_str_buf_new2("{");
    rb_hash_foreach(hash, inspect_i, str);
    rb_str_buf_cat2(str, "}");
    OBJ_INFECT(str, hash);

    return str;
}

/*
 * call-seq:
 *   hsh.to_s     -> string
 *   hsh.inspect  -> string
 *
 * Return the contents of this hash as a string.
 *
 *     h = { "c" => 300, "a" => 100, "d" => 400, "c" => 300  }
 *     h.to_s   #=> "{\"c\"=>300, \"a\"=>100, \"d\"=>400}"
 */

static VALUE
rb_hash_inspect(VALUE hash)
{
    if (RHASH_EMPTY_P(hash))
	return rb_usascii_str_new2("{}");
    return rb_exec_recursive(inspect_hash, hash, 0);
}

/*
 * call-seq:
 *    hsh.to_hash   => hsh
 *
 * Returns +self+.
 */

static VALUE
rb_hash_to_hash(VALUE hash)
{
    return hash;
}

static int
keys_i(VALUE key, VALUE value, VALUE ary)
{
    if (key == Qundef) return ST_CONTINUE;
    rb_ary_push(ary, key);
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.keys    -> array
 *
 *  Returns a new array populated with the keys from this hash. See also
 *  <code>Hash#values</code>.
 *
 *     h = { "a" => 100, "b" => 200, "c" => 300, "d" => 400 }
 *     h.keys   #=> ["a", "b", "c", "d"]
 *
 */

static VALUE
rb_hash_keys(VALUE hash)
{
    VALUE ary;

    ary = rb_ary_new();
    rb_hash_foreach(hash, keys_i, ary);

    return ary;
}

static int
values_i(VALUE key, VALUE value, VALUE ary)
{
    if (key == Qundef) return ST_CONTINUE;
    rb_ary_push(ary, value);
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.values    -> array
 *
 *  Returns a new array populated with the values from <i>hsh</i>. See
 *  also <code>Hash#keys</code>.
 *
 *     h = { "a" => 100, "b" => 200, "c" => 300 }
 *     h.values   #=> [100, 200, 300]
 *
 */

static VALUE
rb_hash_values(VALUE hash)
{
    VALUE ary;

    ary = rb_ary_new();
    rb_hash_foreach(hash, values_i, ary);

    return ary;
}

/*
 *  call-seq:
 *     hsh.has_key?(key)    -> true or false
 *     hsh.include?(key)    -> true or false
 *     hsh.key?(key)        -> true or false
 *     hsh.member?(key)     -> true or false
 *
 *  Returns <code>true</code> if the given key is present in <i>hsh</i>.
 *
 *     h = { "a" => 100, "b" => 200 }
 *     h.has_key?("a")   #=> true
 *     h.has_key?("z")   #=> false
 *
 */

static VALUE
rb_hash_has_key(VALUE hash, VALUE key)
{
    if (!RHASH(hash)->ntbl)
        return Qfalse;
    if (st_lookup(RHASH(hash)->ntbl, key, 0)) {
	return Qtrue;
    }
    return Qfalse;
}

static int
rb_hash_search_value(VALUE key, VALUE value, VALUE arg)
{
    VALUE *data = (VALUE *)arg;

    if (key == Qundef) return ST_CONTINUE;
    if (rb_equal(value, data[1])) {
	data[0] = Qtrue;
	return ST_STOP;
    }
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.has_value?(value)    -> true or false
 *     hsh.value?(value)        -> true or false
 *
 *  Returns <code>true</code> if the given value is present for some key
 *  in <i>hsh</i>.
 *
 *     h = { "a" => 100, "b" => 200 }
 *     h.has_value?(100)   #=> true
 *     h.has_value?(999)   #=> false
 */

static VALUE
rb_hash_has_value(VALUE hash, VALUE val)
{
    VALUE data[2];

    data[0] = Qfalse;
    data[1] = val;
    rb_hash_foreach(hash, rb_hash_search_value, (VALUE)data);
    return data[0];
}

struct equal_data {
    VALUE result;
    st_table *tbl;
    int eql;
};

static int
eql_i(VALUE key, VALUE val1, VALUE arg)
{
    struct equal_data *data = (struct equal_data *)arg;
    st_data_t val2;

    if (key == Qundef) return ST_CONTINUE;
    if (!st_lookup(data->tbl, key, &val2)) {
	data->result = Qfalse;
	return ST_STOP;
    }
    if (!(data->eql ? rb_eql(val1, (VALUE)val2) : (int)rb_equal(val1, (VALUE)val2))) {
	data->result = Qfalse;
	return ST_STOP;
    }
    return ST_CONTINUE;
}

static VALUE
recursive_eql(VALUE hash, VALUE dt, int recur)
{
    struct equal_data *data;

    if (recur) return Qtrue;	/* Subtle! */
    data = (struct equal_data*)dt;
    data->result = Qtrue;
    rb_hash_foreach(hash, eql_i, dt);

    return data->result;
}

static VALUE
hash_equal(VALUE hash1, VALUE hash2, int eql)
{
    struct equal_data data;

    if (hash1 == hash2) return Qtrue;
    if (!RB_TYPE_P(hash2, T_HASH)) {
	if (!rb_respond_to(hash2, rb_intern("to_hash"))) {
	    return Qfalse;
	}
	if (eql)
	    return rb_eql(hash2, hash1);
	else
	    return rb_equal(hash2, hash1);
    }
    if (RHASH_SIZE(hash1) != RHASH_SIZE(hash2))
	return Qfalse;
    if (!RHASH(hash1)->ntbl || !RHASH(hash2)->ntbl)
        return Qtrue;
    if (RHASH(hash1)->ntbl->type != RHASH(hash2)->ntbl->type)
	return Qfalse;
#if 0
    if (!(rb_equal(RHASH_IFNONE(hash1), RHASH_IFNONE(hash2)) &&
	  FL_TEST(hash1, HASH_PROC_DEFAULT) == FL_TEST(hash2, HASH_PROC_DEFAULT)))
	return Qfalse;
#endif

    data.tbl = RHASH(hash2)->ntbl;
    data.eql = eql;
    return rb_exec_recursive_paired(recursive_eql, hash1, hash2, (VALUE)&data);
}

/*
 *  call-seq:
 *     hsh == other_hash    -> true or false
 *
 *  Equality---Two hashes are equal if they each contain the same number
 *  of keys and if each key-value pair is equal to (according to
 *  <code>Object#==</code>) the corresponding elements in the other
 *  hash.
 *
 *     h1 = { "a" => 1, "c" => 2 }
 *     h2 = { 7 => 35, "c" => 2, "a" => 1 }
 *     h3 = { "a" => 1, "c" => 2, 7 => 35 }
 *     h4 = { "a" => 1, "d" => 2, "f" => 35 }
 *     h1 == h2   #=> false
 *     h2 == h3   #=> true
 *     h3 == h4   #=> false
 *
 */

static VALUE
rb_hash_equal(VALUE hash1, VALUE hash2)
{
    return hash_equal(hash1, hash2, FALSE);
}

/*
 *  call-seq:
 *     hash.eql?(other)  -> true or false
 *
 *  Returns <code>true</code> if <i>hash</i> and <i>other</i> are
 *  both hashes with the same content.
 */

static VALUE
rb_hash_eql(VALUE hash1, VALUE hash2)
{
    return hash_equal(hash1, hash2, TRUE);
}

static int
hash_i(VALUE key, VALUE val, VALUE arg)
{
    st_index_t *hval = (st_index_t *)arg;
    st_index_t hdata[2];

    if (key == Qundef) return ST_CONTINUE;
    hdata[0] = rb_hash(key);
    hdata[1] = rb_hash(val);
    *hval ^= st_hash(hdata, sizeof(hdata), 0);
    return ST_CONTINUE;
}

static VALUE
recursive_hash(VALUE hash, VALUE dummy, int recur)
{
    st_index_t hval;

    if (!RHASH(hash)->ntbl)
        return LONG2FIX(0);
    hval = RHASH(hash)->ntbl->num_entries;
    if (!hval) return LONG2FIX(0);
    if (recur)
	hval = rb_hash_uint(rb_hash_start(rb_hash(rb_cHash)), hval);
    else
	rb_hash_foreach(hash, hash_i, (VALUE)&hval);
    hval = rb_hash_end(hval);
    return INT2FIX(hval);
}

/*
 *  call-seq:
 *     hsh.hash   -> fixnum
 *
 *  Compute a hash-code for this hash. Two hashes with the same content
 *  will have the same hash code (and will compare using <code>eql?</code>).
 */

static VALUE
rb_hash_hash(VALUE hash)
{
    return rb_exec_recursive_outer(recursive_hash, hash, 0);
}

static int
rb_hash_invert_i(VALUE key, VALUE value, VALUE hash)
{
    if (key == Qundef) return ST_CONTINUE;
    rb_hash_aset(hash, value, key);
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.invert -> new_hash
 *
 *  Returns a new hash created by using <i>hsh</i>'s values as keys, and
 *  the keys as values.
 *
 *     h = { "n" => 100, "m" => 100, "y" => 300, "d" => 200, "a" => 0 }
 *     h.invert   #=> {0=>"a", 100=>"m", 200=>"d", 300=>"y"}
 *
 */

static VALUE
rb_hash_invert(VALUE hash)
{
    VALUE h = rb_hash_new();

    rb_hash_foreach(hash, rb_hash_invert_i, h);
    return h;
}

static int
rb_hash_update_i(VALUE key, VALUE value, VALUE hash)
{
    if (key == Qundef) return ST_CONTINUE;
    hash_update(hash, key);
    st_insert(RHASH(hash)->ntbl, key, value);
    return ST_CONTINUE;
}

static int
rb_hash_update_block_i(VALUE key, VALUE value, VALUE hash)
{
    if (key == Qundef) return ST_CONTINUE;
    if (rb_hash_has_key(hash, key)) {
	value = rb_yield_values(3, key, rb_hash_aref(hash, key), value);
    }
    hash_update(hash, key);
    st_insert(RHASH(hash)->ntbl, key, value);
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hsh.merge!(other_hash)                                 -> hsh
 *     hsh.update(other_hash)                                 -> hsh
 *     hsh.merge!(other_hash){|key, oldval, newval| block}    -> hsh
 *     hsh.update(other_hash){|key, oldval, newval| block}    -> hsh
 *
 *  Adds the contents of <i>other_hash</i> to <i>hsh</i>.  If no
 *  block is specified, entries with duplicate keys are overwritten
 *  with the values from <i>other_hash</i>, otherwise the value
 *  of each duplicate key is determined by calling the block with
 *  the key, its value in <i>hsh</i> and its value in <i>other_hash</i>.
 *
 *     h1 = { "a" => 100, "b" => 200 }
 *     h2 = { "b" => 254, "c" => 300 }
 *     h1.merge!(h2)   #=> {"a"=>100, "b"=>254, "c"=>300}
 *
 *     h1 = { "a" => 100, "b" => 200 }
 *     h2 = { "b" => 254, "c" => 300 }
 *     h1.merge!(h2) { |key, v1, v2| v1 }
 *                     #=> {"a"=>100, "b"=>200, "c"=>300}
 */

static VALUE
rb_hash_update(VALUE hash1, VALUE hash2)
{
    rb_hash_modify(hash1);
    hash2 = to_hash(hash2);
    if (rb_block_given_p()) {
	rb_hash_foreach(hash2, rb_hash_update_block_i, hash1);
    }
    else {
	rb_hash_foreach(hash2, rb_hash_update_i, hash1);
    }
    return hash1;
}

struct update_arg {
    VALUE hash;
    rb_hash_update_func *func;
};

static int
rb_hash_update_func_i(VALUE key, VALUE value, VALUE arg0)
{
    struct update_arg *arg = (struct update_arg *)arg0;
    VALUE hash = arg->hash;

    if (key == Qundef) return ST_CONTINUE;
    if (rb_hash_has_key(hash, key)) {
	value = (*arg->func)(key, rb_hash_aref(hash, key), value);
    }
    hash_update(hash, key);
    st_insert(RHASH(hash)->ntbl, key, value);
    return ST_CONTINUE;
}

VALUE
rb_hash_update_by(VALUE hash1, VALUE hash2, rb_hash_update_func *func)
{
    rb_hash_modify(hash1);
    hash2 = to_hash(hash2);
    if (func) {
	struct update_arg arg;
	arg.hash = hash1;
	arg.func = func;
	rb_hash_foreach(hash2, rb_hash_update_func_i, (VALUE)&arg);
    }
    else {
	rb_hash_foreach(hash2, rb_hash_update_i, hash1);
    }
    return hash1;
}

/*
 *  call-seq:
 *     hsh.merge(other_hash)                              -> new_hash
 *     hsh.merge(other_hash){|key, oldval, newval| block} -> new_hash
 *
 *  Returns a new hash containing the contents of <i>other_hash</i> and
 *  the contents of <i>hsh</i>. If no block is specified, the value for
 *  entries with duplicate keys will be that of <i>other_hash</i>. Otherwise
 *  the value for each duplicate key is determined by calling the block
 *  with the key, its value in <i>hsh</i> and its value in <i>other_hash</i>.
 *
 *     h1 = { "a" => 100, "b" => 200 }
 *     h2 = { "b" => 254, "c" => 300 }
 *     h1.merge(h2)   #=> {"a"=>100, "b"=>254, "c"=>300}
 *     h1.merge(h2){|key, oldval, newval| newval - oldval}
 *                    #=> {"a"=>100, "b"=>54,  "c"=>300}
 *     h1             #=> {"a"=>100, "b"=>200}
 *
 */

static VALUE
rb_hash_merge(VALUE hash1, VALUE hash2)
{
    return rb_hash_update(rb_obj_dup(hash1), hash2);
}

static int
assoc_i(VALUE key, VALUE val, VALUE arg)
{
    VALUE *args = (VALUE *)arg;

    if (key == Qundef) return ST_CONTINUE;
    if (RTEST(rb_equal(args[0], key))) {
	args[1] = rb_assoc_new(key, val);
	return ST_STOP;
    }
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hash.assoc(obj)   ->  an_array  or  nil
 *
 *  Searches through the hash comparing _obj_ with the key using <code>==</code>.
 *  Returns the key-value pair (two elements array) or +nil+
 *  if no match is found.  See <code>Array#assoc</code>.
 *
 *     h = {"colors"  => ["red", "blue", "green"],
 *          "letters" => ["a", "b", "c" ]}
 *     h.assoc("letters")  #=> ["letters", ["a", "b", "c"]]
 *     h.assoc("foo")      #=> nil
 */

VALUE
rb_hash_assoc(VALUE hash, VALUE obj)
{
    VALUE args[2];

    args[0] = obj;
    args[1] = Qnil;
    rb_hash_foreach(hash, assoc_i, (VALUE)args);
    return args[1];
}

static int
rassoc_i(VALUE key, VALUE val, VALUE arg)
{
    VALUE *args = (VALUE *)arg;

    if (key == Qundef) return ST_CONTINUE;
    if (RTEST(rb_equal(args[0], val))) {
	args[1] = rb_assoc_new(key, val);
	return ST_STOP;
    }
    return ST_CONTINUE;
}

/*
 *  call-seq:
 *     hash.rassoc(obj) -> an_array or nil
 *
 *  Searches through the hash comparing _obj_ with the value using <code>==</code>.
 *  Returns the first key-value pair (two-element array) that matches. See
 *  also <code>Array#rassoc</code>.
 *
 *     a = {1=> "one", 2 => "two", 3 => "three", "ii" => "two"}
 *     a.rassoc("two")    #=> [2, "two"]
 *     a.rassoc("four")   #=> nil
 */

VALUE
rb_hash_rassoc(VALUE hash, VALUE obj)
{
    VALUE args[2];

    args[0] = obj;
    args[1] = Qnil;
    rb_hash_foreach(hash, rassoc_i, (VALUE)args);
    return args[1];
}

/*
 *  call-seq:
 *     hash.flatten -> an_array
 *     hash.flatten(level) -> an_array
 *
 *  Returns a new array that is a one-dimensional flattening of this
 *  hash. That is, for every key or value that is an array, extract
 *  its elements into the new array.  Unlike Array#flatten, this
 *  method does not flatten recursively by default.  The optional
 *  <i>level</i> argument determines the level of recursion to flatten.
 *
 *     a =  {1=> "one", 2 => [2,"two"], 3 => "three"}
 *     a.flatten    # => [1, "one", 2, [2, "two"], 3, "three"]
 *     a.flatten(2) # => [1, "one", 2, 2, "two", 3, "three"]
 */

static VALUE
rb_hash_flatten(int argc, VALUE *argv, VALUE hash)
{
    VALUE ary, tmp;

    ary = rb_hash_to_a(hash);
    if (argc == 0) {
	argc = 1;
	tmp = INT2FIX(1);
	argv = &tmp;
    }
    rb_funcall2(ary, rb_intern("flatten!"), argc, argv);
    return ary;
}

/*
 *  call-seq:
 *     hsh.compare_by_identity -> hsh
 *
 *  Makes <i>hsh</i> compare its keys by their identity, i.e. it
 *  will consider exact same objects as same keys.
 *
 *     h1 = { "a" => 100, "b" => 200, :c => "c" }
 *     h1["a"]        #=> 100
 *     h1.compare_by_identity
 *     h1.compare_by_identity? #=> true
 *     h1["a"]        #=> nil  # different objects.
 *     h1[:c]         #=> "c"  # same symbols are all same.
 *
 */

static VALUE
rb_hash_compare_by_id(VALUE hash)
{
    rb_hash_modify(hash);
    RHASH(hash)->ntbl->type = &identhash;
    rb_hash_rehash(hash);
    return hash;
}

/*
 *  call-seq:
 *     hsh.compare_by_identity? -> true or false
 *
 *  Returns <code>true</code> if <i>hsh</i> will compare its keys by
 *  their identity.  Also see <code>Hash#compare_by_identity</code>.
 *
 */

static VALUE
rb_hash_compare_by_id_p(VALUE hash)
{
    if (!RHASH(hash)->ntbl)
        return Qfalse;
    if (RHASH(hash)->ntbl->type == &identhash) {
	return Qtrue;
    }
    return Qfalse;
}

static int path_tainted = -1;

static char **origenviron;
#ifdef _WIN32
#define GET_ENVIRON(e) ((e) = rb_w32_get_environ())
#define FREE_ENVIRON(e) rb_w32_free_environ(e)
static char **my_environ;
#undef environ
#define environ my_environ
#undef getenv
#define getenv(n) rb_w32_ugetenv(n)
#elif defined(__APPLE__)
#undef environ
#define environ (*_NSGetEnviron())
#define GET_ENVIRON(e) (e)
#define FREE_ENVIRON(e)
#else
extern char **environ;
#define GET_ENVIRON(e) (e)
#define FREE_ENVIRON(e)
#endif
#ifdef ENV_IGNORECASE
#define ENVMATCH(s1, s2) (STRCASECMP((s1), (s2)) == 0)
#define ENVNMATCH(s1, s2, n) (STRNCASECMP((s1), (s2), (n)) == 0)
#else
#define ENVMATCH(n1, n2) (strcmp((n1), (n2)) == 0)
#define ENVNMATCH(s1, s2, n) (memcmp((s1), (s2), (n)) == 0)
#endif

static VALUE
env_str_new(const char *ptr, long len)
{
#ifdef _WIN32
    VALUE str = rb_str_conv_enc(rb_str_new(ptr, len), rb_utf8_encoding(), rb_locale_encoding());
#else
    VALUE str = rb_locale_str_new(ptr, len);
#endif

    rb_obj_freeze(str);
    return str;
}

static VALUE
env_str_new2(const char *ptr)
{
    if (!ptr) return Qnil;
    return env_str_new(ptr, strlen(ptr));
}

static VALUE
env_delete(VALUE obj, VALUE name)
{
    char *nam, *val;

    rb_secure(4);
    SafeStringValue(name);
    nam = RSTRING_PTR(name);
    if (memchr(nam, '\0', RSTRING_LEN(name))) {
	rb_raise(rb_eArgError, "bad environment variable name");
    }
    val = getenv(nam);
    if (val) {
	VALUE value = env_str_new2(val);

	ruby_setenv(nam, 0);
	if (ENVMATCH(nam, PATH_ENV)) {
	    path_tainted = 0;
	}
	return value;
    }
    return Qnil;
}

/*
 * call-seq:
 *   ENV.delete(name)            -> value
 *   ENV.delete(name) { |name| } -> value
 *
 * Deletes the environment variable with +name+ and returns the value of the
 * variable.  If a block is given it will be called when the named environment
 * does not exist.
 */
static VALUE
env_delete_m(VALUE obj, VALUE name)
{
    VALUE val;

    val = env_delete(obj, name);
    if (NIL_P(val) && rb_block_given_p()) rb_yield(name);
    return val;
}

static int env_path_tainted(const char *);

/*
 * call-seq:
 *   ENV[name] -> value
 *
 * Retrieves the +value+ for environment variable +name+ as a String.  Returns
 * +nil+ if the named variable does not exist.
 */
static VALUE
rb_f_getenv(VALUE obj, VALUE name)
{
    char *nam, *env;

    rb_secure(4);
    SafeStringValue(name);
    nam = RSTRING_PTR(name);
    if (memchr(nam, '\0', RSTRING_LEN(name))) {
	rb_raise(rb_eArgError, "bad environment variable name");
    }
    env = getenv(nam);
    if (env) {
	if (ENVMATCH(nam, PATH_ENV) && !env_path_tainted(env)) {
#ifdef _WIN32
	    VALUE str = rb_str_conv_enc(rb_str_new(env, strlen(env)), rb_utf8_encoding(), rb_filesystem_encoding());
#else
	    VALUE str = rb_filesystem_str_new_cstr(env);
#endif

	    rb_obj_freeze(str);
	    return str;
	}
	return env_str_new2(env);
    }
    return Qnil;
}

/*
 * :yield: missing_name
 * call-seq:
 *   ENV.fetch(name)                        -> value
 *   ENV.fetch(name, default)               -> value
 *   ENV.fetch(name) { |missing_name| ... } -> value
 *
 * Retrieves the environment variable +name+.
 *
 * If the given name does not exist and neither +default+ nor a block a
 * provided an IndexError is raised.  If a block is given it is called with
 * the missing name to provide a value.  If a default value is given it will
 * be returned when no block is given.
 */
static VALUE
env_fetch(int argc, VALUE *argv)
{
    VALUE key, if_none;
    long block_given;
    char *nam, *env;

    rb_secure(4);
    rb_scan_args(argc, argv, "11", &key, &if_none);
    block_given = rb_block_given_p();
    if (block_given && argc == 2) {
	rb_warn("block supersedes default value argument");
    }
    SafeStringValue(key);
    nam = RSTRING_PTR(key);
    if (memchr(nam, '\0', RSTRING_LEN(key))) {
	rb_raise(rb_eArgError, "bad environment variable name");
    }
    env = getenv(nam);
    if (!env) {
	if (block_given) return rb_yield(key);
	if (argc == 1) {
	    rb_raise(rb_eKeyError, "key not found");
	}
	return if_none;
    }
    if (ENVMATCH(nam, PATH_ENV) && !env_path_tainted(env))
#ifdef _WIN32
	return rb_str_conv_enc(rb_str_new(env, strlen(env)), rb_utf8_encoding(), rb_filesystem_encoding());
#else
	return rb_filesystem_str_new_cstr(env);
#endif
    return env_str_new2(env);
}

static void
path_tainted_p(const char *path)
{
    path_tainted = rb_path_check(path)?0:1;
}

static int
env_path_tainted(const char *path)
{
    if (path_tainted < 0) {
	path_tainted_p(path);
    }
    return path_tainted;
}

int
rb_env_path_tainted(void)
{
    if (path_tainted < 0) {
	path_tainted_p(getenv(PATH_ENV));
    }
    return path_tainted;
}

#if defined(_WIN32) || (defined(HAVE_SETENV) && defined(HAVE_UNSETENV))
#elif defined __sun
static int
in_origenv(const char *str)
{
    char **env;
    for (env = origenviron; *env; ++env) {
	if (*env == str) return 1;
    }
    return 0;
}
#else
static int
envix(const char *nam)
{
    register int i, len = strlen(nam);
    char **env;

    env = GET_ENVIRON(environ);
    for (i = 0; env[i]; i++) {
	if (ENVNMATCH(env[i],nam,len) && env[i][len] == '=')
	    break;			/* memcmp must come first to avoid */
    }					/* potential SEGV's */
    FREE_ENVIRON(environ);
    return i;
}
#endif

#if defined(_WIN32)
static size_t
getenvsize(const char* p)
{
    const char* porg = p;
    while (*p++) p += strlen(p) + 1;
    return p - porg + 1;
}
static size_t
getenvblocksize()
{
    return (rb_w32_osver() >= 5) ? 32767 : 5120;
}
#endif

void
ruby_setenv(const char *name, const char *value)
{
#if defined(_WIN32)
    VALUE buf;
    int failed = 0;
    if (strchr(name, '=')) {
      fail:
	errno = EINVAL;
	rb_sys_fail("ruby_setenv");
    }
    if (value) {
	const char* p = GetEnvironmentStringsA();
	if (!p) goto fail; /* never happen */
	if (strlen(name) + 2 + strlen(value) + getenvsize(p) >= getenvblocksize()) {
	    goto fail;  /* 2 for '=' & '\0' */
	}
	buf = rb_sprintf("%s=%s", name, value);
    }
    else {
	buf = rb_sprintf("%s=", name);
    }
    failed = putenv(RSTRING_PTR(buf));
    /* even if putenv() failed, clean up and try to delete the
     * variable from the system area. */
    rb_str_resize(buf, 0);
    if (!value || !*value) {
	/* putenv() doesn't handle empty value */
	if (!SetEnvironmentVariable(name, value) &&
	    GetLastError() != ERROR_ENVVAR_NOT_FOUND) goto fail;
    }
    if (failed) goto fail;
#elif defined(HAVE_SETENV) && defined(HAVE_UNSETENV)
#undef setenv
#undef unsetenv
    if (value) {
	if (setenv(name, value, 1))
	    rb_sys_fail("setenv");
    } else {
#ifdef VOID_UNSETENV
	unsetenv(name);
#else
	if (unsetenv(name))
	    rb_sys_fail("unsetenv");
#endif
    }
#elif defined __sun
    size_t len;
    char **env_ptr, *str;
    if (strchr(name, '=')) {
	errno = EINVAL;
	rb_sys_fail("ruby_setenv");
    }
    len = strlen(name);
    for (env_ptr = GET_ENVIRON(environ); (str = *env_ptr) != 0; ++env_ptr) {
	if (!strncmp(str, name, len) && str[len] == '=') {
	    if (!in_origenv(str)) free(str);
	    while ((env_ptr[0] = env_ptr[1]) != 0) env_ptr++;
	    break;
	}
    }
    if (value) {
	str = malloc(len += strlen(value) + 2);
	snprintf(str, len, "%s=%s", name, value);
	if (putenv(str))
	    rb_sys_fail("putenv");
    }
#else  /* WIN32 */
    size_t len;
    int i;
    if (strchr(name, '=')) {
	errno = EINVAL;
	rb_sys_fail("ruby_setenv");
    }
    i=envix(name);		        /* where does it go? */

    if (environ == origenviron) {	/* need we copy environment? */
	int j;
	int max;
	char **tmpenv;

	for (max = i; environ[max]; max++) ;
	tmpenv = ALLOC_N(char*, max+2);
	for (j=0; j<max; j++)		/* copy environment */
	    tmpenv[j] = ruby_strdup(environ[j]);
	tmpenv[max] = 0;
	environ = tmpenv;		/* tell exec where it is now */
    }
    if (environ[i]) {
	char **envp = origenviron;
	while (*envp && *envp != environ[i]) envp++;
	if (!*envp)
	    xfree(environ[i]);
	if (!value) {
	    while (environ[i]) {
		environ[i] = environ[i+1];
		i++;
	    }
	    return;
	}
    }
    else {			/* does not exist yet */
	if (!value) return;
	REALLOC_N(environ, char*, i+2);	/* just expand it a bit */
	environ[i+1] = 0;	/* make sure it's null terminated */
    }
    len = strlen(name) + strlen(value) + 2;
    environ[i] = ALLOC_N(char, len);
    snprintf(environ[i],len,"%s=%s",name,value); /* all that work just for this */
#endif /* WIN32 */
}

void
ruby_unsetenv(const char *name)
{
    ruby_setenv(name, 0);
}

/*
 * call-seq:
 *   ENV[name] = value
 *   ENV.store(name, value) -> value
 *
 * Sets the environment variable +name+ to +value+.  If the value given is
 * +nil+ the environment variable is deleted.
 *
 */
static VALUE
env_aset(VALUE obj, VALUE nm, VALUE val)
{
    char *name, *value;

    if (rb_safe_level() >= 4) {
	rb_raise(rb_eSecurityError, "can't change environment variable");
    }

    if (NIL_P(val)) {
	env_delete(obj, nm);
	return Qnil;
    }
    StringValue(nm);
    StringValue(val);
    name = RSTRING_PTR(nm);
    value = RSTRING_PTR(val);
    if (memchr(name, '\0', RSTRING_LEN(nm)))
	rb_raise(rb_eArgError, "bad environment variable name");
    if (memchr(value, '\0', RSTRING_LEN(val)))
	rb_raise(rb_eArgError, "bad environment variable value");

    ruby_setenv(name, value);
    if (ENVMATCH(name, PATH_ENV)) {
	if (OBJ_TAINTED(val)) {
	    /* already tainted, no check */
	    path_tainted = 1;
	    return val;
	}
	else {
	    path_tainted_p(value);
	}
    }
    return val;
}

/*
 * call-seq:
 *   ENV.keys -> Array
 *
 * Returns every environment variable name in an Array
 */
static VALUE
env_keys(void)
{
    char **env;
    VALUE ary;

    rb_secure(4);
    ary = rb_ary_new();
    env = GET_ENVIRON(environ);
    while (*env) {
	char *s = strchr(*env, '=');
	if (s) {
	    rb_ary_push(ary, env_str_new(*env, s-*env));
	}
	env++;
    }
    FREE_ENVIRON(environ);
    return ary;
}

/*
 * call-seq:
 *   ENV.each_key { |name| } -> Hash
 *   ENV.each_key            -> Enumerator
 *
 * Yields each environment variable name.
 *
 * An Enumerator is returned if no block is given.
 */
static VALUE
env_each_key(VALUE ehash)
{
    VALUE keys;
    long i;

    RETURN_ENUMERATOR(ehash, 0, 0);
    keys = env_keys();	/* rb_secure(4); */
    for (i=0; i<RARRAY_LEN(keys); i++) {
	rb_yield(RARRAY_PTR(keys)[i]);
    }
    return ehash;
}

/*
 * call-seq:
 *   ENV.values -> Array
 *
 * Returns every environment variable value as an Array
 */
static VALUE
env_values(void)
{
    VALUE ary;
    char **env;

    rb_secure(4);
    ary = rb_ary_new();
    env = GET_ENVIRON(environ);
    while (*env) {
	char *s = strchr(*env, '=');
	if (s) {
	    rb_ary_push(ary, env_str_new2(s+1));
	}
	env++;
    }
    FREE_ENVIRON(environ);
    return ary;
}

/*
 * call-seq:
 *   ENV.each_value { |value| } -> Hash
 *   ENV.each_value             -> Enumerator
 *
 * Yields each environment variable +value+.
 *
 * An Enumerator is returned if no block was given.
 */
static VALUE
env_each_value(VALUE ehash)
{
    VALUE values;
    long i;

    RETURN_ENUMERATOR(ehash, 0, 0);
    values = env_values();	/* rb_secure(4); */
    for (i=0; i<RARRAY_LEN(values); i++) {
	rb_yield(RARRAY_PTR(values)[i]);
    }
    return ehash;
}

/*
 * call-seq:
 *   ENV.each      { |name, value| } -> Hash
 *   ENV.each                        -> Enumerator
 *   ENV.each_pair { |name, value| } -> Hash
 *   ENV.each_pair                   -> Enumerator
 *
 * Yields each environment variable +name+ and +value+.
 *
 * If no block is given an Enumerator is returned.
 */
static VALUE
env_each_pair(VALUE ehash)
{
    char **env;
    VALUE ary;
    long i;

    RETURN_ENUMERATOR(ehash, 0, 0);

    rb_secure(4);
    ary = rb_ary_new();
    env = GET_ENVIRON(environ);
    while (*env) {
	char *s = strchr(*env, '=');
	if (s) {
	    rb_ary_push(ary, env_str_new(*env, s-*env));
	    rb_ary_push(ary, env_str_new2(s+1));
	}
	env++;
    }
    FREE_ENVIRON(environ);

    for (i=0; i<RARRAY_LEN(ary); i+=2) {
	rb_yield(rb_assoc_new(RARRAY_PTR(ary)[i], RARRAY_PTR(ary)[i+1]));
    }
    return ehash;
}

/*
 * call-seq:
 *   ENV.reject! { |name, value| } -> Hash or nil
 *   ENV.reject!                   -> Enumerator
 *
 * Equivalent to ENV#delete_if but returns +nil+ if no changes were made.
 *
 * Returns an Enumerator if no block was given.
 */
static VALUE
env_reject_bang(VALUE ehash)
{
    volatile VALUE keys;
    long i;
    int del = 0;

    RETURN_ENUMERATOR(ehash, 0, 0);
    keys = env_keys();	/* rb_secure(4); */
    for (i=0; i<RARRAY_LEN(keys); i++) {
	VALUE val = rb_f_getenv(Qnil, RARRAY_PTR(keys)[i]);
	if (!NIL_P(val)) {
	    if (RTEST(rb_yield_values(2, RARRAY_PTR(keys)[i], val))) {
		FL_UNSET(RARRAY_PTR(keys)[i], FL_TAINT);
		env_delete(Qnil, RARRAY_PTR(keys)[i]);
		del++;
	    }
	}
    }
    if (del == 0) return Qnil;
    return envtbl;
}

/*
 * call-seq:
 *   ENV.delete_if { |name, value| } -> Hash
 *   ENV.delete_if                   -> Enumerator
 *
 * Deletes every environment variable for which the block evaluates to +true+.
 *
 * If no block is given an enumerator is returned instead.
 */
static VALUE
env_delete_if(VALUE ehash)
{
    RETURN_ENUMERATOR(ehash, 0, 0);
    env_reject_bang(ehash);
    return envtbl;
}

/*
 * call-seq:
 *   ENV.values_at(name, ...) -> Array
 *
 * Returns an array containing the environment variable values associated with
 * the given names.  See also ENV.select.
 */
static VALUE
env_values_at(int argc, VALUE *argv)
{
    VALUE result;
    long i;

    rb_secure(4);
    result = rb_ary_new();
    for (i=0; i<argc; i++) {
	rb_ary_push(result, rb_f_getenv(Qnil, argv[i]));
    }
    return result;
}

/*
 * call-seq:
 *   ENV.select { |name, value| } -> Hash
 *   ENV.select                   -> Enumerator
 *
 * Returns a copy of the environment for entries where the block returns true.
 *
 * Returns an Enumerator if no block was given.
 */
static VALUE
env_select(VALUE ehash)
{
    VALUE result;
    char **env;

    RETURN_ENUMERATOR(ehash, 0, 0);
    rb_secure(4);
    result = rb_hash_new();
    env = GET_ENVIRON(environ);
    while (*env) {
	char *s = strchr(*env, '=');
	if (s) {
	    VALUE k = env_str_new(*env, s-*env);
	    VALUE v = env_str_new2(s+1);
	    if (RTEST(rb_yield_values(2, k, v))) {
		rb_hash_aset(result, k, v);
	    }
	}
	env++;
    }
    FREE_ENVIRON(environ);

    return result;
}

/*
 * call-seq:
 *   ENV.select! { |name, value| } -> ENV or nil
 *   ENV.select!                   -> Enumerator
 *
 * Equivalent to ENV#keep_if but returns +nil+ if no changes were made.
 */
static VALUE
env_select_bang(VALUE ehash)
{
    volatile VALUE keys;
    long i;
    int del = 0;

    RETURN_ENUMERATOR(ehash, 0, 0);
    keys = env_keys();	/* rb_secure(4); */
    for (i=0; i<RARRAY_LEN(keys); i++) {
	VALUE val = rb_f_getenv(Qnil, RARRAY_PTR(keys)[i]);
	if (!NIL_P(val)) {
	    if (!RTEST(rb_yield_values(2, RARRAY_PTR(keys)[i], val))) {
		FL_UNSET(RARRAY_PTR(keys)[i], FL_TAINT);
		env_delete(Qnil, RARRAY_PTR(keys)[i]);
		del++;
	    }
	}
    }
    if (del == 0) return Qnil;
    return envtbl;
}

/*
 * call-seq:
 *   ENV.keep_if { |name, value| } -> Hash
 *   ENV.keep_if                   -> Enumerator
 *
 * Deletes every environment variable where the block evaluates to +false+.
 *
 * Returns an enumerator if no block was given.
 */
static VALUE
env_keep_if(VALUE ehash)
{
    RETURN_ENUMERATOR(ehash, 0, 0);
    env_select_bang(ehash);
    return envtbl;
}

/*
 * call-seq:
 *   ENV.clear
 *
 * Removes every environment variable.
 */
VALUE
rb_env_clear(void)
{
    volatile VALUE keys;
    long i;

    keys = env_keys();	/* rb_secure(4); */
    for (i=0; i<RARRAY_LEN(keys); i++) {
	VALUE val = rb_f_getenv(Qnil, RARRAY_PTR(keys)[i]);
	if (!NIL_P(val)) {
	    env_delete(Qnil, RARRAY_PTR(keys)[i]);
	}
    }
    return envtbl;
}

/*
 * call-seq:
 *   ENV.to_s -> "ENV"
 *
 * Returns "ENV"
 */
static VALUE
env_to_s(void)
{
    return rb_usascii_str_new2("ENV");
}

/*
 * call-seq:
 *   ENV.inspect -> string
 *
 * Returns the contents of the environment as a String.
 */
static VALUE
env_inspect(void)
{
    char **env;
    VALUE str, i;

    rb_secure(4);
    str = rb_str_buf_new2("{");
    env = GET_ENVIRON(environ);
    while (*env) {
	char *s = strchr(*env, '=');

	if (env != environ) {
	    rb_str_buf_cat2(str, ", ");
	}
	if (s) {
	    rb_str_buf_cat2(str, "\"");
	    rb_str_buf_cat(str, *env, s-*env);
	    rb_str_buf_cat2(str, "\"=>");
	    i = rb_inspect(rb_str_new2(s+1));
	    rb_str_buf_append(str, i);
	}
	env++;
    }
    FREE_ENVIRON(environ);
    rb_str_buf_cat2(str, "}");
    OBJ_TAINT(str);

    return str;
}

/*
 * call-seq:
 *   ENV.to_a -> Array
 *
 * Converts the environment variables into an array of names and value arrays.
 *
 *   ENV.to_a # => [["TERM" => "xterm-color"], ["SHELL" => "/bin/bash"], ...]
 *
 */
static VALUE
env_to_a(void)
{
    char **env;
    VALUE ary;

    rb_secure(4);
    ary = rb_ary_new();
    env = GET_ENVIRON(environ);
    while (*env) {
	char *s = strchr(*env, '=');
	if (s) {
	    rb_ary_push(ary, rb_assoc_new(env_str_new(*env, s-*env),
					  env_str_new2(s+1)));
	}
	env++;
    }
    FREE_ENVIRON(environ);
    return ary;
}

/*
 * call-seq:
 *   ENV.rehash
 *
 * Re-hashing the environment variables does nothing.  It is provided for
 * compatibility with Hash.
 */
static VALUE
env_none(void)
{
    return Qnil;
}

/*
 * call-seq:
 *   ENV.length
 *   ENV.size
 *
 * Returns the number of environment variables.
 */
static VALUE
env_size(void)
{
    int i;
    char **env;

    rb_secure(4);
    env = GET_ENVIRON(environ);
    for(i=0; env[i]; i++)
	;
    FREE_ENVIRON(environ);
    return INT2FIX(i);
}

/*
 * call-seq:
 *   ENV.empty? -> true or false
 *
 * Returns true when there are no environment variables
 */
static VALUE
env_empty_p(void)
{
    char **env;

    rb_secure(4);
    env = GET_ENVIRON(environ);
    if (env[0] == 0) {
	FREE_ENVIRON(environ);
	return Qtrue;
    }
    FREE_ENVIRON(environ);
    return Qfalse;
}

/*
 * call-seq:
 *   ENV.key?(name)     -> true or false
 *   ENV.include?(name) -> true or false
 *   ENV.has_key?(name) -> true or false
 *   ENV.member?(name)  -> true or false
 *
 * Returns +true+ if there is an environment variable with the given +name+.
 */
static VALUE
env_has_key(VALUE env, VALUE key)
{
    char *s;

    rb_secure(4);
    s = StringValuePtr(key);
    if (memchr(s, '\0', RSTRING_LEN(key)))
	rb_raise(rb_eArgError, "bad environment variable name");
    if (getenv(s)) return Qtrue;
    return Qfalse;
}

/*
 * call-seq:
 *   ENV.assoc(name) -> Array or nil
 *
 * Returns an Array of the name and value of the environment variable with
 * +name+ or +nil+ if the name cannot be found.
 */
static VALUE
env_assoc(VALUE env, VALUE key)
{
    char *s, *e;

    rb_secure(4);
    s = StringValuePtr(key);
    if (memchr(s, '\0', RSTRING_LEN(key)))
	rb_raise(rb_eArgError, "bad environment variable name");
    e = getenv(s);
    if (e) return rb_assoc_new(key, rb_tainted_str_new2(e));
    return Qnil;
}

/*
 * call-seq:
 *   ENV.value?(value) -> true or false
 *   ENV.has_value?(value) -> true or false
 *
 * Returns +true+ if there is an environment variable with the given +value+.
 */
static VALUE
env_has_value(VALUE dmy, VALUE obj)
{
    char **env;

    rb_secure(4);
    obj = rb_check_string_type(obj);
    if (NIL_P(obj)) return Qnil;
    env = GET_ENVIRON(environ);
    while (*env) {
	char *s = strchr(*env, '=');
	if (s++) {
	    long len = strlen(s);
	    if (RSTRING_LEN(obj) == len && strncmp(s, RSTRING_PTR(obj), len) == 0) {
		FREE_ENVIRON(environ);
		return Qtrue;
	    }
	}
	env++;
    }
    FREE_ENVIRON(environ);
    return Qfalse;
}

/*
 * call-seq:
 *   ENV.rassoc(value)
 *
 * Returns an Array of the name and value of the environment variable with
 * +value+ or +nil+ if the value cannot be found.
 */
static VALUE
env_rassoc(VALUE dmy, VALUE obj)
{
    char **env;

    rb_secure(4);
    obj = rb_check_string_type(obj);
    if (NIL_P(obj)) return Qnil;
    env = GET_ENVIRON(environ);
    while (*env) {
	char *s = strchr(*env, '=');
	if (s++) {
	    long len = strlen(s);
	    if (RSTRING_LEN(obj) == len && strncmp(s, RSTRING_PTR(obj), len) == 0) {
		VALUE result = rb_assoc_new(rb_tainted_str_new(*env, s-*env-1), obj);
		FREE_ENVIRON(environ);
		return result;
	    }
	}
	env++;
    }
    FREE_ENVIRON(environ);
    return Qnil;
}

/*
 * call-seq:
 *   ENV.key(value) -> name
 *
 * Returns the name of the environment variable with +value+.  If the value is
 * not found +nil+ is returned.
 */
static VALUE
env_key(VALUE dmy, VALUE value)
{
    char **env;
    VALUE str;

    rb_secure(4);
    StringValue(value);
    env = GET_ENVIRON(environ);
    while (*env) {
	char *s = strchr(*env, '=');
	if (s++) {
	    long len = strlen(s);
	    if (RSTRING_LEN(value) == len && strncmp(s, RSTRING_PTR(value), len) == 0) {
		str = env_str_new(*env, s-*env-1);
		FREE_ENVIRON(environ);
		return str;
	    }
	}
	env++;
    }
    FREE_ENVIRON(environ);
    return Qnil;
}

/*
 * call-seq:
 *   ENV.index(value) -> key
 *
 * Deprecated method that is equivalent to ENV.key
 */
static VALUE
env_index(VALUE dmy, VALUE value)
{
    rb_warn("ENV.index is deprecated; use ENV.key");
    return env_key(dmy, value);
}

/*
 * call-seq:
 *   ENV.to_hash -> Hash
 *
 * Creates a hash with a copy of the environment variables.
 *
 */
static VALUE
env_to_hash(void)
{
    char **env;
    VALUE hash;

    rb_secure(4);
    hash = rb_hash_new();
    env = GET_ENVIRON(environ);
    while (*env) {
	char *s = strchr(*env, '=');
	if (s) {
	    rb_hash_aset(hash, env_str_new(*env, s-*env),
			       env_str_new2(s+1));
	}
	env++;
    }
    FREE_ENVIRON(environ);
    return hash;
}

/*
 * call-seq:
 *   ENV.reject { |name, value| } -> Hash
 *   ENV.reject                   -> Enumerator
 *
 * Same as ENV#delete_if, but works on (and returns) a copy of the
 * environment.
 */
static VALUE
env_reject(void)
{
    return rb_hash_delete_if(env_to_hash());
}

/*
 * call-seq:
 *   ENV.shift -> Array or nil
 *
 * Removes an environment variable name-value pair from ENV and returns it as
 * an Array.  Returns +nil+ if when the environment is empty.
 */
static VALUE
env_shift(void)
{
    char **env;

    rb_secure(4);
    env = GET_ENVIRON(environ);
    if (*env) {
	char *s = strchr(*env, '=');
	if (s) {
	    VALUE key = env_str_new(*env, s-*env);
	    VALUE val = env_str_new2(getenv(RSTRING_PTR(key)));
	    env_delete(Qnil, key);
	    return rb_assoc_new(key, val);
	}
    }
    FREE_ENVIRON(environ);
    return Qnil;
}

/*
 * call-seq:
 *   ENV.invert -> Hash
 *
 * Returns a new hash created by using environment variable names as values
 * and values as names.
 */
static VALUE
env_invert(void)
{
    return rb_hash_invert(env_to_hash());
}

static int
env_replace_i(VALUE key, VALUE val, VALUE keys)
{
    if (key != Qundef) {
	env_aset(Qnil, key, val);
	if (rb_ary_includes(keys, key)) {
	    rb_ary_delete(keys, key);
	}
    }
    return ST_CONTINUE;
}

/*
 * call-seq:
 *   ENV.replace(hash) -> env
 *
 * Replaces the contents of the environment variables with the contents of
 * +hash+.
 */
static VALUE
env_replace(VALUE env, VALUE hash)
{
    volatile VALUE keys;
    long i;

    keys = env_keys();	/* rb_secure(4); */
    if (env == hash) return env;
    hash = to_hash(hash);
    rb_hash_foreach(hash, env_replace_i, keys);

    for (i=0; i<RARRAY_LEN(keys); i++) {
	env_delete(env, RARRAY_PTR(keys)[i]);
    }
    return env;
}

static int
env_update_i(VALUE key, VALUE val)
{
    if (key != Qundef) {
	if (rb_block_given_p()) {
	    val = rb_yield_values(3, key, rb_f_getenv(Qnil, key), val);
	}
	env_aset(Qnil, key, val);
    }
    return ST_CONTINUE;
}

/*
 * call-seq:
 *   ENV.update(hash)                                  -> Hash
 *   ENV.update(hash) { |name, old_value, new_value| } -> Hash
 *
 * Adds the contents of +hash+ to the environment variables.  If no block is
 * specified entries with duplicate keys are overwritten, otherwise the value
 * of each duplicate name is determined by calling the block with the key, its
 * value from the environment and its value from the hash.
 */
static VALUE
env_update(VALUE env, VALUE hash)
{
    rb_secure(4);
    if (env == hash) return env;
    hash = to_hash(hash);
    rb_hash_foreach(hash, env_update_i, 0);
    return env;
}

/*
 *  A Hash is a dictionary-like collection of unique keys and their values.
 *  Also called associative arrays, they are similar to Arrays, but where an
 *  Array uses integers as its index, a Hash allows you to use any object
 *  type.
 *
 *  Hashes enumerate their values in the order that the corresponding keys
 *  were inserted.
 *
 *  A Hash can be easily created by using its implicit form:
 *
 *    grades = { "Jane Doe" => 10, "Jim Doe" => 6 }
 *
 *  Hashes allow an alternate syntax form when your keys are always symbols.
 *  Instead of
 *
 *    options = { :font_size => 10, :font_family => "Arial" }
 *
 *  You could write it as:
 *
 *    options = { font_size: 10, font_family: "Arial" }
 *
 *  Each named key is a symbol you can access in hash:
 *
 *    options[:font_size]  # => 10
 *
 *  A Hash can also be created through its ::new method:
 *
 *    grades = Hash.new
 *    grades["Dorothy Doe"] = 9
 *
 *  Hashes have a <em>default value</em> that is returned when accessing
 *  keys that do not exist in the hash. If no default is set +nil+ is used.
 *  You can set the default value by sending it as an argument to Hash.new:
 *
 *    grades = Hash.new(0)
 *
 *  Or by using the #default= method:
 *
 *    grades = {"Timmy Doe" => 8}
 *    grades.default = 0
 *
 *  Accessing a value in a Hash requires using its key:
 *
 *    puts grades["Jane Doe"] # => 10
 *
 *  === Common Uses
 *
 *  Hashes are an easy way to represent data structures, such as
 *
 *    books         = {}
 *    books[:matz]  = "The Ruby Language"
 *    books[:black] = "The Well-Grounded Rubyist"
 *
 *  Hashes are also commonly used as a way to have named parameters in
 *  functions. Note that no brackets are used below. If a hash is the last
 *  argument on a method call, no braces are needed, thus creating a really
 *  clean interface:
 *
 *    Person.create(name: "John Doe", age: 27)
 *
 *    def self.create(params)
 *      @name = params[:name]
 *      @age  = params[:age]
 *    end
 *
 *  === Hash Keys
 *
 *  Two objects refer to the same hash key when their <code>hash</code> value
 *  is identical and the two objects are <code>eql?</code> to each other.
 *
 *  A user-defined class may be used as a hash key if the <code>hash</code>
 *  and <code>eql?</code> methods are overridden to provide meaningful
 *  behavior.  By default, separate instances refer to separate hash keys.
 *
 *  A typical implementation of <code>hash</code> is based on the
 *  object's data while <code>eql?</code> is usually aliased to the overridden
 *  <code>==</code> method:
 *
 *    class Book
 *      attr_reader :author, :title
 *
 *      def initialize(author, title)
 *        @author = author
 *        @title = title
 *      end
 *
 *      def ==(other)
 *        self.class === other and
 *          other.author == @author and
 *          other.title == @title
 *      end
 *
 *      alias eql? ==
 *
 *      def hash
 *        @author.hash ^ @title.hash # XOR
 *      end
 *    end
 *
 *    book1 = Book.new 'matz', 'Ruby in a Nutshell'
 *    book2 = Book.new 'matz', 'Ruby in a Nutshell'
 *
 *    reviews = {}
 *
 *    reviews[book1] = 'Great reference!'
 *    reviews[book2] = 'Nice and compact!'
 *
 *    reviews.length #=> 1
 *
 *  See also Object#hash and Object#eql?
 */

void
Init_Hash(void)
{
#undef rb_intern
#define rb_intern(str) rb_intern_const(str)

    id_hash = rb_intern("hash");
    id_yield = rb_intern("yield");
    id_default = rb_intern("default");

    rb_cHash = rb_define_class("Hash", rb_cObject);

    rb_include_module(rb_cHash, rb_mEnumerable);

    rb_define_alloc_func(rb_cHash, hash_alloc);
    rb_define_singleton_method(rb_cHash, "[]", rb_hash_s_create, -1);
    rb_define_singleton_method(rb_cHash, "try_convert", rb_hash_s_try_convert, 1);
    rb_define_method(rb_cHash,"initialize", rb_hash_initialize, -1);
    rb_define_method(rb_cHash,"initialize_copy", rb_hash_replace, 1);
    rb_define_method(rb_cHash,"rehash", rb_hash_rehash, 0);

    rb_define_method(rb_cHash,"to_hash", rb_hash_to_hash, 0);
    rb_define_method(rb_cHash,"to_a", rb_hash_to_a, 0);
    rb_define_method(rb_cHash,"inspect", rb_hash_inspect, 0);
    rb_define_alias(rb_cHash, "to_s", "inspect");

    rb_define_method(rb_cHash,"==", rb_hash_equal, 1);
    rb_define_method(rb_cHash,"[]", rb_hash_aref, 1);
    rb_define_method(rb_cHash,"hash", rb_hash_hash, 0);
    rb_define_method(rb_cHash,"eql?", rb_hash_eql, 1);
    rb_define_method(rb_cHash,"fetch", rb_hash_fetch_m, -1);
    rb_define_method(rb_cHash,"[]=", rb_hash_aset, 2);
    rb_define_method(rb_cHash,"store", rb_hash_aset, 2);
    rb_define_method(rb_cHash,"default", rb_hash_default, -1);
    rb_define_method(rb_cHash,"default=", rb_hash_set_default, 1);
    rb_define_method(rb_cHash,"default_proc", rb_hash_default_proc, 0);
    rb_define_method(rb_cHash,"default_proc=", rb_hash_set_default_proc, 1);
    rb_define_method(rb_cHash,"key", rb_hash_key, 1);
    rb_define_method(rb_cHash,"index", rb_hash_index, 1);
    rb_define_method(rb_cHash,"size", rb_hash_size, 0);
    rb_define_method(rb_cHash,"length", rb_hash_size, 0);
    rb_define_method(rb_cHash,"empty?", rb_hash_empty_p, 0);

    rb_define_method(rb_cHash,"each_value", rb_hash_each_value, 0);
    rb_define_method(rb_cHash,"each_key", rb_hash_each_key, 0);
    rb_define_method(rb_cHash,"each_pair", rb_hash_each_pair, 0);
    rb_define_method(rb_cHash,"each", rb_hash_each_pair, 0);

    rb_define_method(rb_cHash,"keys", rb_hash_keys, 0);
    rb_define_method(rb_cHash,"values", rb_hash_values, 0);
    rb_define_method(rb_cHash,"values_at", rb_hash_values_at, -1);

    rb_define_method(rb_cHash,"shift", rb_hash_shift, 0);
    rb_define_method(rb_cHash,"delete", rb_hash_delete, 1);
    rb_define_method(rb_cHash,"delete_if", rb_hash_delete_if, 0);
    rb_define_method(rb_cHash,"keep_if", rb_hash_keep_if, 0);
    rb_define_method(rb_cHash,"select", rb_hash_select, 0);
    rb_define_method(rb_cHash,"select!", rb_hash_select_bang, 0);
    rb_define_method(rb_cHash,"reject", rb_hash_reject, 0);
    rb_define_method(rb_cHash,"reject!", rb_hash_reject_bang, 0);
    rb_define_method(rb_cHash,"clear", rb_hash_clear, 0);
    rb_define_method(rb_cHash,"invert", rb_hash_invert, 0);
    rb_define_method(rb_cHash,"update", rb_hash_update, 1);
    rb_define_method(rb_cHash,"replace", rb_hash_replace, 1);
    rb_define_method(rb_cHash,"merge!", rb_hash_update, 1);
    rb_define_method(rb_cHash,"merge", rb_hash_merge, 1);
    rb_define_method(rb_cHash, "assoc", rb_hash_assoc, 1);
    rb_define_method(rb_cHash, "rassoc", rb_hash_rassoc, 1);
    rb_define_method(rb_cHash, "flatten", rb_hash_flatten, -1);

    rb_define_method(rb_cHash,"include?", rb_hash_has_key, 1);
    rb_define_method(rb_cHash,"member?", rb_hash_has_key, 1);
    rb_define_method(rb_cHash,"has_key?", rb_hash_has_key, 1);
    rb_define_method(rb_cHash,"has_value?", rb_hash_has_value, 1);
    rb_define_method(rb_cHash,"key?", rb_hash_has_key, 1);
    rb_define_method(rb_cHash,"value?", rb_hash_has_value, 1);

    rb_define_method(rb_cHash,"compare_by_identity", rb_hash_compare_by_id, 0);
    rb_define_method(rb_cHash,"compare_by_identity?", rb_hash_compare_by_id_p, 0);

    /* Document-class: ENV
     *
     * ENV is a hash-like accessor for environment variables.
     */

    /*
     * Hack to get RDoc to regard ENV as a class:
     * envtbl = rb_define_class("ENV", rb_cObject);
     */
    origenviron = environ;
    envtbl = rb_obj_alloc(rb_cObject);
    rb_extend_object(envtbl, rb_mEnumerable);

    rb_define_singleton_method(envtbl,"[]", rb_f_getenv, 1);
    rb_define_singleton_method(envtbl,"fetch", env_fetch, -1);
    rb_define_singleton_method(envtbl,"[]=", env_aset, 2);
    rb_define_singleton_method(envtbl,"store", env_aset, 2);
    rb_define_singleton_method(envtbl,"each", env_each_pair, 0);
    rb_define_singleton_method(envtbl,"each_pair", env_each_pair, 0);
    rb_define_singleton_method(envtbl,"each_key", env_each_key, 0);
    rb_define_singleton_method(envtbl,"each_value", env_each_value, 0);
    rb_define_singleton_method(envtbl,"delete", env_delete_m, 1);
    rb_define_singleton_method(envtbl,"delete_if", env_delete_if, 0);
    rb_define_singleton_method(envtbl,"keep_if", env_keep_if, 0);
    rb_define_singleton_method(envtbl,"clear", rb_env_clear, 0);
    rb_define_singleton_method(envtbl,"reject", env_reject, 0);
    rb_define_singleton_method(envtbl,"reject!", env_reject_bang, 0);
    rb_define_singleton_method(envtbl,"select", env_select, 0);
    rb_define_singleton_method(envtbl,"select!", env_select_bang, 0);
    rb_define_singleton_method(envtbl,"shift", env_shift, 0);
    rb_define_singleton_method(envtbl,"invert", env_invert, 0);
    rb_define_singleton_method(envtbl,"replace", env_replace, 1);
    rb_define_singleton_method(envtbl,"update", env_update, 1);
    rb_define_singleton_method(envtbl,"inspect", env_inspect, 0);
    rb_define_singleton_method(envtbl,"rehash", env_none, 0);
    rb_define_singleton_method(envtbl,"to_a", env_to_a, 0);
    rb_define_singleton_method(envtbl,"to_s", env_to_s, 0);
    rb_define_singleton_method(envtbl,"key", env_key, 1);
    rb_define_singleton_method(envtbl,"index", env_index, 1);
    rb_define_singleton_method(envtbl,"size", env_size, 0);
    rb_define_singleton_method(envtbl,"length", env_size, 0);
    rb_define_singleton_method(envtbl,"empty?", env_empty_p, 0);
    rb_define_singleton_method(envtbl,"keys", env_keys, 0);
    rb_define_singleton_method(envtbl,"values", env_values, 0);
    rb_define_singleton_method(envtbl,"values_at", env_values_at, -1);
    rb_define_singleton_method(envtbl,"include?", env_has_key, 1);
    rb_define_singleton_method(envtbl,"member?", env_has_key, 1);
    rb_define_singleton_method(envtbl,"has_key?", env_has_key, 1);
    rb_define_singleton_method(envtbl,"has_value?", env_has_value, 1);
    rb_define_singleton_method(envtbl,"key?", env_has_key, 1);
    rb_define_singleton_method(envtbl,"value?", env_has_value, 1);
    rb_define_singleton_method(envtbl,"to_hash", env_to_hash, 0);
    rb_define_singleton_method(envtbl,"assoc", env_assoc, 1);
    rb_define_singleton_method(envtbl,"rassoc", env_rassoc, 1);

    /*
     * ENV is a Hash-like accessor for environment variables.
     *
     * See ENV (the class) for more details.
     */
    rb_define_global_const("ENV", envtbl);
}
