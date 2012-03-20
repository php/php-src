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
#if !defined(OPENSSL_NO_HMAC)

#include "ossl.h"

#define MakeHMAC(obj, klass, ctx) \
    (obj) = Data_Make_Struct((klass), HMAC_CTX, 0, ossl_hmac_free, (ctx))
#define GetHMAC(obj, ctx) do { \
    Data_Get_Struct((obj), HMAC_CTX, (ctx)); \
    if (!(ctx)) { \
	ossl_raise(rb_eRuntimeError, "HMAC wasn't initialized"); \
    } \
} while (0)
#define SafeGetHMAC(obj, ctx) do { \
    OSSL_Check_Kind((obj), cHMAC); \
    GetHMAC((obj), (ctx)); \
} while (0)

/*
 * Classes
 */
VALUE cHMAC;
VALUE eHMACError;

/*
 * Public
 */

/*
 * Private
 */
static void
ossl_hmac_free(HMAC_CTX *ctx)
{
    HMAC_CTX_cleanup(ctx);
    ruby_xfree(ctx);
}

static VALUE
ossl_hmac_alloc(VALUE klass)
{
    HMAC_CTX *ctx;
    VALUE obj;

    MakeHMAC(obj, klass, ctx);
    HMAC_CTX_init(ctx);

    return obj;
}


/*
 *  call-seq:
 *     HMAC.new(key, digest) -> hmac
 *
 */
static VALUE
ossl_hmac_initialize(VALUE self, VALUE key, VALUE digest)
{
    HMAC_CTX *ctx;

    StringValue(key);
    GetHMAC(self, ctx);
    HMAC_Init(ctx, RSTRING_PTR(key), RSTRING_LENINT(key),
		 GetDigestPtr(digest));

    return self;
}

static VALUE
ossl_hmac_copy(VALUE self, VALUE other)
{
    HMAC_CTX *ctx1, *ctx2;

    rb_check_frozen(self);
    if (self == other) return self;

    GetHMAC(self, ctx1);
    SafeGetHMAC(other, ctx2);

    HMAC_CTX_copy(ctx1, ctx2);
    return self;
}

/*
 *  call-seq:
 *     hmac.update(string) -> self
 *
 */
static VALUE
ossl_hmac_update(VALUE self, VALUE data)
{
    HMAC_CTX *ctx;

    StringValue(data);
    GetHMAC(self, ctx);
    HMAC_Update(ctx, (unsigned char *)RSTRING_PTR(data), RSTRING_LEN(data));

    return self;
}

static void
hmac_final(HMAC_CTX *ctx, unsigned char **buf, unsigned int *buf_len)
{
    HMAC_CTX final;

    HMAC_CTX_copy(&final, ctx);
    if (!(*buf = OPENSSL_malloc(HMAC_size(&final)))) {
	HMAC_CTX_cleanup(&final);
	OSSL_Debug("Allocating %d mem", HMAC_size(&final));
	ossl_raise(eHMACError, "Cannot allocate memory for hmac");
    }
    HMAC_Final(&final, *buf, buf_len);
    HMAC_CTX_cleanup(&final);
}

/*
 *  call-seq:
 *     hmac.digest -> aString
 *
 */
static VALUE
ossl_hmac_digest(VALUE self)
{
    HMAC_CTX *ctx;
    unsigned char *buf;
    unsigned int buf_len;
    VALUE digest;

    GetHMAC(self, ctx);
    hmac_final(ctx, &buf, &buf_len);
    digest = ossl_buf2str((char *)buf, buf_len);

    return digest;
}

/*
 *  call-seq:
 *     hmac.hexdigest -> aString
 *
 */
