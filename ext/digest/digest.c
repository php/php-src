/************************************************

  digest.c -

  $Author$
  created at: Fri May 25 08:57:27 JST 2001

  Copyright (C) 1995-2001 Yukihiro Matsumoto
  Copyright (C) 2001-2006 Akinori MUSHA

  $RoughId: digest.c,v 1.16 2001/07/13 15:38:27 knu Exp $
  $Id$

************************************************/

#include "digest.h"

static VALUE rb_mDigest;
static VALUE rb_mDigest_Instance;
static VALUE rb_cDigest_Class;
static VALUE rb_cDigest_Base;

static ID id_reset, id_update, id_finish, id_digest, id_hexdigest, id_digest_length;
static ID id_metadata;

RUBY_EXTERN void Init_digest_base(void);

/*
 * Document-module: Digest
 *
 * This module provides a framework for message digest libraries.
 *
 * You may want to look at OpenSSL::Digest as it supports support more
 * algorithms.
 *
 * A cryptographic hash function is a procedure that takes data and return a
 * fixed bit string : the hash value, also known as _digest_. Hash functions
 * are also called one-way functions, it is easy to compute a digest from
 * a message, but it is infeasible to generate a message from a digest.
 *
 * == Example
 *
 *   require 'digest'
 *
 *   # Compute a complete digest
 *   sha256 = Digest::SHA256.new
 *   digest = sha256.digest message
 *
 *   # Compute digest by chunks
 *   sha256 = Digest::SHA256.new
 *   sha256.update message1
 *   sha256 << message2 # << is an alias for update
 *
 *   digest = sha256.digest
 *
 * == Digest algorithms
 *
 * Different digest algorithms (or hash functions) are available :
 *
 * HMAC::
 *   See FIPS PUB 198 The Keyed-Hash Message Authentication Code (HMAC)
 * RIPEMD-160::
 *   (as Digest::RMD160) see
 *   http://homes.esat.kuleuven.be/~bosselae/ripemd160.html
 * SHA1::
 *   See FIPS 180 Secure Hash Standard
 * SHA2 family::
 *   See FIPS 180 Secure Hash Standard which defines the following algorithms:
 *   * SHA512
 *   * SHA384
 *   * SHA256
 *
 * The latest versions of the FIPS publications can be found here:
 * http://csrc.nist.gov/publications/PubsFIPS.html
 *
 * Additionally Digest::BubbleBabble encodes a digest as a sequence of
 * consonants and vowels which is more recognizable and comparable than a
 * hexadecimal digest.  See http://en.wikipedia.org/wiki/Bubblebabble
 */

static VALUE
hexencode_str_new(VALUE str_digest)
{
    char *digest;
    size_t digest_len;
    size_t i;
    VALUE str;
    char *p;
    static const char hex[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f'
    };

    StringValue(str_digest);
    digest = RSTRING_PTR(str_digest);
    digest_len = RSTRING_LEN(str_digest);

    if (LONG_MAX / 2 < digest_len) {
        rb_raise(rb_eRuntimeError, "digest string too long");
    }

    str = rb_str_new(0, digest_len * 2);

    for (i = 0, p = RSTRING_PTR(str); i < digest_len; i++) {
        unsigned char byte = digest[i];

        p[i + i]     = hex[byte >> 4];
        p[i + i + 1] = hex[byte & 0x0f];
    }

    return str;
}

/*
 * call-seq:
 *     Digest.hexencode(string) -> hexencoded_string
 *
 * Generates a hex-encoded version of a given _string_.
 */
static VALUE
rb_digest_s_hexencode(VALUE klass, VALUE str)
{
    return hexencode_str_new(str);
}

NORETURN(static void rb_digest_instance_method_unimpl(VALUE self, const char *method));

/*
 * Document-module: Digest::Instance
 *
 * This module provides instance methods for a digest implementation
 * object to calculate message digest values.
 */

static void
rb_digest_instance_method_unimpl(VALUE self, const char *method)
{
    rb_raise(rb_eRuntimeError, "%s does not implement %s()",
	     rb_obj_classname(self), method);
}

