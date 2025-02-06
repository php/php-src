/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
 */

#include "php_openssl_backend.h"

#if PHP_OPENSSL_API_VERSION < 0x30000

#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/dsa.h>
#include <openssl/dh.h>

#if defined(LIBRESSL_VERSION_NUMBER) && !defined(OPENSSL_NO_ENGINE)
#include <openssl/engine.h>
#endif

/* OpenSSL compatibility functions and macros */
#if PHP_OPENSSL_API_VERSION < 0x10100

#define EVP_PKEY_get0_RSA(_pkey) _pkey->pkey.rsa
#define EVP_PKEY_get0_DH(_pkey) _pkey->pkey.dh
#define EVP_PKEY_get0_DSA(_pkey) _pkey->pkey.dsa
#define EVP_PKEY_get0_EC_KEY(_pkey) _pkey->pkey.ec

static int RSA_set0_key(RSA *r, BIGNUM *n, BIGNUM *e, BIGNUM *d)
{
	r->n = n;
	r->e = e;
	r->d = d;

	return 1;
}

static int RSA_set0_factors(RSA *r, BIGNUM *p, BIGNUM *q)
{
	r->p = p;
	r->q = q;

	return 1;
}

static int RSA_set0_crt_params(RSA *r, BIGNUM *dmp1, BIGNUM *dmq1, BIGNUM *iqmp)
{
	r->dmp1 = dmp1;
	r->dmq1 = dmq1;
	r->iqmp = iqmp;

	return 1;
}

static void RSA_get0_key(const RSA *r, const BIGNUM **n, const BIGNUM **e, const BIGNUM **d)
{
	*n = r->n;
	*e = r->e;
	*d = r->d;
}

static void RSA_get0_factors(const RSA *r, const BIGNUM **p, const BIGNUM **q)
{
	*p = r->p;
	*q = r->q;
}

static void RSA_get0_crt_params(const RSA *r, const BIGNUM **dmp1, const BIGNUM **dmq1, const BIGNUM **iqmp)
{
	*dmp1 = r->dmp1;
	*dmq1 = r->dmq1;
	*iqmp = r->iqmp;
}

static void DH_get0_pqg(const DH *dh, const BIGNUM **p, const BIGNUM **q, const BIGNUM **g)
{
	*p = dh->p;
	*q = dh->q;
	*g = dh->g;
}

static int DH_set0_pqg(DH *dh, BIGNUM *p, BIGNUM *q, BIGNUM *g)
{
	dh->p = p;
	dh->q = q;
	dh->g = g;

	return 1;
}

static void DH_get0_key(const DH *dh, const BIGNUM **pub_key, const BIGNUM **priv_key)
{
	*pub_key = dh->pub_key;
	*priv_key = dh->priv_key;
}

static int DH_set0_key(DH *dh, BIGNUM *pub_key, BIGNUM *priv_key)
{
	dh->pub_key = pub_key;
	dh->priv_key = priv_key;

	return 1;
}

static void DSA_get0_pqg(const DSA *d, const BIGNUM **p, const BIGNUM **q, const BIGNUM **g)
{
	*p = d->p;
	*q = d->q;
	*g = d->g;
}

int DSA_set0_pqg(DSA *d, BIGNUM *p, BIGNUM *q, BIGNUM *g)
{
	d->p = p;
	d->q = q;
	d->g = g;

	return 1;
}

static void DSA_get0_key(const DSA *d, const BIGNUM **pub_key, const BIGNUM **priv_key)
{
	*pub_key = d->pub_key;
	*priv_key = d->priv_key;
}

int DSA_set0_key(DSA *d, BIGNUM *pub_key, BIGNUM *priv_key)
{
	d->pub_key = pub_key;
	d->priv_key = priv_key;

	return 1;
}

static const unsigned char *ASN1_STRING_get0_data(const ASN1_STRING *asn1)
{
	return M_ASN1_STRING_data(asn1);
}

static int EVP_PKEY_up_ref(EVP_PKEY *pkey)
{
	return CRYPTO_add(&pkey->references, 1, CRYPTO_LOCK_EVP_PKEY);
}

