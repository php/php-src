/* This is a generated file, edit the .stub.php file instead. */

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

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_createCDATASection, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_createComment arginfo_class_DOMDocument_createCDATASection

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_createDocumentFragment, 0, 0, 0)
ZEND_END_ARG_INFO()

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

#define arginfo_class_DOMDocument_createTextNode arginfo_class_DOMDocument_createCDATASection

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

#define arginfo_class_DOMDocument_normalizeDocument arginfo_class_DOMDocument_createDocumentFragment

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

#define arginfo_class_DOMDocument_validate arginfo_class_DOMDocument_createDocumentFragment

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_xinclude, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMDocument_adoptNode, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, source, DOMNode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DOMDocument_append, 0, 0, IS_VOID, 0)
	ZEND_ARG_VARIADIC_INFO(0, nodes)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMDocument_prepend arginfo_class_DOMDocument_append
