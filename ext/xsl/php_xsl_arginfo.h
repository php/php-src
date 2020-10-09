/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a119247725ff61dbd615cb86ee6201ee6603ba51 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_importStylesheet, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, stylesheet, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_transformToDoc, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, document, IS_OBJECT, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, returnClass, IS_STRING, 1, "null")
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
	ZEND_ARG_TYPE_MASK(0, name, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_getParameter, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XSLTProcessor_removeParameter arginfo_class_XSLTProcessor_getParameter

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_hasExsltSupport, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_registerPHPFunctions, 0, 0, 0)
	ZEND_ARG_TYPE_MASK(0, functions, MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_setProfiling, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XSLTProcessor_setSecurityPrefs, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, preferences, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XSLTProcessor_getSecurityPrefs arginfo_class_XSLTProcessor_hasExsltSupport


ZEND_METHOD(XSLTProcessor, importStylesheet);
ZEND_METHOD(XSLTProcessor, transformToDoc);
ZEND_METHOD(XSLTProcessor, transformToUri);
ZEND_METHOD(XSLTProcessor, transformToXml);
ZEND_METHOD(XSLTProcessor, setParameter);
ZEND_METHOD(XSLTProcessor, getParameter);
ZEND_METHOD(XSLTProcessor, removeParameter);
ZEND_METHOD(XSLTProcessor, hasExsltSupport);
ZEND_METHOD(XSLTProcessor, registerPHPFunctions);
ZEND_METHOD(XSLTProcessor, setProfiling);
ZEND_METHOD(XSLTProcessor, setSecurityPrefs);
ZEND_METHOD(XSLTProcessor, getSecurityPrefs);


static const zend_function_entry class_XSLTProcessor_methods[] = {
	ZEND_ME(XSLTProcessor, importStylesheet, arginfo_class_XSLTProcessor_importStylesheet, ZEND_ACC_PUBLIC)
	ZEND_ME(XSLTProcessor, transformToDoc, arginfo_class_XSLTProcessor_transformToDoc, ZEND_ACC_PUBLIC)
	ZEND_ME(XSLTProcessor, transformToUri, arginfo_class_XSLTProcessor_transformToUri, ZEND_ACC_PUBLIC)
	ZEND_ME(XSLTProcessor, transformToXml, arginfo_class_XSLTProcessor_transformToXml, ZEND_ACC_PUBLIC)
	ZEND_ME(XSLTProcessor, setParameter, arginfo_class_XSLTProcessor_setParameter, ZEND_ACC_PUBLIC)
	ZEND_ME(XSLTProcessor, getParameter, arginfo_class_XSLTProcessor_getParameter, ZEND_ACC_PUBLIC)
	ZEND_ME(XSLTProcessor, removeParameter, arginfo_class_XSLTProcessor_removeParameter, ZEND_ACC_PUBLIC)
	ZEND_ME(XSLTProcessor, hasExsltSupport, arginfo_class_XSLTProcessor_hasExsltSupport, ZEND_ACC_PUBLIC)
	ZEND_ME(XSLTProcessor, registerPHPFunctions, arginfo_class_XSLTProcessor_registerPHPFunctions, ZEND_ACC_PUBLIC)
	ZEND_ME(XSLTProcessor, setProfiling, arginfo_class_XSLTProcessor_setProfiling, ZEND_ACC_PUBLIC)
	ZEND_ME(XSLTProcessor, setSecurityPrefs, arginfo_class_XSLTProcessor_setSecurityPrefs, ZEND_ACC_PUBLIC)
	ZEND_ME(XSLTProcessor, getSecurityPrefs, arginfo_class_XSLTProcessor_getSecurityPrefs, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
