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
#if !defined(OPENSSL_NO_DSA)

#include "ossl.h"

#define GetPKeyDSA(obj, pkey) do { \
    GetPKey((obj), (pkey)); \
    if (EVP_PKEY_type((pkey)->type) != EVP_PKEY_DSA) { /* PARANOIA? */ \
	ossl_raise(rb_eRuntimeError, "THIS IS NOT A DSA!"); \
    } \
} while (0)

#define DSA_HAS_PRIVATE(dsa) ((dsa)->priv_key)
#define DSA_PRIVATE(obj,dsa) (DSA_HAS_PRIVATE(dsa)||OSSL_PKEY_IS_PRIVATE(obj))

/*
 * Classes
 */
VALUE cDSA;
VALUE eDSAError;

/*
 * Public
 */
static VALUE
dsa_instance(VALUE klass, DSA *dsa)
{
    EVP_PKEY *pkey;
    VALUE obj;

    if (!dsa) {
	return Qfalse;
    }
    if (!(pkey = EVP_PKEY_new())) {
	return Qfalse;
    }
    if (!EVP_PKEY_assign_DSA(pkey, dsa)) {
	EVP_PKEY_free(pkey);
	return Qfalse;
    }
    WrapPKey(klass, obj, pkey);

    return obj;
}

VALUE
ossl_dsa_new(EVP_PKEY *pkey)
{
    VALUE obj;

    if (!pkey) {
	obj = dsa_instance(cDSA, DSA_new());
    } else {
	if (EVP_PKEY_type(pkey->type) != EVP_PKEY_DSA) {
	    ossl_raise(rb_eTypeError, "Not a DSA key!");
	}
	WrapPKey(cDSA, obj, pkey);
    }
    if (obj == Qfalse) {
	ossl_raise(eDSAError, NULL);
    }

    return obj;
}

/*
 * Private
 */
#if defined(HAVE_DSA_GENERATE_PARAMETERS_EX) && HAVE_BN_GENCB
struct dsa_blocking_gen_arg {
    DSA *dsa;
    int size;
    unsigned char* seed;
    int seed_len;
    int *counter;
    unsigned long *h;
    BN_GENCB *cb;
    int result;
};

static VALUE
dsa_blocking_gen(void *arg)
{
    struct dsa_blocking_gen_arg *gen = (struct dsa_blocking_gen_arg *)arg;
    gen->result = DSA_generate_parameters_ex(gen->dsa, gen->size, gen->seed, gen->seed_len, gen->counter, gen->h, gen->cb);
    return Qnil;
}
#endif

static DSA *
dsa_generate(int size)
{
#if defined(HAVE_DSA_GENERATE_PARAMETERS_EX) && HAVE_BN_GENCB
    BN_GENCB cb;
    struct ossl_generate_cb_arg cb_arg;
    struct dsa_blocking_gen_arg gen_arg;
    DSA *dsa = DSA_new();
    unsigned char seed[20];
    int seed_len = 20, counter;
    unsigned long h;

    if (!dsa) return 0;
    if (!RAND_bytes(seed, seed_len)) {
	DSA_free(dsa);
	return 0;
    }

    memset(&cb_arg, 0, sizeof(struct ossl_generate_cb_arg));
    if (rb_block_given_p())
	cb_arg.yield = 1;
    BN_GENCB_set(&cb, ossl_generate_cb_2, &cb_arg);
    gen_arg.dsa = dsa;
    gen_arg.size = size;
    gen_arg.seed = seed;
    gen_arg.seed_len = seed_len;
    gen_arg.counter = &counter;
    gen_arg.h = &h;
    gen_arg.cb = &cb;
    if (cb_arg.yield == 1) {
	/* we cannot release GVL when callback proc is supplied */
	dsa_blocking_gen(&gen_arg);
    } else {
	/* there's a chance to unblock */
	rb_thread_blocking_region(dsa_blocking_gen, &gen_arg, ossl_generate_cb_stop, &cb_arg);
    }
    if (!gen_arg.result) {
	DSA_free(dsa);
	if (cb_arg.state) rb_jump_tag(cb_arg.state);
	return 0;
    }
#else
    DSA *dsa;
    unsigned char seed[20];
    int seed_len = 20, counter;
    unsigned long h;

    if (!RAND_bytes(seed, seed_len)) {
	return 0;
    }
    dsa = DSA_generate_parameters(size, seed, seed_len, &counter, &h,
	    rb_block_given_p() ? ossl_generate_cb : NULL, NULL);
    if(!dsa) return 0;
#endif

    if (!DSA_generate_key(dsa)) {
	DSA_free(dsa);
	return 0;
    }

    return dsa;
}

