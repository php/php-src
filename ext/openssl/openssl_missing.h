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
#if !defined(_OSSL_OPENSSL_MISSING_H_)
#define _OSSL_OPENSSL_MISSING_H_

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef TYPEDEF_D2I_OF
typedef char *d2i_of_void();
#endif
#ifndef TYPEDEF_I2D_OF
typedef int i2d_of_void();
#endif

/*
 * These functions are not included in headers of OPENSSL <= 0.9.6b
 */

#if !defined(PEM_read_bio_DSAPublicKey)
# define PEM_read_bio_DSAPublicKey(bp,x,cb,u) (DSA *)PEM_ASN1_read_bio( \
        (d2i_of_void *)d2i_DSAPublicKey,PEM_STRING_DSA_PUBLIC,(bp),(void **)(x),(cb),(u))
#endif

#if !defined(PEM_write_bio_DSAPublicKey)
# define PEM_write_bio_DSAPublicKey(bp,x) \
	PEM_ASN1_write_bio((i2d_of_void *)i2d_DSAPublicKey,\
		PEM_STRING_DSA_PUBLIC,\
		(bp),(char *)(x), NULL, NULL, 0, NULL, NULL)
#endif

#if !defined(DSAPrivateKey_dup)
# define DSAPrivateKey_dup(dsa) (DSA *)ASN1_dup((i2d_of_void *)i2d_DSAPrivateKey, \
	(d2i_of_void *)d2i_DSAPrivateKey,(char *)(dsa))
#endif

#if !defined(DSAPublicKey_dup)
# define DSAPublicKey_dup(dsa) (DSA *)ASN1_dup((i2d_of_void *)i2d_DSAPublicKey, \
	(d2i_of_void *)d2i_DSAPublicKey,(char *)(dsa))
#endif

#if !defined(X509_REVOKED_dup)
# define X509_REVOKED_dup(rev) (X509_REVOKED *)ASN1_dup((i2d_of_void *)i2d_X509_REVOKED, \
	(d2i_of_void *)d2i_X509_REVOKED, (char *)(rev))
#endif

#if !defined(PKCS7_SIGNER_INFO_dup)
#  define PKCS7_SIGNER_INFO_dup(si) (PKCS7_SIGNER_INFO *)ASN1_dup((i2d_of_void *)i2d_PKCS7_SIGNER_INFO, \
	(d2i_of_void *)d2i_PKCS7_SIGNER_INFO, (char *)(si))
#endif

#if !defined(PKCS7_RECIP_INFO_dup)
#  define PKCS7_RECIP_INFO_dup(ri) (PKCS7_RECIP_INFO *)ASN1_dup((i2d_of_void *)i2d_PKCS7_RECIP_INFO, \
	(d2i_of_void *)d2i_PKCS7_RECIP_INFO, (char *)(ri))
#endif

#if !defined(HAVE_HMAC_CTX_INIT)
void HMAC_CTX_init(HMAC_CTX *ctx);
#endif

#if !defined(HAVE_HMAC_CTX_COPY)
void HMAC_CTX_copy(HMAC_CTX *out, HMAC_CTX *in);
#endif

#if !defined(HAVE_HMAC_CTX_CLEANUP)
void HMAC_CTX_cleanup(HMAC_CTX *ctx);
#endif

#if !defined(HAVE_EVP_MD_CTX_CREATE)
EVP_MD_CTX *EVP_MD_CTX_create(void);
#endif

#if !defined(HAVE_EVP_MD_CTX_INIT)
void EVP_MD_CTX_init(EVP_MD_CTX *ctx);
#endif

#if !defined(HAVE_EVP_MD_CTX_CLEANUP)
int EVP_MD_CTX_cleanup(EVP_MD_CTX *ctx);
#endif

#if !defined(HAVE_EVP_MD_CTX_DESTROY)
void EVP_MD_CTX_destroy(EVP_MD_CTX *ctx);
#endif

#if !defined(HAVE_EVP_CIPHER_CTX_COPY)
int EVP_CIPHER_CTX_copy(EVP_CIPHER_CTX *out, EVP_CIPHER_CTX *in);
#endif

