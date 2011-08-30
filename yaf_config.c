/*
  +----------------------------------------------------------------------+
  | Yet Another Framework                                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Xinchen Hui  <laruence@php.net>                              |
  +----------------------------------------------------------------------+
*/
   
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "main/SAPI.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_alloc.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_filestat.h"

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_config.h"

zend_class_entry *yaf_config_ce;
#ifdef HAVE_SPL
extern PHPAPI zend_class_entry *spl_ce_Countable;
#endif

/* {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_config_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()
/* }}} */

#include "configs/ini.c"
#include "configs/simple.c"

static zval * yaf_config_ini_zval_persistent(zval *zvalue TSRMLS_DC);
static zval * yaf_config_ini_zval_losable(zval *zvalue TSRMLS_DC);

/** {{{ yaf_config_ini_modified
*/
static int yaf_config_ini_modified(zval * file, long ctime TSRMLS_DC) {
	zval  n_ctime;
	php_stat(Z_STRVAL_P(file), Z_STRLEN_P(file) + 1, 7 /*YAF_FS_CTIME*/ , &n_ctime TSRMLS_CC);
	if (Z_TYPE(n_ctime) != IS_BOOL && ctime != Z_LVAL(n_ctime)) {
		return Z_LVAL(n_ctime);
	}	
	return 0;
}
/* }}} */

/** {{{ static void yaf_config_cache_dtor(yaf_config_cache **cache)
 */
static void yaf_config_cache_dtor(yaf_config_cache **cache) {
	if (*cache) {
		zend_hash_destroy((*cache)->data);
		pefree((*cache)->data, 1);
		pefree(*cache, 1);
	}
}
/* }}} */

/** {{{ static void yaf_config_zval_dtor(zval **value)
 */
static void yaf_config_zval_dtor(zval **value) {
	if (*value) {
		switch(Z_TYPE_PP(value)) {
			case IS_STRING:
			case IS_CONSTANT:
				CHECK_ZVAL_STRING(*value);
				pefree((*value)->value.str.val, 1);
				pefree(*value, 1);
				break;
			case IS_ARRAY:
			case IS_CONSTANT_ARRAY: {
				zend_hash_destroy((*value)->value.ht);
				pefree((*value)->value.ht, 1);
				pefree(*value, 1);
			}
			break;
		}
	}
}
/* }}} */

/** {{{ static void yaf_config_copy_persistent(HashTable *pdst, HashTable *src TSRMLS_DC)
 */
static void yaf_config_copy_persistent(HashTable *pdst, HashTable *src TSRMLS_DC) {
	zval **ppzval;
	char *key;
	uint keylen;
	long idx;

	for(zend_hash_internal_pointer_reset(src);
			zend_hash_has_more_elements(src) == SUCCESS;
			zend_hash_move_forward(src)) {

		if (zend_hash_get_current_key_ex(src, &key, &keylen, &idx, 0, NULL) == HASH_KEY_IS_LONG) {
			zval *tmp;
			if (zend_hash_get_current_data(src, (void**)&ppzval) == FAILURE) {
				continue;
			}
	
			tmp = yaf_config_ini_zval_persistent(*ppzval TSRMLS_CC);
			if (tmp)
			zend_hash_index_update(pdst, idx, (void **)&tmp, sizeof(zval *), NULL);
			
		} else {
			zval *tmp;
			if (zend_hash_get_current_data(src, (void**)&ppzval) == FAILURE) {
				continue;
			}

			tmp = yaf_config_ini_zval_persistent(*ppzval TSRMLS_CC);
			if (tmp)
			zend_hash_update(pdst, key, keylen, (void **)&tmp, sizeof(zval *), NULL);
		}
	}
}
/* }}} */

/** {{{ static void yaf_config_copy_losable(HashTable *ldst, HashTable *src TSRMLS_DC)
 */
