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
   | Author: Jani Lehtimäki <jkl@njet.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_VAR_H
#define PHP_VAR_H

#include "ext/standard/basic_functions.h"
#include "zend_smart_str_public.h"

PHP_MINIT_FUNCTION(var);

PHPAPI void php_var_dump(zval *struc, int level);
PHPAPI void php_var_export(zval *struc, int level);
PHPAPI void php_var_export_ex(zval *struc, int level, smart_str *buf);

PHPAPI void php_debug_zval_dump(zval *struc, int level);

typedef struct php_serialize_data *php_serialize_data_t;
typedef struct php_unserialize_data *php_unserialize_data_t;

PHPAPI void php_var_serialize(smart_str *buf, zval *struc, php_serialize_data_t *data);
PHPAPI int php_var_unserialize(zval *rval, const unsigned char **p, const unsigned char *max, php_unserialize_data_t *var_hash);
PHPAPI int php_var_unserialize_ref(zval *rval, const unsigned char **p, const unsigned char *max, php_unserialize_data_t *var_hash);
PHPAPI int php_var_unserialize_intern(zval *rval, const unsigned char **p, const unsigned char *max, php_unserialize_data_t *var_hash);

PHPAPI php_serialize_data_t php_var_serialize_init(void);
PHPAPI void php_var_serialize_destroy(php_serialize_data_t d);
PHPAPI php_unserialize_data_t php_var_unserialize_init(void);
PHPAPI void php_var_unserialize_destroy(php_unserialize_data_t d);
PHPAPI HashTable *php_var_unserialize_get_allowed_classes(php_unserialize_data_t d);
PHPAPI void php_var_unserialize_set_allowed_classes(php_unserialize_data_t d, HashTable *classes);
PHPAPI void php_var_unserialize_set_max_depth(php_unserialize_data_t d, zend_long max_depth);
PHPAPI zend_long php_var_unserialize_get_max_depth(php_unserialize_data_t d);
PHPAPI void php_var_unserialize_set_cur_depth(php_unserialize_data_t d, zend_long cur_depth);
PHPAPI zend_long php_var_unserialize_get_cur_depth(php_unserialize_data_t d);

#define PHP_VAR_SERIALIZE_INIT(d) \
	(d) = php_var_serialize_init()

#define PHP_VAR_SERIALIZE_DESTROY(d) \
	php_var_serialize_destroy(d)

#define PHP_VAR_UNSERIALIZE_INIT(d) \
	(d) = php_var_unserialize_init()

#define PHP_VAR_UNSERIALIZE_DESTROY(d) \
	php_var_unserialize_destroy(d)

PHPAPI void php_unserialize_with_options(zval *return_value, const char *buf, const size_t buf_len, HashTable *options, const char* function_name);

PHPAPI void var_replace(php_unserialize_data_t *var_hash, zval *ozval, zval *nzval);
PHPAPI void var_push_dtor(php_unserialize_data_t *var_hash, zval *val);
PHPAPI zval *var_tmp_var(php_unserialize_data_t *var_hashx);
PHPAPI void var_destroy(php_unserialize_data_t *var_hash);

#endif /* PHP_VAR_H */
