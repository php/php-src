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
   | Authors: Levi Morrison <morrison.levi@gmail.com>                     |
   +----------------------------------------------------------------------+
 */

#ifndef ZEND_ATOMIC_H
#define ZEND_ATOMIC_H

#include "zend_portability.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C++" {
# include <atomic>
}
#else
# if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#  error "Zend requires C11 or later"
# endif
# ifdef __STDC_NO_ATOMICS__
#  error "Zend requires C11 atomics"
# endif
# include <stdatomic.h>
#endif

/* Treat zend_atomic_* types as opaque. They have definitions only for size
 * and alignment purposes.
 */

typedef struct zend_atomic_bool_s {
#ifdef __cplusplus
	std::atomic_bool value;
#else
	atomic_bool value;
#endif
} zend_atomic_bool;
typedef struct zend_atomic_int_s {
#ifdef __cplusplus
	std::atomic_int value;
#else
	atomic_int value;
#endif
} zend_atomic_int;

BEGIN_EXTERN_C()

/* The standard atomic operations use sequentially consistent ordering.
 * In C++, argument-dependent lookup selects the std::atomic overloads.
 */
#define ZEND_ATOMIC_BOOL_INIT(obj, desired) atomic_init(&(obj)->value, (bool) (desired))
#define ZEND_ATOMIC_INT_INIT(obj, desired)  atomic_init(&(obj)->value, (int) (desired))

#ifdef __cplusplus
# define ZEND_ATOMIC_BOOL_INITIALIZER(desired) {{(bool) (desired)}}
# define ZEND_ATOMIC_INT_INITIALIZER(desired)  {{(int) (desired)}}
#else
# define ZEND_ATOMIC_BOOL_INITIALIZER(desired) {.value = (desired)}
# define ZEND_ATOMIC_INT_INITIALIZER(desired)  {.value = (desired)}
#endif

static zend_always_inline bool zend_atomic_bool_exchange_ex(zend_atomic_bool *obj, bool desired) {
	return atomic_exchange(&obj->value, desired);
}

static zend_always_inline int zend_atomic_int_exchange_ex(zend_atomic_int *obj, int desired) {
	return atomic_exchange(&obj->value, desired);
}

static zend_always_inline bool zend_atomic_bool_compare_exchange_ex(zend_atomic_bool *obj, bool *expected, bool desired) {
	return atomic_compare_exchange_strong(&obj->value, expected, desired);
}

static zend_always_inline bool zend_atomic_int_compare_exchange_ex(zend_atomic_int *obj, int *expected, int desired) {
	return atomic_compare_exchange_strong(&obj->value, expected, desired);
}

static zend_always_inline bool zend_atomic_bool_load_ex(const zend_atomic_bool *obj) {
	return atomic_load(&obj->value);
}

static zend_always_inline int zend_atomic_int_load_ex(const zend_atomic_int *obj) {
	return atomic_load(&obj->value);
}

static zend_always_inline void zend_atomic_bool_store_ex(zend_atomic_bool *obj, bool desired) {
	atomic_store(&obj->value, desired);
}

static zend_always_inline void zend_atomic_int_store_ex(zend_atomic_int *obj, int desired) {
	atomic_store(&obj->value, desired);
}

ZEND_API void zend_atomic_bool_init(zend_atomic_bool *obj, bool desired);
ZEND_API void zend_atomic_int_init(zend_atomic_int *obj, int desired);

ZEND_API bool zend_atomic_bool_exchange(zend_atomic_bool *obj, bool desired);
ZEND_API int zend_atomic_int_exchange(zend_atomic_int *obj, int desired);

ZEND_API bool zend_atomic_bool_compare_exchange(zend_atomic_bool *obj, bool *expected, bool desired);
ZEND_API bool zend_atomic_int_compare_exchange(zend_atomic_int *obj, int *expected, int desired);

ZEND_API void zend_atomic_bool_store(zend_atomic_bool *obj, bool desired);
ZEND_API void zend_atomic_int_store(zend_atomic_int *obj, int desired);

ZEND_API bool zend_atomic_bool_load(const zend_atomic_bool *obj);
ZEND_API int zend_atomic_int_load(const zend_atomic_int *obj);

END_EXTERN_C()

#endif
