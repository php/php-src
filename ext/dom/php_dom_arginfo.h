/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 9142d10743d4ec2f5e587c67bd111155c6149efd */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_dom_import_simplexml, 0, 1, DOMElement, 0)
	ZEND_ARG_TYPE_INFO(0, node, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMCdataSection___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMComment___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, data, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMParentNode_append, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_INFO(0, nodes)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMParentNode_prepend arginfo_class_DOMParentNode_append

#define arginfo_class_DOMParentNode_replaceChildren arginfo_class_DOMParentNode_append

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMChildNode_remove, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMChildNode_before arginfo_class_DOMParentNode_append

#define arginfo_class_DOMChildNode_after arginfo_class_DOMParentNode_append

#define arginfo_class_DOMChildNode_replaceWith arginfo_class_DOMParentNode_append

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_appendChild, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_DOMNode_C14N, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, exclusive, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, withComments, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, xpath, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nsPrefixes, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_DOMNode_C14NFile, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, exclusive, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, withComments, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, xpath, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nsPrefixes, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_cloneNode, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, deep, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMNode_getLineNo, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMNode_getNodePath, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMNode_hasAttributes, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMNode_hasChildNodes arginfo_class_DOMNode_hasAttributes

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_insertBefore, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, child, DOMNode, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMNode_isDefaultNamespace, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMNode_isSameNode, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, otherNode, DOMNode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMNode_isEqualNode, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, otherNode, DOMNode, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMNode_isSupported, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, feature, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, version, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMNode_lookupNamespaceURI, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMNode_lookupPrefix, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMNode_normalize, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_removeChild, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, child, DOMNode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_replaceChild, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 0)
	ZEND_ARG_OBJ_INFO(0, child, DOMNode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMNode_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, other, DOMNode|DOMNameSpaceNode, MAY_BE_NULL, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOMNode_getRootNode, 0, 0, DOMNode, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMNode_compareDocumentPosition, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, other, DOMNode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMImplementation_getFeature, 0, 2, IS_NEVER, 0)
	ZEND_ARG_TYPE_INFO(0, feature, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, version, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMImplementation_hasFeature arginfo_class_DOMNode_isSupported

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMImplementation_createDocumentType, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, publicId, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, systemId, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMImplementation_createDocument, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, namespace, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, qualifiedName, IS_STRING, 0, "\"\"")
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, doctype, DOMDocumentType, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocumentFragment___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMDocumentFragment_appendXML, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocumentFragment_append arginfo_class_DOMParentNode_append

#define arginfo_class_DOMDocumentFragment_prepend arginfo_class_DOMParentNode_append

#define arginfo_class_DOMDocumentFragment_replaceChildren arginfo_class_DOMParentNode_append

#define arginfo_class_DOMNodeList_count arginfo_class_DOMNode_getLineNo

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOMNodeList_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNodeList_item, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMCharacterData_appendData, 0, 1, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMCharacterData_substringData, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMCharacterData_insertData, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMCharacterData_deleteData, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMCharacterData_replaceData, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMCharacterData_replaceWith arginfo_class_DOMParentNode_append

#define arginfo_class_DOMCharacterData_remove arginfo_class_DOMChildNode_remove

#define arginfo_class_DOMCharacterData_before arginfo_class_DOMParentNode_append

#define arginfo_class_DOMCharacterData_after arginfo_class_DOMParentNode_append

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMAttr___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#define arginfo_class_DOMAttr_isId arginfo_class_DOMNode_hasAttributes

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, namespace, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMElement_getAttribute, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMElement_getAttributeNames, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMElement_getAttributeNS, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_getAttributeNode, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_getAttributeNodeNS, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_DOMElement_getElementsByTagName, 0, 1, DOMNodeList, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_DOMElement_getElementsByTagNameNS, 0, 2, DOMNodeList, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMElement_hasAttribute, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMElement_hasAttributeNS, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMElement_removeAttribute arginfo_class_DOMElement_hasAttribute

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMElement_removeAttributeNS, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_removeAttributeNode, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, attr, DOMAttr, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_setAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMElement_setAttributeNS, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMElement_setAttributeNode arginfo_class_DOMElement_removeAttributeNode

