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

#define WrapX509(klass, obj, x509) do { \
    if (!(x509)) { \
	ossl_raise(rb_eRuntimeError, "CERT wasn't initialized!"); \
    } \
    (obj) = Data_Wrap_Struct((klass), 0, X509_free, (x509)); \
} while (0)
#define GetX509(obj, x509) do { \
    Data_Get_Struct((obj), X509, (x509)); \
    if (!(x509)) { \
	ossl_raise(rb_eRuntimeError, "CERT wasn't initialized!"); \
    } \
} while (0)
#define SafeGetX509(obj, x509) do { \
    OSSL_Check_Kind((obj), cX509Cert); \
    GetX509((obj), (x509)); \
} while (0)

/*
 * Classes
 */
VALUE cX509Cert;
VALUE eX509CertError;

/*
 * Public
 */
VALUE
ossl_x509_new(X509 *x509)
{
    X509 *new;
    VALUE obj;

    if (!x509) {
	new = X509_new();
    } else {
	new = X509_dup(x509);
    }
    if (!new) {
	ossl_raise(eX509CertError, NULL);
    }
    WrapX509(cX509Cert, obj, new);

    return obj;
}

VALUE
ossl_x509_new_from_file(VALUE filename)
{
    X509 *x509;
    FILE *fp;
    VALUE obj;

    SafeStringValue(filename);
    if (!(fp = fopen(RSTRING_PTR(filename), "r"))) {
	ossl_raise(eX509CertError, "%s", strerror(errno));
    }
    rb_fd_fix_cloexec(fileno(fp));
    x509 = PEM_read_X509(fp, NULL, NULL, NULL);
    /*
     * prepare for DER...
#if !defined(OPENSSL_NO_FP_API)
    if (!x509) {
    	(void)ERR_get_error();
	rewind(fp);

	x509 = d2i_X509_fp(fp, NULL);
    }
#endif
    */
    fclose(fp);
    if (!x509) {
	ossl_raise(eX509CertError, NULL);
    }
    WrapX509(cX509Cert, obj, x509);

    return obj;
}

X509 *
GetX509CertPtr(VALUE obj)
{
    X509 *x509;

    SafeGetX509(obj, x509);

    return x509;
}

X509 *
DupX509CertPtr(VALUE obj)
{
    X509 *x509;

    SafeGetX509(obj, x509);

    CRYPTO_add(&x509->references, 1, CRYPTO_LOCK_X509);

    return x509;
}

/*
 * Private
 */
static VALUE
ossl_x509_alloc(VALUE klass)
{
    X509 *x509;
    VALUE obj;

    x509 = X509_new();
    if (!x509) ossl_raise(eX509CertError, NULL);

    WrapX509(klass, obj, x509);

    return obj;
}

/*
 * call-seq:
 *    Certificate.new => cert
 *    Certificate.new(string) => cert
 */
static VALUE
ossl_x509_initialize(int argc, VALUE *argv, VALUE self)
{
    BIO *in;
    X509 *x509, *x = DATA_PTR(self);
    VALUE arg;

    if (rb_scan_args(argc, argv, "01", &arg) == 0) {
	/* create just empty X509Cert */
	return self;
    }
    arg = ossl_to_der_if_possible(arg);
    in = ossl_obj2bio(arg);
    x509 = PEM_read_bio_X509(in, &x, NULL, NULL);
    DATA_PTR(self) = x;
    if (!x509) {
	OSSL_BIO_reset(in);
	x509 = d2i_X509_bio(in, &x);
	DATA_PTR(self) = x;
    }
    BIO_free(in);
    if (!x509) ossl_raise(eX509CertError, NULL);

    return self;
}

static VALUE
ossl_x509_copy(VALUE self, VALUE other)
{
    X509 *a, *b, *x509;

    rb_check_frozen(self);
    if (self == other) return self;

    GetX509(self, a);
    SafeGetX509(other, b);

    x509 = X509_dup(b);
    if (!x509) ossl_raise(eX509CertError, NULL);

    DATA_PTR(self) = x509;
    X509_free(a);

    return self;
}

/*
 * call-seq:
 *    cert.to_der => string
 */
