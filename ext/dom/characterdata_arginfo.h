/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMCharacterData_appendData, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMCharacterData_substringData, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMCharacterData_insertData, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMCharacterData_deleteData arginfo_class_DOMCharacterData_substringData

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMCharacterData_replaceData, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMCharacterData_replaceWith, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_INFO(0, nodes)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMCharacterData_remove, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMCharacterData_before arginfo_class_DOMCharacterData_replaceWith

#define arginfo_class_DOMCharacterData_after arginfo_class_DOMCharacterData_replaceWith
