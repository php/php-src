#ifndef PHP_COM_H
#define PHP_COM_H

#if PHP_WIN32

#include "com.h"

BEGIN_EXTERN_C()

PHP_MINIT_FUNCTION(COM);
PHP_MSHUTDOWN_FUNCTION(COM);

PHP_FUNCTION(com_load);
PHP_FUNCTION(com_invoke);
PHP_FUNCTION(com_invoke_ex);
PHP_FUNCTION(com_addref);
PHP_FUNCTION(com_release);
PHP_FUNCTION(com_propget);
PHP_FUNCTION(com_propput);
PHP_FUNCTION(com_load_typelib);
PHP_FUNCTION(com_isenum);
PHP_FUNCTION(com_event_sink);
PHP_FUNCTION(com_message_pump);
PHP_FUNCTION(com_print_typeinfo);

PHPAPI HRESULT php_COM_invoke(comval *obj, DISPID dispIdMember, WORD wFlags, DISPPARAMS FAR*  pDispParams, VARIANT FAR* pVarResult, char **ErrString TSRMLS_DC);
PHPAPI HRESULT php_COM_get_ids_of_names(comval *obj, OLECHAR FAR* FAR* rgszNames, DISPID FAR* rgDispId TSRMLS_DC);
PHPAPI HRESULT php_COM_release(comval *obj TSRMLS_DC);
PHPAPI HRESULT php_COM_addref(comval *obj TSRMLS_DC);
PHPAPI HRESULT php_COM_destruct(comval *obj TSRMLS_DC);
PHPAPI HRESULT php_COM_set(comval *obj, IDispatch FAR* FAR* pDisp, int cleanup TSRMLS_DC);
PHPAPI HRESULT php_COM_clone(comval *obj, comval *clone, int cleanup TSRMLS_DC);
PHPAPI char *php_COM_error_message(HRESULT hr TSRMLS_DC);
PHPAPI int php_COM_get_le_comval();
PHPAPI int php_COM_set_property_handler(zend_property_reference *property_reference, pval *value);
PHPAPI pval php_COM_get_property_handler(zend_property_reference *property_reference);
PHPAPI void php_COM_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference);
PHPAPI zval *php_COM_object_from_dispatch(IDispatch *disp, zval *val TSRMLS_DC);
PHPAPI int php_COM_load_typelib(ITypeLib *TypeLib, int mode TSRMLS_DC);

/* dispatch.c */
PHPAPI IDispatch *php_COM_export_object(zval *val TSRMLS_DC);
PHPAPI IDispatch *php_COM_export_as_sink(zval *val, GUID *sinkid, HashTable *id_to_name TSRMLS_DC);
int php_COM_dispatch_init(int module_number TSRMLS_DC);

zend_module_entry COM_module_entry;
zend_class_entry COM_class_entry;

#ifdef DEBUG
	extern int resourcecounter;
#endif

END_EXTERN_C()

#define phpext_com_ptr &COM_module_entry

ZEND_BEGIN_MODULE_GLOBALS(com)
	int nothing;
ZEND_END_MODULE_GLOBALS(com)

PHPAPI ZEND_EXTERN_MODULE_GLOBALS(com)
	
#ifdef ZTS
#define COMG(v)	TSRMG(com_globals_id, zend_com_globals *, v)
#else
#define COMG(v)	(com_globals.v)
#endif
	
#else

#define phpext_com_ptr NULL

#endif  /* PHP_WIN32 */

#endif  /* PHP_COM_H */
