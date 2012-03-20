/*
 * $Id$
 * 'OpenSSL for Ruby' project
 * Copyright (C) 2003  Michal Rokos <m.rokos@sh.cvut.cz>
 * Copyright (C) 2003  GOTOU Yuuzou <gotoyuzo@notwork.org>
 * All rights reserved.
 */
/*
 * This program is licenced under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 */
#include "ossl.h"

#if defined(OSSL_OCSP_ENABLED)

#define WrapOCSPReq(klass, obj, req) do { \
    if(!(req)) ossl_raise(rb_eRuntimeError, "Request wasn't initialized!"); \
    (obj) = Data_Wrap_Struct((klass), 0, OCSP_REQUEST_free, (req)); \
} while (0)
#define GetOCSPReq(obj, req) do { \
    Data_Get_Struct((obj), OCSP_REQUEST, (req)); \
    if(!(req)) ossl_raise(rb_eRuntimeError, "Request wasn't initialized!"); \
} while (0)
#define SafeGetOCSPReq(obj, req) do { \
    OSSL_Check_Kind((obj), cOCSPReq); \
    GetOCSPReq((obj), (req)); \
} while (0)

#define WrapOCSPRes(klass, obj, res) do { \
    if(!(res)) ossl_raise(rb_eRuntimeError, "Response wasn't initialized!"); \
    (obj) = Data_Wrap_Struct((klass), 0, OCSP_RESPONSE_free, (res)); \
} while (0)
#define GetOCSPRes(obj, res) do { \
    Data_Get_Struct((obj), OCSP_RESPONSE, (res)); \
    if(!(res)) ossl_raise(rb_eRuntimeError, "Response wasn't initialized!"); \
} while (0)
#define SafeGetOCSPRes(obj, res) do { \
    OSSL_Check_Kind((obj), cOCSPRes); \
    GetOCSPRes((obj), (res)); \
} while (0)

#define WrapOCSPBasicRes(klass, obj, res) do { \
    if(!(res)) ossl_raise(rb_eRuntimeError, "Response wasn't initialized!"); \
    (obj) = Data_Wrap_Struct((klass), 0, OCSP_BASICRESP_free, (res)); \
} while (0)
#define GetOCSPBasicRes(obj, res) do { \
    Data_Get_Struct((obj), OCSP_BASICRESP, (res)); \
    if(!(res)) ossl_raise(rb_eRuntimeError, "Response wasn't initialized!"); \
} while (0)
#define SafeGetOCSPBasicRes(obj, res) do { \
    OSSL_Check_Kind((obj), cOCSPBasicRes); \
    GetOCSPBasicRes((obj), (res)); \
} while (0)

#define WrapOCSPCertId(klass, obj, cid) do { \
    if(!(cid)) ossl_raise(rb_eRuntimeError, "Cert ID wasn't initialized!"); \
    (obj) = Data_Wrap_Struct((klass), 0, OCSP_CERTID_free, (cid)); \
} while (0)
#define GetOCSPCertId(obj, cid) do { \
    Data_Get_Struct((obj), OCSP_CERTID, (cid)); \
    if(!(cid)) ossl_raise(rb_eRuntimeError, "Cert ID wasn't initialized!"); \
} while (0)
#define SafeGetOCSPCertId(obj, cid) do { \
    OSSL_Check_Kind((obj), cOCSPCertId); \
    GetOCSPCertId((obj), (cid)); \
} while (0)

VALUE mOCSP;
VALUE eOCSPError;
VALUE cOCSPReq;
VALUE cOCSPRes;
VALUE cOCSPBasicRes;
VALUE cOCSPCertId;

/*
 * Public
 */
static VALUE
ossl_ocspcertid_new(OCSP_CERTID *cid)
{
    VALUE obj;
    WrapOCSPCertId(cOCSPCertId, obj, cid);
    return obj;
}

/*
 * OCSP::Resquest
 */
static VALUE
ossl_ocspreq_alloc(VALUE klass)
{
    OCSP_REQUEST *req;
    VALUE obj;

    if (!(req = OCSP_REQUEST_new()))
	ossl_raise(eOCSPError, NULL);
    WrapOCSPReq(klass, obj, req);

    return obj;
}

