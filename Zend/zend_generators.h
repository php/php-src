/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2012 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Nikita Popov <nikic@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_GENERATORS_H
#define ZEND_GENERATORS_H

BEGIN_EXTERN_C()

typedef struct _zend_generator {
	zend_object std;

	/* The suspended execution context. */
	zend_execute_data *execute_data;
	/* Current value */
	zval *value;
	/* Current key */
	zval *key;
	/* Variable to put sent value into */
	temp_variable *send_target;
} zend_generator;

extern ZEND_API zend_class_entry *zend_ce_generator;

void zend_register_generator_ce(TSRMLS_D);
void zend_generator_close(zend_generator *generator, zend_bool finished_execution TSRMLS_DC);

END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
