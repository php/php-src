#ifndef PHP_COM_H
#define PHP_COM_H

#if PHP_WIN32

BEGIN_EXTERN_C()

#include "com.h"

extern PHP_MINIT_FUNCTION(COM);
extern PHP_MSHUTDOWN_FUNCTION(COM);

extern int php_COM_get_le_idispatch();

extern zend_module_entry COM_module_entry;
extern zend_class_entry com_class_entry;

extern pval php_COM_get_property_handler(zend_property_reference *property_reference);
extern int php_COM_set_property_handler(zend_property_reference *property_reference, pval *value);
extern char *php_COM_error_message(HRESULT hr);
extern void php_COM_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference);

END_EXTERN_C()

#define COM_module_ptr &COM_module_entry

#else

#define COM_module_ptr NULL

#endif  /* PHP_WIN32 */

#endif  /* PHP_COM_H */
