/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifndef SPL_ITERATORS_H
#define SPL_ITERATORS_H

#include "php.h"
#include "php_spl.h"

extern zend_class_entry *spl_ce_RecursiveIterator;
extern zend_class_entry *spl_ce_RecursiveIteratorIterator;
extern zend_class_entry *spl_ce_FilterIterator;
extern zend_class_entry *spl_ce_ParentIterator;
extern zend_class_entry *spl_ce_SeekableIterator;
extern zend_class_entry *spl_ce_LimitIterator;

PHP_MINIT_FUNCTION(spl_iterators);

typedef struct _spl_dual_it_object {
	zend_object              std;
	struct {
		zval                 *zobject;
		zend_class_entry     *ce;
		zend_object          *object;
		zend_object_iterator *iterator;
	} inner;
	struct {
		zval                 **data;
		char                 *str_key;
		uint                 str_key_len;
		ulong                int_key;
		int                  key_type; /* HASH_KEY_IS_STRING or HASH_KEY_IS_LONG */
		int                  pos;
	} current;
	union {
		struct {
			int              offset;
			int              count;
		} limit;
	} u;
} spl_dual_it_object;

#endif /* SPL_ITERATORS_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