static VALUE
ossl_x509_to_der(VALUE self)
{
    X509 *x509;
    VALUE str;
    long len;
    unsigned char *p;

    GetX509(self, x509);
    if ((len = i2d_X509(x509, NULL)) <= 0)
	ossl_raise(eX509CertError, NULL);
    str = rb_str_new(0, len);
    p = (unsigned char *)RSTRING_PTR(str);
    if (i2d_X509(x509, &p) <= 0)
	ossl_raise(eX509CertError, NULL);
    ossl_str_adjust(str, p);

    return str;
}

/*
 * call-seq:
 *    cert.to_pem => string
 */
static VALUE
ossl_x509_to_pem(VALUE self)
{
    X509 *x509;
    BIO *out;
    VALUE str;

    GetX509(self, x509);
    out = BIO_new(BIO_s_mem());
    if (!out) ossl_raise(eX509CertError, NULL);

    if (!PEM_write_bio_X509(out, x509)) {
	BIO_free(out);
	ossl_raise(eX509CertError, NULL);
    }
    str = ossl_membio2str(out);

    return str;
}

/*
 * call-seq:
 *    cert.to_text => string
 */
static VALUE
ossl_x509_to_text(VALUE self)
{
    X509 *x509;
    BIO *out;
    VALUE str;

    GetX509(self, x509);

    out = BIO_new(BIO_s_mem());
    if (!out) ossl_raise(eX509CertError, NULL);

    if (!X509_print(out, x509)) {
	BIO_free(out);
	ossl_raise(eX509CertError, NULL);
    }
    str = ossl_membio2str(out);

    return str;
}

#if 0
/*
 * Makes from X509 X509_REQuest
 */
static VALUE
ossl_x509_to_req(VALUE self)
{
    X509 *x509;
    X509_REQ *req;
    VALUE obj;

    GetX509(self, x509);
    if (!(req = X509_to_X509_REQ(x509, NULL, EVP_md5()))) {
	ossl_raise(eX509CertError, NULL);
    }
    obj = ossl_x509req_new(req);
    X509_REQ_free(req);

    return obj;
}
#endif

/*
 * call-seq:
 *    cert.version => integer
 */
static VALUE
ossl_x509_get_version(VALUE self)
{
    X509 *x509;

    GetX509(self, x509);

    return LONG2NUM(X509_get_version(x509));
}

/*
 * call-seq:
 *    cert.version = integer => integer
 */
static VALUE
ossl_x509_set_version(VALUE self, VALUE version)
{
    X509 *x509;
    long ver;

    if ((ver = NUM2LONG(version)) < 0) {
	ossl_raise(eX509CertError, "version must be >= 0!");
    }
    GetX509(self, x509);
    if (!X509_set_version(x509, ver)) {
	ossl_raise(eX509CertError, NULL);
    }

    return version;
}

/*
 * call-seq:
 *    cert.serial => integer
 */
static VALUE
ossl_x509_get_serial(VALUE self)
{
    X509 *x509;

    GetX509(self, x509);

    return asn1integer_to_num(X509_get_serialNumber(x509));
}

/*
 * call-seq:
 *    cert.serial = integer => integer
 */
static VALUE
ossl_x509_set_serial(VALUE self, VALUE num)
{
    X509 *x509;

    GetX509(self, x509);

    x509->cert_info->serialNumber =
	num_to_asn1integer(num, X509_get_serialNumber(x509));

    return num;
}

/*
 * call-seq:
 *    cert.signature_algorithm => string
 */
static VALUE
ossl_x509_get_signature_algorithm(VALUE self)
{
    X509 *x509;
    BIO *out;
    VALUE str;

    GetX509(self, x509);
    out = BIO_new(BIO_s_mem());
    if (!out) ossl_raise(eX509CertError, NULL);

    if (!i2a_ASN1_OBJECT(out, x509->cert_info->signature->algorithm)) {
	BIO_free(out);
	ossl_raise(eX509CertError, NULL);
    }
    str = ossl_membio2str(out);

    return str;
}

/*
 * call-seq:
 *    cert.subject => name
 */
static VALUE
ossl_x509_get_subject(VALUE self)
{
    X509 *x509;
    X509_NAME *name;

    GetX509(self, x509);
    if (!(name = X509_get_subject_name(x509))) { /* NO DUP - don't free! */
	ossl_raise(eX509CertError, NULL);
    }

    return ossl_x509name_new(name);
}

