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

#define WrapX509Req(klass, obj, req) do { \
    if (!(req)) { \
	ossl_raise(rb_eRuntimeError, "Req wasn't initialized!"); \
    } \
    (obj) = Data_Wrap_Struct((klass), 0, X509_REQ_free, (req)); \
} while (0)
#define GetX509Req(obj, req) do { \
    Data_Get_Struct((obj), X509_REQ, (req)); \
    if (!(req)) { \
	ossl_raise(rb_eRuntimeError, "Req wasn't initialized!"); \
    } \
} while (0)
#define SafeGetX509Req(obj, req) do { \
    OSSL_Check_Kind((obj), cX509Req); \
    GetX509Req((obj), (req)); \
} while (0)

/*
 * Classes
 */
VALUE cX509Req;
VALUE eX509ReqError;

/*
 * Public functions
 */
VALUE
ossl_x509req_new(X509_REQ *req)
{
    X509_REQ *new;
    VALUE obj;

    if (!req) {
	new = X509_REQ_new();
    } else {
	new = X509_REQ_dup(req);
    }
    if (!new) {
	ossl_raise(eX509ReqError, NULL);
    }
    WrapX509Req(cX509Req, obj, new);

    return obj;
}

X509_REQ *
GetX509ReqPtr(VALUE obj)
{
    X509_REQ *req;

    SafeGetX509Req(obj, req);

    return req;
}

X509_REQ *
DupX509ReqPtr(VALUE obj)
{
    X509_REQ *req, *new;

    SafeGetX509Req(obj, req);
    if (!(new = X509_REQ_dup(req))) {
	ossl_raise(eX509ReqError, NULL);
    }

    return new;
}

/*
 * Private functions
 */
static VALUE
ossl_x509req_alloc(VALUE klass)
{
    X509_REQ *req;
    VALUE obj;

    if (!(req = X509_REQ_new())) {
	ossl_raise(eX509ReqError, NULL);
    }
    WrapX509Req(klass, obj, req);

    return obj;
}

static VALUE
ossl_x509req_initialize(int argc, VALUE *argv, VALUE self)
{
    BIO *in;
    X509_REQ *req, *x = DATA_PTR(self);
    VALUE arg;

    if (rb_scan_args(argc, argv, "01", &arg) == 0) {
	return self;
    }
    arg = ossl_to_der_if_possible(arg);
    in = ossl_obj2bio(arg);
    req = PEM_read_bio_X509_REQ(in, &x, NULL, NULL);
    DATA_PTR(self) = x;
    if (!req) {
	OSSL_BIO_reset(in);
	req = d2i_X509_REQ_bio(in, &x);
	DATA_PTR(self) = x;
    }
    BIO_free(in);
    if (!req) ossl_raise(eX509ReqError, NULL);

    return self;
}

static VALUE
ossl_x509req_copy(VALUE self, VALUE other)
{
    X509_REQ *a, *b, *req;

    rb_check_frozen(self);
    if (self == other) return self;
    GetX509Req(self, a);
    SafeGetX509Req(other, b);
    if (!(req = X509_REQ_dup(b))) {
	ossl_raise(eX509ReqError, NULL);
    }
    X509_REQ_free(a);
    DATA_PTR(self) = req;

    return self;
}

static VALUE
ossl_x509req_to_pem(VALUE self)
{
    X509_REQ *req;
    BIO *out;
    BUF_MEM *buf;
    VALUE str;

    GetX509Req(self, req);
    if (!(out = BIO_new(BIO_s_mem()))) {
	ossl_raise(eX509ReqError, NULL);
    }
    if (!PEM_write_bio_X509_REQ(out, req)) {
	BIO_free(out);
	ossl_raise(eX509ReqError, NULL);
    }
    BIO_get_mem_ptr(out, &buf);
    str = rb_str_new(buf->data, buf->length);
    BIO_free(out);

    return str;
}

static VALUE
ossl_x509req_to_der(VALUE self)
{
    X509_REQ *req;
    VALUE str;
    long len;
    unsigned char *p;

    GetX509Req(self, req);
    if ((len = i2d_X509_REQ(req, NULL)) <= 0)
	ossl_raise(eX509ReqError, NULL);
    str = rb_str_new(0, len);
    p = (unsigned char *)RSTRING_PTR(str);
    if (i2d_X509_REQ(req, &p) <= 0)
	ossl_raise(eX509ReqError, NULL);
    ossl_str_adjust(str, p);

    return str;
}

