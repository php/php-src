/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 94b232499672dfd61c2c585a5d1d8a27d1a4a7ce */

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
	ZEND_ARG_TYPE_MASK(0, start_handler, MAY_BE_CALLABLE|MAY_BE_STRING|MAY_BE_NULL, NULL)
	ZEND_ARG_TYPE_MASK(0, end_handler, MAY_BE_CALLABLE|MAY_BE_STRING|MAY_BE_NULL, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xml_set_character_data_handler, 0, 2, IS_TRUE, 0)
	ZEND_ARG_OBJ_INFO(0, parser, XMLParser, 0)
	ZEND_ARG_TYPE_MASK(0, handler, MAY_BE_CALLABLE|MAY_BE_STRING|MAY_BE_NULL, NULL)
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
	ZEND_RAW_FENTRY("xml_set_object", zif_xml_set_object, arginfo_xml_set_object, ZEND_ACC_DEPRECATED, NULL, NULL)
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
	REGISTER_LONG_CONSTANT("XML_OPTION_PARSE_HUGE", PHP_XML_OPTION_PARSE_HUGE, CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("XML_SAX_IMPL", PHP_XML_SAX_IMPL, CONST_PERSISTENT);


	zend_attribute *attribute_Deprecated_func_xml_set_object_0 = zend_add_function_attribute(zend_hash_str_find_ptr(CG(function_table), "xml_set_object", sizeof("xml_set_object") - 1), ZSTR_KNOWN(ZEND_STR_DEPRECATED_CAPITALIZED), 2);
	zval attribute_Deprecated_func_xml_set_object_0_arg0;
	zend_string *attribute_Deprecated_func_xml_set_object_0_arg0_str = zend_string_init("8.4", strlen("8.4"), 1);
	ZVAL_STR(&attribute_Deprecated_func_xml_set_object_0_arg0, attribute_Deprecated_func_xml_set_object_0_arg0_str);
	ZVAL_COPY_VALUE(&attribute_Deprecated_func_xml_set_object_0->args[0].value, &attribute_Deprecated_func_xml_set_object_0_arg0);
	attribute_Deprecated_func_xml_set_object_0->args[0].name = ZSTR_KNOWN(ZEND_STR_SINCE);
	zval attribute_Deprecated_func_xml_set_object_0_arg1;
	zend_string *attribute_Deprecated_func_xml_set_object_0_arg1_str = zend_string_init("provide a proper method callable to xml_set_*_handler() functions", strlen("provide a proper method callable to xml_set_*_handler() functions"), 1);
	ZVAL_STR(&attribute_Deprecated_func_xml_set_object_0_arg1, attribute_Deprecated_func_xml_set_object_0_arg1_str);
	ZVAL_COPY_VALUE(&attribute_Deprecated_func_xml_set_object_0->args[1].value, &attribute_Deprecated_func_xml_set_object_0_arg1);
	attribute_Deprecated_func_xml_set_object_0->args[1].name = ZSTR_KNOWN(ZEND_STR_MESSAGE);
}

static zend_class_entry *register_class_XMLParser(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "XMLParser", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}