/*
 * call-seq:
 *     digest_obj.update(string) -> digest_obj
 *     digest_obj << string -> digest_obj
 *
 * Updates the digest using a given _string_ and returns self.
 *
 * The update() method and the left-shift operator are overridden by
 * each implementation subclass. (One should be an alias for the
 * other)
 */
static VALUE
rb_digest_instance_update(VALUE self, VALUE str)
{
    rb_digest_instance_method_unimpl(self, "update");
}

/*
 * call-seq:
 *     digest_obj.instance_eval { finish } -> digest_obj
 *
 * Finishes the digest and returns the resulting hash value.
 *
 * This method is overridden by each implementation subclass and often
 * made private, because some of those subclasses may leave internal
 * data uninitialized.  Do not call this method from outside.  Use
 * #digest!() instead, which ensures that internal data be reset for
 * security reasons.
 */
static VALUE
rb_digest_instance_finish(VALUE self)
{
    rb_digest_instance_method_unimpl(self, "finish");
}

/*
 * call-seq:
 *     digest_obj.reset -> digest_obj
 *
 * Resets the digest to the initial state and returns self.
 *
 * This method is overridden by each implementation subclass.
 */
static VALUE
rb_digest_instance_reset(VALUE self)
{
    rb_digest_instance_method_unimpl(self, "reset");
}

/*
 * call-seq:
 *     digest_obj.new -> another_digest_obj
 *
 * Returns a new, initialized copy of the digest object.  Equivalent
 * to digest_obj.clone().reset().
 */
static VALUE
rb_digest_instance_new(VALUE self)
{
    VALUE clone = rb_obj_clone(self);
    rb_funcall(clone, id_reset, 0);
    return clone;
}

/*
 * call-seq:
 *     digest_obj.digest -> string
 *     digest_obj.digest(string) -> string
 *
 * If none is given, returns the resulting hash value of the digest,
 * keeping the digest's state.
 *
 * If a _string_ is given, returns the hash value for the given
 * _string_, resetting the digest to the initial state before and
 * after the process.
 */
static VALUE
rb_digest_instance_digest(int argc, VALUE *argv, VALUE self)
{
    VALUE str, value;

    if (rb_scan_args(argc, argv, "01", &str) > 0) {
        rb_funcall(self, id_reset, 0);
        rb_funcall(self, id_update, 1, str);
        value = rb_funcall(self, id_finish, 0);
        rb_funcall(self, id_reset, 0);
    } else {
        value = rb_funcall(rb_obj_clone(self), id_finish, 0);
    }

    return value;
}

/*
 * call-seq:
 *     digest_obj.digest! -> string
 *
 * Returns the resulting hash value and resets the digest to the
 * initial state.
 */
static VALUE
rb_digest_instance_digest_bang(VALUE self)
{
    VALUE value = rb_funcall(self, id_finish, 0);
    rb_funcall(self, id_reset, 0);

    return value;
}

/*
 * call-seq:
 *     digest_obj.hexdigest -> string
 *     digest_obj.hexdigest(string) -> string
 *
 * If none is given, returns the resulting hash value of the digest in
 * a hex-encoded form, keeping the digest's state.
 *
 * If a _string_ is given, returns the hash value for the given
 * _string_ in a hex-encoded form, resetting the digest to the initial
 * state before and after the process.
 */
static VALUE
rb_digest_instance_hexdigest(int argc, VALUE *argv, VALUE self)
{
    VALUE str, value;

    if (rb_scan_args(argc, argv, "01", &str) > 0) {
        rb_funcall(self, id_reset, 0);
        rb_funcall(self, id_update, 1, str);
        value = rb_funcall(self, id_finish, 0);
        rb_funcall(self, id_reset, 0);
    } else {
        value = rb_funcall(rb_obj_clone(self), id_finish, 0);
    }

    return hexencode_str_new(value);
}

/*
 * call-seq:
 *     digest_obj.hexdigest! -> string
 *
 * Returns the resulting hash value in a hex-encoded form and resets
 * the digest to the initial state.
 */
static VALUE
rb_digest_instance_hexdigest_bang(VALUE self)
{
    VALUE value = rb_funcall(self, id_finish, 0);
    rb_funcall(self, id_reset, 0);

    return hexencode_str_new(value);
}