static VALUE
ossl_x509req_to_text(VALUE self)
{
    X509_REQ *req;
    BIO *out;
    BUF_MEM *buf;
    VALUE str;

    GetX509Req(self, req);
    if (!(out = BIO_new(BIO_s_mem()))) {
	ossl_raise(eX509ReqError, NULL);
    }
    if (!X509_REQ_print(out, req)) {
	BIO_free(out);
	ossl_raise(eX509ReqError, NULL);
    }
    BIO_get_mem_ptr(out, &buf);
    str = rb_str_new(buf->data, buf->length);
    BIO_free(out);

    return str;
}

#if 0
/*
 * Makes X509 from X509_REQuest
 */
static VALUE
ossl_x509req_to_x509(VALUE self, VALUE days, VALUE key)
{
    X509_REQ *req;
    X509 *x509;

    GetX509Req(self, req);
    ...
    if (!(x509 = X509_REQ_to_X509(req, d, pkey))) {
	ossl_raise(eX509ReqError, NULL);
    }

    return ossl_x509_new(x509);
}
#endif

static VALUE
ossl_x509req_get_version(VALUE self)
{
    X509_REQ *req;
    long version;

    GetX509Req(self, req);
    version = X509_REQ_get_version(req);

    return LONG2FIX(version);
}

static VALUE
ossl_x509req_set_version(VALUE self, VALUE version)
{
    X509_REQ *req;
    long ver;

    if ((ver = FIX2LONG(version)) < 0) {
	ossl_raise(eX509ReqError, "version must be >= 0!");
    }
    GetX509Req(self, req);
    if (!X509_REQ_set_version(req, ver)) {
	ossl_raise(eX509ReqError, NULL);
    }

    return version;
}

static VALUE
ossl_x509req_get_subject(VALUE self)
{
    X509_REQ *req;
    X509_NAME *name;

    GetX509Req(self, req);
    if (!(name = X509_REQ_get_subject_name(req))) { /* NO DUP - don't free */
	ossl_raise(eX509ReqError, NULL);
    }

    return ossl_x509name_new(name);
}

static VALUE
ossl_x509req_set_subject(VALUE self, VALUE subject)
{
    X509_REQ *req;

    GetX509Req(self, req);
    /* DUPs name */
    if (!X509_REQ_set_subject_name(req, GetX509NamePtr(subject))) {
	ossl_raise(eX509ReqError, NULL);
    }

    return subject;
}

static VALUE
ossl_x509req_get_signature_algorithm(VALUE self)
{
    X509_REQ *req;
    BIO *out;
    BUF_MEM *buf;
    VALUE str;

    GetX509Req(self, req);

    if (!(out = BIO_new(BIO_s_mem()))) {
	ossl_raise(eX509ReqError, NULL);
    }
    if (!i2a_ASN1_OBJECT(out, req->sig_alg->algorithm)) {
	BIO_free(out);
	ossl_raise(eX509ReqError, NULL);
    }
    BIO_get_mem_ptr(out, &buf);
    str = rb_str_new(buf->data, buf->length);
    BIO_free(out);
    return str;
}

static VALUE
ossl_x509req_get_public_key(VALUE self)
{
    X509_REQ *req;
    EVP_PKEY *pkey;

    GetX509Req(self, req);
    if (!(pkey = X509_REQ_get_pubkey(req))) { /* adds reference */
	ossl_raise(eX509ReqError, NULL);
    }

    return ossl_pkey_new(pkey); /* NO DUP - OK */
}

static VALUE
ossl_x509req_set_public_key(VALUE self, VALUE key)
{
    X509_REQ *req;
    EVP_PKEY *pkey;

    GetX509Req(self, req);
    pkey = GetPKeyPtr(key); /* NO NEED TO DUP */
    if (!X509_REQ_set_pubkey(req, pkey)) {
	ossl_raise(eX509ReqError, NULL);
    }

    return key;
}

static VALUE
ossl_x509req_sign(VALUE self, VALUE key, VALUE digest)
{
    X509_REQ *req;
    EVP_PKEY *pkey;
    const EVP_MD *md;

    GetX509Req(self, req);
    pkey = GetPrivPKeyPtr(key); /* NO NEED TO DUP */
    md = GetDigestPtr(digest);
    if (!X509_REQ_sign(req, pkey, md)) {
	ossl_raise(eX509ReqError, NULL);
    }

    return self;
}

