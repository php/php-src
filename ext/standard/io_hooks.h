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
*/

#ifndef PHP_STANDARD_IO_HOOKS_H
#define PHP_STANDARD_IO_HOOKS_H

#include "main/php_io_hooks.h"

BEGIN_EXTERN_C()

/* Borrowed: no reference is added */
PHPAPI zend_object *php_io_operation_get_zobj(php_io_op *op);

typedef struct _php_io_opqueue_sub php_io_opqueue_sub;

typedef struct {
	php_io_queue *queue;
	php_io_opqueue_sub *subs;
	zend_object std;
} php_io_opqueue_obj;

#define PHP_IO_OPQUEUE_FROM_ZOBJ(o) ZEND_CONTAINER_OF(o, php_io_opqueue_obj, std)

PHPAPI extern zend_class_entry *php_io_operation_queue_ce;
PHPAPI extern zend_object_handlers php_io_opqueue_handlers;
PHPAPI zend_object *php_io_opqueue_create_object(zend_class_entry *ce);

PHP_MINIT_FUNCTION(io_hooks);

END_EXTERN_C()

#endif /* PHP_STANDARD_IO_HOOKS_H */
