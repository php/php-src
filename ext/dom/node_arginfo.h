/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMChildNode_remove, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMChildNode_before, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_INFO(0, nodes)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMChildNode_after arginfo_class_DOMChildNode_before

#define arginfo_class_DOMChildNode_replaceWith arginfo_class_DOMChildNode_before

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_appendChild, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, newChild, DOMNode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_C14N, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, exclusive, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, with_comments, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, xpath, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, ns_prefixes, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_C14NFile, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, exclusive, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, with_comments, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, xpath, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, ns_prefixes, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_cloneNode, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, recursive, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_getLineNo, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMNode_getNodePath arginfo_class_DOMNode_getLineNo

#define arginfo_class_DOMNode_hasAttributes arginfo_class_DOMNode_getLineNo

#define arginfo_class_DOMNode_hasChildNodes arginfo_class_DOMNode_getLineNo

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_insertBefore, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, newChild, DOMNode, 0)
	ZEND_ARG_OBJ_INFO(0, refChild, DOMNode, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_isDefaultNamespace, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_isSameNode, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, other, DOMNode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_isSupported, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, feature, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, version, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_lookupNamespaceUri, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMNode_lookupPrefix arginfo_class_DOMNode_isDefaultNamespace

#define arginfo_class_DOMNode_normalize arginfo_class_DOMNode_getLineNo

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_removeChild, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, oldChild, DOMNode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_replaceChild, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, newChild, DOMNode, 0)
	ZEND_ARG_OBJ_INFO(0, oldChild, DOMNode, 0)
ZEND_END_ARG_INFO()