static VALUE
ossl_ocspreq_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE arg;
    const unsigned char *p;

    rb_scan_args(argc, argv, "01", &arg);
    if(!NIL_P(arg)){
	OCSP_REQUEST *req = DATA_PTR(self), *x;
	arg = ossl_to_der_if_possible(arg);
	StringValue(arg);
	p = (unsigned char*)RSTRING_PTR(arg);
	x = d2i_OCSP_REQUEST(&req, &p, RSTRING_LEN(arg));
	DATA_PTR(self) = req;
	if(!x){
	    ossl_raise(eOCSPError, "cannot load DER encoded request");
	}
    }

    return self;
}

static VALUE
ossl_ocspreq_add_nonce(int argc, VALUE *argv, VALUE self)
{
    OCSP_REQUEST *req;
    VALUE val;
    int ret;

    rb_scan_args(argc, argv, "01", &val);
    if(NIL_P(val)) {
	GetOCSPReq(self, req);
	ret = OCSP_request_add1_nonce(req, NULL, -1);
    }
    else{
	StringValue(val);
	GetOCSPReq(self, req);
	ret = OCSP_request_add1_nonce(req, (unsigned char *)RSTRING_PTR(val), RSTRING_LENINT(val));
    }
    if(!ret) ossl_raise(eOCSPError, NULL);

    return self;
}

/* Check nonce validity in a request and response.
 * Return value reflects result:
 *  1: nonces present and equal.
 *  2: nonces both absent.
 *  3: nonce present in response only.
 *  0: nonces both present and not equal.
 * -1: nonce in request only.
 *
 *  For most responders clients can check return > 0.
 *  If responder doesn't handle nonces return != 0 may be
 *  necessary. return == 0 is always an error.
 */
static VALUE
ossl_ocspreq_check_nonce(VALUE self, VALUE basic_resp)
{
    OCSP_REQUEST *req;
    OCSP_BASICRESP *bs;
    int res;

    GetOCSPReq(self, req);
    SafeGetOCSPBasicRes(basic_resp, bs);
    res = OCSP_check_nonce(req, bs);

    return INT2NUM(res);
}

static VALUE
ossl_ocspreq_add_certid(VALUE self, VALUE certid)
{
    OCSP_REQUEST *req;
    OCSP_CERTID *id;

    GetOCSPReq(self, req);
    GetOCSPCertId(certid, id);
    if(!OCSP_request_add0_id(req, OCSP_CERTID_dup(id)))
	ossl_raise(eOCSPError, NULL);

    return self;
}

static VALUE
ossl_ocspreq_get_certid(VALUE self)
{
    OCSP_REQUEST *req;
    OCSP_ONEREQ *one;
    OCSP_CERTID *id;
    VALUE ary, tmp;
    int i, count;

    GetOCSPReq(self, req);
    count = OCSP_request_onereq_count(req);
    ary = (count > 0) ? rb_ary_new() : Qnil;
    for(i = 0; i < count; i++){
	one = OCSP_request_onereq_get0(req, i);
	if(!(id = OCSP_CERTID_dup(OCSP_onereq_get0_id(one))))
	    ossl_raise(eOCSPError, NULL);
	WrapOCSPCertId(cOCSPCertId, tmp, id);
	rb_ary_push(ary, tmp);
    }

    return ary;
}

static VALUE
ossl_ocspreq_sign(int argc, VALUE *argv, VALUE self)
{
    VALUE signer_cert, signer_key, certs, flags;
    OCSP_REQUEST *req;
    X509 *signer;
    EVP_PKEY *key;
    STACK_OF(X509) *x509s;
    unsigned long flg;
    int ret;

    rb_scan_args(argc, argv, "22", &signer_cert, &signer_key, &certs, &flags);
    signer = GetX509CertPtr(signer_cert);
    key = GetPrivPKeyPtr(signer_key);
    flg = NIL_P(flags) ? 0 : NUM2INT(flags);
    if(NIL_P(certs)){
	x509s = sk_X509_new_null();
	flags |= OCSP_NOCERTS;
    }
    else x509s = ossl_x509_ary2sk(certs);
    GetOCSPReq(self, req);
    ret = OCSP_request_sign(req, signer, key, EVP_sha1(), x509s, flg);
    sk_X509_pop_free(x509s, X509_free);
    if(!ret) ossl_raise(eOCSPError, NULL);

    return self;
}

