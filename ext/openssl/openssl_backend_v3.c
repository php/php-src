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

#if PHP_OPENSSL_API_VERSION >= 0x30000

#include <openssl/core_names.h>
#include <openssl/param_build.h>
#include <openssl/provider.h>

ZEND_EXTERN_MODULE_GLOBALS(openssl)

void php_openssl_backend_shutdown(void)
{
	(void) 0;
}

void php_openssl_backend_init_libctx(OSSL_LIB_CTX **plibctx, char **ppropq)
{
	/* The return value is not checked because we cannot reasonable fail in GINIT so using NULL 
	 * (default context) is probably better. */
	*plibctx = OSSL_LIB_CTX_new();
	*ppropq = NULL;
}

void php_openssl_backend_destroy_libctx(OSSL_LIB_CTX *libctx, char *propq)
{
	if (libctx != NULL) {
		OSSL_LIB_CTX_free(libctx);
	}
	if (propq != NULL) {
		free(propq);
	}
}

EVP_PKEY_CTX *php_openssl_pkey_new_from_name(const char *name, int id)
{
	return EVP_PKEY_CTX_new_from_name(OPENSSL_G(libctx), name, OPENSSL_G(propq));
}

EVP_PKEY_CTX *php_openssl_pkey_new_from_pkey(EVP_PKEY *pkey)
{
	return  EVP_PKEY_CTX_new_from_pkey(OPENSSL_G(libctx), pkey, OPENSSL_G(propq));
}

EVP_PKEY *php_openssl_pkey_init_rsa(zval *data)
{
	BIGNUM *n = NULL, *e = NULL, *d = NULL, *p = NULL, *q = NULL;
	BIGNUM *dmp1 = NULL, *dmq1 = NULL, *iqmp = NULL;
	EVP_PKEY *pkey = NULL;
	EVP_PKEY_CTX *ctx = php_openssl_pkey_new_from_name("RSA", EVP_PKEY_RSA);
	OSSL_PARAM *params = NULL;
	OSSL_PARAM_BLD *bld = OSSL_PARAM_BLD_new();

	OPENSSL_PKEY_SET_BN(data, n);
	OPENSSL_PKEY_SET_BN(data, e);
	OPENSSL_PKEY_SET_BN(data, d);
	OPENSSL_PKEY_SET_BN(data, p);
	OPENSSL_PKEY_SET_BN(data, q);
	OPENSSL_PKEY_SET_BN(data, dmp1);
	OPENSSL_PKEY_SET_BN(data, dmq1);
	OPENSSL_PKEY_SET_BN(data, iqmp);

	if (!ctx || !bld || !n || !d) {
		goto cleanup;
	}

	OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_N, n);
	OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_D, d);
	if (e) {
		OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_E, e);
	}
	if (p) {
		OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_FACTOR1, p);
	}
	if (q) {
		OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_FACTOR2, q);
	}
	if (dmp1) {
		OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_EXPONENT1, dmp1);
	}
	if (dmq1) {
		OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_EXPONENT2, dmq1);
	}
	if (iqmp) {
		OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_COEFFICIENT1, iqmp);
	}

	params = OSSL_PARAM_BLD_to_param(bld);
	if (!params) {
		goto cleanup;
	}

	if (EVP_PKEY_fromdata_init(ctx) <= 0 ||
			EVP_PKEY_fromdata(ctx, &pkey, EVP_PKEY_KEYPAIR, params) <= 0) {
		goto cleanup;
	}

cleanup:
	php_openssl_store_errors();
	EVP_PKEY_CTX_free(ctx);
	OSSL_PARAM_free(params);
	OSSL_PARAM_BLD_free(bld);
	BN_free(n);
	BN_free(e);
	BN_free(d);
	BN_free(p);
	BN_free(q);
	BN_free(dmp1);
	BN_free(dmq1);
	BN_free(iqmp);
	return pkey;
}

