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
*/

#ifndef ZEND_SANITIZERS_H
#define ZEND_SANITIZERS_H

#include "zend_portability.h"

#ifdef __SANITIZE_ADDRESS__
# include <sanitizer/asan_interface.h>
#else
# define ASAN_POISON_MEMORY_REGION(_ptr, _size)
# define ASAN_UNPOISON_MEMORY_REGION(_ptr, _size)
#endif

#if __has_feature(memory_sanitizer)
# include <sanitizer/msan_interface.h>
# define MSAN_POISON_MEMORY_REGION(_ptr, _size)   __msan_allocated_memory(_ptr, _size)
# define MSAN_UNPOISON_MEMORY_REGION(_ptr, _size) __msan_unpoison(_ptr, _size)
#else
# define MSAN_POISON_MEMORY_REGION(_ptr, _size)
# define MSAN_UNPOISON_MEMORY_REGION(_ptr, _size)
#endif

/* Mark memory region as unaddressable (ASAN) and uninitialized (MSAN) */
#define ZEND_POISON_MEMORY_REGION(_ptr, _size) do {     \
	ZEND_ASSERT(!(((uintptr_t) (_ptr)) & 7));           \
	ASAN_POISON_MEMORY_REGION((_ptr), (_size));         \
	MSAN_POISON_MEMORY_REGION((_ptr), (_size));         \
} while (0);

/* Mark memory region as addressable (ASAN) without changing initialization state (MSAN) */
#define ZEND_UNPOISON_MEMORY_REGION(_ptr, _size) do {   \
	ZEND_ASSERT(!(((uintptr_t) (_ptr)) & 7));           \
	ASAN_UNPOISON_MEMORY_REGION((_ptr), (_size));       \
	/* No MSAN_UNPOISON_MEMORY_REGION */                \
} while (0);

#endif /* ZEND_SANITIZERS_H */