/*
 * call-seq:
 *     digest_obj.to_s -> string
 *
 * Returns digest_obj.hexdigest().
 */
static VALUE
rb_digest_instance_to_s(VALUE self)
{
    return rb_funcall(self, id_hexdigest, 0);
}

/*
 * call-seq:
 *     digest_obj.inspect -> string
 *
 * Creates a printable version of the digest object.
 */
static VALUE
rb_digest_instance_inspect(VALUE self)
{
    VALUE str;
    size_t digest_len = 32;	/* about this size at least */
    const char *cname;

    cname = rb_obj_classname(self);

    /* #<Digest::ClassName: xxxxx...xxxx> */
    str = rb_str_buf_new(2 + strlen(cname) + 2 + digest_len * 2 + 1);
    rb_str_buf_cat2(str, "#<");
    rb_str_buf_cat2(str, cname);
    rb_str_buf_cat2(str, ": ");
    rb_str_buf_append(str, rb_digest_instance_hexdigest(0, 0, self));
    rb_str_buf_cat2(str, ">");
    return str;
}

/*
 * call-seq:
 *     digest_obj == another_digest_obj -> boolean
 *     digest_obj == string -> boolean
 *
 * If a string is given, checks whether it is equal to the hex-encoded
 * hash value of the digest object.  If another digest instance is
 * given, checks whether they have the same hash value.  Otherwise
 * returns false.
 */
static VALUE
rb_digest_instance_equal(VALUE self, VALUE other)
{
    VALUE str1, str2;

    if (rb_obj_is_kind_of(other, rb_mDigest_Instance) == Qtrue) {
        str1 = rb_digest_instance_digest(0, 0, self);
        str2 = rb_digest_instance_digest(0, 0, other);
    } else {
        str1 = rb_digest_instance_to_s(self);
        str2 = other;
    }

    /* never blindly assume that subclass methods return strings */
    StringValue(str1);
    StringValue(str2);

    if (RSTRING_LEN(str1) == RSTRING_LEN(str2) &&
	rb_str_cmp(str1, str2) == 0) {
	return Qtrue;
    }
    return Qfalse;
}

/*
 * call-seq:
 *     digest_obj.digest_length -> integer
 *
 * Returns the length of the hash value of the digest.
 *
 * This method should be overridden by each implementation subclass.
 * If not, digest_obj.digest().length() is returned.
 */
static VALUE
rb_digest_instance_digest_length(VALUE self)
{
    /* subclasses really should redefine this method */
    VALUE digest = rb_digest_instance_digest(0, 0, self);

    /* never blindly assume that #digest() returns a string */
    StringValue(digest);
    return INT2NUM(RSTRING_LEN(digest));
}

/*
 * call-seq:
 *     digest_obj.length -> integer
 *     digest_obj.size -> integer
 *
 * Returns digest_obj.digest_length().
 */
static VALUE
rb_digest_instance_length(VALUE self)
{
    return rb_funcall(self, id_digest_length, 0);
}

/*
 * call-seq:
 *     digest_obj.block_length -> integer
 *
 * Returns the block length of the digest.
 *
 * This method is overridden by each implementation subclass.
 */
static VALUE
rb_digest_instance_block_length(VALUE self)
{
    rb_digest_instance_method_unimpl(self, "block_length");
}

/*
 * Document-class: Digest::Class
 *
 * This module stands as a base class for digest implementation
 * classes.
 */

/*
 * call-seq:
 *     Digest::Class.digest(string, *parameters) -> hash_string
 *
 * Returns the hash value of a given _string_.  This is equivalent to
 * Digest::Class.new(*parameters).digest(string), where extra
 * _parameters_, if any, are passed through to the constructor and the
 * _string_ is passed to #digest().
 */
static VALUE
rb_digest_class_s_digest(int argc, VALUE *argv, VALUE klass)
{
    VALUE str;
    volatile VALUE obj;

    if (argc < 1) {
        rb_raise(rb_eArgError, "no data given");
    }

    str = *argv++;
    argc--;

    StringValue(str);

    obj = rb_obj_alloc(klass);
    rb_obj_call_init(obj, argc, argv);

    return rb_funcall(obj, id_digest, 1, str);
}