EVP_PKEY *php_openssl_pkey_init_dsa(zval *data, bool *is_private)
{
	BIGNUM *p = NULL, *q = NULL, *g = NULL, *priv_key = NULL, *pub_key = NULL;
	EVP_PKEY *param_key = NULL, *pkey = NULL;
	EVP_PKEY_CTX *ctx = php_openssl_pkey_new_from_name("DSA", EVP_PKEY_DSA);
	OSSL_PARAM *params = NULL;
	OSSL_PARAM_BLD *bld = OSSL_PARAM_BLD_new();

	OPENSSL_PKEY_SET_BN(data, p);
	OPENSSL_PKEY_SET_BN(data, q);
	OPENSSL_PKEY_SET_BN(data, g);
	OPENSSL_PKEY_SET_BN(data, priv_key);
	OPENSSL_PKEY_SET_BN(data, pub_key);

	*is_private = false;

	if (!ctx || !bld || !p || !q || !g) {
		goto cleanup;
	}

	OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_FFC_P, p);
	OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_FFC_Q, q);
	OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_FFC_G, g);
	// TODO: We silently ignore priv_key if pub_key is not given, unlike in the DH case.
	if (pub_key) {
		OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_PUB_KEY, pub_key);
		if (priv_key) {
			OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_PRIV_KEY, priv_key);
		}
	}

	params = OSSL_PARAM_BLD_to_param(bld);
	if (!params) {
		goto cleanup;
	}

	if (EVP_PKEY_fromdata_init(ctx) <= 0 ||
			EVP_PKEY_fromdata(ctx, &param_key, EVP_PKEY_KEYPAIR, params) <= 0) {
		goto cleanup;
	}

	if (pub_key) {
		*is_private = priv_key != NULL;
		EVP_PKEY_up_ref(param_key);
		pkey = param_key;
	} else {
		*is_private = true;
		EVP_PKEY_CTX_free(ctx);
		ctx = php_openssl_pkey_new_from_pkey(param_key);
		if (EVP_PKEY_keygen_init(ctx) <= 0 || EVP_PKEY_keygen(ctx, &pkey) <= 0) {
			goto cleanup;
		}
	}

cleanup:
	php_openssl_store_errors();
	EVP_PKEY_free(param_key);
	EVP_PKEY_CTX_free(ctx);
	OSSL_PARAM_free(params);
	OSSL_PARAM_BLD_free(bld);
	BN_free(p);
	BN_free(q);
	BN_free(g);
	BN_free(priv_key);
	BN_free(pub_key);
	return pkey;
}

EVP_PKEY *php_openssl_pkey_init_dh(zval *data, bool *is_private)
{
	BIGNUM *p = NULL, *q = NULL, *g = NULL, *priv_key = NULL, *pub_key = NULL;
	EVP_PKEY *param_key = NULL, *pkey = NULL;
	EVP_PKEY_CTX *ctx = php_openssl_pkey_new_from_name("DH", EVP_PKEY_DH);
	OSSL_PARAM *params = NULL;
	OSSL_PARAM_BLD *bld = OSSL_PARAM_BLD_new();

	OPENSSL_PKEY_SET_BN(data, p);
	OPENSSL_PKEY_SET_BN(data, q);
	OPENSSL_PKEY_SET_BN(data, g);
	OPENSSL_PKEY_SET_BN(data, priv_key);
	OPENSSL_PKEY_SET_BN(data, pub_key);

	*is_private = false;

	if (!ctx || !bld || !p || !g) {
		goto cleanup;
	}

	OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_FFC_P, p);
	OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_FFC_G, g);
	if (q) {
		OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_FFC_Q, q);
	}
	if (priv_key) {
		OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_PRIV_KEY, priv_key);
		if (!pub_key) {
			pub_key = php_openssl_dh_pub_from_priv(priv_key, g, p);
			if (!pub_key) {
				goto cleanup;
			}
		}
	}
	if (pub_key) {
		OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_PUB_KEY, pub_key);
	}

	params = OSSL_PARAM_BLD_to_param(bld);
	if (!params) {
		goto cleanup;
	}

	if (EVP_PKEY_fromdata_init(ctx) <= 0 ||
			EVP_PKEY_fromdata(ctx, &param_key, EVP_PKEY_KEYPAIR, params) <= 0) {
		goto cleanup;
	}

	if (pub_key || priv_key) {
		*is_private = priv_key != NULL;
		EVP_PKEY_up_ref(param_key);
		pkey = param_key;
	} else {
		*is_private = true;
		EVP_PKEY_CTX_free(ctx);
		ctx = php_openssl_pkey_new_from_pkey(param_key);
		if (EVP_PKEY_keygen_init(ctx) <= 0 || EVP_PKEY_keygen(ctx, &pkey) <= 0) {
			goto cleanup;
		}
	}

