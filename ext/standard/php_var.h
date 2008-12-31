/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jani Lehtimäki <jkl@njet.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_VAR_H
#define PHP_VAR_H

#include "ext/standard/php_smart_str_public.h"

PHP_FUNCTION(var_dump);
PHP_FUNCTION(var_inspect);
PHP_FUNCTION(var_export);
PHP_FUNCTION(debug_zval_dump);
PHP_FUNCTION(serialize);
PHP_FUNCTION(unserialize);
PHP_FUNCTION(memory_get_usage);
PHP_FUNCTION(memory_get_peak_usage);

PHPAPI void php_var_dump(zval **struc, int level, int verbose TSRMLS_DC);
PHPAPI void php_var_export(zval **struc, int level TSRMLS_DC);
PHPAPI void php_debug_zval_dump(zval **struc, int level, int verbose TSRMLS_DC);

/* typdef HashTable php_serialize_data_t; */
#define php_serialize_data_t HashTable

struct php_unserialize_data {
	void *first;
	void *first_dtor;
};

typedef struct php_unserialize_data php_unserialize_data_t;

PHPAPI void php_var_serialize(smart_str *buf, zval **struc, php_serialize_data_t *var_hash TSRMLS_DC);
PHPAPI int php_var_unserialize(zval **rval, const unsigned char **p, const unsigned char *max, php_unserialize_data_t *var_hash TSRMLS_DC);

#define PHP_VAR_SERIALIZE_INIT(var_hash) \
   zend_hash_init(&(var_hash), 10, NULL, NULL, 0)
#define PHP_VAR_SERIALIZE_DESTROY(var_hash) \
   zend_hash_destroy(&(var_hash))

#define PHP_VAR_UNSERIALIZE_INIT(var_hash) \
	(var_hash).first = 0; \
	(var_hash).first_dtor = 0
#define PHP_VAR_UNSERIALIZE_DESTROY(var_hash) \
	var_destroy(&(var_hash))

PHPAPI void var_replace(php_unserialize_data_t *var_hash, zval *ozval, zval **nzval);
PHPAPI void var_destroy(php_unserialize_data_t *var_hash);

#define PHP_VAR_UNSERIALIZE_ZVAL_CHANGED(var_hash, ozval, nzval) \
	var_replace((var_hash), (ozval), &(nzval))
	
PHPAPI zend_class_entry *php_create_empty_class(char *class_name, int len);

static inline int php_varname_check_string(char * name, int name_len, zend_bool silent TSRMLS_DC) /* {{{ */
{
	if (name_len == sizeof("GLOBALS")-1 && !memcmp(name, "GLOBALS", sizeof("GLOBALS")-1)) {
		if (!silent) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attempted GLOBALS variable overwrite");
		}
		return FAILURE;
	} else if (name[0] == '_' &&
				(
				 (name_len == sizeof("_GET")-1 && !memcmp(name, "_GET", sizeof("_GET"))) ||
				 (name_len == sizeof("_POST")-1 && !memcmp(name, "_POST", sizeof("_POST"))) ||
				 (name_len == sizeof("_COOKIE")-1 && !memcmp(name, "_COOKIE", sizeof("_COOKIE"))) ||
				 (name_len == sizeof("_ENV")-1 && !memcmp(name, "_ENV", sizeof("_ENV"))) ||
				 (name_len == sizeof("_SERVER")-1 && !memcmp(name, "_SERVER", sizeof("_SERVER"))) ||
				 (name_len == sizeof("_SESSION")-1 && !memcmp(name, "_SESSION", sizeof("_SESSION"))) ||
				 (name_len == sizeof("_FILES")-1 && !memcmp(name, "_FILES", sizeof("_FILES"))) ||
				 (name_len == sizeof("_REQUEST")-1 && !memcmp(name, "_REQUEST", sizeof("_REQUEST")))
				)
		   ) {
			if (!silent) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attempted super-global (%s) variable overwrite", name);
			}
			return FAILURE;
	} else if (name[0] == 'H' &&
			(
			 (name_len == sizeof("HTTP_POST_VARS")-1 && !memcmp(name, "HTTP_POST_VARS", sizeof("HTTP_POST_VARS"))) ||
			 (name_len == sizeof("HTTP_GET_VARS")-1 && !memcmp(name, "HTTP_GET_VARS", sizeof("HTTP_GET_VARS"))) ||
			 (name_len == sizeof("HTTP_COOKIE_VARS")-1 && !memcmp(name, "HTTP_COOKIE_VARS", sizeof("HTTP_COOKIE_VARS"))) ||
			 (name_len == sizeof("HTTP_ENV_VARS")-1 && !memcmp(name, "HTTP_ENV_VARS", sizeof("HTTP_ENV_VARS"))) ||
			 (name_len == sizeof("HTTP_SESSION_VARS")-1 && !memcmp(name, "HTTP_SESSION_VARS", sizeof("HTTP_SESSION_VARS"))) ||
			 (name_len == sizeof("HTTP_SERVER_VARS")-1 && !memcmp(name, "HTTP_SERVER_VARS", sizeof("HTTP_SERVER_VARS"))) ||
			 (name_len == sizeof("HTTP_RAW_POST_DATA")-1 && !memcmp(name, "HTTP_RAW_POST_DATA", sizeof("HTTP_RAW_POST_DATA"))) ||
			 (name_len == sizeof("HTTP_POST_VARS")-1 && !memcmp(name, "HTTP_POST_FILES", sizeof("HTTP_POST_FILES")))
			)
			) {
		if (!silent) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attempted long input array (%s) overwrite", name);
		}
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

