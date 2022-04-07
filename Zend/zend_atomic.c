// TODO: license

#include "zend_atomic.h"

/* The general strategy here is to use C11's stdatomic.h functions when they
 * are available, and then fall-back to non-atomic operations, except for
 * Windows, which has C++ std::atomic_bool but not C atomic_bool.
 *
 * The defines HAVE_ATOMIC_BOOL should only be set when it is determined that
 * representations of atomic_bool and bool are the same, as this is relied on
 * in the implementation.
 */

#if HAVE_STDATOMIC_H
#include <stdatomic.h>
#define HAVE_ATOMIC_BOOL 1
#elif ZEND_WIN32
#include <atomic>
using std::atomic_bool;
using std::atomic_exchange;
using std::atomic_load;
using std::atomic_store;
static_assert(sizeof(atomic_bool) == sizeof(bool), "Repr of atomic_bool and bool must match");
static_assert(alignof(atomic_bool) == alignof(bool), "Repr of atomic_bool and bool must match");
#define HAVE_ATOMIC_BOOL 1
#else
#define HAVE_ATOMIC_BOOL 0
#endif

#if HAVE_ATOMIC_BOOL

ZEND_API bool zend_atomic_bool_exchange(zend_atomic_bool *obj, bool desired) {
	return atomic_exchange((atomic_bool *)obj, desired);
}

ZEND_API bool zend_atomic_bool_load(const zend_atomic_bool *obj) {
	return atomic_load((const atomic_bool *)obj);
}

ZEND_API void zend_atomic_bool_store(zend_atomic_bool *obj, bool desired) {
	atomic_store((atomic_bool *)obj, desired);
}

#else

/* Yes, these are not guaranteed to be atomic. Understand that previously
 * atomics were never used, so the fact they are sometimes used is an
 * improvement. As more platforms support C11 atomics, or as we add support
 * for more platforms through intrinsics/asm, this should be used less and
 * less until it can be removed.
 */

ZEND_API bool zend_atomic_bool_exchange(zend_atomic_bool *obj, bool desired) {
	bool previous = obj->bytes;
	obj->bytes = desired;
	return previous;
}

ZEND_API bool zend_atomic_bool_load(const zend_atomic_bool *obj) {
	return obj->bytes;
}

ZEND_API void zend_atomic_bool_store(zend_atomic_bool *obj, bool desired) {
	obj->bytes = desired;
}

#endif
