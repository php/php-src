#ifndef PHP_TYPEDEF_VARIANT_H
#define PHP_TYPEDEF_VARIANT_H

#if PHP_WIN32

BEGIN_EXTERN_C()

#include "conversion.h"
#include "variant.h"

void php_VARIANT_init(int module_number TSRMLS_DC);

PHPAPI int php_VARIANT_get_le_variant();

END_EXTERN_C()

#endif  /* PHP_WIN32 */

#endif  /* PHP_TYPEDEF_VARIANT_H */
