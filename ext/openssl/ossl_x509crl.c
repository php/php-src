/*
 * $Id$
 * 'OpenSSL for Ruby' project
 * Copyright (C) 2001-2002 Michal Rokos <m.rokos@sh.cvut.cz>
 * All rights reserved.
 */
/*
 * This program is licenced under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 */
#include "ossl.h"

#define WrapX509CRL(klass, obj, crl) do { \
    if (!(crl)) { \
	ossl_raise(rb_eRuntimeError, "CRL wasn't initialized!"); \
    } \
    (obj) = Data_Wrap_Struct((klass), 0, X509_CRL_free, (crl)); \
} while (0)
#define GetX509CRL(obj, crl) do { \
    Data_Get_Struct((obj), X509_CRL, (crl)); \
    if (!(crl)) { \
	ossl_raise(rb_eRuntimeError, "CRL wasn't initialized!"); \
    } \
} while (0)
#define SafeGetX509CRL(obj, crl) do { \
    OSSL_Check_Kind((obj), cX509CRL); \
    GetX509CRL((obj), (crl)); \
} while (0)

/*
 * Classes
 */
VALUE cX509CRL;
VALUE eX509CRLError;

/*
 * PUBLIC
 */
X509_CRL *
GetX509CRLPtr(VALUE obj)
{
    X509_CRL *crl;

    SafeGetX509CRL(obj, crl);

    return crl;
}

X509_CRL *
DupX509CRLPtr(VALUE obj)
{
    X509_CRL *crl;

    SafeGetX509CRL(obj, crl);
    CRYPTO_add(&crl->references, 1, CRYPTO_LOCK_X509_CRL);

    return crl;
}

VALUE
ossl_x509crl_new(X509_CRL *crl)
{
    X509_CRL *tmp;
    VALUE obj;

    tmp = crl ? X509_CRL_dup(crl) : X509_CRL_new();
    if(!tmp) ossl_raise(eX509CRLError, NULL);
    WrapX509CRL(cX509CRL, obj, tmp);

    return obj;
}

/*
 * PRIVATE
 */
static VALUE
ossl_x509crl_alloc(VALUE klass)
{
    X509_CRL *crl;
    VALUE obj;

    if (!(crl = X509_CRL_new())) {
	ossl_raise(eX509CRLError, NULL);
    }
    WrapX509CRL(klass, obj, crl);

    return obj;
}

static VALUE
ossl_x509crl_initialize(int argc, VALUE *argv, VALUE self)
{
    BIO *in;
    X509_CRL *crl, *x = DATA_PTR(self);
    VALUE arg;

    if (rb_scan_args(argc, argv, "01", &arg) == 0) {
	return self;
    }
    arg = ossl_to_der_if_possible(arg);
    in = ossl_obj2bio(arg);
    crl = PEM_read_bio_X509_CRL(in, &x, NULL, NULL);
    DATA_PTR(self) = x;
    if (!crl) {
	OSSL_BIO_reset(in);
	crl = d2i_X509_CRL_bio(in, &x);
	DATA_PTR(self) = x;
    }
    BIO_free(in);
    if (!crl) ossl_raise(eX509CRLError, NULL);

    return self;
}

static VALUE
ossl_x509crl_copy(VALUE self, VALUE other)
{
    X509_CRL *a, *b, *crl;

    rb_check_frozen(self);
    if (self == other) return self;
    GetX509CRL(self, a);
    SafeGetX509CRL(other, b);
    if (!(crl = X509_CRL_dup(b))) {
	ossl_raise(eX509CRLError, NULL);
    }
    X509_CRL_free(a);
    DATA_PTR(self) = crl;

    return self;
}

static VALUE
ossl_x509crl_get_version(VALUE self)
{
    X509_CRL *crl;
    long ver;

    GetX509CRL(self, crl);
    ver = X509_CRL_get_version(crl);

    return LONG2NUM(ver);
}

static VALUE
ossl_x509crl_set_version(VALUE self, VALUE version)
{
    X509_CRL *crl;
    long ver;

    if ((ver = NUM2LONG(version)) < 0) {
	ossl_raise(eX509CRLError, "version must be >= 0!");
    }
    GetX509CRL(self, crl);
    if (!X509_CRL_set_version(crl, ver)) {
	ossl_raise(eX509CRLError, NULL);
    }

    return version;
}