/*
 * call-seq:
 *     Digest::Class.hexdigest(string[, ...]) -> hash_string
 *
 * Returns the hex-encoded hash value of a given _string_.  This is
 * almost equivalent to
 * Digest.hexencode(Digest::Class.new(*parameters).digest(string)).
 */
static VALUE
rb_digest_class_s_hexdigest(int argc, VALUE *argv, VALUE klass)
{
    return hexencode_str_new(rb_funcall2(klass, id_digest, argc, argv));
}

/* :nodoc: */
static VALUE
rb_digest_class_init(VALUE self)
{
    return self;
}

/*
 * Document-class: Digest::Base
 *
 * This abstract class provides a common interface to message digest
 * implementation classes written in C.
 */

static rb_digest_metadata_t *
get_digest_base_metadata(VALUE klass)
{
    VALUE p;
    VALUE obj;
    rb_digest_metadata_t *algo;

    for (p = klass; !NIL_P(p); p = rb_class_superclass(p)) {
        if (rb_ivar_defined(p, id_metadata)) {
            obj = rb_ivar_get(p, id_metadata);
            break;
        }
    }

    if (NIL_P(p))
        rb_raise(rb_eRuntimeError, "Digest::Base cannot be directly inherited in Ruby");

    Data_Get_Struct(obj, rb_digest_metadata_t, algo);

    switch (algo->api_version) {
      case 2:
        break;

      /*
       * put conversion here if possible when API is updated
       */

      default:
        rb_raise(rb_eRuntimeError, "Incompatible digest API version");
    }

    return algo;
}

static VALUE
rb_digest_base_alloc(VALUE klass)
{
    rb_digest_metadata_t *algo;
    VALUE obj;
    void *pctx;

    if (klass == rb_cDigest_Base) {
	rb_raise(rb_eNotImpError, "Digest::Base is an abstract class");
    }

    algo = get_digest_base_metadata(klass);

    pctx = xmalloc(algo->ctx_size);
    algo->init_func(pctx);

    obj = Data_Wrap_Struct(klass, 0, xfree, pctx);

    return obj;
}

/* :nodoc: */
static VALUE
rb_digest_base_copy(VALUE copy, VALUE obj)
{
    rb_digest_metadata_t *algo;
    void *pctx1, *pctx2;

    if (copy == obj) return copy;

    rb_check_frozen(copy);

    algo = get_digest_base_metadata(rb_obj_class(copy));

    Data_Get_Struct(obj, void, pctx1);
    Data_Get_Struct(copy, void, pctx2);
    memcpy(pctx2, pctx1, algo->ctx_size);

    return copy;
}

/* :nodoc: */
static VALUE
rb_digest_base_reset(VALUE self)
{
    rb_digest_metadata_t *algo;
    void *pctx;

    algo = get_digest_base_metadata(rb_obj_class(self));

    Data_Get_Struct(self, void, pctx);

    algo->init_func(pctx);

    return self;
}

/* :nodoc: */
static VALUE
rb_digest_base_update(VALUE self, VALUE str)
{
    rb_digest_metadata_t *algo;
    void *pctx;

    algo = get_digest_base_metadata(rb_obj_class(self));

    Data_Get_Struct(self, void, pctx);

    StringValue(str);
    algo->update_func(pctx, (unsigned char *)RSTRING_PTR(str), RSTRING_LEN(str));

    return self;
}

/* :nodoc: */
static VALUE
rb_digest_base_finish(VALUE self)
{
    rb_digest_metadata_t *algo;
    void *pctx;
    VALUE str;

    algo = get_digest_base_metadata(rb_obj_class(self));

    Data_Get_Struct(self, void, pctx);

    str = rb_str_new(0, algo->digest_len);
    algo->finish_func(pctx, (unsigned char *)RSTRING_PTR(str));

    /* avoid potential coredump caused by use of a finished context */
    algo->init_func(pctx);

    return str;
}

/* :nodoc: */
static VALUE
rb_digest_base_digest_length(VALUE self)
{
    rb_digest_metadata_t *algo;

    algo = get_digest_base_metadata(rb_obj_class(self));

    return INT2NUM(algo->digest_len);
}

