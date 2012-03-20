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

#define WrapX509Ext(klass, obj, ext) do { \
    if (!(ext)) { \
	ossl_raise(rb_eRuntimeError, "EXT wasn't initialized!"); \
    } \
    (obj) = Data_Wrap_Struct((klass), 0, X509_EXTENSION_free, (ext)); \
} while (0)
#define GetX509Ext(obj, ext) do { \
    Data_Get_Struct((obj), X509_EXTENSION, (ext)); \
    if (!(ext)) { \
	ossl_raise(rb_eRuntimeError, "EXT wasn't initialized!"); \
    } \
} while (0)
#define SafeGetX509Ext(obj, ext) do { \
    OSSL_Check_Kind((obj), cX509Ext); \
    GetX509Ext((obj), (ext)); \
} while (0)
#define MakeX509ExtFactory(klass, obj, ctx) do { \
    if (!((ctx) = OPENSSL_malloc(sizeof(X509V3_CTX)))) \
        ossl_raise(rb_eRuntimeError, "CTX wasn't allocated!"); \
    X509V3_set_ctx((ctx), NULL, NULL, NULL, NULL, 0); \
    (obj) = Data_Wrap_Struct((klass), 0, ossl_x509extfactory_free, (ctx)); \
} while (0)
#define GetX509ExtFactory(obj, ctx) do { \
    Data_Get_Struct((obj), X509V3_CTX, (ctx)); \
    if (!(ctx)) { \
	ossl_raise(rb_eRuntimeError, "CTX wasn't initialized!"); \
    } \
} while (0)

/*
 * Classes
 */
VALUE cX509Ext;
VALUE cX509ExtFactory;
VALUE eX509ExtError;

/*
 * Public
 */
VALUE
ossl_x509ext_new(X509_EXTENSION *ext)
{
    X509_EXTENSION *new;
    VALUE obj;

    if (!ext) {
	new = X509_EXTENSION_new();
    } else {
	new = X509_EXTENSION_dup(ext);
    }
    if (!new) {
	ossl_raise(eX509ExtError, NULL);
    }
    WrapX509Ext(cX509Ext, obj, new);

    return obj;
}

X509_EXTENSION *
GetX509ExtPtr(VALUE obj)
{
    X509_EXTENSION *ext;

    SafeGetX509Ext(obj, ext);

    return ext;
}

X509_EXTENSION *
DupX509ExtPtr(VALUE obj)
{
    X509_EXTENSION *ext, *new;

    SafeGetX509Ext(obj, ext);
    if (!(new = X509_EXTENSION_dup(ext))) {
	ossl_raise(eX509ExtError, NULL);
    }

    return new;
}

/*
 * Private
 */
/*
 * Ext factory
 */
static void
ossl_x509extfactory_free(X509V3_CTX *ctx)
{
    OPENSSL_free(ctx);
}

static VALUE
ossl_x509extfactory_alloc(VALUE klass)
{
    X509V3_CTX *ctx;
    VALUE obj;

    MakeX509ExtFactory(klass, obj, ctx);
    rb_iv_set(obj, "@config", Qnil);

    return obj;
}

static VALUE
ossl_x509extfactory_set_issuer_cert(VALUE self, VALUE cert)
{
    X509V3_CTX *ctx;

    GetX509ExtFactory(self, ctx);
    rb_iv_set(self, "@issuer_certificate", cert);
    ctx->issuer_cert = GetX509CertPtr(cert); /* NO DUP NEEDED */

    return cert;
}

static VALUE
ossl_x509extfactory_set_subject_cert(VALUE self, VALUE cert)
{
    X509V3_CTX *ctx;

    GetX509ExtFactory(self, ctx);
    rb_iv_set(self, "@subject_certificate", cert);
    ctx->subject_cert = GetX509CertPtr(cert); /* NO DUP NEEDED */

    return cert;
}

static VALUE
ossl_x509extfactory_set_subject_req(VALUE self, VALUE req)
{
    X509V3_CTX *ctx;

    GetX509ExtFactory(self, ctx);
    rb_iv_set(self, "@subject_request", req);
    ctx->subject_req = GetX509ReqPtr(req); /* NO DUP NEEDED */

    return req;
}