cleanup:
	php_openssl_store_errors();
	EVP_PKEY_free(param_key);
	EVP_PKEY_CTX_free(ctx);
	OSSL_PARAM_free(params);
	OSSL_PARAM_BLD_free(bld);
	BN_free(p);
	BN_free(q);
	BN_free(g);
	BN_free(priv_key);
	BN_free(pub_key);
	return pkey;
}

#ifdef HAVE_EVP_PKEY_EC
EVP_PKEY *php_openssl_pkey_init_ec(zval *data, bool *is_private) {
	int nid = NID_undef;
	BIGNUM *p = NULL, *a = NULL, *b = NULL, *order = NULL, *g_x = NULL, *g_y = NULL, *cofactor = NULL;
	BIGNUM *x = NULL, *y = NULL, *d = NULL;
	EC_POINT *point_g = NULL;
	EC_POINT *point_q = NULL;
	unsigned char *point_g_buf = NULL;
	unsigned char *point_q_buf = NULL;
	EC_GROUP *group = NULL;
	EVP_PKEY *param_key = NULL, *pkey = NULL;
	EVP_PKEY_CTX *ctx = php_openssl_pkey_new_from_name("EC", EVP_PKEY_EC);
	BN_CTX *bctx = BN_CTX_new();
	OSSL_PARAM *params = NULL;
	OSSL_PARAM_BLD *bld = OSSL_PARAM_BLD_new();

	*is_private = false;

	if (!ctx || !bld || !bctx) {
		goto cleanup;
	}

	zval *curve_name_zv = zend_hash_str_find(Z_ARRVAL_P(data), "curve_name", sizeof("curve_name") - 1);
	if (curve_name_zv && Z_TYPE_P(curve_name_zv) == IS_STRING && Z_STRLEN_P(curve_name_zv) > 0) {
		nid = OBJ_sn2nid(Z_STRVAL_P(curve_name_zv));
		if (nid == NID_undef) {
			php_error_docref(NULL, E_WARNING, "Unknown elliptic curve (short) name %s", Z_STRVAL_P(curve_name_zv));
			goto cleanup;
		}

		if (!(group = EC_GROUP_new_by_curve_name_ex(OPENSSL_G(libctx), OPENSSL_G(propq), nid))) {
			goto cleanup;
		}

		if (!OSSL_PARAM_BLD_push_utf8_string(bld, OSSL_PKEY_PARAM_GROUP_NAME, Z_STRVAL_P(curve_name_zv), Z_STRLEN_P(curve_name_zv))) {
			goto cleanup;
		}
	} else {
		OPENSSL_PKEY_SET_BN(data, p);
		OPENSSL_PKEY_SET_BN(data, a);
		OPENSSL_PKEY_SET_BN(data, b);
		OPENSSL_PKEY_SET_BN(data, order);

		if (!(p && a && b && order)) {
			if (!p && !a && !b && !order) {
				php_error_docref(NULL, E_WARNING, "Missing params: curve_name");
			} else {
				php_error_docref(NULL, E_WARNING, "Missing params: curve_name or p, a, b, order");
			}
			goto cleanup;
		}

		if (!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_EC_P, p) ||
			!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_EC_A, a) ||
			!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_EC_B, b) ||
			!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_EC_ORDER, order) ||
			!OSSL_PARAM_BLD_push_utf8_string(bld, OSSL_PKEY_PARAM_EC_FIELD_TYPE, SN_X9_62_prime_field, 0)) {
				goto cleanup;
			}

		if (!(group = EC_GROUP_new_curve_GFp(p, a, b, bctx))) {
			goto cleanup;
		}

		if (!(point_g = EC_POINT_new(group))) {
			goto cleanup;
		}

		zval *generator_zv = zend_hash_str_find(Z_ARRVAL_P(data), "generator", sizeof("generator") - 1);
		if (generator_zv && Z_TYPE_P(generator_zv) == IS_STRING && Z_STRLEN_P(generator_zv) > 0) {
			if (!EC_POINT_oct2point(group, point_g, (unsigned char *)Z_STRVAL_P(generator_zv), Z_STRLEN_P(generator_zv), bctx) ||
				!OSSL_PARAM_BLD_push_octet_string(bld, OSSL_PKEY_PARAM_EC_GENERATOR, Z_STRVAL_P(generator_zv), Z_STRLEN_P(generator_zv))) {
				goto cleanup;
			}
		} else {
			OPENSSL_PKEY_SET_BN(data, g_x);
			OPENSSL_PKEY_SET_BN(data, g_y);

			if (!g_x || !g_y) {
				php_error_docref(
					NULL, E_WARNING, "Missing params: generator or g_x and g_y");
				goto cleanup;
			}

			if (!EC_POINT_set_affine_coordinates(group, point_g, g_x, g_y, bctx)) {
				goto cleanup;
			}

			size_t point_g_buf_len =
				EC_POINT_point2buf(group, point_g, POINT_CONVERSION_COMPRESSED, &point_g_buf, bctx);
			if (!point_g_buf_len) {
				goto cleanup;
			}

			if (!OSSL_PARAM_BLD_push_octet_string(bld, OSSL_PKEY_PARAM_EC_GENERATOR, point_g_buf, point_g_buf_len)) {
				goto cleanup;
			}
		}

		zval *seed_zv = zend_hash_str_find(Z_ARRVAL_P(data), "seed", sizeof("seed") - 1);
		if (seed_zv && Z_TYPE_P(seed_zv) == IS_STRING && Z_STRLEN_P(seed_zv) > 0) {
			if (!EC_GROUP_set_seed(group, (unsigned char *)Z_STRVAL_P(seed_zv), Z_STRLEN_P(seed_zv)) ||
				!OSSL_PARAM_BLD_push_octet_string(bld, OSSL_PKEY_PARAM_EC_SEED, Z_STRVAL_P(seed_zv), Z_STRLEN_P(seed_zv))) {
				goto cleanup;
			}
		}

		OPENSSL_PKEY_SET_BN(data, cofactor);
		if (!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_EC_COFACTOR, cofactor) ||
			!EC_GROUP_set_generator(group, point_g, order, cofactor)) {
			goto cleanup;
		}

		nid = EC_GROUP_check_named_curve(group, 0, bctx);
	}

	/* custom params not supported with SM2, SKIP */
	if (nid != NID_sm2) {
		OPENSSL_PKEY_SET_BN(data, d);
		OPENSSL_PKEY_SET_BN(data, x);
		OPENSSL_PKEY_SET_BN(data, y);

		if (d) {
			point_q = EC_POINT_new(group);
			if (!point_q || !EC_POINT_mul(group, point_q, d, NULL, NULL, bctx) ||
				!OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_PRIV_KEY, d)) {
				goto cleanup;
			}
		} else if (x && y) {
			/* OpenSSL does not allow setting EC_PUB_X/EC_PUB_Y, so convert to encoded format. */
			point_q = EC_POINT_new(group);
			if (!point_q || !EC_POINT_set_affine_coordinates(group, point_q, x, y, bctx)) {
				goto cleanup;
			}
		}

		if (point_q) {
			size_t point_q_buf_len =
				EC_POINT_point2buf(group, point_q, POINT_CONVERSION_COMPRESSED, &point_q_buf, bctx);
			if (!point_q_buf_len ||
				!OSSL_PARAM_BLD_push_octet_string(bld, OSSL_PKEY_PARAM_PUB_KEY, point_q_buf, point_q_buf_len)) {
				goto cleanup;
			}
		}
	}

	params = OSSL_PARAM_BLD_to_param(bld);
	if (!params) {
		goto cleanup;
	}

	if (d || (x && y)) {
		if (EVP_PKEY_fromdata_init(ctx) <= 0 ||
			EVP_PKEY_fromdata(ctx, &param_key, EVP_PKEY_KEYPAIR, params) <= 0) {
			goto cleanup;
		}
		EVP_PKEY_CTX_free(ctx);
		ctx = EVP_PKEY_CTX_new(param_key, NULL);
	}

	if (EVP_PKEY_check(ctx) || EVP_PKEY_public_check_quick(ctx)) {
		*is_private = d != NULL;
		EVP_PKEY_up_ref(param_key);
		pkey = param_key;
	} else {
		*is_private = true;
		if (EVP_PKEY_keygen_init(ctx) != 1 ||
				EVP_PKEY_CTX_set_params(ctx, params) != 1 ||
				EVP_PKEY_generate(ctx, &pkey) != 1) {
			goto cleanup;
		}
	}

