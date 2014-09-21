/* 
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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

#include "ext/standard/basic_functions.h"
#include "zend_smart_str_public.h"

PHP_FUNCTION(var_dump);
PHP_FUNCTION(var_export);
PHP_FUNCTION(debug_zval_dump);
PHP_FUNCTION(serialize);
PHP_FUNCTION(unserialize);
PHP_FUNCTION(memory_get_usage);
PHP_FUNCTION(memory_get_peak_usage);

PHPAPI void php_var_dump(zval *struc, int level TSRMLS_DC);
PHPAPI void php_var_export(zval *struc, int level TSRMLS_DC);
PHPAPI void php_var_export_ex(zval *struc, int level, smart_str *buf TSRMLS_DC);

PHPAPI void php_debug_zval_dump(zval *struc, int level TSRMLS_DC);

typedef HashTable* php_serialize_data_t;

struct php_unserialize_data {
	void *first;
	void *last;
	void *first_dtor;
	void *last_dtor;
};

typedef struct php_unserialize_data* php_unserialize_data_t;

PHPAPI void php_var_serialize(smart_str *buf, zval *struc, php_serialize_data_t *var_hash TSRMLS_DC);
PHPAPI int php_var_unserialize(zval *rval, const unsigned char **p, const unsigned char *max, php_unserialize_data_t *var_hash TSRMLS_DC);
PHPAPI int php_var_unserialize_ref(zval *rval, const unsigned char **p, const unsigned char *max, php_unserialize_data_t *var_hash TSRMLS_DC);
PHPAPI int php_var_unserialize_intern(zval *rval, const unsigned char **p, const unsigned char *max, php_unserialize_data_t *var_hash TSRMLS_DC);

#define PHP_VAR_SERIALIZE_INIT(var_hash_ptr) \
do  { \
	/* fprintf(stderr, "SERIALIZE_INIT      == lock: %u, level: %u\n", BG(serialize_lock), BG(serialize).level); */ \
	if (BG(serialize_lock) || !BG(serialize).level) { \
		ALLOC_HASHTABLE(var_hash_ptr); \
		zend_hash_init((var_hash_ptr), 16, NULL, NULL, 0); \
		if (!BG(serialize_lock)) { \
			BG(serialize).var_hash = (void *)(var_hash_ptr); \
			BG(serialize).level = 1; \
		} \
	} else { \
		(var_hash_ptr) = (php_serialize_data_t)BG(serialize).var_hash; \
		++BG(serialize).level; \
	} \
} while(0)

#define PHP_VAR_SERIALIZE_DESTROY(var_hash_ptr) \
do { \
	/* fprintf(stderr, "SERIALIZE_DESTROY   == lock: %u, level: %u\n", BG(serialize_lock), BG(serialize).level); */ \
	if (BG(serialize_lock) || !BG(serialize).level) { \
		zend_hash_destroy((var_hash_ptr)); \
		FREE_HASHTABLE(var_hash_ptr); \
	} else { \
		if (!--BG(serialize).level) { \
			zend_hash_destroy((php_serialize_data_t)BG(serialize).var_hash); \
			FREE_HASHTABLE((php_serialize_data_t)BG(serialize).var_hash); \
			BG(serialize).var_hash = NULL; \
		} \
	} \
} while (0)

#define PHP_VAR_UNSERIALIZE_INIT(var_hash_ptr) \
do { \
	/* fprintf(stderr, "UNSERIALIZE_INIT    == lock: %u, level: %u\n", BG(serialize_lock), BG(unserialize).level); */ \
	if (BG(serialize_lock) || !BG(unserialize).level) { \
		(var_hash_ptr) = (php_unserialize_data_t)ecalloc(1, sizeof(struct php_unserialize_data)); \
		if (!BG(serialize_lock)) { \
			BG(unserialize).var_hash = (void *)(var_hash_ptr); \
			BG(unserialize).level = 1; \
		} \
	} else { \
		(var_hash_ptr) = (php_unserialize_data_t)BG(unserialize).var_hash; \
		++BG(unserialize).level; \
	} \
} while (0)

#define PHP_VAR_UNSERIALIZE_DESTROY(var_hash_ptr) \
do { \
	/* fprintf(stderr, "UNSERIALIZE_DESTROY == lock: %u, level: %u\n", BG(serialize_lock), BG(unserialize).level); */ \
	if (BG(serialize_lock) || !BG(unserialize).level) { \
		var_destroy(&(var_hash_ptr)); \
		efree(var_hash_ptr); \
	} else { \
		if (!--BG(unserialize).level) { \
			var_destroy(&(var_hash_ptr)); \
			efree((var_hash_ptr)); \
			BG(unserialize).var_hash = NULL; \
		} \
	} \
} while (0)

PHPAPI void var_replace(php_unserialize_data_t *var_hash, zval *ozval, zval *nzval);
PHPAPI void var_push_dtor(php_unserialize_data_t *var_hash, zval *val);
PHPAPI void var_push_dtor_no_addref(php_unserialize_data_t *var_hashx, zval *rval);
PHPAPI void var_destroy(php_unserialize_data_t *var_hash);
	
#endif /* PHP_VAR_H */