#if !defined(HAVE_EVP_DIGESTINIT_EX)
#  define EVP_DigestInit_ex(ctx, md, engine) EVP_DigestInit((ctx), (md))
#endif
#if !defined(HAVE_EVP_DIGESTFINAL_EX)
#  define EVP_DigestFinal_ex(ctx, buf, len) EVP_DigestFinal((ctx), (buf), (len))
#endif

#if !defined(HAVE_EVP_CIPHERINIT_EX)
#  define EVP_CipherInit_ex(ctx, type, impl, key, iv, enc) EVP_CipherInit((ctx), (type), (key), (iv), (enc))
#endif
#if !defined(HAVE_EVP_CIPHERFINAL_EX)
#  define EVP_CipherFinal_ex(ctx, outm, outl) EVP_CipherFinal((ctx), (outm), (outl))
#endif

#if !defined(EVP_CIPHER_name)
#  define EVP_CIPHER_name(e) OBJ_nid2sn(EVP_CIPHER_nid(e))
#endif

#if !defined(EVP_MD_name)
#  define EVP_MD_name(e) OBJ_nid2sn(EVP_MD_type(e))
#endif

#if !defined(HAVE_EVP_HMAC_INIT_EX)
#  define HMAC_Init_ex(ctx, key, len, digest, engine) HMAC_Init((ctx), (key), (len), (digest))
#endif

#if !defined(PKCS7_is_detached)
#  define PKCS7_is_detached(p7) (PKCS7_type_is_signed(p7) && PKCS7_get_detached(p7))
#endif

#if !defined(PKCS7_type_is_encrypted)
#  define PKCS7_type_is_encrypted(a) (OBJ_obj2nid((a)->type) == NID_pkcs7_encrypted)
#endif

#if !defined(HAVE_OPENSSL_CLEANSE)
#define OPENSSL_cleanse(p, l) memset((p), 0, (l))
#endif

#if !defined(HAVE_X509_STORE_GET_EX_DATA)
void *X509_STORE_get_ex_data(X509_STORE *str, int idx);
#endif

#if !defined(HAVE_X509_STORE_SET_EX_DATA)
int X509_STORE_set_ex_data(X509_STORE *str, int idx, void *data);
#endif

#if !defined(HAVE_X509_CRL_SET_VERSION)
int X509_CRL_set_version(X509_CRL *x, long version);
#endif

#if !defined(HAVE_X509_CRL_SET_ISSUER_NAME)
int X509_CRL_set_issuer_name(X509_CRL *x, X509_NAME *name);
#endif

#if !defined(HAVE_X509_CRL_SORT)
int X509_CRL_sort(X509_CRL *c);
#endif

#if !defined(HAVE_X509_CRL_ADD0_REVOKED)
int X509_CRL_add0_revoked(X509_CRL *crl, X509_REVOKED *rev);
#endif

#if !defined(HAVE_BN_MOD_SQR)
int BN_mod_sqr(BIGNUM *r, const BIGNUM *a, const BIGNUM *m, BN_CTX *ctx);
#endif

#if !defined(HAVE_BN_MOD_ADD)
int BN_mod_add(BIGNUM *r, const BIGNUM *a, const BIGNUM *b, const BIGNUM *m, BN_CTX *ctx);
#endif

#if !defined(HAVE_BN_MOD_SUB)
int BN_mod_sub(BIGNUM *r, const BIGNUM *a, const BIGNUM *b, const BIGNUM *m, BN_CTX *ctx);
#endif

#if !defined(HAVE_BN_RAND_RANGE)
int BN_rand_range(BIGNUM *r, BIGNUM *range);
#endif

#if !defined(HAVE_BN_PSEUDO_RAND_RANGE)
int BN_pseudo_rand_range(BIGNUM *r, BIGNUM *range);
#endif

#if !defined(HAVE_CONF_GET1_DEFAULT_CONFIG_FILE)
char *CONF_get1_default_config_file(void);
#endif

#if !defined(HAVE_PEM_DEF_CALLBACK)
int PEM_def_callback(char *buf, int num, int w, void *key);
#endif

#if !defined(HAVE_ASN1_PUT_EOC)
int ASN1_put_eoc(unsigned char **pp);
#endif

#if defined(__cplusplus)
}
#endif


#endif /* _OSSL_OPENSSL_MISSING_H_ */

