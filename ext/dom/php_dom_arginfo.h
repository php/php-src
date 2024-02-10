/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 0ca751c6e5ffbe432208649cfd2ff20aadda5275 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_dom_import_simplexml, 0, 1, DOMElement, 0)
	ZEND_ARG_TYPE_INFO(0, node, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_DOM_import_simplexml, 0, 1, DOM\\Element, 0)
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMNode___sleep, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMNode___wakeup arginfo_class_DOMChildNode_remove

#define arginfo_class_DOMNameSpaceNode___sleep arginfo_class_DOMNode___sleep

#define arginfo_class_DOMNameSpaceNode___wakeup arginfo_class_DOMChildNode_remove

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

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX(arginfo_class_DOMImplementation_createDocument, 0, 0, DOMDocument, 0)
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

#define arginfo_class_DOMElement_getAttributeNames arginfo_class_DOMNode___sleep

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

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_DOMDocument_registerNodeClass, 0, 2, IS_TRUE, 0)
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

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMXPath_registerPhpFunctionNS, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callable, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMXPath_quote, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_ParentNode_append, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_OBJ_TYPE_MASK(0, nodes, DOM\\\116ode, MAY_BE_STRING)
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_ParentNode_prepend arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_ParentNode_replaceChildren arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_ChildNode_remove arginfo_class_DOMChildNode_remove

#define arginfo_class_DOM_ChildNode_before arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_ChildNode_after arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_ChildNode_replaceWith arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_Node___construct arginfo_class_DOMDocumentFragment___construct

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Node_getRootNode, 0, 0, DOM\\\116ode, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Node_hasChildNodes, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_Node_normalize arginfo_class_DOMChildNode_remove

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Node_cloneNode, 0, 0, DOM\\\116ode, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, deep, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Node_isEqualNode, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, otherNode, DOM\\\116ode, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_Node_isSameNode arginfo_class_DOM_Node_isEqualNode

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Node_compareDocumentPosition, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, other, DOM\\\116ode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Node_contains, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, other, DOM\\\116ode, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Node_lookupPrefix, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Node_lookupNamespaceURI, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Node_isDefaultNamespace, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Node_insertBefore, 0, 2, DOM\\\116ode, 0)
	ZEND_ARG_OBJ_INFO(0, node, DOM\\\116ode, 0)
	ZEND_ARG_OBJ_INFO(0, child, DOM\\\116ode, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Node_appendChild, 0, 1, DOM\\\116ode, 0)
	ZEND_ARG_OBJ_INFO(0, node, DOM\\\116ode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Node_replaceChild, 0, 2, DOM\\\116ode, 0)
	ZEND_ARG_OBJ_INFO(0, node, DOM\\\116ode, 0)
	ZEND_ARG_OBJ_INFO(0, child, DOM\\\116ode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Node_removeChild, 0, 1, DOM\\\116ode, 0)
	ZEND_ARG_OBJ_INFO(0, child, DOM\\\116ode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Node_getLineNo, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Node_getNodePath, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_DOM_Node_C14N, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, exclusive, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, withComments, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, xpath, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nsPrefixes, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_DOM_Node_C14NFile, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, exclusive, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, withComments, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, xpath, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nsPrefixes, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_Node___sleep arginfo_class_DOMNode___sleep

#define arginfo_class_DOM_Node___wakeup arginfo_class_DOMChildNode_remove

#define arginfo_class_DOM_NodeList_count arginfo_class_DOMNode_getLineNo

#define arginfo_class_DOM_NodeList_getIterator arginfo_class_DOMNodeList_getIterator

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_NodeList_item, 0, 1, DOM\\\116ode, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_NamedNodeMap_item, 0, 1, DOM\\Attr, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_NamedNodeMap_getNamedItem, 0, 1, DOM\\Attr, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_NamedNodeMap_getNamedItemNS, 0, 2, DOM\\Attr, 1)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_NamedNodeMap_count arginfo_class_DOMNode_getLineNo

#define arginfo_class_DOM_NamedNodeMap_getIterator arginfo_class_DOMNodeList_getIterator

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_HTMLCollection_item, 0, 1, DOM\\Element, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_HTMLCollection_count arginfo_class_DOMNode_getLineNo

#define arginfo_class_DOM_HTMLCollection_getIterator arginfo_class_DOMNodeList_getIterator

#define arginfo_class_DOM_Element_hasAttributes arginfo_class_DOM_Node_hasChildNodes

#define arginfo_class_DOM_Element_getAttributeNames arginfo_class_DOMNode___sleep

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Element_getAttribute, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Element_getAttributeNS, 0, 2, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Element_setAttribute, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Element_setAttributeNS, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Element_removeAttribute, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Element_removeAttributeNS, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_Element_toggleAttribute arginfo_class_DOMElement_toggleAttribute

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Element_hasAttribute, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Element_hasAttributeNS, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_Element_getAttributeNode arginfo_class_DOM_NamedNodeMap_getNamedItem

#define arginfo_class_DOM_Element_getAttributeNodeNS arginfo_class_DOM_NamedNodeMap_getNamedItemNS

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Element_setAttributeNode, 0, 1, DOM\\Attr, 1)
	ZEND_ARG_OBJ_INFO(0, attr, DOM\\Attr, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_Element_setAttributeNodeNS arginfo_class_DOM_Element_setAttributeNode

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Element_removeAttributeNode, 0, 1, DOM\\Attr, 0)
	ZEND_ARG_OBJ_INFO(0, attr, DOM\\Attr, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Element_getElementsByTagName, 0, 1, DOM\\HTMLCollection, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Element_getElementsByTagNameNS, 0, 2, DOM\\HTMLCollection, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Element_insertAdjacentElement, 0, 2, DOM\\Element, 1)
	ZEND_ARG_TYPE_INFO(0, where, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, element, DOM\\Element, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_Element_insertAdjacentText arginfo_class_DOMElement_insertAdjacentText

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Element_setIdAttribute, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isId, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Element_setIdAttributeNS, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isId, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Element_setIdAttributeNode, 0, 2, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, attr, DOM\\Attr, 0)
	ZEND_ARG_TYPE_INFO(0, isId, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_Element_remove arginfo_class_DOMChildNode_remove

#define arginfo_class_DOM_Element_before arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_Element_after arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_Element_replaceWith arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_Element_append arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_Element_prepend arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_Element_replaceChildren arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_Attr_isId arginfo_class_DOM_Node_hasChildNodes

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_CharacterData_substringData, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_CharacterData_appendData, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_CharacterData_insertData, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_CharacterData_deleteData, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_CharacterData_replaceData, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_CharacterData_remove arginfo_class_DOMChildNode_remove

#define arginfo_class_DOM_CharacterData_before arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_CharacterData_after arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_CharacterData_replaceWith arginfo_class_DOM_ParentNode_append

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Text_splitText, 0, 1, DOM\\Text, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_DocumentType_remove arginfo_class_DOMChildNode_remove

#define arginfo_class_DOM_DocumentType_before arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_DocumentType_after arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_DocumentType_replaceWith arginfo_class_DOM_ParentNode_append

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_DocumentFragment_appendXML, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_DocumentFragment_append arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_DocumentFragment_prepend arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_DocumentFragment_replaceChildren arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_Document_getElementsByTagName arginfo_class_DOM_Element_getElementsByTagName

#define arginfo_class_DOM_Document_getElementsByTagNameNS arginfo_class_DOM_Element_getElementsByTagNameNS

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Document_createElement, 0, 1, DOM\\Element, 0)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Document_createElementNS, 0, 2, DOM\\Element, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Document_createDocumentFragment, 0, 0, DOM\\DocumentFragment, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Document_createTextNode, 0, 1, DOM\\Text, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Document_createCDATASection, 0, 1, DOM\\CDATASection, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Document_createComment, 0, 1, DOM\\Comment, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Document_createProcessingInstruction, 0, 2, DOM\\ProcessingInstruction, 0)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Document_importNode, 0, 1, DOM\\\116ode, 0)
	ZEND_ARG_OBJ_INFO(0, node, DOM\\\116ode, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, deep, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_Document_adoptNode arginfo_class_DOM_Node_appendChild

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Document_createAttribute, 0, 1, DOM\\Attr, 0)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Document_createAttributeNS, 0, 2, DOM\\Attr, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_Document_getElementById, 0, 1, DOM\\Element, 1)
	ZEND_ARG_TYPE_INFO(0, elementId, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Document_registerNodeClass, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, baseClass, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, extendedClass, IS_STRING, 1)
