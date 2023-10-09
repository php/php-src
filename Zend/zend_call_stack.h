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
   | Authors: Arnaud Le Blanc <arnaud.lb@gmail.com>                       |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_CALL_STACK_H
#define ZEND_CALL_STACK_H

#include "zend.h"
#include "zend_portability.h"
#ifdef __APPLE__
# include <pthread.h>
#endif

#ifdef ZEND_CHECK_STACK_LIMIT

typedef struct _zend_call_stack {
	void *base;
	size_t max_size;
} zend_call_stack;

ZEND_API void zend_call_stack_init(void);

ZEND_API bool zend_call_stack_get(zend_call_stack *stack);

/** Returns an approximation of the current stack position */
static zend_always_inline void *zend_call_stack_position(void) {
#ifdef ZEND_WIN32
	return _AddressOfReturnAddress();
#elif PHP_HAVE_BUILTIN_FRAME_ADDRESS
	return __builtin_frame_address(0);
#else
	void *a;
	void *pos = (void*)&a;
	return pos;
#endif
}

static zend_always_inline bool zend_call_stack_overflowed(void *stack_limit) {
	return (uintptr_t) zend_call_stack_position() <= (uintptr_t) stack_limit;
}

static inline void* zend_call_stack_limit(void *base, size_t size, size_t reserved_size)
{
	if (UNEXPECTED(size > (uintptr_t)base)) {
		return (void*)0;
	}

	base = (int8_t*)base - size;

	if (UNEXPECTED(UINTPTR_MAX - (uintptr_t)base < reserved_size)) {
		return (void*)UINTPTR_MAX;
	}

	return (int8_t*)base + reserved_size;
}

static inline size_t zend_call_stack_default_size(void)
{
#ifdef __linux__
	return 8 * 1024 * 1024;
#endif
#if defined(__FreeBSD__) || defined(__NetBSD__)
	return 4 * 1024 * 1024;
#endif
#if defined(__DragonFly__)
	return 2 * 1024 * 1024;
#endif
#ifdef __OpenBSD__
	return 512 * 1024;
#endif
#ifdef __APPLE__
	// https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/Multithreading/CreatingThreads/CreatingThreads.html
	if (pthread_main_np()) {
		return 8 * 1024 * 1024;
	}
	return 512 * 1024;
#endif
#ifdef __HAIKU__
	return 64 * 4096;
#endif

	return 2 * 1024 * 1024;
}

#endif /* ZEND_CHECK_STACK_LIMIT */
#endif /* ZEND_CALL_STACK_H */