cleanup:
	php_openssl_store_errors();
	EVP_PKEY_free(param_key);
	EVP_PKEY_CTX_free(ctx);
	BN_CTX_free(bctx);
	OSSL_PARAM_free(params);
	OSSL_PARAM_BLD_free(bld);
	EC_GROUP_free(group);
	EC_POINT_free(point_g);
	EC_POINT_free(point_q);
	OPENSSL_free(point_g_buf);
	OPENSSL_free(point_q_buf);
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
	return pkey;
}
#endif

void php_openssl_pkey_object_curve_25519_448(zval *return_value, const char *name, zval *data) {
	EVP_PKEY *pkey = NULL;
	EVP_PKEY_CTX *ctx = NULL;
	OSSL_PARAM *params = NULL;
	OSSL_PARAM_BLD *bld = OSSL_PARAM_BLD_new();
	bool is_private;

	RETVAL_FALSE;

	if (!bld) {
		goto cleanup;
	}

	zval *priv_key = zend_hash_str_find(Z_ARRVAL_P(data), "priv_key", sizeof("priv_key") - 1);
	if (priv_key && Z_TYPE_P(priv_key) == IS_STRING && Z_STRLEN_P(priv_key) > 0) {
		if (!OSSL_PARAM_BLD_push_octet_string(bld, OSSL_PKEY_PARAM_PRIV_KEY, Z_STRVAL_P(priv_key), Z_STRLEN_P(priv_key))) {
			goto cleanup;
		}
	}

	zval *pub_key = zend_hash_str_find(Z_ARRVAL_P(data), "pub_key", sizeof("pub_key") - 1);
	if (pub_key && Z_TYPE_P(pub_key) == IS_STRING && Z_STRLEN_P(pub_key) > 0) {
		if (!OSSL_PARAM_BLD_push_octet_string(bld, OSSL_PKEY_PARAM_PUB_KEY, Z_STRVAL_P(pub_key), Z_STRLEN_P(pub_key))) {
			goto cleanup;
		}
	}

	params = OSSL_PARAM_BLD_to_param(bld);
	ctx = php_openssl_pkey_new_from_name(name, 0);
	if (!params || !ctx) {
		goto cleanup;
	}

	if (pub_key || priv_key) {
		if (EVP_PKEY_fromdata_init(ctx) <= 0 ||
				EVP_PKEY_fromdata(ctx, &pkey, EVP_PKEY_KEYPAIR, params) <= 0) {
			goto cleanup;
		}
		is_private = priv_key != NULL;
	} else {
		is_private = true;
		if (EVP_PKEY_keygen_init(ctx) <= 0 || EVP_PKEY_keygen(ctx, &pkey) <= 0) {
			goto cleanup;
		}
	}

	if (pkey) {
		php_openssl_pkey_object_init(return_value, pkey, is_private);
	}

cleanup:
	php_openssl_store_errors();
	EVP_PKEY_CTX_free(ctx);
	OSSL_PARAM_free(params);
	OSSL_PARAM_BLD_free(bld);
}