static VALUE
ossl_ocspreq_verify(int argc, VALUE *argv, VALUE self)
{
    VALUE certs, store, flags;
    OCSP_REQUEST *req;
    STACK_OF(X509) *x509s;
    X509_STORE *x509st;
    int flg, result;

    rb_scan_args(argc, argv, "21", &certs, &store, &flags);
    x509st = GetX509StorePtr(store);
    flg = NIL_P(flags) ? 0 : NUM2INT(flags);
    x509s = ossl_x509_ary2sk(certs);
    GetOCSPReq(self, req);
    result = OCSP_request_verify(req, x509s, x509st, flg);
    sk_X509_pop_free(x509s, X509_free);
    if(!result) rb_warn("%s", ERR_error_string(ERR_peek_error(), NULL));

    return result ? Qtrue : Qfalse;
}

static VALUE
ossl_ocspreq_to_der(VALUE self)
{
    OCSP_REQUEST *req;
    VALUE str;
    unsigned char *p;
    long len;

    GetOCSPReq(self, req);
    if((len = i2d_OCSP_REQUEST(req, NULL)) <= 0)
	ossl_raise(eOCSPError, NULL);
    str = rb_str_new(0, len);
    p = (unsigned char *)RSTRING_PTR(str);
    if(i2d_OCSP_REQUEST(req, &p) <= 0)
	ossl_raise(eOCSPError, NULL);
    ossl_str_adjust(str, p);

    return str;
}

/*
 * OCSP::Response
 */
static VALUE
ossl_ocspres_s_create(VALUE klass, VALUE status, VALUE basic_resp)
{
    OCSP_BASICRESP *bs;
    OCSP_RESPONSE *res;
    VALUE obj;
    int st = NUM2INT(status);

    if(NIL_P(basic_resp)) bs = NULL;
    else GetOCSPBasicRes(basic_resp, bs); /* NO NEED TO DUP */
    if(!(res = OCSP_response_create(st, bs)))
	ossl_raise(eOCSPError, NULL);
    WrapOCSPRes(klass, obj, res);

    return obj;
}

static VALUE
ossl_ocspres_alloc(VALUE klass)
{
    OCSP_RESPONSE *res;
    VALUE obj;

    if(!(res = OCSP_RESPONSE_new()))
	ossl_raise(eOCSPError, NULL);
    WrapOCSPRes(klass, obj, res);

    return obj;
}

static VALUE
ossl_ocspres_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE arg;
    const unsigned char *p;

    rb_scan_args(argc, argv, "01", &arg);
    if(!NIL_P(arg)){
	OCSP_RESPONSE *res = DATA_PTR(self), *x;
	arg = ossl_to_der_if_possible(arg);
	StringValue(arg);
	p = (unsigned char *)RSTRING_PTR(arg);
	x = d2i_OCSP_RESPONSE(&res, &p, RSTRING_LEN(arg));
	DATA_PTR(self) = res;
	if(!x){
	    ossl_raise(eOCSPError, "cannot load DER encoded response");
	}
    }

    return self;
}

static VALUE
ossl_ocspres_status(VALUE self)
{
    OCSP_RESPONSE *res;
    int st;

    GetOCSPRes(self, res);
    st = OCSP_response_status(res);

    return INT2NUM(st);
}

static VALUE
ossl_ocspres_status_string(VALUE self)
{
    OCSP_RESPONSE *res;
    int st;

    GetOCSPRes(self, res);
    st = OCSP_response_status(res);

    return rb_str_new2(OCSP_response_status_str(st));
}

static VALUE
ossl_ocspres_get_basic(VALUE self)
{
    OCSP_RESPONSE *res;
    OCSP_BASICRESP *bs;
    VALUE ret;

    GetOCSPRes(self, res);
    if(!(bs = OCSP_response_get1_basic(res)))
	return Qnil;
    WrapOCSPBasicRes(cOCSPBasicRes, ret, bs);

    return ret;
}

