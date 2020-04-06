/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, uri, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_getAttribute, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_getAttributeNS, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMElement_getAttributeNode arginfo_class_DOMElement_getAttribute

#define arginfo_class_DOMElement_getAttributeNodeNS arginfo_class_DOMElement_getAttributeNS

#define arginfo_class_DOMElement_getElementsByTagName arginfo_class_DOMElement_getAttribute

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_getElementsByTagNameNS, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMElement_hasAttribute arginfo_class_DOMElement_getAttribute

#define arginfo_class_DOMElement_hasAttributeNS arginfo_class_DOMElement_getAttributeNS

#define arginfo_class_DOMElement_removeAttribute arginfo_class_DOMElement_getAttribute

#define arginfo_class_DOMElement_removeAttributeNS arginfo_class_DOMElement_getAttributeNS

#define arginfo_class_DOMElement_removeAttributeNode arginfo_class_DOMElement_getAttribute

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_setAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_setAttributeNS, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_setAttributeNode, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, attr, DOMAttr, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMElement_setAttributeNodeNS arginfo_class_DOMElement_setAttributeNode

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_setIdAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isId, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_setIdAttributeNS, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isId, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_setIdAttributeNode, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, attr, DOMAttr, 0)
	ZEND_ARG_TYPE_INFO(0, isId, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMElement_remove, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMElement_before, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_INFO(0, nodes)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMElement_after arginfo_class_DOMElement_before

#define arginfo_class_DOMElement_replaceWith arginfo_class_DOMElement_before

#define arginfo_class_DOMElement_append arginfo_class_DOMElement_before

#define arginfo_class_DOMElement_prepend arginfo_class_DOMElement_before