#define arginfo_class_DOMElement_setAttributeNodeNS arginfo_class_DOMElement_removeAttributeNode

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMElement_setIdAttribute, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isId, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMElement_setIdAttributeNS, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isId, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMElement_setIdAttributeNode, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, attr, DOMAttr, 0)
	ZEND_ARG_TYPE_INFO(0, isId, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMElement_toggleAttribute, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, force, _IS_BOOL, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_DOMElement_remove arginfo_class_DOMChildNode_remove

#define arginfo_class_DOMElement_before arginfo_class_DOMParentNode_append

#define arginfo_class_DOMElement_after arginfo_class_DOMParentNode_append

#define arginfo_class_DOMElement_replaceWith arginfo_class_DOMParentNode_append

#define arginfo_class_DOMElement_append arginfo_class_DOMParentNode_append

#define arginfo_class_DOMElement_prepend arginfo_class_DOMParentNode_append

#define arginfo_class_DOMElement_replaceChildren arginfo_class_DOMParentNode_append

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOMElement_insertAdjacentElement, 0, 2, DOMElement, 1)
	ZEND_ARG_TYPE_INFO(0, where, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, element, DOMElement, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMElement_insertAdjacentText, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, where, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, version, IS_STRING, 0, "\"1.0\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_createAttribute, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_createAttributeNS, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_createCDATASection arginfo_class_DOMCdataSection___construct

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_DOMDocument_createComment, 0, 1, DOMComment, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_DOMDocument_createDocumentFragment, 0, 0, DOMDocumentFragment, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_createElement, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_createElementNS, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_createEntityReference, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_createProcessingInstruction, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, data, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_DOMDocument_createTextNode, 0, 1, DOMText, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_DOMDocument_getElementById, 0, 1, DOMElement, 1)
	ZEND_ARG_TYPE_INFO(0, elementId, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_getElementsByTagName arginfo_class_DOMElement_getElementsByTagName

#define arginfo_class_DOMDocument_getElementsByTagNameNS arginfo_class_DOMElement_getElementsByTagNameNS

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_importNode, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, deep, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMDocument_load, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMDocument_loadXML, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_normalizeDocument arginfo_class_DOMNode_normalize

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMDocument_registerNodeClass, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, baseClass, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, extendedClass, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_DOMDocument_save, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#if defined(LIBXML_HTML_ENABLED)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMDocument_loadHTML, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_HTML_ENABLED)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMDocument_loadHTMLFile, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_HTML_ENABLED)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_DOMDocument_saveHTML, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, node, DOMNode, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_HTML_ENABLED)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_DOMDocument_saveHTMLFile, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_DOMDocument_saveXML, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, node, DOMNode, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMDocument_schemaValidate, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMDocument_schemaValidateSource, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMDocument_relaxNGValidate, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMDocument_relaxNGValidateSource, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_class_DOMDocument_validate arginfo_class_DOMNode_hasAttributes

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX(arginfo_class_DOMDocument_xinclude, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_DOMDocument_adoptNode, 0, 1, DOMNode, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_append arginfo_class_DOMParentNode_append

#define arginfo_class_DOMDocument_prepend arginfo_class_DOMParentNode_append

#define arginfo_class_DOMDocument_replaceChildren arginfo_class_DOMParentNode_append

#define arginfo_class_DOMText___construct arginfo_class_DOMComment___construct

#define arginfo_class_DOMText_isWhitespaceInElementContent arginfo_class_DOMNode_hasAttributes

#define arginfo_class_DOMText_isElementContentWhitespace arginfo_class_DOMNode_hasAttributes

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMText_splitText, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_DOMNamedNodeMap_getNamedItem, 0, 1, DOMNode, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_DOMNamedNodeMap_getNamedItemNS, 0, 2, DOMNode, 1)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_DOMNamedNodeMap_item, 0, 1, DOMNode, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMNamedNodeMap_count arginfo_class_DOMNode_getLineNo

#define arginfo_class_DOMNamedNodeMap_getIterator arginfo_class_DOMNodeList_getIterator

#define arginfo_class_DOMEntityReference___construct arginfo_class_DOMDocument_createEntityReference

#define arginfo_class_DOMProcessingInstruction___construct arginfo_class_DOMAttr___construct

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMXPath___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, document, DOMDocument, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, registerNodeNS, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMXPath_evaluate, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, expression, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, contextNode, DOMNode, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, registerNodeNS, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
#define arginfo_class_DOMXPath_query arginfo_class_DOMXPath_evaluate
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMXPath_registerNamespace, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMXPath_registerPhpFunctions, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_MASK(0, restrict, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()
#endif


ZEND_FUNCTION(dom_import_simplexml);
ZEND_METHOD(DOMCdataSection, __construct);
ZEND_METHOD(DOMComment, __construct);
ZEND_METHOD(DOMNode, appendChild);
ZEND_METHOD(DOMNode, C14N);
ZEND_METHOD(DOMNode, C14NFile);
ZEND_METHOD(DOMNode, cloneNode);
ZEND_METHOD(DOMNode, getLineNo);
ZEND_METHOD(DOMNode, getNodePath);
ZEND_METHOD(DOMNode, hasAttributes);
ZEND_METHOD(DOMNode, hasChildNodes);
ZEND_METHOD(DOMNode, insertBefore);
ZEND_METHOD(DOMNode, isDefaultNamespace);
ZEND_METHOD(DOMNode, isSameNode);
ZEND_METHOD(DOMNode, isEqualNode);
ZEND_METHOD(DOMNode, isSupported);
ZEND_METHOD(DOMNode, lookupNamespaceURI);
ZEND_METHOD(DOMNode, lookupPrefix);
ZEND_METHOD(DOMNode, normalize);
ZEND_METHOD(DOMNode, removeChild);
ZEND_METHOD(DOMNode, replaceChild);
ZEND_METHOD(DOMNode, contains);
ZEND_METHOD(DOMNode, getRootNode);
ZEND_METHOD(DOMNode, compareDocumentPosition);
ZEND_METHOD(DOMImplementation, getFeature);
ZEND_METHOD(DOMImplementation, hasFeature);
ZEND_METHOD(DOMImplementation, createDocumentType);
ZEND_METHOD(DOMImplementation, createDocument);
ZEND_METHOD(DOMDocumentFragment, __construct);
ZEND_METHOD(DOMDocumentFragment, appendXML);
ZEND_METHOD(DOMElement, append);
ZEND_METHOD(DOMElement, prepend);
ZEND_METHOD(DOMDocument, replaceChildren);
ZEND_METHOD(DOMNodeList, count);
ZEND_METHOD(DOMNodeList, getIterator);
ZEND_METHOD(DOMNodeList, item);
ZEND_METHOD(DOMCharacterData, appendData);
ZEND_METHOD(DOMCharacterData, substringData);
ZEND_METHOD(DOMCharacterData, insertData);
ZEND_METHOD(DOMCharacterData, deleteData);
ZEND_METHOD(DOMCharacterData, replaceData);
ZEND_METHOD(DOMElement, replaceWith);
ZEND_METHOD(DOMElement, remove);
ZEND_METHOD(DOMElement, before);
ZEND_METHOD(DOMElement, after);
ZEND_METHOD(DOMAttr, __construct);
ZEND_METHOD(DOMAttr, isId);
ZEND_METHOD(DOMElement, __construct);
ZEND_METHOD(DOMElement, getAttribute);
ZEND_METHOD(DOMElement, getAttributeNames);
ZEND_METHOD(DOMElement, getAttributeNS);
ZEND_METHOD(DOMElement, getAttributeNode);
ZEND_METHOD(DOMElement, getAttributeNodeNS);
ZEND_METHOD(DOMElement, getElementsByTagName);
ZEND_METHOD(DOMElement, getElementsByTagNameNS);
ZEND_METHOD(DOMElement, hasAttribute);
ZEND_METHOD(DOMElement, hasAttributeNS);
ZEND_METHOD(DOMElement, removeAttribute);
ZEND_METHOD(DOMElement, removeAttributeNS);
ZEND_METHOD(DOMElement, removeAttributeNode);
ZEND_METHOD(DOMElement, setAttribute);
ZEND_METHOD(DOMElement, setAttributeNS);
ZEND_METHOD(DOMElement, setAttributeNode);
ZEND_METHOD(DOMElement, setAttributeNodeNS);
ZEND_METHOD(DOMElement, setIdAttribute);
ZEND_METHOD(DOMElement, setIdAttributeNS);
ZEND_METHOD(DOMElement, setIdAttributeNode);
ZEND_METHOD(DOMElement, toggleAttribute);
ZEND_METHOD(DOMElement, replaceChildren);
ZEND_METHOD(DOMElement, insertAdjacentElement);
ZEND_METHOD(DOMElement, insertAdjacentText);
ZEND_METHOD(DOMDocument, __construct);
ZEND_METHOD(DOMDocument, createAttribute);
ZEND_METHOD(DOMDocument, createAttributeNS);
ZEND_METHOD(DOMDocument, createCDATASection);
ZEND_METHOD(DOMDocument, createComment);
ZEND_METHOD(DOMDocument, createDocumentFragment);
ZEND_METHOD(DOMDocument, createElement);
ZEND_METHOD(DOMDocument, createElementNS);
ZEND_METHOD(DOMDocument, createEntityReference);
ZEND_METHOD(DOMDocument, createProcessingInstruction);
ZEND_METHOD(DOMDocument, createTextNode);
ZEND_METHOD(DOMDocument, getElementById);
ZEND_METHOD(DOMDocument, getElementsByTagName);
ZEND_METHOD(DOMDocument, getElementsByTagNameNS);
ZEND_METHOD(DOMDocument, importNode);
ZEND_METHOD(DOMDocument, load);
ZEND_METHOD(DOMDocument, loadXML);
ZEND_METHOD(DOMDocument, normalizeDocument);
ZEND_METHOD(DOMDocument, registerNodeClass);
ZEND_METHOD(DOMDocument, save);
#if defined(LIBXML_HTML_ENABLED)
ZEND_METHOD(DOMDocument, loadHTML);
#endif
#if defined(LIBXML_HTML_ENABLED)
ZEND_METHOD(DOMDocument, loadHTMLFile);
#endif
#if defined(LIBXML_HTML_ENABLED)
ZEND_METHOD(DOMDocument, saveHTML);
#endif
#if defined(LIBXML_HTML_ENABLED)
ZEND_METHOD(DOMDocument, saveHTMLFile);
#endif
ZEND_METHOD(DOMDocument, saveXML);
#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_METHOD(DOMDocument, schemaValidate);
#endif
#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_METHOD(DOMDocument, schemaValidateSource);
#endif
#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_METHOD(DOMDocument, relaxNGValidate);
#endif
#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_METHOD(DOMDocument, relaxNGValidateSource);
#endif
ZEND_METHOD(DOMDocument, validate);
ZEND_METHOD(DOMDocument, xinclude);
ZEND_METHOD(DOMDocument, adoptNode);
ZEND_METHOD(DOMText, __construct);
ZEND_METHOD(DOMText, isWhitespaceInElementContent);
ZEND_METHOD(DOMText, splitText);
ZEND_METHOD(DOMNamedNodeMap, getNamedItem);
ZEND_METHOD(DOMNamedNodeMap, getNamedItemNS);
ZEND_METHOD(DOMNamedNodeMap, item);
ZEND_METHOD(DOMNamedNodeMap, count);
ZEND_METHOD(DOMNamedNodeMap, getIterator);
ZEND_METHOD(DOMEntityReference, __construct);
ZEND_METHOD(DOMProcessingInstruction, __construct);
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


static const zend_function_entry ext_functions[] = {
	ZEND_FE(dom_import_simplexml, arginfo_dom_import_simplexml)
	ZEND_FE_END
};


static const zend_function_entry class_DOMDocumentType_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_DOMCdataSection_methods[] = {
	ZEND_ME(DOMCdataSection, __construct, arginfo_class_DOMCdataSection___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_DOMComment_methods[] = {
	ZEND_ME(DOMComment, __construct, arginfo_class_DOMComment___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_DOMParentNode_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(DOMParentNode, append, arginfo_class_DOMParentNode_append, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DOMParentNode, prepend, arginfo_class_DOMParentNode_prepend, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DOMParentNode, replaceChildren, arginfo_class_DOMParentNode_replaceChildren, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_DOMChildNode_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(DOMChildNode, remove, arginfo_class_DOMChildNode_remove, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DOMChildNode, before, arginfo_class_DOMChildNode_before, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DOMChildNode, after, arginfo_class_DOMChildNode_after, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(DOMChildNode, replaceWith, arginfo_class_DOMChildNode_replaceWith, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};


static const zend_function_entry class_DOMNode_methods[] = {
	ZEND_ME(DOMNode, appendChild, arginfo_class_DOMNode_appendChild, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, C14N, arginfo_class_DOMNode_C14N, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, C14NFile, arginfo_class_DOMNode_C14NFile, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, cloneNode, arginfo_class_DOMNode_cloneNode, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, getLineNo, arginfo_class_DOMNode_getLineNo, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, getNodePath, arginfo_class_DOMNode_getNodePath, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, hasAttributes, arginfo_class_DOMNode_hasAttributes, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, hasChildNodes, arginfo_class_DOMNode_hasChildNodes, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, insertBefore, arginfo_class_DOMNode_insertBefore, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, isDefaultNamespace, arginfo_class_DOMNode_isDefaultNamespace, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, isSameNode, arginfo_class_DOMNode_isSameNode, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, isEqualNode, arginfo_class_DOMNode_isEqualNode, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, isSupported, arginfo_class_DOMNode_isSupported, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, lookupNamespaceURI, arginfo_class_DOMNode_lookupNamespaceURI, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, lookupPrefix, arginfo_class_DOMNode_lookupPrefix, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, normalize, arginfo_class_DOMNode_normalize, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, removeChild, arginfo_class_DOMNode_removeChild, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, replaceChild, arginfo_class_DOMNode_replaceChild, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, contains, arginfo_class_DOMNode_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, getRootNode, arginfo_class_DOMNode_getRootNode, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, compareDocumentPosition, arginfo_class_DOMNode_compareDocumentPosition, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_DOMNameSpaceNode_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_DOMImplementation_methods[] = {
	ZEND_ME(DOMImplementation, getFeature, arginfo_class_DOMImplementation_getFeature, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMImplementation, hasFeature, arginfo_class_DOMImplementation_hasFeature, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMImplementation, createDocumentType, arginfo_class_DOMImplementation_createDocumentType, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMImplementation, createDocument, arginfo_class_DOMImplementation_createDocument, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_DOMDocumentFragment_methods[] = {
	ZEND_ME(DOMDocumentFragment, __construct, arginfo_class_DOMDocumentFragment___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocumentFragment, appendXML, arginfo_class_DOMDocumentFragment_appendXML, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(DOMElement, append, append, arginfo_class_DOMDocumentFragment_append, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(DOMElement, prepend, prepend, arginfo_class_DOMDocumentFragment_prepend, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(DOMDocument, replaceChildren, replaceChildren, arginfo_class_DOMDocumentFragment_replaceChildren, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_DOMNodeList_methods[] = {
	ZEND_ME(DOMNodeList, count, arginfo_class_DOMNodeList_count, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNodeList, getIterator, arginfo_class_DOMNodeList_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNodeList, item, arginfo_class_DOMNodeList_item, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_DOMCharacterData_methods[] = {
	ZEND_ME(DOMCharacterData, appendData, arginfo_class_DOMCharacterData_appendData, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMCharacterData, substringData, arginfo_class_DOMCharacterData_substringData, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMCharacterData, insertData, arginfo_class_DOMCharacterData_insertData, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMCharacterData, deleteData, arginfo_class_DOMCharacterData_deleteData, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMCharacterData, replaceData, arginfo_class_DOMCharacterData_replaceData, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(DOMElement, replaceWith, replaceWith, arginfo_class_DOMCharacterData_replaceWith, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(DOMElement, remove, remove, arginfo_class_DOMCharacterData_remove, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(DOMElement, before, before, arginfo_class_DOMCharacterData_before, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(DOMElement, after, after, arginfo_class_DOMCharacterData_after, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_DOMAttr_methods[] = {
	ZEND_ME(DOMAttr, __construct, arginfo_class_DOMAttr___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMAttr, isId, arginfo_class_DOMAttr_isId, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_DOMElement_methods[] = {
	ZEND_ME(DOMElement, __construct, arginfo_class_DOMElement___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, getAttribute, arginfo_class_DOMElement_getAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, getAttributeNames, arginfo_class_DOMElement_getAttributeNames, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, getAttributeNS, arginfo_class_DOMElement_getAttributeNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, getAttributeNode, arginfo_class_DOMElement_getAttributeNode, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, getAttributeNodeNS, arginfo_class_DOMElement_getAttributeNodeNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, getElementsByTagName, arginfo_class_DOMElement_getElementsByTagName, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, getElementsByTagNameNS, arginfo_class_DOMElement_getElementsByTagNameNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, hasAttribute, arginfo_class_DOMElement_hasAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, hasAttributeNS, arginfo_class_DOMElement_hasAttributeNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, removeAttribute, arginfo_class_DOMElement_removeAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, removeAttributeNS, arginfo_class_DOMElement_removeAttributeNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, removeAttributeNode, arginfo_class_DOMElement_removeAttributeNode, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, setAttribute, arginfo_class_DOMElement_setAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, setAttributeNS, arginfo_class_DOMElement_setAttributeNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, setAttributeNode, arginfo_class_DOMElement_setAttributeNode, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, setAttributeNodeNS, arginfo_class_DOMElement_setAttributeNodeNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, setIdAttribute, arginfo_class_DOMElement_setIdAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, setIdAttributeNS, arginfo_class_DOMElement_setIdAttributeNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, setIdAttributeNode, arginfo_class_DOMElement_setIdAttributeNode, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, toggleAttribute, arginfo_class_DOMElement_toggleAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, remove, arginfo_class_DOMElement_remove, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, before, arginfo_class_DOMElement_before, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, after, arginfo_class_DOMElement_after, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, replaceWith, arginfo_class_DOMElement_replaceWith, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, append, arginfo_class_DOMElement_append, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, prepend, arginfo_class_DOMElement_prepend, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, replaceChildren, arginfo_class_DOMElement_replaceChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, insertAdjacentElement, arginfo_class_DOMElement_insertAdjacentElement, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, insertAdjacentText, arginfo_class_DOMElement_insertAdjacentText, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_DOMDocument_methods[] = {
	ZEND_ME(DOMDocument, __construct, arginfo_class_DOMDocument___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, createAttribute, arginfo_class_DOMDocument_createAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, createAttributeNS, arginfo_class_DOMDocument_createAttributeNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, createCDATASection, arginfo_class_DOMDocument_createCDATASection, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, createComment, arginfo_class_DOMDocument_createComment, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, createDocumentFragment, arginfo_class_DOMDocument_createDocumentFragment, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, createElement, arginfo_class_DOMDocument_createElement, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, createElementNS, arginfo_class_DOMDocument_createElementNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, createEntityReference, arginfo_class_DOMDocument_createEntityReference, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, createProcessingInstruction, arginfo_class_DOMDocument_createProcessingInstruction, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, createTextNode, arginfo_class_DOMDocument_createTextNode, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, getElementById, arginfo_class_DOMDocument_getElementById, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, getElementsByTagName, arginfo_class_DOMDocument_getElementsByTagName, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, getElementsByTagNameNS, arginfo_class_DOMDocument_getElementsByTagNameNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, importNode, arginfo_class_DOMDocument_importNode, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, load, arginfo_class_DOMDocument_load, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, loadXML, arginfo_class_DOMDocument_loadXML, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, normalizeDocument, arginfo_class_DOMDocument_normalizeDocument, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, registerNodeClass, arginfo_class_DOMDocument_registerNodeClass, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, save, arginfo_class_DOMDocument_save, ZEND_ACC_PUBLIC)
#if defined(LIBXML_HTML_ENABLED)
	ZEND_ME(DOMDocument, loadHTML, arginfo_class_DOMDocument_loadHTML, ZEND_ACC_PUBLIC)
#endif
#if defined(LIBXML_HTML_ENABLED)
	ZEND_ME(DOMDocument, loadHTMLFile, arginfo_class_DOMDocument_loadHTMLFile, ZEND_ACC_PUBLIC)
#endif
#if defined(LIBXML_HTML_ENABLED)
	ZEND_ME(DOMDocument, saveHTML, arginfo_class_DOMDocument_saveHTML, ZEND_ACC_PUBLIC)
#endif
#if defined(LIBXML_HTML_ENABLED)
	ZEND_ME(DOMDocument, saveHTMLFile, arginfo_class_DOMDocument_saveHTMLFile, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME(DOMDocument, saveXML, arginfo_class_DOMDocument_saveXML, ZEND_ACC_PUBLIC)
#if defined(LIBXML_SCHEMAS_ENABLED)
	ZEND_ME(DOMDocument, schemaValidate, arginfo_class_DOMDocument_schemaValidate, ZEND_ACC_PUBLIC)
#endif
#if defined(LIBXML_SCHEMAS_ENABLED)
	ZEND_ME(DOMDocument, schemaValidateSource, arginfo_class_DOMDocument_schemaValidateSource, ZEND_ACC_PUBLIC)
#endif
#if defined(LIBXML_SCHEMAS_ENABLED)
	ZEND_ME(DOMDocument, relaxNGValidate, arginfo_class_DOMDocument_relaxNGValidate, ZEND_ACC_PUBLIC)
#endif
#if defined(LIBXML_SCHEMAS_ENABLED)
	ZEND_ME(DOMDocument, relaxNGValidateSource, arginfo_class_DOMDocument_relaxNGValidateSource, ZEND_ACC_PUBLIC)
#endif
	ZEND_ME(DOMDocument, validate, arginfo_class_DOMDocument_validate, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, xinclude, arginfo_class_DOMDocument_xinclude, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, adoptNode, arginfo_class_DOMDocument_adoptNode, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(DOMElement, append, append, arginfo_class_DOMDocument_append, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(DOMElement, prepend, prepend, arginfo_class_DOMDocument_prepend, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, replaceChildren, arginfo_class_DOMDocument_replaceChildren, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_DOMException_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_DOMText_methods[] = {
	ZEND_ME(DOMText, __construct, arginfo_class_DOMText___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMText, isWhitespaceInElementContent, arginfo_class_DOMText_isWhitespaceInElementContent, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(DOMText, isElementContentWhitespace, isWhitespaceInElementContent, arginfo_class_DOMText_isElementContentWhitespace, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMText, splitText, arginfo_class_DOMText_splitText, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_DOMNamedNodeMap_methods[] = {
	ZEND_ME(DOMNamedNodeMap, getNamedItem, arginfo_class_DOMNamedNodeMap_getNamedItem, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNamedNodeMap, getNamedItemNS, arginfo_class_DOMNamedNodeMap_getNamedItemNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNamedNodeMap, item, arginfo_class_DOMNamedNodeMap_item, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNamedNodeMap, count, arginfo_class_DOMNamedNodeMap_count, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNamedNodeMap, getIterator, arginfo_class_DOMNamedNodeMap_getIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_DOMEntity_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_DOMEntityReference_methods[] = {
	ZEND_ME(DOMEntityReference, __construct, arginfo_class_DOMEntityReference___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_DOMNotation_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_DOMProcessingInstruction_methods[] = {
	ZEND_ME(DOMProcessingInstruction, __construct, arginfo_class_DOMProcessingInstruction___construct, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


#if defined(LIBXML_XPATH_ENABLED)
static const zend_function_entry class_DOMXPath_methods[] = {
	ZEND_ME(DOMXPath, __construct, arginfo_class_DOMXPath___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMXPath, evaluate, arginfo_class_DOMXPath_evaluate, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMXPath, query, arginfo_class_DOMXPath_query, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMXPath, registerNamespace, arginfo_class_DOMXPath_registerNamespace, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMXPath, registerPhpFunctions, arginfo_class_DOMXPath_registerPhpFunctions, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
#endif

static void register_php_dom_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("XML_ELEMENT_NODE", XML_ELEMENT_NODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NODE", XML_ATTRIBUTE_NODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_TEXT_NODE", XML_TEXT_NODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_CDATA_SECTION_NODE", XML_CDATA_SECTION_NODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ENTITY_REF_NODE", XML_ENTITY_REF_NODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ENTITY_NODE", XML_ENTITY_NODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_PI_NODE", XML_PI_NODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_COMMENT_NODE", XML_COMMENT_NODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DOCUMENT_NODE", XML_DOCUMENT_NODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DOCUMENT_TYPE_NODE", XML_DOCUMENT_TYPE_NODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DOCUMENT_FRAG_NODE", XML_DOCUMENT_FRAG_NODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_NOTATION_NODE", XML_NOTATION_NODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_HTML_DOCUMENT_NODE", XML_HTML_DOCUMENT_NODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_DTD_NODE", XML_DTD_NODE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ELEMENT_DECL_NODE", XML_ELEMENT_DECL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_DECL_NODE", XML_ATTRIBUTE_DECL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ENTITY_DECL_NODE", XML_ENTITY_DECL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_NAMESPACE_DECL_NODE", XML_NAMESPACE_DECL, CONST_PERSISTENT);
#if defined(XML_GLOBAL_NAMESPACE)
	REGISTER_LONG_CONSTANT("XML_GLOBAL_NAMESPACE", XML_GLOBAL_NAMESPACE, CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("XML_LOCAL_NAMESPACE", XML_LOCAL_NAMESPACE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_CDATA", XML_ATTRIBUTE_CDATA, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_ID", XML_ATTRIBUTE_ID, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_IDREF", XML_ATTRIBUTE_IDREF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_IDREFS", XML_ATTRIBUTE_IDREFS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_ENTITY", XML_ATTRIBUTE_ENTITIES, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NMTOKEN", XML_ATTRIBUTE_NMTOKEN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NMTOKENS", XML_ATTRIBUTE_NMTOKENS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_ENUMERATION", XML_ATTRIBUTE_ENUMERATION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ATTRIBUTE_NOTATION", XML_ATTRIBUTE_NOTATION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_PHP_ERR", PHP_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_INDEX_SIZE_ERR", INDEX_SIZE_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOMSTRING_SIZE_ERR", DOMSTRING_SIZE_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_HIERARCHY_REQUEST_ERR", HIERARCHY_REQUEST_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_WRONG_DOCUMENT_ERR", WRONG_DOCUMENT_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_INVALID_CHARACTER_ERR", INVALID_CHARACTER_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_NO_DATA_ALLOWED_ERR", NO_DATA_ALLOWED_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_NO_MODIFICATION_ALLOWED_ERR", NO_MODIFICATION_ALLOWED_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_NOT_FOUND_ERR", NOT_FOUND_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_NOT_SUPPORTED_ERR", NOT_SUPPORTED_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_INUSE_ATTRIBUTE_ERR", INUSE_ATTRIBUTE_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_INVALID_STATE_ERR", INVALID_STATE_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_SYNTAX_ERR", SYNTAX_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_INVALID_MODIFICATION_ERR", INVALID_MODIFICATION_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_NAMESPACE_ERR", NAMESPACE_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_INVALID_ACCESS_ERR", INVALID_ACCESS_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM_VALIDATION_ERR", VALIDATION_ERR, CONST_PERSISTENT);
}

static zend_class_entry *register_class_DOMDocumentType(zend_class_entry *class_entry_DOMNode)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMDocumentType", class_DOMDocumentType_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOMNode);

	zval property_name_default_value;
	ZVAL_UNDEF(&property_name_default_value);
	zend_string *property_name_name = zend_string_init("name", sizeof("name") - 1, 1);
	zend_declare_typed_property(class_entry, property_name_name, &property_name_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_name_name);

	zval property_entities_default_value;
	ZVAL_UNDEF(&property_entities_default_value);
	zend_string *property_entities_name = zend_string_init("entities", sizeof("entities") - 1, 1);
	zend_string *property_entities_class_DOMNamedNodeMap = zend_string_init("DOMNamedNodeMap", sizeof("DOMNamedNodeMap")-1, 1);
	zend_declare_typed_property(class_entry, property_entities_name, &property_entities_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_entities_class_DOMNamedNodeMap, 0, 0));
	zend_string_release(property_entities_name);

	zval property_notations_default_value;
	ZVAL_UNDEF(&property_notations_default_value);
	zend_string *property_notations_name = zend_string_init("notations", sizeof("notations") - 1, 1);
	zend_string *property_notations_class_DOMNamedNodeMap = zend_string_init("DOMNamedNodeMap", sizeof("DOMNamedNodeMap")-1, 1);
	zend_declare_typed_property(class_entry, property_notations_name, &property_notations_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_notations_class_DOMNamedNodeMap, 0, 0));
	zend_string_release(property_notations_name);

	zval property_publicId_default_value;
	ZVAL_UNDEF(&property_publicId_default_value);
	zend_string *property_publicId_name = zend_string_init("publicId", sizeof("publicId") - 1, 1);
	zend_declare_typed_property(class_entry, property_publicId_name, &property_publicId_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_publicId_name);

	zval property_systemId_default_value;
	ZVAL_UNDEF(&property_systemId_default_value);
	zend_string *property_systemId_name = zend_string_init("systemId", sizeof("systemId") - 1, 1);
	zend_declare_typed_property(class_entry, property_systemId_name, &property_systemId_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_systemId_name);

	zval property_internalSubset_default_value;
	ZVAL_UNDEF(&property_internalSubset_default_value);
	zend_string *property_internalSubset_name = zend_string_init("internalSubset", sizeof("internalSubset") - 1, 1);
	zend_declare_typed_property(class_entry, property_internalSubset_name, &property_internalSubset_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_internalSubset_name);

	return class_entry;
}

static zend_class_entry *register_class_DOMCdataSection(zend_class_entry *class_entry_DOMText)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMCdataSection", class_DOMCdataSection_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOMText);

	return class_entry;
}

static zend_class_entry *register_class_DOMComment(zend_class_entry *class_entry_DOMCharacterData)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMComment", class_DOMComment_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOMCharacterData);

	return class_entry;
}

static zend_class_entry *register_class_DOMParentNode(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMParentNode", class_DOMParentNode_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_DOMChildNode(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMChildNode", class_DOMChildNode_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_DOMNode(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMNode", class_DOMNode_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	zval const_DOCUMENT_POSITION_DISCONNECTED_value;
	ZVAL_LONG(&const_DOCUMENT_POSITION_DISCONNECTED_value, 0x1);
	zend_string *const_DOCUMENT_POSITION_DISCONNECTED_name = zend_string_init_interned("DOCUMENT_POSITION_DISCONNECTED", sizeof("DOCUMENT_POSITION_DISCONNECTED") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOCUMENT_POSITION_DISCONNECTED_name, &const_DOCUMENT_POSITION_DISCONNECTED_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOCUMENT_POSITION_DISCONNECTED_name);

	zval const_DOCUMENT_POSITION_PRECEDING_value;
	ZVAL_LONG(&const_DOCUMENT_POSITION_PRECEDING_value, 0x2);
	zend_string *const_DOCUMENT_POSITION_PRECEDING_name = zend_string_init_interned("DOCUMENT_POSITION_PRECEDING", sizeof("DOCUMENT_POSITION_PRECEDING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOCUMENT_POSITION_PRECEDING_name, &const_DOCUMENT_POSITION_PRECEDING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOCUMENT_POSITION_PRECEDING_name);

	zval const_DOCUMENT_POSITION_FOLLOWING_value;
	ZVAL_LONG(&const_DOCUMENT_POSITION_FOLLOWING_value, 0x4);
	zend_string *const_DOCUMENT_POSITION_FOLLOWING_name = zend_string_init_interned("DOCUMENT_POSITION_FOLLOWING", sizeof("DOCUMENT_POSITION_FOLLOWING") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOCUMENT_POSITION_FOLLOWING_name, &const_DOCUMENT_POSITION_FOLLOWING_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOCUMENT_POSITION_FOLLOWING_name);

	zval const_DOCUMENT_POSITION_CONTAINS_value;
	ZVAL_LONG(&const_DOCUMENT_POSITION_CONTAINS_value, 0x8);
	zend_string *const_DOCUMENT_POSITION_CONTAINS_name = zend_string_init_interned("DOCUMENT_POSITION_CONTAINS", sizeof("DOCUMENT_POSITION_CONTAINS") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOCUMENT_POSITION_CONTAINS_name, &const_DOCUMENT_POSITION_CONTAINS_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOCUMENT_POSITION_CONTAINS_name);

	zval const_DOCUMENT_POSITION_CONTAINED_BY_value;
	ZVAL_LONG(&const_DOCUMENT_POSITION_CONTAINED_BY_value, 0x10);
	zend_string *const_DOCUMENT_POSITION_CONTAINED_BY_name = zend_string_init_interned("DOCUMENT_POSITION_CONTAINED_BY", sizeof("DOCUMENT_POSITION_CONTAINED_BY") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOCUMENT_POSITION_CONTAINED_BY_name, &const_DOCUMENT_POSITION_CONTAINED_BY_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOCUMENT_POSITION_CONTAINED_BY_name);

	zval const_DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC_value;
	ZVAL_LONG(&const_DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC_value, 0x20);
	zend_string *const_DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC_name = zend_string_init_interned("DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC", sizeof("DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC") - 1, 1);
	zend_declare_typed_class_constant(class_entry, const_DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC_name, &const_DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(const_DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC_name);

	zval property_nodeName_default_value;
	ZVAL_UNDEF(&property_nodeName_default_value);
	zend_string *property_nodeName_name = zend_string_init("nodeName", sizeof("nodeName") - 1, 1);
	zend_declare_typed_property(class_entry, property_nodeName_name, &property_nodeName_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_nodeName_name);

	zval property_nodeValue_default_value;
	ZVAL_UNDEF(&property_nodeValue_default_value);
	zend_string *property_nodeValue_name = zend_string_init("nodeValue", sizeof("nodeValue") - 1, 1);
	zend_declare_typed_property(class_entry, property_nodeValue_name, &property_nodeValue_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_nodeValue_name);

	zval property_nodeType_default_value;
	ZVAL_UNDEF(&property_nodeType_default_value);
	zend_string *property_nodeType_name = zend_string_init("nodeType", sizeof("nodeType") - 1, 1);
	zend_declare_typed_property(class_entry, property_nodeType_name, &property_nodeType_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_nodeType_name);

	zval property_parentNode_default_value;
	ZVAL_UNDEF(&property_parentNode_default_value);
	zend_string *property_parentNode_name = zend_string_init("parentNode", sizeof("parentNode") - 1, 1);
	zend_string *property_parentNode_class_DOMNode = zend_string_init("DOMNode", sizeof("DOMNode")-1, 1);
	zend_declare_typed_property(class_entry, property_parentNode_name, &property_parentNode_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_parentNode_class_DOMNode, 0, MAY_BE_NULL));
	zend_string_release(property_parentNode_name);

	zval property_parentElement_default_value;
	ZVAL_UNDEF(&property_parentElement_default_value);
	zend_string *property_parentElement_name = zend_string_init("parentElement", sizeof("parentElement") - 1, 1);
	zend_string *property_parentElement_class_DOMElement = zend_string_init("DOMElement", sizeof("DOMElement")-1, 1);
	zend_declare_typed_property(class_entry, property_parentElement_name, &property_parentElement_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_parentElement_class_DOMElement, 0, MAY_BE_NULL));
	zend_string_release(property_parentElement_name);

	zval property_childNodes_default_value;
	ZVAL_UNDEF(&property_childNodes_default_value);
	zend_string *property_childNodes_name = zend_string_init("childNodes", sizeof("childNodes") - 1, 1);
	zend_string *property_childNodes_class_DOMNodeList = zend_string_init("DOMNodeList", sizeof("DOMNodeList")-1, 1);
	zend_declare_typed_property(class_entry, property_childNodes_name, &property_childNodes_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_childNodes_class_DOMNodeList, 0, 0));
	zend_string_release(property_childNodes_name);

	zval property_firstChild_default_value;
	ZVAL_UNDEF(&property_firstChild_default_value);
	zend_string *property_firstChild_name = zend_string_init("firstChild", sizeof("firstChild") - 1, 1);
	zend_string *property_firstChild_class_DOMNode = zend_string_init("DOMNode", sizeof("DOMNode")-1, 1);
	zend_declare_typed_property(class_entry, property_firstChild_name, &property_firstChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_firstChild_class_DOMNode, 0, MAY_BE_NULL));
	zend_string_release(property_firstChild_name);

	zval property_lastChild_default_value;
	ZVAL_UNDEF(&property_lastChild_default_value);
	zend_string *property_lastChild_name = zend_string_init("lastChild", sizeof("lastChild") - 1, 1);
	zend_string *property_lastChild_class_DOMNode = zend_string_init("DOMNode", sizeof("DOMNode")-1, 1);
	zend_declare_typed_property(class_entry, property_lastChild_name, &property_lastChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_lastChild_class_DOMNode, 0, MAY_BE_NULL));
	zend_string_release(property_lastChild_name);

	zval property_previousSibling_default_value;
	ZVAL_UNDEF(&property_previousSibling_default_value);
	zend_string *property_previousSibling_name = zend_string_init("previousSibling", sizeof("previousSibling") - 1, 1);
	zend_string *property_previousSibling_class_DOMNode = zend_string_init("DOMNode", sizeof("DOMNode")-1, 1);
	zend_declare_typed_property(class_entry, property_previousSibling_name, &property_previousSibling_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_previousSibling_class_DOMNode, 0, MAY_BE_NULL));
	zend_string_release(property_previousSibling_name);

	zval property_nextSibling_default_value;
	ZVAL_UNDEF(&property_nextSibling_default_value);
	zend_string *property_nextSibling_name = zend_string_init("nextSibling", sizeof("nextSibling") - 1, 1);
	zend_string *property_nextSibling_class_DOMNode = zend_string_init("DOMNode", sizeof("DOMNode")-1, 1);
	zend_declare_typed_property(class_entry, property_nextSibling_name, &property_nextSibling_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_nextSibling_class_DOMNode, 0, MAY_BE_NULL));
	zend_string_release(property_nextSibling_name);

	zval property_attributes_default_value;
	ZVAL_UNDEF(&property_attributes_default_value);
	zend_string *property_attributes_name = zend_string_init("attributes", sizeof("attributes") - 1, 1);
	zend_string *property_attributes_class_DOMNamedNodeMap = zend_string_init("DOMNamedNodeMap", sizeof("DOMNamedNodeMap")-1, 1);
	zend_declare_typed_property(class_entry, property_attributes_name, &property_attributes_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_attributes_class_DOMNamedNodeMap, 0, MAY_BE_NULL));
	zend_string_release(property_attributes_name);

	zval property_isConnected_default_value;
	ZVAL_UNDEF(&property_isConnected_default_value);
	zend_string *property_isConnected_name = zend_string_init("isConnected", sizeof("isConnected") - 1, 1);
	zend_declare_typed_property(class_entry, property_isConnected_name, &property_isConnected_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_isConnected_name);

	zval property_ownerDocument_default_value;
	ZVAL_UNDEF(&property_ownerDocument_default_value);
	zend_string *property_ownerDocument_name = zend_string_init("ownerDocument", sizeof("ownerDocument") - 1, 1);
	zend_string *property_ownerDocument_class_DOMDocument = zend_string_init("DOMDocument", sizeof("DOMDocument")-1, 1);
	zend_declare_typed_property(class_entry, property_ownerDocument_name, &property_ownerDocument_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_ownerDocument_class_DOMDocument, 0, MAY_BE_NULL));
	zend_string_release(property_ownerDocument_name);

	zval property_namespaceURI_default_value;
	ZVAL_UNDEF(&property_namespaceURI_default_value);
	zend_string *property_namespaceURI_name = zend_string_init("namespaceURI", sizeof("namespaceURI") - 1, 1);
	zend_declare_typed_property(class_entry, property_namespaceURI_name, &property_namespaceURI_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_namespaceURI_name);

	zval property_prefix_default_value;
	ZVAL_UNDEF(&property_prefix_default_value);
	zend_string *property_prefix_name = zend_string_init("prefix", sizeof("prefix") - 1, 1);
	zend_declare_typed_property(class_entry, property_prefix_name, &property_prefix_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_prefix_name);

	zval property_localName_default_value;
	ZVAL_UNDEF(&property_localName_default_value);
	zend_string *property_localName_name = zend_string_init("localName", sizeof("localName") - 1, 1);
	zend_declare_typed_property(class_entry, property_localName_name, &property_localName_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_localName_name);

	zval property_baseURI_default_value;
	ZVAL_UNDEF(&property_baseURI_default_value);
	zend_string *property_baseURI_name = zend_string_init("baseURI", sizeof("baseURI") - 1, 1);
	zend_declare_typed_property(class_entry, property_baseURI_name, &property_baseURI_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_baseURI_name);

	zval property_textContent_default_value;
	ZVAL_UNDEF(&property_textContent_default_value);
	zend_string *property_textContent_name = zend_string_init("textContent", sizeof("textContent") - 1, 1);
	zend_declare_typed_property(class_entry, property_textContent_name, &property_textContent_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_textContent_name);

	return class_entry;
}

static zend_class_entry *register_class_DOMNameSpaceNode(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMNameSpaceNode", class_DOMNameSpaceNode_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	zval property_nodeName_default_value;
	ZVAL_UNDEF(&property_nodeName_default_value);
	zend_string *property_nodeName_name = zend_string_init("nodeName", sizeof("nodeName") - 1, 1);
	zend_declare_typed_property(class_entry, property_nodeName_name, &property_nodeName_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_nodeName_name);

	zval property_nodeValue_default_value;
	ZVAL_UNDEF(&property_nodeValue_default_value);
	zend_string *property_nodeValue_name = zend_string_init("nodeValue", sizeof("nodeValue") - 1, 1);
	zend_declare_typed_property(class_entry, property_nodeValue_name, &property_nodeValue_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_nodeValue_name);

	zval property_nodeType_default_value;
	ZVAL_UNDEF(&property_nodeType_default_value);
	zend_string *property_nodeType_name = zend_string_init("nodeType", sizeof("nodeType") - 1, 1);
	zend_declare_typed_property(class_entry, property_nodeType_name, &property_nodeType_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_nodeType_name);

	zval property_prefix_default_value;
	ZVAL_UNDEF(&property_prefix_default_value);
	zend_string *property_prefix_name = zend_string_init("prefix", sizeof("prefix") - 1, 1);
	zend_declare_typed_property(class_entry, property_prefix_name, &property_prefix_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_prefix_name);

	zval property_localName_default_value;
	ZVAL_UNDEF(&property_localName_default_value);
	zend_string *property_localName_name = zend_string_init("localName", sizeof("localName") - 1, 1);
	zend_declare_typed_property(class_entry, property_localName_name, &property_localName_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_localName_name);

	zval property_namespaceURI_default_value;
	ZVAL_UNDEF(&property_namespaceURI_default_value);
	zend_string *property_namespaceURI_name = zend_string_init("namespaceURI", sizeof("namespaceURI") - 1, 1);
	zend_declare_typed_property(class_entry, property_namespaceURI_name, &property_namespaceURI_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_namespaceURI_name);

	zval property_isConnected_default_value;
	ZVAL_UNDEF(&property_isConnected_default_value);
	zend_string *property_isConnected_name = zend_string_init("isConnected", sizeof("isConnected") - 1, 1);
	zend_declare_typed_property(class_entry, property_isConnected_name, &property_isConnected_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_isConnected_name);

	zval property_ownerDocument_default_value;
	ZVAL_UNDEF(&property_ownerDocument_default_value);
	zend_string *property_ownerDocument_name = zend_string_init("ownerDocument", sizeof("ownerDocument") - 1, 1);
	zend_string *property_ownerDocument_class_DOMDocument = zend_string_init("DOMDocument", sizeof("DOMDocument")-1, 1);
	zend_declare_typed_property(class_entry, property_ownerDocument_name, &property_ownerDocument_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_ownerDocument_class_DOMDocument, 0, MAY_BE_NULL));
	zend_string_release(property_ownerDocument_name);

	zval property_parentNode_default_value;
	ZVAL_UNDEF(&property_parentNode_default_value);
	zend_string *property_parentNode_name = zend_string_init("parentNode", sizeof("parentNode") - 1, 1);
	zend_string *property_parentNode_class_DOMNode = zend_string_init("DOMNode", sizeof("DOMNode")-1, 1);
	zend_declare_typed_property(class_entry, property_parentNode_name, &property_parentNode_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_parentNode_class_DOMNode, 0, MAY_BE_NULL));
	zend_string_release(property_parentNode_name);

	zval property_parentElement_default_value;
	ZVAL_UNDEF(&property_parentElement_default_value);
	zend_string *property_parentElement_name = zend_string_init("parentElement", sizeof("parentElement") - 1, 1);
	zend_string *property_parentElement_class_DOMElement = zend_string_init("DOMElement", sizeof("DOMElement")-1, 1);
	zend_declare_typed_property(class_entry, property_parentElement_name, &property_parentElement_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_parentElement_class_DOMElement, 0, MAY_BE_NULL));
	zend_string_release(property_parentElement_name);

	return class_entry;
}

static zend_class_entry *register_class_DOMImplementation(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMImplementation", class_DOMImplementation_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}

static zend_class_entry *register_class_DOMDocumentFragment(zend_class_entry *class_entry_DOMNode, zend_class_entry *class_entry_DOMParentNode)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMDocumentFragment", class_DOMDocumentFragment_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOMNode);
	zend_class_implements(class_entry, 1, class_entry_DOMParentNode);

	zval property_firstElementChild_default_value;
	ZVAL_UNDEF(&property_firstElementChild_default_value);
	zend_string *property_firstElementChild_name = zend_string_init("firstElementChild", sizeof("firstElementChild") - 1, 1);
	zend_string *property_firstElementChild_class_DOMElement = zend_string_init("DOMElement", sizeof("DOMElement")-1, 1);
	zend_declare_typed_property(class_entry, property_firstElementChild_name, &property_firstElementChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_firstElementChild_class_DOMElement, 0, MAY_BE_NULL));
	zend_string_release(property_firstElementChild_name);

	zval property_lastElementChild_default_value;
	ZVAL_UNDEF(&property_lastElementChild_default_value);
	zend_string *property_lastElementChild_name = zend_string_init("lastElementChild", sizeof("lastElementChild") - 1, 1);
	zend_string *property_lastElementChild_class_DOMElement = zend_string_init("DOMElement", sizeof("DOMElement")-1, 1);
	zend_declare_typed_property(class_entry, property_lastElementChild_name, &property_lastElementChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_lastElementChild_class_DOMElement, 0, MAY_BE_NULL));
	zend_string_release(property_lastElementChild_name);

	zval property_childElementCount_default_value;
	ZVAL_UNDEF(&property_childElementCount_default_value);
	zend_string *property_childElementCount_name = zend_string_init("childElementCount", sizeof("childElementCount") - 1, 1);
	zend_declare_typed_property(class_entry, property_childElementCount_name, &property_childElementCount_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_childElementCount_name);

	return class_entry;
}

static zend_class_entry *register_class_DOMNodeList(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMNodeList", class_DOMNodeList_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 2, class_entry_IteratorAggregate, class_entry_Countable);

	zval property_length_default_value;
	ZVAL_UNDEF(&property_length_default_value);
	zend_string *property_length_name = zend_string_init("length", sizeof("length") - 1, 1);
	zend_declare_typed_property(class_entry, property_length_name, &property_length_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_length_name);

	return class_entry;
}

static zend_class_entry *register_class_DOMCharacterData(zend_class_entry *class_entry_DOMNode, zend_class_entry *class_entry_DOMChildNode)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMCharacterData", class_DOMCharacterData_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOMNode);
	zend_class_implements(class_entry, 1, class_entry_DOMChildNode);

	zval property_data_default_value;
	ZVAL_UNDEF(&property_data_default_value);
	zend_string *property_data_name = zend_string_init("data", sizeof("data") - 1, 1);
	zend_declare_typed_property(class_entry, property_data_name, &property_data_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_data_name);

	zval property_length_default_value;
	ZVAL_UNDEF(&property_length_default_value);
	zend_string *property_length_name = zend_string_init("length", sizeof("length") - 1, 1);
	zend_declare_typed_property(class_entry, property_length_name, &property_length_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_length_name);

	zval property_previousElementSibling_default_value;
	ZVAL_UNDEF(&property_previousElementSibling_default_value);
	zend_string *property_previousElementSibling_name = zend_string_init("previousElementSibling", sizeof("previousElementSibling") - 1, 1);
	zend_string *property_previousElementSibling_class_DOMElement = zend_string_init("DOMElement", sizeof("DOMElement")-1, 1);
	zend_declare_typed_property(class_entry, property_previousElementSibling_name, &property_previousElementSibling_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_previousElementSibling_class_DOMElement, 0, MAY_BE_NULL));
	zend_string_release(property_previousElementSibling_name);

	zval property_nextElementSibling_default_value;
	ZVAL_UNDEF(&property_nextElementSibling_default_value);
	zend_string *property_nextElementSibling_name = zend_string_init("nextElementSibling", sizeof("nextElementSibling") - 1, 1);
	zend_string *property_nextElementSibling_class_DOMElement = zend_string_init("DOMElement", sizeof("DOMElement")-1, 1);
	zend_declare_typed_property(class_entry, property_nextElementSibling_name, &property_nextElementSibling_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_nextElementSibling_class_DOMElement, 0, MAY_BE_NULL));
	zend_string_release(property_nextElementSibling_name);

	return class_entry;
}

static zend_class_entry *register_class_DOMAttr(zend_class_entry *class_entry_DOMNode)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMAttr", class_DOMAttr_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOMNode);

	zval property_name_default_value;
	ZVAL_UNDEF(&property_name_default_value);
	zend_string *property_name_name = zend_string_init("name", sizeof("name") - 1, 1);
	zend_declare_typed_property(class_entry, property_name_name, &property_name_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_name_name);

	zval property_specified_default_value;
	ZVAL_TRUE(&property_specified_default_value);
	zend_string *property_specified_name = zend_string_init("specified", sizeof("specified") - 1, 1);
	zend_declare_typed_property(class_entry, property_specified_name, &property_specified_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_specified_name);

	zval property_value_default_value;
	ZVAL_UNDEF(&property_value_default_value);
	zend_string *property_value_name = zend_string_init("value", sizeof("value") - 1, 1);
	zend_declare_typed_property(class_entry, property_value_name, &property_value_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_value_name);

	zval property_ownerElement_default_value;
	ZVAL_UNDEF(&property_ownerElement_default_value);
	zend_string *property_ownerElement_name = zend_string_init("ownerElement", sizeof("ownerElement") - 1, 1);
	zend_string *property_ownerElement_class_DOMElement = zend_string_init("DOMElement", sizeof("DOMElement")-1, 1);
	zend_declare_typed_property(class_entry, property_ownerElement_name, &property_ownerElement_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_ownerElement_class_DOMElement, 0, MAY_BE_NULL));
	zend_string_release(property_ownerElement_name);

	zval property_schemaTypeInfo_default_value;
	ZVAL_NULL(&property_schemaTypeInfo_default_value);
	zend_string *property_schemaTypeInfo_name = zend_string_init("schemaTypeInfo", sizeof("schemaTypeInfo") - 1, 1);
	zend_declare_typed_property(class_entry, property_schemaTypeInfo_name, &property_schemaTypeInfo_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ANY));
	zend_string_release(property_schemaTypeInfo_name);

	return class_entry;
}

static zend_class_entry *register_class_DOMElement(zend_class_entry *class_entry_DOMNode, zend_class_entry *class_entry_DOMParentNode, zend_class_entry *class_entry_DOMChildNode)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMElement", class_DOMElement_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOMNode);
	zend_class_implements(class_entry, 2, class_entry_DOMParentNode, class_entry_DOMChildNode);

	zval property_tagName_default_value;
	ZVAL_UNDEF(&property_tagName_default_value);
	zend_string *property_tagName_name = zend_string_init("tagName", sizeof("tagName") - 1, 1);
	zend_declare_typed_property(class_entry, property_tagName_name, &property_tagName_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_tagName_name);

	zval property_className_default_value;
	ZVAL_UNDEF(&property_className_default_value);
	zend_string *property_className_name = zend_string_init("className", sizeof("className") - 1, 1);
	zend_declare_typed_property(class_entry, property_className_name, &property_className_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_className_name);

	zval property_id_default_value;
	ZVAL_UNDEF(&property_id_default_value);
	zend_string *property_id_name = zend_string_init("id", sizeof("id") - 1, 1);
	zend_declare_typed_property(class_entry, property_id_name, &property_id_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_id_name);

	zval property_schemaTypeInfo_default_value;
	ZVAL_NULL(&property_schemaTypeInfo_default_value);
	zend_string *property_schemaTypeInfo_name = zend_string_init("schemaTypeInfo", sizeof("schemaTypeInfo") - 1, 1);
	zend_declare_typed_property(class_entry, property_schemaTypeInfo_name, &property_schemaTypeInfo_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ANY));
	zend_string_release(property_schemaTypeInfo_name);

	zval property_firstElementChild_default_value;
	ZVAL_UNDEF(&property_firstElementChild_default_value);
	zend_string *property_firstElementChild_name = zend_string_init("firstElementChild", sizeof("firstElementChild") - 1, 1);
	zend_string *property_firstElementChild_class_DOMElement = zend_string_init("DOMElement", sizeof("DOMElement")-1, 1);
	zend_declare_typed_property(class_entry, property_firstElementChild_name, &property_firstElementChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_firstElementChild_class_DOMElement, 0, MAY_BE_NULL));
	zend_string_release(property_firstElementChild_name);

	zval property_lastElementChild_default_value;
	ZVAL_UNDEF(&property_lastElementChild_default_value);
	zend_string *property_lastElementChild_name = zend_string_init("lastElementChild", sizeof("lastElementChild") - 1, 1);
	zend_string *property_lastElementChild_class_DOMElement = zend_string_init("DOMElement", sizeof("DOMElement")-1, 1);
	zend_declare_typed_property(class_entry, property_lastElementChild_name, &property_lastElementChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_lastElementChild_class_DOMElement, 0, MAY_BE_NULL));
	zend_string_release(property_lastElementChild_name);

	zval property_childElementCount_default_value;
	ZVAL_UNDEF(&property_childElementCount_default_value);
	zend_string *property_childElementCount_name = zend_string_init("childElementCount", sizeof("childElementCount") - 1, 1);
	zend_declare_typed_property(class_entry, property_childElementCount_name, &property_childElementCount_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_childElementCount_name);

	zval property_previousElementSibling_default_value;
	ZVAL_UNDEF(&property_previousElementSibling_default_value);
	zend_string *property_previousElementSibling_name = zend_string_init("previousElementSibling", sizeof("previousElementSibling") - 1, 1);
	zend_string *property_previousElementSibling_class_DOMElement = zend_string_init("DOMElement", sizeof("DOMElement")-1, 1);
	zend_declare_typed_property(class_entry, property_previousElementSibling_name, &property_previousElementSibling_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_previousElementSibling_class_DOMElement, 0, MAY_BE_NULL));
	zend_string_release(property_previousElementSibling_name);

	zval property_nextElementSibling_default_value;
	ZVAL_UNDEF(&property_nextElementSibling_default_value);
	zend_string *property_nextElementSibling_name = zend_string_init("nextElementSibling", sizeof("nextElementSibling") - 1, 1);
	zend_string *property_nextElementSibling_class_DOMElement = zend_string_init("DOMElement", sizeof("DOMElement")-1, 1);
	zend_declare_typed_property(class_entry, property_nextElementSibling_name, &property_nextElementSibling_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_nextElementSibling_class_DOMElement, 0, MAY_BE_NULL));
	zend_string_release(property_nextElementSibling_name);

	return class_entry;
}

static zend_class_entry *register_class_DOMDocument(zend_class_entry *class_entry_DOMNode, zend_class_entry *class_entry_DOMParentNode)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMDocument", class_DOMDocument_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOMNode);
	zend_class_implements(class_entry, 1, class_entry_DOMParentNode);

	zval property_doctype_default_value;
	ZVAL_UNDEF(&property_doctype_default_value);
	zend_string *property_doctype_name = zend_string_init("doctype", sizeof("doctype") - 1, 1);
	zend_string *property_doctype_class_DOMDocumentType = zend_string_init("DOMDocumentType", sizeof("DOMDocumentType")-1, 1);
	zend_declare_typed_property(class_entry, property_doctype_name, &property_doctype_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_doctype_class_DOMDocumentType, 0, MAY_BE_NULL));
	zend_string_release(property_doctype_name);

	zval property_implementation_default_value;
	ZVAL_UNDEF(&property_implementation_default_value);
	zend_string *property_implementation_name = zend_string_init("implementation", sizeof("implementation") - 1, 1);
	zend_string *property_implementation_class_DOMImplementation = zend_string_init("DOMImplementation", sizeof("DOMImplementation")-1, 1);
	zend_declare_typed_property(class_entry, property_implementation_name, &property_implementation_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_implementation_class_DOMImplementation, 0, 0));
	zend_string_release(property_implementation_name);

	zval property_documentElement_default_value;
	ZVAL_UNDEF(&property_documentElement_default_value);
	zend_string *property_documentElement_name = zend_string_init("documentElement", sizeof("documentElement") - 1, 1);
	zend_string *property_documentElement_class_DOMElement = zend_string_init("DOMElement", sizeof("DOMElement")-1, 1);
	zend_declare_typed_property(class_entry, property_documentElement_name, &property_documentElement_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_documentElement_class_DOMElement, 0, MAY_BE_NULL));
	zend_string_release(property_documentElement_name);

	zval property_actualEncoding_default_value;
	ZVAL_UNDEF(&property_actualEncoding_default_value);
	zend_string *property_actualEncoding_name = zend_string_init("actualEncoding", sizeof("actualEncoding") - 1, 1);
	zend_declare_typed_property(class_entry, property_actualEncoding_name, &property_actualEncoding_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_actualEncoding_name);

	zval property_encoding_default_value;
	ZVAL_UNDEF(&property_encoding_default_value);
	zend_string *property_encoding_name = zend_string_init("encoding", sizeof("encoding") - 1, 1);
	zend_declare_typed_property(class_entry, property_encoding_name, &property_encoding_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_encoding_name);

	zval property_xmlEncoding_default_value;
	ZVAL_UNDEF(&property_xmlEncoding_default_value);
	zend_string *property_xmlEncoding_name = zend_string_init("xmlEncoding", sizeof("xmlEncoding") - 1, 1);
	zend_declare_typed_property(class_entry, property_xmlEncoding_name, &property_xmlEncoding_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_xmlEncoding_name);

	zval property_standalone_default_value;
	ZVAL_UNDEF(&property_standalone_default_value);
	zend_string *property_standalone_name = zend_string_init("standalone", sizeof("standalone") - 1, 1);
	zend_declare_typed_property(class_entry, property_standalone_name, &property_standalone_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_standalone_name);

	zval property_xmlStandalone_default_value;
	ZVAL_UNDEF(&property_xmlStandalone_default_value);
	zend_string *property_xmlStandalone_name = zend_string_init("xmlStandalone", sizeof("xmlStandalone") - 1, 1);
	zend_declare_typed_property(class_entry, property_xmlStandalone_name, &property_xmlStandalone_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_xmlStandalone_name);

	zval property_version_default_value;
	ZVAL_UNDEF(&property_version_default_value);
	zend_string *property_version_name = zend_string_init("version", sizeof("version") - 1, 1);
	zend_declare_typed_property(class_entry, property_version_name, &property_version_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_version_name);

	zval property_xmlVersion_default_value;
	ZVAL_UNDEF(&property_xmlVersion_default_value);
	zend_string *property_xmlVersion_name = zend_string_init("xmlVersion", sizeof("xmlVersion") - 1, 1);
	zend_declare_typed_property(class_entry, property_xmlVersion_name, &property_xmlVersion_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_xmlVersion_name);

	zval property_strictErrorChecking_default_value;
	ZVAL_UNDEF(&property_strictErrorChecking_default_value);
	zend_string *property_strictErrorChecking_name = zend_string_init("strictErrorChecking", sizeof("strictErrorChecking") - 1, 1);
	zend_declare_typed_property(class_entry, property_strictErrorChecking_name, &property_strictErrorChecking_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_strictErrorChecking_name);

	zval property_documentURI_default_value;
	ZVAL_UNDEF(&property_documentURI_default_value);
	zend_string *property_documentURI_name = zend_string_init("documentURI", sizeof("documentURI") - 1, 1);
	zend_declare_typed_property(class_entry, property_documentURI_name, &property_documentURI_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_documentURI_name);

	zval property_config_default_value;
	ZVAL_UNDEF(&property_config_default_value);
	zend_string *property_config_name = zend_string_init("config", sizeof("config") - 1, 1);
	zend_declare_typed_property(class_entry, property_config_name, &property_config_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_ANY));
	zend_string_release(property_config_name);

	zval property_formatOutput_default_value;
	ZVAL_UNDEF(&property_formatOutput_default_value);
	zend_string *property_formatOutput_name = zend_string_init("formatOutput", sizeof("formatOutput") - 1, 1);
	zend_declare_typed_property(class_entry, property_formatOutput_name, &property_formatOutput_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_formatOutput_name);

	zval property_validateOnParse_default_value;
	ZVAL_UNDEF(&property_validateOnParse_default_value);
	zend_string *property_validateOnParse_name = zend_string_init("validateOnParse", sizeof("validateOnParse") - 1, 1);
	zend_declare_typed_property(class_entry, property_validateOnParse_name, &property_validateOnParse_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_validateOnParse_name);

	zval property_resolveExternals_default_value;
	ZVAL_UNDEF(&property_resolveExternals_default_value);
	zend_string *property_resolveExternals_name = zend_string_init("resolveExternals", sizeof("resolveExternals") - 1, 1);
	zend_declare_typed_property(class_entry, property_resolveExternals_name, &property_resolveExternals_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_resolveExternals_name);

	zval property_preserveWhiteSpace_default_value;
	ZVAL_UNDEF(&property_preserveWhiteSpace_default_value);
	zend_string *property_preserveWhiteSpace_name = zend_string_init("preserveWhiteSpace", sizeof("preserveWhiteSpace") - 1, 1);
	zend_declare_typed_property(class_entry, property_preserveWhiteSpace_name, &property_preserveWhiteSpace_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_preserveWhiteSpace_name);

	zval property_recover_default_value;
	ZVAL_UNDEF(&property_recover_default_value);
	zend_string *property_recover_name = zend_string_init("recover", sizeof("recover") - 1, 1);
	zend_declare_typed_property(class_entry, property_recover_name, &property_recover_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_recover_name);

	zval property_substituteEntities_default_value;
	ZVAL_UNDEF(&property_substituteEntities_default_value);
	zend_string *property_substituteEntities_name = zend_string_init("substituteEntities", sizeof("substituteEntities") - 1, 1);
	zend_declare_typed_property(class_entry, property_substituteEntities_name, &property_substituteEntities_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_substituteEntities_name);

	zval property_firstElementChild_default_value;
	ZVAL_UNDEF(&property_firstElementChild_default_value);
	zend_string *property_firstElementChild_name = zend_string_init("firstElementChild", sizeof("firstElementChild") - 1, 1);
	zend_string *property_firstElementChild_class_DOMElement = zend_string_init("DOMElement", sizeof("DOMElement")-1, 1);
	zend_declare_typed_property(class_entry, property_firstElementChild_name, &property_firstElementChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_firstElementChild_class_DOMElement, 0, MAY_BE_NULL));
	zend_string_release(property_firstElementChild_name);

	zval property_lastElementChild_default_value;
	ZVAL_UNDEF(&property_lastElementChild_default_value);
	zend_string *property_lastElementChild_name = zend_string_init("lastElementChild", sizeof("lastElementChild") - 1, 1);
	zend_string *property_lastElementChild_class_DOMElement = zend_string_init("DOMElement", sizeof("DOMElement")-1, 1);
	zend_declare_typed_property(class_entry, property_lastElementChild_name, &property_lastElementChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_lastElementChild_class_DOMElement, 0, MAY_BE_NULL));
	zend_string_release(property_lastElementChild_name);

	zval property_childElementCount_default_value;
	ZVAL_UNDEF(&property_childElementCount_default_value);
	zend_string *property_childElementCount_name = zend_string_init("childElementCount", sizeof("childElementCount") - 1, 1);
	zend_declare_typed_property(class_entry, property_childElementCount_name, &property_childElementCount_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_childElementCount_name);

	return class_entry;
}

static zend_class_entry *register_class_DOMException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMException", class_DOMException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Exception);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	zval property_code_default_value;
	ZVAL_LONG(&property_code_default_value, 0);
	zend_string *property_code_name = zend_string_init("code", sizeof("code") - 1, 1);
	zend_declare_typed_property(class_entry, property_code_name, &property_code_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_NONE(0));
	zend_string_release(property_code_name);

	return class_entry;
}

static zend_class_entry *register_class_DOMText(zend_class_entry *class_entry_DOMCharacterData)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMText", class_DOMText_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOMCharacterData);

	zval property_wholeText_default_value;
	ZVAL_UNDEF(&property_wholeText_default_value);
	zend_string *property_wholeText_name = zend_string_init("wholeText", sizeof("wholeText") - 1, 1);
	zend_declare_typed_property(class_entry, property_wholeText_name, &property_wholeText_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_wholeText_name);

	return class_entry;
}

static zend_class_entry *register_class_DOMNamedNodeMap(zend_class_entry *class_entry_IteratorAggregate, zend_class_entry *class_entry_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMNamedNodeMap", class_DOMNamedNodeMap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 2, class_entry_IteratorAggregate, class_entry_Countable);

	zval property_length_default_value;
	ZVAL_UNDEF(&property_length_default_value);
	zend_string *property_length_name = zend_string_init("length", sizeof("length") - 1, 1);
	zend_declare_typed_property(class_entry, property_length_name, &property_length_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_length_name);

	return class_entry;
}

static zend_class_entry *register_class_DOMEntity(zend_class_entry *class_entry_DOMNode)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMEntity", class_DOMEntity_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOMNode);

	zval property_publicId_default_value;
	ZVAL_UNDEF(&property_publicId_default_value);
	zend_string *property_publicId_name = zend_string_init("publicId", sizeof("publicId") - 1, 1);
	zend_declare_typed_property(class_entry, property_publicId_name, &property_publicId_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_publicId_name);

	zval property_systemId_default_value;
	ZVAL_UNDEF(&property_systemId_default_value);
	zend_string *property_systemId_name = zend_string_init("systemId", sizeof("systemId") - 1, 1);
	zend_declare_typed_property(class_entry, property_systemId_name, &property_systemId_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_systemId_name);

	zval property_notationName_default_value;
	ZVAL_UNDEF(&property_notationName_default_value);
	zend_string *property_notationName_name = zend_string_init("notationName", sizeof("notationName") - 1, 1);
	zend_declare_typed_property(class_entry, property_notationName_name, &property_notationName_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_notationName_name);

	zval property_actualEncoding_default_value;
	ZVAL_NULL(&property_actualEncoding_default_value);
	zend_string *property_actualEncoding_name = zend_string_init("actualEncoding", sizeof("actualEncoding") - 1, 1);
	zend_declare_typed_property(class_entry, property_actualEncoding_name, &property_actualEncoding_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_actualEncoding_name);

	zval property_encoding_default_value;
	ZVAL_NULL(&property_encoding_default_value);
	zend_string *property_encoding_name = zend_string_init("encoding", sizeof("encoding") - 1, 1);
	zend_declare_typed_property(class_entry, property_encoding_name, &property_encoding_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_encoding_name);

	zval property_version_default_value;
	ZVAL_NULL(&property_version_default_value);
	zend_string *property_version_name = zend_string_init("version", sizeof("version") - 1, 1);
	zend_declare_typed_property(class_entry, property_version_name, &property_version_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_version_name);

	return class_entry;
}

static zend_class_entry *register_class_DOMEntityReference(zend_class_entry *class_entry_DOMNode)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMEntityReference", class_DOMEntityReference_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOMNode);

	return class_entry;
}

static zend_class_entry *register_class_DOMNotation(zend_class_entry *class_entry_DOMNode)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMNotation", class_DOMNotation_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOMNode);

	zval property_publicId_default_value;
	ZVAL_UNDEF(&property_publicId_default_value);
	zend_string *property_publicId_name = zend_string_init("publicId", sizeof("publicId") - 1, 1);
	zend_declare_typed_property(class_entry, property_publicId_name, &property_publicId_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_publicId_name);

	zval property_systemId_default_value;
	ZVAL_UNDEF(&property_systemId_default_value);
	zend_string *property_systemId_name = zend_string_init("systemId", sizeof("systemId") - 1, 1);
	zend_declare_typed_property(class_entry, property_systemId_name, &property_systemId_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_systemId_name);

	return class_entry;
}

static zend_class_entry *register_class_DOMProcessingInstruction(zend_class_entry *class_entry_DOMNode)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMProcessingInstruction", class_DOMProcessingInstruction_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOMNode);

	zval property_target_default_value;
	ZVAL_UNDEF(&property_target_default_value);
	zend_string *property_target_name = zend_string_init("target", sizeof("target") - 1, 1);
	zend_declare_typed_property(class_entry, property_target_name, &property_target_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_target_name);

	zval property_data_default_value;
	ZVAL_UNDEF(&property_data_default_value);
	zend_string *property_data_name = zend_string_init("data", sizeof("data") - 1, 1);
	zend_declare_typed_property(class_entry, property_data_name, &property_data_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_data_name);

	return class_entry;
}

#if defined(LIBXML_XPATH_ENABLED)
static zend_class_entry *register_class_DOMXPath(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "DOMXPath", class_DOMXPath_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

	zval property_document_default_value;
	ZVAL_UNDEF(&property_document_default_value);
	zend_string *property_document_name = zend_string_init("document", sizeof("document") - 1, 1);
	zend_string *property_document_class_DOMDocument = zend_string_init("DOMDocument", sizeof("DOMDocument")-1, 1);
	zend_declare_typed_property(class_entry, property_document_name, &property_document_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_document_class_DOMDocument, 0, 0));
	zend_string_release(property_document_name);

	zval property_registerNodeNamespaces_default_value;
	ZVAL_UNDEF(&property_registerNodeNamespaces_default_value);
	zend_string *property_registerNodeNamespaces_name = zend_string_init("registerNodeNamespaces", sizeof("registerNodeNamespaces") - 1, 1);
	zend_declare_typed_property(class_entry, property_registerNodeNamespaces_name, &property_registerNodeNamespaces_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_registerNodeNamespaces_name);

	return class_entry;
}
#endif