#if PHP_OPENSSL_API_VERSION < 0x10002

static int X509_get_signature_nid(const X509 *x)
{
	return OBJ_obj2nid(x->sig_alg->algorithm);
}

#endif

#define OpenSSL_version		SSLeay_version
#define OPENSSL_VERSION		SSLEAY_VERSION
#define X509_getm_notBefore	X509_get_notBefore
#define X509_getm_notAfter	X509_get_notAfter
#define EVP_CIPHER_CTX_reset	EVP_CIPHER_CTX_cleanup

#endif

void php_openssl_backend_shutdown(void)
{
	#ifdef LIBRESSL_VERSION_NUMBER
	EVP_cleanup();

	/* prevent accessing locking callback from unloaded extension */
	CRYPTO_set_locking_callback(NULL);

#ifndef OPENSSL_NO_ENGINE
	/* Free engine list initialized by OPENSSL_config */
	ENGINE_cleanup();
#endif

	/* free allocated error strings */
	ERR_free_strings();
	CONF_modules_free();
#endif
}

static bool php_openssl_pkey_init_rsa_data(RSA *rsa, zval *data)
{
	BIGNUM *n, *e, *d, *p, *q, *dmp1, *dmq1, *iqmp;

	OPENSSL_PKEY_SET_BN(data, n);
	OPENSSL_PKEY_SET_BN(data, e);
	OPENSSL_PKEY_SET_BN(data, d);
	if (!n || !d || !RSA_set0_key(rsa, n, e, d)) {
		return 0;
	}

	OPENSSL_PKEY_SET_BN(data, p);
	OPENSSL_PKEY_SET_BN(data, q);
	if ((p || q) && !RSA_set0_factors(rsa, p, q)) {
		return 0;
	}

	OPENSSL_PKEY_SET_BN(data, dmp1);
	OPENSSL_PKEY_SET_BN(data, dmq1);
	OPENSSL_PKEY_SET_BN(data, iqmp);
	if ((dmp1 || dmq1 || iqmp) && !RSA_set0_crt_params(rsa, dmp1, dmq1, iqmp)) {
		return 0;
	}

	return 1;
}

EVP_PKEY *php_openssl_pkey_init_rsa(zval *data)
{
	EVP_PKEY *pkey = EVP_PKEY_new();
	if (!pkey) {
		php_openssl_store_errors();
		return NULL;
	}

	RSA *rsa = RSA_new();
	if (!rsa) {
		php_openssl_store_errors();
		EVP_PKEY_free(pkey);
		return NULL;
	}

	if (!php_openssl_pkey_init_rsa_data(rsa, data) || !EVP_PKEY_assign_RSA(pkey, rsa)) {
		php_openssl_store_errors();
		EVP_PKEY_free(pkey);
		RSA_free(rsa);
		return NULL;
	}

	return pkey;
}

static bool php_openssl_pkey_init_dsa_data(DSA *dsa, zval *data, bool *is_private)
{
	BIGNUM *p, *q, *g, *priv_key, *pub_key;
	const BIGNUM *priv_key_const, *pub_key_const;

	OPENSSL_PKEY_SET_BN(data, p);
	OPENSSL_PKEY_SET_BN(data, q);
	OPENSSL_PKEY_SET_BN(data, g);
	if (!p || !q || !g || !DSA_set0_pqg(dsa, p, q, g)) {
		return 0;
	}

	OPENSSL_PKEY_SET_BN(data, pub_key);
	OPENSSL_PKEY_SET_BN(data, priv_key);
	*is_private = priv_key != NULL;
	if (pub_key) {
		return DSA_set0_key(dsa, pub_key, priv_key);
	}

	/* generate key */
	PHP_OPENSSL_RAND_ADD_TIME();
	if (!DSA_generate_key(dsa)) {
		php_openssl_store_errors();
		return 0;
	}

	/* if BN_mod_exp return -1, then DSA_generate_key succeed for failed key
	 * so we need to double check that public key is created */
	DSA_get0_key(dsa, &pub_key_const, &priv_key_const);
	if (!pub_key_const || BN_is_zero(pub_key_const)) {
		return 0;
	}
	/* all good */
	*is_private = true;
	return 1;
}