static VALUE
ossl_x509extfactory_set_crl(VALUE self, VALUE crl)
{
    X509V3_CTX *ctx;

    GetX509ExtFactory(self, ctx);
    rb_iv_set(self, "@crl", crl);
    ctx->crl = GetX509CRLPtr(crl); /* NO DUP NEEDED */

    return crl;
}

#ifdef HAVE_X509V3_SET_NCONF
static VALUE
ossl_x509extfactory_set_config(VALUE self, VALUE config)
{
    X509V3_CTX *ctx;
    CONF *conf;

    GetX509ExtFactory(self, ctx);
    rb_iv_set(self, "@config", config);
    conf = GetConfigPtr(config);  /* NO DUP NEEDED */
    X509V3_set_nconf(ctx, conf);

    return config;
}
#else
#define ossl_x509extfactory_set_config rb_f_notimplement
#endif

static VALUE
ossl_x509extfactory_initialize(int argc, VALUE *argv, VALUE self)
{
    /*X509V3_CTX *ctx;*/
    VALUE issuer_cert, subject_cert, subject_req, crl;

    /*GetX509ExtFactory(self, ctx);*/

    rb_scan_args(argc, argv, "04",
		 &issuer_cert, &subject_cert, &subject_req, &crl);
    if (!NIL_P(issuer_cert))
	ossl_x509extfactory_set_issuer_cert(self, issuer_cert);
    if (!NIL_P(subject_cert))
	ossl_x509extfactory_set_subject_cert(self, subject_cert);
    if (!NIL_P(subject_req))
	ossl_x509extfactory_set_subject_req(self, subject_req);
    if (!NIL_P(crl))
	ossl_x509extfactory_set_crl(self, crl);

    return self;
}

/*
 * Array to X509_EXTENSION
 * Structure:
 * ["ln", "value", bool_critical] or
 * ["sn", "value", bool_critical] or
 * ["ln", "critical,value"] or the same for sn
 * ["ln", "value"] => not critical
 */
static VALUE
ossl_x509extfactory_create_ext(int argc, VALUE *argv, VALUE self)
{
    X509V3_CTX *ctx;
    X509_EXTENSION *ext;
    VALUE oid, value, critical, valstr, obj;
    int nid;
#ifdef HAVE_X509V3_EXT_NCONF_NID
    VALUE rconf;
    CONF *conf;
#else
    static LHASH *empty_lhash;
#endif

    rb_scan_args(argc, argv, "21", &oid, &value, &critical);
    StringValue(oid);
    StringValue(value);
    if(NIL_P(critical)) critical = Qfalse;

    nid = OBJ_ln2nid(RSTRING_PTR(oid));
    if(!nid) nid = OBJ_sn2nid(RSTRING_PTR(oid));
    if(!nid) ossl_raise(eX509ExtError, "unknown OID `%s'", RSTRING_PTR(oid));
    valstr = rb_str_new2(RTEST(critical) ? "critical," : "");
    rb_str_append(valstr, value);
    GetX509ExtFactory(self, ctx);
#ifdef HAVE_X509V3_EXT_NCONF_NID
    rconf = rb_iv_get(self, "@config");
    conf = NIL_P(rconf) ? NULL : GetConfigPtr(rconf);
    ext = X509V3_EXT_nconf_nid(conf, ctx, nid, RSTRING_PTR(valstr));
#else
    if (!empty_lhash) empty_lhash = lh_new(NULL, NULL);
    ext = X509V3_EXT_conf_nid(empty_lhash, ctx, nid, RSTRING_PTR(valstr));
#endif
    if (!ext){
	ossl_raise(eX509ExtError, "%s = %s",
		   RSTRING_PTR(oid), RSTRING_PTR(value));
    }
    WrapX509Ext(cX509Ext, obj, ext);

    return obj;
}

/*
 * Ext
 */
