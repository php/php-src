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

#define WrapX509Rev(klass, obj, rev) do { \
    if (!(rev)) { \
	ossl_raise(rb_eRuntimeError, "REV wasn't initialized!"); \
    } \
    (obj) = Data_Wrap_Struct((klass), 0, X509_REVOKED_free, (rev)); \
} while (0)
#define GetX509Rev(obj, rev) do { \
    Data_Get_Struct((obj), X509_REVOKED, (rev)); \
    if (!(rev)) { \
	ossl_raise(rb_eRuntimeError, "REV wasn't initialized!"); \
    } \
} while (0)
#define SafeGetX509Rev(obj, rev) do { \
    OSSL_Check_Kind((obj), cX509Rev); \
    GetX509Rev((obj), (rev)); \
} while (0)

/*
 * Classes
 */
VALUE cX509Rev;
VALUE eX509RevError;

/*
 * PUBLIC
 */
VALUE
ossl_x509revoked_new(X509_REVOKED *rev)
{
    X509_REVOKED *new;
    VALUE obj;

    if (!rev) {
	new = X509_REVOKED_new();
    } else {
	new = X509_REVOKED_dup(rev);
    }
    if (!new) {
	ossl_raise(eX509RevError, NULL);
    }
    WrapX509Rev(cX509Rev, obj, new);

    return obj;
}

X509_REVOKED *
DupX509RevokedPtr(VALUE obj)
{
    X509_REVOKED *rev, *new;

    SafeGetX509Rev(obj, rev);
    if (!(new = X509_REVOKED_dup(rev))) {
	ossl_raise(eX509RevError, NULL);
    }

    return new;
}

/*
 * PRIVATE
 */
static VALUE
ossl_x509revoked_alloc(VALUE klass)
{
    X509_REVOKED *rev;
    VALUE obj;

    if (!(rev = X509_REVOKED_new())) {
	ossl_raise(eX509RevError, NULL);
    }
    WrapX509Rev(klass, obj, rev);

    return obj;
}

static VALUE
ossl_x509revoked_initialize(int argc, VALUE *argv, VALUE self)
{
    /* EMPTY */
    return self;
}

static VALUE
ossl_x509revoked_get_serial(VALUE self)
{
    X509_REVOKED *rev;

    GetX509Rev(self, rev);

    return asn1integer_to_num(rev->serialNumber);
}

static VALUE
ossl_x509revoked_set_serial(VALUE self, VALUE num)
{
    X509_REVOKED *rev;

    GetX509Rev(self, rev);
    rev->serialNumber = num_to_asn1integer(num, rev->serialNumber);

    return num;
}

static VALUE
ossl_x509revoked_get_time(VALUE self)
{
    X509_REVOKED *rev;

    GetX509Rev(self, rev);

    return asn1time_to_time(rev->revocationDate);
}

static VALUE
ossl_x509revoked_set_time(VALUE self, VALUE time)
{
    X509_REVOKED *rev;
    time_t sec;

    sec = time_to_time_t(time);
    GetX509Rev(self, rev);
    if (!X509_time_adj(rev->revocationDate, 0, &sec)) {
	ossl_raise(eX509RevError, NULL);
    }

    return time;
}
/*
 * Gets X509v3 extensions as array of X509Ext objects
 */
static VALUE
ossl_x509revoked_get_extensions(VALUE self)
{
    X509_REVOKED *rev;
    int count, i;
    X509_EXTENSION *ext;
    VALUE ary;

    GetX509Rev(self, rev);
    count = X509_REVOKED_get_ext_count(rev);
    if (count < 0) {
	OSSL_Debug("count < 0???");
	return rb_ary_new();
    }
    ary = rb_ary_new2(count);
    for (i=0; i<count; i++) {
	ext = X509_REVOKED_get_ext(rev, i);
	rb_ary_push(ary, ossl_x509ext_new(ext));
    }

    return ary;
}

/*
 * Sets X509_EXTENSIONs
 */
static VALUE
ossl_x509revoked_set_extensions(VALUE self, VALUE ary)
{
    X509_REVOKED *rev;
    X509_EXTENSION *ext;
    int i;
    VALUE item;

    Check_Type(ary, T_ARRAY);
    for (i=0; i<RARRAY_LEN(ary); i++) {
	OSSL_Check_Kind(RARRAY_PTR(ary)[i], cX509Ext);
    }
    GetX509Rev(self, rev);
    sk_X509_EXTENSION_pop_free(rev->extensions, X509_EXTENSION_free);
    rev->extensions = NULL;
    for (i=0; i<RARRAY_LEN(ary); i++) {
	item = RARRAY_PTR(ary)[i];
	ext = DupX509ExtPtr(item);
	if(!X509_REVOKED_add_ext(rev, ext, -1)) {
	    ossl_raise(eX509RevError, NULL);
	}
    }

    return ary;
}

static VALUE
ossl_x509revoked_add_extension(VALUE self, VALUE ext)
{
    X509_REVOKED *rev;

    GetX509Rev(self, rev);
    if(!X509_REVOKED_add_ext(rev, DupX509ExtPtr(ext), -1)) {
	ossl_raise(eX509RevError, NULL);
    }

    return ext;
}

/*
 * INIT
 */
void
Init_ossl_x509revoked()
{
    eX509RevError = rb_define_class_under(mX509, "RevokedError", eOSSLError);

    cX509Rev = rb_define_class_under(mX509, "Revoked", rb_cObject);

    rb_define_alloc_func(cX509Rev, ossl_x509revoked_alloc);
    rb_define_method(cX509Rev, "initialize", ossl_x509revoked_initialize, -1);

    rb_define_method(cX509Rev, "serial", ossl_x509revoked_get_serial, 0);
    rb_define_method(cX509Rev, "serial=", ossl_x509revoked_set_serial, 1);
    rb_define_method(cX509Rev, "time", ossl_x509revoked_get_time, 0);
    rb_define_method(cX509Rev, "time=", ossl_x509revoked_set_time, 1);
    rb_define_method(cX509Rev, "extensions", ossl_x509revoked_get_extensions, 0);
    rb_define_method(cX509Rev, "extensions=", ossl_x509revoked_set_extensions, 1);
    rb_define_method(cX509Rev, "add_extension", ossl_x509revoked_add_extension, 1);
}

