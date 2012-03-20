/*
 * $Id$
 * 'OpenSSL for Ruby' project
 * Copyright (C) 2001-2002  Michal Rokos <m.rokos@sh.cvut.cz>
 * All rights reserved.
 */
/*
 * This program is licenced under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 */
#include "ossl.h"

/*
 * Classes
 */
VALUE mRandom;
VALUE eRandomError;

/*
 * Struct
 */

/*
 * Public
 */

/*
 * Private
 */

/*
 *  call-seq:
 *     seed(str) -> str
 *
 */
static VALUE
ossl_rand_seed(VALUE self, VALUE str)
{
    StringValue(str);
    RAND_seed(RSTRING_PTR(str), RSTRING_LENINT(str));

    return str;
}

/*
 *  call-seq:
 *     add(str, entropy) -> self
 *
 */
static VALUE
ossl_rand_add(VALUE self, VALUE str, VALUE entropy)
{
    StringValue(str);
    RAND_add(RSTRING_PTR(str), RSTRING_LENINT(str), NUM2DBL(entropy));

    return self;
}

/*
 *  call-seq:
 *     load_random_file(filename) -> true
 *
 */
static VALUE
ossl_rand_load_file(VALUE self, VALUE filename)
{
    SafeStringValue(filename);

    if(!RAND_load_file(RSTRING_PTR(filename), -1)) {
	ossl_raise(eRandomError, NULL);
    }
    return Qtrue;
}

/*
 *  call-seq:
 *     write_random_file(filename) -> true
 *
 */
static VALUE
ossl_rand_write_file(VALUE self, VALUE filename)
{
    SafeStringValue(filename);
    if (RAND_write_file(RSTRING_PTR(filename)) == -1) {
	ossl_raise(eRandomError, NULL);
    }
    return Qtrue;
}

/*
 *  call-seq:
 *     random_bytes(length) -> aString
 *
 */
static VALUE
ossl_rand_bytes(VALUE self, VALUE len)
{
    VALUE str;
    int n = NUM2INT(len);

    str = rb_str_new(0, n);
    if (!RAND_bytes((unsigned char *)RSTRING_PTR(str), n)) {
	ossl_raise(eRandomError, NULL);
    }

    return str;
}

/*
 *  call-seq:
 *     pseudo_bytes(length) -> aString
 *
 */
static VALUE
ossl_rand_pseudo_bytes(VALUE self, VALUE len)
{
    VALUE str;
    int n = NUM2INT(len);

    str = rb_str_new(0, n);
    if (!RAND_pseudo_bytes((unsigned char *)RSTRING_PTR(str), n)) {
	ossl_raise(eRandomError, NULL);
    }

    return str;
}

/*
 *  call-seq:
 *     egd(filename) -> true
 *
 */
static VALUE
ossl_rand_egd(VALUE self, VALUE filename)
{
    SafeStringValue(filename);

    if(!RAND_egd(RSTRING_PTR(filename))) {
	ossl_raise(eRandomError, NULL);
    }
    return Qtrue;
}

/*
 *  call-seq:
 *     egd_bytes(filename, length) -> true
 *
 */
static VALUE
ossl_rand_egd_bytes(VALUE self, VALUE filename, VALUE len)
{
    int n = NUM2INT(len);

    SafeStringValue(filename);

    if (!RAND_egd_bytes(RSTRING_PTR(filename), n)) {
	ossl_raise(eRandomError, NULL);
    }
    return Qtrue;
}

/*
 *  call-seq:
 *     status? => true | false
 *
 * Return true if the PRNG has been seeded with enough data, false otherwise.
 */
static VALUE
ossl_rand_status(VALUE self)
{
    return RAND_status() ? Qtrue : Qfalse;
}

#define DEFMETH(class, name, func, argc) \
	rb_define_method((class), (name), (func), (argc)); \
	rb_define_singleton_method((class), (name), (func), (argc));

/*
 * INIT
 */
void
Init_ossl_rand()
{
#if 0
    mOSSL = rb_define_module("OpenSSL"); /* let rdoc know about mOSSL */
#endif

    mRandom = rb_define_module_under(mOSSL, "Random");

    eRandomError = rb_define_class_under(mRandom, "RandomError", eOSSLError);

    DEFMETH(mRandom, "seed", ossl_rand_seed, 1);
    DEFMETH(mRandom, "random_add", ossl_rand_add, 2);
    DEFMETH(mRandom, "load_random_file", ossl_rand_load_file, 1);
    DEFMETH(mRandom, "write_random_file", ossl_rand_write_file, 1);
    DEFMETH(mRandom, "random_bytes", ossl_rand_bytes, 1);
    DEFMETH(mRandom, "pseudo_bytes", ossl_rand_pseudo_bytes, 1);
    DEFMETH(mRandom, "egd", ossl_rand_egd, 1);
    DEFMETH(mRandom, "egd_bytes", ossl_rand_egd_bytes, 2);
    DEFMETH(mRandom, "status?", ossl_rand_status, 0)
}