static void yaf_config_copy_losable(HashTable *ldst, HashTable *src TSRMLS_DC) {
	zval **ppzval, *tmp;
	char *key;  
	long idx; 
	uint keylen;

	for(zend_hash_internal_pointer_reset(src);
			zend_hash_has_more_elements(src) == SUCCESS;
			zend_hash_move_forward(src)) {

		if (zend_hash_get_current_key_ex(src, &key, &keylen, &idx, 0, NULL) == HASH_KEY_IS_LONG) {
			if (zend_hash_get_current_data(src, (void**)&ppzval) == FAILURE) {
				continue;
			}
	
			tmp = yaf_config_ini_zval_losable(*ppzval TSRMLS_CC);
			zend_hash_index_update(ldst, idx, (void **)&tmp, sizeof(zval *), NULL);
			
		} else {
			if (zend_hash_get_current_data(src, (void**)&ppzval) == FAILURE) {
				continue;
			}

			tmp = yaf_config_ini_zval_losable(*ppzval TSRMLS_CC);
			zend_hash_update(ldst, key, keylen, (void **)&tmp, sizeof(zval *), NULL);
		}
	}
}
/* }}} */

/** {{{ static zval * yaf_config_ini_zval_persistent(zval *zvalue TSRMLS_DC)
 */
static zval * yaf_config_ini_zval_persistent(zval *zvalue TSRMLS_DC) {
	zval *ret = (zval *)pemalloc(sizeof(zval), 1);
	INIT_PZVAL(ret);
	switch (zvalue->type) {
		case IS_RESOURCE:
		case IS_OBJECT:
			break;
		case IS_BOOL:
		case IS_LONG:
		case IS_NULL:
			break;
		case IS_CONSTANT:
		case IS_STRING:
				CHECK_ZVAL_STRING(zvalue);
				Z_TYPE_P(ret) = IS_STRING;
				ret->value.str.val = pestrndup(zvalue->value.str.val, zvalue->value.str.len, 1);
				ret->value.str.len = zvalue->value.str.len;
			break;
		case IS_ARRAY:
		case IS_CONSTANT_ARRAY: {
				HashTable *tmp_ht, *original_ht = zvalue->value.ht;
				tmp_ht = (HashTable *)pemalloc(sizeof(HashTable), 1);
				if (!tmp_ht) {
					return NULL;
				}

				zend_hash_init(tmp_ht, zend_hash_num_elements(original_ht), NULL, (dtor_func_t)yaf_config_zval_dtor, 1);
				yaf_config_copy_persistent(tmp_ht, original_ht TSRMLS_CC);
				Z_TYPE_P(ret) = IS_ARRAY;
				ret->value.ht = tmp_ht;
			}
			break;
	}

	return ret;
}
/* }}} */

/** {{{ static zval * yaf_config_ini_zval_losable(zval *zvalue TSRMLS_DC)
 */
static zval * yaf_config_ini_zval_losable(zval *zvalue TSRMLS_DC) {
	zval *ret;
	MAKE_STD_ZVAL(ret);
	switch (zvalue->type) {
		case IS_RESOURCE:
		case IS_OBJECT:
			break;
		case IS_BOOL:
		case IS_LONG:
		case IS_NULL:
			break;
		case IS_CONSTANT:
		case IS_STRING:
				CHECK_ZVAL_STRING(zvalue);
				ZVAL_STRINGL(ret, zvalue->value.str.val, zvalue->value.str.len, 1);
			break;
		case IS_ARRAY:
		case IS_CONSTANT_ARRAY: {
				HashTable *original_ht = zvalue->value.ht;
				array_init(ret);
				yaf_config_copy_losable(Z_ARRVAL_P(ret), original_ht TSRMLS_CC);
			}
			break;
	}

	return ret;
}
/* }}} */

/** {{{ static yaf_config_t * yaf_config_ini_unserialize(yaf_config_t *this_ptr, zval *filename, zval *section TSRMLS_DC)
 */
static yaf_config_t * yaf_config_ini_unserialize(yaf_config_t *this_ptr, zval *filename, zval *section TSRMLS_DC) {
	char *key;
	uint len;
	yaf_config_cache **ppval;

	if (!YAF_G(configs)) {
		return NULL;
	}

	len = spprintf(&key, 0, "%s#%s", Z_STRVAL_P(filename), Z_STRVAL_P(section));

	if (zend_hash_find(YAF_G(configs), key, len + 1, (void **)&ppval) == SUCCESS) {
		if (yaf_config_ini_modified(filename, (*ppval)->ctime TSRMLS_CC)) {
			efree(key);
			return NULL;
		} else {
			zval *props;

			MAKE_STD_ZVAL(props);
			array_init(props);
			yaf_config_copy_losable(Z_ARRVAL_P(props), (*ppval)->data TSRMLS_CC);
			efree(key);
			return yaf_config_ini_instance(this_ptr, props, section TSRMLS_CC);
		}
		efree(key);
	}

	return NULL;
}
/* }}} */