ZEND_END_ARG_INFO()

#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Document_schemaValidate, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Document_schemaValidateSource, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Document_relaxNGValidate, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_Document_relaxNGValidateSource, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_class_DOM_Document_append arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_Document_prepend arginfo_class_DOM_ParentNode_append

#define arginfo_class_DOM_Document_replaceChildren arginfo_class_DOM_ParentNode_append

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_HTMLDocument_createEmpty, 0, 0, DOM\\HTMLDocument, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 0, "\"UTF-8\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_HTMLDocument_createFromFile, 0, 1, DOM\\HTMLDocument, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, overrideEncoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_HTMLDocument_createFromString, 0, 1, DOM\\HTMLDocument, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, overrideEncoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_DOM_HTMLDocument_saveXML, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, node, DOM\\\116ode, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_DOM_HTMLDocument_saveXMLFile, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_DOM_HTMLDocument_saveHTML, 0, 0, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, node, DOM\\\116ode, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_DOM_HTMLDocument_saveHTMLFile, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_XMLDocument_createEmpty, 0, 0, DOM\\XMLDocument, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, version, IS_STRING, 0, "\"1.0\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 0, "\"UTF-8\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_XMLDocument_createFromFile, 0, 1, DOM\\XMLDocument, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, overrideEncoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_XMLDocument_createFromString, 0, 1, DOM\\XMLDocument, 0)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, overrideEncoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOM_XMLDocument_createEntityReference, 0, 1, DOM\\EntityReference, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_XMLDocument_validate arginfo_class_DOM_Node_hasChildNodes

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_DOM_XMLDocument_xinclude, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_DOM_XMLDocument_saveXML arginfo_class_DOM_HTMLDocument_saveXML