EVP_PKEY *php_openssl_pkey_init_dsa(zval *data, bool *is_private)
{
	EVP_PKEY *pkey = EVP_PKEY_new();
	if (!pkey) {
		php_openssl_store_errors();
		return NULL;
	}

	DSA *dsa = DSA_new();
	if (!dsa) {
		php_openssl_store_errors();
		EVP_PKEY_free(pkey);
		return NULL;
	}

	if (!php_openssl_pkey_init_dsa_data(dsa, data, is_private)
			|| !EVP_PKEY_assign_DSA(pkey, dsa)) {
		php_openssl_store_errors();
		EVP_PKEY_free(pkey);
		DSA_free(dsa);
		return NULL;
	}

	return pkey;
}

static bool php_openssl_pkey_init_dh_data(DH *dh, zval *data, bool *is_private)
{
	BIGNUM *p, *q, *g, *priv_key, *pub_key;

	OPENSSL_PKEY_SET_BN(data, p);
	OPENSSL_PKEY_SET_BN(data, q);
	OPENSSL_PKEY_SET_BN(data, g);
	if (!p || !g || !DH_set0_pqg(dh, p, q, g)) {
		return 0;
	}

	OPENSSL_PKEY_SET_BN(data, priv_key);
	OPENSSL_PKEY_SET_BN(data, pub_key);
	*is_private = priv_key != NULL;
	if (pub_key) {
		return DH_set0_key(dh, pub_key, priv_key);
	}
	if (priv_key) {
		pub_key = php_openssl_dh_pub_from_priv(priv_key, g, p);
		if (pub_key == NULL) {
			return 0;
		}
		return DH_set0_key(dh, pub_key, priv_key);
	}

	/* generate key */
	PHP_OPENSSL_RAND_ADD_TIME();
	if (!DH_generate_key(dh)) {
		php_openssl_store_errors();
		return 0;
	}
	/* all good */
	*is_private = true;
	return 1;
}

EVP_PKEY *php_openssl_pkey_init_dh(zval *data, bool *is_private)
{
	EVP_PKEY *pkey = EVP_PKEY_new();
	if (!pkey) {
		php_openssl_store_errors();
		return NULL;
	}

	DH *dh = DH_new();
	if (!dh) {
		EVP_PKEY_free(pkey);
		return NULL;
	}

	if (!php_openssl_pkey_init_dh_data(dh, data, is_private) || !EVP_PKEY_assign_DH(pkey, dh)) {
		php_openssl_store_errors();
		EVP_PKEY_free(pkey);
		DH_free(dh);
		return NULL;
	}

	return pkey;
}