static VALUE
ossl_ocspres_to_der(VALUE self)
{
    OCSP_RESPONSE *res;
    VALUE str;
    long len;
    unsigned char *p;

    GetOCSPRes(self, res);
    if((len = i2d_OCSP_RESPONSE(res, NULL)) <= 0)
	ossl_raise(eOCSPError, NULL);
    str = rb_str_new(0, len);
    p = (unsigned char *)RSTRING_PTR(str);
    if(i2d_OCSP_RESPONSE(res, &p) <= 0)
	ossl_raise(eOCSPError, NULL);
    ossl_str_adjust(str, p);

    return str;
}

/*
 * OCSP::BasicResponse
 */
static VALUE
ossl_ocspbres_alloc(VALUE klass)
{
    OCSP_BASICRESP *bs;
    VALUE obj;

    if(!(bs = OCSP_BASICRESP_new()))
	ossl_raise(eOCSPError, NULL);
    WrapOCSPBasicRes(klass, obj, bs);

    return obj;
}

static VALUE
ossl_ocspbres_initialize(int argc, VALUE *argv, VALUE self)
{
    return self;
}

static VALUE
ossl_ocspbres_copy_nonce(VALUE self, VALUE request)
{
    OCSP_BASICRESP *bs;
    OCSP_REQUEST *req;
    int ret;

    GetOCSPBasicRes(self, bs);
    SafeGetOCSPReq(request, req);
    ret = OCSP_copy_nonce(bs, req);

    return INT2NUM(ret);
}

static VALUE
ossl_ocspbres_add_nonce(int argc, VALUE *argv, VALUE self)
{
    OCSP_BASICRESP *bs;
    VALUE val;
    int ret;

    rb_scan_args(argc, argv, "01", &val);
    if(NIL_P(val)) {
	GetOCSPBasicRes(self, bs);
	ret = OCSP_basic_add1_nonce(bs, NULL, -1);
    }
    else{
	StringValue(val);
	GetOCSPBasicRes(self, bs);
	ret = OCSP_basic_add1_nonce(bs, (unsigned char *)RSTRING_PTR(val), RSTRING_LENINT(val));
    }
    if(!ret) ossl_raise(eOCSPError, NULL);

    return self;
}

static VALUE
ossl_ocspbres_add_status(VALUE self, VALUE cid, VALUE status,
			 VALUE reason, VALUE revtime,
			 VALUE thisupd, VALUE nextupd, VALUE ext)
{
    OCSP_BASICRESP *bs;
    OCSP_SINGLERESP *single;
    OCSP_CERTID *id;
    int st, rsn;
    ASN1_TIME *ths, *nxt, *rev;
    int error, i, rstatus = 0;
    VALUE tmp;

    st = NUM2INT(status);
    rsn = NIL_P(status) ? 0 : NUM2INT(reason);
    if(!NIL_P(ext)){
	/* All ary's members should be X509Extension */
	Check_Type(ext, T_ARRAY);
	for (i = 0; i < RARRAY_LEN(ext); i++)
	    OSSL_Check_Kind(RARRAY_PTR(ext)[i], cX509Ext);
    }

    error = 0;
    ths = nxt = rev = NULL;
    if(!NIL_P(revtime)){
	tmp = rb_protect(rb_Integer, revtime, &rstatus);
	if(rstatus) goto err;
	rev = X509_gmtime_adj(NULL, NUM2INT(tmp));
    }
    tmp = rb_protect(rb_Integer, thisupd, &rstatus);
    if(rstatus) goto err;
    ths = X509_gmtime_adj(NULL, NUM2INT(tmp));
    tmp = rb_protect(rb_Integer, nextupd, &rstatus);
    if(rstatus) goto err;
    nxt = X509_gmtime_adj(NULL, NUM2INT(tmp));

    GetOCSPBasicRes(self, bs);
    SafeGetOCSPCertId(cid, id);
    if(!(single = OCSP_basic_add1_status(bs, id, st, rsn, rev, ths, nxt))){
	error = 1;
	goto err;
    }

    if(!NIL_P(ext)){
	X509_EXTENSION *x509ext;
	sk_X509_EXTENSION_pop_free(single->singleExtensions, X509_EXTENSION_free);
	single->singleExtensions = NULL;
	for(i = 0; i < RARRAY_LEN(ext); i++){
	    x509ext = DupX509ExtPtr(RARRAY_PTR(ext)[i]);
	    if(!OCSP_SINGLERESP_add_ext(single, x509ext, -1)){
		X509_EXTENSION_free(x509ext);
		error = 1;
		goto err;
	    }
	    X509_EXTENSION_free(x509ext);
	}
    }

 err:
    ASN1_TIME_free(ths);
    ASN1_TIME_free(nxt);
    ASN1_TIME_free(rev);
    if(error) ossl_raise(eOCSPError, NULL);
    if(rstatus) rb_jump_tag(rstatus);

    return self;
}

