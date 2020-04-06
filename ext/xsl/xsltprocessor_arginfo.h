/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_importStylesheet, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, stylesheet, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_transformToDoc, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, document, IS_OBJECT, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, return_class, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_transformToUri, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, document, IS_OBJECT, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_transformToXml, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, document, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_setParameter, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_getParameter, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XSLTProcessor_removeParameter arginfo_class_XSLTProcessor_getParameter

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_hasExsltSupport, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_registerPHPFunctions, 0, 0, 0)
	ZEND_ARG_INFO(0, restrict)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_setProfiling, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_setSecurityPrefs, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, securityPrefs, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XSLTProcessor_getSecurityPrefs arginfo_class_XSLTProcessor_hasExsltSupport
