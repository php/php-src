/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 0188a53f262d3f8e19b5b64d163bdee84f1be6b8 */

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
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 0)
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, name, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_open, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_XMLReader_expand, 0, 0, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, baseNode, DOMNode, 1, "null")
ZEND_END_ARG_INFO()


ZEND_METHOD(XMLReader, close);
ZEND_METHOD(XMLReader, getAttribute);
ZEND_METHOD(XMLReader, getAttributeNo);
ZEND_METHOD(XMLReader, getAttributeNs);
ZEND_METHOD(XMLReader, getParserProperty);
ZEND_METHOD(XMLReader, isValid);
ZEND_METHOD(XMLReader, lookupNamespace);
ZEND_METHOD(XMLReader, moveToAttribute);
ZEND_METHOD(XMLReader, moveToAttributeNo);
ZEND_METHOD(XMLReader, moveToAttributeNs);
ZEND_METHOD(XMLReader, moveToElement);
ZEND_METHOD(XMLReader, moveToFirstAttribute);
ZEND_METHOD(XMLReader, moveToNextAttribute);
ZEND_METHOD(XMLReader, read);
ZEND_METHOD(XMLReader, next);
ZEND_METHOD(XMLReader, open);
ZEND_METHOD(XMLReader, readInnerXml);
ZEND_METHOD(XMLReader, readOuterXml);
ZEND_METHOD(XMLReader, readString);
ZEND_METHOD(XMLReader, setSchema);
ZEND_METHOD(XMLReader, setParserProperty);
ZEND_METHOD(XMLReader, setRelaxNGSchema);
ZEND_METHOD(XMLReader, setRelaxNGSchemaSource);
ZEND_METHOD(XMLReader, XML);
ZEND_METHOD(XMLReader, expand);


static const zend_function_entry class_XMLReader_methods[] = {
	ZEND_ME(XMLReader, close, arginfo_class_XMLReader_close, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, getAttribute, arginfo_class_XMLReader_getAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, getAttributeNo, arginfo_class_XMLReader_getAttributeNo, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, getAttributeNs, arginfo_class_XMLReader_getAttributeNs, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, getParserProperty, arginfo_class_XMLReader_getParserProperty, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, isValid, arginfo_class_XMLReader_isValid, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, lookupNamespace, arginfo_class_XMLReader_lookupNamespace, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, moveToAttribute, arginfo_class_XMLReader_moveToAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, moveToAttributeNo, arginfo_class_XMLReader_moveToAttributeNo, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, moveToAttributeNs, arginfo_class_XMLReader_moveToAttributeNs, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, moveToElement, arginfo_class_XMLReader_moveToElement, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, moveToFirstAttribute, arginfo_class_XMLReader_moveToFirstAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, moveToNextAttribute, arginfo_class_XMLReader_moveToNextAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, read, arginfo_class_XMLReader_read, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, next, arginfo_class_XMLReader_next, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, open, arginfo_class_XMLReader_open, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(XMLReader, readInnerXml, arginfo_class_XMLReader_readInnerXml, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, readOuterXml, arginfo_class_XMLReader_readOuterXml, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, readString, arginfo_class_XMLReader_readString, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, setSchema, arginfo_class_XMLReader_setSchema, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, setParserProperty, arginfo_class_XMLReader_setParserProperty, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, setRelaxNGSchema, arginfo_class_XMLReader_setRelaxNGSchema, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, setRelaxNGSchemaSource, arginfo_class_XMLReader_setRelaxNGSchemaSource, ZEND_ACC_PUBLIC)
	ZEND_ME(XMLReader, XML, arginfo_class_XMLReader_XML, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(XMLReader, expand, arginfo_class_XMLReader_expand, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
