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
VALUE mPKey;
VALUE cPKey;
VALUE ePKeyError;
ID id_private_q;

/*
 * callback for generating keys
 */
void
ossl_generate_cb(int p, int n, void *arg)
{
    VALUE ary;

    ary = rb_ary_new2(2);
    rb_ary_store(ary, 0, INT2NUM(p));
    rb_ary_store(ary, 1, INT2NUM(n));

    rb_yield(ary);
}

#if HAVE_BN_GENCB
/* OpenSSL 2nd version of GN generation callback */
int
ossl_generate_cb_2(int p, int n, BN_GENCB *cb)
{
    VALUE ary;
    struct ossl_generate_cb_arg *arg;
    int state;

    arg = (struct ossl_generate_cb_arg *)cb->arg;
    if (arg->yield) {
	ary = rb_ary_new2(2);
	rb_ary_store(ary, 0, INT2NUM(p));
	rb_ary_store(ary, 1, INT2NUM(n));

	/*
	* can be break by raising exception or 'break'
	*/
	rb_protect(rb_yield, ary, &state);
	if (state) {
	    arg->stop = 1;
	    arg->state = state;
	}
    }
    if (arg->stop) return 0;
    return 1;
}

void
ossl_generate_cb_stop(void *ptr)
{
    struct ossl_generate_cb_arg *arg = (struct ossl_generate_cb_arg *)ptr;
    arg->stop = 1;
}
#endif

/*
 * Public
 */
VALUE
ossl_pkey_new(EVP_PKEY *pkey)
{
    if (!pkey) {
	ossl_raise(ePKeyError, "Cannot make new key from NULL.");
    }
    switch (EVP_PKEY_type(pkey->type)) {
#if !defined(OPENSSL_NO_RSA)
    case EVP_PKEY_RSA:
	return ossl_rsa_new(pkey);
#endif
#if !defined(OPENSSL_NO_DSA)
    case EVP_PKEY_DSA:
	return ossl_dsa_new(pkey);
#endif
#if !defined(OPENSSL_NO_DH)
    case EVP_PKEY_DH:
	return ossl_dh_new(pkey);
#endif
#if !defined(OPENSSL_NO_EC) && (OPENSSL_VERSION_NUMBER >= 0x0090802fL)
    case EVP_PKEY_EC:
	return ossl_ec_new(pkey);
#endif
    default:
	ossl_raise(ePKeyError, "unsupported key type");
    }
    return Qnil; /* not reached */
}

VALUE
ossl_pkey_new_from_file(VALUE filename)
{
    FILE *fp;
    EVP_PKEY *pkey;

    SafeStringValue(filename);
    if (!(fp = fopen(RSTRING_PTR(filename), "r"))) {
	ossl_raise(ePKeyError, "%s", strerror(errno));
    }
    rb_fd_fix_cloexec(fileno(fp));

    pkey = PEM_read_PrivateKey(fp, NULL, ossl_pem_passwd_cb, NULL);
    fclose(fp);
    if (!pkey) {
	ossl_raise(ePKeyError, NULL);
    }

    return ossl_pkey_new(pkey);
}

/*
 *  call-seq:
 *     OpenSSL::PKey.read(string [, pwd ] ) -> PKey
 *     OpenSSL::PKey.read(file [, pwd ]) -> PKey
 *
 * === Parameters
 * * +string+ is a DER- or PEM-encoded string containing an arbitrary private
 * or public key.
 * * +file+ is an instance of +File+ containing a DER- or PEM-encoded
 * arbitrary private or public key.
 * * +pwd+ is an optional password in case +string+ or +file+ is an encrypted
 * PEM resource.
 */
static VALUE
ossl_pkey_new_from_data(int argc, VALUE *argv, VALUE self)
{
     EVP_PKEY *pkey;
     BIO *bio;
     VALUE data, pass;
     char *passwd = NULL;

     rb_scan_args(argc, argv, "11", &data, &pass);

     bio = ossl_obj2bio(data);
     if (!(pkey = d2i_PrivateKey_bio(bio, NULL))) {
	OSSL_BIO_reset(bio);
	if (!NIL_P(pass)) {
	    passwd = StringValuePtr(pass);
	}
	if (!(pkey = PEM_read_bio_PrivateKey(bio, NULL, ossl_pem_passwd_cb, passwd))) {
	    OSSL_BIO_reset(bio);
	    if (!(pkey = d2i_PUBKEY_bio(bio, NULL))) {
		OSSL_BIO_reset(bio);
		if (!NIL_P(pass)) {
		    passwd = StringValuePtr(pass);
		}
		pkey = PEM_read_bio_PUBKEY(bio, NULL, ossl_pem_passwd_cb, passwd);
	    }
	}
    }

    BIO_free(bio);
    if (!pkey)
	ossl_raise(rb_eArgError, "Could not parse PKey");
    return ossl_pkey_new(pkey);
}