/** {{{ static void yaf_config_ini_serialize(yaf_config_t *this_ptr, zval *filename, zval *section TSRMLS_DC)
 */
static void yaf_config_ini_serialize(yaf_config_t *this_ptr, zval *filename, zval *section TSRMLS_DC) {
	char *key;
	uint len;
	long ctime;
	zval *configs;
	HashTable *persistent;
	yaf_config_cache *cache;

	if (!YAF_G(configs)) {
		YAF_G(configs) = (HashTable *)pemalloc(sizeof(HashTable), 1);
		if (!YAF_G(configs)) {
			return;
		}
		zend_hash_init(YAF_G(configs), 8, NULL, (dtor_func_t) yaf_config_cache_dtor, 1);
	}

	cache = (yaf_config_cache *)pemalloc(sizeof(yaf_config_cache), 1);
	
	if (!cache) {
		return;
	}

	persistent = (HashTable *)pemalloc(sizeof(HashTable), 1);
	if (!persistent) {
		return;
	}

	configs = zend_read_property(yaf_config_ini_ce, this_ptr, ZEND_STRL(YAF_CONFIG_PROPERT_NAME), 1 TSRMLS_CC);

	zend_hash_init(persistent, zend_hash_num_elements(Z_ARRVAL_P(configs)), NULL, (dtor_func_t) yaf_config_zval_dtor, 1);

	yaf_config_copy_persistent(persistent, Z_ARRVAL_P(configs) TSRMLS_CC);

	ctime = yaf_config_ini_modified(filename, 0 TSRMLS_CC);
	cache->ctime = ctime;
	cache->data  = persistent;
	len = spprintf(&key, 0, "%s#%s", Z_STRVAL_P(filename), Z_STRVAL_P(section));
	
	zend_hash_update(YAF_G(configs), key, len + 1, (void **)&cache, sizeof(yaf_config_cache *), NULL);

	efree(key);
}
/* }}} */

/** {{{ yaf_config_t * yaf_config_instance(yaf_config_t *this_ptr, zval *arg1, zval *arg2 TSRMLS_DC)
 */
yaf_config_t * yaf_config_instance(yaf_config_t *this_ptr, zval *arg1, zval *arg2 TSRMLS_DC) {
	yaf_config_t *instance;

	if (!arg1) {
		return NULL;
	}

	if (Z_TYPE_P(arg1) == IS_STRING) {
		if (strncasecmp(Z_STRVAL_P(arg1) + Z_STRLEN_P(arg1) - 3, "ini", 3) == 0) {
			if (YAF_G(cache_config)) {
				if ((instance = yaf_config_ini_unserialize(this_ptr, arg1, arg2 TSRMLS_CC))) {
					return instance;
				}
			}

			instance = yaf_config_ini_instance(this_ptr, arg1, arg2 TSRMLS_CC);

			if (!instance) {
				return NULL;
			}

			if (YAF_G(cache_config)) {
				yaf_config_ini_serialize(instance, arg1, arg2 TSRMLS_CC);
			}

			return instance;
		}
	}

	if (Z_TYPE_P(arg1) == IS_ARRAY) {
		zval *readonly;

		MAKE_STD_ZVAL(readonly);
		ZVAL_BOOL(readonly, 1);
		instance = yaf_config_simple_instance(this_ptr, arg1, readonly TSRMLS_CC);
		efree(readonly);
		return instance;
	}

	yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "Expects a string or an array as parameter");
	return NULL;
}
/* }}} */

/** {{{ yaf_config_methods
*/
zend_function_entry yaf_config_methods[] = {
	PHP_ABSTRACT_ME(yaf_config, get, NULL)
	PHP_ABSTRACT_ME(yaf_config, set, NULL)
	PHP_ABSTRACT_ME(yaf_config, readonly, NULL)
	PHP_ABSTRACT_ME(yaf_config, toArray, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(config) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Config_Abstract", "Yaf\\Config_Abstract", yaf_config_methods);
	yaf_config_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
	yaf_config_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	zend_declare_property_null(yaf_config_ce, YAF_STRL(YAF_CONFIG_PROPERT_NAME), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_bool(yaf_config_ce, YAF_STRL(YAF_CONFIG_PROPERT_NAME_READONLY), 1, ZEND_ACC_PROTECTED TSRMLS_CC);

	YAF_STARTUP(config_ini);
	YAF_STARTUP(config_simple);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
