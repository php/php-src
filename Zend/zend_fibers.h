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

#define ZEND_FIBER_GUARD_PAGES 1

#define ZEND_FIBER_DEFAULT_C_STACK_SIZE (4096 * (((sizeof(void *)) < 8) ? 256 : 512))
#define ZEND_FIBER_VM_STACK_SIZE (1024 * sizeof(zval))

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

typedef enum {
	ZEND_FIBER_TRANSFER_FLAG_ERROR = 1 << 0,
} zend_fiber_transfer_flag;

void zend_register_fiber_ce(void);
void zend_fiber_init(void);
void zend_fiber_shutdown(void);

extern ZEND_API zend_class_entry *zend_ce_fiber;

typedef struct _zend_fiber zend_fiber;
typedef struct _zend_fiber_context zend_fiber_context;
typedef struct _zend_fiber_stack zend_fiber_stack;

/* Encapsulates data needed for a context switch. */
typedef struct _zend_fiber_transfer {
	/* Fiber that will be switched to / has resumed us. */
	zend_fiber_context *context;
	/* Value to that should be send to (or was received from) a fiber. */
	zval value;
	/* Bitmask of flags defined in enum zend_fiber_transfer_flag. */
	uint8_t flags;
} zend_fiber_transfer;

/* Coroutine functions must populate the given transfer with a new context
 * and (optional) data before they return. */
typedef void (*zend_fiber_coroutine)(zend_fiber_transfer *transfer);

struct _zend_fiber_context {
	/* Handle to fiber state as needed by boost.context */
	void *handle;
	/* Pointer that identifies the fiber type. */
	void *kind;
	zend_fiber_coroutine function;
	zend_fiber_stack *stack;
	zend_fiber_status status;
	uint8_t flags;
};

/* Zend VM state that needs to be captured / restored during fiber context switch. */
typedef struct _zend_fiber_vm_state {
	zend_vm_stack vm_stack;
	zval *vm_stack_top;
	zval *vm_stack_end;
	size_t vm_stack_page_size;
	zend_execute_data *current_execute_data;
	int error_reporting;
	uint32_t jit_trace_num;
	JMP_BUF *bailout;
	zend_fiber *active_fiber;
} zend_fiber_vm_state;

struct _zend_fiber {
	/* PHP object handle. */
	zend_object std;

	zend_fiber_context context;

	/* Fiber that resumed us. */
	zend_fiber_context *caller;

	/* Fiber that suspended us. */
	zend_fiber_context *previous;

	/* Callback and info / cache to be used when fiber is started. */
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	/* Current Zend VM execute data being run by the fiber. */
	zend_execute_data *execute_data;

	/* Frame on the bottom of the fiber vm stack. */
	zend_execute_data *stack_bottom;

	/* Storage for fiber return value. */
	zval result;
};

/* These functions may be used to create custom fiber objects using the bundled fiber switching context. */
ZEND_API bool zend_fiber_init_context(zend_fiber_context *context, void *kind, zend_fiber_coroutine coroutine, size_t stack_size);
ZEND_API void zend_fiber_destroy_context(zend_fiber_context *context);
ZEND_API void zend_fiber_switch_context(zend_fiber_transfer *transfer);

ZEND_API void zend_fiber_switch_block(void);
ZEND_API void zend_fiber_switch_unblock(void);
ZEND_API bool zend_fiber_switch_blocked(void);

END_EXTERN_C()

static zend_always_inline zend_fiber *zend_fiber_from_context(zend_fiber_context *context)
{
	ZEND_ASSERT(context->kind == zend_ce_fiber && "Fiber context does not belong to a Zend fiber");

	return (zend_fiber *)(((char *) context) - XtOffsetOf(zend_fiber, context));
}

static zend_always_inline zend_fiber_context *zend_fiber_get_context(zend_fiber *fiber)
{
	return &fiber->context;
}

static zend_always_inline void zend_fiber_capture_vm_state(zend_fiber_vm_state *state)
{
	state->vm_stack = EG(vm_stack);
	state->vm_stack_top = EG(vm_stack_top);
	state->vm_stack_end = EG(vm_stack_end);
	state->vm_stack_page_size = EG(vm_stack_page_size);
	state->current_execute_data = EG(current_execute_data);
	state->error_reporting = EG(error_reporting);
	state->jit_trace_num = EG(jit_trace_num);
	state->bailout = EG(bailout);
	state->active_fiber = EG(active_fiber);
}

static zend_always_inline void zend_fiber_restore_vm_state(zend_fiber_vm_state *state)
{
	EG(vm_stack) = state->vm_stack;
	EG(vm_stack_top) = state->vm_stack_top;
	EG(vm_stack_end) = state->vm_stack_end;
	EG(vm_stack_page_size) = state->vm_stack_page_size;
	EG(current_execute_data) = state->current_execute_data;
	EG(error_reporting) = state->error_reporting;
	EG(jit_trace_num) = state->jit_trace_num;
	EG(bailout) = state->bailout;
	EG(active_fiber) = state->active_fiber;
}

#endif
