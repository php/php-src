#ifndef PHP_COM_H
#define PHP_COM_H

#if PHP_WIN32

#include "com.h"

BEGIN_EXTERN_C()

extern PHP_MINIT_FUNCTION(COM);
extern PHP_MSHUTDOWN_FUNCTION(COM);

extern PHPAPI HRESULT php_COM_invoke(comval *obj, DISPID dispIdMember, WORD wFlags, DISPPARAMS FAR*  pDispParams, VARIANT FAR* pVarResult);
extern PHPAPI HRESULT php_COM_get_ids_of_names(comval *obj, OLECHAR FAR* FAR* rgszNames, DISPID FAR* rgDispId);
extern PHPAPI HRESULT php_COM_release(comval *obj);
extern PHPAPI HRESULT php_COM_addref(comval *obj);
extern PHPAPI HRESULT php_COM_destruct(comval *obj);
extern PHPAPI HRESULT php_COM_set(comval *obj, IDispatch FAR* pDisp, int cleanup);
extern PHPAPI HRESULT php_COM_clone(comval *obj, comval *clone, int cleanup);

extern int php_COM_get_le_comval();

extern zend_module_entry COM_module_entry;
extern zend_class_entry com_class_entry;

extern pval php_COM_get_property_handler(zend_property_reference *property_reference);
extern int php_COM_set_property_handler(zend_property_reference *property_reference, pval *value);
extern char *php_COM_error_message(HRESULT hr);
extern void php_COM_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference);

#ifdef DEBUG
	extern int resourcecounter;
#endif

END_EXTERN_C()

#define COM_module_ptr &COM_module_entry

#else

#define COM_module_ptr NULL

#endif  /* PHP_WIN32 */

#endif  /* PHP_COM_H */
