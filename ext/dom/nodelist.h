/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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

dom_nodelist_dimension_index dom_modern_nodelist_get_index(const zval *offset);
zval *dom_modern_nodelist_read_dimension(zend_object *object, zval *offset, int type, zval *rv);
int dom_modern_nodelist_has_dimension(zend_object *object, zval *member, int check_empty);

#endif
