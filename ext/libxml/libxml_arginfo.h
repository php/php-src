/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 1cc89432b61dc1997afe96e02f82a8fd1a6fcfc4 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_libxml_set_streams_context, 0, 1, IS_VOID, 0)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_libxml_use_internal_errors, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, use_errors, _IS_BOOL, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_libxml_get_last_error, 0, 0, LibXMLError, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_libxml_get_errors, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_libxml_clear_errors, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_libxml_disable_entity_loader, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, disable, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_libxml_set_external_entity_loader, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, resolver_function, IS_CALLABLE, 1)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(libxml_set_streams_context);
ZEND_FUNCTION(libxml_use_internal_errors);
ZEND_FUNCTION(libxml_get_last_error);
ZEND_FUNCTION(libxml_get_errors);
ZEND_FUNCTION(libxml_clear_errors);
ZEND_FUNCTION(libxml_disable_entity_loader);
ZEND_FUNCTION(libxml_set_external_entity_loader);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(libxml_set_streams_context, arginfo_libxml_set_streams_context)
	ZEND_FE(libxml_use_internal_errors, arginfo_libxml_use_internal_errors)
	ZEND_FE(libxml_get_last_error, arginfo_libxml_get_last_error)
	ZEND_FE(libxml_get_errors, arginfo_libxml_get_errors)
	ZEND_FE(libxml_clear_errors, arginfo_libxml_clear_errors)
	ZEND_DEP_FE(libxml_disable_entity_loader, arginfo_libxml_disable_entity_loader)
	ZEND_FE(libxml_set_external_entity_loader, arginfo_libxml_set_external_entity_loader)
	ZEND_FE_END
};


static const zend_function_entry class_LibXMLError_methods[] = {
	ZEND_FE_END
};
