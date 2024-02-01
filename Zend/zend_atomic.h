/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Levi Morrison <morrison.levi@gmail.com>                     |
   +----------------------------------------------------------------------+
 */

#ifndef ZEND_ATOMIC_H
#define ZEND_ATOMIC_H

#include "zend_portability.h"

#include <stdbool.h>

#define ZEND_GCC_PREREQ(x, y) \
	((__GNUC__ == (x) && __GNUC_MINOR__ >= (y)) || (__GNUC__ > (x)))

/* Builtins are used to avoid library linkage */
#if __has_feature(c_atomic) && defined(__clang__)
#define	HAVE_C11_ATOMICS 1
#elif ZEND_GCC_PREREQ(4, 7)
#define	HAVE_GNUC_ATOMICS 1
#elif defined(__GNUC__)
#define	HAVE_SYNC_ATOMICS 1
#elif !defined(ZEND_WIN32)
#define HAVE_NO_ATOMICS 1
#endif

#undef ZEND_GCC_PREREQ

/* Treat zend_atomic_* types as opaque. They have definitions only for size
 * and alignment purposes.
 */

#if defined(ZEND_WIN32) || defined(HAVE_SYNC_ATOMICS)
typedef struct zend_atomic_bool_s {
	volatile char value;
} zend_atomic_bool;
#elif defined(HAVE_C11_ATOMICS)
typedef struct zend_atomic_bool_s {
	_Atomic(bool) value;
} zend_atomic_bool;
#else
typedef struct zend_atomic_bool_s {
	volatile bool value;
} zend_atomic_bool;
#endif

BEGIN_EXTERN_C()

#ifdef ZEND_WIN32

#ifndef InterlockedExchange8
#define InterlockedExchange8 _InterlockedExchange8
#endif
#ifndef InterlockedOr8
#define InterlockedOr8 _InterlockedOr8
#endif

#define ZEND_ATOMIC_BOOL_INIT(obj, desired) ((obj)->value = (desired))

static zend_always_inline bool zend_atomic_bool_exchange_ex(zend_atomic_bool *obj, bool desired) {
	return InterlockedExchange8(&obj->value, desired);
}

/* On this platform it is non-const due to Iterlocked API*/
static zend_always_inline bool zend_atomic_bool_load_ex(zend_atomic_bool *obj) {
	/* Or'ing with false won't change the value. */
	return InterlockedOr8(&obj->value, false);
}

static zend_always_inline void zend_atomic_bool_store_ex(zend_atomic_bool *obj, bool desired) {
	(void)InterlockedExchange8(&obj->value, desired);
}

#elif defined(HAVE_C11_ATOMICS)

#define ZEND_ATOMIC_BOOL_INIT(obj, desired) __c11_atomic_init(&(obj)->value, (desired))

static zend_always_inline bool zend_atomic_bool_exchange_ex(zend_atomic_bool *obj, bool desired) {
	return __c11_atomic_exchange(&obj->value, desired, __ATOMIC_SEQ_CST);
}

static zend_always_inline bool zend_atomic_bool_load_ex(const zend_atomic_bool *obj) {
	return __c11_atomic_load(&obj->value, __ATOMIC_SEQ_CST);
}

static zend_always_inline void zend_atomic_bool_store_ex(zend_atomic_bool *obj, bool desired) {
	__c11_atomic_store(&obj->value, desired, __ATOMIC_SEQ_CST);
}

#elif defined(HAVE_GNUC_ATOMICS)

#define ZEND_ATOMIC_BOOL_INIT(obj, desired) ((obj)->value = (desired))

static zend_always_inline bool zend_atomic_bool_exchange_ex(zend_atomic_bool *obj, bool desired) {
	bool prev = false;
	__atomic_exchange(&obj->value, &desired, &prev, __ATOMIC_SEQ_CST);
	return prev;
}

static zend_always_inline bool zend_atomic_bool_load_ex(const zend_atomic_bool *obj) {
	bool prev = false;
	__atomic_load(&obj->value, &prev, __ATOMIC_SEQ_CST);
	return prev;
}

static zend_always_inline void zend_atomic_bool_store_ex(zend_atomic_bool *obj, bool desired) {
	__atomic_store(&obj->value, &desired, __ATOMIC_SEQ_CST);
}

#elif defined(HAVE_SYNC_ATOMICS)

#define ZEND_ATOMIC_BOOL_INIT(obj, desired) ((obj)->value = (desired))

static zend_always_inline bool zend_atomic_bool_exchange_ex(zend_atomic_bool *obj, bool desired) {
	bool prev = __sync_lock_test_and_set(&obj->value, desired);

	/* __sync_lock_test_and_set only does an acquire barrier, so sync
	 * immediately after.
	 */
	__sync_synchronize();
	return prev;
}

static zend_always_inline bool zend_atomic_bool_load_ex(zend_atomic_bool *obj) {
	/* Or'ing false won't change the value */
	return __sync_fetch_and_or(&obj->value, false);
}

static zend_always_inline void zend_atomic_bool_store_ex(zend_atomic_bool *obj, bool desired) {
	__sync_synchronize();
	obj->value = desired;
	__sync_synchronize();
}

#elif defined(HAVE_NO_ATOMICS)

#warning No atomics support detected. Please open an issue with platform details.

#define ZEND_ATOMIC_BOOL_INIT(obj, desired) ((obj)->value = (desired))

static zend_always_inline void zend_atomic_bool_store_ex(zend_atomic_bool *obj, bool desired) {
	obj->value = desired;
}

static zend_always_inline bool zend_atomic_bool_load_ex(const zend_atomic_bool *obj) {
	return obj->value;
}

static zend_always_inline bool zend_atomic_bool_exchange_ex(zend_atomic_bool *obj, bool desired) {
	bool prev = obj->value;
	obj->value = desired;
	return prev;
}

#endif

ZEND_API void zend_atomic_bool_init(zend_atomic_bool *obj, bool desired);
ZEND_API bool zend_atomic_bool_exchange(zend_atomic_bool *obj, bool desired);
ZEND_API void zend_atomic_bool_store(zend_atomic_bool *obj, bool desired);

#if defined(ZEND_WIN32) || defined(HAVE_SYNC_ATOMICS)
/* On these platforms it is non-const due to underlying APIs. */
ZEND_API bool zend_atomic_bool_load(zend_atomic_bool *obj);
#else
ZEND_API bool zend_atomic_bool_load(const zend_atomic_bool *obj);
#endif

END_EXTERN_C()

#endif