/*
 * Checks that cert signature is made with PRIVversion of this PUBLIC 'key'
 */
static VALUE
ossl_x509req_verify(VALUE self, VALUE key)
{
    X509_REQ *req;
    EVP_PKEY *pkey;
    int i;

    GetX509Req(self, req);
    pkey = GetPKeyPtr(key); /* NO NEED TO DUP */
    if ((i = X509_REQ_verify(req, pkey)) < 0) {
	ossl_raise(eX509ReqError, NULL);
    }
    if (i > 0) {
	return Qtrue;
    }

    return Qfalse;
}

static VALUE
ossl_x509req_get_attributes(VALUE self)
{
    X509_REQ *req;
    int count, i;
    X509_ATTRIBUTE *attr;
    VALUE ary;

    GetX509Req(self, req);

    count = X509_REQ_get_attr_count(req);
    if (count < 0) {
	OSSL_Debug("count < 0???");
	return rb_ary_new();
    }
    ary = rb_ary_new2(count);
    for (i=0; i<count; i++) {
	attr = X509_REQ_get_attr(req, i);
	rb_ary_push(ary, ossl_x509attr_new(attr));
    }

    return ary;
}

static VALUE
ossl_x509req_set_attributes(VALUE self, VALUE ary)
{
    X509_REQ *req;
    X509_ATTRIBUTE *attr;
    int i;
    VALUE item;

    Check_Type(ary, T_ARRAY);
    for (i=0;i<RARRAY_LEN(ary); i++) {
	OSSL_Check_Kind(RARRAY_PTR(ary)[i], cX509Attr);
    }
    GetX509Req(self, req);
    sk_X509_ATTRIBUTE_pop_free(req->req_info->attributes, X509_ATTRIBUTE_free);
    req->req_info->attributes = NULL;
    for (i=0;i<RARRAY_LEN(ary); i++) {
	item = RARRAY_PTR(ary)[i];
	attr = DupX509AttrPtr(item);
	if (!X509_REQ_add1_attr(req, attr)) {
	    ossl_raise(eX509ReqError, NULL);
	}
    }
    return ary;
}

static VALUE
ossl_x509req_add_attribute(VALUE self, VALUE attr)
{
    X509_REQ *req;

    GetX509Req(self, req);
    if (!X509_REQ_add1_attr(req, DupX509AttrPtr(attr))) {
	ossl_raise(eX509ReqError, NULL);
    }

    return attr;
}

/*
 * X509_REQUEST init
 */
void
Init_ossl_x509req()
{
    eX509ReqError = rb_define_class_under(mX509, "RequestError", eOSSLError);

    cX509Req = rb_define_class_under(mX509, "Request", rb_cObject);

    rb_define_alloc_func(cX509Req, ossl_x509req_alloc);
    rb_define_method(cX509Req, "initialize", ossl_x509req_initialize, -1);
    rb_define_copy_func(cX509Req, ossl_x509req_copy);

    rb_define_method(cX509Req, "to_pem", ossl_x509req_to_pem, 0);
    rb_define_method(cX509Req, "to_der", ossl_x509req_to_der, 0);
    rb_define_alias(cX509Req, "to_s", "to_pem");
    rb_define_method(cX509Req, "to_text", ossl_x509req_to_text, 0);
    rb_define_method(cX509Req, "version", ossl_x509req_get_version, 0);
    rb_define_method(cX509Req, "version=", ossl_x509req_set_version, 1);
    rb_define_method(cX509Req, "subject", ossl_x509req_get_subject, 0);
    rb_define_method(cX509Req, "subject=", ossl_x509req_set_subject, 1);
    rb_define_method(cX509Req, "signature_algorithm", ossl_x509req_get_signature_algorithm, 0);
    rb_define_method(cX509Req, "public_key", ossl_x509req_get_public_key, 0);
    rb_define_method(cX509Req, "public_key=", ossl_x509req_set_public_key, 1);
    rb_define_method(cX509Req, "sign", ossl_x509req_sign, 2);
    rb_define_method(cX509Req, "verify", ossl_x509req_verify, 1);
    rb_define_method(cX509Req, "attributes", ossl_x509req_get_attributes, 0);
    rb_define_method(cX509Req, "attributes=", ossl_x509req_set_attributes, 1);
    rb_define_method(cX509Req, "add_attribute", ossl_x509req_add_attribute, 1);
}

