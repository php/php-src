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

#include "zend_atomic.h"

/* This file contains the non-inline copy of atomic functions. This is useful
 * for extensions written in languages such as Rust. C and C++ compilers are
 * probably going to inline these functions, but in the case they don't, this
 * is also where the code will go.
 */

/* Defined for FFI users; everyone else use ZEND_ATOMIC_*_INIT.
 * This is NOT ATOMIC as it is meant for initialization.
 */
ZEND_API void zend_atomic_bool_init(zend_atomic_bool *obj, bool desired) {
	ZEND_ATOMIC_BOOL_INIT(obj, desired);
}

ZEND_API void zend_atomic_int_init(zend_atomic_int *obj, int desired) {
	ZEND_ATOMIC_INT_INIT(obj, desired);
}

ZEND_API bool zend_atomic_bool_exchange(zend_atomic_bool *obj, bool desired) {
	return zend_atomic_bool_exchange_ex(obj, desired);
}

ZEND_API int zend_atomic_int_exchange(zend_atomic_int *obj, int desired) {
	return zend_atomic_int_exchange_ex(obj, desired);
}

ZEND_API bool zend_atomic_bool_compare_exchange(zend_atomic_bool *obj, bool *expected, bool desired)
{
	return zend_atomic_bool_compare_exchange_ex(obj, expected, desired);
}

ZEND_API bool zend_atomic_int_compare_exchange(zend_atomic_int *obj, int *expected, int desired)
{
	return zend_atomic_int_compare_exchange_ex(obj, expected, desired);
}

ZEND_API void zend_atomic_bool_store(zend_atomic_bool *obj, bool desired) {
	zend_atomic_bool_store_ex(obj, desired);
}

ZEND_API void zend_atomic_int_store(zend_atomic_int *obj, int desired) {
	zend_atomic_int_store_ex(obj, desired);
}

#if defined(ZEND_WIN32) || defined(HAVE_SYNC_ATOMICS)
/* On these platforms it is non-const due to underlying APIs. */
ZEND_API bool zend_atomic_bool_load(zend_atomic_bool *obj) {
	return zend_atomic_bool_load_ex(obj);
}
ZEND_API int zend_atomic_int_load(zend_atomic_int *obj) {
	return zend_atomic_int_load_ex(obj);
}
#else
ZEND_API bool zend_atomic_bool_load(const zend_atomic_bool *obj) {
	return zend_atomic_bool_load_ex(obj);
}
ZEND_API int zend_atomic_int_load(const zend_atomic_int *obj) {
	return zend_atomic_int_load_ex(obj);
}
#endif
