#ifndef PHP_TYPEDEF_VARIANT_H
#define PHP_TYPEDEF_VARIANT_H

#if PHP_WIN32

BEGIN_EXTERN_C()

#include "conversion.h"
#include "variant.h"

PHP_MINIT_FUNCTION(VARIANT);
PHP_MSHUTDOWN_FUNCTION(VARIANT);

PHPAPI int php_VARIANT_get_le_variant();

zend_module_entry VARIANT_module_entry;

END_EXTERN_C()

#define VARIANT_module_ptr &VARIANT_module_entry

#else

#define VARIANT_module_ptr NULL

#endif  /* PHP_WIN32 */

#define phpext_VARIANT_ptr VARIANT_module_ptr

#endif  /* PHP_TYPEDEF_VARIANT_H */