static VALUE
ossl_x509crl_get_signature_algorithm(VALUE self)
{
    X509_CRL *crl;
    BIO *out;
    BUF_MEM *buf;
    VALUE str;

    GetX509CRL(self, crl);
    if (!(out = BIO_new(BIO_s_mem()))) {
	ossl_raise(eX509CRLError, NULL);
    }
    if (!i2a_ASN1_OBJECT(out, crl->sig_alg->algorithm)) {
	BIO_free(out);
	ossl_raise(eX509CRLError, NULL);
    }
    BIO_get_mem_ptr(out, &buf);
    str = rb_str_new(buf->data, buf->length);
    BIO_free(out);
    return str;
}

static VALUE
ossl_x509crl_get_issuer(VALUE self)
{
    X509_CRL *crl;

    GetX509CRL(self, crl);

    return ossl_x509name_new(X509_CRL_get_issuer(crl)); /* NO DUP - don't free */
}

static VALUE
ossl_x509crl_set_issuer(VALUE self, VALUE issuer)
{
    X509_CRL *crl;

    GetX509CRL(self, crl);

    if (!X509_CRL_set_issuer_name(crl, GetX509NamePtr(issuer))) { /* DUPs name */
	ossl_raise(eX509CRLError, NULL);
    }
    return issuer;
}

static VALUE
ossl_x509crl_get_last_update(VALUE self)
{
    X509_CRL *crl;

    GetX509CRL(self, crl);

    return asn1time_to_time(X509_CRL_get_lastUpdate(crl));
}

static VALUE
ossl_x509crl_set_last_update(VALUE self, VALUE time)
{
    X509_CRL *crl;
    time_t sec;

    sec = time_to_time_t(time);
    GetX509CRL(self, crl);
    if (!X509_time_adj(crl->crl->lastUpdate, 0, &sec)) {
	ossl_raise(eX509CRLError, NULL);
    }

    return time;
}

static VALUE
ossl_x509crl_get_next_update(VALUE self)
{
    X509_CRL *crl;

    GetX509CRL(self, crl);

    return asn1time_to_time(X509_CRL_get_nextUpdate(crl));
}

static VALUE
ossl_x509crl_set_next_update(VALUE self, VALUE time)
{
    X509_CRL *crl;
    time_t sec;

    sec = time_to_time_t(time);
    GetX509CRL(self, crl);
    /* This must be some thinko in OpenSSL */
    if (!(crl->crl->nextUpdate = X509_time_adj(crl->crl->nextUpdate, 0, &sec))){
	ossl_raise(eX509CRLError, NULL);
    }

    return time;
}

static VALUE
ossl_x509crl_get_revoked(VALUE self)
{
    X509_CRL *crl;
    int i, num;
    X509_REVOKED *rev;
    VALUE ary, revoked;

    GetX509CRL(self, crl);
    num = sk_X509_REVOKED_num(X509_CRL_get_REVOKED(crl));
    if (num < 0) {
	OSSL_Debug("num < 0???");
	return rb_ary_new();
    }
    ary = rb_ary_new2(num);
    for(i=0; i<num; i++) {
	/* NO DUP - don't free! */
	rev = sk_X509_REVOKED_value(X509_CRL_get_REVOKED(crl), i);
	revoked = ossl_x509revoked_new(rev);
	rb_ary_push(ary, revoked);
    }

    return ary;
}

static VALUE
ossl_x509crl_set_revoked(VALUE self, VALUE ary)
{
    X509_CRL *crl;
    X509_REVOKED *rev;
    int i;

    Check_Type(ary, T_ARRAY);
    /* All ary members should be X509 Revoked */
    for (i=0; i<RARRAY_LEN(ary); i++) {
	OSSL_Check_Kind(RARRAY_PTR(ary)[i], cX509Rev);
    }
    GetX509CRL(self, crl);
    sk_X509_REVOKED_pop_free(crl->crl->revoked, X509_REVOKED_free);
    crl->crl->revoked = NULL;
    for (i=0; i<RARRAY_LEN(ary); i++) {
	rev = DupX509RevokedPtr(RARRAY_PTR(ary)[i]);
	if (!X509_CRL_add0_revoked(crl, rev)) { /* NO DUP - don't free! */
	    ossl_raise(eX509CRLError, NULL);
	}
    }
    X509_CRL_sort(crl);

    return ary;
}

