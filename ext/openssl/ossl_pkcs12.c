/*
 * This program is licenced under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 * $Id$
 */
#include "ossl.h"

#define WrapPKCS12(klass, obj, p12) do { \
    if(!(p12)) ossl_raise(rb_eRuntimeError, "PKCS12 wasn't initialized."); \
    (obj) = Data_Wrap_Struct((klass), 0, PKCS12_free, (p12)); \
} while (0)

#define GetPKCS12(obj, p12) do { \
    Data_Get_Struct((obj), PKCS12, (p12)); \
    if(!(p12)) ossl_raise(rb_eRuntimeError, "PKCS12 wasn't initialized."); \
} while (0)

#define SafeGetPKCS12(obj, p12) do { \
    OSSL_Check_Kind((obj), cPKCS12); \
    GetPKCS12((obj), (p12)); \
} while (0)

#define ossl_pkcs12_set_key(o,v)      rb_iv_set((o), "@key", (v))
#define ossl_pkcs12_set_cert(o,v)     rb_iv_set((o), "@certificate", (v))
#define ossl_pkcs12_set_ca_certs(o,v) rb_iv_set((o), "@ca_certs", (v))
#define ossl_pkcs12_get_key(o)        rb_iv_get((o), "@key")
#define ossl_pkcs12_get_cert(o)       rb_iv_get((o), "@certificate")
#define ossl_pkcs12_get_ca_certs(o)   rb_iv_get((o), "@ca_certs")

/*
 * Classes
 */
VALUE cPKCS12;
VALUE ePKCS12Error;

/*
 * Private
 */
static VALUE
ossl_pkcs12_s_allocate(VALUE klass)
{
    PKCS12 *p12;
    VALUE obj;

    if(!(p12 = PKCS12_new())) ossl_raise(ePKCS12Error, NULL);
    WrapPKCS12(klass, obj, p12);

    return obj;
}

/*
 * call-seq:
 *    PKCS12.create(pass, name, key, cert [, ca, [, key_pbe [, cert_pbe [, key_iter [, mac_iter [, keytype]]]]]])
 *
 * === Parameters
 * * +pass+ - string
 * * +name+ - A string describing the key.
 * * +key+ - Any PKey.
 * * +cert+ - A X509::Certificate.
 * * * The public_key portion of the certificate must contain a valid public key.
 * * * The not_before and not_after fields must be filled in.
 * * +ca+ - An optional array of X509::Certificate's.
 * * +key_pbe+ - string
 * * +cert_pbe+ - string
 * * +key_iter+ - integer
 * * +mac_iter+ - integer
 * * +keytype+ - An integer representing an MSIE specific extension.
 *
 * Any optional arguments may be supplied as nil to preserve the OpenSSL defaults.
 *
 * See the OpenSSL documentation for PKCS12_create().
 */
static VALUE
ossl_pkcs12_s_create(int argc, VALUE *argv, VALUE self)
{
    VALUE pass, name, pkey, cert, ca, key_nid, cert_nid, key_iter, mac_iter, keytype;
    VALUE obj;
    char *passphrase, *friendlyname;
    EVP_PKEY *key;
    X509 *x509;
    STACK_OF(X509) *x509s;
    int nkey = 0, ncert = 0, kiter = 0, miter = 0, ktype = 0;
    PKCS12 *p12;

    rb_scan_args(argc, argv, "46", &pass, &name, &pkey, &cert, &ca, &key_nid, &cert_nid, &key_iter, &mac_iter, &keytype);
    passphrase = NIL_P(pass) ? NULL : StringValuePtr(pass);
    friendlyname = NIL_P(name) ? NULL : StringValuePtr(name);
    key = GetPKeyPtr(pkey);
    x509 = GetX509CertPtr(cert);
    x509s = NIL_P(ca) ? NULL : ossl_x509_ary2sk(ca);
/* TODO: make a VALUE to nid function */
    if (!NIL_P(key_nid)) {
        if ((nkey = OBJ_txt2nid(StringValuePtr(key_nid))) == NID_undef)
            ossl_raise(rb_eArgError, "Unknown PBE algorithm %s", StringValuePtr(key_nid));
    }
    if (!NIL_P(cert_nid)) {
        if ((ncert = OBJ_txt2nid(StringValuePtr(cert_nid))) == NID_undef)
            ossl_raise(rb_eArgError, "Unknown PBE algorithm %s", StringValuePtr(cert_nid));
    }
    if (!NIL_P(key_iter))
        kiter = NUM2INT(key_iter);
    if (!NIL_P(mac_iter))
        miter = NUM2INT(mac_iter);
    if (!NIL_P(keytype))
        ktype = NUM2INT(keytype);

    p12 = PKCS12_create(passphrase, friendlyname, key, x509, x509s,
                        nkey, ncert, kiter, miter, ktype);
    sk_X509_pop_free(x509s, X509_free);
    if(!p12) ossl_raise(ePKCS12Error, NULL);
    WrapPKCS12(cPKCS12, obj, p12);

    ossl_pkcs12_set_key(obj, pkey);
    ossl_pkcs12_set_cert(obj, cert);
    ossl_pkcs12_set_ca_certs(obj, ca);

    return obj;
}

