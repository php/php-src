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
   | Authors: Hans-Peter Oeri (University of St.Gallen) <hp@oeri.ch>      |
   +----------------------------------------------------------------------+
 */

#ifndef RESOURCEBUNDLE_ITERATOR_H
#define RESOURCEBUNDLE_ITERATOR_H

#include <zend.h>

#include "resourcebundle/resourcebundle_class.h"

typedef struct {
	zend_object_iterator  intern;
	ResourceBundle_object *subject;
	bool             is_table;
	zend_long             length;
	zval                  current;
	char                  *currentkey;
	zend_long             i;
} ResourceBundle_iterator;

#ifdef __cplusplus
extern "C" {
#endif
zend_object_iterator *resourcebundle_get_iterator( zend_class_entry *ce, zval *object, int byref );
#ifdef __cplusplus
}
#endif

#endif // #ifndef RESOURCEBUNDLE_ITERATOR_H