/*
 * call-seq:
 *    cert.subject = name => name
 */
static VALUE
ossl_x509_set_subject(VALUE self, VALUE subject)
{
    X509 *x509;

    GetX509(self, x509);
    if (!X509_set_subject_name(x509, GetX509NamePtr(subject))) { /* DUPs name */
	ossl_raise(eX509CertError, NULL);
    }

    return subject;
}

/*
 * call-seq:
 *    cert.issuer => name
 */
static VALUE
ossl_x509_get_issuer(VALUE self)
{
    X509 *x509;
    X509_NAME *name;

    GetX509(self, x509);
    if(!(name = X509_get_issuer_name(x509))) { /* NO DUP - don't free! */
	ossl_raise(eX509CertError, NULL);
    }

    return ossl_x509name_new(name);
}

/*
 * call-seq:
 *    cert.issuer = name => name
 */
static VALUE
ossl_x509_set_issuer(VALUE self, VALUE issuer)
{
    X509 *x509;

    GetX509(self, x509);
    if (!X509_set_issuer_name(x509, GetX509NamePtr(issuer))) { /* DUPs name */
	ossl_raise(eX509CertError, NULL);
    }

    return issuer;
}

/*
 * call-seq:
 *    cert.not_before => time
 */
static VALUE
ossl_x509_get_not_before(VALUE self)
{
    X509 *x509;
    ASN1_UTCTIME *asn1time;

    GetX509(self, x509);
    if (!(asn1time = X509_get_notBefore(x509))) { /* NO DUP - don't free! */
	ossl_raise(eX509CertError, NULL);
    }

    return asn1time_to_time(asn1time);
}

/*
 * call-seq:
 *    cert.not_before = time => time
 */
static VALUE
ossl_x509_set_not_before(VALUE self, VALUE time)
{
    X509 *x509;
    time_t sec;

    sec = time_to_time_t(time);
    GetX509(self, x509);
    if (!X509_time_adj(X509_get_notBefore(x509), 0, &sec)) {
	ossl_raise(eX509CertError, NULL);
    }

    return time;
}

/*
 * call-seq:
 *    cert.not_after => time
 */
static VALUE
ossl_x509_get_not_after(VALUE self)
{
    X509 *x509;
    ASN1_TIME *asn1time;

    GetX509(self, x509);
    if (!(asn1time = X509_get_notAfter(x509))) { /* NO DUP - don't free! */
	ossl_raise(eX509CertError, NULL);
    }

    return asn1time_to_time(asn1time);
}

/*
 * call-seq:
 *    cert.not_before = time => time
 */
static VALUE
ossl_x509_set_not_after(VALUE self, VALUE time)
{
    X509 *x509;
    time_t sec;

    sec = time_to_time_t(time);
    GetX509(self, x509);
    if (!X509_time_adj(X509_get_notAfter(x509), 0, &sec)) {
	ossl_raise(eX509CertError, NULL);
    }

    return time;
}

/*
 * call-seq:
 *    cert.public_key => key
 */
static VALUE
ossl_x509_get_public_key(VALUE self)
{
    X509 *x509;
    EVP_PKEY *pkey;

    GetX509(self, x509);
    if (!(pkey = X509_get_pubkey(x509))) { /* adds an reference */
	ossl_raise(eX509CertError, NULL);
    }

    return ossl_pkey_new(pkey); /* NO DUP - OK */
}

/*
 * call-seq:
 *    cert.public_key = key => key
 */
static VALUE
ossl_x509_set_public_key(VALUE self, VALUE key)
{
    X509 *x509;

    GetX509(self, x509);
    if (!X509_set_pubkey(x509, GetPKeyPtr(key))) { /* DUPs pkey */
	ossl_raise(eX509CertError, NULL);
    }

    return key;
}

/*
 * call-seq:
 *    cert.sign(key, digest) => self
 */
static VALUE
ossl_x509_sign(VALUE self, VALUE key, VALUE digest)
{
    X509 *x509;
    EVP_PKEY *pkey;
    const EVP_MD *md;

    pkey = GetPrivPKeyPtr(key); /* NO NEED TO DUP */
    md = GetDigestPtr(digest);
    GetX509(self, x509);
    if (!X509_sign(x509, pkey, md)) {
	ossl_raise(eX509CertError, NULL);
    }

    return self;
}