#ifdef HAVE_EVP_PKEY_EC
static bool php_openssl_pkey_init_ec_data(EC_KEY *eckey, zval *data, bool *is_private) {
	BIGNUM *p = NULL, *a = NULL, *b = NULL, *order = NULL, *g_x = NULL, *g_y = NULL , *cofactor = NULL;
	BIGNUM *x = NULL, *y = NULL, *d = NULL;
	EC_POINT *point_g = NULL;
	EC_POINT *point_q = NULL;
	EC_GROUP *group = NULL;
	BN_CTX *bctx = BN_CTX_new();

	*is_private = false;

	zval *curve_name_zv = zend_hash_str_find(Z_ARRVAL_P(data), "curve_name", sizeof("curve_name") - 1);
	if (curve_name_zv && Z_TYPE_P(curve_name_zv) == IS_STRING && Z_STRLEN_P(curve_name_zv) > 0) {
		int nid = OBJ_sn2nid(Z_STRVAL_P(curve_name_zv));
		if (nid == NID_undef) {
			php_error_docref(NULL, E_WARNING, "Unknown elliptic curve (short) name %s", Z_STRVAL_P(curve_name_zv));
			goto clean_exit;
		}

		if (!(group = EC_GROUP_new_by_curve_name(nid))) {
			goto clean_exit;
		}
		EC_GROUP_set_asn1_flag(group, OPENSSL_EC_NAMED_CURVE);
	} else {
		OPENSSL_PKEY_SET_BN(data, p);
		OPENSSL_PKEY_SET_BN(data, a);
		OPENSSL_PKEY_SET_BN(data, b);
		OPENSSL_PKEY_SET_BN(data, order);

		if (!(p && a && b && order)) {
			if (!p && !a && !b && !order) {
				php_error_docref(NULL, E_WARNING, "Missing params: curve_name");
			} else {
				php_error_docref(
					NULL, E_WARNING, "Missing params: curve_name or p, a, b, order");
			}
			goto clean_exit;
		}

		if (!(group = EC_GROUP_new_curve_GFp(p, a, b, bctx))) {
			goto clean_exit;
		}

		if (!(point_g = EC_POINT_new(group))) {
			goto clean_exit;
		}

		zval *generator_zv = zend_hash_str_find(Z_ARRVAL_P(data), "generator", sizeof("generator") - 1);
		if (generator_zv && Z_TYPE_P(generator_zv) == IS_STRING && Z_STRLEN_P(generator_zv) > 0) {
			if (!(EC_POINT_oct2point(group, point_g, (unsigned char *)Z_STRVAL_P(generator_zv), Z_STRLEN_P(generator_zv), bctx))) {
				goto clean_exit;
			}
		} else {
			OPENSSL_PKEY_SET_BN(data, g_x);
			OPENSSL_PKEY_SET_BN(data, g_y);

			if (!g_x || !g_y) {
				php_error_docref(
					NULL, E_WARNING, "Missing params: generator or g_x and g_y");
				goto clean_exit;
			}

			if (!EC_POINT_set_affine_coordinates_GFp(group, point_g, g_x, g_y, bctx)) {
				goto clean_exit;
			}
		}

		zval *seed_zv = zend_hash_str_find(Z_ARRVAL_P(data), "seed", sizeof("seed") - 1);
		if (seed_zv && Z_TYPE_P(seed_zv) == IS_STRING && Z_STRLEN_P(seed_zv) > 0) {
			if (!EC_GROUP_set_seed(group, (unsigned char *)Z_STRVAL_P(seed_zv), Z_STRLEN_P(seed_zv))) {
				goto clean_exit;
			}
		}

		/*
		 * OpenSSL uses 0 cofactor as a marker for "unknown cofactor".
		 * So accept cofactor == NULL or cofactor >= 0.
		 * Internally, the lib will check the cofactor value.
		 */
		OPENSSL_PKEY_SET_BN(data, cofactor);
		if (!EC_GROUP_set_generator(group, point_g, order, cofactor)) {
			goto clean_exit;
		}
		EC_GROUP_set_asn1_flag(group, OPENSSL_EC_EXPLICIT_CURVE);
	}

	EC_GROUP_set_point_conversion_form(group, POINT_CONVERSION_UNCOMPRESSED);

	if (!EC_KEY_set_group(eckey, group)) {
		goto clean_exit;
	}

	OPENSSL_PKEY_SET_BN(data, d);
	OPENSSL_PKEY_SET_BN(data, x);
	OPENSSL_PKEY_SET_BN(data, y);

	if (d) {
		*is_private = true;
		if (!EC_KEY_set_private_key(eckey, d)) {
			goto clean_exit;
		}

		point_q = EC_POINT_new(group);
		if (!point_q || !EC_POINT_mul(group, point_q, d, NULL, NULL, bctx)) {
			goto clean_exit;
		}
	} else if (x && y) {
		/* OpenSSL does not allow setting EC_PUB_X/EC_PUB_Y, so convert to encoded format. */
		point_q = EC_POINT_new(group);
		if (!point_q || !EC_POINT_set_affine_coordinates_GFp(group, point_q, x, y, bctx)) {
			goto clean_exit;
		}
	}

	if (point_q != NULL) {
		if (!EC_KEY_set_public_key(eckey, point_q)) {
			goto clean_exit;
		}
	}

	if (!EC_KEY_check_key(eckey)) {
		*is_private = true;
		PHP_OPENSSL_RAND_ADD_TIME();
		EC_KEY_generate_key(eckey);
	}

clean_exit:
	php_openssl_store_errors();
	BN_CTX_free(bctx);
	EC_GROUP_free(group);
	EC_POINT_free(point_g);
	EC_POINT_free(point_q);
	BN_free(p);
	BN_free(a);
	BN_free(b);
	BN_free(order);
	BN_free(g_x);
	BN_free(g_y);
	BN_free(cofactor);
	BN_free(d);
	BN_free(x);
	BN_free(y);
	return EC_KEY_check_key(eckey);
}

