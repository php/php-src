/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 1d7decd89754f2fe94cae120a34473173bbffae3 */

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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMChildNode_remove, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMChildNode_before arginfo_class_DOMParentNode_append

#define arginfo_class_DOMChildNode_after arginfo_class_DOMParentNode_append

#define arginfo_class_DOMChildNode_replaceWith arginfo_class_DOMParentNode_append

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_appendChild, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_C14N, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, exclusive, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, withComments, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, xpath, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nsPrefixes, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_C14NFile, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, exclusive, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, withComments, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, xpath, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, nsPrefixes, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_cloneNode, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, deep, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_getLineNo, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMNode_getNodePath arginfo_class_DOMNode_getLineNo

#define arginfo_class_DOMNode_hasAttributes arginfo_class_DOMNode_getLineNo

#define arginfo_class_DOMNode_hasChildNodes arginfo_class_DOMNode_getLineNo

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_insertBefore, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, child, DOMNode, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_isDefaultNamespace, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_isSameNode, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, otherNode, DOMNode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_isSupported, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, feature, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, version, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_lookupNamespaceURI, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMNode_lookupPrefix arginfo_class_DOMNode_isDefaultNamespace

#define arginfo_class_DOMNode_normalize arginfo_class_DOMNode_getLineNo

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_removeChild, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, child, DOMNode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNode_replaceChild, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 0)
	ZEND_ARG_OBJ_INFO(0, child, DOMNode, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMImplementation_getFeature arginfo_class_DOMNode_isSupported

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

#define arginfo_class_DOMDocumentFragment___construct arginfo_class_DOMNode_getLineNo

#define arginfo_class_DOMDocumentFragment_appendXML arginfo_class_DOMCdataSection___construct

#define arginfo_class_DOMDocumentFragment_append arginfo_class_DOMParentNode_append

#define arginfo_class_DOMDocumentFragment_prepend arginfo_class_DOMParentNode_append

#define arginfo_class_DOMNodeList_count arginfo_class_DOMNode_getLineNo

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DOMNodeList_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNodeList_item, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMCharacterData_appendData arginfo_class_DOMCdataSection___construct

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

#define arginfo_class_DOMCharacterData_replaceWith arginfo_class_DOMParentNode_append

#define arginfo_class_DOMCharacterData_remove arginfo_class_DOMChildNode_remove

#define arginfo_class_DOMCharacterData_before arginfo_class_DOMParentNode_append

#define arginfo_class_DOMCharacterData_after arginfo_class_DOMParentNode_append

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMAttr___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#define arginfo_class_DOMAttr_isId arginfo_class_DOMNode_getLineNo

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, namespace, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_getAttribute, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_getAttributeNS, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMElement_getAttributeNode arginfo_class_DOMElement_getAttribute

#define arginfo_class_DOMElement_getAttributeNodeNS arginfo_class_DOMElement_getAttributeNS

#define arginfo_class_DOMElement_getElementsByTagName arginfo_class_DOMElement_getAttribute

#define arginfo_class_DOMElement_getElementsByTagNameNS arginfo_class_DOMElement_getAttributeNS

#define arginfo_class_DOMElement_hasAttribute arginfo_class_DOMElement_getAttribute

#define arginfo_class_DOMElement_hasAttributeNS arginfo_class_DOMElement_getAttributeNS

#define arginfo_class_DOMElement_removeAttribute arginfo_class_DOMElement_getAttribute

#define arginfo_class_DOMElement_removeAttributeNS arginfo_class_DOMElement_getAttributeNS

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_removeAttributeNode, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, attr, DOMAttr, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_setAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_setAttributeNS, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMElement_setAttributeNode arginfo_class_DOMElement_removeAttributeNode

#define arginfo_class_DOMElement_setAttributeNodeNS arginfo_class_DOMElement_removeAttributeNode

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_setIdAttribute, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isId, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_setIdAttributeNS, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, isId, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_setIdAttributeNode, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, attr, DOMAttr, 0)
	ZEND_ARG_TYPE_INFO(0, isId, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMElement_remove arginfo_class_DOMChildNode_remove

#define arginfo_class_DOMElement_before arginfo_class_DOMParentNode_append

#define arginfo_class_DOMElement_after arginfo_class_DOMParentNode_append

#define arginfo_class_DOMElement_replaceWith arginfo_class_DOMParentNode_append

#define arginfo_class_DOMElement_append arginfo_class_DOMParentNode_append

#define arginfo_class_DOMElement_prepend arginfo_class_DOMParentNode_append

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

#define arginfo_class_DOMDocument_createComment arginfo_class_DOMCdataSection___construct

#define arginfo_class_DOMDocument_createDocumentFragment arginfo_class_DOMNode_getLineNo

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

#define arginfo_class_DOMDocument_createTextNode arginfo_class_DOMCdataSection___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_getElementById, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, elementId, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_getElementsByTagName arginfo_class_DOMElement_getAttribute

#define arginfo_class_DOMDocument_getElementsByTagNameNS arginfo_class_DOMElement_getAttributeNS

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_importNode, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, deep, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_load, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_loadXML, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_normalizeDocument arginfo_class_DOMNode_getLineNo

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_registerNodeClass, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, baseClass, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, extendedClass, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_save arginfo_class_DOMDocument_load

#if defined(LIBXML_HTML_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_loadHTML, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_HTML_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_loadHTMLFile, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_HTML_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_saveHTML, 0, 0, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, node, DOMNode, 1, "null")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_HTML_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_saveHTMLFile, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_saveXML, 0, 0, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, node, DOMNode, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_schemaValidate, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_schemaValidateSource, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_relaxNGValidate, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_relaxNGValidateSource, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#define arginfo_class_DOMDocument_validate arginfo_class_DOMNode_getLineNo

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_xinclude, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_adoptNode arginfo_class_DOMNode_appendChild

#define arginfo_class_DOMDocument_append arginfo_class_DOMParentNode_append

#define arginfo_class_DOMDocument_prepend arginfo_class_DOMParentNode_append

#define arginfo_class_DOMText___construct arginfo_class_DOMComment___construct

#define arginfo_class_DOMText_isWhitespaceInElementContent arginfo_class_DOMNode_getLineNo

#define arginfo_class_DOMText_isElementContentWhitespace arginfo_class_DOMNode_getLineNo

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMText_splitText, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMNamedNodeMap_getNamedItem arginfo_class_DOMElement_getAttribute

#define arginfo_class_DOMNamedNodeMap_getNamedItemNS arginfo_class_DOMElement_getAttributeNS

#define arginfo_class_DOMNamedNodeMap_item arginfo_class_DOMNodeList_item

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
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMXPath_evaluate, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, expression, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, contextNode, DOMNode, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, registerNodeNS, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
#define arginfo_class_DOMXPath_query arginfo_class_DOMXPath_evaluate
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMXPath_registerNamespace, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMXPath_registerPhpFunctions, 0, 0, 0)
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
ZEND_METHOD(DOMNode, isSupported);
ZEND_METHOD(DOMNode, lookupNamespaceURI);
ZEND_METHOD(DOMNode, lookupPrefix);
ZEND_METHOD(DOMNode, normalize);
ZEND_METHOD(DOMNode, removeChild);
ZEND_METHOD(DOMNode, replaceChild);
ZEND_METHOD(DOMImplementation, getFeature);
ZEND_METHOD(DOMImplementation, hasFeature);
ZEND_METHOD(DOMImplementation, createDocumentType);
ZEND_METHOD(DOMImplementation, createDocument);
ZEND_METHOD(DOMDocumentFragment, __construct);
ZEND_METHOD(DOMDocumentFragment, appendXML);
ZEND_METHOD(DOMDocumentFragment, append);
ZEND_METHOD(DOMDocumentFragment, prepend);
ZEND_METHOD(DOMNodeList, count);
ZEND_METHOD(DOMNodeList, getIterator);
ZEND_METHOD(DOMNodeList, item);
ZEND_METHOD(DOMCharacterData, appendData);
ZEND_METHOD(DOMCharacterData, substringData);
ZEND_METHOD(DOMCharacterData, insertData);
ZEND_METHOD(DOMCharacterData, deleteData);
ZEND_METHOD(DOMCharacterData, replaceData);
ZEND_METHOD(DOMCharacterData, replaceWith);
ZEND_METHOD(DOMCharacterData, remove);
ZEND_METHOD(DOMCharacterData, before);
ZEND_METHOD(DOMCharacterData, after);
ZEND_METHOD(DOMAttr, __construct);
ZEND_METHOD(DOMAttr, isId);
ZEND_METHOD(DOMElement, __construct);
ZEND_METHOD(DOMElement, getAttribute);
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
ZEND_METHOD(DOMElement, remove);
ZEND_METHOD(DOMElement, before);
ZEND_METHOD(DOMElement, after);
ZEND_METHOD(DOMElement, replaceWith);
ZEND_METHOD(DOMElement, append);
ZEND_METHOD(DOMElement, prepend);
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
ZEND_METHOD(DOMDocument, append);
ZEND_METHOD(DOMDocument, prepend);
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
	ZEND_ME(DOMNode, isSupported, arginfo_class_DOMNode_isSupported, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, lookupNamespaceURI, arginfo_class_DOMNode_lookupNamespaceURI, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, lookupPrefix, arginfo_class_DOMNode_lookupPrefix, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, normalize, arginfo_class_DOMNode_normalize, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, removeChild, arginfo_class_DOMNode_removeChild, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMNode, replaceChild, arginfo_class_DOMNode_replaceChild, ZEND_ACC_PUBLIC)
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
	ZEND_ME(DOMDocumentFragment, append, arginfo_class_DOMDocumentFragment_append, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocumentFragment, prepend, arginfo_class_DOMDocumentFragment_prepend, ZEND_ACC_PUBLIC)
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
	ZEND_ME(DOMCharacterData, replaceWith, arginfo_class_DOMCharacterData_replaceWith, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMCharacterData, remove, arginfo_class_DOMCharacterData_remove, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMCharacterData, before, arginfo_class_DOMCharacterData_before, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMCharacterData, after, arginfo_class_DOMCharacterData_after, ZEND_ACC_PUBLIC)
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
	ZEND_ME(DOMElement, remove, arginfo_class_DOMElement_remove, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, before, arginfo_class_DOMElement_before, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, after, arginfo_class_DOMElement_after, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, replaceWith, arginfo_class_DOMElement_replaceWith, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, append, arginfo_class_DOMElement_append, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMElement, prepend, arginfo_class_DOMElement_prepend, ZEND_ACC_PUBLIC)
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
	ZEND_ME(DOMDocument, append, arginfo_class_DOMDocument_append, ZEND_ACC_PUBLIC)
	ZEND_ME(DOMDocument, prepend, arginfo_class_DOMDocument_prepend, ZEND_ACC_PUBLIC)
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


static const zend_function_entry class_DOMXPath_methods[] = {
#if defined(LIBXML_XPATH_ENABLED)
	ZEND_ME(DOMXPath, __construct, arginfo_class_DOMXPath___construct, ZEND_ACC_PUBLIC)
#endif
#if defined(LIBXML_XPATH_ENABLED)
	ZEND_ME(DOMXPath, evaluate, arginfo_class_DOMXPath_evaluate, ZEND_ACC_PUBLIC)
#endif
#if defined(LIBXML_XPATH_ENABLED)
	ZEND_ME(DOMXPath, query, arginfo_class_DOMXPath_query, ZEND_ACC_PUBLIC)
#endif
#if defined(LIBXML_XPATH_ENABLED)
	ZEND_ME(DOMXPath, registerNamespace, arginfo_class_DOMXPath_registerNamespace, ZEND_ACC_PUBLIC)
#endif
#if defined(LIBXML_XPATH_ENABLED)
	ZEND_ME(DOMXPath, registerPhpFunctions, arginfo_class_DOMXPath_registerPhpFunctions, ZEND_ACC_PUBLIC)
#endif
	ZEND_FE_END
};