static VALUE
ossl_hmac_hexdigest(VALUE self)
{
    HMAC_CTX *ctx;
    unsigned char *buf;
    char *hexbuf;
    unsigned int buf_len;
    VALUE hexdigest;

    GetHMAC(self, ctx);
    hmac_final(ctx, &buf, &buf_len);
    if (string2hex(buf, buf_len, &hexbuf, NULL) != 2 * (int)buf_len) {
	OPENSSL_free(buf);
	ossl_raise(eHMACError, "Memory alloc error");
    }
    OPENSSL_free(buf);
    hexdigest = ossl_buf2str(hexbuf, 2 * buf_len);

    return hexdigest;
}

/*
 *  call-seq:
 *     hmac.reset -> self
 *
 */
static VALUE
ossl_hmac_reset(VALUE self)
{
    HMAC_CTX *ctx;

    GetHMAC(self, ctx);
    HMAC_Init(ctx, NULL, 0, NULL);

    return self;
}

/*
 *  call-seq:
 *     HMAC.digest(digest, key, data) -> aString
 *
 */
static VALUE
ossl_hmac_s_digest(VALUE klass, VALUE digest, VALUE key, VALUE data)
{
    unsigned char *buf;
    unsigned int buf_len;

    StringValue(key);
    StringValue(data);
    buf = HMAC(GetDigestPtr(digest), RSTRING_PTR(key), RSTRING_LENINT(key),
	       (unsigned char *)RSTRING_PTR(data), RSTRING_LEN(data), NULL, &buf_len);

    return rb_str_new((const char *)buf, buf_len);
}

/*
 *  call-seq:
 *     HMAC.digest(digest, key, data) -> aString
 *
 */
static VALUE
ossl_hmac_s_hexdigest(VALUE klass, VALUE digest, VALUE key, VALUE data)
{
    unsigned char *buf;
    char *hexbuf;
    unsigned int buf_len;
    VALUE hexdigest;

    StringValue(key);
    StringValue(data);

    buf = HMAC(GetDigestPtr(digest), RSTRING_PTR(key), RSTRING_LENINT(key),
	       (unsigned char *)RSTRING_PTR(data), RSTRING_LEN(data), NULL, &buf_len);
    if (string2hex(buf, buf_len, &hexbuf, NULL) != 2 * (int)buf_len) {
	ossl_raise(eHMACError, "Cannot convert buf to hexbuf");
    }
    hexdigest = ossl_buf2str(hexbuf, 2 * buf_len);

    return hexdigest;
}

/*
 * INIT
 */
void
Init_ossl_hmac()
{
#if 0
    mOSSL = rb_define_module("OpenSSL"); /* let rdoc know about mOSSL */
#endif

    eHMACError = rb_define_class_under(mOSSL, "HMACError", eOSSLError);

    cHMAC = rb_define_class_under(mOSSL, "HMAC", rb_cObject);

    rb_define_alloc_func(cHMAC, ossl_hmac_alloc);
    rb_define_singleton_method(cHMAC, "digest", ossl_hmac_s_digest, 3);
    rb_define_singleton_method(cHMAC, "hexdigest", ossl_hmac_s_hexdigest, 3);

    rb_define_method(cHMAC, "initialize", ossl_hmac_initialize, 2);
    rb_define_copy_func(cHMAC, ossl_hmac_copy);

    rb_define_method(cHMAC, "reset", ossl_hmac_reset, 0);
    rb_define_method(cHMAC, "update", ossl_hmac_update, 1);
    rb_define_alias(cHMAC, "<<", "update");
    rb_define_method(cHMAC, "digest", ossl_hmac_digest, 0);
    rb_define_method(cHMAC, "hexdigest", ossl_hmac_hexdigest, 0);
    rb_define_alias(cHMAC, "inspect", "hexdigest");
    rb_define_alias(cHMAC, "to_s", "hexdigest");
}

#else /* NO_HMAC */
#  warning >>> OpenSSL is compiled without HMAC support <<<
void
Init_ossl_hmac()
{
    rb_warning("HMAC will NOT be avaible: OpenSSL is compiled without HMAC.");
}
#endif /* NO_HMAC */