static VALUE
ossl_ocspbres_get_status(VALUE self)
{
    OCSP_BASICRESP *bs;
    OCSP_SINGLERESP *single;
    OCSP_CERTID *cid;
    ASN1_TIME *revtime, *thisupd, *nextupd;
    int status, reason;
    X509_EXTENSION *x509ext;
    VALUE ret, ary, ext;
    int count, ext_count, i, j;

    GetOCSPBasicRes(self, bs);
    ret = rb_ary_new();
    count = OCSP_resp_count(bs);
    for(i = 0; i < count; i++){
	single = OCSP_resp_get0(bs, i);
	if(!single) continue;

	revtime = thisupd = nextupd = NULL;
	status = OCSP_single_get0_status(single, &reason, &revtime,
					 &thisupd, &nextupd);
	if(status < 0) continue;
	if(!(cid = OCSP_CERTID_dup(single->certId)))
	    ossl_raise(eOCSPError, NULL);
	ary = rb_ary_new();
	rb_ary_push(ary, ossl_ocspcertid_new(cid));
	rb_ary_push(ary, INT2NUM(status));
	rb_ary_push(ary, INT2NUM(reason));
	rb_ary_push(ary, revtime ? asn1time_to_time(revtime) : Qnil);
	rb_ary_push(ary, thisupd ? asn1time_to_time(thisupd) : Qnil);
	rb_ary_push(ary, nextupd ? asn1time_to_time(nextupd) : Qnil);
	ext = rb_ary_new();
	ext_count = OCSP_SINGLERESP_get_ext_count(single);
	for(j = 0; j < ext_count; j++){
	    x509ext = OCSP_SINGLERESP_get_ext(single, j);
	    rb_ary_push(ext, ossl_x509ext_new(x509ext));
	}
	rb_ary_push(ary, ext);
	rb_ary_push(ret, ary);
    }

    return ret;
}

static VALUE
ossl_ocspbres_sign(int argc, VALUE *argv, VALUE self)
{
    VALUE signer_cert, signer_key, certs, flags;
    OCSP_BASICRESP *bs;
    X509 *signer;
    EVP_PKEY *key;
    STACK_OF(X509) *x509s;
    unsigned long flg;
    int ret;

    rb_scan_args(argc, argv, "22", &signer_cert, &signer_key, &certs, &flags);
    signer = GetX509CertPtr(signer_cert);
    key = GetPrivPKeyPtr(signer_key);
    flg = NIL_P(flags) ? 0 : NUM2INT(flags);
    if(NIL_P(certs)){
	x509s = sk_X509_new_null();
	flg |= OCSP_NOCERTS;
    }
    else{
	x509s = ossl_x509_ary2sk(certs);
    }
    GetOCSPBasicRes(self, bs);
    ret = OCSP_basic_sign(bs, signer, key, EVP_sha1(), x509s, flg);
    sk_X509_pop_free(x509s, X509_free);
    if(!ret) ossl_raise(eOCSPError, NULL);

    return self;
}

static VALUE
ossl_ocspbres_verify(int argc, VALUE *argv, VALUE self)
{
    VALUE certs, store, flags, result;
    OCSP_BASICRESP *bs;
    STACK_OF(X509) *x509s;
    X509_STORE *x509st;
    int flg;

    rb_scan_args(argc, argv, "21", &certs, &store, &flags);
    x509st = GetX509StorePtr(store);
    flg = NIL_P(flags) ? 0 : NUM2INT(flags);
    x509s = ossl_x509_ary2sk(certs);
    GetOCSPBasicRes(self, bs);
    result = OCSP_basic_verify(bs, x509s, x509st, flg) > 0 ? Qtrue : Qfalse;
    sk_X509_pop_free(x509s, X509_free);
    if(!result) rb_warn("%s", ERR_error_string(ERR_peek_error(), NULL));

    return result;
}

