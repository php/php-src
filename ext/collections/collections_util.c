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
   | Author: Tyson Andre <tandre@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#include "collections_util.h"

/* Override get_properties_for and use the default implementation of get_properties. See https://github.com/php/php-src/issues/9697#issuecomment-1273613175 */
HashTable* collections_noop_empty_array_get_properties_for(zend_object *obj, zend_prop_purpose purpose) {
	(void)obj;
	(void)purpose;
	return NULL;
}

HashTable* collections_noop_get_gc(zend_object *obj, zval **table, int *n) {
	/* Zend/zend_gc.c does not initialize table or n. So we need to set n to 0 at minimum. */
	*n = 0;
	(void) table;
	(void) obj;
	/* Nothing needs to be garbage collected */
	return NULL;
}

HashTable *collections_internaliterator_get_gc(zend_object_iterator *iter, zval **table, int *n)
{
	*table = &iter->data;
	*n = 1;
	return NULL;
}