EVP_PKEY *
GetPKeyPtr(VALUE obj)
{
    EVP_PKEY *pkey;

    SafeGetPKey(obj, pkey);

    return pkey;
}

EVP_PKEY *
GetPrivPKeyPtr(VALUE obj)
{
    EVP_PKEY *pkey;

    if (rb_funcall(obj, id_private_q, 0, NULL) != Qtrue) {
	ossl_raise(rb_eArgError, "Private key is needed.");
    }
    SafeGetPKey(obj, pkey);

    return pkey;
}

EVP_PKEY *
DupPKeyPtr(VALUE obj)
{
    EVP_PKEY *pkey;

    SafeGetPKey(obj, pkey);
    CRYPTO_add(&pkey->references, 1, CRYPTO_LOCK_EVP_PKEY);

    return pkey;
}

EVP_PKEY *
DupPrivPKeyPtr(VALUE obj)
{
    EVP_PKEY *pkey;

    if (rb_funcall(obj, id_private_q, 0, NULL) != Qtrue) {
	ossl_raise(rb_eArgError, "Private key is needed.");
    }
    SafeGetPKey(obj, pkey);
    CRYPTO_add(&pkey->references, 1, CRYPTO_LOCK_EVP_PKEY);

    return pkey;
}

/*
 * Private
 */
static VALUE
ossl_pkey_alloc(VALUE klass)
{
    EVP_PKEY *pkey;
    VALUE obj;

    if (!(pkey = EVP_PKEY_new())) {
	ossl_raise(ePKeyError, NULL);
    }
    WrapPKey(klass, obj, pkey);

    return obj;
}

/*
 *  call-seq:
 *      PKeyClass.new -> self
 *
 * Because PKey is an abstract class, actually calling this method explicitly
 * will raise a +NotImplementedError+.
 */
static VALUE
ossl_pkey_initialize(VALUE self)
{
    if (rb_obj_is_instance_of(self, cPKey)) {
	ossl_raise(rb_eNotImpError, "OpenSSL::PKey::PKey is an abstract class.");
    }
    return self;
}

/*
 *  call-seq:
 *      pkey.sign(digest, data) -> String
 *
 * To sign the +String+ +data+, +digest+, an instance of OpenSSL::Digest, must
 * be provided. The return value is again a +String+ containing the signature.
 * A PKeyError is raised should errors occur.
 * Any previous state of the +Digest+ instance is irrelevant to the signature
 * outcome, the digest instance is reset to its initial state during the
 * operation.
 *
 * == Example
 *   data = 'Sign me!'
 *   digest = OpenSSL::Digest::SHA256.new
 *   pkey = OpenSSL::PKey::RSA.new(2048)
 *   signature = pkey.sign(digest, data)
 */
static VALUE
ossl_pkey_sign(VALUE self, VALUE digest, VALUE data)
{
    EVP_PKEY *pkey;
    EVP_MD_CTX ctx;
    unsigned int buf_len;
    VALUE str;

    if (rb_funcall(self, id_private_q, 0, NULL) != Qtrue) {
	ossl_raise(rb_eArgError, "Private key is needed.");
    }
    GetPKey(self, pkey);
    EVP_SignInit(&ctx, GetDigestPtr(digest));
    StringValue(data);
    EVP_SignUpdate(&ctx, RSTRING_PTR(data), RSTRING_LEN(data));
    str = rb_str_new(0, EVP_PKEY_size(pkey)+16);
    if (!EVP_SignFinal(&ctx, (unsigned char *)RSTRING_PTR(str), &buf_len, pkey))
	ossl_raise(ePKeyError, NULL);
    assert((long)buf_len <= RSTRING_LEN(str));
    rb_str_set_len(str, buf_len);

    return str;
}

/*
 *  call-seq:
 *      pkey.verify(digest, signature, data) -> String
 *
 * To verify the +String+ +signature+, +digest+, an instance of
 * OpenSSL::Digest, must be provided to re-compute the message digest of the
 * original +data+, also a +String+. The return value is +true+ if the
 * signature is valid, +false+ otherwise. A PKeyError is raised should errors
 * occur.
 * Any previous state of the +Digest+ instance is irrelevant to the validation
 * outcome, the digest instance is reset to its initial state during the
 * operation.
 *
 * == Example
 *   data = 'Sign me!'
 *   digest = OpenSSL::Digest::SHA256.new
 *   pkey = OpenSSL::PKey::RSA.new(2048)
 *   signature = pkey.sign(digest, data)
 *   pub_key = pkey.public_key
 *   puts pub_key.verify(digest, signature, data) # => true
 */