/*
 * call-seq:
 *    PKCS12.new -> pkcs12
 *    PKCS12.new(str) -> pkcs12
 *    PKCS12.new(str, pass) -> pkcs12
 *
 * === Parameters
 * * +str+ - Must be a DER encoded PKCS12 string.
 * * +pass+ - string
 */
static VALUE
ossl_pkcs12_initialize(int argc, VALUE *argv, VALUE self)
{
    BIO *in;
    VALUE arg, pass, pkey, cert, ca;
    char *passphrase;
    EVP_PKEY *key;
    X509 *x509;
    STACK_OF(X509) *x509s = NULL;
    int st = 0;
    PKCS12 *pkcs = DATA_PTR(self);

    if(rb_scan_args(argc, argv, "02", &arg, &pass) == 0) return self;
    passphrase = NIL_P(pass) ? NULL : StringValuePtr(pass);
    in = ossl_obj2bio(arg);
    d2i_PKCS12_bio(in, &pkcs);
    DATA_PTR(self) = pkcs;
    BIO_free(in);

    pkey = cert = ca = Qnil;
    if(!PKCS12_parse(pkcs, passphrase, &key, &x509, &x509s))
	ossl_raise(ePKCS12Error, "PKCS12_parse");
    pkey = rb_protect((VALUE(*)_((VALUE)))ossl_pkey_new, (VALUE)key,
		      &st); /* NO DUP */
    if(st) goto err;
    cert = rb_protect((VALUE(*)_((VALUE)))ossl_x509_new, (VALUE)x509, &st);
    if(st) goto err;
    if(x509s){
	ca =
	    rb_protect((VALUE(*)_((VALUE)))ossl_x509_sk2ary, (VALUE)x509s, &st);
	if(st) goto err;
    }

  err:
    X509_free(x509);
    sk_X509_pop_free(x509s, X509_free);
    ossl_pkcs12_set_key(self, pkey);
    ossl_pkcs12_set_cert(self, cert);
    ossl_pkcs12_set_ca_certs(self, ca);
    if(st) rb_jump_tag(st);

    return self;
}

static VALUE
ossl_pkcs12_to_der(VALUE self)
{
    PKCS12 *p12;
    VALUE str;
    long len;
    unsigned char *p;

    GetPKCS12(self, p12);
    if((len = i2d_PKCS12(p12, NULL)) <= 0)
	ossl_raise(ePKCS12Error, NULL);
    str = rb_str_new(0, len);
    p = (unsigned char *)RSTRING_PTR(str);
    if(i2d_PKCS12(p12, &p) <= 0)
	ossl_raise(ePKCS12Error, NULL);
    ossl_str_adjust(str, p);

    return str;
}

void
Init_ossl_pkcs12()
{
    /*
     * Defines a file format commonly used to store private keys with
     * accompanying public key certificates, protected with a password-based
     * symmetric key.
     */
    cPKCS12 = rb_define_class_under(mOSSL, "PKCS12", rb_cObject);
    ePKCS12Error = rb_define_class_under(cPKCS12, "PKCS12Error", eOSSLError);
    rb_define_singleton_method(cPKCS12, "create", ossl_pkcs12_s_create, -1);

    rb_define_alloc_func(cPKCS12, ossl_pkcs12_s_allocate);
    rb_attr(cPKCS12, rb_intern("key"), 1, 0, Qfalse);
    rb_attr(cPKCS12, rb_intern("certificate"), 1, 0, Qfalse);
    rb_attr(cPKCS12, rb_intern("ca_certs"), 1, 0, Qfalse);
    rb_define_method(cPKCS12, "initialize", ossl_pkcs12_initialize, -1);
    rb_define_method(cPKCS12, "to_der", ossl_pkcs12_to_der, 0);
}
