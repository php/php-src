/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 49adb3008ade3f92f9b764ead9366efc6681e46f */

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

static zend_class_entry *register_class_XMLReader(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "XMLReader", class_XMLReader_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	zval property_attributeCount_default_value;
	ZVAL_UNDEF(&property_attributeCount_default_value);
	zend_string *property_attributeCount_name = zend_string_init("attributeCount", sizeof("attributeCount") - 1, 1);
	zend_declare_typed_property(class_entry, property_attributeCount_name, &property_attributeCount_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_attributeCount_name);

	zval property_baseURI_default_value;
	ZVAL_UNDEF(&property_baseURI_default_value);
	zend_string *property_baseURI_name = zend_string_init("baseURI", sizeof("baseURI") - 1, 1);
	zend_declare_typed_property(class_entry, property_baseURI_name, &property_baseURI_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_baseURI_name);

	zval property_depth_default_value;
	ZVAL_UNDEF(&property_depth_default_value);
	zend_string *property_depth_name = zend_string_init("depth", sizeof("depth") - 1, 1);
	zend_declare_typed_property(class_entry, property_depth_name, &property_depth_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_LONG));
	zend_string_release(property_depth_name);

	zval property_hasAttributes_default_value;
	ZVAL_UNDEF(&property_hasAttributes_default_value);
	zend_string *property_hasAttributes_name = zend_string_init("hasAttributes", sizeof("hasAttributes") - 1, 1);
	zend_declare_typed_property(class_entry, property_hasAttributes_name, &property_hasAttributes_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_hasAttributes_name);

	zval property_hasValue_default_value;
	ZVAL_UNDEF(&property_hasValue_default_value);
	zend_string *property_hasValue_name = zend_string_init("hasValue", sizeof("hasValue") - 1, 1);
	zend_declare_typed_property(class_entry, property_hasValue_name, &property_hasValue_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_hasValue_name);

	zval property_isDefault_default_value;
	ZVAL_UNDEF(&property_isDefault_default_value);
	zend_string *property_isDefault_name = zend_string_init("isDefault", sizeof("isDefault") - 1, 1);
	zend_declare_typed_property(class_entry, property_isDefault_name, &property_isDefault_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_isDefault_name);

	zval property_isEmptyElement_default_value;
	ZVAL_UNDEF(&property_isEmptyElement_default_value);
	zend_string *property_isEmptyElement_name = zend_string_init("isEmptyElement", sizeof("isEmptyElement") - 1, 1);
	zend_declare_typed_property(class_entry, property_isEmptyElement_name, &property_isEmptyElement_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_isEmptyElement_name);

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

	zval property_namespaceURI_default_value;
	ZVAL_UNDEF(&property_namespaceURI_default_value);
	zend_string *property_namespaceURI_name = zend_string_init("namespaceURI", sizeof("namespaceURI") - 1, 1);
	zend_declare_typed_property(class_entry, property_namespaceURI_name, &property_namespaceURI_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_namespaceURI_name);

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

	zval property_value_default_value;
	ZVAL_UNDEF(&property_value_default_value);
	zend_string *property_value_name = zend_string_init("value", sizeof("value") - 1, 1);
	zend_declare_typed_property(class_entry, property_value_name, &property_value_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_value_name);

	zval property_xmlLang_default_value;
	ZVAL_UNDEF(&property_xmlLang_default_value);
	zend_string *property_xmlLang_name = zend_string_init("xmlLang", sizeof("xmlLang") - 1, 1);
	zend_declare_typed_property(class_entry, property_xmlLang_name, &property_xmlLang_default_value, ZEND_ACC_PUBLIC, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_xmlLang_name);

	return class_entry;
}