static VALUE
ossl_x509crl_add_revoked(VALUE self, VALUE revoked)
{
    X509_CRL *crl;
    X509_REVOKED *rev;

    GetX509CRL(self, crl);
    rev = DupX509RevokedPtr(revoked);
    if (!X509_CRL_add0_revoked(crl, rev)) { /* NO DUP - don't free! */
	ossl_raise(eX509CRLError, NULL);
    }
    X509_CRL_sort(crl);

    return revoked;
}

static VALUE
ossl_x509crl_sign(VALUE self, VALUE key, VALUE digest)
{
    X509_CRL *crl;
    EVP_PKEY *pkey;
    const EVP_MD *md;

    GetX509CRL(self, crl);
    pkey = GetPrivPKeyPtr(key); /* NO NEED TO DUP */
    md = GetDigestPtr(digest);
    if (!X509_CRL_sign(crl, pkey, md)) {
	ossl_raise(eX509CRLError, NULL);
    }

    return self;
}

static VALUE
ossl_x509crl_verify(VALUE self, VALUE key)
{
    X509_CRL *crl;
    int ret;

    GetX509CRL(self, crl);
    if ((ret = X509_CRL_verify(crl, GetPKeyPtr(key))) < 0) {
	ossl_raise(eX509CRLError, NULL);
    }
    if (ret == 1) {
	return Qtrue;
    }

    return Qfalse;
}

static VALUE
ossl_x509crl_to_der(VALUE self)
{
    X509_CRL *crl;
    BIO *out;
    BUF_MEM *buf;
    VALUE str;

    GetX509CRL(self, crl);
    if (!(out = BIO_new(BIO_s_mem()))) {
	ossl_raise(eX509CRLError, NULL);
    }
    if (!i2d_X509_CRL_bio(out, crl)) {
	BIO_free(out);
	ossl_raise(eX509CRLError, NULL);
    }
    BIO_get_mem_ptr(out, &buf);
    str = rb_str_new(buf->data, buf->length);
    BIO_free(out);

    return str;
}

static VALUE
ossl_x509crl_to_pem(VALUE self)
{
    X509_CRL *crl;
    BIO *out;
    BUF_MEM *buf;
    VALUE str;

    GetX509CRL(self, crl);
    if (!(out = BIO_new(BIO_s_mem()))) {
	ossl_raise(eX509CRLError, NULL);
    }
    if (!PEM_write_bio_X509_CRL(out, crl)) {
	BIO_free(out);
	ossl_raise(eX509CRLError, NULL);
    }
    BIO_get_mem_ptr(out, &buf);
    str = rb_str_new(buf->data, buf->length);
    BIO_free(out);

    return str;
}

static VALUE
ossl_x509crl_to_text(VALUE self)
{
    X509_CRL *crl;
    BIO *out;
    BUF_MEM *buf;
    VALUE str;

    GetX509CRL(self, crl);
    if (!(out = BIO_new(BIO_s_mem()))) {
	ossl_raise(eX509CRLError, NULL);
    }
    if (!X509_CRL_print(out, crl)) {
	BIO_free(out);
	ossl_raise(eX509CRLError, NULL);
    }
    BIO_get_mem_ptr(out, &buf);
    str = rb_str_new(buf->data, buf->length);
    BIO_free(out);

    return str;
}

/*
 * Gets X509v3 extensions as array of X509Ext objects
 */
static VALUE
ossl_x509crl_get_extensions(VALUE self)
{
    X509_CRL *crl;
    int count, i;
    X509_EXTENSION *ext;
    VALUE ary;

    GetX509CRL(self, crl);
    count = X509_CRL_get_ext_count(crl);
    if (count < 0) {
	OSSL_Debug("count < 0???");
	return rb_ary_new();
    }
    ary = rb_ary_new2(count);
    for (i=0; i<count; i++) {
	ext = X509_CRL_get_ext(crl, i); /* NO DUP - don't free! */
	rb_ary_push(ary, ossl_x509ext_new(ext));
    }

    return ary;
}

