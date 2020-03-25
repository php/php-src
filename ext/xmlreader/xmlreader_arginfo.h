/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_close, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_getAttribute, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_getAttributeNo, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_getAttributeNs, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_getParserProperty, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLReader_isValid arginfo_class_XMLReader_close

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_lookupNamespace, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLReader_moveToAttribute arginfo_class_XMLReader_getAttribute

#define arginfo_class_XMLReader_moveToAttributeNo arginfo_class_XMLReader_getAttributeNo

#define arginfo_class_XMLReader_moveToAttributeNs arginfo_class_XMLReader_getAttributeNs

#define arginfo_class_XMLReader_moveToElement arginfo_class_XMLReader_close

#define arginfo_class_XMLReader_moveToFirstAttribute arginfo_class_XMLReader_close

#define arginfo_class_XMLReader_moveToNextAttribute arginfo_class_XMLReader_close

#define arginfo_class_XMLReader_read arginfo_class_XMLReader_close

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_next, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, localname, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_open, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, URI, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLReader_readInnerXml arginfo_class_XMLReader_close

#define arginfo_class_XMLReader_readOuterXml arginfo_class_XMLReader_close

#define arginfo_class_XMLReader_readString arginfo_class_XMLReader_close

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_setSchema, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_setParserProperty, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, property, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_XMLReader_setRelaxNGSchema arginfo_class_XMLReader_setSchema

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_setRelaxNGSchemaSource, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_XML, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, source, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_expand, 0, 0, 0)
	ZEND_ARG_OBJ_INFO(0, basenode, DOMNode, 1)
ZEND_END_ARG_INFO()