#define arginfo_class_DOM_XMLDocument_saveXMLFile arginfo_class_DOM_HTMLDocument_saveXMLFile

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOM_XPath___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, document, DOM\\Document, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, registerNodeNS, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_XPath_evaluate, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, expression, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, contextNode, DOM\\DOM\\\116ode, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, registerNodeNS, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
#define arginfo_class_DOM_XPath_query arginfo_class_DOM_XPath_evaluate
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_XPath_registerNamespace, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOM_XPath_registerPhpFunctions, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_MASK(0, restrict, MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_NULL, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
#define arginfo_class_DOM_XPath_registerPhpFunctionNS arginfo_class_DOMXPath_registerPhpFunctionNS
#endif

ZEND_FUNCTION(dom_import_simplexml);
ZEND_FUNCTION(DOM_import_simplexml);
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
ZEND_METHOD(DOMNode, __sleep);
ZEND_METHOD(DOMNode, __wakeup);
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
#if defined(LIBXML_XPATH_ENABLED)
ZEND_METHOD(DOMXPath, registerPhpFunctionNS);
#endif
#if defined(LIBXML_XPATH_ENABLED)
ZEND_METHOD(DOMXPath, quote);
#endif
ZEND_METHOD(DOM_Node, __construct);
ZEND_METHOD(DOM_Node, isEqualNode);
ZEND_METHOD(DOM_Node, isSameNode);
ZEND_METHOD(DOM_Node, compareDocumentPosition);
ZEND_METHOD(DOM_Node, contains);
ZEND_METHOD(DOM_Node, lookupPrefix);
ZEND_METHOD(DOM_Node, isDefaultNamespace);
ZEND_METHOD(DOM_Node, insertBefore);
ZEND_METHOD(DOM_Node, appendChild);
ZEND_METHOD(DOM_Node, replaceChild);
ZEND_METHOD(DOM_Node, removeChild);
ZEND_METHOD(DOM_Node, getNodePath);
ZEND_METHOD(DOM_Element, removeAttribute);
ZEND_METHOD(DOM_Element, setAttributeNodeNS);
ZEND_METHOD(DOM_Element, removeAttributeNode);
ZEND_METHOD(DOM_Element, getElementsByTagName);
ZEND_METHOD(DOM_Element, getElementsByTagNameNS);
ZEND_METHOD(DOM_Element, insertAdjacentElement);
ZEND_METHOD(DOM_Element, setIdAttributeNode);
ZEND_METHOD(DOM_CharacterData, appendData);
ZEND_METHOD(DOM_CharacterData, insertData);
ZEND_METHOD(DOM_CharacterData, deleteData);
ZEND_METHOD(DOM_CharacterData, replaceData);
ZEND_METHOD(DOM_Document, createElement);
ZEND_METHOD(DOM_Document, createElementNS);
ZEND_METHOD(DOM_Document, createProcessingInstruction);
ZEND_METHOD(DOM_Document, importNode);
ZEND_METHOD(DOM_Document, adoptNode);
ZEND_METHOD(DOM_Document, registerNodeClass);
ZEND_METHOD(DOM_HTMLDocument, createEmpty);
ZEND_METHOD(DOM_HTMLDocument, createFromFile);
ZEND_METHOD(DOM_HTMLDocument, createFromString);
ZEND_METHOD(DOM_XMLDocument, saveXML);
ZEND_METHOD(DOM_HTMLDocument, saveHTML);
ZEND_METHOD(DOM_HTMLDocument, saveHTMLFile);
ZEND_METHOD(DOM_XMLDocument, createEmpty);
ZEND_METHOD(DOM_XMLDocument, createFromFile);
ZEND_METHOD(DOM_XMLDocument, createFromString);
#if defined(LIBXML_XPATH_ENABLED)
ZEND_METHOD(DOM_XPath, __construct);
#endif
#if defined(LIBXML_XPATH_ENABLED)
ZEND_METHOD(DOM_XPath, evaluate);
#endif
#if defined(LIBXML_XPATH_ENABLED)
ZEND_METHOD(DOM_XPath, query);
#endif

static const zend_function_entry ext_functions[] = {
	ZEND_FE(dom_import_simplexml, arginfo_dom_import_simplexml)
	ZEND_RAW_FENTRY(ZEND_NS_NAME("DOM", "import_simplexml"), zif_DOM_import_simplexml, arginfo_DOM_import_simplexml, 0, NULL, NULL)
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
	ZEND_RAW_FENTRY("append", NULL, arginfo_class_DOMParentNode_append, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("prepend", NULL, arginfo_class_DOMParentNode_prepend, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("replaceChildren", NULL, arginfo_class_DOMParentNode_replaceChildren, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DOMChildNode_methods[] = {
	ZEND_RAW_FENTRY("remove", NULL, arginfo_class_DOMChildNode_remove, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("before", NULL, arginfo_class_DOMChildNode_before, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("after", NULL, arginfo_class_DOMChildNode_after, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("replaceWith", NULL, arginfo_class_DOMChildNode_replaceWith, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
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
	ZEND_ME(DOMNode, __sleep, arginfo_class_DOMNode___sleep, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, __wakeup, arginfo_class_DOMNode___wakeup, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_DOMNameSpaceNode_methods[] = {
	ZEND_RAW_FENTRY("__sleep", zim_DOMNode___sleep, arginfo_class_DOMNameSpaceNode___sleep, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("__wakeup", zim_DOMNode___wakeup, arginfo_class_DOMNameSpaceNode___wakeup, ZEND_ACC_PUBLIC, NULL, NULL)
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
	ZEND_RAW_FENTRY("append", zim_DOMElement_append, arginfo_class_DOMDocumentFragment_append, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("prepend", zim_DOMElement_prepend, arginfo_class_DOMDocumentFragment_prepend, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("replaceChildren", zim_DOMDocument_replaceChildren, arginfo_class_DOMDocumentFragment_replaceChildren, ZEND_ACC_PUBLIC, NULL, NULL)
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
	ZEND_RAW_FENTRY("replaceWith", zim_DOMElement_replaceWith, arginfo_class_DOMCharacterData_replaceWith, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("remove", zim_DOMElement_remove, arginfo_class_DOMCharacterData_remove, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("before", zim_DOMElement_before, arginfo_class_DOMCharacterData_before, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("after", zim_DOMElement_after, arginfo_class_DOMCharacterData_after, ZEND_ACC_PUBLIC, NULL, NULL)
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
	ZEND_RAW_FENTRY("getElementsByTagName", zim_DOMElement_getElementsByTagName, arginfo_class_DOMDocument_getElementsByTagName, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getElementsByTagNameNS", zim_DOMElement_getElementsByTagNameNS, arginfo_class_DOMDocument_getElementsByTagNameNS, ZEND_ACC_PUBLIC, NULL, NULL)
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
	ZEND_RAW_FENTRY("append", zim_DOMElement_append, arginfo_class_DOMDocument_append, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("prepend", zim_DOMElement_prepend, arginfo_class_DOMDocument_prepend, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(DOMDocument, replaceChildren, arginfo_class_DOMDocument_replaceChildren, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_DOMException_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_DOMText_methods[] = {
	ZEND_ME(DOMText, __construct, arginfo_class_DOMText___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMText, isWhitespaceInElementContent, arginfo_class_DOMText_isWhitespaceInElementContent, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("isElementContentWhitespace", zim_DOMText_isWhitespaceInElementContent, arginfo_class_DOMText_isElementContentWhitespace, ZEND_ACC_PUBLIC, NULL, NULL)
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
	ZEND_ME(DOMXPath, registerPhpFunctionNS, arginfo_class_DOMXPath_registerPhpFunctionNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMXPath, quote, arginfo_class_DOMXPath_quote, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};
#endif

static const zend_function_entry class_DOM_ParentNode_methods[] = {
	ZEND_RAW_FENTRY("append", NULL, arginfo_class_DOM_ParentNode_append, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("prepend", NULL, arginfo_class_DOM_ParentNode_prepend, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("replaceChildren", NULL, arginfo_class_DOM_ParentNode_replaceChildren, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DOM_ChildNode_methods[] = {
	ZEND_RAW_FENTRY("remove", NULL, arginfo_class_DOM_ChildNode_remove, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("before", NULL, arginfo_class_DOM_ChildNode_before, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("after", NULL, arginfo_class_DOM_ChildNode_after, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_RAW_FENTRY("replaceWith", NULL, arginfo_class_DOM_ChildNode_replaceWith, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DOM_Node_methods[] = {
	ZEND_ME(DOM_Node, __construct, arginfo_class_DOM_Node___construct, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_RAW_FENTRY("getRootNode", zim_DOMNode_getRootNode, arginfo_class_DOM_Node_getRootNode, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("hasChildNodes", zim_DOMNode_hasChildNodes, arginfo_class_DOM_Node_hasChildNodes, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("normalize", zim_DOMNode_normalize, arginfo_class_DOM_Node_normalize, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("cloneNode", zim_DOMNode_cloneNode, arginfo_class_DOM_Node_cloneNode, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(DOM_Node, isEqualNode, arginfo_class_DOM_Node_isEqualNode, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Node, isSameNode, arginfo_class_DOM_Node_isSameNode, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Node, compareDocumentPosition, arginfo_class_DOM_Node_compareDocumentPosition, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Node, contains, arginfo_class_DOM_Node_contains, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Node, lookupPrefix, arginfo_class_DOM_Node_lookupPrefix, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("lookupNamespaceURI", zim_DOMNode_lookupNamespaceURI, arginfo_class_DOM_Node_lookupNamespaceURI, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(DOM_Node, isDefaultNamespace, arginfo_class_DOM_Node_isDefaultNamespace, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Node, insertBefore, arginfo_class_DOM_Node_insertBefore, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Node, appendChild, arginfo_class_DOM_Node_appendChild, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Node, replaceChild, arginfo_class_DOM_Node_replaceChild, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Node, removeChild, arginfo_class_DOM_Node_removeChild, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("getLineNo", zim_DOMNode_getLineNo, arginfo_class_DOM_Node_getLineNo, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(DOM_Node, getNodePath, arginfo_class_DOM_Node_getNodePath, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("C14N", zim_DOMNode_C14N, arginfo_class_DOM_Node_C14N, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("C14NFile", zim_DOMNode_C14NFile, arginfo_class_DOM_Node_C14NFile, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("__sleep", zim_DOMNode___sleep, arginfo_class_DOM_Node___sleep, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("__wakeup", zim_DOMNode___wakeup, arginfo_class_DOM_Node___wakeup, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DOM_NodeList_methods[] = {
	ZEND_RAW_FENTRY("count", zim_DOMNodeList_count, arginfo_class_DOM_NodeList_count, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getIterator", zim_DOMNodeList_getIterator, arginfo_class_DOM_NodeList_getIterator, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("item", zim_DOMNodeList_item, arginfo_class_DOM_NodeList_item, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DOM_NamedNodeMap_methods[] = {
	ZEND_RAW_FENTRY("item", zim_DOMNamedNodeMap_item, arginfo_class_DOM_NamedNodeMap_item, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getNamedItem", zim_DOMNamedNodeMap_getNamedItem, arginfo_class_DOM_NamedNodeMap_getNamedItem, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getNamedItemNS", zim_DOMNamedNodeMap_getNamedItemNS, arginfo_class_DOM_NamedNodeMap_getNamedItemNS, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("count", zim_DOMNamedNodeMap_count, arginfo_class_DOM_NamedNodeMap_count, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getIterator", zim_DOMNamedNodeMap_getIterator, arginfo_class_DOM_NamedNodeMap_getIterator, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DOM_HTMLCollection_methods[] = {
	ZEND_RAW_FENTRY("item", zim_DOMNodeList_item, arginfo_class_DOM_HTMLCollection_item, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("count", zim_DOMNodeList_count, arginfo_class_DOM_HTMLCollection_count, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getIterator", zim_DOMNodeList_getIterator, arginfo_class_DOM_HTMLCollection_getIterator, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DOM_Element_methods[] = {
	ZEND_RAW_FENTRY("hasAttributes", zim_DOMNode_hasAttributes, arginfo_class_DOM_Element_hasAttributes, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getAttributeNames", zim_DOMElement_getAttributeNames, arginfo_class_DOM_Element_getAttributeNames, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getAttribute", zim_DOMElement_getAttribute, arginfo_class_DOM_Element_getAttribute, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getAttributeNS", zim_DOMElement_getAttributeNS, arginfo_class_DOM_Element_getAttributeNS, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("setAttribute", zim_DOMElement_setAttribute, arginfo_class_DOM_Element_setAttribute, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("setAttributeNS", zim_DOMElement_setAttributeNS, arginfo_class_DOM_Element_setAttributeNS, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(DOM_Element, removeAttribute, arginfo_class_DOM_Element_removeAttribute, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("removeAttributeNS", zim_DOMElement_removeAttributeNS, arginfo_class_DOM_Element_removeAttributeNS, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("toggleAttribute", zim_DOMElement_toggleAttribute, arginfo_class_DOM_Element_toggleAttribute, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("hasAttribute", zim_DOMElement_hasAttribute, arginfo_class_DOM_Element_hasAttribute, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("hasAttributeNS", zim_DOMElement_hasAttributeNS, arginfo_class_DOM_Element_hasAttributeNS, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getAttributeNode", zim_DOMElement_getAttributeNode, arginfo_class_DOM_Element_getAttributeNode, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getAttributeNodeNS", zim_DOMElement_getAttributeNodeNS, arginfo_class_DOM_Element_getAttributeNodeNS, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("setAttributeNode", zim_DOM_Element_setAttributeNodeNS, arginfo_class_DOM_Element_setAttributeNode, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(DOM_Element, setAttributeNodeNS, arginfo_class_DOM_Element_setAttributeNodeNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Element, removeAttributeNode, arginfo_class_DOM_Element_removeAttributeNode, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Element, getElementsByTagName, arginfo_class_DOM_Element_getElementsByTagName, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Element, getElementsByTagNameNS, arginfo_class_DOM_Element_getElementsByTagNameNS, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Element, insertAdjacentElement, arginfo_class_DOM_Element_insertAdjacentElement, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("insertAdjacentText", zim_DOMElement_insertAdjacentText, arginfo_class_DOM_Element_insertAdjacentText, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("setIdAttribute", zim_DOMElement_setIdAttribute, arginfo_class_DOM_Element_setIdAttribute, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("setIdAttributeNS", zim_DOMElement_setIdAttributeNS, arginfo_class_DOM_Element_setIdAttributeNS, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(DOM_Element, setIdAttributeNode, arginfo_class_DOM_Element_setIdAttributeNode, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("remove", zim_DOMElement_remove, arginfo_class_DOM_Element_remove, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("before", zim_DOMElement_before, arginfo_class_DOM_Element_before, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("after", zim_DOMElement_after, arginfo_class_DOM_Element_after, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("replaceWith", zim_DOMElement_replaceWith, arginfo_class_DOM_Element_replaceWith, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("append", zim_DOMElement_append, arginfo_class_DOM_Element_append, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("prepend", zim_DOMElement_prepend, arginfo_class_DOM_Element_prepend, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("replaceChildren", zim_DOMElement_replaceChildren, arginfo_class_DOM_Element_replaceChildren, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DOM_Attr_methods[] = {
	ZEND_RAW_FENTRY("isId", zim_DOMAttr_isId, arginfo_class_DOM_Attr_isId, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DOM_CharacterData_methods[] = {
	ZEND_RAW_FENTRY("substringData", zim_DOMCharacterData_substringData, arginfo_class_DOM_CharacterData_substringData, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(DOM_CharacterData, appendData, arginfo_class_DOM_CharacterData_appendData, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_CharacterData, insertData, arginfo_class_DOM_CharacterData_insertData, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_CharacterData, deleteData, arginfo_class_DOM_CharacterData_deleteData, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_CharacterData, replaceData, arginfo_class_DOM_CharacterData_replaceData, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("remove", zim_DOMElement_remove, arginfo_class_DOM_CharacterData_remove, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("before", zim_DOMElement_before, arginfo_class_DOM_CharacterData_before, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("after", zim_DOMElement_after, arginfo_class_DOM_CharacterData_after, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("replaceWith", zim_DOMElement_replaceWith, arginfo_class_DOM_CharacterData_replaceWith, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DOM_Text_methods[] = {
	ZEND_RAW_FENTRY("splitText", zim_DOMText_splitText, arginfo_class_DOM_Text_splitText, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DOM_CDATASection_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_DOM_ProcessingInstruction_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_DOM_Comment_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_DOM_DocumentType_methods[] = {
	ZEND_RAW_FENTRY("remove", zim_DOMElement_remove, arginfo_class_DOM_DocumentType_remove, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("before", zim_DOMElement_before, arginfo_class_DOM_DocumentType_before, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("after", zim_DOMElement_after, arginfo_class_DOM_DocumentType_after, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("replaceWith", zim_DOMElement_replaceWith, arginfo_class_DOM_DocumentType_replaceWith, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DOM_DocumentFragment_methods[] = {
	ZEND_RAW_FENTRY("appendXML", zim_DOMDocumentFragment_appendXML, arginfo_class_DOM_DocumentFragment_appendXML, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("append", zim_DOMElement_append, arginfo_class_DOM_DocumentFragment_append, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("prepend", zim_DOMElement_prepend, arginfo_class_DOM_DocumentFragment_prepend, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("replaceChildren", zim_DOMElement_replaceChildren, arginfo_class_DOM_DocumentFragment_replaceChildren, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DOM_Entity_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_DOM_EntityReference_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_DOM_Notation_methods[] = {
	ZEND_FE_END
};

static const zend_function_entry class_DOM_Document_methods[] = {
	ZEND_RAW_FENTRY("getElementsByTagName", zim_DOM_Element_getElementsByTagName, arginfo_class_DOM_Document_getElementsByTagName, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getElementsByTagNameNS", zim_DOM_Element_getElementsByTagNameNS, arginfo_class_DOM_Document_getElementsByTagNameNS, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(DOM_Document, createElement, arginfo_class_DOM_Document_createElement, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Document, createElementNS, arginfo_class_DOM_Document_createElementNS, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("createDocumentFragment", zim_DOMDocument_createDocumentFragment, arginfo_class_DOM_Document_createDocumentFragment, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("createTextNode", zim_DOMDocument_createTextNode, arginfo_class_DOM_Document_createTextNode, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("createCDATASection", zim_DOMDocument_createCDATASection, arginfo_class_DOM_Document_createCDATASection, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("createComment", zim_DOMDocument_createComment, arginfo_class_DOM_Document_createComment, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(DOM_Document, createProcessingInstruction, arginfo_class_DOM_Document_createProcessingInstruction, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Document, importNode, arginfo_class_DOM_Document_importNode, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_Document, adoptNode, arginfo_class_DOM_Document_adoptNode, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("createAttribute", zim_DOMDocument_createAttribute, arginfo_class_DOM_Document_createAttribute, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("createAttributeNS", zim_DOMDocument_createAttributeNS, arginfo_class_DOM_Document_createAttributeNS, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("getElementById", zim_DOMDocument_getElementById, arginfo_class_DOM_Document_getElementById, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(DOM_Document, registerNodeClass, arginfo_class_DOM_Document_registerNodeClass, ZEND_ACC_PUBLIC)
#if defined(LIBXML_SCHEMAS_ENABLED)
	ZEND_RAW_FENTRY("schemaValidate", zim_DOMDocument_schemaValidate, arginfo_class_DOM_Document_schemaValidate, ZEND_ACC_PUBLIC, NULL, NULL)
#endif
#if defined(LIBXML_SCHEMAS_ENABLED)
	ZEND_RAW_FENTRY("schemaValidateSource", zim_DOMDocument_schemaValidateSource, arginfo_class_DOM_Document_schemaValidateSource, ZEND_ACC_PUBLIC, NULL, NULL)
#endif
#if defined(LIBXML_SCHEMAS_ENABLED)
	ZEND_RAW_FENTRY("relaxNGValidate", zim_DOMDocument_relaxNGValidate, arginfo_class_DOM_Document_relaxNGValidate, ZEND_ACC_PUBLIC, NULL, NULL)
#endif
#if defined(LIBXML_SCHEMAS_ENABLED)
	ZEND_RAW_FENTRY("relaxNGValidateSource", zim_DOMDocument_relaxNGValidateSource, arginfo_class_DOM_Document_relaxNGValidateSource, ZEND_ACC_PUBLIC, NULL, NULL)
#endif
	ZEND_RAW_FENTRY("append", zim_DOMElement_append, arginfo_class_DOM_Document_append, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("prepend", zim_DOMElement_prepend, arginfo_class_DOM_Document_prepend, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("replaceChildren", zim_DOMDocument_replaceChildren, arginfo_class_DOM_Document_replaceChildren, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

static const zend_function_entry class_DOM_HTMLDocument_methods[] = {
	ZEND_ME(DOM_HTMLDocument, createEmpty, arginfo_class_DOM_HTMLDocument_createEmpty, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(DOM_HTMLDocument, createFromFile, arginfo_class_DOM_HTMLDocument_createFromFile, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(DOM_HTMLDocument, createFromString, arginfo_class_DOM_HTMLDocument_createFromString, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_RAW_FENTRY("saveXML", zim_DOM_XMLDocument_saveXML, arginfo_class_DOM_HTMLDocument_saveXML, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("saveXMLFile", zim_DOMDocument_save, arginfo_class_DOM_HTMLDocument_saveXMLFile, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(DOM_HTMLDocument, saveHTML, arginfo_class_DOM_HTMLDocument_saveHTML, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_HTMLDocument, saveHTMLFile, arginfo_class_DOM_HTMLDocument_saveHTMLFile, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_DOM_XMLDocument_methods[] = {
	ZEND_ME(DOM_XMLDocument, createEmpty, arginfo_class_DOM_XMLDocument_createEmpty, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(DOM_XMLDocument, createFromFile, arginfo_class_DOM_XMLDocument_createFromFile, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(DOM_XMLDocument, createFromString, arginfo_class_DOM_XMLDocument_createFromString, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_RAW_FENTRY("createEntityReference", zim_DOMDocument_createEntityReference, arginfo_class_DOM_XMLDocument_createEntityReference, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("validate", zim_DOMDocument_validate, arginfo_class_DOM_XMLDocument_validate, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("xinclude", zim_DOMDocument_xinclude, arginfo_class_DOM_XMLDocument_xinclude, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_ME(DOM_XMLDocument, saveXML, arginfo_class_DOM_XMLDocument_saveXML, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("saveXMLFile", zim_DOMDocument_save, arginfo_class_DOM_XMLDocument_saveXMLFile, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_FE_END
};

#if defined(LIBXML_XPATH_ENABLED)
static const zend_function_entry class_DOM_XPath_methods[] = {
	ZEND_ME(DOM_XPath, __construct, arginfo_class_DOM_XPath___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_XPath, evaluate, arginfo_class_DOM_XPath_evaluate, ZEND_ACC_PUBLIC)
	ZEND_ME(DOM_XPath, query, arginfo_class_DOM_XPath_query, ZEND_ACC_PUBLIC)
	ZEND_RAW_FENTRY("registerNamespace", zim_DOMXPath_registerNamespace, arginfo_class_DOM_XPath_registerNamespace, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("registerPhpFunctions", zim_DOMXPath_registerPhpFunctions, arginfo_class_DOM_XPath_registerPhpFunctions, ZEND_ACC_PUBLIC, NULL, NULL)
	ZEND_RAW_FENTRY("registerPhpFunctionNS", zim_DOMXPath_registerPhpFunctionNS, arginfo_class_DOM_XPath_registerPhpFunctionNS, ZEND_ACC_PUBLIC, NULL, NULL)
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
	REGISTER_LONG_CONSTANT("DOM\\INDEX_SIZE_ERR", INDEX_SIZE_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\STRING_SIZE_ERR", DOMSTRING_SIZE_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\HIERARCHY_REQUEST_ERR", HIERARCHY_REQUEST_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\WRONG_DOCUMENT_ERR", WRONG_DOCUMENT_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\INVALID_CHARACTER_ERR", INVALID_CHARACTER_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\NO_DATA_ALLOWED_ERR", NO_DATA_ALLOWED_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\NO_MODIFICATION_ALLOWED_ERR", NO_MODIFICATION_ALLOWED_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\NOT_FOUND_ERR", NOT_FOUND_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\NOT_SUPPORTED_ERR", NOT_SUPPORTED_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\INUSE_ATTRIBUTE_ERR", INUSE_ATTRIBUTE_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\INVALID_STATE_ERR", INVALID_STATE_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\SYNTAX_ERR", SYNTAX_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\INVALID_MODIFICATION_ERR", INVALID_MODIFICATION_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\NAMESPACE_ERR", NAMESPACE_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\INVALID_ACCESS_ERR", INVALID_ACCESS_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\VALIDATION_ERR", VALIDATION_ERR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DOM\\HTML_NO_DEFAULT_NS", DOM_HTML_NO_DEFAULT_NS, CONST_PERSISTENT);
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
	zend_register_class_alias("DOM\\DOMException", class_entry);

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

static zend_class_entry *register_class_DOM_ParentNode(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "ParentNode", class_DOM_ParentNode_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_DOM_ChildNode(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "ChildNode", class_DOM_ChildNode_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}

static zend_class_entry *register_class_DOM_Node(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "Node", class_DOM_Node_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

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

	zval property_nodeType_default_value;
	ZVAL_UNDEF(&property_nodeType_default_value);
	zend_string *property_nodeType_name = zend_string_init("nodeType", sizeof("nodeType") - 1, 1);
	zend_declare_typed_property(class_entry, property_nodeType_name, &property_nodeType_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_nodeType_name);

	zval property_nodeName_default_value;
	ZVAL_UNDEF(&property_nodeName_default_value);
	zend_string *property_nodeName_name = zend_string_init("nodeName", sizeof("nodeName") - 1, 1);
	zend_declare_typed_property(class_entry, property_nodeName_name, &property_nodeName_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_nodeName_name);

	zval property_baseURI_default_value;
	ZVAL_UNDEF(&property_baseURI_default_value);
	zend_string *property_baseURI_name = zend_string_init("baseURI", sizeof("baseURI") - 1, 1);
	zend_declare_typed_property(class_entry, property_baseURI_name, &property_baseURI_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_baseURI_name);

	zval property_isConnected_default_value;
	ZVAL_UNDEF(&property_isConnected_default_value);
	zend_string *property_isConnected_name = zend_string_init("isConnected", sizeof("isConnected") - 1, 1);
	zend_declare_typed_property(class_entry, property_isConnected_name, &property_isConnected_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_isConnected_name);

	zval property_ownerDocument_default_value;
	ZVAL_UNDEF(&property_ownerDocument_default_value);
	zend_string *property_ownerDocument_name = zend_string_init("ownerDocument", sizeof("ownerDocument") - 1, 1);
	zend_string *property_ownerDocument_class_DOM_Document = zend_string_init("DOM\\Document", sizeof("DOM\\Document")-1, 1);
	zend_declare_typed_property(class_entry, property_ownerDocument_name, &property_ownerDocument_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_ownerDocument_class_DOM_Document, 0, MAY_BE_NULL));
	zend_string_release(property_ownerDocument_name);

	zval property_parentNode_default_value;
	ZVAL_UNDEF(&property_parentNode_default_value);
	zend_string *property_parentNode_name = zend_string_init("parentNode", sizeof("parentNode") - 1, 1);
	zend_string *property_parentNode_class_DOM_Node = zend_string_init("DOM\\\116ode", sizeof("DOM\\\116ode")-1, 1);
	zend_declare_typed_property(class_entry, property_parentNode_name, &property_parentNode_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_parentNode_class_DOM_Node, 0, MAY_BE_NULL));
	zend_string_release(property_parentNode_name);

	zval property_parentElement_default_value;
	ZVAL_UNDEF(&property_parentElement_default_value);
	zend_string *property_parentElement_name = zend_string_init("parentElement", sizeof("parentElement") - 1, 1);
	zend_string *property_parentElement_class_DOM_Element = zend_string_init("DOM\\Element", sizeof("DOM\\Element")-1, 1);
	zend_declare_typed_property(class_entry, property_parentElement_name, &property_parentElement_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_parentElement_class_DOM_Element, 0, MAY_BE_NULL));
	zend_string_release(property_parentElement_name);

	zval property_childNodes_default_value;
	ZVAL_UNDEF(&property_childNodes_default_value);
	zend_string *property_childNodes_name = zend_string_init("childNodes", sizeof("childNodes") - 1, 1);
	zend_string *property_childNodes_class_DOM_NodeList = zend_string_init("DOM\\\116odeList", sizeof("DOM\\\116odeList")-1, 1);
	zend_declare_typed_property(class_entry, property_childNodes_name, &property_childNodes_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_childNodes_class_DOM_NodeList, 0, 0));
	zend_string_release(property_childNodes_name);

	zval property_firstChild_default_value;
	ZVAL_UNDEF(&property_firstChild_default_value);
	zend_string *property_firstChild_name = zend_string_init("firstChild", sizeof("firstChild") - 1, 1);
	zend_string *property_firstChild_class_DOM_Node = zend_string_init("DOM\\\116ode", sizeof("DOM\\\116ode")-1, 1);
	zend_declare_typed_property(class_entry, property_firstChild_name, &property_firstChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_firstChild_class_DOM_Node, 0, MAY_BE_NULL));
	zend_string_release(property_firstChild_name);

	zval property_lastChild_default_value;
	ZVAL_UNDEF(&property_lastChild_default_value);
	zend_string *property_lastChild_name = zend_string_init("lastChild", sizeof("lastChild") - 1, 1);
	zend_string *property_lastChild_class_DOM_Node = zend_string_init("DOM\\\116ode", sizeof("DOM\\\116ode")-1, 1);
	zend_declare_typed_property(class_entry, property_lastChild_name, &property_lastChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_lastChild_class_DOM_Node, 0, MAY_BE_NULL));
	zend_string_release(property_lastChild_name);

	zval property_previousSibling_default_value;
	ZVAL_UNDEF(&property_previousSibling_default_value);
	zend_string *property_previousSibling_name = zend_string_init("previousSibling", sizeof("previousSibling") - 1, 1);
	zend_string *property_previousSibling_class_DOM_Node = zend_string_init("DOM\\\116ode", sizeof("DOM\\\116ode")-1, 1);
	zend_declare_typed_property(class_entry, property_previousSibling_name, &property_previousSibling_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_previousSibling_class_DOM_Node, 0, MAY_BE_NULL));
	zend_string_release(property_previousSibling_name);

	zval property_nextSibling_default_value;
	ZVAL_UNDEF(&property_nextSibling_default_value);
	zend_string *property_nextSibling_name = zend_string_init("nextSibling", sizeof("nextSibling") - 1, 1);
	zend_string *property_nextSibling_class_DOM_Node = zend_string_init("DOM\\\116ode", sizeof("DOM\\\116ode")-1, 1);
	zend_declare_typed_property(class_entry, property_nextSibling_name, &property_nextSibling_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_nextSibling_class_DOM_Node, 0, MAY_BE_NULL));
	zend_string_release(property_nextSibling_name);

	zval property_nodeValue_default_value;
	ZVAL_UNDEF(&property_nodeValue_default_value);
	zend_string *property_nodeValue_name = zend_string_init("nodeValue", sizeof("nodeValue") - 1, 1);
	zend_declare_typed_property(class_entry, property_nodeValue_name, &property_nodeValue_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_nodeValue_name);

	zval property_textContent_default_value;
	ZVAL_UNDEF(&property_textContent_default_value);
	zend_string *property_textContent_name = zend_string_init("textContent", sizeof("textContent") - 1, 1);
	zend_declare_typed_property(class_entry, property_textContent_name, &property_textContent_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_textContent_name);

	return class_entry;
}

static zend_class_entry *register_class_DOM_NodeList(zend_class_entry *class_entry_DOM_IteratorAggregate, zend_class_entry *class_entry_DOM_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "NodeList", class_DOM_NodeList_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 2, class_entry_DOM_IteratorAggregate, class_entry_DOM_Countable);

	zval property_length_default_value;
	ZVAL_UNDEF(&property_length_default_value);
	zend_string *property_length_name = zend_string_init("length", sizeof("length") - 1, 1);
	zend_declare_typed_property(class_entry, property_length_name, &property_length_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_length_name);

	return class_entry;
}

static zend_class_entry *register_class_DOM_NamedNodeMap(zend_class_entry *class_entry_DOM_IteratorAggregate, zend_class_entry *class_entry_DOM_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "NamedNodeMap", class_DOM_NamedNodeMap_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 2, class_entry_DOM_IteratorAggregate, class_entry_DOM_Countable);

	zval property_length_default_value;
	ZVAL_UNDEF(&property_length_default_value);
	zend_string *property_length_name = zend_string_init("length", sizeof("length") - 1, 1);
	zend_declare_typed_property(class_entry, property_length_name, &property_length_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_length_name);

	return class_entry;
}

static zend_class_entry *register_class_DOM_HTMLCollection(zend_class_entry *class_entry_DOM_IteratorAggregate, zend_class_entry *class_entry_DOM_Countable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "HTMLCollection", class_DOM_HTMLCollection_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 2, class_entry_DOM_IteratorAggregate, class_entry_DOM_Countable);

	zval property_length_default_value;
	ZVAL_UNDEF(&property_length_default_value);
	zend_string *property_length_name = zend_string_init("length", sizeof("length") - 1, 1);
	zend_declare_typed_property(class_entry, property_length_name, &property_length_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_length_name);

	return class_entry;
}

static zend_class_entry *register_class_DOM_Element(zend_class_entry *class_entry_DOM_Node, zend_class_entry *class_entry_DOM_ParentNode, zend_class_entry *class_entry_DOM_ChildNode)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "Element", class_DOM_Element_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_Node);
	zend_class_implements(class_entry, 2, class_entry_DOM_ParentNode, class_entry_DOM_ChildNode);

	zval property_namespaceURI_default_value;
	ZVAL_UNDEF(&property_namespaceURI_default_value);
	zend_string *property_namespaceURI_name = zend_string_init("namespaceURI", sizeof("namespaceURI") - 1, 1);
	zend_declare_typed_property(class_entry, property_namespaceURI_name, &property_namespaceURI_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_namespaceURI_name);

	zval property_prefix_default_value;
	ZVAL_UNDEF(&property_prefix_default_value);
	zend_string *property_prefix_name = zend_string_init("prefix", sizeof("prefix") - 1, 1);
	zend_declare_typed_property(class_entry, property_prefix_name, &property_prefix_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_prefix_name);

	zval property_localName_default_value;
	ZVAL_UNDEF(&property_localName_default_value);
	zend_string *property_localName_name = zend_string_init("localName", sizeof("localName") - 1, 1);
	zend_declare_typed_property(class_entry, property_localName_name, &property_localName_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_localName_name);

	zval property_tagName_default_value;
	ZVAL_UNDEF(&property_tagName_default_value);
	zend_string *property_tagName_name = zend_string_init("tagName", sizeof("tagName") - 1, 1);
	zend_declare_typed_property(class_entry, property_tagName_name, &property_tagName_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_tagName_name);

	zval property_id_default_value;
	ZVAL_UNDEF(&property_id_default_value);
	zend_string *property_id_name = zend_string_init("id", sizeof("id") - 1, 1);
	zend_declare_typed_property(class_entry, property_id_name, &property_id_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_id_name);

	zval property_className_default_value;
	ZVAL_UNDEF(&property_className_default_value);
	zend_string *property_className_name = zend_string_init("className", sizeof("className") - 1, 1);
	zend_declare_typed_property(class_entry, property_className_name, &property_className_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_className_name);

	zval property_attributes_default_value;
	ZVAL_UNDEF(&property_attributes_default_value);
	zend_string *property_attributes_name = zend_string_init("attributes", sizeof("attributes") - 1, 1);
	zend_string *property_attributes_class_DOM_NamedNodeMap = zend_string_init("DOM\\\116amedNodeMap", sizeof("DOM\\\116amedNodeMap")-1, 1);
	zend_declare_typed_property(class_entry, property_attributes_name, &property_attributes_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_attributes_class_DOM_NamedNodeMap, 0, 0));
	zend_string_release(property_attributes_name);

	zval property_firstElementChild_default_value;
	ZVAL_UNDEF(&property_firstElementChild_default_value);
	zend_string *property_firstElementChild_name = zend_string_init("firstElementChild", sizeof("firstElementChild") - 1, 1);
	zend_string *property_firstElementChild_class_DOM_Element = zend_string_init("DOM\\Element", sizeof("DOM\\Element")-1, 1);
	zend_declare_typed_property(class_entry, property_firstElementChild_name, &property_firstElementChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_firstElementChild_class_DOM_Element, 0, MAY_BE_NULL));
	zend_string_release(property_firstElementChild_name);

	zval property_lastElementChild_default_value;
	ZVAL_UNDEF(&property_lastElementChild_default_value);
	zend_string *property_lastElementChild_name = zend_string_init("lastElementChild", sizeof("lastElementChild") - 1, 1);
	zend_string *property_lastElementChild_class_DOM_Element = zend_string_init("DOM\\Element", sizeof("DOM\\Element")-1, 1);
	zend_declare_typed_property(class_entry, property_lastElementChild_name, &property_lastElementChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_lastElementChild_class_DOM_Element, 0, MAY_BE_NULL));
	zend_string_release(property_lastElementChild_name);

	zval property_childElementCount_default_value;
	ZVAL_UNDEF(&property_childElementCount_default_value);
	zend_string *property_childElementCount_name = zend_string_init("childElementCount", sizeof("childElementCount") - 1, 1);
	zend_declare_typed_property(class_entry, property_childElementCount_name, &property_childElementCount_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_childElementCount_name);

	zval property_previousElementSibling_default_value;
	ZVAL_UNDEF(&property_previousElementSibling_default_value);
	zend_string *property_previousElementSibling_name = zend_string_init("previousElementSibling", sizeof("previousElementSibling") - 1, 1);
	zend_string *property_previousElementSibling_class_DOM_Element = zend_string_init("DOM\\Element", sizeof("DOM\\Element")-1, 1);
	zend_declare_typed_property(class_entry, property_previousElementSibling_name, &property_previousElementSibling_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_previousElementSibling_class_DOM_Element, 0, MAY_BE_NULL));
	zend_string_release(property_previousElementSibling_name);

	zval property_nextElementSibling_default_value;
	ZVAL_UNDEF(&property_nextElementSibling_default_value);
	zend_string *property_nextElementSibling_name = zend_string_init("nextElementSibling", sizeof("nextElementSibling") - 1, 1);
	zend_string *property_nextElementSibling_class_DOM_Element = zend_string_init("DOM\\Element", sizeof("DOM\\Element")-1, 1);
	zend_declare_typed_property(class_entry, property_nextElementSibling_name, &property_nextElementSibling_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_nextElementSibling_class_DOM_Element, 0, MAY_BE_NULL));
	zend_string_release(property_nextElementSibling_name);

	return class_entry;
}

static zend_class_entry *register_class_DOM_Attr(zend_class_entry *class_entry_DOM_Node)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "Attr", class_DOM_Attr_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_Node);

	zval property_namespaceURI_default_value;
	ZVAL_UNDEF(&property_namespaceURI_default_value);
	zend_string *property_namespaceURI_name = zend_string_init("namespaceURI", sizeof("namespaceURI") - 1, 1);
	zend_declare_typed_property(class_entry, property_namespaceURI_name, &property_namespaceURI_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_namespaceURI_name);

	zval property_prefix_default_value;
	ZVAL_UNDEF(&property_prefix_default_value);
	zend_string *property_prefix_name = zend_string_init("prefix", sizeof("prefix") - 1, 1);
	zend_declare_typed_property(class_entry, property_prefix_name, &property_prefix_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_prefix_name);

	zval property_localName_default_value;
	ZVAL_UNDEF(&property_localName_default_value);
	zend_string *property_localName_name = zend_string_init("localName", sizeof("localName") - 1, 1);
	zend_declare_typed_property(class_entry, property_localName_name, &property_localName_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_localName_name);

	zval property_name_default_value;
	ZVAL_UNDEF(&property_name_default_value);
	zend_string *property_name_name = zend_string_init("name", sizeof("name") - 1, 1);
	zend_declare_typed_property(class_entry, property_name_name, &property_name_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_name_name);

	zval property_value_default_value;
	ZVAL_UNDEF(&property_value_default_value);
	zend_string *property_value_name = zend_string_init("value", sizeof("value") - 1, 1);
	zend_declare_typed_property(class_entry, property_value_name, &property_value_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_value_name);

	zval property_ownerElement_default_value;
	ZVAL_UNDEF(&property_ownerElement_default_value);
	zend_string *property_ownerElement_name = zend_string_init("ownerElement", sizeof("ownerElement") - 1, 1);
	zend_string *property_ownerElement_class_DOM_Element = zend_string_init("DOM\\Element", sizeof("DOM\\Element")-1, 1);
	zend_declare_typed_property(class_entry, property_ownerElement_name, &property_ownerElement_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_ownerElement_class_DOM_Element, 0, MAY_BE_NULL));
	zend_string_release(property_ownerElement_name);

	zval property_specified_default_value;
	ZVAL_TRUE(&property_specified_default_value);
	zend_string *property_specified_name = zend_string_init("specified", sizeof("specified") - 1, 1);
	zend_declare_typed_property(class_entry, property_specified_name, &property_specified_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_specified_name);

	return class_entry;
}

static zend_class_entry *register_class_DOM_CharacterData(zend_class_entry *class_entry_DOM_Node, zend_class_entry *class_entry_DOM_ChildNode)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "CharacterData", class_DOM_CharacterData_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_Node);
	zend_class_implements(class_entry, 1, class_entry_DOM_ChildNode);

	zval property_previousElementSibling_default_value;
	ZVAL_UNDEF(&property_previousElementSibling_default_value);
	zend_string *property_previousElementSibling_name = zend_string_init("previousElementSibling", sizeof("previousElementSibling") - 1, 1);
	zend_string *property_previousElementSibling_class_DOM_Element = zend_string_init("DOM\\Element", sizeof("DOM\\Element")-1, 1);
	zend_declare_typed_property(class_entry, property_previousElementSibling_name, &property_previousElementSibling_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_previousElementSibling_class_DOM_Element, 0, MAY_BE_NULL));
	zend_string_release(property_previousElementSibling_name);

	zval property_nextElementSibling_default_value;
	ZVAL_UNDEF(&property_nextElementSibling_default_value);
	zend_string *property_nextElementSibling_name = zend_string_init("nextElementSibling", sizeof("nextElementSibling") - 1, 1);
	zend_string *property_nextElementSibling_class_DOM_Element = zend_string_init("DOM\\Element", sizeof("DOM\\Element")-1, 1);
	zend_declare_typed_property(class_entry, property_nextElementSibling_name, &property_nextElementSibling_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_nextElementSibling_class_DOM_Element, 0, MAY_BE_NULL));
	zend_string_release(property_nextElementSibling_name);

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

	return class_entry;
}

static zend_class_entry *register_class_DOM_Text(zend_class_entry *class_entry_DOM_CharacterData)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "Text", class_DOM_Text_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_CharacterData);

	zval property_wholeText_default_value;
	ZVAL_UNDEF(&property_wholeText_default_value);
	zend_string *property_wholeText_name = zend_string_init("wholeText", sizeof("wholeText") - 1, 1);
	zend_declare_typed_property(class_entry, property_wholeText_name, &property_wholeText_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_wholeText_name);

	return class_entry;
}

static zend_class_entry *register_class_DOM_CDATASection(zend_class_entry *class_entry_DOM_Text)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "CDATASection", class_DOM_CDATASection_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_Text);

	return class_entry;
}

static zend_class_entry *register_class_DOM_ProcessingInstruction(zend_class_entry *class_entry_DOM_CharacterData)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "ProcessingInstruction", class_DOM_ProcessingInstruction_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_CharacterData);

	zval property_target_default_value;
	ZVAL_UNDEF(&property_target_default_value);
	zend_string *property_target_name = zend_string_init("target", sizeof("target") - 1, 1);
	zend_declare_typed_property(class_entry, property_target_name, &property_target_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_target_name);

	return class_entry;
}

static zend_class_entry *register_class_DOM_Comment(zend_class_entry *class_entry_DOM_CharacterData)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "Comment", class_DOM_Comment_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_CharacterData);

	return class_entry;
}

static zend_class_entry *register_class_DOM_DocumentType(zend_class_entry *class_entry_DOM_Node, zend_class_entry *class_entry_DOM_ChildNode)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "DocumentType", class_DOM_DocumentType_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_Node);
	zend_class_implements(class_entry, 1, class_entry_DOM_ChildNode);

	zval property_name_default_value;
	ZVAL_UNDEF(&property_name_default_value);
	zend_string *property_name_name = zend_string_init("name", sizeof("name") - 1, 1);
	zend_declare_typed_property(class_entry, property_name_name, &property_name_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_name_name);

	zval property_entities_default_value;
	ZVAL_UNDEF(&property_entities_default_value);
	zend_string *property_entities_name = zend_string_init("entities", sizeof("entities") - 1, 1);
	zend_string *property_entities_class_DOM_NamedNodeMap = zend_string_init("DOM\\\116amedNodeMap", sizeof("DOM\\\116amedNodeMap")-1, 1);
	zend_declare_typed_property(class_entry, property_entities_name, &property_entities_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_entities_class_DOM_NamedNodeMap, 0, 0));
	zend_string_release(property_entities_name);

	zval property_notations_default_value;
	ZVAL_UNDEF(&property_notations_default_value);
	zend_string *property_notations_name = zend_string_init("notations", sizeof("notations") - 1, 1);
	zend_string *property_notations_class_DOM_NamedNodeMap = zend_string_init("DOM\\\116amedNodeMap", sizeof("DOM\\\116amedNodeMap")-1, 1);
	zend_declare_typed_property(class_entry, property_notations_name, &property_notations_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_notations_class_DOM_NamedNodeMap, 0, 0));
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

static zend_class_entry *register_class_DOM_DocumentFragment(zend_class_entry *class_entry_DOM_Node, zend_class_entry *class_entry_DOM_ParentNode)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "DocumentFragment", class_DOM_DocumentFragment_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_Node);
	zend_class_implements(class_entry, 1, class_entry_DOM_ParentNode);

	zval property_firstElementChild_default_value;
	ZVAL_UNDEF(&property_firstElementChild_default_value);
	zend_string *property_firstElementChild_name = zend_string_init("firstElementChild", sizeof("firstElementChild") - 1, 1);
	zend_string *property_firstElementChild_class_DOM_Element = zend_string_init("DOM\\Element", sizeof("DOM\\Element")-1, 1);
	zend_declare_typed_property(class_entry, property_firstElementChild_name, &property_firstElementChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_firstElementChild_class_DOM_Element, 0, MAY_BE_NULL));
	zend_string_release(property_firstElementChild_name);

	zval property_lastElementChild_default_value;
	ZVAL_UNDEF(&property_lastElementChild_default_value);
	zend_string *property_lastElementChild_name = zend_string_init("lastElementChild", sizeof("lastElementChild") - 1, 1);
	zend_string *property_lastElementChild_class_DOM_Element = zend_string_init("DOM\\Element", sizeof("DOM\\Element")-1, 1);
	zend_declare_typed_property(class_entry, property_lastElementChild_name, &property_lastElementChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_lastElementChild_class_DOM_Element, 0, MAY_BE_NULL));
	zend_string_release(property_lastElementChild_name);

	zval property_childElementCount_default_value;
	ZVAL_UNDEF(&property_childElementCount_default_value);
	zend_string *property_childElementCount_name = zend_string_init("childElementCount", sizeof("childElementCount") - 1, 1);
	zend_declare_typed_property(class_entry, property_childElementCount_name, &property_childElementCount_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_childElementCount_name);

	return class_entry;
}

static zend_class_entry *register_class_DOM_Entity(zend_class_entry *class_entry_DOM_Node)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "Entity", class_DOM_Entity_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_Node);

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

	return class_entry;
}

static zend_class_entry *register_class_DOM_EntityReference(zend_class_entry *class_entry_DOM_Node)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "EntityReference", class_DOM_EntityReference_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_Node);

	return class_entry;
}

static zend_class_entry *register_class_DOM_Notation(zend_class_entry *class_entry_DOM_Node)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "Notation", class_DOM_Notation_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_Node);

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

static zend_class_entry *register_class_DOM_Document(zend_class_entry *class_entry_DOM_Node, zend_class_entry *class_entry_DOM_ParentNode)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "Document", class_DOM_Document_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_Node);
	class_entry->ce_flags |= ZEND_ACC_ABSTRACT;
	zend_class_implements(class_entry, 1, class_entry_DOM_ParentNode);

	zval property_URL_default_value;
	ZVAL_UNDEF(&property_URL_default_value);
	zend_string *property_URL_name = zend_string_init("URL", sizeof("URL") - 1, 1);
	zend_declare_typed_property(class_entry, property_URL_name, &property_URL_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_URL_name);

	zval property_documentURI_default_value;
	ZVAL_UNDEF(&property_documentURI_default_value);
	zend_string *property_documentURI_name = zend_string_init("documentURI", sizeof("documentURI") - 1, 1);
	zend_declare_typed_property(class_entry, property_documentURI_name, &property_documentURI_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_documentURI_name);

	zval property_characterSet_default_value;
	ZVAL_UNDEF(&property_characterSet_default_value);
	zend_string *property_characterSet_name = zend_string_init("characterSet", sizeof("characterSet") - 1, 1);
	zend_declare_typed_property(class_entry, property_characterSet_name, &property_characterSet_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_characterSet_name);

	zval property_charset_default_value;
	ZVAL_UNDEF(&property_charset_default_value);
	zend_string *property_charset_name = zend_string_init("charset", sizeof("charset") - 1, 1);
	zend_declare_typed_property(class_entry, property_charset_name, &property_charset_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_charset_name);

	zval property_inputEncoding_default_value;
	ZVAL_UNDEF(&property_inputEncoding_default_value);
	zend_string *property_inputEncoding_name = zend_string_init("inputEncoding", sizeof("inputEncoding") - 1, 1);
	zend_declare_typed_property(class_entry, property_inputEncoding_name, &property_inputEncoding_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_inputEncoding_name);

	zval property_doctype_default_value;
	ZVAL_UNDEF(&property_doctype_default_value);
	zend_string *property_doctype_name = zend_string_init("doctype", sizeof("doctype") - 1, 1);
	zend_string *property_doctype_class_DOM_DocumentType = zend_string_init("DOM\\DocumentType", sizeof("DOM\\DocumentType")-1, 1);
	zend_declare_typed_property(class_entry, property_doctype_name, &property_doctype_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_doctype_class_DOM_DocumentType, 0, MAY_BE_NULL));
	zend_string_release(property_doctype_name);

	zval property_documentElement_default_value;
	ZVAL_UNDEF(&property_documentElement_default_value);
	zend_string *property_documentElement_name = zend_string_init("documentElement", sizeof("documentElement") - 1, 1);
	zend_string *property_documentElement_class_DOM_Element = zend_string_init("DOM\\Element", sizeof("DOM\\Element")-1, 1);
	zend_declare_typed_property(class_entry, property_documentElement_name, &property_documentElement_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_documentElement_class_DOM_Element, 0, MAY_BE_NULL));
	zend_string_release(property_documentElement_name);

	zval property_firstElementChild_default_value;
	ZVAL_UNDEF(&property_firstElementChild_default_value);
	zend_string *property_firstElementChild_name = zend_string_init("firstElementChild", sizeof("firstElementChild") - 1, 1);
	zend_string *property_firstElementChild_class_DOM_Element = zend_string_init("DOM\\Element", sizeof("DOM\\Element")-1, 1);
	zend_declare_typed_property(class_entry, property_firstElementChild_name, &property_firstElementChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_firstElementChild_class_DOM_Element, 0, MAY_BE_NULL));
	zend_string_release(property_firstElementChild_name);

	zval property_lastElementChild_default_value;
	ZVAL_UNDEF(&property_lastElementChild_default_value);
	zend_string *property_lastElementChild_name = zend_string_init("lastElementChild", sizeof("lastElementChild") - 1, 1);
	zend_string *property_lastElementChild_class_DOM_Element = zend_string_init("DOM\\Element", sizeof("DOM\\Element")-1, 1);
	zend_declare_typed_property(class_entry, property_lastElementChild_name, &property_lastElementChild_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_lastElementChild_class_DOM_Element, 0, MAY_BE_NULL));
	zend_string_release(property_lastElementChild_name);

	zval property_childElementCount_default_value;
	ZVAL_UNDEF(&property_childElementCount_default_value);
	zend_string *property_childElementCount_name = zend_string_init("childElementCount", sizeof("childElementCount") - 1, 1);
	zend_declare_typed_property(class_entry, property_childElementCount_name, &property_childElementCount_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_childElementCount_name);

	return class_entry;
}

static zend_class_entry *register_class_DOM_HTMLDocument(zend_class_entry *class_entry_DOM_Document)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "HTMLDocument", class_DOM_HTMLDocument_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_Document);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}

static zend_class_entry *register_class_DOM_XMLDocument(zend_class_entry *class_entry_DOM_Document)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "XMLDocument", class_DOM_XMLDocument_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DOM_Document);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	zval property_xmlEncoding_default_value;
	ZVAL_UNDEF(&property_xmlEncoding_default_value);
	zend_string *property_xmlEncoding_name = zend_string_init("xmlEncoding", sizeof("xmlEncoding") - 1, 1);
	zend_declare_typed_property(class_entry, property_xmlEncoding_name, &property_xmlEncoding_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING|MAY_BE_NULL));
	zend_string_release(property_xmlEncoding_name);

	zval property_xmlStandalone_default_value;
	ZVAL_UNDEF(&property_xmlStandalone_default_value);
	zend_string *property_xmlStandalone_name = zend_string_init("xmlStandalone", sizeof("xmlStandalone") - 1, 1);
	zend_declare_typed_property(class_entry, property_xmlStandalone_name, &property_xmlStandalone_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_xmlStandalone_name);

	zval property_xmlVersion_default_value;
	ZVAL_UNDEF(&property_xmlVersion_default_value);
	zend_string *property_xmlVersion_name = zend_string_init("xmlVersion", sizeof("xmlVersion") - 1, 1);
	zend_declare_typed_property(class_entry, property_xmlVersion_name, &property_xmlVersion_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_xmlVersion_name);

	zval property_formatOutput_default_value;
	ZVAL_UNDEF(&property_formatOutput_default_value);
	zend_string *property_formatOutput_name = zend_string_init("formatOutput", sizeof("formatOutput") - 1, 1);
	zend_declare_typed_property(class_entry, property_formatOutput_name, &property_formatOutput_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_formatOutput_name);

	return class_entry;
}

#if defined(LIBXML_XPATH_ENABLED)
static zend_class_entry *register_class_DOM_XPath(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DOM", "XPath", class_DOM_XPath_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NOT_SERIALIZABLE;

	zval property_document_default_value;
	ZVAL_UNDEF(&property_document_default_value);
	zend_string *property_document_name = zend_string_init("document", sizeof("document") - 1, 1);
	zend_string *property_document_class_DOM_DOM_Document = zend_string_init("DOM\\DOM\\Document", sizeof("DOM\\DOM\\Document")-1, 1);
	zend_declare_typed_property(class_entry, property_document_name, &property_document_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_CLASS(property_document_class_DOM_DOM_Document, 0, 0));
	zend_string_release(property_document_name);

	zval property_registerNodeNamespaces_default_value;
	ZVAL_UNDEF(&property_registerNodeNamespaces_default_value);
	zend_string *property_registerNodeNamespaces_name = zend_string_init("registerNodeNamespaces", sizeof("registerNodeNamespaces") - 1, 1);
	zend_declare_typed_property(class_entry, property_registerNodeNamespaces_name, &property_registerNodeNamespaces_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_registerNodeNamespaces_name);

	return class_entry;
}
#endif
