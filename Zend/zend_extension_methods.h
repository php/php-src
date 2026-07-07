/* Prototype: Swift-style extension methods registry (RFC draft).
 * Registry maps lc(target class name) -> HashTable of lc(method) -> zend_function*.
 */
#ifndef ZEND_EXTENSION_METHODS_H
#define ZEND_EXTENSION_METHODS_H

#include "zend.h"

BEGIN_EXTERN_C()

void zend_extension_methods_startup(void);
void zend_extension_methods_shutdown(void);

/* Called when an `extension Target { ... }` block's synthetic CE is linked. */
ZEND_API void zend_extension_methods_register(zend_string *target_lc, zend_class_entry *ext_ce);

/* Fallback lookup: walks ce and its ancestry/interfaces for a registered method. */
ZEND_API zend_function *zend_extension_methods_get(const zend_class_entry *ce, zend_string *lc_method_name);

END_EXTERN_C()

#endif