/*
 *  call-seq:
 *    DSA.generate(size) -> dsa
 *
 * Creates a new DSA instance by generating a private/public key pair
 * from scratch.
 *
 * === Parameters
 * * +size+ is an integer representing the desired key size.
 *
 */
static VALUE
ossl_dsa_s_generate(VALUE klass, VALUE size)
{
    DSA *dsa = dsa_generate(NUM2INT(size)); /* err handled by dsa_instance */
    VALUE obj = dsa_instance(klass, dsa);

    if (obj == Qfalse) {
	DSA_free(dsa);
	ossl_raise(eDSAError, NULL);
    }

    return obj;
}

/*
 *  call-seq:
 *    DSA.new([size | string [, pass]) -> dsa
 *
 * Creates a new DSA instance by reading an existing key from +string+.
 *
 * === Parameters
 * * +size+ is an integer representing the desired key size.
 * * +string+ contains a DER or PEM encoded key.
 * * +pass+ is a string that contains an optional password.
 *
 * === Examples
 *  DSA.new -> dsa
 *  DSA.new(1024) -> dsa
 *  DSA.new(File.read('dsa.pem')) -> dsa
 *  DSA.new(File.read('dsa.pem'), 'mypassword') -> dsa
 *
 */
static VALUE
ossl_dsa_initialize(int argc, VALUE *argv, VALUE self)
{
    EVP_PKEY *pkey;
    DSA *dsa;
    BIO *in;
    char *passwd = NULL;
    VALUE arg, pass;

    GetPKey(self, pkey);
    if(rb_scan_args(argc, argv, "02", &arg, &pass) == 0) {
        dsa = DSA_new();
    }
    else if (FIXNUM_P(arg)) {
	if (!(dsa = dsa_generate(FIX2INT(arg)))) {
	    ossl_raise(eDSAError, NULL);
	}
    }
    else {
	if (!NIL_P(pass)) passwd = StringValuePtr(pass);
	arg = ossl_to_der_if_possible(arg);
	in = ossl_obj2bio(arg);
	dsa = PEM_read_bio_DSAPrivateKey(in, NULL, ossl_pem_passwd_cb, passwd);
	if (!dsa) {
	    OSSL_BIO_reset(in);
	    dsa = PEM_read_bio_DSA_PUBKEY(in, NULL, NULL, NULL);
	}
	if (!dsa) {
	    OSSL_BIO_reset(in);
	    dsa = d2i_DSAPrivateKey_bio(in, NULL);
	}
	if (!dsa) {
	    OSSL_BIO_reset(in);
	    dsa = d2i_DSA_PUBKEY_bio(in, NULL);
	}
	if (!dsa) {
	    OSSL_BIO_reset(in);
	    dsa = PEM_read_bio_DSAPublicKey(in, NULL, NULL, NULL);
	}
	BIO_free(in);
	if (!dsa) {
	    ERR_clear_error();
	    ossl_raise(eDSAError, "Neither PUB key nor PRIV key");
	}
    }
    if (!EVP_PKEY_assign_DSA(pkey, dsa)) {
	DSA_free(dsa);
	ossl_raise(eDSAError, NULL);
    }

    return self;
}

/*
 *  call-seq:
 *    dsa.public? -> true | false
 *
 * Indicates whether this DSA instance has a public key associated with it or
 * not. The public key may be retrieved with DSA#public_key.
 */
static VALUE
ossl_dsa_is_public(VALUE self)
{
    EVP_PKEY *pkey;

    GetPKeyDSA(self, pkey);

    return (pkey->pkey.dsa->pub_key) ? Qtrue : Qfalse;
}

