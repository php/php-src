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
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_NODELIST_H
#define PHP_NODELIST_H

enum dom_nodelist_dimension_index_type {
	DOM_NODELIST_DIM_ILLEGAL,
	DOM_NODELIST_DIM_STRING,
	DOM_NODELIST_DIM_LONG,
};

typedef struct dom_nodelist_dimension_index {
	union {
		zend_long lval;
		zend_string *str;
	};
	enum dom_nodelist_dimension_index_type type;
} dom_nodelist_dimension_index;

void php_dom_nodelist_get_item_into_zval(dom_nnodemap_object *objmap, zend_long index, zval *return_value);
zend_long php_dom_get_nodelist_length(dom_object *obj);
dom_nodelist_dimension_index dom_modern_nodelist_get_index(const zval *offset);
zval *dom_modern_nodelist_read_dimension(zend_object *object, zval *offset, int type, zval *rv);
int dom_modern_nodelist_has_dimension(zend_object *object, zval *member, int check_empty);

#endif