static VALUE
ossl_x509ext_alloc(VALUE klass)
{
    X509_EXTENSION *ext;
    VALUE obj;

    if(!(ext = X509_EXTENSION_new())){
	ossl_raise(eX509ExtError, NULL);
    }
    WrapX509Ext(klass, obj, ext);

    return obj;
}

/*
 * call-seq:
 *    OpenSSL::X509::Extension.new asn1
 *    OpenSSL::X509::Extension.new name, value
 *    OpenSSL::X509::Extension.new name, value, critical
 *
 * Creates an X509 extension.
 *
 * The extension may be created from +asn1+ data or from an extension +name+
 * and +value+.  The +name+ may be either an OID or an extension name.  If
 * +critical+ is true the extension is marked critical.
 */
static VALUE
ossl_x509ext_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE oid, value, critical;
    const unsigned char *p;
    X509_EXTENSION *ext, *x;

    GetX509Ext(self, ext);
    if(rb_scan_args(argc, argv, "12", &oid, &value, &critical) == 1){
	oid = ossl_to_der_if_possible(oid);
	StringValue(oid);
	p = (unsigned char *)RSTRING_PTR(oid);
	x = d2i_X509_EXTENSION(&ext, &p, RSTRING_LEN(oid));
	DATA_PTR(self) = ext;
	if(!x)
	    ossl_raise(eX509ExtError, NULL);
	return self;
    }
    rb_funcall(self, rb_intern("oid="), 1, oid);
    rb_funcall(self, rb_intern("value="), 1, value);
    if(argc > 2) rb_funcall(self, rb_intern("critical="), 1, critical);

    return self;
}

static VALUE
ossl_x509ext_set_oid(VALUE self, VALUE oid)
{
    X509_EXTENSION *ext;
    ASN1_OBJECT *obj;
    char *s;

    s = StringValuePtr(oid);
    obj = OBJ_txt2obj(s, 0);
    if(!obj) obj = OBJ_txt2obj(s, 1);
    if(!obj) ossl_raise(eX509ExtError, NULL);
    GetX509Ext(self, ext);
    X509_EXTENSION_set_object(ext, obj);

    return oid;
}

static VALUE
ossl_x509ext_set_value(VALUE self, VALUE data)
{
    X509_EXTENSION *ext;
    ASN1_OCTET_STRING *asn1s;
    char *s;

    data = ossl_to_der_if_possible(data);
    StringValue(data);
    if(!(s = OPENSSL_malloc(RSTRING_LEN(data))))
	ossl_raise(eX509ExtError, "malloc error");
    memcpy(s, RSTRING_PTR(data), RSTRING_LEN(data));
    if(!(asn1s = ASN1_OCTET_STRING_new())){
	OPENSSL_free(s);
	ossl_raise(eX509ExtError, NULL);
    }
    if(!M_ASN1_OCTET_STRING_set(asn1s, s, RSTRING_LENINT(data))){
	OPENSSL_free(s);
	ASN1_OCTET_STRING_free(asn1s);
	ossl_raise(eX509ExtError, NULL);
    }
    OPENSSL_free(s);
    GetX509Ext(self, ext);
    X509_EXTENSION_set_data(ext, asn1s);

    return data;
}

static VALUE
ossl_x509ext_set_critical(VALUE self, VALUE flag)
{
    X509_EXTENSION *ext;

    GetX509Ext(self, ext);
    X509_EXTENSION_set_critical(ext, RTEST(flag) ? 1 : 0);

    return flag;
}

static VALUE
ossl_x509ext_get_oid(VALUE obj)
{
    X509_EXTENSION *ext;
    ASN1_OBJECT *extobj;
    BIO *out;
    VALUE ret;
    int nid;

    GetX509Ext(obj, ext);
    extobj = X509_EXTENSION_get_object(ext);
    if ((nid = OBJ_obj2nid(extobj)) != NID_undef)
	ret = rb_str_new2(OBJ_nid2sn(nid));
    else{
	if (!(out = BIO_new(BIO_s_mem())))
	    ossl_raise(eX509ExtError, NULL);
	i2a_ASN1_OBJECT(out, extobj);
	ret = ossl_membio2str(out);
    }

    return ret;
}

