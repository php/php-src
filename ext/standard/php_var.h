/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Jani Lehtimäki <jkl@njet.net>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_VAR_H
#define PHP_VAR_H

#include "ext/standard/php_smart_str_public.h"

PHP_FUNCTION(var_dump);
PHP_FUNCTION(serialize);
PHP_FUNCTION(unserialize);

void php_var_dump(zval **struc, int level TSRMLS_DC);

/* typdef HashTable php_serialize_data_t; */
#define php_serialize_data_t HashTable

PHPAPI void php_var_serialize(smart_str *buf, zval **struc, php_serialize_data_t *var_hash TSRMLS_DC);
PHPAPI int php_var_unserialize(zval **rval, const char **p, const char *max, php_serialize_data_t *var_hash TSRMLS_DC);

#define PHP_VAR_SERIALIZE_INIT(var_hash) \
   zend_hash_init(&(var_hash), 10, NULL, NULL, 0)
#define PHP_VAR_SERIALIZE_DESTROY(var_hash) \
   zend_hash_destroy(&(var_hash))

#define PHP_VAR_UNSERIALIZE_INIT(var_hash) \
   zend_hash_init(&(var_hash), 10, NULL, NULL, 0)
#define PHP_VAR_UNSERIALIZE_DESTROY(var_hash) \
   zend_hash_destroy(&(var_hash))

#define PHP_VAR_UNSERIALIZE_ZVAL_CHANGED(var_hash, ozval, nzval) \
if (var_hash) { \
    HashPosition pos; \
    zval **zval_ref; \
    zend_hash_internal_pointer_reset_ex(var_hash, &pos); \
    while (zend_hash_get_current_data_ex(var_hash, (void **) &zval_ref, &pos) == SUCCESS) { \
        if (*zval_ref == ozval) { \
            char *string_key; \
            uint str_key_len; \
            ulong num_key; \
							\
            zend_hash_get_current_key_ex(var_hash, &string_key, &str_key_len, &num_key, 1, &pos); \
            /* this is our hash and it _will_ be number indexed! */ \
            zend_hash_index_update(var_hash, num_key, &nzval, sizeof(zval *), NULL); \
            break; \
        }  \
        zend_hash_move_forward_ex(var_hash, &pos); \
    } \
}

PHPAPI zend_class_entry *php_create_empty_class(char *class_name, int len);

#endif /* PHP_VAR_H */
