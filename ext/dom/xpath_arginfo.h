/* This is a generated file, edit the .stub.php file instead. */

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMXPath___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, doc, DOMDocument, 0)
	ZEND_ARG_TYPE_INFO(0, registerNodeNS, _IS_BOOL, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMXPath_evaluate, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, expr, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, context, DOMNode, 1)
	ZEND_ARG_TYPE_INFO(0, registerNodeNS, _IS_BOOL, 0)
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
	ZEND_ARG_INFO(0, restrict)
ZEND_END_ARG_INFO()
#endif