/*
 * OCSP::CertificateId
 */
static VALUE
ossl_ocspcid_alloc(VALUE klass)
{
    OCSP_CERTID *id;
    VALUE obj;

    if(!(id = OCSP_CERTID_new()))
	ossl_raise(eOCSPError, NULL);
    WrapOCSPCertId(klass, obj, id);

    return obj;
}

static VALUE
ossl_ocspcid_initialize(int argc, VALUE *argv, VALUE self)
{
    OCSP_CERTID *id, *newid;
    X509 *x509s, *x509i;
    VALUE subject, issuer, digest;
    const EVP_MD *md;

    if (rb_scan_args(argc, argv, "21", &subject, &issuer, &digest) == 0) {
	return self;
    }

    x509s = GetX509CertPtr(subject); /* NO NEED TO DUP */
    x509i = GetX509CertPtr(issuer); /* NO NEED TO DUP */

    if (!NIL_P(digest)) {
	md = GetDigestPtr(digest);
	newid = OCSP_cert_to_id(md, x509s, x509i);
    } else {
	newid = OCSP_cert_to_id(NULL, x509s, x509i);
    }
    if(!newid)
	ossl_raise(eOCSPError, NULL);
    GetOCSPCertId(self, id);
    OCSP_CERTID_free(id);
    RDATA(self)->data = newid;

    return self;
}

static VALUE
ossl_ocspcid_cmp(VALUE self, VALUE other)
{
    OCSP_CERTID *id, *id2;
    int result;

    GetOCSPCertId(self, id);
    SafeGetOCSPCertId(other, id2);
    result = OCSP_id_cmp(id, id2);

    return (result == 0) ? Qtrue : Qfalse;
}

static VALUE
ossl_ocspcid_cmp_issuer(VALUE self, VALUE other)
{
    OCSP_CERTID *id, *id2;
    int result;

    GetOCSPCertId(self, id);
    SafeGetOCSPCertId(other, id2);
    result = OCSP_id_issuer_cmp(id, id2);

    return (result == 0) ? Qtrue : Qfalse;
}

static VALUE
ossl_ocspcid_get_serial(VALUE self)
{
    OCSP_CERTID *id;

    GetOCSPCertId(self, id);

    return asn1integer_to_num(id->serialNumber);
}

