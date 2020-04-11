/* This is a generated file, edit the .stub.php file instead. */

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMXPath___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, doc, DOMDocument, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, registerNodeNS, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMXPath_evaluate, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, expr, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, context, DOMNode, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, registerNodeNS, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
#define arginfo_class_DOMXPath_query arginfo_class_DOMXPath_evaluate
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMXPath_registerNamespace, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMXPath_registerPhpFunctions, 0, 0, 0)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(0, restrict, "null")
ZEND_END_ARG_INFO()
#endif


#if defined(LIBXML_XPATH_ENABLED)
ZEND_METHOD(DOMXPath, __construct);
#endif
#if defined(LIBXML_XPATH_ENABLED)
ZEND_METHOD(DOMXPath, evaluate);
#endif
#if defined(LIBXML_XPATH_ENABLED)
ZEND_METHOD(DOMXPath, query);
#endif
#if defined(LIBXML_XPATH_ENABLED)
ZEND_METHOD(DOMXPath, registerNamespace);
#endif
#if defined(LIBXML_XPATH_ENABLED)
ZEND_METHOD(DOMXPath, registerPhpFunctions);
#endif


static const zend_function_entry class_DOMXPath_methods[] = {
#if defined(LIBXML_XPATH_ENABLED)
	ZEND_ME(DOMXPath, __construct, arginfo_class_DOMXPath___construct, ZEND_ACC_PUBLIC)
#endif
#if defined(LIBXML_XPATH_ENABLED)
	ZEND_ME(DOMXPath, evaluate, arginfo_class_DOMXPath_evaluate, ZEND_ACC_PUBLIC)
#endif
#if defined(LIBXML_XPATH_ENABLED)
	ZEND_ME(DOMXPath, query, arginfo_class_DOMXPath_query, ZEND_ACC_PUBLIC)
#endif
#if defined(LIBXML_XPATH_ENABLED)
	ZEND_ME(DOMXPath, registerNamespace, arginfo_class_DOMXPath_registerNamespace, ZEND_ACC_PUBLIC)
#endif
#if defined(LIBXML_XPATH_ENABLED)
	ZEND_ME(DOMXPath, registerPhpFunctions, arginfo_class_DOMXPath_registerPhpFunctions, ZEND_ACC_PUBLIC)
#endif
	ZEND_FE_END
};