static VALUE
ossl_x509ext_get_value(VALUE obj)
{
    X509_EXTENSION *ext;
    BIO *out;
    VALUE ret;

    GetX509Ext(obj, ext);
    if (!(out = BIO_new(BIO_s_mem())))
	ossl_raise(eX509ExtError, NULL);
    if (!X509V3_EXT_print(out, ext, 0, 0))
	M_ASN1_OCTET_STRING_print(out, ext->value);
    ret = ossl_membio2str(out);

    return ret;
}

static VALUE
ossl_x509ext_get_critical(VALUE obj)
{
    X509_EXTENSION *ext;

    GetX509Ext(obj, ext);
    return X509_EXTENSION_get_critical(ext) ? Qtrue : Qfalse;
}

static VALUE
ossl_x509ext_to_der(VALUE obj)
{
    X509_EXTENSION *ext;
    unsigned char *p;
    long len;
    VALUE str;

    GetX509Ext(obj, ext);
    if((len = i2d_X509_EXTENSION(ext, NULL)) <= 0)
	ossl_raise(eX509ExtError, NULL);
    str = rb_str_new(0, len);
    p = (unsigned char *)RSTRING_PTR(str);
    if(i2d_X509_EXTENSION(ext, &p) < 0)
	ossl_raise(eX509ExtError, NULL);
    ossl_str_adjust(str, p);

    return str;
}

/*
 * INIT
 */
void
Init_ossl_x509ext()
{
    eX509ExtError = rb_define_class_under(mX509, "ExtensionError", eOSSLError);

    cX509ExtFactory = rb_define_class_under(mX509, "ExtensionFactory", rb_cObject);

    rb_define_alloc_func(cX509ExtFactory, ossl_x509extfactory_alloc);
    rb_define_method(cX509ExtFactory, "initialize", ossl_x509extfactory_initialize, -1);

    rb_attr(cX509ExtFactory, rb_intern("issuer_certificate"), 1, 0, Qfalse);
    rb_attr(cX509ExtFactory, rb_intern("subject_certificate"), 1, 0, Qfalse);
    rb_attr(cX509ExtFactory, rb_intern("subject_request"), 1, 0, Qfalse);
    rb_attr(cX509ExtFactory, rb_intern("crl"), 1, 0, Qfalse);
    rb_attr(cX509ExtFactory, rb_intern("config"), 1, 0, Qfalse);

    rb_define_method(cX509ExtFactory, "issuer_certificate=", ossl_x509extfactory_set_issuer_cert, 1);
    rb_define_method(cX509ExtFactory, "subject_certificate=", ossl_x509extfactory_set_subject_cert, 1);
    rb_define_method(cX509ExtFactory, "subject_request=", ossl_x509extfactory_set_subject_req, 1);
    rb_define_method(cX509ExtFactory, "crl=", ossl_x509extfactory_set_crl, 1);
    rb_define_method(cX509ExtFactory, "config=", ossl_x509extfactory_set_config, 1);
    rb_define_method(cX509ExtFactory, "create_ext", ossl_x509extfactory_create_ext, -1);

    cX509Ext = rb_define_class_under(mX509, "Extension", rb_cObject);
    rb_define_alloc_func(cX509Ext, ossl_x509ext_alloc);
    rb_define_method(cX509Ext, "initialize", ossl_x509ext_initialize, -1);
    rb_define_method(cX509Ext, "oid=", ossl_x509ext_set_oid, 1);
    rb_define_method(cX509Ext, "value=", ossl_x509ext_set_value, 1);
    rb_define_method(cX509Ext, "critical=", ossl_x509ext_set_critical, 1);
    rb_define_method(cX509Ext, "oid", ossl_x509ext_get_oid, 0);
    rb_define_method(cX509Ext, "value", ossl_x509ext_get_value, 0);
    rb_define_method(cX509Ext, "critical?", ossl_x509ext_get_critical, 0);
    rb_define_method(cX509Ext, "to_der", ossl_x509ext_to_der, 0);
}