static inline int php_varname_check_unicode(UChar *name, int name_len, zend_bool silent TSRMLS_DC) /* {{{ */
{
	if (name_len == sizeof("GLOBALS")-1 && !zend_cmp_unicode_and_literal(name, name_len, "GLOBALS", sizeof("GLOBALS")-1)) {
		if (!silent) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attempted GLOBALS variable overwrite");
		}
		return FAILURE;
	} else if (name[0] == 0x5f /* '_' */ &&
			(
			 (name_len == sizeof("_GET")-1 && !zend_cmp_unicode_and_literal(name, name_len, "_GET", sizeof("_GET")-1)) ||
			 (name_len == sizeof("_POST")-1 && !zend_cmp_unicode_and_literal(name, name_len, "_POST", sizeof("_POST")-1)) ||
			 (name_len == sizeof("_COOKIE")-1 && !zend_cmp_unicode_and_literal(name, name_len, "_COOKIE", sizeof("_COOKIE")-1)) ||
			 (name_len == sizeof("_ENV")-1 && !zend_cmp_unicode_and_literal(name, name_len, "_ENV", sizeof("_ENV")-1)) ||
			 (name_len == sizeof("_SERVER")-1 && !zend_cmp_unicode_and_literal(name, name_len, "_SERVER", sizeof("_SERVER")-1)) ||
			 (name_len == sizeof("_SESSION")-1 && !zend_cmp_unicode_and_literal(name, name_len, "_SESSION", sizeof("_SESSION")-1)) ||
			 (name_len == sizeof("_FILES")-1 && !zend_cmp_unicode_and_literal(name, name_len, "_FILES", sizeof("_FILES")-1)) ||
			 (name_len == sizeof("_REQUEST")-1 && !zend_cmp_unicode_and_literal(name, name_len, "_REQUEST", sizeof("_REQUEST")-1))
			)
			) {
		if (!silent) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attempted super-global (%r) variable overwrite", name);
		}
		return FAILURE;
	} else if (name[0] == 0x48 /* 'H' */ &&
			(
			 (name_len == sizeof("HTTP_POST_VARS")-1 && !zend_cmp_unicode_and_literal(name, name_len, "HTTP_POST_VARS", sizeof("HTTP_POST_VARS")-1)) ||
			 (name_len == sizeof("HTTP_GET_VARS")-1 && !zend_cmp_unicode_and_literal(name, name_len, "HTTP_GET_VARS", sizeof("HTTP_GET_VARS")-1)) ||
			 (name_len == sizeof("HTTP_COOKIE_VARS")-1 && !zend_cmp_unicode_and_literal(name, name_len, "HTTP_COOKIE_VARS", sizeof("HTTP_COOKIE_VARS")-1)) ||
			 (name_len == sizeof("HTTP_ENV_VARS")-1 && !zend_cmp_unicode_and_literal(name, name_len, "HTTP_ENV_VARS", sizeof("HTTP_ENV_VARS")-1)) ||
			 (name_len == sizeof("HTTP_SESSION_VARS")-1 && !zend_cmp_unicode_and_literal(name, name_len, "HTTP_SESSION_VARS", sizeof("HTTP_SESSION_VARS")-1)) ||
			 (name_len == sizeof("HTTP_SERVER_VARS")-1 && !zend_cmp_unicode_and_literal(name, name_len, "HTTP_SERVER_VARS", sizeof("HTTP_SERVER_VARS")-1)) ||
			 (name_len == sizeof("HTTP_RAW_POST_DATA")-1 && !zend_cmp_unicode_and_literal(name, name_len, "HTTP_RAW_POST_DATA", sizeof("HTTP_RAW_POST_DATA")-1)) ||
			 (name_len == sizeof("HTTP_POST_FILES")-1 && !zend_cmp_unicode_and_literal(name, name_len, "HTTP_POST_FILES", sizeof("HTTP_POST_FILES")-1))
			)
			) {
		if (!silent) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attempted long input array (%r) overwrite", name);
		}
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

static inline int php_varname_check(zend_uchar type, zstr name, int name_len, zend_bool silent TSRMLS_DC) /* {{{ */
{
	if (type == IS_UNICODE) {
		return php_varname_check_unicode(name.u, name_len, silent TSRMLS_CC);
	}
	return php_varname_check_string(name.s, name_len, silent TSRMLS_CC);
}
/* }}} */

#endif /* PHP_VAR_H */
