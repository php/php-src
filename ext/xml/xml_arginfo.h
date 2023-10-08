/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f87e295b35cd43db72a936ee5745297a45730090 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_xml_parser_create, 0, 0, XMLParser, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_xml_parser_create_ns, 0, 0, XMLParser, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, encoding, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, separator, IS_STRING, 0, "\":\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_set_object, 0, 2, IS_TRUE, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_set_element_handler, 0, 3, IS_TRUE, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_INFO(0, start_handler)
	ZEND_ARG_INFO(0, end_handler)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_set_character_data_handler, 0, 2, IS_TRUE, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_INFO(0, handler)
ZEND_END_ARG_INFO()

#define arginfo_xml_set_processing_instruction_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_default_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_unparsed_entity_decl_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_notation_decl_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_external_entity_ref_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_start_namespace_decl_handler arginfo_xml_set_character_data_handler

#define arginfo_xml_set_end_namespace_decl_handler arginfo_xml_set_character_data_handler

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_parse, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, is_final, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xml_parse_into_struct, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_INFO(1, values)
	ZEND_ARG_INFO_WITH_DEFAULT_VALUE(1, index, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_get_error_code, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_error_string, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, error_code, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_xml_get_current_line_number arginfo_xml_get_error_code

#define arginfo_xml_get_current_column_number arginfo_xml_get_error_code

#define arginfo_xml_get_current_byte_index arginfo_xml_get_error_code

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_parser_free, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_parser_set_option, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xml_parser_get_option, 0, 2, MAY_BE_STRING|MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(xml_parser_create);
ZEND_FUNCTION(xml_parser_create_ns);
ZEND_FUNCTION(xml_set_object);
ZEND_FUNCTION(xml_set_element_handler);
ZEND_FUNCTION(xml_set_character_data_handler);
ZEND_FUNCTION(xml_set_processing_instruction_handler);
ZEND_FUNCTION(xml_set_default_handler);
ZEND_FUNCTION(xml_set_unparsed_entity_decl_handler);
ZEND_FUNCTION(xml_set_notation_decl_handler);
ZEND_FUNCTION(xml_set_external_entity_ref_handler);
ZEND_FUNCTION(xml_set_start_namespace_decl_handler);
ZEND_FUNCTION(xml_set_end_namespace_decl_handler);
ZEND_FUNCTION(xml_parse);
ZEND_FUNCTION(xml_parse_into_struct);
ZEND_FUNCTION(xml_get_error_code);
ZEND_FUNCTION(xml_error_string);
ZEND_FUNCTION(xml_get_current_line_number);
ZEND_FUNCTION(xml_get_current_column_number);
ZEND_FUNCTION(xml_get_current_byte_index);
ZEND_FUNCTION(xml_parser_free);
ZEND_FUNCTION(xml_parser_set_option);
ZEND_FUNCTION(xml_parser_get_option);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(xml_parser_create, arginfo_xml_parser_create)
	ZEND_FE(xml_parser_create_ns, arginfo_xml_parser_create_ns)
	ZEND_FE(xml_set_object, arginfo_xml_set_object)
	ZEND_FE(xml_set_element_handler, arginfo_xml_set_element_handler)
	ZEND_FE(xml_set_character_data_handler, arginfo_xml_set_character_data_handler)
	ZEND_FE(xml_set_processing_instruction_handler, arginfo_xml_set_processing_instruction_handler)
	ZEND_FE(xml_set_default_handler, arginfo_xml_set_default_handler)
	ZEND_FE(xml_set_unparsed_entity_decl_handler, arginfo_xml_set_unparsed_entity_decl_handler)
	ZEND_FE(xml_set_notation_decl_handler, arginfo_xml_set_notation_decl_handler)
	ZEND_FE(xml_set_external_entity_ref_handler, arginfo_xml_set_external_entity_ref_handler)
	ZEND_FE(xml_set_start_namespace_decl_handler, arginfo_xml_set_start_namespace_decl_handler)
	ZEND_FE(xml_set_end_namespace_decl_handler, arginfo_xml_set_end_namespace_decl_handler)
	ZEND_FE(xml_parse, arginfo_xml_parse)
	ZEND_FE(xml_parse_into_struct, arginfo_xml_parse_into_struct)
	ZEND_FE(xml_get_error_code, arginfo_xml_get_error_code)
	ZEND_FE(xml_error_string, arginfo_xml_error_string)
	ZEND_FE(xml_get_current_line_number, arginfo_xml_get_current_line_number)
	ZEND_FE(xml_get_current_column_number, arginfo_xml_get_current_column_number)
	ZEND_FE(xml_get_current_byte_index, arginfo_xml_get_current_byte_index)
	ZEND_FE(xml_parser_free, arginfo_xml_parser_free)
	ZEND_FE(xml_parser_set_option, arginfo_xml_parser_set_option)
	ZEND_FE(xml_parser_get_option, arginfo_xml_parser_get_option)
	ZEND_FE_END
};


static const zend_function_entry class_XMLParser_methods[] = {
	ZEND_FE_END
};

static void register_xml_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("XML_ERROR_NONE", XML_ERROR_NONE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_NO_MEMORY", XML_ERROR_NO_MEMORY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_SYNTAX", XML_ERROR_SYNTAX, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_NO_ELEMENTS", XML_ERROR_NO_ELEMENTS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_INVALID_TOKEN", XML_ERROR_INVALID_TOKEN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_UNCLOSED_TOKEN", XML_ERROR_UNCLOSED_TOKEN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_PARTIAL_CHAR", XML_ERROR_PARTIAL_CHAR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_TAG_MISMATCH", XML_ERROR_TAG_MISMATCH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_DUPLICATE_ATTRIBUTE", XML_ERROR_DUPLICATE_ATTRIBUTE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_JUNK_AFTER_DOC_ELEMENT", XML_ERROR_JUNK_AFTER_DOC_ELEMENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_PARAM_ENTITY_REF", XML_ERROR_PARAM_ENTITY_REF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_UNDEFINED_ENTITY", XML_ERROR_UNDEFINED_ENTITY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_RECURSIVE_ENTITY_REF", XML_ERROR_RECURSIVE_ENTITY_REF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_ASYNC_ENTITY", XML_ERROR_ASYNC_ENTITY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_BAD_CHAR_REF", XML_ERROR_BAD_CHAR_REF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_BINARY_ENTITY_REF", XML_ERROR_BINARY_ENTITY_REF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF", XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_MISPLACED_XML_PI", XML_ERROR_MISPLACED_XML_PI, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_UNKNOWN_ENCODING", XML_ERROR_UNKNOWN_ENCODING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_INCORRECT_ENCODING", XML_ERROR_INCORRECT_ENCODING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_UNCLOSED_CDATA_SECTION", XML_ERROR_UNCLOSED_CDATA_SECTION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_ERROR_EXTERNAL_ENTITY_HANDLING", XML_ERROR_EXTERNAL_ENTITY_HANDLING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_OPTION_CASE_FOLDING", PHP_XML_OPTION_CASE_FOLDING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_OPTION_TARGET_ENCODING", PHP_XML_OPTION_TARGET_ENCODING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_OPTION_SKIP_TAGSTART", PHP_XML_OPTION_SKIP_TAGSTART, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XML_OPTION_SKIP_WHITE", PHP_XML_OPTION_SKIP_WHITE, CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("XML_SAX_IMPL", PHP_XML_SAX_IMPL, CONST_PERSISTENT);
}

static zend_class_entry *register_class_XMLParser(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "XMLParser", class_XMLParser_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE;

	return class_entry;
}