/*
 * call-seq:
 *    cert.verify(key) => true | false
 *
 * Checks that cert signature is made with PRIVversion of this PUBLIC 'key'
 */
static VALUE
ossl_x509_verify(VALUE self, VALUE key)
{
    X509 *x509;
    EVP_PKEY *pkey;
    int i;

    pkey = GetPKeyPtr(key); /* NO NEED TO DUP */
    GetX509(self, x509);
    if ((i = X509_verify(x509, pkey)) < 0) {
	ossl_raise(eX509CertError, NULL);
    }
    if (i > 0) {
	return Qtrue;
    }

    return Qfalse;
}

/*
 * call-seq:
 *    cert.check_private_key(key)
 *
 * Checks if 'key' is PRIV key for this cert
 */
static VALUE
ossl_x509_check_private_key(VALUE self, VALUE key)
{
    X509 *x509;
    EVP_PKEY *pkey;

    /* not needed private key, but should be */
    pkey = GetPrivPKeyPtr(key); /* NO NEED TO DUP */
    GetX509(self, x509);
    if (!X509_check_private_key(x509, pkey)) {
	OSSL_Warning("Check private key:%s", OSSL_ErrMsg());
	return Qfalse;
    }

    return Qtrue;
}

/*
 * call-seq:
 *    cert.extensions => [extension...]
 */
static VALUE
ossl_x509_get_extensions(VALUE self)
{
    X509 *x509;
    int count, i;
    X509_EXTENSION *ext;
    VALUE ary;

    GetX509(self, x509);
    count = X509_get_ext_count(x509);
    if (count < 0) {
	return rb_ary_new();
    }
    ary = rb_ary_new2(count);
    for (i=0; i<count; i++) {
	ext = X509_get_ext(x509, i); /* NO DUP - don't free! */
	rb_ary_push(ary, ossl_x509ext_new(ext));
    }

    return ary;
}

/*
 * call-seq:
 *    cert.extensions = [ext...] => [ext...]
 */
static VALUE
ossl_x509_set_extensions(VALUE self, VALUE ary)
{
    X509 *x509;
    X509_EXTENSION *ext;
    int i;

    Check_Type(ary, T_ARRAY);
    /* All ary's members should be X509Extension */
    for (i=0; i<RARRAY_LEN(ary); i++) {
	OSSL_Check_Kind(RARRAY_PTR(ary)[i], cX509Ext);
    }
    GetX509(self, x509);
    sk_X509_EXTENSION_pop_free(x509->cert_info->extensions, X509_EXTENSION_free);
    x509->cert_info->extensions = NULL;
    for (i=0; i<RARRAY_LEN(ary); i++) {
	ext = DupX509ExtPtr(RARRAY_PTR(ary)[i]);

	if (!X509_add_ext(x509, ext, -1)) { /* DUPs ext - FREE it */
	    X509_EXTENSION_free(ext);
	    ossl_raise(eX509CertError, NULL);
	}
	X509_EXTENSION_free(ext);
    }

    return ary;
}

/*
 * call-seq:
 *    cert.add_extension(extension) => extension
 */
static VALUE
ossl_x509_add_extension(VALUE self, VALUE extension)
{
    X509 *x509;
    X509_EXTENSION *ext;

    GetX509(self, x509);
    ext = DupX509ExtPtr(extension);
    if (!X509_add_ext(x509, ext, -1)) { /* DUPs ext - FREE it */
	X509_EXTENSION_free(ext);
	ossl_raise(eX509CertError, NULL);
    }
    X509_EXTENSION_free(ext);

    return extension;
}