/*
 * Sets X509_EXTENSIONs
 */
static VALUE
ossl_x509crl_set_extensions(VALUE self, VALUE ary)
{
    X509_CRL *crl;
    X509_EXTENSION *ext;
    int i;

    Check_Type(ary, T_ARRAY);
    /* All ary members should be X509 Extensions */
    for (i=0; i<RARRAY_LEN(ary); i++) {
	OSSL_Check_Kind(RARRAY_PTR(ary)[i], cX509Ext);
    }
    GetX509CRL(self, crl);
    sk_X509_EXTENSION_pop_free(crl->crl->extensions, X509_EXTENSION_free);
    crl->crl->extensions = NULL;
    for (i=0; i<RARRAY_LEN(ary); i++) {
	ext = DupX509ExtPtr(RARRAY_PTR(ary)[i]);
	if(!X509_CRL_add_ext(crl, ext, -1)) { /* DUPs ext - FREE it */
	    X509_EXTENSION_free(ext);
	    ossl_raise(eX509CRLError, NULL);
	}
	X509_EXTENSION_free(ext);
    }

    return ary;
}

static VALUE
ossl_x509crl_add_extension(VALUE self, VALUE extension)
{
    X509_CRL *crl;
    X509_EXTENSION *ext;

    GetX509CRL(self, crl);
    ext = DupX509ExtPtr(extension);
    if (!X509_CRL_add_ext(crl, ext, -1)) { /* DUPs ext - FREE it */
	X509_EXTENSION_free(ext);
	ossl_raise(eX509CRLError, NULL);
    }
    X509_EXTENSION_free(ext);

    return extension;
}

/*
 * INIT
 */
void
Init_ossl_x509crl()
{
    eX509CRLError = rb_define_class_under(mX509, "CRLError", eOSSLError);

    cX509CRL = rb_define_class_under(mX509, "CRL", rb_cObject);

    rb_define_alloc_func(cX509CRL, ossl_x509crl_alloc);
    rb_define_method(cX509CRL, "initialize", ossl_x509crl_initialize, -1);
    rb_define_copy_func(cX509CRL, ossl_x509crl_copy);

    rb_define_method(cX509CRL, "version", ossl_x509crl_get_version, 0);
    rb_define_method(cX509CRL, "version=", ossl_x509crl_set_version, 1);
    rb_define_method(cX509CRL, "signature_algorithm", ossl_x509crl_get_signature_algorithm, 0);
    rb_define_method(cX509CRL, "issuer", ossl_x509crl_get_issuer, 0);
    rb_define_method(cX509CRL, "issuer=", ossl_x509crl_set_issuer, 1);
    rb_define_method(cX509CRL, "last_update", ossl_x509crl_get_last_update, 0);
    rb_define_method(cX509CRL, "last_update=", ossl_x509crl_set_last_update, 1);
    rb_define_method(cX509CRL, "next_update", ossl_x509crl_get_next_update, 0);
    rb_define_method(cX509CRL, "next_update=", ossl_x509crl_set_next_update, 1);
    rb_define_method(cX509CRL, "revoked", ossl_x509crl_get_revoked, 0);
    rb_define_method(cX509CRL, "revoked=", ossl_x509crl_set_revoked, 1);
    rb_define_method(cX509CRL, "add_revoked", ossl_x509crl_add_revoked, 1);
    rb_define_method(cX509CRL, "sign", ossl_x509crl_sign, 2);
    rb_define_method(cX509CRL, "verify", ossl_x509crl_verify, 1);
    rb_define_method(cX509CRL, "to_der", ossl_x509crl_to_der, 0);
    rb_define_method(cX509CRL, "to_pem", ossl_x509crl_to_pem, 0);
    rb_define_alias(cX509CRL, "to_s", "to_pem");
    rb_define_method(cX509CRL, "to_text", ossl_x509crl_to_text, 0);
    rb_define_method(cX509CRL, "extensions", ossl_x509crl_get_extensions, 0);
    rb_define_method(cX509CRL, "extensions=", ossl_x509crl_set_extensions, 1);
    rb_define_method(cX509CRL, "add_extension", ossl_x509crl_add_extension, 1);
}

