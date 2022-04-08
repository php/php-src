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