void
Init_ossl_ocsp()
{
    mOCSP = rb_define_module_under(mOSSL, "OCSP");

    eOCSPError = rb_define_class_under(mOCSP, "OCSPError", eOSSLError);

    cOCSPReq = rb_define_class_under(mOCSP, "Request", rb_cObject);
    rb_define_alloc_func(cOCSPReq, ossl_ocspreq_alloc);
    rb_define_method(cOCSPReq, "initialize", ossl_ocspreq_initialize, -1);
    rb_define_method(cOCSPReq, "add_nonce", ossl_ocspreq_add_nonce, -1);
    rb_define_method(cOCSPReq, "check_nonce", ossl_ocspreq_check_nonce, 1);
    rb_define_method(cOCSPReq, "add_certid", ossl_ocspreq_add_certid, 1);
    rb_define_method(cOCSPReq, "certid", ossl_ocspreq_get_certid, 0);
    rb_define_method(cOCSPReq, "sign", ossl_ocspreq_sign, -1);
    rb_define_method(cOCSPReq, "verify", ossl_ocspreq_verify, -1);
    rb_define_method(cOCSPReq, "to_der", ossl_ocspreq_to_der, 0);

    cOCSPRes = rb_define_class_under(mOCSP, "Response", rb_cObject);
    rb_define_singleton_method(cOCSPRes, "create", ossl_ocspres_s_create, 2);
    rb_define_alloc_func(cOCSPRes, ossl_ocspres_alloc);
    rb_define_method(cOCSPRes, "initialize", ossl_ocspres_initialize, -1);
    rb_define_method(cOCSPRes, "status", ossl_ocspres_status, 0);
    rb_define_method(cOCSPRes, "status_string", ossl_ocspres_status_string, 0);
    rb_define_method(cOCSPRes, "basic", ossl_ocspres_get_basic, 0);
    rb_define_method(cOCSPRes, "to_der", ossl_ocspres_to_der, 0);

    cOCSPBasicRes = rb_define_class_under(mOCSP, "BasicResponse", rb_cObject);
    rb_define_alloc_func(cOCSPBasicRes, ossl_ocspbres_alloc);
    rb_define_method(cOCSPBasicRes, "initialize", ossl_ocspbres_initialize, -1);
    rb_define_method(cOCSPBasicRes, "copy_nonce", ossl_ocspbres_copy_nonce, 1);
    rb_define_method(cOCSPBasicRes, "add_nonce", ossl_ocspbres_add_nonce, -1);
    rb_define_method(cOCSPBasicRes, "add_status", ossl_ocspbres_add_status, 7);
    rb_define_method(cOCSPBasicRes, "status", ossl_ocspbres_get_status, 0);
    rb_define_method(cOCSPBasicRes, "sign", ossl_ocspbres_sign, -1);
    rb_define_method(cOCSPBasicRes, "verify", ossl_ocspbres_verify, -1);

    cOCSPCertId = rb_define_class_under(mOCSP, "CertificateId", rb_cObject);
    rb_define_alloc_func(cOCSPCertId, ossl_ocspcid_alloc);
    rb_define_method(cOCSPCertId, "initialize", ossl_ocspcid_initialize, -1);
    rb_define_method(cOCSPCertId, "cmp", ossl_ocspcid_cmp, 1);
    rb_define_method(cOCSPCertId, "cmp_issuer", ossl_ocspcid_cmp_issuer, 1);
    rb_define_method(cOCSPCertId, "serial", ossl_ocspcid_get_serial, 0);

#define DefOCSPConst(x) rb_define_const(mOCSP, #x, INT2NUM(OCSP_##x))

    DefOCSPConst(RESPONSE_STATUS_SUCCESSFUL);
    DefOCSPConst(RESPONSE_STATUS_MALFORMEDREQUEST);
    DefOCSPConst(RESPONSE_STATUS_INTERNALERROR);
    DefOCSPConst(RESPONSE_STATUS_TRYLATER);
    DefOCSPConst(RESPONSE_STATUS_SIGREQUIRED);
    DefOCSPConst(RESPONSE_STATUS_UNAUTHORIZED);

    DefOCSPConst(REVOKED_STATUS_NOSTATUS);
    DefOCSPConst(REVOKED_STATUS_UNSPECIFIED);
    DefOCSPConst(REVOKED_STATUS_KEYCOMPROMISE);
    DefOCSPConst(REVOKED_STATUS_CACOMPROMISE);
    DefOCSPConst(REVOKED_STATUS_AFFILIATIONCHANGED);
    DefOCSPConst(REVOKED_STATUS_SUPERSEDED);
    DefOCSPConst(REVOKED_STATUS_CESSATIONOFOPERATION);
    DefOCSPConst(REVOKED_STATUS_CERTIFICATEHOLD);
    DefOCSPConst(REVOKED_STATUS_REMOVEFROMCRL);

    DefOCSPConst(NOCERTS);
    DefOCSPConst(NOINTERN);
    DefOCSPConst(NOSIGS);
    DefOCSPConst(NOCHAIN);
    DefOCSPConst(NOVERIFY);
    DefOCSPConst(NOEXPLICIT);
    DefOCSPConst(NOCASIGN);
    DefOCSPConst(NODELEGATED);
    DefOCSPConst(NOCHECKS);
    DefOCSPConst(TRUSTOTHER);
    DefOCSPConst(RESPID_KEY);
    DefOCSPConst(NOTIME);

#define DefOCSPVConst(x) rb_define_const(mOCSP, "V_" #x, INT2NUM(V_OCSP_##x))

    DefOCSPVConst(CERTSTATUS_GOOD);
    DefOCSPVConst(CERTSTATUS_REVOKED);
    DefOCSPVConst(CERTSTATUS_UNKNOWN);
    DefOCSPVConst(RESPID_NAME);
    DefOCSPVConst(RESPID_KEY);
}

#else /* ! OSSL_OCSP_ENABLED */
void
Init_ossl_ocsp()
{
}
#endif
