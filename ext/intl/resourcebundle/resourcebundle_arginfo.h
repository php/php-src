/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ResourceBundle___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, locale, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, bundlename, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, fallback, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ResourceBundle_create arginfo_class_ResourceBundle___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ResourceBundle_get, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_TYPE_INFO(0, fallback, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ResourceBundle_count, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ResourceBundle_getLocales, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, bundlename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ResourceBundle_getErrorCode arginfo_class_ResourceBundle_count

#define arginfo_class_ResourceBundle_getErrorMessage arginfo_class_ResourceBundle_count

#define arginfo_resourcebundle_create arginfo_class_ResourceBundle___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_resourcebundle_get, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, bundle, ResourceBundle, 0)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_resourcebundle_count, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, bundle, ResourceBundle, 0)
ZEND_END_ARG_INFO()

#define arginfo_resourcebundle_locales arginfo_class_ResourceBundle_getLocales

#define arginfo_resourcebundle_get_error_code arginfo_resourcebundle_count

#define arginfo_resourcebundle_get_error_message arginfo_resourcebundle_count
