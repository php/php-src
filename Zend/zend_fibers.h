/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Aaron Piotrowski <aaron@trowski.com>                        |
   |          Martin Schröder <m.schroeder2007@gmail.com>                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_FIBERS_H
#define ZEND_FIBERS_H

#include "zend_API.h"
#include "zend_types.h"

BEGIN_EXTERN_C()

void zend_register_fiber_ce(void);
void zend_fiber_init(void);

extern ZEND_API zend_class_entry *zend_ce_fiber;

typedef struct _zend_fiber_context zend_fiber_context;

typedef void (*zend_fiber_coroutine)(zend_fiber_context *context);

typedef struct _zend_fiber_stack {
	void *pointer;
	size_t size;

#ifdef HAVE_VALGRIND
	int valgrind;
#endif
} zend_fiber_stack;

typedef struct _zend_fiber_context {
	void *self;
	void *caller;
	zend_fiber_coroutine function;
	zend_fiber_stack stack;
} zend_fiber_context;

typedef struct _zend_fiber {
	/* Fiber PHP object handle. */
	zend_object std;

	/* Status of the fiber, one of the ZEND_FIBER_STATUS_* constants. */
	zend_uchar status;

	/* Callback and info / cache to be used when fiber is started. */
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	/* Context of this fiber, will be initialized during call to Fiber::start(). */
	zend_fiber_context context;

	/* Current Zend VM execute data being run by the fiber. */
	zend_execute_data *execute_data;

	/* Frame on the bottom of the fiber vm stack. */
	zend_execute_data *stack_bottom;

	/* Exception to be thrown from Fiber::suspend(). */
	zval *exception;

	/* Storage for temporaries and fiber return value. */
	zval value;
} zend_fiber;

static const zend_uchar ZEND_FIBER_STATUS_INIT      = 0x0;
static const zend_uchar ZEND_FIBER_STATUS_SUSPENDED = 0x1;
static const zend_uchar ZEND_FIBER_STATUS_RUNNING   = 0x2;
static const zend_uchar ZEND_FIBER_STATUS_RETURNED  = 0x4;
static const zend_uchar ZEND_FIBER_STATUS_THREW     = 0x8;
static const zend_uchar ZEND_FIBER_STATUS_SHUTDOWN  = 0x10;

static const zend_uchar ZEND_FIBER_STATUS_FINISHED  = 0x1c;

ZEND_API zend_bool zend_fiber_init_context(zend_fiber_context *context, zend_fiber_coroutine coroutine, size_t stack_size);
ZEND_API void zend_fiber_destroy_context(zend_fiber_context *context);

ZEND_COLD void zend_error_suspend_fiber(
		int orig_type, zend_string *error_filename, uint32_t error_lineno, zend_string *message);

ZEND_API void zend_fiber_switch_context(zend_fiber_context *to);
ZEND_API void zend_fiber_suspend_context(zend_fiber_context *current);

#define ZEND_FIBER_GUARD_PAGES 1

#define ZEND_FIBER_DEFAULT_C_STACK_SIZE (4096 * (((sizeof(void *)) < 8) ? 256 : 512))

#define ZEND_FIBER_VM_STACK_SIZE (1024 * sizeof(zval))

END_EXTERN_C()

#endif