/*
 *  call-seq:
 *    dsa.private? -> true | false
 *
 * Indicates whether this DSA instance has a private key associated with it or
 * not. The private key may be retrieved with DSA#private_key.
 */
static VALUE
ossl_dsa_is_private(VALUE self)
{
    EVP_PKEY *pkey;

    GetPKeyDSA(self, pkey);

    return (DSA_PRIVATE(self, pkey->pkey.dsa)) ? Qtrue : Qfalse;
}

/*
 *  call-seq:
 *    dsa.to_pem([cipher, password]) -> aString
 *
 * Encodes this DSA to its PEM encoding.
 *
 * === Parameters
 * * +cipher+ is an OpenSSL::Cipher.
 * * +password+ is a string containing your password.
 *
 * === Examples
 *  DSA.to_pem -> aString
 *  DSA.to_pem(cipher, 'mypassword') -> aString
 *
 */
static VALUE
ossl_dsa_export(int argc, VALUE *argv, VALUE self)
{
    EVP_PKEY *pkey;
    BIO *out;
    const EVP_CIPHER *ciph = NULL;
    char *passwd = NULL;
    VALUE cipher, pass, str;

    GetPKeyDSA(self, pkey);
    rb_scan_args(argc, argv, "02", &cipher, &pass);
    if (!NIL_P(cipher)) {
	ciph = GetCipherPtr(cipher);
	if (!NIL_P(pass)) {
	    passwd = StringValuePtr(pass);
	}
    }
    if (!(out = BIO_new(BIO_s_mem()))) {
	ossl_raise(eDSAError, NULL);
    }
    if (DSA_HAS_PRIVATE(pkey->pkey.dsa)) {
	if (!PEM_write_bio_DSAPrivateKey(out, pkey->pkey.dsa, ciph,
					 NULL, 0, ossl_pem_passwd_cb, passwd)){
	    BIO_free(out);
	    ossl_raise(eDSAError, NULL);
	}
    } else {
	if (!PEM_write_bio_DSA_PUBKEY(out, pkey->pkey.dsa)) {
	    BIO_free(out);
	    ossl_raise(eDSAError, NULL);
	}
    }
    str = ossl_membio2str(out);

    return str;
}

/*
 *  call-seq:
 *    dsa.to_der -> aString
 *
 * Encodes this DSA to its DER encoding.
 *
 */
static VALUE
ossl_dsa_to_der(VALUE self)
{
    EVP_PKEY *pkey;
    int (*i2d_func)_((DSA*, unsigned char**));
    unsigned char *p;
    long len;
    VALUE str;

    GetPKeyDSA(self, pkey);
    if(DSA_HAS_PRIVATE(pkey->pkey.dsa))
	i2d_func = (int(*)_((DSA*,unsigned char**)))i2d_DSAPrivateKey;
    else
	i2d_func = i2d_DSA_PUBKEY;
    if((len = i2d_func(pkey->pkey.dsa, NULL)) <= 0)
	ossl_raise(eDSAError, NULL);
    str = rb_str_new(0, len);
    p = (unsigned char *)RSTRING_PTR(str);
    if(i2d_func(pkey->pkey.dsa, &p) < 0)
	ossl_raise(eDSAError, NULL);
    ossl_str_adjust(str, p);

    return str;
}

/*
 *  call-seq:
 *    dsa.params -> hash
 *
 * Stores all parameters of key to the hash
 * INSECURE: PRIVATE INFORMATIONS CAN LEAK OUT!!!
 * Don't use :-)) (I's up to you)
 */
static VALUE
ossl_dsa_get_params(VALUE self)
{
    EVP_PKEY *pkey;
    VALUE hash;

    GetPKeyDSA(self, pkey);

    hash = rb_hash_new();

    rb_hash_aset(hash, rb_str_new2("p"), ossl_bn_new(pkey->pkey.dsa->p));
    rb_hash_aset(hash, rb_str_new2("q"), ossl_bn_new(pkey->pkey.dsa->q));
    rb_hash_aset(hash, rb_str_new2("g"), ossl_bn_new(pkey->pkey.dsa->g));
    rb_hash_aset(hash, rb_str_new2("pub_key"), ossl_bn_new(pkey->pkey.dsa->pub_key));
    rb_hash_aset(hash, rb_str_new2("priv_key"), ossl_bn_new(pkey->pkey.dsa->priv_key));

    return hash;
}

