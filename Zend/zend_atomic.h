// TODO: license

#ifndef ZEND_ATOMIC_H
#define ZEND_ATOMIC_H

#include "zend_portability.h"

#include <stdbool.h>

/* These functions correspond to C11's stdatomic.h functions but are type-
 * specialized because it's difficult to provide portable routines such as
 * exchange without making functions.
 *
 * Treat zend_atomic_* types as opaque. They have definitions only for size
 * and alignment purposes.
 */

typedef struct zend_atomic_bool_s {
	volatile bool bytes;
} zend_atomic_bool;

BEGIN_EXTERN_C()

ZEND_API bool zend_atomic_bool_load(const zend_atomic_bool *obj);
ZEND_API void zend_atomic_bool_store(zend_atomic_bool *obj, bool desired);
ZEND_API bool zend_atomic_bool_exchange(zend_atomic_bool *obj, bool desired);

END_EXTERN_C()

#endif