static void php_openssl_copy_bn_param(
		zval *ary, EVP_PKEY *pkey, const char *param, const char *name) {
	BIGNUM *bn = NULL;
	if (EVP_PKEY_get_bn_param(pkey, param, &bn) > 0) {
		php_openssl_add_bn_to_array(ary, bn, name);
		BN_free(bn);
	}
}

#ifdef HAVE_EVP_PKEY_EC
static zend_string *php_openssl_get_utf8_param(
		EVP_PKEY *pkey, const char *param, const char *name) {
	char buf[64];
	size_t len;
	if (EVP_PKEY_get_utf8_string_param(pkey, param, buf, sizeof(buf), &len) > 0) {
		zend_string *str = zend_string_alloc(len, 0);
		memcpy(ZSTR_VAL(str), buf, len);
		ZSTR_VAL(str)[len] = '\0';
		return str;
	}
	return NULL;
}
#endif

static void php_openssl_copy_octet_string_param(
		zval *ary, EVP_PKEY *pkey, const char *param, const char *name)
{
	unsigned char buf[64];
	size_t len;
	if (EVP_PKEY_get_octet_string_param(pkey, param, buf, sizeof(buf), &len) > 0) {
		zend_string *str = zend_string_alloc(len, 0);
		memcpy(ZSTR_VAL(str), buf, len);
		ZSTR_VAL(str)[len] = '\0';
		add_assoc_str(ary, name, str);
	}
}