/*
 *  call-seq:
 *    dsa.to_text -> aString
 *
 * Prints all parameters of key to buffer
 * INSECURE: PRIVATE INFORMATIONS CAN LEAK OUT!!!
 * Don't use :-)) (I's up to you)
 */
static VALUE
ossl_dsa_to_text(VALUE self)
{
    EVP_PKEY *pkey;
    BIO *out;
    VALUE str;

    GetPKeyDSA(self, pkey);
    if (!(out = BIO_new(BIO_s_mem()))) {
	ossl_raise(eDSAError, NULL);
    }
    if (!DSA_print(out, pkey->pkey.dsa, 0)) { /* offset = 0 */
	BIO_free(out);
	ossl_raise(eDSAError, NULL);
    }
    str = ossl_membio2str(out);

    return str;
}

/*
 *  call-seq:
 *    dsa.public_key -> aDSA
 *
 * Returns a new DSA instance that carries just the public key information.
 * If the current instance has also private key information, this will no
 * longer be present in the new instance. This feature is helpful for
 * publishing the public key information without leaking any of the private
 * information.
 *
 * === Example
 *  dsa = OpenSSL::PKey::DSA.new(2048) # has public and private information
 *  pub_key = dsa.public_key # has only the public part available
 *  pub_key_der = pub_key.to_der # it's safe to publish this
 *
 *
 */
static VALUE
ossl_dsa_to_public_key(VALUE self)
{
    EVP_PKEY *pkey;
    DSA *dsa;
    VALUE obj;

    GetPKeyDSA(self, pkey);
    /* err check performed by dsa_instance */
    dsa = DSAPublicKey_dup(pkey->pkey.dsa);
    obj = dsa_instance(CLASS_OF(self), dsa);
    if (obj == Qfalse) {
	DSA_free(dsa);
	ossl_raise(eDSAError, NULL);
    }
    return obj;
}

#define ossl_dsa_buf_size(pkey) (DSA_size((pkey)->pkey.dsa)+16)

/*
 *  call-seq:
 *    dsa.syssign(string) -> aString
 *
 * Computes and returns the DSA signature of +string+, where +string+ is
 * expected to be an already-computed message digest of the original input
 * data. The signature is issued using the private key of this DSA instance.
 *
 * === Parameters
 * * +string+ is a message digest of the original input data to be signed
 *
 * === Example
 *  dsa = OpenSSL::PKey::DSA.new(2048)
 *  doc = "Sign me"
 *  digest = OpenSSL::Digest::SHA1.digest(doc)
 *  sig = dsa.syssign(digest)
 *
 *
 */
static VALUE
ossl_dsa_sign(VALUE self, VALUE data)
{
    EVP_PKEY *pkey;
    unsigned int buf_len;
    VALUE str;

    GetPKeyDSA(self, pkey);
    StringValue(data);
    if (!DSA_PRIVATE(self, pkey->pkey.dsa)) {
	ossl_raise(eDSAError, "Private DSA key needed!");
    }
    str = rb_str_new(0, ossl_dsa_buf_size(pkey));
    if (!DSA_sign(0, (unsigned char *)RSTRING_PTR(data), RSTRING_LENINT(data),
		  (unsigned char *)RSTRING_PTR(str),
		  &buf_len, pkey->pkey.dsa)) { /* type is ignored (0) */
	ossl_raise(eDSAError, NULL);
    }
    rb_str_set_len(str, buf_len);

    return str;
}

/*
 *  call-seq:
 *    dsa.sysverify(digest, sig) -> true | false
 *
 * Verifies whether the signature is valid given the message digest input. It
 * does so by validating +sig+ using the public key of this DSA instance.
 *
 * === Parameters
 * * +digest+ is a message digest of the original input data to be signed
 * * +sig+ is a DSA signature value
 *
 * === Example
 *  dsa = OpenSSL::PKey::DSA.new(2048)
 *  doc = "Sign me"
 *  digest = OpenSSL::Digest::SHA1.digest(doc)
 *  sig = dsa.syssign(digest)
 *  puts dsa.sysverify(digest, sig) # => true
 *
 */