EVP_PKEY *php_openssl_pkey_init_ec(zval *data, bool *is_private) {
	EVP_PKEY *pkey = EVP_PKEY_new();
	if (!pkey) {
		php_openssl_store_errors();
		return NULL;
	}

	EC_KEY *ec = EC_KEY_new();
	if (!ec) {
		EVP_PKEY_free(pkey);
		return NULL;
	}

	if (!php_openssl_pkey_init_ec_data(ec, data, is_private) || !EVP_PKEY_assign_EC_KEY(pkey, ec)) {
		php_openssl_store_errors();
		EVP_PKEY_free(pkey);
		EC_KEY_free(ec);
		return NULL;
	}

	return pkey;
}

#endif

zend_long php_openssl_pkey_get_details(zval *return_value, EVP_PKEY *pkey)
{
	zend_long ktype;
	switch (EVP_PKEY_base_id(pkey)) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			{
				RSA *rsa = EVP_PKEY_get0_RSA(pkey);
				ktype = OPENSSL_KEYTYPE_RSA;

				if (rsa != NULL) {
					zval z_rsa;
					const BIGNUM *n, *e, *d, *p, *q, *dmp1, *dmq1, *iqmp;

					RSA_get0_key(rsa, &n, &e, &d);
					RSA_get0_factors(rsa, &p, &q);
					RSA_get0_crt_params(rsa, &dmp1, &dmq1, &iqmp);

					array_init(&z_rsa);
					OPENSSL_PKEY_GET_BN(z_rsa, n);
					OPENSSL_PKEY_GET_BN(z_rsa, e);
					OPENSSL_PKEY_GET_BN(z_rsa, d);
					OPENSSL_PKEY_GET_BN(z_rsa, p);
					OPENSSL_PKEY_GET_BN(z_rsa, q);
					OPENSSL_PKEY_GET_BN(z_rsa, dmp1);
					OPENSSL_PKEY_GET_BN(z_rsa, dmq1);
					OPENSSL_PKEY_GET_BN(z_rsa, iqmp);
					add_assoc_zval(return_value, "rsa", &z_rsa);
				}
			}
			break;
		case EVP_PKEY_DSA:
		case EVP_PKEY_DSA2:
		case EVP_PKEY_DSA3:
		case EVP_PKEY_DSA4:
			{
				DSA *dsa = EVP_PKEY_get0_DSA(pkey);
				ktype = OPENSSL_KEYTYPE_DSA;

				if (dsa != NULL) {
					zval z_dsa;
					const BIGNUM *p, *q, *g, *priv_key, *pub_key;

					DSA_get0_pqg(dsa, &p, &q, &g);
					DSA_get0_key(dsa, &pub_key, &priv_key);

					array_init(&z_dsa);
					OPENSSL_PKEY_GET_BN(z_dsa, p);
					OPENSSL_PKEY_GET_BN(z_dsa, q);
					OPENSSL_PKEY_GET_BN(z_dsa, g);
					OPENSSL_PKEY_GET_BN(z_dsa, priv_key);
					OPENSSL_PKEY_GET_BN(z_dsa, pub_key);
					add_assoc_zval(return_value, "dsa", &z_dsa);
				}
			}
			break;
		case EVP_PKEY_DH:
			{
				DH *dh = EVP_PKEY_get0_DH(pkey);
				ktype = OPENSSL_KEYTYPE_DH;

				if (dh != NULL) {
					zval z_dh;
					const BIGNUM *p, *q, *g, *priv_key, *pub_key;

					DH_get0_pqg(dh, &p, &q, &g);
					DH_get0_key(dh, &pub_key, &priv_key);

					array_init(&z_dh);
					OPENSSL_PKEY_GET_BN(z_dh, p);
					OPENSSL_PKEY_GET_BN(z_dh, g);
					OPENSSL_PKEY_GET_BN(z_dh, priv_key);
					OPENSSL_PKEY_GET_BN(z_dh, pub_key);
					add_assoc_zval(return_value, "dh", &z_dh);
				}
			}
			break;
#ifdef HAVE_EVP_PKEY_EC
		case EVP_PKEY_EC:
			ktype = OPENSSL_KEYTYPE_EC;
			if (EVP_PKEY_get0_EC_KEY(pkey) != NULL) {
				zval ec;
				const EC_GROUP *ec_group;
				const EC_POINT *pub;
				int nid;
				char *crv_sn;
				ASN1_OBJECT *obj;
				// openssl recommends a buffer length of 80
				char oir_buf[80];
				const EC_KEY *ec_key = EVP_PKEY_get0_EC_KEY(pkey);
				BIGNUM *x = BN_new();
				BIGNUM *y = BN_new();
				const BIGNUM *d;

				ec_group = EC_KEY_get0_group(ec_key);

				array_init(&ec);

				/** Curve nid (numerical identifier) used for ASN1 mapping */
				nid = EC_GROUP_get_curve_name(ec_group);
				if (nid != NID_undef) {
					crv_sn = (char*) OBJ_nid2sn(nid);
					if (crv_sn != NULL) {
						add_assoc_string(&ec, "curve_name", crv_sn);
					}

					obj = OBJ_nid2obj(nid);
					if (obj != NULL) {
						int oir_len = OBJ_obj2txt(oir_buf, sizeof(oir_buf), obj, 1);
						add_assoc_stringl(&ec, "curve_oid", (char*) oir_buf, oir_len);
						ASN1_OBJECT_free(obj);
					}
				}

				pub = EC_KEY_get0_public_key(ec_key);

				if (EC_POINT_get_affine_coordinates_GFp(ec_group, pub, x, y, NULL)) {
					php_openssl_add_bn_to_array(&ec, x, "x");
					php_openssl_add_bn_to_array(&ec, y, "y");
				} else {
					php_openssl_store_errors();
				}

				if ((d = EC_KEY_get0_private_key(EVP_PKEY_get0_EC_KEY(pkey))) != NULL) {
					php_openssl_add_bn_to_array(&ec, d, "d");
				}

				add_assoc_zval(return_value, "ec", &ec);

				BN_free(x);
				BN_free(y);
			}
			break;
#endif
		default:
			ktype = -1;
			break;
	}

	return ktype;
}

zend_string *php_openssl_dh_compute_key(EVP_PKEY *pkey, char *pub_str, size_t pub_len)
{
	DH *dh = EVP_PKEY_get0_DH(pkey);
	if (dh == NULL) {
		return NULL;
	}

	BIGNUM *pub = BN_bin2bn((unsigned char*)pub_str, (int)pub_len, NULL);
	zend_string *data = zend_string_alloc(DH_size(dh), 0);
	int len = DH_compute_key((unsigned char*)ZSTR_VAL(data), pub, dh);
	BN_free(pub);

	if (len < 0) {
		php_openssl_store_errors();
		zend_string_release_ex(data, 0);
		return NULL;
	}

	ZSTR_LEN(data) = len;
	ZSTR_VAL(data)[len] = 0;
	return data;
}

void php_openssl_get_cipher_methods(zval *return_value, bool aliases)
{
	array_init(return_value);
	OBJ_NAME_do_all_sorted(OBJ_NAME_TYPE_CIPHER_METH,
		aliases ? php_openssl_add_method_or_alias : php_openssl_add_method,
		return_value);
}

#endif