static void php_openssl_copy_curve_25519_448_params(
		zval *return_value, const char *assoc_name, EVP_PKEY *pkey)
{
	zval ary;
	array_init(&ary);
	add_assoc_zval(return_value, assoc_name, &ary);
	php_openssl_copy_octet_string_param(&ary, pkey, OSSL_PKEY_PARAM_PRIV_KEY, "priv_key");
	php_openssl_copy_octet_string_param(&ary, pkey, OSSL_PKEY_PARAM_PUB_KEY, "pub_key");
}

zend_long php_openssl_pkey_get_details(zval *return_value, EVP_PKEY *pkey)
{
	zval ary;
	int base_id = 0;
	zend_long ktype;

	if (EVP_PKEY_id(pkey) != EVP_PKEY_KEYMGMT) {
		base_id = EVP_PKEY_base_id(pkey);
	} else {
		const char *type_name = EVP_PKEY_get0_type_name(pkey);
		if (type_name) {
			int nid = OBJ_txt2nid(type_name);
			if (nid != NID_undef) {
				base_id = EVP_PKEY_type(nid);
			}
		}
	}

	switch (base_id) {
		case EVP_PKEY_RSA:
			ktype = OPENSSL_KEYTYPE_RSA;
			array_init(&ary);
			add_assoc_zval(return_value, "rsa", &ary);
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_RSA_N, "n");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_RSA_E, "e");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_RSA_D, "d");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_RSA_FACTOR1, "p");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_RSA_FACTOR2, "q");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_RSA_EXPONENT1, "dmp1");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_RSA_EXPONENT2, "dmq1");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_RSA_COEFFICIENT1, "iqmp");
			break;
		case EVP_PKEY_DSA:
			ktype = OPENSSL_KEYTYPE_DSA;
			array_init(&ary);
			add_assoc_zval(return_value, "dsa", &ary);
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_FFC_P, "p");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_FFC_Q, "q");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_FFC_G, "g");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_PRIV_KEY, "priv_key");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_PUB_KEY, "pub_key");
			break;
		case EVP_PKEY_DH:
			ktype = OPENSSL_KEYTYPE_DH;
			array_init(&ary);
			add_assoc_zval(return_value, "dh", &ary);
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_FFC_P, "p");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_FFC_G, "g");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_PRIV_KEY, "priv_key");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_PUB_KEY, "pub_key");
			break;