static VALUE
ossl_dsa_verify(VALUE self, VALUE digest, VALUE sig)
{
    EVP_PKEY *pkey;
    int ret;

    GetPKeyDSA(self, pkey);
    StringValue(digest);
    StringValue(sig);
    /* type is ignored (0) */
    ret = DSA_verify(0, (unsigned char *)RSTRING_PTR(digest), RSTRING_LENINT(digest),
		     (unsigned char *)RSTRING_PTR(sig), RSTRING_LENINT(sig), pkey->pkey.dsa);
    if (ret < 0) {
	ossl_raise(eDSAError, NULL);
    }
    else if (ret == 1) {
	return Qtrue;
    }

    return Qfalse;
}

OSSL_PKEY_BN(dsa, p)
OSSL_PKEY_BN(dsa, q)
OSSL_PKEY_BN(dsa, g)
OSSL_PKEY_BN(dsa, pub_key)
OSSL_PKEY_BN(dsa, priv_key)

/*
 * INIT
 */
void
Init_ossl_dsa()
{
#if 0
    mOSSL = rb_define_module("OpenSSL"); /* let rdoc know about mOSSL and mPKey */
    mPKey = rb_define_module_under(mOSSL, "PKey");
#endif

    /* Document-class: OpenSSL::PKey::DSAError
     *
     * Generic exception that is raised if an operation on a DSA PKey
     * fails unexpectedly or in case an instantiation of an instance of DSA
     * fails due to non-conformant input data.
     */
    eDSAError = rb_define_class_under(mPKey, "DSAError", ePKeyError);

    /* Document-class: OpenSSL::PKey::DSA
     *
     * DSA, the Digital Signature Algorithm, is specified in NIST's
     * FIPS 186-3. It is an asymmetric public key algorithm that may be used
     * similar to e.g. RSA.
     * Please note that for OpenSSL versions prior to 1.0.0 the digest
     * algorithms OpenSSL::Digest::DSS (equivalent to SHA) or
     * OpenSSL::Digest::DSS1 (equivalent to SHA-1) must be used for issuing
     * signatures with a DSA key using OpenSSL::PKey#sign.
     * Starting with OpenSSL 1.0.0, digest algorithms are no longer restricted,
     * any Digest may be used for signing.
     */
    cDSA = rb_define_class_under(mPKey, "DSA", cPKey);

    rb_define_singleton_method(cDSA, "generate", ossl_dsa_s_generate, 1);
    rb_define_method(cDSA, "initialize", ossl_dsa_initialize, -1);

    rb_define_method(cDSA, "public?", ossl_dsa_is_public, 0);
    rb_define_method(cDSA, "private?", ossl_dsa_is_private, 0);
    rb_define_method(cDSA, "to_text", ossl_dsa_to_text, 0);
    rb_define_method(cDSA, "export", ossl_dsa_export, -1);
    rb_define_alias(cDSA, "to_pem", "export");
    rb_define_alias(cDSA, "to_s", "export");
    rb_define_method(cDSA, "to_der", ossl_dsa_to_der, 0);
    rb_define_method(cDSA, "public_key", ossl_dsa_to_public_key, 0);
    rb_define_method(cDSA, "syssign", ossl_dsa_sign, 1);
    rb_define_method(cDSA, "sysverify", ossl_dsa_verify, 2);

    DEF_OSSL_PKEY_BN(cDSA, dsa, p);
    DEF_OSSL_PKEY_BN(cDSA, dsa, q);
    DEF_OSSL_PKEY_BN(cDSA, dsa, g);
    DEF_OSSL_PKEY_BN(cDSA, dsa, pub_key);
    DEF_OSSL_PKEY_BN(cDSA, dsa, priv_key);

    rb_define_method(cDSA, "params", ossl_dsa_get_params, 0);
}

#else /* defined NO_DSA */
void
Init_ossl_dsa()
{
}
#endif /* NO_DSA */
