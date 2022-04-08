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

#include "zend_atomic.h"

#if !defined(ZEND_WIN32)
#error This implementation is meant for Windows only. Please refer to zend_atomic.c.
#endif

#include <atomic>
using std::atomic_bool;

static_assert(sizeof(atomic_bool) == sizeof(bool), "Repr of atomic_bool and bool must match");
static_assert(alignof(atomic_bool) == alignof(bool), "Repr of atomic_bool and bool must match");

ZEND_API bool zend_atomic_bool_exchange(zend_atomic_bool *obj, bool desired) {
	return std::atomic_exchange((atomic_bool *)obj, desired);
}

ZEND_API bool zend_atomic_bool_load(const zend_atomic_bool *obj) {
	return std::atomic_load((const atomic_bool *)obj);
}

ZEND_API void zend_atomic_bool_store(zend_atomic_bool *obj, bool desired) {
	std::atomic_store((atomic_bool *)obj, desired);
}