/* :nodoc: */
static VALUE
rb_digest_base_block_length(VALUE self)
{
    rb_digest_metadata_t *algo;

    algo = get_digest_base_metadata(rb_obj_class(self));

    return INT2NUM(algo->block_len);
}

void
Init_digest(void)
{
    id_reset           = rb_intern("reset");
    id_update          = rb_intern("update");
    id_finish          = rb_intern("finish");
    id_digest          = rb_intern("digest");
    id_hexdigest       = rb_intern("hexdigest");
    id_digest_length   = rb_intern("digest_length");

    /*
     * module Digest
     */
    rb_mDigest = rb_define_module("Digest");

    /* module functions */
    rb_define_module_function(rb_mDigest, "hexencode", rb_digest_s_hexencode, 1);

    /*
     * module Digest::Instance
     */
    rb_mDigest_Instance = rb_define_module_under(rb_mDigest, "Instance");

    /* instance methods that should be overridden */
    rb_define_method(rb_mDigest_Instance, "update", rb_digest_instance_update, 1);
    rb_define_method(rb_mDigest_Instance, "<<", rb_digest_instance_update, 1);
    rb_define_private_method(rb_mDigest_Instance, "finish", rb_digest_instance_finish, 0);
    rb_define_method(rb_mDigest_Instance, "reset", rb_digest_instance_reset, 0);
    rb_define_method(rb_mDigest_Instance, "digest_length", rb_digest_instance_digest_length, 0);
    rb_define_method(rb_mDigest_Instance, "block_length", rb_digest_instance_block_length, 0);

    /* instance methods that may be overridden */
    rb_define_method(rb_mDigest_Instance, "==", rb_digest_instance_equal, 1);
    rb_define_method(rb_mDigest_Instance, "inspect", rb_digest_instance_inspect, 0);

    /* instance methods that need not usually be overridden */
    rb_define_method(rb_mDigest_Instance, "new", rb_digest_instance_new, 0);
    rb_define_method(rb_mDigest_Instance, "digest", rb_digest_instance_digest, -1);
    rb_define_method(rb_mDigest_Instance, "digest!", rb_digest_instance_digest_bang, 0);
    rb_define_method(rb_mDigest_Instance, "hexdigest", rb_digest_instance_hexdigest, -1);
    rb_define_method(rb_mDigest_Instance, "hexdigest!", rb_digest_instance_hexdigest_bang, 0);
    rb_define_method(rb_mDigest_Instance, "to_s", rb_digest_instance_to_s, 0);
    rb_define_method(rb_mDigest_Instance, "length", rb_digest_instance_length, 0);
    rb_define_method(rb_mDigest_Instance, "size", rb_digest_instance_length, 0);

    /*
     * class Digest::Class
     */
    rb_cDigest_Class = rb_define_class_under(rb_mDigest, "Class", rb_cObject);
    rb_define_method(rb_cDigest_Class, "initialize",  rb_digest_class_init, 0);
    rb_include_module(rb_cDigest_Class, rb_mDigest_Instance);

    /* class methods */
    rb_define_singleton_method(rb_cDigest_Class, "digest", rb_digest_class_s_digest, -1);
    rb_define_singleton_method(rb_cDigest_Class, "hexdigest", rb_digest_class_s_hexdigest, -1);

    id_metadata = rb_intern("metadata");

    /* class Digest::Base < Digest::Class */
    rb_cDigest_Base = rb_define_class_under(rb_mDigest, "Base", rb_cDigest_Class);

    rb_define_alloc_func(rb_cDigest_Base, rb_digest_base_alloc);

    rb_define_method(rb_cDigest_Base, "initialize_copy",  rb_digest_base_copy, 1);
    rb_define_method(rb_cDigest_Base, "reset", rb_digest_base_reset, 0);
    rb_define_method(rb_cDigest_Base, "update", rb_digest_base_update, 1);
    rb_define_method(rb_cDigest_Base, "<<", rb_digest_base_update, 1);
    rb_define_private_method(rb_cDigest_Base, "finish", rb_digest_base_finish, 0);
    rb_define_method(rb_cDigest_Base, "digest_length", rb_digest_base_digest_length, 0);
    rb_define_method(rb_cDigest_Base, "block_length", rb_digest_base_block_length, 0);
}
