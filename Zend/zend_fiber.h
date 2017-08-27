/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2017 Zend Technologies Ltd. (http://www.zend.com) |
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
   |          Bob Weinand <bobwei9@hotmail.com>                           |
   |          Haitao Lv <php@lvht.net>                                    |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_FIBERS_H
#define ZEND_FIBERS_H

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_fiber;

typedef struct _zend_fiber zend_fiber;

struct _zend_fiber {
	zend_object std;

	zval closure;

	/* The suspended execution context. */
	zend_execute_data *execute_data;

	/* The separate stack used by fiber */
	zend_vm_stack stack;
	zval *first_frame;
	zval *stack_top;
	zval *stack_end;

	zval value;
	zval *send_target;

	zval *vars;
	int n_vars;

	zend_uchar status;
};

static const zend_uchar ZEND_FIBER_STATUS_UNINITED  = 0;
static const zend_uchar ZEND_FIBER_STATUS_SUSPENDED = 1;
static const zend_uchar ZEND_FIBER_STATUS_RUNNING   = 2;
static const zend_uchar ZEND_FIBER_STATUS_FINISHED  = 3;
static const zend_uchar ZEND_FIBER_STATUS_DEAD      = 4;

void zend_register_fiber_ce(void);
ZEND_API void zend_fiber_create_zval(zend_execute_data *call, zend_op_array *op_array, zval *return_value);
ZEND_API void zend_fiber_close(zend_fiber *fiber, zend_bool finished_execution);
ZEND_API void zend_fiber_resume(zend_fiber *fiber);

#define REGISTER_FIBER_CLASS_CONST_LONG(const_name, value) \
	zend_declare_class_constant_long(zend_ce_fiber, const_name, sizeof(const_name)-1, (zend_long)value);

void zend_fiber_yield_from(zend_fiber *fiber, zend_fiber *from);
ZEND_API zend_execute_data *zend_fiber_check_placeholder_frame(zend_execute_data *ptr);

END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
