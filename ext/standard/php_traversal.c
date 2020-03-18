/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Tjerk Meesters <datibbaw@php.net>                           |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_traversal.h"

void php_traverse(zval *t, php_traverse_each_t each_func, int traverse_mode, void *context)
{
	zend_bool should_continue;
	zval *value;
	zval key;

	if (Z_TYPE_P(t) == IS_ARRAY) {
		zend_ulong num_key;
		zend_string *string_key;

		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(t), num_key, string_key, value) {
			if (traverse_mode == PHP_TRAVERSE_MODE_KEY_VAL) {
				if (string_key) {
					ZVAL_STR(&key, string_key);
				} else {
					ZVAL_LONG(&key, num_key);
				}
				should_continue = each_func(value, &key, context);
			} else {
				should_continue = each_func(value, NULL, context);
			}
			if (!should_continue) break;
		} ZEND_HASH_FOREACH_END();
	} else {
		zend_class_entry *ce = Z_OBJCE_P(t);
		zend_object_iterator *iter = ce->get_iterator(ce, t, 0);
		if (EG(exception)) goto fail;
		iter->index = 0;
		if (iter->funcs->rewind) {
			iter->funcs->rewind(iter);
		}
		while (!EG(exception) && iter->funcs->valid(iter) == SUCCESS) {
			value = iter->funcs->get_current_data(iter);
			if (EG(exception) || value == NULL) break;

			if (traverse_mode == PHP_TRAVERSE_MODE_KEY_VAL) {
				if (iter->funcs->get_current_key) {
					iter->funcs->get_current_key(iter, &key);
					if (EG(exception)) break;
				} else {
					ZVAL_NULL(&key);
				}

				should_continue = each_func(value, &key, context);
				zval_ptr_dtor(&key);
			} else {
				should_continue = each_func(value, NULL, context);
			}
			if (!should_continue) break;

			iter->index++;
			iter->funcs->move_forward(iter);
		}
fail:
		zend_iterator_dtor(iter);
	}
}