#ifdef HAVE_EVP_PKEY_EC
		case EVP_PKEY_EC: {
			ktype = OPENSSL_KEYTYPE_EC;
			array_init(&ary);
			add_assoc_zval(return_value, "ec", &ary);

			zend_string *curve_name = php_openssl_get_utf8_param(
				pkey, OSSL_PKEY_PARAM_GROUP_NAME, "curve_name");
			if (curve_name) {
				add_assoc_str(&ary, "curve_name", curve_name);

				int nid = OBJ_sn2nid(ZSTR_VAL(curve_name));
				if (nid != NID_undef) {
					ASN1_OBJECT *obj = OBJ_nid2obj(nid);
					if (obj) {
						// OpenSSL recommends a buffer length of 80.
						char oir_buf[80];
						int oir_len = OBJ_obj2txt(oir_buf, sizeof(oir_buf), obj, 1);
						add_assoc_stringl(&ary, "curve_oid", oir_buf, oir_len);
						ASN1_OBJECT_free(obj);
					}
				}
			}

			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_EC_PUB_X, "x");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_EC_PUB_Y, "y");
			php_openssl_copy_bn_param(&ary, pkey, OSSL_PKEY_PARAM_PRIV_KEY, "d");
			break;
		}
#endif
        case EVP_PKEY_X25519: {
			ktype = OPENSSL_KEYTYPE_X25519;
			php_openssl_copy_curve_25519_448_params(return_value, "x25519", pkey);
			break;
		}
		case EVP_PKEY_ED25519: {
			ktype = OPENSSL_KEYTYPE_ED25519;
			php_openssl_copy_curve_25519_448_params(return_value, "ed25519", pkey);
			break;
		}
		case EVP_PKEY_X448: {
			ktype = OPENSSL_KEYTYPE_X448;
			php_openssl_copy_curve_25519_448_params(return_value, "x448", pkey);
			break;
		}
		case EVP_PKEY_ED448: {
			ktype = OPENSSL_KEYTYPE_ED448;
			php_openssl_copy_curve_25519_448_params(return_value, "ed448", pkey);
			break;
		}
        default:
			ktype = -1;
			break;
    }

	return ktype;
}

zend_string *php_openssl_dh_compute_key(EVP_PKEY *pkey, char *pub_str, size_t pub_len)
{
	EVP_PKEY *peer_key = EVP_PKEY_new();
	if (!peer_key || EVP_PKEY_copy_parameters(peer_key, pkey) <= 0 ||
			EVP_PKEY_set1_encoded_public_key(peer_key, (unsigned char *) pub_str, pub_len) <= 0) {
		php_openssl_store_errors();
		EVP_PKEY_free(peer_key);
		return NULL;
	}

	zend_string *result = php_openssl_pkey_derive(pkey, peer_key, 0);
	EVP_PKEY_free(peer_key);
	return result;
}

static void php_openssl_add_cipher_name(const char *name, void *arg)
{
	size_t len = strlen(name);
	zend_string *str = zend_string_alloc(len, 0);
	zend_str_tolower_copy(ZSTR_VAL(str), name, len);
	add_next_index_str((zval*)arg, str);
}

static void php_openssl_add_cipher_or_alias(EVP_CIPHER *cipher, void *arg)
{
	EVP_CIPHER_names_do_all(cipher, php_openssl_add_cipher_name, arg);
}

static void php_openssl_add_cipher(EVP_CIPHER *cipher, void *arg)
{
	php_openssl_add_cipher_name(EVP_CIPHER_get0_name(cipher), arg);
}

static int php_openssl_compare_func(Bucket *a, Bucket *b)
{
	return string_compare_function(&a->val, &b->val);
}

void php_openssl_get_cipher_methods(zval *return_value, bool aliases)
{
	array_init(return_value);
	EVP_CIPHER_do_all_provided(NULL,
		aliases ? php_openssl_add_cipher_or_alias : php_openssl_add_cipher,
		return_value);
	zend_hash_sort(Z_ARRVAL_P(return_value), php_openssl_compare_func, 1);
}

#endif