static VALUE
ossl_pkey_verify(VALUE self, VALUE digest, VALUE sig, VALUE data)
{
    EVP_PKEY *pkey;
    EVP_MD_CTX ctx;

    GetPKey(self, pkey);
    EVP_VerifyInit(&ctx, GetDigestPtr(digest));
    StringValue(sig);
    StringValue(data);
    EVP_VerifyUpdate(&ctx, RSTRING_PTR(data), RSTRING_LEN(data));
    switch (EVP_VerifyFinal(&ctx, (unsigned char *)RSTRING_PTR(sig), RSTRING_LENINT(sig), pkey)) {
    case 0:
	return Qfalse;
    case 1:
	return Qtrue;
    default:
	ossl_raise(ePKeyError, NULL);
    }
    return Qnil; /* dummy */
}

/*
 * INIT
 */
void
Init_ossl_pkey()
{
#if 0
    mOSSL = rb_define_module("OpenSSL"); /* let rdoc know about mOSSL */
#endif

    /* Document-module: OpenSSL::PKey
     *
     * == Asymmetric Public Key Algorithms
     *
     * Asymmetric public key algorithms solve the problem of establishing and
     * sharing secret keys to en-/decrypt messages. The key in such an
     * algorithm consists of two parts: a public key that may be distributed
     * to others and a private key that needs to remain secret.
     *
     * Messages encrypted with a public key can only be encrypted by
     * recipients that are in possession of the associated private key.
     * Since public key algorithms are considerably slower than symmetric
     * key algorithms (cf. OpenSSL::Cipher) they are often used to establish
     * a symmetric key shared between two parties that are in possession of
     * each other's public key.
     *
     * Asymmetric algorithms offer a lot of nice features that are used in a
     * lot of different areas. A very common application is the creation and
     * validation of digital signatures. To sign a document, the signatory
     * generally uses a message digest algorithm (cf. OpenSSL::Digest) to
     * compute a digest of the document that is then encrypted (i.e. signed)
     * using the private key. Anyone in possession of the public key may then
     * verify the signature by computing the message digest of the original
     * document on their own, decrypting the signature using the signatory's
     * public key and comparing the result to the message digest they
     * previously computed. The signature is valid if and only if the
     * decrypted signature is equal to this message digest.
     *
     * The PKey module offers support for three popular public/private key
     * algorithms:
     * * RSA (OpenSSL::PKey::RSA)
     * * DSA (OpenSSL::PKey::DSA)
     * * Elliptic Curve Cryptography (OpenSSL::PKey::EC)
     * Each of these implementations is in fact a sub-class of the abstract
     * PKey class which offers the interface for supporting digital signatures
     * in the form of PKey#sign and PKey#verify.
     *
     * == Diffie-Hellman Key Exchange
     *
     * Finally PKey also features OpenSSL::PKey::DH, an implementation of
     * the Diffie-Hellman key exchange protocol based on discrete logarithms
     * in finite fields, the same basis that DSA is built on.
     * The Diffie-Hellman protocol can be used to exchange (symmetric) keys
     * over insecure channels without needing any prior joint knowledge
     * between the participating parties. As the security of DH demands
     * relatively long "public keys" (i.e. the part that is overtly
     * transmitted between participants) DH tends to be quite slow. If
     * security or speed is your primary concern, OpenSSL::PKey::EC offers
     * another implementation of the Diffie-Hellman protocol.
     *
     */
    mPKey = rb_define_module_under(mOSSL, "PKey");

    /* Document-class: OpenSSL::PKey::PKeyError
     *
     *Raised when errors occur during PKey#sign or PKey#verify.
     */
    ePKeyError = rb_define_class_under(mPKey, "PKeyError", eOSSLError);

    /* Document-class: OpenSSL::PKey::PKey
     *
     * An abstract class that bundles signature creation (PKey#sign) and
     * validation (PKey#verify) that is common to all implementations except
     * OpenSSL::PKey::DH
     * * OpenSSL::PKey::RSA
     * * OpenSSL::PKey::DSA
     * * OpenSSL::PKey::EC
     */
    cPKey = rb_define_class_under(mPKey, "PKey", rb_cObject);

    rb_define_module_function(mPKey, "read", ossl_pkey_new_from_data, -1);

    rb_define_alloc_func(cPKey, ossl_pkey_alloc);
    rb_define_method(cPKey, "initialize", ossl_pkey_initialize, 0);

    rb_define_method(cPKey, "sign", ossl_pkey_sign, 2);
    rb_define_method(cPKey, "verify", ossl_pkey_verify, 3);

    id_private_q = rb_intern("private?");

    /*
     * INIT rsa, dsa, dh, ec
     */
    Init_ossl_rsa();
    Init_ossl_dsa();
    Init_ossl_dh();
    Init_ossl_ec();
}

