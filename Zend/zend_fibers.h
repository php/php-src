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

typedef enum {
	ZEND_FIBER_STATUS_INIT,
	ZEND_FIBER_STATUS_RUNNING,
	ZEND_FIBER_STATUS_SUSPENDED,
	ZEND_FIBER_STATUS_DEAD,
} zend_fiber_status;

typedef enum {
	ZEND_FIBER_FLAG_THREW     = 1 << 0,
	ZEND_FIBER_FLAG_BAILOUT   = 1 << 1,
	ZEND_FIBER_FLAG_DESTROYED = 1 << 2,
} zend_fiber_flag;

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

#ifdef __SANITIZE_ADDRESS__
	const void *prior_pointer;
	size_t prior_size;
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

	/* Status of the fiber, one of the zend_fiber_status values. */
	zend_fiber_status status;

	/* Flags of the fiber, bit field of the zend_fiber_flag values. */
	zend_uchar flags;

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

/* These functions create and manipulate a Fiber object, allowing any internal function to start, resume, or suspend a fiber. */
ZEND_API zend_fiber *zend_fiber_create(const zend_fcall_info *fci, const zend_fcall_info_cache *fci_cache);
ZEND_API void zend_fiber_start(zend_fiber *fiber, zval *params, uint32_t param_count, zend_array *named_params, zval *return_value);
ZEND_API void zend_fiber_suspend(zval *value, zval *return_value);
ZEND_API void zend_fiber_resume(zend_fiber *fiber, zval *value, zval *return_value);
ZEND_API void zend_fiber_throw(zend_fiber *fiber, zval *exception, zval *return_value);

/* These functions may be used to create custom fibers (coroutines) using the bundled fiber switching context. */
ZEND_API zend_bool zend_fiber_init_context(zend_fiber_context *context, zend_fiber_coroutine coroutine, size_t stack_size);
ZEND_API void zend_fiber_destroy_context(zend_fiber_context *context);
ZEND_API void zend_fiber_switch_context(zend_fiber_context *to);
ZEND_API void zend_fiber_suspend_context(zend_fiber_context *current);

#define ZEND_FIBER_GUARD_PAGES 1

#define ZEND_FIBER_DEFAULT_C_STACK_SIZE (4096 * (((sizeof(void *)) < 8) ? 256 : 512))

#define ZEND_FIBER_VM_STACK_SIZE (1024 * sizeof(zval))

END_EXTERN_C()

#endif
