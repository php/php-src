/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Transliterator___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Transliterator_create, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, direction, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Transliterator_createFromRules, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, rules, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, direction, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Transliterator_createInverse arginfo_class_Transliterator___construct

#define arginfo_class_Transliterator_listIDs arginfo_class_Transliterator___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Transliterator_transliterate, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Transliterator_getErrorCode arginfo_class_Transliterator___construct

#define arginfo_class_Transliterator_getErrorMessage arginfo_class_Transliterator___construct
