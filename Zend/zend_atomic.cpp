// TODO: license

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
