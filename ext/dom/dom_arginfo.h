/* This is a generated file, edit the .stub.php file instead. */

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

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMAttr___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMAttr_isId arginfo_class_DOMNode_getLineNo

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMCdataSection___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

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

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMComment___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, version, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_createAttribute, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_createAttributeNS, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_createCDATASection arginfo_class_DOMCharacterData_appendData

#define arginfo_class_DOMDocument_createComment arginfo_class_DOMCharacterData_appendData

#define arginfo_class_DOMDocument_createDocumentFragment arginfo_class_DOMNode_getLineNo

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_createElement, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, tagName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_createElementNS, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_createEntityReference arginfo_class_DOMDocument_createAttribute

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_createProcessingInstruction, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_createTextNode arginfo_class_DOMCharacterData_appendData

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_getElementById, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, elementId, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_getElementsByTagName, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, tagName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_getElementsByTagNameNS, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_importNode, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, importedNode, DOMNode, 0)
	ZEND_ARG_TYPE_INFO(0, deep, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_load, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_loadXML, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_normalizeDocument arginfo_class_DOMNode_getLineNo

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_registerNodeClass, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, baseclass, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, extendedclass, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_save arginfo_class_DOMDocument_load

#if defined(LIBXML_HTML_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_loadHTML, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_HTML_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_loadHTMLFile, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_HTML_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_saveHTML, 0, 0, 0)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 1)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_HTML_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_saveHTMLFile, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_saveXML, 0, 0, 0)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 1)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_schemaValidate, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_SCHEMAS_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_schemaValidateSource, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
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
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_adoptNode, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, source, DOMNode, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocumentFragment___construct arginfo_class_DOMNode_getLineNo

#define arginfo_class_DOMDocumentFragment_appendXML arginfo_class_DOMCharacterData_appendData

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMElement_getAttribute arginfo_class_DOMDocument_createAttribute

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMElement_getAttributeNS, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, localName, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMElement_getAttributeNode arginfo_class_DOMDocument_createAttribute

#define arginfo_class_DOMElement_getAttributeNodeNS arginfo_class_DOMElement_getAttributeNS

#define arginfo_class_DOMElement_getElementsByTagName arginfo_class_DOMDocument_createAttribute

#define arginfo_class_DOMElement_getElementsByTagNameNS arginfo_class_DOMDocument_getElementsByTagNameNS

#define arginfo_class_DOMElement_hasAttribute arginfo_class_DOMDocument_createAttribute

#define arginfo_class_DOMElement_hasAttributeNS arginfo_class_DOMElement_getAttributeNS

#define arginfo_class_DOMElement_removeAttribute arginfo_class_DOMDocument_createAttribute

#define arginfo_class_DOMElement_removeAttributeNS arginfo_class_DOMElement_getAttributeNS

#define arginfo_class_DOMElement_removeAttributeNode arginfo_class_DOMDocument_createAttribute

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

#define arginfo_class_DOMEntityReference___construct arginfo_class_DOMDocument_createAttribute

#define arginfo_class_DOMImplementation_getFeature arginfo_class_DOMNode_isSupported

#define arginfo_class_DOMImplementation_hasFeature arginfo_class_DOMNode_isSupported

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMImplementation_createDocumentType, 0, 0, 3)
	ZEND_ARG_INFO(0, qualifiedName)
	ZEND_ARG_INFO(0, publicId)
	ZEND_ARG_INFO(0, systemId)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMImplementation_createDocument, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, doctype, DOMDocumentType, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMNamedNodeMap_getNamedItem arginfo_class_DOMDocument_createAttribute

#define arginfo_class_DOMNamedNodeMap_getNamedItemNS arginfo_class_DOMElement_getAttributeNS

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMNamedNodeMap_item, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMNamedNodeMap_count arginfo_class_DOMNode_getLineNo

#define arginfo_class_DOMNodeList_count arginfo_class_DOMNode_getLineNo

#define arginfo_class_DOMNodeList_item arginfo_class_DOMNamedNodeMap_item

#define arginfo_class_DOMProcessingInstruction___construct arginfo_class_DOMAttr___construct

#define arginfo_class_DOMText___construct arginfo_class_DOMComment___construct

#define arginfo_class_DOMText_isWhitespaceInElementContent arginfo_class_DOMNode_getLineNo

#define arginfo_class_DOMText_isElementContentWhitespace arginfo_class_DOMNode_getLineNo

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMText_splitText, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMXPath___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, doc, DOMDocument, 0)
	ZEND_ARG_TYPE_INFO(0, registerNodeNS, _IS_BOOL, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMXPath_evaluate, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, expr, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, context, DOMNode, 1)
	ZEND_ARG_TYPE_INFO(0, registerNodeNS, _IS_BOOL, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
#define arginfo_class_DOMXPath_query arginfo_class_DOMXPath_evaluate
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMXPath_registerNamespace, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LIBXML_XPATH_ENABLED)
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMXPath_registerPhpFunctions, 0, 0, 0)
	ZEND_ARG_INFO(0, restrict)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_dom_import_simplexml, 0, 1, DOMElement, 1)
	ZEND_ARG_TYPE_INFO(0, node, IS_OBJECT, 0)
ZEND_END_ARG_INFO()
