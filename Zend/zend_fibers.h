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

#define ZEND_FIBER_C_STACK_ALIGNMENT (4 * 1024)
#define ZEND_FIBER_DEFAULT_C_STACK_SIZE (ZEND_FIBER_C_STACK_ALIGNMENT * (sizeof(void *) * 64))
#define ZEND_FIBER_MIN_C_STACK_SIZE (128 * 1024)
#define ZEND_FIBER_MAX_C_STACK_SIZE (16 * 1024 * 1024)

#define ZEND_FIBER_DEFAULT_VM_STACK_SIZE (sizeof(zval) * 1024)

#define ZEND_FIBER_GUARD_PAGES 1

extern ZEND_API zend_class_entry *zend_ce_fiber;

typedef struct _zend_fiber zend_fiber;
typedef struct _zend_fiber_context zend_fiber_context;
typedef struct _zend_fiber_executor zend_fiber_executor;

#define ZEND_FIBER_STATUS_MAP(XX) \
	XX(INIT,      0) \
	XX(RUNNING,   1) \
	XX(SUSPENDED, 2) \
	XX(DEAD,      3) \

typedef enum {
#define ZEND_FIBER_STATUS_GEN(name, value) ZEND_FIBER_STATUS_##name = (value),
	ZEND_FIBER_STATUS_MAP(ZEND_FIBER_STATUS_GEN)
#undef ZEND_FIBER_STATUS_GEN
} zend_fiber_status;

#define ZEND_FIBER_FLAG_MAP(XX) \
	XX(THREW,     1 << 0) \
	XX(BAILOUT,   1 << 1) \
	XX(DESTROYED, 1 << 2) \

typedef enum {
#define ZEND_FIBER_FLAG_GEN(name, value) ZEND_FIBER_FLAG_##name = (value),
	ZEND_FIBER_FLAG_MAP(ZEND_FIBER_FLAG_GEN)
#undef ZEND_FIBER_FLAG_GEN
} zend_fiber_flag;

struct _zend_fiber_context {
	void *ptr;
	void *stack;
	size_t stack_size;
#ifdef HAVE_VALGRIND
	unsigned int valgrind_stack_id;
#endif
#ifdef __SANITIZE_ADDRESS__
	void *asan_fake_stack;
	const void *asan_stack_bottom;
	size_t asan_stack_size;
#endif
};

struct _zend_fiber_executor
{
	JMP_BUF *bailout;
	int error_reporting;
	int exit_status;
	zval *vm_stack_top;
	zval *vm_stack_end;
	zend_vm_stack vm_stack;
	size_t vm_stack_page_size;
	zend_execute_data *current_execute_data;
	uint32_t jit_trace_num;
};

/* unrelated fields on executor_globals may be overwritten
 * if the we use aligned size, so we define unaligned size here. */
#define ZEND_FIBER_EXECUTOR_UNALIGNED_SIZE (XtOffsetOf(zend_fiber_executor, jit_trace_num) + sizeof(uint32_t))

ZEND_STATIC_ASSERT(ZEND_FIBER_EXECUTOR_UNALIGNED_SIZE ==
	XtOffsetOf(zend_executor_globals, jit_trace_num) -
	XtOffsetOf(zend_executor_globals, bailout) + sizeof(uint32_t)
);

struct _zend_fiber {
	/* Fiber PHP object handle. */
	zend_object std;
	/* The one who resumed this fiber */
	zend_fiber *from;
	/* Previous one, the target when we suspend. */
	zend_fiber *previous;
	/* Status of the fiber, one of the ZEND_FIBER_STATUS_* constants. */
	zend_uchar status;
	/* Flags of the fiber, collection of the ZEND_FIBER_FLAG_* constants. */
	zend_uchar flags;
	/* Callback and info / cache to be used when fiber is started. */
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	/* Context of this fiber, will be initialized during call to Fiber::start(). */
	zend_fiber_context context;
	/* Executor globals. */
	zend_fiber_executor *executor;
	/* Root execute data. */
	zend_execute_data *execute_data;
	/* Storage for fiber return value. */
	zval retval;
};

void zend_register_fiber_ce(void);

void zend_fiber_init(void);
void zend_fiber_shutdown(void);

/* These functions create and manipulate a Fiber object, allowing any internal function to start, resume, or suspend a fiber. */
ZEND_API zend_fiber *zend_fiber_create(const zend_fcall_info *fci, const zend_fcall_info_cache *fci_cache);
ZEND_API bool zend_fiber_start(zend_fiber *fiber, zval *data, zval *retval);
ZEND_API void zend_fiber_jump(zend_fiber *fiber, zval *data, zval *retval);
ZEND_API bool zend_fiber_resume(zend_fiber *fiber, zval *data, zval *retval);
ZEND_API bool zend_fiber_suspend(zval *data, zval *retval);
ZEND_API bool zend_fiber_throw(zend_fiber *fiber, zend_object *exception, zval *retval);

END_EXTERN_C()

#endif