static VALUE
ossl_x509_inspect(VALUE self)
{
    VALUE str;
    const char *cname = rb_class2name(rb_obj_class(self));

    str = rb_str_new2("#<");
    rb_str_cat2(str, cname);
    rb_str_cat2(str, " ");

    rb_str_cat2(str, "subject=");
    rb_str_append(str, rb_inspect(ossl_x509_get_subject(self)));
    rb_str_cat2(str, ", ");

    rb_str_cat2(str, "issuer=");
    rb_str_append(str, rb_inspect(ossl_x509_get_issuer(self)));
    rb_str_cat2(str, ", ");

    rb_str_cat2(str, "serial=");
    rb_str_append(str, rb_inspect(ossl_x509_get_serial(self)));
    rb_str_cat2(str, ", ");

    rb_str_cat2(str, "not_before=");
    rb_str_append(str, rb_inspect(ossl_x509_get_not_before(self)));
    rb_str_cat2(str, ", ");

    rb_str_cat2(str, "not_after=");
    rb_str_append(str, rb_inspect(ossl_x509_get_not_after(self)));

    str = rb_str_cat2(str, ">");

    return str;
}

/*
 * INIT
 */
void
Init_ossl_x509cert()
{

#if 0
    mOSSL = rb_define_module("OpenSSL"); /* let rdoc know about mOSSL */
    mX509 = rb_define_module_under(mOSSL, "X509");
#endif

    eX509CertError = rb_define_class_under(mX509, "CertificateError", eOSSLError);

    /* Document-class: OpenSSL::X509::Certificate
     *
     * Implementation of an X.509 certificate as specified in RFC 5280.
     * Provides access to a certificate's attributes and allows certificates
     * to be read from a string, but also supports the creation of new
     * certificates from scratch.
     *
     * === Reading a certificate from a file
     *
     * Certificate is capable of handling DER-encoded certificates and
     * certificates encoded in OpenSSL's PEM format.
     *
     *   raw = File.read "cert.cer" # DER- or PEM-encoded
     *   certificate = OpenSSL::X509::Certificate.new raw
     *
     * === Saving a certificate to a file
     *
     * A certificate may be encoded in DER format
     *
     *   cert = ...
     *   File.open("cert.cer", "wb") { |f| f.print cert.to_der }
     *
     * or in PEM format
     *
     *   cert = ...
     *   File.open("cert.pem", "wb") { |f| f.print cert.to_pem }
     *
     * X.509 certificates are associated with a private/public key pair,
     * typically a RSA, DSA or ECC key (see also OpenSSL::PKey::RSA,
     * OpenSSL::PKey::DSA and OpenSSL::PKey::EC), the public key itself is
     * stored within the certificate and can be accessed in form of an
     * OpenSSL::PKey. Certificates are typically used to be able to associate
     * some form of identity with a key pair, for example web servers serving
     * pages over HTTPs use certificates to authenticate themselves to the user.
     *
     * The public key infrastructure (PKI) model relies on trusted certificate
     * authorities ("root CAs") that issue these certificates, so that end
     * users need to base their trust just on a selected few authorities
     * that themselves again vouch for subordinate CAs issuing their
     * certificates to end users.
     *
     * The OpenSSL::X509 module provides the tools to set up an independent
     * PKI, similar to scenarios where the 'openssl' command line tool is
     * used for issuing certificates in a private PKI.
     *
     * === Creating a root CA certificate and an end-entity certificate
     *
     * First, we need to create a "self-signed" root certificate. To do so,
     * we need to generate a key first. Please note that the choice of "1"
     * as a serial number is considered a security flaw for real certificates.
     * Secure choices are integers in the two-digit byte range and ideally
     * not sequential but secure random numbers, steps omitted here to keep
     * the example concise.
     *
     *   root_key = OpenSSL::PKey::RSA.new 2048 # the CA's public/private key
     *   root_ca = OpenSSL::X509::Certificate.new
     *   root_ca.version = 2 # cf. RFC 5280 - to make it a "v3" certificate
     *   root_ca.serial = 1
     *   root_ca.subject = OpenSSL::X509::Name.parse "/DC=org/DC=ruby-lang/CN=Ruby CA"
     *   root_ca.issuer = root_ca.subject # root CA's are "self-signed"
     *   root_ca.public_key = root_key.public_key
     *   root_ca.not_before = Time.now
     *   root_ca.not_after = root_ca.not_before + 2 * 365 * 24 * 60 * 60 # 2 years validity
     *   ef = OpenSSL::X509::ExtensionFactory.new
     *   ef.subject_certificate = root_ca
     *   ef.issuer_certificate = root_ca
     *   root_ca.add_extension(ef.create_extension("basicConstraints","CA:TRUE",true))
     *   root_ca.add_extension(ef.create_extension("keyUsage","keyCertSign, cRLSign", true))
     *   root_ca.add_extension(ef.create_extension("subjectKeyIdentifier","hash",false))
     *   root_ca.add_extension(ef.create_extension("authorityKeyIdentifier","keyid:always",false))
     *   root_ca.sign(root_key, OpenSSL::Digest::SHA256.new)
     *
     * The next step is to create the end-entity certificate using the root CA
     * certificate.
     *
     *   key = OpenSSL::PKey::RSA.new 2048
     *   cert = OpenSSL::X509::Certificate.new
     *   cert.version = 2
     *   cert.serial = 2
     *   cert.subject = OpenSSL::X509::Name.parse "/DC=org/DC=ruby-lang/CN=Ruby certificate"
     *   cert.issuer = root_ca.subject # root CA is the issuer
     *   cert.public_key = key.public_key
     *   cert.not_before = Time.now
     *   cert.not_after = cert.not_before + 1 * 365 * 24 * 60 * 60 # 1 years validity
     *   ef = OpenSSL::X509::ExtensionFactory.new
     *   ef.subject_certificate = cert
     *   ef.issuer_certificate = root_ca
     *   cert.add_extension(ef.create_extension("keyUsage","digitalSignature", true))
     *   cert.add_extension(ef.create_extension("subjectKeyIdentifier","hash",false))
     *   cert.sign(root_key, OpenSSL::Digest::SHA256.new)
     *
     */
    cX509Cert = rb_define_class_under(mX509, "Certificate", rb_cObject);

    rb_define_alloc_func(cX509Cert, ossl_x509_alloc);
    rb_define_method(cX509Cert, "initialize", ossl_x509_initialize, -1);
    rb_define_copy_func(cX509Cert, ossl_x509_copy);

    rb_define_method(cX509Cert, "to_der", ossl_x509_to_der, 0);
    rb_define_method(cX509Cert, "to_pem", ossl_x509_to_pem, 0);
    rb_define_alias(cX509Cert, "to_s", "to_pem");
    rb_define_method(cX509Cert, "to_text", ossl_x509_to_text, 0);
    rb_define_method(cX509Cert, "version", ossl_x509_get_version, 0);
    rb_define_method(cX509Cert, "version=", ossl_x509_set_version, 1);
    rb_define_method(cX509Cert, "signature_algorithm", ossl_x509_get_signature_algorithm, 0);
    rb_define_method(cX509Cert, "serial", ossl_x509_get_serial, 0);
    rb_define_method(cX509Cert, "serial=", ossl_x509_set_serial, 1);
    rb_define_method(cX509Cert, "subject", ossl_x509_get_subject, 0);
    rb_define_method(cX509Cert, "subject=", ossl_x509_set_subject, 1);
    rb_define_method(cX509Cert, "issuer", ossl_x509_get_issuer, 0);
    rb_define_method(cX509Cert, "issuer=", ossl_x509_set_issuer, 1);
    rb_define_method(cX509Cert, "not_before", ossl_x509_get_not_before, 0);
    rb_define_method(cX509Cert, "not_before=", ossl_x509_set_not_before, 1);
    rb_define_method(cX509Cert, "not_after", ossl_x509_get_not_after, 0);
    rb_define_method(cX509Cert, "not_after=", ossl_x509_set_not_after, 1);
    rb_define_method(cX509Cert, "public_key", ossl_x509_get_public_key, 0);
    rb_define_method(cX509Cert, "public_key=", ossl_x509_set_public_key, 1);
    rb_define_method(cX509Cert, "sign", ossl_x509_sign, 2);
    rb_define_method(cX509Cert, "verify", ossl_x509_verify, 1);
    rb_define_method(cX509Cert, "check_private_key", ossl_x509_check_private_key, 1);
    rb_define_method(cX509Cert, "extensions", ossl_x509_get_extensions, 0);
    rb_define_method(cX509Cert, "extensions=", ossl_x509_set_extensions, 1);
    rb_define_method(cX509Cert, "add_extension", ossl_x509_add_extension, 1);
    rb_define_method(cX509Cert, "inspect", ossl_x509_inspect, 0);
}

